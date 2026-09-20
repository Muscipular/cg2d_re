# cg2d 窗口过程还原

## 边界与分派证据

- 原函数为 `WIN_PROC@[0x466DC0,0x467904)`，大小 `0xB44`，IDA 统计 892 条指令、136 个基本块。
- 唯一静态引用是 `WinMain@0x46591A` 写入 `WNDCLASSA.lpfnWndProc`；当前 `ClientEntry.cpp` 已沿相同位置登记 `WindowProcedure`，新增定义即正式接线。
- 第一张跳表：`jpt_466E05` / `byte_467920`，覆盖消息 `0x0002..0x00A0`，显式目标为 `2、5、6、10h、20h、A0h`。
- 第二张跳表：`jpt_467139` / `byte_4679D8`，覆盖 `0x0101..0x010E`，显式目标为 `101h、102h、104h、10Dh、10Eh`；`100h` 在 `0x4670BF` 单独分派，`10Fh` 在 `0x467288` 单独分派。
- 第三张跳表：`jpt_4672CC` / `byte_467A04`，覆盖 `0x0112..0x0204`，显式目标为 `112h、200h..204h`；`205h` 在 `0x46766F` 单独分派。
- 第四张跳表：`jpt_467719` / `byte_467B10`，覆盖 `0x0206..0x0282`，显式目标为 `206h、207h、20Ah、281h、282h`。
- 所有非显式消息在 `0x4678E4..0x467901` 以原参数调用 `DefWindowProcA`，返回原生 `LRESULT`。

## 25 个显式消息

| 序号 | 消息 | 原地址与行为 | 返回 |
|---:|---|---|---:|
| 1 | `WM_DESTROY(2)` | `0x46703E`：`PostQuitMessage(0)` | 0 |
| 2 | `WM_SIZE(5)` | `0x466EA8`：更新活动态、帧截止与独立客户区尺寸；窗口化时执行 0/1/2 两阶段外框修正 | 0 |
| 3 | `WM_ACTIVATE(6)` | `0x466E0C`：无条件重置键状态；严格 `wParam32==1/2` 时检查独立 `IsLost` 并恢复调色板；按 `gRendererFlags&0x10` 更新活动态；刷新帧截止 | 0 |
| 4 | `WM_CLOSE(10h)` | `0x467027`：`DestroyWindow` | 0 |
| 5 | `WM_SETCURSOR(20h)` | `0x467052`：仅 `LOWORD(lParam)==HTCLIENT` 时按 `gValue9A6F44` 设置既有光标 | 1；否则默认 |
| 6 | `WM_NCMOUSEMOVE(A0h)` | `0x467086`：清 held/edge，不清 pulse；仅 suppression 从 0 转 1 时显示系统光标 | 0 |
| 7 | `WM_KEYDOWN(100h)` | `0x4670BF`：先消费 `gGraphicDirty==1`；仅活动态 2 写唯一 256 字节键表 | 0 |
| 8 | `WM_KEYUP(101h)` | `0x467140`：先消费 dirty；键字节写为 `(old&F5h)|1`，无活动态门禁 | 0 |
| 9 | `WM_CHAR(102h)` | `0x467242`：低字节 CR 或 `GetAsyncKeyState(VK_MENU)` 完整 `SHORT` 非零时吞掉，否则把低字节交给 `Input::AppendChar@44ADB0` | 0 |
| 10 | `WM_SYSKEYDOWN(104h)` | `0x46717E`：先消费 dirty；F10 走按键按下规则；Return 在允许时严格执行 Rebuild→WndInit→tick→phase=0→deadline；其他消息默认 | 0 或默认 |
| 11 | `WM_IME_STARTCOMPOSITION(10Dh)` | 跳表直接落入零返回公共尾部 | 0 |
| 12 | `WM_IME_ENDCOMPOSITION(10Eh)` | `0x467275`：只清组合字符串首字节 | 0 |
| 13 | `WM_IME_COMPOSITION(10Fh)` | `0x467288`：把 `lParam` 低 16 位 flags 交给真实 `sub_459580` | 0 |
| 14 | `WM_SYSCOMMAND(112h)` | `0x4672D3`：仅完整 `wParam32==F060h` 且窗口化时显示 CP950 退出确认；严格 `IDCANCEL` 刷新帧截止，其余默认 | 0 或默认 |
| 15 | `WM_MOUSEMOVE(200h)` | `0x467348`：映射坐标、重置位置；suppression 从 1 转 0 时隐藏系统光标；最后消费 dirty | 0 |
| 16 | `WM_LBUTTONDOWN(201h)` | `0x4673FC`：仅活动态 2，映射后调用 `UpdateLDown@506900` | 0 |
| 17 | `WM_LBUTTONUP(202h)` | `0x46749C`：无活动态门禁，映射后调用 `UpdateLUp@5069E0` | 0 |
| 18 | `WM_LBUTTONDBLCLK(203h)` | `0x46752F`：仅活动态 2，映射后调用 `UpdateLDbClick@506A90` | 0 |
| 19 | `WM_RBUTTONDOWN(204h)` | `0x4675CF`：仅活动态 2，映射后调用 `UpdateRDown@506B50` | 0 |
| 20 | `WM_RBUTTONUP(205h)` | `0x46766F`：无活动态门禁，映射后调用 `sub_506C40` | 0 |
| 21 | `WM_RBUTTONDBLCLK(206h)` | `0x467720`：仅活动态 2，映射后调用 `sub_506CF0` | 0 |
| 22 | `WM_MBUTTONDOWN(207h)` | 跳表直接落入零返回公共尾部；其他中键消息仍默认 | 0 |
| 23 | `WM_MOUSEWHEEL(20Ah)` | `0x4677C0`：仅活动态 2；符号扩展 `HIWORD(wParam)` 后取负，交给 `sub_506DB0` 语义入口 | 0 |
| 24 | `WM_IME_SETCONTEXT(281h)` | `0x4678D8`：仅把 x86 `lParam32` 清除低四位和符号位，再调用默认过程 | 默认过程原值 |
| 25 | `WM_IME_NOTIFY(282h)` | `0x4677EE`：刷新 conversion/sentence；3/5 重建候选表 0，4 释放，其他通知也吞掉 | 0 |

