# LUM01：L01–L08 Lua 业务迁移

日期：2026-10-17。状态：已替换 x64 LuaJIT 预编译库，L01数据绑定及双状态生命周期已接入；L02–L08实施中，尚未构建／运行验收。

## 1. 用户要求与边界

迁移 `twcg_hook` 清单中的 L01–L08，直接接入 `cg2d_re` 的业务调用。LuaJIT 使用预编译 `.lib`；不迁入 Detours、地址补丁、汇编跳板或窗口过程替换，不构建 LuaJIT 源码。沿用现有 `m_log*`、64 位时间、UTF-8 源码、原生窗口与 GPU 渲染接口。

依子项目 AGENTS 第8条工程委托记录方案；不改函数表、不写 JSON 台账、不使用已废弃的统计脚本。Lua 迁移本身不延续上一轮透明问题的客户端启动授权。

## 2. 已核实的依赖

### 2.1 预编译库核对与替换

- 本线原有 `3rd/luajit/lua51.lib` 和 Lua 头文件；`luajit.h` 标识 `LuaJIT 2.1.ROLLING`。
- 对替换前的库执行只读 COFF 检查，成员为 **`14C machine (x86)`**，链接指令包含 **`/DEFAULTLIB:LIBCMT`**，是带静态 CRT 依赖的 x86 对象库。
- 本线 CMake 目标为 Windows/MSVC/x64。此库不能链接到该目标，不能通过重命名、忽略默认库或修改目标机器标记解决。
- 同源目录下 `twcg_hook`、`cg-moonboy-hook`、`gmsv-hook`、`cgmsv-hook` 中发现的 Lua 库也均为 x86；其中参考 `luajit.lib` 是指向 `luajit.exe` 的导入库，不是本线可用的 x64 静态库。
- 用户随后指定 `cgmsv_server/3rd/luajit/lua51.lib` 并要求替换项目库。该库成员均为 **`8664 machine (x64)`**，链接指令为 **`LIBCMT`／`OLDNAMES`**；配套头文件与本线的 LuaJIT 版本、版本号和版本符号声明一致。
- 已将指定库复制到本线 `3rd/luajit/lua51.lib`，9,916,302 字节，复制后逐字节核对一致，未计算摘要。架构阻塞解除；头文件与 CMake 未改，后续接入仍需落实静态 CRT 与客户端构建配置的合同，不把复制成功当作链接验证。

库确认后的构建方案：在本线 CMake 定义预编译静态导入目标，使用库中实际存在的 Lua C API 导出符号将它链接为 `cg2d_luajit.dll`。该 DLL 使用库自带的 LIBCMT，客户端保留原 CRT 配置，通过生成的导入库调用；Lua 状态、内部文件及内存始终由 LuaJIT 自身创建和释放，不跨 CRT 释放。DLL 随客户端复制。此适配仅链接已有对象，不编译 LuaJIT 源码，不忽略默认 CRT，也不改全项目 CRT。

### 2.2 脚本与编码

参考仓库只有 `lua/demo.lua`、`lua/battleSkillEffect.lua`，缺少 UI 入口和自动战斗策略；但本次已在用户实际游戏资源目录核实存在：

- `luaUI/init.lua`，及 `CONST.lua`、`ModuleBase.lua`、`GlobalEvent.lua`、`ModuleSystem.lua`、`ModuleConfig.lua` 和 `modules/`。
- `lua/autobattle_base.lua`，含 `AutoBattleEvent` 和简化模式策略。

因此，旧盘点中的“脚本缺失”仅适用于参考仓库，不能据此删除本轮脚本功能。脚本按游戏运行目录读取，外部资源目录本轮只读，不搬入其 Git 仓库、不代改用户模块配置。自定义策略 `lua/autobattle.lua` 由既有脚本按需加载，缺失处理沿脚本本身，不编造默认策略。

运行目录 `luaUI/README.md` 明确脚本使用 GBK。Lua 字符串继续兼容该字节合同：进入本线宽文本 UI 时使用已有 GBK→宽文本转换，返回脚本时使用宽文本→GBK转换；原协议字节不做重复编码。C++ 与本 SPEC 仍为 UTF-8，不把脚本一并改成 UTF-8。

## 3. 逐项迁移内容

| 编号 | 参考入口 | 迁移内容与本线承接 |
| --- | --- | --- |
| L01 | `src/hooks/lua.cpp`、`include/extLua.h` | 绑定 `BattleEntry`、`PlayerDataVal`、`InvItemData`、`PetData`、`SkillData`、`TechData`、`PetTechData`。保留脚本字段名、空槽、数组索引和布尔／整数语义；逐字段读取本线真实玩家、宠物、技能、战斗槽。 |
| L02 | `src/hooks/UI/luaUI.cpp`、`UIControls/` | `WinMgr` 创建／查找／聚焦／关闭，原生与 Lua 窗口引用，图片、动画、文字、PNG、输入框、滚动视图、MapEx、Canvas 及控件回调。保持窗口代数与控件编号校验，失效引用不悬挂到已释放对象。 |
| L03 | `src/hooks/UI/UIExt.cpp` | 将原生窗口与扩展窗口合并到层级／焦点／命中调度。覆盖创建、更新、绘制、关闭、拖动、滚轮和输入框失焦，保留回调期间开关窗口的延迟处理。 |
| L04 | `luaUI.cpp`、`luaIO.cpp`、`luaConst.cpp` | 场景、焦点、关闭、按键、聊天、收发包事件及注销；音频、消息、协议编码、文件／YAML、UI／自动战斗 Bridge。聊天回调返回整数1才阻止发送，错误恢复 Lua 栈并用 `m_log*` 报告。 |
| L05 | `src/hooks/UI/autoBattle.cpp` | 记录手动 `Rg` 动作、生成上一回合及当前战斗数据、调用 `AutoBattleEvent`、执行既有技能／物品／宠物命令、完整与简化面板。调用本线业务函数，不保留原地址函数指针。 |
| L06 | `src/hooks/UI/copilot.cpp` | 原全图／局部／未知区域规划、分段移动、卡住障碍记录与重规划、到达／换图停止；读取本线地图与移动状态。保留脚本既有 `GetAutoCopliotState`／`GetAutoCopliotRoute` 拼写兼容。 |
| L07 | `UIControls/MapExControl.cpp` | 扩展地图的文件／可见区同步、分块缓存、视口更新、标记点和鼠标地图坐标；纹理接本线已有表面／PNG绘制接口，资源由控件拥有和释放。 |
| L08 | `UIControls/CanvasControl.cpp` | 迁移 Lua 图元参数与控件行为，调用已有 `render::PrepareCanvasShape`、`render::DrawCanvasClipped`；保留图元快照、路径、描边和父子裁剪。 |

