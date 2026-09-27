# cg2d 完整复刻独立进度

> 更新日期：2026-09-27，`RPLAN.32` 压缩后增量：graphic 资源/上传、动画装载/播放、CoordLoad 与类型化 64 位地址变换缓存。对应[总 SPEC](cg2d_full_reconstruction_plan.md)、[执行规则](../AGENTS.md)。
> 本文件是本线唯一执行进度；旧 M 系列与根 `plans/progess.md` 不参与本线完成度。
> RPLAN.32 压缩时核对会话 `01a0d16c-6460-7520-a994-993fa9977215`、当前源码和证据，只整理文档，不重新验收旧实现；后续实现另记增量。压缩前全文固定于 `a0831a64d01113b14edeed366f6bdefd3f9afdd3`，查阅方法见 §4。

## 1. 当前状态与阻断

**结论：局部函数与资源缓存已有较多实现，完整客户端仍未闭合。** 当前 CMake 只有 `cg2d_re_core/startup/render` 三个静态库目标，没有完整客户端链接目标。既有编译不证明未定义依赖已经解决；没有真实画面、播放或完整流程运行验收。全量分类与覆盖清单尚未收口，不能给出可靠的整体完成百分比。

当前执行限制：**不构建、不新增或执行测试、不运行客户端、不检查编译产物；仅本线程处理，不使用 subAgent 或独立 review。** 音频暂缓继续推进。cg3a 的地图、graphic、anime 部分按本日用户裁决作为可信参考。

