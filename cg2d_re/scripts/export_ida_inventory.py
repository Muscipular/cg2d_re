# -*- coding: utf-8 -*-
"""只读采集当前 IDA 元数据；由主线程审阅后显式调用，不自动执行或导出。"""

import hashlib
import json
from collections import Counter
from pathlib import Path

import ida_bytes
import ida_funcs
import ida_ida
import ida_nalt
import ida_segment
import idautils


# 固定基线只用于拒绝错误数据库，不据名称或标志排除函数。
_EXPECTED_BASELINE = {
    "root_filename": "cg2d.exe",
    "input_sha256": "dad3f292c6df28e9b7208ad1d339f56ab68c1e728f6595e849e37296551c0f08",
    "imagebase": 0x400000,
    "processor": "metapc",
    "is_32bit_exactly": True,
    "is_big_endian": False,
}
_TEXT_RANGE = (0x401000, 0x54D000)
_TEXT_SHA256 = "7d5813705a3dd034d391650c2fbfe37e286a6a28d080715bccd19551f8c25669"
_IMPORT_SLOT_SIZE = 4


def _read_initialized_bytes(start, size):
    """读取缓冲区及有效初始化字节数；缓冲区不得直接进入快照。"""
    result = ida_bytes.get_bytes_and_mask(start, size, ida_bytes.GMB_READALL)
    if not isinstance(result, tuple) or len(result) != 2:
        raise RuntimeError(f"读取字节及初始化掩码失败：VA 0x{start:08X}，长度 {size}。")
    data, mask = result
    if (
        not isinstance(data, bytes)
        or not isinstance(mask, bytes)
        or len(data) != size
        or len(mask) < (size + 7) // 8
    ):
        raise RuntimeError(f"字节或掩码类型、长度异常：VA 0x{start:08X}。")
    # 每位对应一个字节，低位在先；只统计请求长度内的位，忽略多余尾位。
    whole, tail = divmod(size, 8)
    initialized = sum(bin(value).count("1") for value in mask[:whole])
    if tail:
        initialized += bin(mask[whole] & ((1 << tail) - 1)).count("1")
    return data, initialized


def _collect_segments():
    """保留全部四段元数据，执行段必须完整初始化且通过 .text 指纹门禁。"""
    quantity = ida_segment.get_segm_qty()
    if quantity != 4:
        raise RuntimeError(f"段数门禁失败：应为 4，实际为 {quantity}。")
    segments = []
    for index in range(quantity):
        segment = ida_segment.getnseg(index)
        if segment is None or segment.start_ea >= segment.end_ea:
            raise RuntimeError(f"段枚举失败或范围无效：索引 {index}。")
        segments.append({
            "start": int(segment.start_ea),
            "end": int(segment.end_ea),
            "name": ida_segment.get_segm_name(segment),
            "permissions": int(segment.perm),
        })
    segments.sort(key=lambda item: item["start"])
    text_segments = [item for item in segments if item["name"] == ".text"]
    if (
        len(text_segments) != 1
        or (text_segments[0]["start"], text_segments[0]["end"]) != _TEXT_RANGE
        or not text_segments[0]["permissions"] & ida_segment.SEGPERM_EXEC
    ):
        raise RuntimeError("基线门禁失败：.text 名称、范围或可执行权限不符。")
    for segment in segments:
        if not segment["permissions"] & ida_segment.SEGPERM_EXEC:
            continue
        size = segment["end"] - segment["start"]
        data, initialized = _read_initialized_bytes(segment["start"], size)
        if initialized != size:
            raise RuntimeError(
                f"可执行段 {segment['name']} 未完整初始化：{initialized}/{size} 字节。"
            )
        segment["initialized_bytes"] = initialized
        segment["sha256"] = hashlib.sha256(data).hexdigest()
        if segment["name"] == ".text" and segment["sha256"] != _TEXT_SHA256:
            raise RuntimeError("基线门禁失败：.text 已初始化字节 SHA-256 不符。")
    return segments


def _collect_functions(imagebase):
    """逐函数枚举并保留各自全部块；计数器按不同归属函数统计共享块。"""
    functions = []
    chunk_owners = Counter()
    for index in range(ida_funcs.get_func_qty()):
        function = ida_funcs.getn_func(index)
        if function is None:
            raise RuntimeError(f"函数枚举失败：索引 {index}。")
        start, end, flags = int(function.start_ea), int(function.end_ea), int(function.flags)
        rva = start - imagebase
        if not 0 <= rva <= 0xFFFFFFFF:
            raise RuntimeError(f"函数 RVA 无法表示为八位稳定标识：VA 0x{start:X}。")
        chunks = sorted((int(a), int(b)) for a, b in idautils.Chunks(start))
        if (start, end) not in chunks or any(a >= b for a, b in chunks):
            raise RuntimeError(f"函数块范围无效或缺少入口块：VA 0x{start:08X}。")
        functions.append({
            "id": f"F:{rva:08X}",
            "va": start,
            "rva": rva,
            "name": ida_funcs.get_func_name(start),
            "entry_chunk_end": end,
            "flags": flags,
            "ida_library_flag": bool(flags & ida_funcs.FUNC_LIB),
            "ida_thunk_flag": bool(flags & ida_funcs.FUNC_THUNK),
            "chunks": [[a, b] for a, b in chunks],
        })
        chunk_owners.update(set(chunks))
    functions.sort(key=lambda item: item["va"])
    return functions, chunk_owners


