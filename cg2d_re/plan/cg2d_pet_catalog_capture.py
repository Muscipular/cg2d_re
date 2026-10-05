# -*- coding: utf-8 -*-
"""PCP01：仅为旧目录原端生产取证；定义/构造不安装，arm须已有明确许可。
配套 doc/cg2d_pet_catalog_production_spec.md。不得用本工具推定目录容量。
"""

import ntpath

import ida_dbg
import ida_ida
import ida_idd
import ida_nalt
import idc


class PetCatalogCapture(ida_dbg.DBG_Hooks):
    """三个定点写监视；命中保持暂停，证据只放本对象，清理由disarm显式完成。"""

    TARGETS = (
        (0xCA0478, 2, "首条used"),
        (0xC8AF84, 4, "目录页数"),
        (0xCC3E64, 4, "目录图号"),
    )
    REGISTERS = ("EIP", "EAX", "EBX", "ECX", "EDX", "ESI", "EDI", "EBP", "ESP")

    def __init__(self):
        super().__init__()
        self.existing_breakpoints = []
        self.owned = {}
        self.owned_fields = {}
        self.events = []
        self.hooked = False
        self.state = "未安装"

    @staticmethod
    def _breakpoint(ea):
        point = ida_dbg.bpt_t()
        return point if ida_dbg.get_bpt(ea, point) else None

    @staticmethod
    def _point_fields(point):
        return {
            "ea": int(point.ea), "size": int(point.size), "type": int(point.type),
            "flags": int(point.flags), "pid": int(point.pid), "tid": int(point.tid),
            "pass_count": int(point.pass_count),
        }

    @staticmethod
    def _memory(ea, size):
        # 真实调试内存；None/短读就是失败，不取静态数据库或补零。
        data = idc.read_dbg_memory(ea, size)
        if data is None or len(data) != size:
            raise RuntimeError("调试内存读取失败：" + hex(ea))
        return data.hex()

    def arm(self):
        """仅暂停的同名同基址原进程；不负责启动、附加或恢复运行。"""
        if self.owned or self.hooked:
            raise RuntimeError("本对象已安装或尚有未清理点")
        if (ida_nalt.get_root_filename().lower() != "cg2d.exe"
                or ida_ida.inf_is_64bit() or ida_nalt.get_imagebase() != 0x400000):
            raise RuntimeError("当前数据库不是本项原端")
        if not ida_dbg.is_debugger_on() or ida_dbg.get_process_state() != ida_dbg.DSTATE_SUSP:
            raise RuntimeError("原进程必须已附加并暂停")
        module = ida_idd.modinfo_t()
        if (not ida_dbg.get_module_info(0x400000, module)
                or ntpath.basename(module.name).lower() != "cg2d.exe"
                or module.base != 0x400000):
            raise RuntimeError("运行模块名/基址不符，先核身份或重定位")

        self.existing_breakpoints = []
        hardware = 0
        for index in range(ida_dbg.get_bpt_qty()):
            point = ida_dbg.bpt_t()
            if not ida_dbg.getn_bpt(index, point):
                raise RuntimeError("现有断点读取失败")
            self.existing_breakpoints.append(self._point_fields(point))
            if point.type != ida_idd.BPT_SOFT and point.flags & ida_dbg.BPT_ENABLED:
                hardware += 1
        if hardware > 1:
            raise RuntimeError("三个硬件监视所需槽不足，不改现有点")
        for ea, size, _name in self.TARGETS:
            if ea < module.base or ea + size > module.base + module.size:
                raise RuntimeError("监视字段不在已核模块内")
            for offset in range(size):
                if self._breakpoint(ea + offset) is not None:
                    raise RuntimeError("监视范围已有断点：" + hex(ea))

        if not self.hook():
            raise RuntimeError("安装通知hook失败，未添加断点")
        self.hooked = True
        try:
            for ea, size, name in self.TARGETS:
                if not ida_dbg.add_bpt(ea, size, ida_idd.BPT_WRITE):
                    raise RuntimeError("安装写断点失败：" + hex(ea))
                self.owned[ea] = (size, name)
                point = self._breakpoint(ea)
                if (point is None or point.ea != ea or point.size != size
                        or point.type != ida_idd.BPT_WRITE
                        or not point.flags & ida_dbg.BPT_ENABLED
                        or not point.flags & ida_dbg.BPT_BRK):
                    raise RuntimeError("写断点未按要求生效：" + hex(ea))
                self.owned_fields[ea] = self._point_fields(point)
        except Exception as install_error:
            try:
                self.disarm()
            except Exception as cleanup_error:
                raise RuntimeError(
                    "安装失败且清理未完成：" + str(install_error) + "；" + str(cleanup_error)
                ) from cleanup_error
            raise
        self.state = "已安装，进程仍暂停"
        return self.report()

    def dbg_bpt(self, tid, bptea):
        if bptea not in self.owned:
            return 0
        event = {
            "sequence": len(self.events) + 1, "tid": int(tid),
            "watch_address": int(bptea), "registers": {}, "memory": {},
        }
        try:
            if ida_dbg.get_current_thread() != tid:
                raise RuntimeError("当前线程不是事件线程，不采错线程寄存器")
            for name in self.REGISTERS:
                value = ida_idd.regval_t()
                if not ida_dbg.get_reg_val(name, value):
                    raise RuntimeError("寄存器读取失败：" + name)
                event["registers"][name] = int(value.ival)
            for ea, size, name in self.TARGETS:
                event["memory"][name] = {"ea": ea, "hex": self._memory(ea, size)}
            event["memory"]["首条96B"] = {"ea": 0xCA0478, "hex": self._memory(0xCA0478, 96)}
            ip = event["registers"]["EIP"]
            module = ida_idd.modinfo_t()
            if not ida_dbg.get_module_info(ip, module):
                raise RuntimeError("暂停指令模块无法读取")
            start = max(int(module.base), ip - 16)
            end = min(int(module.base + module.size), ip + 16)
            if end <= start:
                raise RuntimeError("暂停指令不在可定位模块范围")
            event["instruction_module"] = ntpath.basename(module.name)
            event["memory"]["暂停位置邻窗"] = {"ea": start, "hex": self._memory(start, end - start)}
        except Exception as error:
            event["error"] = str(error)
        self.events.append(event)
        self.state = "已命中，保持暂停；EIP尚非已证写者"
        return 0

    def dbg_process_exit(self, pid, tid, ea, exit_code):
        self.state = "进程已退出，等待显式清理"
        self.events.append({"process_exit": int(exit_code)})

    def dbg_process_detach(self, pid, tid, ea):
        self.state = "进程已脱离，等待显式清理"
        self.events.append({"process_detach": True})

    def disarm(self):
        """仅删本轮自己创建的点；冲突或失败明确报出，保留对象供继续清理。"""
        if ida_dbg.get_process_state() == ida_dbg.DSTATE_RUN:
            raise RuntimeError("须先暂停进程再清理，工具不擅自暂停/终止")
        errors = []
        for ea, (size, _name) in tuple(self.owned.items()):
            point = self._breakpoint(ea)
            if point is None:
                del self.owned[ea]
                self.owned_fields.pop(ea, None)
                continue
            if (point.ea != ea or point.size != size or point.type != ida_idd.BPT_WRITE
                    or (ea in self.owned_fields
                        and self._point_fields(point) != self.owned_fields[ea])):
                errors.append("点已被改动，不删他人点：" + hex(ea))
                continue
            if not ida_dbg.del_bpt(ea) or self._breakpoint(ea) is not None:
                errors.append("删除未完成：" + hex(ea))
                continue
            del self.owned[ea]
            self.owned_fields.pop(ea, None)
        if not self.owned and self.hooked:
            if self.unhook():
                self.hooked = False
            else:
                errors.append("通知hook解除失败")
        if errors:
            self.state = "清理未完成"
            raise RuntimeError("；".join(errors))
        self.state = "已清理"
        return self.report()

    def report(self):
        return {
            "state": self.state, "hooked": self.hooked,
            "owned_breakpoints": list(self.owned),
            "existing_breakpoint_fields": self.existing_breakpoints,
            "events": self.events,
            "capacity_proven": False,
        }