| 功能链 | 当前已有内容 | 仍缺或待确认 | 证据 |
|---|---|---|---|
| 入口与主循环 | WinMain、配置/参数、窗口、MainLoop、World::Update 分发及多项退出/防护叶；主循环已有网络、清屏与 Flip 调用 | `InitializeClient_4576B0` 仅声明；初始化、状态目标和全部退出依赖未闭合 | [入口](../doc/cg2d_startup_flow.md)、[主循环](../doc/cg2d_main_loop.md)、[退出](../doc/cg2d_client_shutdown.md) |
| 地图文件与状态 | 三平面缓冲、`468570/4688A0/468C40/469290` 文件/校验函数、第四平面 `4693F0/46D9C0`、`469030/469870`、回复 `509100/5093F0` 已有本体；`467F30/468330` 已定义 | 地图绘制下游、路径父链及世界流程未闭合；`46B6F0` 的共享邻槽布局方案尚待确认，不再因单纯异常故障模拟停工 | [场景 §78–112](../doc/cg2d_scene_state.md)、[资源来源](../doc/cg2d_resource_sources.md) |
| Graphic 读取与分配 | `GraphicDecode/GraphicInfo/GraphicRead/Palette`；`4574E0` 四表与 `457670` 释放；`459960` 九槽资源加载、主表/双索引和文件基址已有本体；`522570` 读图并接 `5222D0` 恒空建链；DBF、独立调色板双池/颜色刷新已实现，释放已接退出 | InitializeClient/绘制父链与运行加载未验收 | [GraphicInit](../doc/cg2d_graphic_info.md)、[DX §5.10–5.18](../doc/cg2d_graphic_dx_cache.md) |
| DX 缓存与像素 | x64 DX 表、4096 节点及 reset/invalidate；`5231A0` CPU 建面；104 项动态及四项静态调色板池、领取/老化、`44E290`；`451310/450400` 清面/索引像素写入；独立调色板刷新、`5222D0/520B10` 建链上传；`44CD80/44D490` 双缓存使用完整 64 位 `TextureSurface*` 身份 | DrawAllQueued 的刷新计数重置、复用帧写入、变换缓存的父绘制/场景清理调用未闭合；不得用 AutoMap 代替非零独立调色板索引 | [DX §79](../doc/cg2d_graphic_dx_cache.md)、[x64 SPEC §10–14](../doc/cg2d_graphic_dx_cache_x64_spec.md) |
| SDL 画面与生命周期 | SdlPresenter、唯一 Back/Temp、像素格式发布 helper；`Flip_44C560` 已接主循环，`RendererDestroy_44C3C0` 已接退出链 | `Renderer::Init/Rebuild`、`DrawAllQueued_520DD0`、真实像素生产和模式 2/3/4 未闭合；presenter 创建未由初始化父链调用，不能称已有可运行画面 | [呈现器](../doc/cg2d_sdl_presenter.md)、[缓存](../doc/cg2d_graphic_dx_cache.md) |
| Anime | 查询、P1/P4 分配、清理与主索引池；`AnimeLoadAction/CoordInit` 共用 GraphicInit 基址；`FrameMove@517880/PlayFrame@517DA0` 完整本体已接真实装载/图形加载，现有角色预览等四处调用已解析；`CoordLoad@45BF60` 文件/索引生产本体已有 | `AnimeInit@45BB20`、`CoordLoadInit@45C040` 仅取证，后者原大小写比较边界尚未落实；初始化、战斗 caller 和画面仍缺；`9A4A54` 正常资源身份/归属仍须落实。越界/溢出不另设专项恢复门禁 | [动画 §1–11](../doc/cg2d_animation_runtime.md)、[x64 SPEC §10](../doc/cg2d_graphic_dx_cache_x64_spec.md) |
| 世界效果与 Yobi | 500 项效果链、kind 1/2/4 更新、`46E4A0` 调度；观察链四叶、五槽生产/标死、`440FE0/51D5B0`；玩家 `FDA880/FF02B4` 唯一槽与 FrameMove/PlayFrame 本体已建立 | `46F1D0` 仅有描述符和声明；绘制、其他观察回调、玩家发布/清理/整块重置及 world 对象表仍缺 | [场景 §20、114–116](../doc/cg2d_scene_state.md)、[Yobi](../doc/cg2d_yobi_runtime.md) |
| 登录、角色、UI | 多个状态本体、请求轮询、角色槽/预览、图文入队、输入/IME、原字节提示；`455C90` 断线展示和 `462880` 创建入口已有本体 | `532D80` 的 71 项 UI owner、`534290` 的 profile 双槽/转换、`RenderChara_521F20`、真实图文消费等仍缺；局部文字布局通过不代表完整显示 | [登录](../doc/cg2d_login_scene.md)、[控制器](../doc/cg2d_login_state_controller.md)、[角色](../doc/cg2d_character_select_scene.md) |
| 网络 | workspace/队列 owner、`593850` 连续输入存储、组包/发送叶、保活、`507DF0` 收发循环本体及 `534EF0` 平台适配器已落位 | `NetworkDispatch_512E40` 仅声明，回复分发和完整收发未闭合；用户排除的 `403C50/403E50` 加密链不再实施，但最终排除接线仍须明确 | [网络](../doc/cg2d_network_runtime.md) |
| 输入、存档、时间 | 保存槽/codec、多组恢复/写回、`520160` 父保存、显示设置；LocalTime/TZ/DST/Convert、四字符串与四项获批 std 入口已有实现 | `507140` 完整命中/对象身份、共享玩家与 UI 状态、邮件/相册等 owner、跨模块初始化和网络校时链未闭合 | [命中](../doc/cg2d_sub_507140.md)、[存档](../doc/cg2d_login_saved_state_storage.md)、[时间](../doc/cg2d_game_time.md) |
| 音频（暂缓） | SDL 设备/样本/64 通道、`524230/52C450/52CB50/52C680`、BGM `52BE60`、`52B8E0` 工作线程通知、`52CCA0/52B940/52B630` 已有实现 | 完整客户端接线、实际解码/发声、播放起停及通知运行未验收；暂缓不等于上述入口未实现 | [音频：SDL BGM 播放及后续章节](../doc/cg2d_audio_update.md) |

### 1.1 本轮纠正的过时状态