def _unassigned_executable_ranges(segments, chunks):
    """求执行段内未被当前函数块覆盖的半开区间，不判断区间内容类别。"""
    union = []
    for start, end in sorted(chunks):
        if union and start <= union[-1][1]:
            union[-1][1] = max(union[-1][1], end)
        else:
            union.append([start, end])
    ranges = []
    for segment in segments:
        if not segment["permissions"] & ida_segment.SEGPERM_EXEC:
            continue
        cursor, limit = segment["start"], segment["end"]
        for start, end in union:
            if end <= cursor:
                continue
            if start >= limit:
                break
            if start > cursor:
                ranges.append([cursor, start])
            cursor = max(cursor, min(end, limit))
            if cursor == limit:
                break
        if cursor < limit:
            ranges.append([cursor, limit])
    return ranges


def _collect_imports(imagebase):
    """枚举数据库导入记录；不读取 DLL，不解析或推测运行时地址。"""
    modules = []
    for index in range(ida_nalt.get_import_module_qty()):
        name = ida_nalt.get_import_module_name(index)
        raw_entries = []

        def collect(ea, entry_name, ordinal):
            # 回调只收集原始元数据，后续检查放在枚举之外，避免回调吞掉异常。
            raw_entries.append((ea, entry_name, ordinal))
            return True

        status = ida_nalt.enum_import_names(index, collect)
        if status != 1:
            raise RuntimeError(f"导入枚举未成功结束：模块 {index}（{name}），返回 {status}。")
        entries = []
        for ea, entry_name, ordinal in raw_entries:
            ea = int(ea)
            data, initialized = _read_initialized_bytes(ea, _IMPORT_SLOT_SIZE)
            entries.append({
                "va": ea,
                "rva": ea - imagebase,
                "name": entry_name,
                "ordinal": int(ordinal),
                "initialized_bytes": initialized,
                "initialized_hex": data.hex() if initialized == _IMPORT_SLOT_SIZE else None,
            })
        entries.sort(key=lambda item: (
            item["va"], item["name"] is not None, item["name"] or "", item["ordinal"]
        ))
        modules.append({"index": index, "name": name, "entries": entries})
    return modules


def build_inventory():
    """通过固定基线门禁后构建结构化元数据；不写磁盘或修改 IDA。"""
    input_sha256 = ida_nalt.retrieve_input_file_sha256()
    if not isinstance(input_sha256, bytes) or len(input_sha256) != 32:
        raise RuntimeError("基线门禁失败：IDA 未返回有效的原始输入 SHA-256。")
    baseline = {
        "root_filename": ida_nalt.get_root_filename(),
        "input_sha256": input_sha256.hex(),
        "imagebase": int(ida_nalt.get_imagebase()),
        "processor": ida_ida.inf_get_procname(),
        "is_32bit_exactly": bool(ida_ida.inf_is_32bit_exactly()),
        "is_big_endian": bool(ida_ida.inf_is_be()),
    }
    for field, expected in _EXPECTED_BASELINE.items():
        if baseline[field] != expected:
            raise RuntimeError(f"基线门禁失败：{field} 不符，实际为 {baseline[field]!r}。")
    segments = _collect_segments()
    functions, chunk_owners = _collect_functions(baseline["imagebase"])
    ranges = _unassigned_executable_ranges(segments, chunk_owners)
    imports = _collect_imports(baseline["imagebase"])
    executable_segments = [
        item for item in segments if item["permissions"] & ida_segment.SEGPERM_EXEC
    ]
    import_entries = [entry for module in imports for entry in module["entries"]]
    counts = {
        "segment_count": len(segments),
        "executable_segment_count": len(executable_segments),
        "function_count": len(functions),
        "chunk_association_count": sum(len(item["chunks"]) for item in functions),
        "unique_chunk_count": len(chunk_owners),
        "shared_chunk_count": sum(count > 1 for count in chunk_owners.values()),
        "library_flag_count": sum(item["ida_library_flag"] for item in functions),
        "thunk_flag_count": sum(item["ida_thunk_flag"] for item in functions),
        "executable_byte_count": sum(item["end"] - item["start"] for item in executable_segments),
        "executable_initialized_byte_count": sum(
            item["initialized_bytes"] for item in executable_segments
        ),
        "unassigned_executable_range_count": len(ranges),
        "unassigned_executable_byte_count": sum(end - start for start, end in ranges),
        "import_module_count": len(imports),
        "import_count": len(import_entries),
        "import_initialized_byte_count": sum(item["initialized_bytes"] for item in import_entries),
        "import_fully_initialized_slot_count": sum(
            item["initialized_bytes"] == _IMPORT_SLOT_SIZE for item in import_entries
        ),
    }
    return {
        "schema": {
            "version": 1,
            "description": "当前 IDA 的只读元数据快照；不记录时间，日期由主线程文档登记。",
            "addresses": "地址为整数；VA 为绝对地址，RVA 为 VA 减 imagebase；F: 后为八位大写十六进制 RVA。",
            "ranges": "所有 start/end 与二元列表均为 VA 半开区间 [start,end)，包含 start，不包含 end。",
            "functions": "entry_chunk_end 仅为入口块终点；chunks 保留该函数全部块及共享块归属。",
            "flags": "flags、permissions 为 IDA 原始值；名称、库与跳板标志仅为元数据，不表示完成或批准例外。",
            "chunk_counts": "归属数累计所有函数的 chunks；不同块按相同起止地址去重；共享块指归属至少两个函数的不同块。",
            "segments": "仅可执行段读取完整区间；全部字节经掩码确认初始化后才记录 sha256。",
            "unassigned_executable_ranges": "可执行段内未被当前函数块覆盖的区间；不认定为代码、填充或完成遗漏。",
            "imports": "逐模块保留名称、序号及条目；导入记录不证明磁盘 DLL 或运行时版本。",
            "import_slots": "每槽四字节；initialized_bytes 为掩码确认的字节数，仅全定义时输出 initialized_hex，否则为 null；不推测零值或运行时地址。",
            "scope": "除明确要求的全初始化导入槽十六进制外，不导出字节内容、原 EXE、资源、业务数据、伪代码或注释。",
        },
        "baseline": baseline,
        "segments": segments,
        "functions": functions,
        "imports": imports,
        "unassigned_executable_ranges": ranges,
        "counts": counts,
    }


