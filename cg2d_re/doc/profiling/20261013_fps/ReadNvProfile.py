# -*- coding: utf-8 -*-
# 只读已安装 NVIDIA 驱动的相关 DRS 设置；没有设置、保存或删除配置调用。
# ABI/常量来源：https://github.com/NVIDIA/nvapi（nvapi.h / nvapi_interface.h / NvApiDriverSettings.h）。
import ctypes as c
import json
from pathlib import Path

class Binary(c.Structure):
    _fields_ = [('length', c.c_uint32), ('data', c.c_ubyte * 4096)]

class Value(c.Union):
    _pack_ = 4
    _fields_ = [('dword', c.c_uint32), ('binary', Binary), ('text', c.c_wchar * 2048), ('qword', c.c_uint64)]

class Setting(c.Structure):
    _pack_ = 4
    _fields_ = [('version', c.c_uint32), ('name', c.c_wchar * 2048),
                ('id', c.c_uint32), ('type', c.c_uint32), ('location', c.c_uint32),
                ('current_predefined', c.c_uint32), ('predefined_valid', c.c_uint32),
                ('predefined', Value), ('current', Value)]

class Application(c.Structure):
    _fields_ = [('version', c.c_uint32), ('predefined', c.c_uint32),
                ('name', c.c_wchar * 2048), ('friendly', c.c_wchar * 2048), ('launcher', c.c_wchar * 2048)]

nv = c.WinDLL('C:/Windows/System32/nvapi64.dll')
query = nv.nvapi_QueryInterface
query.argtypes = [c.c_uint32]
query.restype = c.c_void_p

def api(identifier, *args):
    address = query(identifier)
    if not address:
        raise RuntimeError(f'驱动未提供查询接口 {identifier:x}')
    return c.WINFUNCTYPE(c.c_int, *args)(address)

def check(status):
    if status != 0:
        raise RuntimeError(f'NVAPI 查询失败：{status}')

check(api(0x0150e828)())
session = c.c_void_p()
check(api(0x0694d52e, c.POINTER(c.c_void_p))(c.byref(session)))
settings = {'帧率上限': 0x10835002, '空闲帧率上限': 0x10835016, '垂直同步': 0x00a879cf,
            '撕裂控制': 0x005a375c, '可变刷新同步': 0x10a879ce, 'G-SYNC模式': 0x1194f158,
            'G-SYNC应用覆盖': 0x10a879cf, '电源管理': 0x1057eb71, '电池帧率': 0x10115c8c}
result = {}
try:
    check(api(0x375dbd6b, c.c_void_p)(session))
    profiles = []
    for name, identifier in [('基础配置', 0xda8466a0), ('当前全局配置', 0x617bff9f)]:
        profile = c.c_void_p()
        check(api(identifier, c.c_void_p, c.POINTER(c.c_void_p))(session, c.byref(profile)))
        profiles.append((name, profile))
    for name in ['cg2d_re_client.exe', 'sdl_official_probe.exe']:
        profile = c.c_void_p(); app = Application(); app.version = c.sizeof(app) | (1 << 16)
        status = api(0xeee566b2, c.c_void_p, c.c_wchar_p, c.POINTER(c.c_void_p), c.POINTER(Application))(
            session, name, c.byref(profile), c.byref(app))
        if status == 0:
            profiles.append((name, profile))
        else:
            result[name] = {'查找状态': status}
    get_setting = api(0x73bf8338, c.c_void_p, c.c_void_p, c.c_uint32, c.POINTER(Setting))
    for name, profile in profiles:
        values = {}
        for label, identifier in settings.items():
            setting = Setting(); setting.version = c.sizeof(setting) | (1 << 16)
            status = get_setting(session, profile, identifier, c.byref(setting))
            values[label] = {'status': status}
            if status == 0:
                values[label].update({'id': f'{identifier:08x}', 'type': setting.type,
                    'current': f'{setting.current.dword:08x}', 'predefined': f'{setting.predefined.dword:08x}',
                    'is_predefined': setting.current_predefined, 'location': setting.location})
        result[name] = values
finally:
    check(api(0xdad9cff8, c.c_void_p)(session))
text = json.dumps(result, ensure_ascii=False, indent=2)
(Path(__file__).parent / 'nvidia_profile_read.json').write_text(text + '\n', encoding='utf-8', newline='\n')
print(text)
