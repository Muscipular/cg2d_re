import argparse
import os
import random
import re
import subprocess
import time
from pathlib import Path


FUNCTION_SYMBOL = re.compile(
    rb"^[ \t]*[0-9A-Fa-f]+[ \t]+00000000[ \t]+SECT[0-9A-Fa-f]+"
    rb"[ \t]+notype[ \t]+\(\)[ \t]+External[ \t]+\|[ \t]*([^ \t(]+)"
)
CLIENT_OBJECT = re.compile(r"CMakeFiles/cg2d_re_client\.dir/.+\.obj")


def diagnostic(data):
    try:
        return data.decode("utf-8")
    except UnicodeDecodeError:
        return repr(data)


def ninja_objects(args):
    if not args.ninja.is_file() or not (args.binary_dir / args.ninja_build_file).is_file():
        raise RuntimeError("Ninja或当前配置的构建图不存在。")
    target = os.path.relpath(args.target_file, args.binary_dir).replace("\\", "/")
    result = subprocess.run(
        [str(args.ninja), "-f", args.ninja_build_file, "-t", "query", target],
        cwd=args.binary_dir,
        capture_output=True,
    )
    if result.returncode != 0:
        raise RuntimeError(f"无法查询客户端Ninja链接输入：{target}\n{diagnostic(result.stderr)}")

    objects = []
    reading_inputs = False
    for line in result.stdout.decode("utf-8").splitlines():
        if re.match(r"^[ \t]+input:", line):
            reading_inputs = True
        elif re.match(r"^[ \t]+(?:validations|outputs):", line):
            reading_inputs = False
        elif reading_inputs:
            input_path = re.sub(r"^\|\|?[ \t]+", "", line.strip()).replace("\\", "/")
            if CLIENT_OBJECT.fullmatch(input_path):
                object_file = args.binary_dir / input_path
                if not object_file.is_file():
                    raise RuntimeError(f"Ninja链接输入不存在：{object_file}")
                objects.append(object_file)
            elif input_path.endswith(".obj"):
                raise RuntimeError(f"Ninja链接输入包含非客户端对象：{input_path}")
    return objects


def object_batches(objects, dumpbin):
    offset = 0
    while offset < len(objects):
        count = min(16, len(objects) - offset)
        while True:
            batch = objects[offset:offset + count]
            command = [str(dumpbin), "/SYMBOLS", *(str(path) for path in batch)]
            if len(subprocess.list2cmdline(command)) <= 24000:
                break
            if count == 1:
                raise RuntimeError(f"DUMPBIN对象路径过长：{batch[0]}")
            count //= 2
        yield command, batch
        offset += count


def main():
    parser = argparse.ArgumentParser(description="生成客户端函数顺序文件")
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("--object-dir", type=Path)
    source.add_argument("--ninja", type=Path)
    parser.add_argument("--ninja-build-file")
    parser.add_argument("--binary-dir", type=Path)
    parser.add_argument("--target-file", type=Path)
    parser.add_argument("--order-file", type=Path, required=True)
    parser.add_argument("--dumpbin", type=Path, required=True)
    args = parser.parse_args()
    started = time.perf_counter()

    if not args.dumpbin.is_file():
        raise RuntimeError(f"DUMPBIN不存在：{args.dumpbin}")
    if args.ninja is not None:
        if not args.ninja_build_file or args.binary_dir is None or args.target_file is None:
            raise RuntimeError("缺少Ninja函数顺序生成参数。")
        objects = ninja_objects(args)
    else:
        if not args.object_dir.is_dir():
            raise RuntimeError(f"客户端对象目录不存在：{args.object_dir}")
        objects = list(args.object_dir.rglob("*.obj"))
    if not objects:
        raise RuntimeError("客户端链接输入中没有.obj。")
    objects = sorted(set(objects), key=str)

    symbols = set()
    dumpbin_seconds = 0.0
    for command, batch in object_batches(objects, args.dumpbin):
        before_dumpbin = time.perf_counter()
        result = subprocess.run(command, capture_output=True)
        dumpbin_seconds += time.perf_counter() - before_dumpbin
        if result.returncode != 0:
            raise RuntimeError(
                f"无法读取客户端对象符号：{', '.join(map(str, batch))}\n"
                f"{diagnostic(result.stderr)}"
            )
        # 仅解码匹配到的符号，避免本机编码的说明文字影响提取。
        for line in result.stdout.splitlines():
            match = FUNCTION_SYMBOL.match(line)
            if match:
                symbols.add(match.group(1).decode("utf-8"))

    if len(symbols) < 2:
        raise RuntimeError(f"可排序的客户端公开函数不足两个：{len(symbols)}")
    ordered = sorted(symbols)
    random.shuffle(ordered)
    if args.order_file.exists():
        previous = args.order_file.read_text(encoding="utf-8").splitlines()
        if previous == ordered:
            ordered[0], ordered[1] = ordered[1], ordered[0]
    with args.order_file.open("w", encoding="utf-8", newline="\n") as stream:
        stream.write("\n".join(ordered) + "\n")

    elapsed = time.perf_counter() - started
    print(
        f"-- 客户端函数顺序已重新生成：{len(ordered)}个公开函数符号"
        f"（总耗时{elapsed:.2f}s，DUMPBIN耗时{dumpbin_seconds:.2f}s）",
        flush=True,
    )


if __name__ == "__main__":
    try:
        main()
    except (OSError, UnicodeError, ValueError, RuntimeError) as error:
        raise SystemExit(str(error))