- `53AF90` 已于 `a0831a6` 实现；历史内存探针耗尽 8358 字节并产出 10808 字节，首项数 1350、步长 8。`522900` 后续已有独立源码与静态证据，不能用历史探针代替正式 C++ 运行验收。
- `44E290/450400/522570/GraphicAlloc/GraphicInit/BuildSurfaceCache/Flip/Renderer::Destroy` 已有本体，不再列为“完全未实现”；`520B10` 上传后续也已落位，上游创建和最终绘制仍缺。
- 地图四个短头相关函数已有后续实现；`520160`、保活/命令构造、网络循环和 BGM 播放等早期“待实现”描述不得继续当作当前阻塞。
- DX A 案表承载、x64 节点和 CPU 建面已有局部选择与实现；§13 独立调色板/DBF 存储已在 `GraphicRead` 落位。用户明确 `5222D0` 第二参数恒空、只保留未使用参数、不实现非空分支；恒空建链及 `520B10` 上传本体后续已实现，父绘制尚未接入。
- 原会话末次成功路由为 `ida-17524`；旧 `ida-17524/cg2d.exe` 失败不等于数据库关闭。压缩日志时未调用 IDA；后续 DBF 小步已确认现有会话并只读取证。
- 目标会话收到“不构建”前已完成一次 Debug core 编译，此后停止；后续实现仅作静态检查，不构建。

### 1.2 下一步进入条件

1. 优先沿已展开的地图/graphic/anime 依赖补齐真实父链，不回到孤立叶函数数量或空窗口目标；先定位现有实现，避免误报缺失或重复定义。
2. 独立调色板按 [DX x64 SPEC §13](../doc/cg2d_graphic_dx_cache_x64_spec.md) 的已选资源归属推进；`5222D0` 保留未使用第二参数，仅实现原空参数路径，不再追查非空类型/生产者，不用空实现补齐链接。
3. AnimeInit 优先对照 cg3a 可信实现及已有 §7 证据，区分正常归属/布局缺口与不专项处理的异常；不擅自采用历史提出的扩容、补零或拒绝方案。
4. 真正需新设计或存在具体证据冲突的项先提交 SPEC；无重大冲突的已过内容直接继承。未决项仅阻断相关范围，不放弃完整客户端最终验收。
5. surface 变换缓存的完整 64 位地址和 `T*` 优先已由用户确认，双树与变换/清理本体已落位，按 [DX x64 SPEC §14](../doc/cg2d_graphic_dx_cache_x64_spec.md) 继续接父绘制。`dataBits` 同时承载文本 token 等用途，正常共享身份/布局仍需区分，不把整列无差别扩成指针；已证地址用途按新裁决处理。

上述是当前重点，不是全量债务清单。未列入摘要的任务不视为完成或排除；具体字段、失败残留、未接 caller 和局部验证前提留在对应证据与固定历史。

## 2. 共用裁决与验收口径

详细执行规则以 [AGENTS.md](../AGENTS.md) 为准，以下保留关键裁决索引。

