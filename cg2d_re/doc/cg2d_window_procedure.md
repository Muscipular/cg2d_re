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
- IME 唯一 owner 位于 platform 内部状态：description `[100]@73ED70`、
  `HIMC@73EDD8`、previous HIMC `73EDDC`、composition `[256]@73EDE0`、
  attribute `[256]@73EEE0`、cursor position `73EFE0`、conversion `73EFE4`、
  sentence `73EFE8`、candidate owner `73EFEC`，以及语义证据不足、仅保留地址关联的
  byte `73EFF0`。内部头仅暴露 `uintptr_t`/定宽值/owner 指针，不向业务层泄漏 `HIMC`。
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
- `platform::InitializeIme_458FD0()` / `ShutdownIme_459040()`：已按原 IME 生命周期实现；
  退出已接入 `ShutdownClient_457A40@0x457A7C`，初始化留待 `0x457804` 精确位置，
  证据见 `cg2d_ime_lifecycle.md`。
- `WIN_PROC` 的 892 条指令中不存在 `SetCapture`、`ReleaseCapture`、拖拽复位或 DPI/偏移/裁剪/`ScreenToClient` 坐标变换；本实现不补这些行为。

## `Renderer::IsLost@4536D0` 的窗口激活依赖

- 原函数区间 `[0x4536D0,0x45375C)`，无参，返回完整 W32 `0/1`。`0x4536D0` 读取 `W32[0x103CF24]`；仅正数进入循环，`0x4536E0` 从池首 `0x1020F08` 开始，`0x453700` 每轮推进 `0x1C`。各项只读首字段 surface 指针；非空才在 `0x4536EE` 调其 vtable `+0x60`。调用结果非零时 `0x4536F5` 将累计值设为 1，不提前退出；`0x4536FA` 每轮重读 count，`0x453703/705` 以有符号比较决定是否续行。
- 随后依序读取 `gTempSurface@0x103CF14`（`0x453708`）、renderer `+0x0C` 的 Back（`0x453720/725`）、renderer `+0x08` 的 front（`0x45373B/741`）。各指针非空才调用其 surface vtable `+0x60`；返回非零将结果置 1，仍继续后续检查。front 为空或其调用返回零时返回先前累计值；front 调用返回非零时直接返回 1（`0x453746..759`）。函数本体不检查 renderer 指针是否为空。
- 唯一直接调用点 `WIN_PROC@0x466E2D`：`0x466E11..2B` 仅完整 `wParam32` 为 1/2 且 renderer 非空才进入；`0x466E32` 严格比较结果与 1，命中后 `0x466E36` 写 `gRendererLost=1`，不因本次返回零清该槽。与 `TestCooperativeLevel@44C6B0` 是两个独立入口。
- 缓存池半开区间 `[0x1020F08,0x103CF08)` 及 count 的创建/发布合同见 [DX 缓存证据第 63–70 节](cg2d_graphic_dx_cache.md#63-g3b1-证据来源与覆盖边界)：创建入口清 count，整轮完成才发布 4096，不把失败前缀当作已发布计数。这里仅记录 `IsLost` 对当前 count 的读取方式，不推断间接修改或失败后的可用池长度。
- 新端当前只有 [WindowProcedureDependencies.hpp](../src/platform/WindowProcedureDependencies.hpp) 的声明和窗口调用；[SurfaceState.hpp](../include/cg/render/SurfaceState.hpp) 的 CPU Back/Temp 像素数组不是原 surface 状态或 4096 项缓存的 owner。已批准的 DX7 底层 surface lost 机制替换不等于批准上层 `IsLost` 固定返回零或一。统一资源归属、创建/重建/销毁时点，以及 SDL 驱动与等价失败状态的生产、探测、消费策略仍待 SPEC 裁决；本项不新增伪探测实现，不计窗口激活链、链接或运行完成。

<a id="surface-loss-native"></a>
## IsLost 原生资源检测

上述为早期状态。本轮复核4536D0全函数57条指令及466E0C..466E3C调用门禁；
当前节点池、Temp、Back与SDL呈现器均已有唯一owner和实际创建/释放链。
按既有DX7底层替换与用户工程委托，采用以下局部SPEC，不恢复COM或驱动HRESULT。

1. 原序扫描gTextureNodeCount个节点，每轮重读S32数量，非空T*才检查；
   命中lost只累计1，不提前结束。之后检查Temp、Back、front，返回完整0/1。
   不扩大至原函数未检查的AutoMap、变换缓存或3D接口。
2. 节点/Temp由GetTextureSdlSurface借用既有SDL包装；非空原生T*的包装
   或pixels缺失视为失效。Back借用GetCpuBackSdlSurface，非空时检查pixels。
   空资源槽仍跳过，不把尚未创建/已释放当成失效，不探测已释放指针。
   CPU内存不因失焦或SDL_SURFACE_LOCKED丢失，不锁面、不改变引用计数。
3. front沿SdlPresenter唯一Impl。未发布Impl表示原front为空；
   已发布实例通过SDL_GetRenderOutputSize探测输出查询失败，不把零尺寸、
   最小化或失焦作为失败。既有四项提交操作的失败均锁存为资源失效。
4. 每个成功创建的呈现器登记自己的SDL事件观察器，仅匹配自身windowID的
   RENDER_DEVICE_RESET/RENDER_DEVICE_LOST，记录失效且不渲染、不改全局
   gRendererLost、不消费事件。回调可能跨线程，仅写Impl内原子标志；
   注册失败沿Create现有失败清理，不发布半初始化实例。
   TARGETS_RESET不单独置位：本呈现器只有STREAMING纹理，没有TARGET纹理。
5. 失效在该Impl生命周期内不清除，Present检测前后保留失效结果；
   Destroy先移除观察器，再按现有顺序释放纹理/renderer/窗口/VIDEO。
   新建Impl才恢复初始健康态。Flip原失败置位及帧奇偶翻转不变，
   WM_ACTIVATE仍仅在结果==1时置位，不因本次健康清除已有lost。
6. IsLost不泵消息、不触发呈现或重建、不合并TestCooperativeLevel；
   SDL类型只留render后端。检测不保证GPU故障在驱动报告前可见，
   不宣称DX7所有异常时点等价或窗口激活流程已经运行验收。

API合同按本地SDL3-3.4.8头文件核对：SDL_events.h的SDL_RenderEvent含windowID，
Add/RemoveEventWatch及可能跨线程回调，SDL_render.h的GetRenderOutputSize
返回bool且仅限主线程。沿现有主线程呈现约束，不新增依赖或修改输入事件循环。

观察器释放补证：只读SDL官方release-3.4.8的
`src/events/SDL_eventwatch.c`，Dispatch与Remove均持同一list锁；
跨线程Remove等当前回调返回，同线程分发期间移除则标记removed，
后续迭代跳过。新观察器只做windowID判断与原子写入，不调用销毁/事件API。
源码来源：
`https://raw.githubusercontent.com/libsdl-org/SDL/release-3.4.8/src/events/SDL_eventwatch.c`。
未下载为工程依赖，不以SDL源码代替cg2d指令证据。

实施结果：IsLost_4536D0已有唯一本体，原WM_ACTIVATE调用无需修改即对应到实现；
SdlPresenter新增原生探测、两设备事件观察及四提交失败锁存。
静态核对节点/Temp/Back/front次序、动态count、空槽/锁定语义、
事件windowID过滤、失败不发布、先注销后释放及新Impl重置边界。
既有Flip、RendererDestroy、TestCooperativeLevel、全屏切换、截图和
WindowProcedure本体逐字保留，466B80先Destroy后Init的现有调用不变。
未执行新增函数、构建、测试、客户端或检查产物；设备丢失恢复和画面仍未运行验收。
本步只补局部适配与唯一进度，不对齐总SPEC、不压缩日志。

## IME候选通知的上下文重取 SPEC（2026-10-02）

本组沿原WIN_PROC的候选通知分支修复共享状态读取顺序。原4677EE先读取gHIMC；非空时467804调用ImmGetConversionStatus，随后467809重新读取gHIMC。原转换状态API的返回不作门禁，因此不能沿用调用前快照，也不能仅在成功时重取。当前代码的const context把两次原读取合并，需修正。

候选通知常量按本机Windows SDK Imm.h核实：变化3、关闭4、打开5。原打开路径467821查询所需容量，释放旧候选表后467841分配；467849先发布新表，467850再读取gHIMC，467857以本次分配返回指针填表。变化路径467890查询容量，释放旧表后4678B0分配；4678B5先读取gHIMC，4678BE再发布新表，4678C7填表。两者均保留分配失败后仍调用填表API、容量为零时清候选槽；关闭路径仅释放并清槽。

按既有工程委托采用以下最小修复：context改为可更新局部，在转换状态调用后无条件重取；calloc结果先存非拥有局部指针，按通知种类保留读取context与发布共享候选槽的先后顺序，填表传本次分配结果。沿用现有std::calloc/std::free承载，不变更分配器、不新建状态或添加失败早退。此次不批准CRT宿主提案，也不将其私有堆、错误槽和new-handler差异记为等价。

只改WM_IME_NOTIFY块，其他消息、候选列表绘制与输入数据流保持。静态核原4677EE..4678D5、两个原calloc调用点、共享上下文和候选槽的读写顺序，以及零容量/空分配路径；不新增或执行测试、不构建、查产物、运行客户端或哈希校验。完整输入法重入、显示与生命周期仍需后续适用运行验收。

## 按键写序与七鼠标坐标路径修复（BUT01，2026-10-08）

[本步原基线](cg2d_window_input_gate_restore_spec.md#window-input-gate-baseline)固定十个局部窗口376条1307B/十页、十四ftol和480/640两原F32常量8B。原WIN_PROC892条与46591A窗口注册只是既有完整合同及本次身份核实；不称本步重验了全部消息或跳表。

[最新裁决](cg2d_window_input_gate_restore_spec.md#window-input-gate-decision)覆盖上文“有效键域外停止写入”和“零尺寸轴回原始坐标”旧策略：按RPLAN.24删除两个键域外早退及两个零轴门禁，非640×480仍先Y后X、各在除法处读当前Height/Width，不增加保护、预取、浮点或故障模拟。普通按下保原多次W8写及末次fresh读；抬起保一次(old&F5)|1写，活动态/dirty原序保持。

F10的上文“按下规则”只说明最终旗位结果；原4671C3..4671D4只在局部old完成OR2/可选OR0C/OR10后发布一次。当前该分支改为一次W8写，无活动态门禁，先处理dirty/返0保持。七鼠标提供者、up/down门禁、原生默认参数/返回、RECT局部适配及2026-10-02 IME上下文重取全文保留。退出提示沿原字节，旧CP950命名不作新编码裁决。

源仅一cpp四片段/三个既有私有helper与F10，不增加helper/owner/F；既有语义181补局部证据，LOOP/INPUT各限定I/S。窗口完整生产消费、焦点/IME、资源/alias寿命、所有callee、正常业务失败、机器ABI/链接、显示/运行及R8继续，异常不专项处理或记等价通过。

### BUT01 实际限定静态结果

四处源码修复、整个cpp仅四片段差异、十原局部376条1307B/十四ftol/两常量8B、原写序与七调用/回调注册、完整机器与派生差异模型、十三文件编码/四锚点及四限定I-S各25来源已通过。文本汇总和Check均退出0。
实际LOOP136/INPUT30、E1022；语义1723/有效1716、文档1834（49.84%）、F864/R7已审429余2632、pending18/ECX pending7、closed11/全门禁保持。此结论只限本SPEC局部开发静态，未编译、链接、运行或证明完整窗口/输入验收，整体目标继续active。
