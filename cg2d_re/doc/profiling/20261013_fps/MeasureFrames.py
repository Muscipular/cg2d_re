# -*- coding: utf-8 -*-
# 登录界面中只读 gFrameCount；不暂停线程，不改变客户端内存或系统计时设置。
import argparse, ctypes, json, re, time
from pathlib import Path

parser = argparse.ArgumentParser(description='按同版 MAP 读取客户端已完成帧数。')
parser.add_argument('--pid', type=int, required=True)
parser.add_argument('--base', type=lambda value: int(value, 0), required=True)
parser.add_argument('--map-file', type=Path, required=True)
parser.add_argument('--seconds', type=float, default=5)
parser.add_argument('--output', type=Path)
args = parser.parse_args()
map_text = args.map_file.read_text(encoding='utf-8')
kernel = ctypes.WinDLL('kernel32', use_last_error=True)
kernel.OpenProcess.argtypes = [ctypes.c_uint, ctypes.c_bool, ctypes.c_uint]
kernel.OpenProcess.restype = ctypes.c_void_p
kernel.ReadProcessMemory.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_size_t, ctypes.POINTER(ctypes.c_size_t)]
kernel.CloseHandle.argtypes = [ctypes.c_void_p]
handle = kernel.OpenProcess(0x410, False, args.pid)
if not handle:
    raise ctypes.WinError(ctypes.get_last_error())

def read(address):
    value = ctypes.c_uint32(); count = ctypes.c_size_t()
    if not kernel.ReadProcessMemory(handle, address, ctypes.byref(value), 4, ctypes.byref(count)) or count.value != 4:
        raise ctypes.WinError(ctypes.get_last_error())
    return value.value

def address(symbol):
    found = re.search(r'\?' + symbol + r'@platform@cg@@\S+\s+([0-9a-fA-F]{16})', map_text)
    return args.base + int(found[1], 16) - 0x140000000

try:
    image_timestamp = read(args.base + read(args.base + 0x3c) + 8)
    map_timestamp = int(re.search(r'Timestamp is ([0-9a-fA-F]+)', map_text)[1], 16)
    if image_timestamp != map_timestamp:
        raise SystemExit('MAP 时间戳与运行客户端不符。')
    frame_address = address('gFrameCount'); state_address = address('gWindowActiveState')
    first = read(frame_address); start = time.perf_counter(); states = []
    while time.perf_counter() - start < args.seconds:
        states.append(read(state_address)); time.sleep(.1)
    last = read(frame_address); elapsed = time.perf_counter() - start
    completed = (last - first) & 0xffffffff
    result = {'pid': args.pid, 'map_timestamp': f'{map_timestamp:x}', 'seconds': elapsed,
              'completed_frames': completed, 'fps': completed / elapsed, 'active_states': sorted(set(states))}
    if args.output:
        args.output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8', newline='\n')
    print(json.dumps(result, ensure_ascii=False))
finally:
    kernel.CloseHandle(handle)