## 坐标与状态归属

- 鼠标原始坐标严格按无符号 `LOWORD/HIWORD(lParam32)` 取得；只有当前客户区严格为 `640×480` 时直传。
- 其他非零尺寸严格先计算 Y、再计算 X。旧端路径是 `fild`→载入 `480.0F/640.0F`→`fidiv`→x87 `__ftol`；因原始坐标为非负整数、客户区尺寸为正整数，其截断结果严格等价于先做安全的 W32 乘积，再做整数除法：`(uint32(y)*480u)/clientH`、`(uint32(x)*640u)/clientW`。对应 14 个 `__ftol` 调用：`0x46738E/3AE、0x467462/482、0x4674F5/515、0x467595/5B5、0x467635/655、0x4676C8/6E8、0x467786/7A6`。
- `gCurrentClientWidth/gCurrentClientHeight` 唯一承接原 `0x9A6F2C/30`；IDA 的全部引用均位于 `WIN_PROC`，不与目标尺寸 `gWindowWidth/gWindowHeight` 合并。
- `gWindowResizePhase` 唯一承接原 `0x9A7F68`；五处引用为 `0x466F19、0x466F2A、0x466F3D、0x466F9F、0x46722B`。
- `platform::gKeyStates[256]` 唯一承接原 `0x73EFF8`；`ClearKeyFlags_459760` 逐项 `&=EAh`，`ResetKeyState_459780` 逐项 `&=E8h`。CharacterSelect 索引 17 已改读此存储。
- `render::gGraphicDirty` 唯一承接原 `0x1020EE8`，与 pending count 和 special flag 分立；IDA 还显示未来 `FillDirtyArea@520AD0`、`DrawAllQueued@520DD0` 会复用。
- IME 唯一 owner 位于 platform 内部状态：`HIMC@73EDD8`、保留但未接入的 previous HIMC `73EDDC`、composition `[256]@73EDE0`、attribute `[256]@73EEE0`、cursor position `73EFE0`、conversion `73EFE4`、sentence `73EFE8`、candidate owner `73EFEC`。内部头仅暴露 `uintptr_t`/定宽值/owner 指针，不向业务层泄漏 `HIMC`。
- 滚轮入口 `sub_506DB0@[0x506DB0,0x506DC3)` 只对既有 `gLegacyMouseState.tailReset` 做 W32 回绕加法并返回原 delta。

## x64 适配与安全边界

- `HWND/HCURSOR/HIMC/LRESULT` 保持本机指针宽度；`HWND_NOTOPMOST` 使用 SDK 常量，`GWL_STYLE` 仍为 `LONG`。
- 所有需要解释的消息载荷先显式截取原 x86 低 32 位；默认消息仍把本机 `WPARAM/LPARAM` 原样传给 `DefWindowProcA`。
- `WM_IME_SETCONTEXT` 只将过滤后的低 32 位提升成原生 `LPARAM`；返回值不截断。
- 坐标换算不使用 x64 的 float32 中间结果，以免相对旧 x87 扩展精度发生二次舍入。例如 `x=29759、clientW=32781` 时，旧端截断为 580，float32 会舍入为 581，整数等价式仍得到 580。最大乘积分别为 `65535*480` 与 `65535*640`，均不溢出 W32。
- 有效 Win32 虚拟键域为 `0..255`。新端只对域外索引停止键表写入，避免越界；有效域行为不变。
- 客户区某一维为零时，仅该轴保留原始无符号坐标以避免 C++ 浮点转整数未定义行为；另一非零轴仍按原公式换算。所有非零尺寸行为不变。
- 两个 `RECT` 局部量清零初始化，只约束 Win32 API 失败后的未定义局部读取；API 成功路径不变。
- 候选表分配保持原行为：`calloc(1,size)` 后不增加失败分支，仍调用 `ImmGetCandidateListA`。

## 依赖状态与否定结论

- `render::IsLost_4536D0()`：IDA 原型为独立无参 `int()`，本项不把它并入 `TestCooperativeLevel@44C6B0`。
- `input::AppendChar_44ADB0(char)`：已按 `Input::AppendChar@[0x44ADB0,0x44B044)` 实现并由 `WM_CHAR` 接入；保持旧 ANSI/DBCS 字节逻辑，证据与安全边界见 `cg2d_input_append.md`。
- `platform::Sub_459580(int16 flags)`：已按 `sub_459580@[0x459580,0x4596C8)` 实现并由 `WM_IME_COMPOSITION` 接入；四段独立 flags、返回低字节及安全边界见 `cg2d_ime_composition.md`。
- `WIN_PROC` 的 892 条指令中不存在 `SetCapture`、`ReleaseCapture`、拖拽复位或 DPI/偏移/裁剪/`ScreenToClient` 坐标变换；本实现不补这些行为。