L08 不重复移植 R08 已有的路径解析和渲染。L07 当前尚无 MapEx 控件，不能把已有小地图绘制计为该项完成。E10 技能演绎状态机不属于本轮。

## 4. 必须落实的接线与寿命

### 4.1 Lua 状态

参考 `GetLuaState`／`NewLuaState` 使用 base、bit、table、string、math 库和注册回调。保留 UI 与自动战斗两个上下文及 Bridge 的字符串／数值传递，不用一个全局状态吞掉两者隔离。

在 `ClientInitialization.cpp` 的资源、音频、原生 UI 初始化完成后，显式创建状态、注册绑定并载入入口；取消依赖 Hook 初始化和静态注册顺序。关闭时先停止 Lua 事件／自动动作、关闭扩展窗口与释放 registry 引用，再关闭 Lua 状态；MapEx、动画、输入上下文的释放必须早于 renderer／资源关闭。部分初始化失败也遵循这一顺序，不在关闭后访问 Lua。

### 4.2 数据对象

本线玩家由 `world::detail::gPlayerRuntimeSlots` 拥有，物品已使用宽字符串，布局与参考 `PlayerDataVal` 不同；禁止强制转换或按原地址偏移直接复制。战斗表使用 `gBattleEntries59E980`／`gBattleAllEntries59E980`、`gBattleCurrentPosition5ABCA4` 和已命名战斗状态，读取 CryptInt 的真实值。

绑定优先复用当前 owner 的字段及语义，按需补命名或字段级访问，不造一套脱离网络更新的 Lua 数据副本。保留脚本观察时机，并处理战斗对象、窗口及控件释放后的引用。

### 4.3 窗口与输入

现有 `WindowRuntime_Update` 包含 GBK 转换失败处理、G07 页按钮优先命中、拖动物品和快捷键等业务；接入扩展调度时逐段保留，不直接用参考 `WindowUpdateHook` 覆盖。原生窗口仍由现有创建／释放链管理，Lua 窗口单独拥有，不能把扩展编号直接索引原生固定表。

`WindowProcedure.cpp` 已有 `WM_MOUSEWHEEL` 分支，直接将增量送入 Lua 输入调度；不调用 `SetWindowLongPtr` 再挂一个窗口过程。控件失焦时恢复原输入上下文，嵌套滚动与裁剪影响命中和绘制两条路径。

### 4.4 网络与聊天

本线 N01 接收／发送扩展处理器均只执行首个匹配项。Lua 与自动战斗不能简单重复注册同一头，否则会与既有 `BtWt`、`SpDt`、`INVCF` 或彼此争抢事件。实施时在现有明文收发入口增加同线程 Lua 事件分发，保持原扩展处理器及核心协议解析顺序；Lua 事件注销／脚本重载不能累积悬挂回调。

`Rg` 记录与 Lua 发包订阅分别消费实际正文，命令、字段、转义和单 LF 保持原格式。回调重入发送前必须保存被借用的当前报文，避免覆盖共享协议工作区。聊天接入 `SceneState.cpp` 的真实发送链，按参考拦截位置核对禁言计时等副作用，不只拦截某一个 UI 按钮。

### 4.5 文件、音频与时间

文件／YAML 沿参考 `data/luaUI` 目录、文件名规则、返回值与表结构转换，复用已有 yaml-cpp。Lua `LogToFile` 名称可为脚本兼容保留，日志实现统一走 `m_log*`，不额外维护 `autobattle.log`。音频接已迁 WAV 业务，计时接本线 64 位接口，不恢复 MIDI 或32位 tick。

## 5. 实施顺序与验证状态

1. 使用已替换的 x64 库，落实 CRT 合同并接入导入目标；实现 L01 状态和数据绑定。
2. 实现 L02／L03 窗口、控件及调度，再接 L04 事件、工具和 Bridge。
3. 接 L05 自动战斗和实际策略，再接 L06 寻路。
4. 完成 L07 地图控件与 L08 Canvas 绑定，逐一核对现有运行脚本使用的公开接口。

每步只提交该步业务、SPEC 和唯一进度；集中核对声明／定义／调用、x64类型、编码、回调重入与资源释放。不以已存在渲染底层或脚本文件代表 Lua 功能完成。

当前L01及运行库链接配置已写入，L02–L08继续实施，没有构建、测试、启动或停止客户端。后续编译／运行需按当时用户授权执行，运行验收覆盖脚本加载、窗口创建／关闭／重载、输入失焦、聊天阻止发送、协议事件与重连、战斗操作、寻路到达／换图，以及 MapEx／Canvas 裁剪；未取得证据前不标已通过。
