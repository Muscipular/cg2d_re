"""现有 x64 客户端只读内存采样；仅使用 Python 标准库与 Windows 查询 API。

用法：python InspectMemory.py PID 客户端MAP路径 输出Markdown路径 [--scan-tables]
不挂调试器、不暂停、不注入、不修改目标进程；快照不具备跨字段原子性。
布局以本次源码的 x64 类型为依据，换版本需重新核对；MAP 与 PE 时间戳不符即退出。
"""
import collections
import ctypes as c
from ctypes import wintypes as w
import datetime
from pathlib import Path
import re
import struct
import sys


class Region(c.Structure):
    _fields_ = [("base", c.c_void_p), ("allocation", c.c_void_p),
                ("allocation_protect", w.DWORD), ("partition", w.WORD),
                ("size", c.c_size_t), ("state", w.DWORD),
                ("protect", w.DWORD), ("kind", w.DWORD)]


class Counters(c.Structure):
    _fields_ = [("cb", w.DWORD), ("faults", w.DWORD)] + [
        (name, c.c_size_t) for name in (
            "peak_ws", "ws", "peak_paged", "paged", "peak_nonpaged",
            "nonpaged", "pagefile", "peak_pagefile", "private")]


class WorkingPage(c.Structure):
    _fields_ = [("address", c.c_void_p), ("flags", c.c_size_t)]


class AudioSpec(c.Structure):
    _fields_ = [("format", c.c_int), ("channels", c.c_int), ("freq", c.c_int)]


class AudioSample(c.Structure):
    _fields_ = [("spec", AudioSpec), ("data", c.c_void_p), ("length", c.c_uint32),
                ("mix", AudioSpec), ("pcm", c.c_void_p), ("frames", c.c_size_t)]


class AudioChannel(c.Structure):
    _fields_ = [("stream", c.c_void_p), ("sample", c.c_void_p), ("signal", c.c_void_p),
                ("cursor", c.c_size_t), ("loop_start", c.c_size_t), ("loop_end", c.c_size_t),
                ("left", c.c_float), ("right", c.c_float), ("loop", c.c_bool),
                ("repeats", c.c_uint32), ("playing", c.c_bool), ("failed", c.c_bool),
                ("reverb", c.c_void_p), ("output_frames", c.c_uint64),
                ("has_tail", c.c_bool), ("draining", c.c_bool)]


class AudioPrefix(c.Structure):
    # SdlAudioBackend::Impl 到 fixedMusicSample 为止的普通字段，末尾效果对象不读取。
    _fields_ = [("initialized", c.c_bool), ("device", c.c_uint32),
                ("samples", AudioSample * 500), ("channels", AudioChannel * 64),
                ("bgm_sample", AudioSample), ("bgm", AudioChannel),
                ("stop_request", c.c_void_p), ("worker", c.c_void_p),
                ("stop_notice", c.c_void_p), ("ready", c.c_void_p),
                ("abort", c.c_bool), ("bound", c.c_bool), ("gain", c.c_float),
                ("suppressed", c.c_bool), ("fixed_sample", AudioSample)]