| 裁决 | 当前适用范围 |
|---|---|
| RPLAN.1–7 | 本线独立、原 x86/新 x64、功能先于启动；旧 Debug 许可已被 09-27“不构建”覆盖 |
| RPLAN.8、10–11 | STONAGE 只读 GBK，仅作架构/命名线索；MainLoop → 基本状态 loop → 登录 scene |
| RPLAN.12–15 | 屏幕键盘排除；SDL3/32 位色；DX7 原生 API 可等价替换，上层副作用不豁免；保留索引图/调色板 |
| RPLAN.16 | 原端兼容数据保留原编码字节，现代边界集中转换；源码/文档 UTF-8；具体编码不猜测 |
| RPLAN.18、22 | 已过内容无重大冲突不重审，状态检查增量化；不再以 `.i64`/EXE 哈希为门禁，废弃代码图谱 |
| RPLAN.24 | 正常逻辑按 cg2d，可有据等价替代；越界、数学溢出等异常不专项处理、不作阻塞、不冒称故障等价；不擅加保护或输入限制，覆盖冲突的 RPLAN.20 旧限制 |
| RPLAN.26 | Malloc/Free/Atol/GetEnv 采用对应 std；不扩大到其他 CRT 或抹去正常失败、借用身份和调用顺序 |
| RPLAN.29 | 全部工作仅本线程，不用 subAgent/独立 review；覆盖旧模型与角色分工，不回写历史复核结论 |
| 09-25 IDA | 仅现有已打开会话只读 MCP；禁止重开/重载/重启/保存/搬动，不用 IDALib 替代；证据足够的项可继续 |
| 09-25 地图短头 | 仅原未写栈字节按已批局部方案补零，其决定的值/分支不计逐次等价；已定义副作用、完整头及正文短读仍保留，不能推广至 anime |
| 09-25 至 09-27 SDL | 自动 driver、统一 CPU Back/Temp、CPU 节点面与 SDL 最终提交已选；音频原生替换已选但当前暂缓；上层生命周期/内容仍须接通 |
| 09-27 不构建 | Debug/Release 均停，不新增或运行测试/客户端，不检查产物；静态可行性、完整链接和运行结论分别报告 |
| RPLAN.32 可信参考 | cg3a 地图、graphic、anime 为可信参考，不降为纯线索；不等于本线完成，不代批未决设计，不覆盖具体基线冲突 |
| 09-27 DX §13 | 本线程受托决定资源归属为 `cg::resource::GraphicRead`；用户确认第二参数恒空，保留未使用参数、删除/不实现非空分支；旧非空门禁不再适用。建链/上传后续已实现，父流程验收仍缺 |
| 09-27 64 位地址与 T* | 用户确认按地址缓存的字段使用 64 位，已知对象类型时优先原生 `T*` 非拥有引用；仅在数值排序边界转 `uintptr_t`。DX §14 不再待批；非地址字段、混用字段归属和生命周期不自动获豁免 |

范围排除仍为：8/16/24 位色渲染、屏幕键盘、`newest` 五函数检查链、用户排除的网络加密链。排除项保留证据，不计已实现；必要排除接线另行完成。音频暂缓不在此列表。

既有窄适配（队列负计数、特定表索引、网络分配失败、x87 数值承接等）按原证据范围保留；不因日志压缩修改代码、推广适配或重写历史通过域。

## 3. 阶段成果索引

以下编号用于定位，不是范围内全部通过的声明。“局部静态”继承当时限定范围；旧独立复核保留为历史，09-24 后检查由原执行线程完成。本次未复核全部函数。