def _unique_json_object(pairs):
    """拒绝已有快照中的重复键，避免解析时静默丢失内容。"""
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError(f"JSON 含重复键：{key}。")
        result[key] = value
    return result


def _reject_json_constant(value):
    """拒绝标准 JSON 不允许的 NaN、Infinity 等常量。"""
    raise ValueError(f"JSON 含非法常量：{value}。")


def _same_json_value(left, right):
    """比较 JSON 语义，忽略对象键顺序，但不将布尔值当作数字。"""
    if isinstance(left, bool) or isinstance(right, bool):
        return type(left) is type(right) and left == right
    if isinstance(left, dict) and isinstance(right, dict):
        return left.keys() == right.keys() and all(
            _same_json_value(left[key], right[key]) for key in left
        )
    if isinstance(left, list) and isinstance(right, list):
        return len(left) == len(right) and all(
            _same_json_value(a, b) for a, b in zip(left, right)
        )
    return left == right


def export_inventory(output_path):
    """仅向脚本所在子项目 doc 导出；相对路径以 doc 为基准，绝不覆盖已有文件。"""
    try:
        doc = (Path(__file__).resolve().parent.parent / "doc").resolve(strict=True)
        target = Path(output_path)
        if not target.is_absolute():
            target = doc / target
        if target.is_symlink():
            raise ValueError("导出目标不能是符号链接。")
        target = target.resolve()
        if not doc.is_dir() or target.parent != doc or target.suffix != ".json" or ":" in target.name:
            raise ValueError("导出目标必须是脚本所属 cg2d_re/doc 直属目录下的 .json 文件。")
    except (OSError, RuntimeError) as error:
        raise RuntimeError("无法解析脚本所属 doc 目录或导出目标。") from error

    inventory = build_inventory()
    serialized = json.dumps(inventory, ensure_ascii=False, sort_keys=True, indent=2, allow_nan=False)
    status = "已写入"
    try:
        # 独占创建同时处理并发文件创建；不创建目录、不改名、不删除或覆盖。
        with target.open("x", encoding="utf-8", newline="\n") as stream:
            stream.write(serialized)
            stream.write("\n")
    except FileExistsError:
        try:
            with target.open("r", encoding="utf-8") as stream:
                existing = json.load(
                    stream, object_pairs_hook=_unique_json_object, parse_constant=_reject_json_constant
                )
        except (OSError, ValueError) as error:
            raise RuntimeError(f"已有快照损坏或无法读取，拒绝覆盖：{target}。") from error
        if not _same_json_value(existing, inventory):
            raise RuntimeError(f"已有快照与当前元数据不同，拒绝覆盖；请明确指定新文件名：{target}。")
        status = "已存在"
    except (OSError, UnicodeError) as error:
        raise RuntimeError(f"写入快照失败，不自动删除、改名或覆盖：{target}。") from error
    return {"path": str(target), "status": status, "counts": inventory["counts"]}