def main():
    pid, map_path, output_path = int(sys.argv[1]), Path(sys.argv[2]), Path(sys.argv[3])
    kernel = c.WinDLL("kernel32", use_last_error=True)
    psapi = c.WinDLL("psapi", use_last_error=True)
    kernel.OpenProcess.argtypes = [w.DWORD, w.BOOL, w.DWORD]
    kernel.OpenProcess.restype = w.HANDLE
    kernel.CloseHandle.argtypes = [w.HANDLE]
    kernel.ReadProcessMemory.argtypes = [w.HANDLE, c.c_void_p, c.c_void_p, c.c_size_t, c.POINTER(c.c_size_t)]
    kernel.VirtualQueryEx.argtypes = [w.HANDLE, c.c_void_p, c.POINTER(Region), c.c_size_t]
    kernel.VirtualQueryEx.restype = c.c_size_t
    psapi.EnumProcessModulesEx.argtypes = [w.HANDLE, c.c_void_p, w.DWORD, c.POINTER(w.DWORD), w.DWORD]
    psapi.GetModuleFileNameExW.argtypes = [w.HANDLE, c.c_void_p, w.LPWSTR, w.DWORD]
    psapi.GetProcessMemoryInfo.argtypes = [w.HANDLE, c.POINTER(Counters), w.DWORD]
    psapi.QueryWorkingSetEx.argtypes = [w.HANDLE, c.c_void_p, w.DWORD]
    handle = kernel.OpenProcess(0x0400 | 0x0010, False, pid)
    if not handle:
        raise c.WinError(c.get_last_error())
    lines = ["# 只读内存采样", "", "时间：" + datetime.datetime.now(
        datetime.timezone(datetime.timedelta(hours=8))).isoformat(), "", f"PID：{pid}", ""]

    def read(address, size):
        buffer = c.create_string_buffer(size)
        done = c.c_size_t()
        if not kernel.ReadProcessMemory(handle, address, buffer, size, c.byref(done)) or done.value != size:
            raise c.WinError(c.get_last_error())
        return buffer.raw

    def unpack(address, fmt):
        return struct.unpack(fmt, read(address, struct.calcsize(fmt)))

    def mib(value):
        return f"{value / 1048576:.3f}"

    try:
        modules = (c.c_void_p * 2048)()
        needed = w.DWORD()
        if not psapi.EnumProcessModulesEx(handle, modules, c.sizeof(modules), c.byref(needed), 3):
            raise c.WinError(c.get_last_error())
        base = modules[0]
        filename = c.create_unicode_buffer(32768)
        if not psapi.GetModuleFileNameExW(handle, base, filename, len(filename)):
            raise c.WinError(c.get_last_error())
        image_path = Path(filename.value)
        pe_offset = unpack(base + 0x3C, "<I")[0]
        pe = read(base + pe_offset, 264)
        timestamp = struct.unpack_from("<I", pe, 8)[0]
        assert pe[:4] == b"PE\0\0" and struct.unpack_from("<H", pe, 24)[0] == 0x20B
        with image_path.open("rb") as image_file:
            image_file.seek(pe_offset)
            disk_pe = image_file.read(264)
            # PE32+ OptionalHeader.ImageBase 会被系统加载器改为 ASLR 实际基址。
            assert struct.unpack_from("<Q", pe, 48)[0] == base
            assert disk_pe[:48] == pe[:48] and disk_pe[56:] == pe[56:], "除 ASLR ImageBase 外 PE 头不同"
        map_text = map_path.read_text(encoding="utf-8", errors="strict")
        assert int(re.search(r"Timestamp is ([0-9a-fA-F]+)", map_text)[1], 16) == timestamp
        preferred = int(re.search(r"Preferred load address is ([0-9a-fA-F]+)", map_text)[1], 16)
        assert struct.unpack_from("<Q", disk_pe, 48)[0] == preferred
        symbols = {}
        for match in re.finditer(r"^\s+[0-9a-fA-F]{4}:[0-9a-fA-F]+\s+(\S+)\s+([0-9a-fA-F]{16})\s", map_text, re.M):
            symbols[match[1]] = base + int(match[2], 16) - preferred

        def symbol(name):
            found = [address for key, address in symbols.items() if key.startswith("?" + name + "@")]
            assert len(found) == 1, (name, found)
            return found[0]

        def global_value(name, fmt="<i"):
            return unpack(symbol(name), fmt)[0]

        lines += [f"目标：`{image_path.name}`；MAP/磁盘/进程 PE 头核对通过；未计算摘要。", ""]
        counters = Counters(cb=c.sizeof(Counters))
        if not psapi.GetProcessMemoryInfo(handle, c.byref(counters), c.sizeof(counters)):
            raise c.WinError(c.get_last_error())
        lines += [f"工作集：{mib(counters.ws)} MiB；私有提交：{mib(counters.private)} MiB；峰值工作集：{mib(counters.peak_ws)} MiB。", ""]
        regions = []
        address = 0
        while True:
            region = Region()
            if not kernel.VirtualQueryEx(handle, address, c.byref(region), c.sizeof(region)):
                if c.get_last_error() != 87:
                    raise c.WinError(c.get_last_error())
                break
            if region.state == 0x1000:
                regions.append((region.base, region.size, region.kind, region.allocation))
            address = (region.base or 0) + region.size
        totals = collections.defaultdict(lambda: [0, 0, 0])
        for start, size, kind, allocation in regions:
            totals[kind][0] += size
            for offset in range(0, size, 4096 * 8192):
                count = min(8192, (size - offset) // 4096)
                pages = (WorkingPage * count)()
                for i in range(count):
                    pages[i].address = start + offset + i * 4096
                if not psapi.QueryWorkingSetEx(handle, pages, c.sizeof(pages)):
                    raise c.WinError(c.get_last_error())
                for page in pages:
                    if page.flags & 1:
                        totals[kind][1] += 4096
                        if not page.flags & (1 << 15):
                            totals[kind][2] += 4096
        lines += ["## 虚拟内存分类（读取业务对象之前）", "", "| 类别 | 已提交 MiB | 驻留 MiB | 驻留私有页 MiB |", "| --- | ---: | ---: | ---: |"]
        for kind, values in totals.items():
            lines.append("| " + {0x20000: "私有", 0x40000: "映射", 0x1000000: "映像"}.get(kind, hex(kind)) + " | " + " | ".join(map(mib, values)) + " |")
        lines += ["", "## 业务全局量", "", "| 全局量 | 值 |", "| --- | ---: |"]
        names = ["gBinLoaderGraphicCapacity", "gMaxGraphicCount", "gBinLoaderAnimeCapacity", "gMaxAnimateCount9A4A4C", "gAnimateCount9A4A60", "gTextureNodeCount"]
        values = {}
        for name in names:
            values[name] = global_value(name)
            lines.append(f"| {name} | {values[name]} |")
        pixels = global_value("gGraphicTexturePixels", "<Q")
        lines.append(f"| gGraphicTexturePixels | {pixels} |")
        tables = [("gGraphicInfo", values[names[0]] * 32),
                  ("gGraphicDxCache103CFA4", values[names[0]] * 40),
                  ("gGraphicMapNoIndex", values[names[0]] * 4 + 40),
                  ("gAnimateRecords9A4A58", values[names[2]] * 56),
                  ("gAnimateCache9A4A5C", values[names[2]] * 48)]
        lines += ["", "## 主表请求大小与所在区域", "", "| 主表 | 源码请求 MiB | 所在区域 MiB |", "| --- | ---: | ---: |"]
        for name, size in tables:
            ptr = global_value(name, "<Q")
            region = next(r for r in regions if r[0] <= ptr < r[0] + r[1])
            assert ptr + size <= region[0] + region[1], (name, size, region)
            lines.append(f"| {name} | {mib(size)} | {mib(region[1])} |")
        lines += ["", f"主表请求合计：{mib(sum(size for _, size in tables))} MiB。",
                  f"图形节点 CPU 像素计数：{mib(pixels * 4)} MiB（不含 GPU/驱动副本）。", ""]
        node_bytes = read(symbol("gTextureNodes"), 4096 * 40)
        surface_count = 0
        pixel_sum = 0
        dimensions = collections.Counter()
        for i in range(4096):
            ptr = struct.unpack_from("<Q", node_bytes, i * 40)[0]
            if ptr:
                width, height = unpack(ptr, "<ii")
                assert 0 < width <= 32768 and 0 < height <= 32768
                surface_count += 1
                pixel_sum += width * height
                dimensions[(width, height)] += 1
        lines += [f"节点实读：{surface_count} 张表面，{mib(pixel_sum * 4)} MiB；与全局像素计数差：{pixel_sum - pixels} 像素。", "",
                  "最大尺寸节点（宽×高、数量）：" + "；".join(f"{width}×{height}：{count}" for (width, height), count in sorted(dimensions.items(), key=lambda item: item[0][0]*item[0][1], reverse=True)[:10]), ""]
        backend = global_value("gSdlAudioBackend", "<Q")
        if backend:
            audio = AudioPrefix.from_buffer_copy(read(backend, c.sizeof(AudioPrefix)))
            assert audio.initialized and audio.device != 0
            groups = [("音效", list(audio.samples)), ("普通BGM", [audio.bgm_sample]), ("固定音乐", [audio.fixed_sample])]
            lines += ["## 音频双份 PCM", "", "| 类别 | 已加载样本数 | 原 PCM MiB | F32 双声道 MiB |", "| --- | ---: | ---: | ---: |"]
            for label, samples in groups:
                active = [s for s in samples if s.data]
                for sample in active:
                    assert sample.pcm and sample.mix.format == 0x8120 and sample.mix.channels == 2
                    assert sample.spec.freq == sample.mix.freq and 0 < sample.spec.freq <= 384000
                    assert sample.length == sample.frames * sample.spec.channels * ((sample.spec.format & 255) // 8)
                    for ptr, size in [(sample.data, sample.length), (sample.pcm, sample.frames * 8)]:
                        assert any(start <= ptr and ptr + size <= start + length for start, length, _, _ in regions)
                lines.append(f"| {label} | {len(active)} | {mib(sum(s.length for s in active))} | {mib(sum(s.frames * 8 for s in active))} |")
            effect_formats = collections.defaultdict(lambda: [0, 0, 0])
            for sample in audio.samples:
                if sample.data:
                    entry = effect_formats[(sample.spec.format, sample.spec.channels, sample.spec.freq)]
                    entry[0] += 1
                    entry[1] += sample.length
                    entry[2] += sample.frames * 8
            lines += ["", "| 音效格式 / 声道 / Hz | 数量 | 原 PCM MiB | F32 双声道 MiB |", "| --- | ---: | ---: | ---: |"]
            for (fmt, channels, freq), (count, original, converted) in sorted(effect_formats.items()):
                lines.append(f"| {hex(fmt)} / {channels} / {freq} | {count} | {mib(original)} | {mib(converted)} |")
            raw_bytes = sum(s.length for _, group in groups for s in group if s.data)
            mono_saved = sum(s.frames * 4 for s in audio.samples if s.data and s.spec.channels == 1)
            lines += ["", f"全部原 PCM 副本合计：{mib(raw_bytes)} MiB。",
                      f"音效单声道维持 F32 单声道的理论节省：{mib(mono_saved)} MiB（还需实现和等价验证）。", ""]
            definitions = Path(__file__).resolve().parents[3] / "src/audio/LegacySeDefinitions.inc"
            paths = re.findall(r'\{\d+, L"([^"]+)"', definitions.read_text(encoding="utf-8", errors="strict"))
            # 初始化遇缺失文件会压缩槽号；只有全部270项加载时才能按表序映射。
            if len(paths) == 270 and sum(bool(s.data) for s in audio.samples) == 270:
                first = {}
                duplicated_raw = duplicated_float = unique_mono = 0
                lines += ["## 音效同路径重复预加载", "", "| 同路径槽号 | 原 PCM 重复 MiB | F32 重复 MiB |", "| --- | ---: | ---: |"]
                for index, path in enumerate(paths):
                    sample = audio.samples[index]
                    if path in first:
                        previous = audio.samples[first[path]]
                        assert sample.length == previous.length and sample.frames == previous.frames
                        assert bytes(sample.spec) == bytes(previous.spec)
                        assert sample.data != previous.data and sample.pcm != previous.pcm
                        duplicated_raw += sample.length
                        duplicated_float += sample.frames * 8
                        lines.append(f"| {first[path]} / {index} | {mib(sample.length)} | {mib(sample.frames * 8)} |")
                    else:
                        first[path] = index
                        if sample.spec.channels == 1:
                            unique_mono += sample.frames * 4
                lines += ["", f"270 个槽引用 {len(first)} 个不同路径；重复原 PCM {mib(duplicated_raw)} MiB，重复 F32 {mib(duplicated_float)} MiB。",
                          f"先移除全部原 PCM、再共享同路径 F32，合计理论节省 {mib(raw_bytes + duplicated_float)} MiB。",
                          f"在上述两步之后，唯一单声道音效改为 F32 单声道，额外理论节省 {mib(unique_mono)} MiB；三步合计 {mib(raw_bytes + duplicated_float + unique_mono)} MiB。",
                          "重复判据为源码同路径、相同格式/长度、独立分配；未计算 PCM 摘要，未比较音频正文。", ""]
            lines += ["音效最大样本（槽号从0开始）：", "", "| 槽号 | 秒 | 原 PCM MiB | F32 双声道 MiB |", "| --- | ---: | ---: | ---: |"]
            for index, sample in sorted(enumerate(audio.samples), key=lambda item: item[1].frames, reverse=True)[:10]:
                if sample.data:
                    lines.append(f"| {index} | {sample.frames / sample.mix.freq:.2f} | {mib(sample.length)} | {mib(sample.frames * 8)} |")
            active_channels = [channel for channel in audio.channels if channel.stream]
            for channel in active_channels:
                assert backend + AudioPrefix.samples.offset <= channel.sample < backend + AudioPrefix.channels.offset
                assert (channel.sample - backend - AudioPrefix.samples.offset) % c.sizeof(AudioSample) == 0
            if audio.bgm.sample:
                assert audio.bgm.sample == backend + AudioPrefix.bgm_sample.offset
            lines += ["", f"已绑定音效通道：{len(active_channels)}；正在播放：{sum(channel.playing for channel in active_channels)}。",
                      f"布局核对：Sample={c.sizeof(AudioSample)}，Channel={c.sizeof(AudioChannel)}，samples偏移={AudioPrefix.samples.offset}，bgmSample偏移={AudioPrefix.bgm_sample.offset}，fixedMusicSample偏移={AudioPrefix.fixed_sample.offset}。",
                      "已核对音频格式、采样率、字节数/帧数关系、缓冲区归属及通道引用；不读取 PCM 正文。", ""]
        lines += ["## 最大已提交区域", "", "| 区域基址 | MiB | 类型 |", "| --- | ---: | --- |"]
        for start, size, kind, allocation in sorted(regions, key=lambda r: r[1], reverse=True)[:25]:
            lines.append(f"| 0x{start:X} | {mib(size)} | {hex(kind)} |")
        lines += ["", "局限：查询未暂停客户端，计数可能变化；提交、驻留、源码请求是不同口径，不能互加。没有采集历史分配栈，不能据此断言泄漏。"]
        if "--scan-tables" in sys.argv[4:]:
            capacity = values["gBinLoaderGraphicCapacity"]
            bound = values["gMaxGraphicCount"]
            lines += ["", "## 主表逐项只读扫描", "", "此项会读取已分配主表正文，可能将非驻留页面调入；上面的工作集在扫描前取得。"]
            for name, stride in [("gGraphicInfo", 32), ("gGraphicDxCache103CFA4", 40)]:
                ptr = global_value(name, "<Q")
                counts = collections.Counter()
                page_sets = {n: set() for n in (64, 256, 1024)}
                highest = -1
                for start in range(0, capacity, 16384):
                    count = min(16384, capacity - start)
                    block = read(ptr + start * stride, count * stride)
                    for offset in range(count):
                        record = block[offset * stride:(offset + 1) * stride]
                        index = start + offset
                        if any(record):
                            counts["非零记录"] += 1
                            highest = index
                            if index >= bound:
                                counts["加载上界以后的非零记录"] += 1
                            for n, pages in page_sets.items():
                                pages.add(index // n)
                        if stride == 40:
                            head, width, height, palette, b0, b1, flag, b3, action, x, y = struct.unpack("<QHH4xQBBBBHhh6x", record)
                            counts["head非空"] += bool(head)
                            counts["palette非空"] += bool(palette)
                            counts["宽高非零"] += bool(width or height)
                            counts["动作属性非零"] += bool(action)
                            counts["坐标非零"] += bool(x or y)
                            counts["四字节旧标志非零"] += bool(b0 or b1 or flag or b3)
                lines += ["", f"### {name}", "", f"扫描 {capacity} 项；最高非零下标 {highest}；上界以后的非零项 {counts['加载上界以后的非零记录']}。", ""]
                for label, count in counts.items():
                    lines.append(f"- {label}：{count}")
                if stride == 40:
                    lines += ["", "按当前非零记录分布测算的稀疏分页载荷（只计原40字节记录，不含分配开销，也不预测其他场景）：", ""]
                    for n, pages in page_sets.items():
                        lines.append(f"- 每页 {n} 项：{len(pages)} 页，{mib(len(pages) * n * stride)} MiB。")
            ptr = global_value("gGraphicMapNoIndex", "<Q")
            highest_map = nonzero_maps = 0
            for start in range(0, capacity + 10, 65536):
                count = min(65536, capacity + 10 - start)
                for offset, (value,) in enumerate(struct.iter_unpack("<i", read(ptr + start * 4, count * 4))):
                    if value:
                        highest_map = start + offset
                        nonzero_maps += 1
            lines += ["", f"MapNo 索引：{nonzero_maps} 个非零项，最高非零下标 {highest_map}。仅是本次已加载键分布，不代表查询者可访问上界。", ""]
        output_path.write_text("\n".join(lines).rstrip() + "\n", encoding="utf-8", errors="strict")
        print("\n".join(lines))
    finally:
        kernel.CloseHandle(handle)


if __name__ == "__main__":
    main()