| 阶段/编号 | 聚合成果与边界 | 证据入口 |
|---|---|---|
| R0.1.1、R0.1.2.1、R0.6.1、R1.1.1、R3.1.1 | IDA 身份、九槽资源来源、连续资源名、3680 函数/218 导入槽/3061 补集快照有静态记录；不是覆盖分母冻结或全部资源/环境通过 | [基线](../doc/cg2d_reconstruction_baseline.md)、[台账](../doc/cg2d_reconstruction_inventory.md)、[来源](../doc/cg2d_resource_sources.md) |
| R3.2.1–4、R4.2.1.C | 解码/查询/读取/调色板及 SDL 最终帧适配器局部实现；当前接线增量见 §1 | [资源实现](../doc/cg2d_resource_implementation.md)、[解码](../doc/cg2d_graphic_decode.md)、[呈现器](../doc/cg2d_sdl_presenter.md) |
| R2.1.1–4、R2.2、R6.1.1a–b | 启动、主循环、退出体、基本状态分发局部静态落位；完整初始化与全部目标未闭合 | [入口](../doc/cg2d_startup_flow.md)、[主循环](../doc/cg2d_main_loop.md)、[场景](../doc/cg2d_scene_state.md) |
| R6.1.2.1–19 | 登录/请求/场景本体和 `.8–19` 原字节提示对齐有局部静态结论；连接提示宽 102、列 3、X=224、宽 192 仅为原限定布局结果，不是最终字形验收 | [登录](../doc/cg2d_login_scene.md)、[登录状态](../doc/cg2d_login_state_controller.md)、[编码 SPEC](../doc/cg2d_text_encoding_spec.md) |
| R6.1.3.1–18 | 角色列表/选择/请求/modal/命中、DrawGraphic 与队列；动画和画面消费者仍缺 | [角色](../doc/cg2d_character_select_scene.md)、[图形封装](../doc/cg2d_legacy_graphic_wrapper.md) |
| R6.1.3.19–95 | CryptInt、鼠标/追踪、窗口、IME、输入/字体/文本队列、能力 3/4 场景、保存槽及跨输入恢复等局部成果；子项状态和遗留不合并为整链通过 | [输入](../doc/cg2d_input_context.md)、[窗口](../doc/cg2d_window_procedure.md)、[存档](../doc/cg2d_login_saved_state_storage.md)、[Yobi](../doc/cg2d_yobi_runtime.md) |
| R6.1.3.96–99、115–119 | 屏幕键盘历史叶与 newest 原端证据；后续按用户范围排除，不转记完成 | [键盘](../doc/cg2d_external_keyboard_wrapper.md)、[连接](../doc/cg2d_login_state_controller.md) |
| R6.1.3.100–146 | 显示、资源、网络 owner/分配、存档、队列、波纹/Back/Temp 等局部成果，`.108` 生命周期及消费者等缺口由当前 §1 收准 | [显示](../doc/cg2d_display_settings.md)、[网络](../doc/cg2d_network_runtime.md)、[波纹](../doc/cg2d_yobi_runtime.md) |
| R6.1.3.147–150、158–159、166；09-24 至 09-26 增量 | 命令/base62/文本变换/发送、连续文本 owner、黑名单查询和 NetworkLoop 本体已有后续实现；dispatcher、排除链接线及跨 UI 生命周期未闭合 | [网络](../doc/cg2d_network_runtime.md)、[黑名单](../doc/cg2d_blacklist_state.md) |
| R6.1.3.151–157、160–160.9 | Yobi 生成、对象身份、命中与 DX 证据/方案；查询叶、模式提示等已有后续实现，`507140/507090`、对象身份及 DX 父消费者不能统一记为完成 | [命中](../doc/cg2d_sub_507140.md)、[Yobi](../doc/cg2d_yobi_runtime.md)、[DX](../doc/cg2d_graphic_dx_cache.md) |
| R6.1.3.161–165、168–176 | RAND、日志、LocalTime/TZ/DST/Convert 和指定 std 入口有本体；旧 GetEnv 方案停止，不再阻塞；完整时间/网络校时未验收 | [时间](../doc/cg2d_game_time.md)、[RAND](../doc/cg2d_random_range.md)、[音频](../doc/cg2d_audio_update.md) |
| R6.1.3.167、177–188、179.1–7 | 黑名单删除、输入/日志和场景初始化、地图参数/cut/aut、独立重置叶；`467F30/468330/46E2A0` 后续已部分承接，不能沿用早期“均未编码” | [场景](../doc/cg2d_scene_state.md)、[黑名单](../doc/cg2d_blacklist_state.md) |
| R6.1.3.180–181、189–193 等未闭合项 | 玩家/对象表、大缓冲、邮件、相册及其他共享布局/接口仍按各项证据处理；摘要未列出不表示放行或删除债务 | [场景](../doc/cg2d_scene_state.md)、[Yobi](../doc/cg2d_yobi_runtime.md)、[固定旧全文](#4-规划沿革与历史追溯) |
| 09-24 至 09-26 主循环/地图/设置增量 | 防护链、方向角/队列、地图四平面、文件/回复、显示与存档写回、场景效果/观察链局部落位；父流程仍按 §1 收口 | [主循环](../doc/cg2d_main_loop.md)、[场景](../doc/cg2d_scene_state.md)、[当前记录](../doc/cg2d_current_saved_record.md) |
| 09-26 音频增量 | SDL 音效、BGM、通知、退出及共享表等已有源码与历史 Debug 记录；不代表实际播放或完整链接，当前暂缓 | [音频](../doc/cg2d_audio_update.md) |
| 09-27 anime/graphic 增量 | 分配/查询/清理/索引池，DX 表/节点/调色板/像素叶，Flip/Destroy/读图/DBF 解压已落位；未完成整链 | [动画](../doc/cg2d_animation_runtime.md)、[DX](../doc/cg2d_graphic_dx_cache.md)、[呈现器](../doc/cg2d_sdl_presenter.md) |

### 3.1 最近关键提交

| 提交 | 本次据以收准的结果 | 验证限制 |
|---|---|---|
| `bdd1416`、`76856a5`、`d413f2d` | 四表分配释放、动画双表/文件清理、主索引池链 | 历史 Debug 静态库编译，不含初始化生产或运行 |
| `d64efbb` | AnimeInit 全函数装载合同和待决点 | 仅文档取证，异常门禁按 RPLAN.24 收准 |
| `92931bd`、`8a04047`、`7d83578` | 节点 reset、DX 引用失效、CPU BuildSurfaceCache | 局部实现/历史 Debug，不含全部生命周期 |
| `51e4610`、`533c9ba`、`7da5477` | Flip、Destroy 父入口、像素格式槽 | 创建父入口及画面未闭合 |
| `b64b52b`、`7cc21d0`、`7bc393d` | 调色板池、领取/复用、32 位构建 | 父上传和完整绘制未闭合 |
| `c2bcef5`、`c27460c`、`4c97e94`、`cd90d83`、`73a62f3` | 清面、索引写像素、当前解码图五槽、LoadGraphic 和不透明参数声明 | 不能视为 `520B10/5222D0` 已实现 |
| `38dcb4c` | DX x64 §13 原始提案 | 后续已受托选择资源归属，用户裁决第二参数恒空；方案已定不计实现 |
| `782859b`、`b38afad`、`a0831a6` | SDL 接线核对、DBF 字节补证及 `53AF90` 解压叶 | 样本探针非正式 C++ 运行；收到“不构建”后不再编译 |

## 4. 规划沿革与历史追溯

RPLAN.17 的第一次压缩前记录固定于 `3808ef2d309e1f407117bfe47390f36e7b8a4786`；本次 RPLAN.32 前的 952 行记录固定于 `a0831a64d01113b14edeed366f6bdefd3f9afdd3`。两者均保留全部原文，不另复制日志。旧模型、代理姓名、复核轮次和反复格式检查从当前摘要移除，不抹去历史事实。

```powershell
git -c i18n.logOutputEncoding=utf-8 show a0831a64d01113b14edeed366f6bdefd3f9afdd3:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c i18n.logOutputEncoding=utf-8 show 3808ef2d309e1f407117bfe47390f36e7b8a4786:cg2d_re/plan/cg2d_full_reconstruction_progress.md
git -c i18n.logOutputEncoding=utf-8 log --oneline -- cg2d_re
```

历史“未实现/待批”是当时快照；后续证据、明确裁决和当前源码可以更新该状态，但不能由文档压缩创造新的实现或验收。RPLAN.19/21/23/25/27/28 的代理分工已由 RPLAN.29 覆盖；RPLAN.30/31 的防护状态与重复定义纠偏保留在上述历史。

### 本次简记

- **2026-09-27，RPLAN.32：** 分析目标会话与当前工作树，按功能链将日志从 952 行压缩为 119 行并保留固定追溯；汇总规则，将“不构建”、RPLAN.24、可信参考和已实现/待批界限同步至计划、DX 与动画文档。五份文档严格 UTF-8 无 BOM/LF、相对文件链接、限定差异及两次存档对象检查通过。只改文档，不改生产代码、根进度或既有未提交文件；未调用 IDA、未构建/测试/运行，未使用 subAgent，不将本次文档检查算作旧实现重新验收。
- **2026-09-27，DX §13 裁决：** 按用户委托，独立调色板队列、计数、索引、记录与 DBF 索引统一归既有 `cg::resource::GraphicRead`，render 仅窄访问。用户明确 `5222D0` 第二参数恒空，保留未使用 `void*`，非空分支不进入新端实现；当前本体尚无定义，无现存分支可删，仅修改声明注释和未使用参数标记。SPEC、总计划及相关证据已同步，不重写原指令事实；资源存储、DBF 父链、节点构建和上传仍待实现。六文件 UTF-8 无 BOM/LF、相对文件链接、限定差异及声明/调用点静态检查通过；不构建/测试/运行，不使用 subAgent，不改既有未提交文件。
- **2026-09-27，`522900/522B30/522C50`：** 按现有 IDA 完整指令与 cg3a 可信静态表，在 `GraphicRead` 落位跨次 DBF 索引/解压块、1752 项队列和独立调色板双池；保留 signed 键覆盖、第二 W32 数值入队、短读/magic/解压返回及发布释放次序，退出原 `457A45` 已接线。Win32 API 留在平台窄边界。60 段表与参考逐项静态一致；十文件严格 UTF-8 无 BOM/LF、Markdown 文件链接、限定差异及声明/定义/调用点检查通过。不构建/测试/运行，不使用 subAgent；初始化/重建父入口、`522CB0/523140/5222D0/520B10` 与完整链接/画面仍缺，详见 [DX §5.15](../doc/cg2d_graphic_dx_cache.md)。
- **2026-09-27，`522CB0/523140`：** 继 `207245c` 后实现独立 BGR 读取、原色/色调双表和 S16 版本刷新；保留短表三字节边界补零、局部长度不覆盖共享长度、失败残留、loaded 只读和 0/1/2 返回。绘制记录包装只改原 +22。七文件限定差异、严格 UTF-8 无 BOM/LF、文件链接与接口静态检查通过；不构建/测试/运行，不使用 subAgent。`DrawAllQueued/520B10` 与初始化父调用仍缺，见 [DX §5.16](../doc/cg2d_graphic_dx_cache.md)。
- **2026-09-27，`5222D0/520B10`：** 继 `17c9d39` 后实现恒空参数节点建链及真实上传，沿用既有 x64 节点、调色板池和 CPU 写像素叶；独立色表通过资源窄借用读取。保留扫描回绕、部分链、旧图解绑、调试原字节、限流跳过返回 1、读图失败写序和 W16/S32 比较。十二文件限定差异、声明/定义/调用、严格 UTF-8 无 BOM/LF 与文件链接检查通过；不构建/测试/运行，不使用 subAgent。父绘制仍须接每帧计数清零/复用帧写入及三个上传 caller，初始化/重建、anime 父链和完整 SPEC 未完成，见 [DX §5.17–5.18](../doc/cg2d_graphic_dx_cache.md)。
- **2026-09-27，DX §14 提案：** `f079368` 已提交上述建链/上传。随后定位 `DrawAllQueued@[520DD0,521832)` 的真实直接调用，确认完整父绘制仍依赖 `44DA90/44D520` 的变换缓存以及 Yobi 身份等未决项，不接半段父函数。继承既有 §59/§74 证据，提出仅将 surface 地址键拓宽为 `uintptr_t`、接受原生地址排序导致不同缓存面释放顺序可能变化的窄方案，待用户确认；未实施，不自动批准对象身份。两份文档限定差异、严格 UTF-8 无 BOM/LF 与文件链接检查通过，不构建/测试/运行。
- **2026-09-27，`GraphicInit@459960`：** §14 未获回复，转推进不依赖其裁决的资源父入口。在 `GraphicInfo` 实现完整九槽数据/Info 生产、主表字段写入、mapNo 末写索引及首见记录树、共享文件基址；保留必需槽错误清理差异、容量失败返回 3、索引分配失败 -1、EOF 后仍消费记录及跨文件累计次数。只将有据 STL 操作等价承接，不搬入 cg3a 的额外拒绝或最大 seqNo 基址。六文件限定差异、声明/定义/引用、严格 UTF-8 无 BOM/LF 与文件链接静态检查通过；不构建/测试/运行，不使用 subAgent。`InitializeClient`、Anime/Coordinate 消费者与整体 SPEC 仍缺，详见 [GraphicInit 证据](../doc/cg2d_graphic_info.md)。
- **2026-09-27，`AnimeLoadAction/CoordInit`：** `7fc6e46` 提交 GraphicInit 后，沿同一文件基址实现 action/帧生产与坐标加载，接既有领取/复用/清理链。细化既有 action 24B 前缀而不变 32B x64 布局，保留旧 28/12/8 字节计数、帧尾未写字节、2000B 共用栈区、原门禁顺序、短读和失败残留。六文件限定差异、接口/既有清理引用、严格 UTF-8 无 BOM/LF 与文件链接静态检查通过；不构建/测试/运行，不使用 subAgent。初始化文件生产、FrameMove/PlayFrame、§14 和整体 SPEC 仍未完成，见 [动画 §8–9](../doc/cg2d_animation_runtime.md)。
- **2026-09-27，`FrameMove/PlayFrame`：** 继 `f7071c9`，实现两播放本体并接现有资源/图形/音效入口；新增状态归既有 opaque Yobi 对象，DX 宽高仅窄读。保留静图范围和速率读取差异、隐藏严格为 1、U16 动作匹配、无符号速率运算、末帧两处均严格 ==1、事件/坐标/翻转写序。现有四处预览/场景调用解析到真实 FrameMove；音频实现未改。八文件限定差异、声明/定义/调用点、严格 UTF-8 无 BOM/LF 与文件链接静态检查通过；不构建/测试/运行，不使用 subAgent。文件初始化、战斗调用、对象身份和完整绘制仍缺，§14 未实施，详见 [动画 §10](../doc/cg2d_animation_runtime.md)。
- **2026-09-27，`CoordLoad@45BF60`：** 继 `704c23e`，在既有动画模块实现坐标数据文件发布及 Info 索引读取；保留版本不符泄留 Info、失败时数据槽残留、EOF 前检查、短读继续消费及 offset/count 写序。版本初值保留实参地址低 W8，不猜零，不宣称新旧地址依赖的零字节短读分支一致。父 `CoordLoadInit` 的大小写比较尚未落实，未接半段父函数；AnimeInit、§14 和整体 SPEC 仍未完成。仅静态检查，不构建/测试/运行、不用 subAgent，见 [动画 §11](../doc/cg2d_animation_runtime.md)。
- **2026-09-27，64 位类型化地址键与 `44CD80/44D490`：** `bf910db` 已提交 CoordLoad。用户随后批准地址缓存改 64 位并明确 `T*` 优先；在既有渲染模块落位双缓存、32 位像素变换与按场景清理。键保存 `TextureSurface*`，仅地址不等时转 `uintptr_t` 排序；非地址分量保持 S32/S16，矩形按 left/right/top/bottom 比较。保留原查找/发布时点、分配失败回退、特殊取样和逐项释放，不新增按帧清空或句柄表。父绘制与场景清理 caller 仍缺；类型规则和 SPEC 同步，未改根进度或既有用户改动。八文件限定差异、接口、UTF-8 无 BOM/LF 与文档文件链接静态检查通过；不构建/测试/运行、不用 subAgent，见 [DX §79](../doc/cg2d_graphic_dx_cache.md)。
