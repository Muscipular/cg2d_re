# cg2d SPEC 差异与收尾任务表

> 核对日期：2026-10-04。依据当前工作区的总 SPEC、五份独立 SPEC、局部合同、语义机器清单及定点源码；本轮没有新增 IDA 取证。
> 2026-10-04后续同步：总SPEC及五份独立SPEC已压缩为当前有效契约；原全文保存在Git命名快照 `cg2d-re-spec-history-20261004-before-closure-compression`，本表任务号、状态和验收边界不变。
> 配套[追踪验收体系](../doc/cg2d_traceability_acceptance_spec.md)及[度量看板](cg2d_measurement_dashboard.md)：本表60任务保持，功能条目和原函数分母分开；关闭任务须在验收登记簿附当前证据。
> 本表是收尾任务索引与静态状态快照；[独立进度](cg2d_full_reconstruction_progress.md)仍是唯一执行进度。后续完成某项时，在独立进度记录任务号、证据、实际验证级别，并同步本表对应行。
> **整体 SPEC 未完成。** 最新[具名决议](../doc/cg2d_closure_decisions_spec.md)已同步本表：8适配已定、原格式明文接线已定、受限CRT宿主承接已定；实施状态见各任务行，完整互通未验，音频仍暂停，未以数量推算完成率。

## 1. 核对口径与主要结论

1. 同一事项优先读局部合同和进度末尾的后续增量，再与当前源码/机器字段核对；旧摘要中的“缺失”不能覆盖后来已落位的本体或存储。
2. 区分源码本体、真实父调用、共享数据生产、初始化/退出、完整流程验证。声明存在、静态表有地址或对象初值为零，均不能替代后续层次。
3. 18 个 pending 只是已显式登记的入口：**8 项适配源码均已对齐、2 项原格式明文帧源码工作项已闭但完整互通待验、1 项原事实待证、7 项音频暂停**。堆模式 3、CRT 启动、异常传播、旧 UI 数据和 R1/R7/R8 等另有债务。
4. “已批准替换”“范围排除”“暂停”分开登记。取消网络加密不等于取消网络；暂停音频不等于总目标不含音频；不允许运行不等于免除运行验收。
5. 本轮只读取源文件/文档并做静态一致性检查；不构建、不检查编译产物、不新增或运行测试、不执行客户端/算法样例、不计算文件摘要，不使用子代理。

| 状态 | 本表含义 | 可以如何推进 |
|---|---|---|
| 局部静态已核 | 指定本体、数据或调用已有证据；不代表整链完成 | 保留既有结果，只补剩余依赖 |
| 待取证 | 原值、容量、调用关系或可达性尚无充分事实 | 沿现有证据确定缺口；需新事实时仅使用获准的原 IDA 会话 |
| 待裁决 | 已存在明确待批的行为差异或范围变化 | 保持原提案边界，取得针对性批准后实施 |
| 待实现/接线 | 已知契约尚缺本体、真实调用或数据生产 | 先满足对应证据及设计前提，再按单项推进 |
| 暂停 | 用户要求暂缓的音频工作 | 等用户恢复；不计排除或已完成 |
| 待验收 | 完整流程、运行或适用环境验证未做 | 保留门禁，等待对应执行限制解除 |
| 范围排除 | 既有明确授权不实施的范围 | 核排除边界和遗留接线，不补假实现 |

## 2. 数量基线：哪些能数，哪些不能数

| 指标 | 当前值 | 来源与使用限制 |
|---|---|---|
| 原函数元数据 | 3680 个函数条目；218 个导入、13 个导入模块 | [原库存](../doc/cg2d_ida_inventory.json)的快照统计，不是待实现函数数 |
| 未归属执行区间 | 原快照 3061 段、46483 字节 | 同上；后续已有分类，不能把原快照全部当成今天未分析项 |
| 显式语义记录 | **1144** | [机器清单](../doc/cg2d_semantic_function_mappings.json)实数；含实现、关联、宿主承接等不同等级 |
| 显式未决入口 | **18** | 同清单 pending_records；不是总剩余任务数 |
| 独立分类引用 | **4** | 屏幕键盘、网络加解密、低色深、函数边界；newest 另有既定排除 |
| 现代 UI 直接覆盖 | 69 组去重元素、133 个主回调直接缺口 0/0 | [窗口合同](../doc/cg2d_ui_window_runtime.md)；不包含旧表 99 条 handler 或全部生产者 |
| 网络直接分发 | 108 路已有；直接回复缺口 1（murD） | [分发合同](../doc/cg2d_network_dispatch.md)；收发两帧父源码/局部接线已闭，完整互通仍待；52C3D0是另一音频依赖 |
| 格式调用 | 1000 真实 + 13 排除 + 6 暂停 + 3 原生替换 + 1 待事实/实施 = 1023 | [stdio 合同](../doc/cg2d_stdio_contract.md)；真实点为 999 直接 + 1 唯一原字节补证 |
| 字符串长度 | 19 真实 + 4 已批宿主承接 + 1 待 = 24 | [字符串合同](../doc/cg2d_legacy_string_compare.md)；内部共享尾不增入口数 |
| 比较接线 | strncmp 12 原点/10 源表达式；strcmp 2/2 原点 | 同上及最新进度；完整异常/聊天/网络/堆父链未验 |
| 工程目标 | core/startup/render 三个 STATIC 目标；没有 add_executable | [CMake 源码](../CMakeLists.txt)；未执行配置或构建 |

1144 不能除以 3680 得到完成率：两者覆盖口径不同，语义记录也并非全部函数已实现。现代 UI 的 0/0、网络的 1、格式的 23 余点相互不能相加为“最后几个缺口”。

## 3. SPEC 与当前实现的具体差异

| SPEC / 条款 | 要求或批准范围 | 当前已具备 | 仍差什么 | 收尾任务 |
|---|---|---|---|---|
| [总 SPEC §2、§7、§10](cg2d_full_reconstruction_plan.md) | 函数、布局、数据、分支、调用时序和真实流程逐项闭环 | 本体与局部证据持续增加；元数据与显式映射可追溯 | 全量分类、传递依赖、适用原新对照未闭合 | R01–R05、V01–V06 |
| [DX x64 SPEC §10–16](../doc/cg2d_graphic_dx_cache_x64_spec.md) | 原生非拥有指针、32B 图项、48B 队列、共享 owner；SDL/CPU 替换保留上层契约 | 表/节点、双变换缓存、RenderChara、DrawAllQueued、507140 指定调用及初始化探测已有 | 完整清理、两树原 CRT 生命周期、模式 2/3/4 与像素对照未闭合 | C09、G01–G05 |
| [文本 SPEC §2–3、§5–7](../doc/cg2d_text_encoding_spec.md) | 原字节权威、容量/NUL/索引和布局不变；沿D05保留现有输入/显示边界 | 连接提示原 12 字节已对齐；GDI 原字节消费及队列 case 4 已接 | 各来源编码、输入/IME 索引和既存额外门禁、字体/字形验收仍未闭合 | T01–T03、V04 |
| [创建 gate SPEC](../doc/cg2d_creation_gate_adaptation_spec.md) | 固定 entryGate=1 已按本轮委托裁决 | 显式参数完整本体已存在 | WorldUpdate已显式传1，源码接线差异收口；创建数据和真实流程仍待 | P08 |
| [网络排除接线 SPEC](../doc/cg2d_network_exclusion_wiring_spec.md) | 只排除两加解密叶；保留帧、队列、发送长度及状态副作用 | 上层组包/分发与两完整非加密帧父、真实接线已有 | 保留原帧/正文/状态，仅旁路加解密传明文；全部业务/生命周期与互通待验，不自定义分包 | P09、P10、U10 |
| [CRT 宿主提案](../doc/cg2d_crt_host_migration_spec.md) | 受限宿主承接已获本轮具名裁决；私有库差异单列 | 已批准的窄替代继续有效；大量旧 CRT 本体已落位 | 按两个子SPEC落实真实提供者/分配域/应用语义，既有兼容层和真实流程验收保留 | C01–C12 |
| [启动局部 SPEC](../doc/cg2d_startup_flow.md) | 原启动顺序、C/C++ 表、退出登记、唯一构析与共享对象 | 父入口/C5/浮点槽、多个 CRT provider、七共享槽和 14 标准流存储已有 | C++90 表、登记数据、SI01本组以外活动构析与完整退出 | C01–C04、V02 |
| [stdio / 内存局部 SPEC](../doc/cg2d_stdio_contract.md)、[堆合同](../doc/cg2d_crt_memory_contract.md) | 原 FILE/描述符和分配域、正常失败、副作用及所有权保留 | 文件核心、流对象方法表、模式 2 和原堆外层已有 | 剩余调用点、codecvt 生产、模式3整域宿主迁移、应用分配域逐项处理 | C05、C08、C10–C12 |
| [世界/UI/战斗局部合同](../doc/cg2d_ui_window_runtime.md)、[战斗合同](../doc/ui_windows/shared_battle.md) | 真实路由及生产—消费—清理完整对应 | 多数页面本体、预览工厂、Yobi 生命周期和战斗分发已有 | 部分页父、旧表路由、目录/名单/版本数据、攻击生产与载荷清理 | P01–P08、U01–U11 |
| [音频替换合同](../doc/cg2d_audio_update.md) | SDL 替换已批，上层状态/时序和播放验收仍保留 | 有局部播放、通知及退出实现 | 七入口与完整音频仍暂停，不从范围移除 | P11–P17、V05 |

### 3.1 已确认的文档滞后与防止重复施工

| 旧摘要或表述 | 当前证据 | 本轮处理与后续口径 |
|---|---|---|
| 总览“语义 1110” | 后续 1110→1113→1128→1136；机器 records=1136 | 更新当前总览；历史小步原计数保留 |
| “OS 四槽/三种子初值与定义缺失” | LegacyRuntimeExit、LegacyFileBuffer、LegacyStreamBuffer 已有 7 个真实定义；PE 载入证据已登记 | 只保留运行时生产、启动及 owner 门禁，不重做零值取证 |
| “14 个标准流全局缺定义” | LegacyIos.cpp 已有六 filebuf、两输入流、六输出流的被动聚合存储 | 四个 Init、登记容量/别名、启动/退出仍待；不把被动存储当构造完成 |
| “七项旧 UI/版本数据全缺定义” | LegacyUiRecord.cpp 已补 CC3E64/C8AF84/D30134/CF46D0 四 S32 | 缺定义收窄为 CA0478、CB1D1C、9A5C10；四标量运行时写者仍待 |
| DX 速查仍称命中/DrawAllQueued/初始化探测缺失 | DrawAllQueued.cpp、GraphicHitTest.cpp、RendererResources.cpp 及后续合同已有本体/指定接线 | 更新 DX 当前速查，保留全场景清理/画面门禁 |
| “异常元数据均未生产”或只提异常准备 | 已有六 TypeDescriptor、六 CatchableType、五数组/13 引用，PMD/匹配/范围选择等叶已补 | 5402AB/5407BC/54B123私有职责按D02由宿主承接；真实caller/异常映射及SDK行为仍待，不手造ThrowInfo，不重做数据叶 |
| “数组 helper 父调用待接” | 后续世界 1000、主宠物 5、浏览 5 三数组已接真实构析 helper 与 typed 回调 | 保留 C++/SEH/SDK 实际行为验收；原早期日志不改写 |
| 某些机器记录 status/sources 注释仍保留旧“仅 extern/种子待” | 同记录的后续 boundary/数据绑定及源码已有增量 | R04 定点整理；本轮不改机器证据或批量重分状态，不能只读 status 得出全部缺口 |

以上是状态摘要差异；没有将未运行功能改判为已验收。局部历史提案的“未批准/未实现”保留其日期，只有已被后续明确裁决覆盖的范围才更新当前口径。

## 4. R0–R8 阶段进度

| 阶段 | 当前进度 | 距离阶段出口的实际差距 | 关联任务 |
|---|---|---|---|
| R0 基线/裁决 | IDA 基线、x64、SDL 自动驱动及主要范围已有裁决 | 运行环境冻结、图标/光标资源条件及既定适配实施；P07原事实仍缺，不能重设已定基线 | P01–P18、G05、V01 |
| R1 全量盘点 | 3680 原元数据、1144 显式语义与 4 分类引用 | 全量函数/数据/导入/间接目标分母及未知归属未闭合 | R01–R04 |
| R2 启动/主循环/退出 | MainLoop、状态更新、WinMain 与 CRT 父源码已有 | 实际 CRT 启用、共享构析、入口/退出所有传递依赖未闭合 | C01–C12、U11、V02 |
| R3 平台/资源 | 地图、graphic、anime、输入及存档已有局部实现/接线 | 实际来源、生命周期、文件链、输入差异、原资源与音频尚缺 | G01、G05、T01–T03、P11–P17 |
| R4 渲染替换 | CPU Back/Temp/节点、队列、命中与提交已有 | 缓存初始化/重建/清理闭环和模式/像素对照 | C09、G01–G04、V04 |
| R5 网络 | 108 路分发、两帧父及大量请求/回复已有 | 全部业务数据/生命周期、murD与实际收发未闭合 | P09、P10、P16、U01–U10、V03 |
| R6 世界/UI/战斗 | 现代直接表及大量旧页面、Yobi、战斗动作已有 | 旧路由、适配待实施页面、数据 owner、攻击/载荷和生命周期 | P01–P08、U01–U11 |
| R7 覆盖收口 | 已有函数边界/未归属区间增量分类 | 全量差集、间接调用、共享尾和例外核算未完成 | R01–R05 |
| R8 整体对照 | 只有历史局部验证，本轮仅静态文档核对 | 完整链接及同输入/资源/初值的原新流程对照均未验 | V01–V06 |

## 5. 18 个显式未闭合入口逐项推进表

P 编号对应机器 pending_records 的原入口，不增加或缩减该分母。每行的完成条件是源码/差异收口条件，完整运行仍服从 V 系列门禁。

| 编号 / 原入口 | 状态与已有成果 | 下一步及前提 | 收口证据 / SPEC |
|---|---|---|---|
| P01 / 424620 | 源码工作项已关闭：完整3089条攻击生产、五表/四输出及既有三父已接 | D01次级图号入口0一次、跨轮状态/完整技能/收尾保持；全战斗/生命周期与运行另验 | [源码收口](../doc/cg2d_battle_attack_targets_spec.md#attack-target-source-closure)；不以源call表达式数推原覆盖率 |
| P02 / 4FF650 | 源码工作项已关闭：完整919条/第四按钮适配及D64E13写者已接 | 条件布局、全部消费与W32返回保持；外层/完整后续及运行仍待 | [源码收口](../doc/ui_windows/legacy_skill_panels.md#skill-detail-source-closure)；四原写点同源 |
| P03 / 402020 | 源码工作项已关闭：完整102条装载/计数及两内部caller已接 | 独立1000B仅入口清零一次；完整旧路由/文件与运行仍待 | [静态收口](../doc/ui_windows/legacy_log_viewer.md#legacy-log-source-closure)；正常返回S32页数 |
| P04 / 4021A0 | 源码工作项已关闭：完整686条页面与内部链已接 | 独立1000B仅入口清零；旧表外层、输入/音频与运行仍待 | [静态收口](../doc/ui_windows/legacy_log_viewer.md#legacy-log-source-closure)；连续owner及15/16语义保留 |
| P05 / 4D62A0 | 源码工作项已关闭：完整279条/格式后首字节1及共享消费已接 | 其余255B不清，原循环/发送保持；外层/完整流程/运行仍待 | [源码收口](../doc/ui_windows/legacy_give_receive_confirmation.md#give-receive-source-closure)；连续两256B owner |
| P06 / 4F7870 | 源码工作项已关闭：完整634条及唯一父调用已接 | 两hover入口-1；正常扫描/尾重查保持，旧外层与运行仍待 | [源码收口](../doc/ui_windows/legacy_pet_list.md#pet-state-source-closure)；五独立S16待提交状态及数量保持 |
| P07 / 4D8DA0 | 待事实；旧62初始化已有，真实表物理68项指向本父已证 | C9A94C模式生产/mode0第二命令仍待；现代4A0140不生产该槽，不猜或删按钮 | [模式/表项补证](../doc/ui_windows/legacy_shop_choice.md#shop-choice-mode-reachability)；缺xref不证明不可达 |
| P08 / 4626A0 | 源码工作项已关闭：WorldUpdate显式gate=1，旧声明已清 | 本体及顺序保持；完整创建/数据/请求验收沿U03/V03继续 | [静态证据](../doc/cg2d_creation_and_module_adaptation_evidence.md#p08-创建入口显式gate)；不算整体功能验收 |
| P09 / 403C50 | 源码工作项已关闭：完整154条明文取包与真实Loop调用已接 | 原W16整包/容量/CRLF/NUL/队列/状态保持；完整互通及V03仍待 | [源码收口](../doc/cg2d_network_plaintext_frame_spec.md#frame-source-closure)；仅401AB0位置旁路，无新分包门槛 |
| P10 / 403E50 | 源码工作项已关闭：完整146条明文帧与真实发送回调已接 | 原首标记/正文/旧尾/报告/原头和长度发布保持；完整互通及V03仍待 | [源码收口](../doc/cg2d_network_plaintext_frame_spec.md#frame-source-closure)；仅401960位置旁路，无加密或新格式 |
| P11 / 454C30 | 暂停；音乐初始化依赖尚缺 | 用户恢复音频后沿原初始化链推进 | 初始化、失败释放与共享状态；[音频](../doc/cg2d_audio_update.md) |
| P12 / 454EE0 | 暂停；退出清理依赖尚缺 | 恢复后沿真实退出调用处理 | 停止/线程/流/设备的清理顺序；[音频](../doc/cg2d_audio_update.md) |
| P13 / 458AE0 | 暂停；平台能力依赖尚缺 | 恢复后核原能力语义及 SDL 对应 | 返回与消费者对应，禁止补固定版本码；[音频](../doc/cg2d_audio_update.md) |
| P14 / 4FDBD0 | 暂停；旧7音乐页未完 | 恢复音频后实现并接旧路由 | 原状态/音量应用/文字与选择；[设置](../doc/cg2d_display_settings.md) |
| P15 / 4FE0E0 | 暂停；旧8音效页未完 | 同上，保留音乐/音效状态分工 | 原事件与实际后端应用；[设置](../doc/cg2d_display_settings.md) |
| P16 / 50E680 | 暂停；murD 直接回复缺口 | 恢复后补真实处理，不用默认返回填表 | dispatcher→处理→音频副作用；[音频](../doc/cg2d_audio_update.md) |
| P17 / 52C3D0 | 暂停；BGM音量应用缺口 | 恢复后接真实音量应用 | 共享值、应用时点及后端消费；[音频](../doc/cg2d_audio_update.md) |
| P18 / 5202E0 | 核对工作项已关闭：现有256B一次预清经静态核对 | 原API/后续失败与成功消费保持；完整登录/API及运行沿V03继续 | [静态证据](../doc/cg2d_creation_and_module_adaptation_evidence.md#p18-模块路径预清)；未计算/执行摘要 |

## 6. pending 以外的收尾任务

### 6.1 CRT 与基础依赖

| 编号 / 范围 | 当前证据级别 | 剩余差异与下一步 | 依赖与完成条件 |
|---|---|---|---|
| C01 / CRT 总策略 | 受限承接已定，NSH01/EXH01字节域、NCE01九抛出点及LCH01实现对象/两退出登记已迁移 | 其他组按具名子SPEC继续，不把局部承接计为原体等价/完整CRT | [运行库子SPEC](../doc/cg2d_crt_runtime_boundary_spec.md)、[LCH01](../doc/cg2d_locale_object_heap_migration_spec.md#locale-object-heap-source-closure)；完整门禁开放 |
| C02 / C++90 初始化表 | 90槽原值已有；AIS01当前66应用+2 SDK流配置槽同一TU原序，六图形/资源调用已迁入 | 17Legacy流或经典locale活动归属、两暂停音频及完整全表/宿主对应仍待；29新语义仅绑定 | 保持open；[AIS01](../doc/cg2d_active_initializer_order_spec.md#active-initializer-order-source-closure)，不造空回调数组或以当前投影代全90 |
| C03 / 四Init与登记 | 14流/七共享槽、SI01四Init对象、SR01登记0～8前缀及LCH01 locale提供者已有 | 原完整容量/较大正常索引/间接别名、经典55315C全表caller、全应用序及Legacy/宿主活动流构析仍待 | 保持open；[SR01](../doc/cg2d_standard_stream_registry_prefix_spec.md#standard-registry-source-closure)、[LCH01](../doc/cg2d_locale_object_heap_migration_spec.md#locale-object-heap-source-closure)，不凭登记接口称生命周期闭合 |
| C04 / 构析与入口迁移 | 保护/世界/UI及AIS01六图形/资源当前活动调用统一原相对序，旧三模块删除；多字节前置、LCH01两登记已有 | 全90/经典caller/标准流活动归属、完整异常清理与快速退出仍待 | 保持open；[AIS01](../doc/cg2d_active_initializer_order_spec.md#active-initializer-order-source-closure)、[LCH01](../doc/cg2d_locale_object_heap_migration_spec.md#locale-object-heap-source-closure)；源级投影不代实际SDK/运行 |
| C05 / codecvt | 旧消费者与方法槽在，真实生产未闭 | 宿主内部用完整宿主流类型；保留Legacy消费者仍补真实facet生产 | 不造空表/桥半类型/猜编码；[运行库子SPEC](../doc/cg2d_crt_runtime_boundary_spec.md) |
| C06 / 异常查找捕获 | 原数据/部分叶及NCE01六本机类型/原逻辑层级已有 | 旧私有5402AB/5407BC职责与实际Typed捕获/清理、硬件SEH仍须成组映射 | 保持open；[NCE01](../doc/cg2d_native_cpp_exception_spec.md#native-cpp-exception-source-closure)，不把compiler类型源码当SDK/运行验收 |
| C07 / 抛出与SDK | NCE01八实际throw/一当前异常重抛、单payload owner及原copy/析构/what已接；旧SDK声明撤销 | 实际C++20生成ABI、按值捕获/展开/非法重抛与SDK及完整生命周期仍待 | 保持open；[NCE01收口](../doc/cg2d_native_cpp_exception_spec.md#native-cpp-exception-source-closure)，不手造ThrowInfo/RVA或记原54B123体等价 |
| C08 / 原堆模式3 | 旧外层/模式2已有；NSH01字符串、EXH01异常文本及LCH01实现对象storage成对宿主承接 | facet/向量/其他CRT对象与扩容/容量/释放继续整组迁移，不复造16B双链 | 保持open；[LCH01](../doc/cg2d_locale_object_heap_migration_spec.md#locale-object-heap-source-closure)、[NSH01](../doc/cg2d_narrow_string_heap_migration_spec.md#narrow-string-heap-source-closure)；引用/共享/逻辑容量保持，其他域及运行未闭 |
| C09 / 图形双树 | 唯一被动存储/构析/即时登记已接，AIS01将两调用按世界表后原序迁入并删除旧模块 | 全原应用序/活动流及完整逐surface/快速退出清理仍待 | 保持open；[双树源码](../doc/cg2d_graphic_transform_lifecycle_spec.md#transform-cache-lifecycle-source-closure)、[AIS01](../doc/cg2d_active_initializer_order_spec.md#active-initializer-order-source-closure)，TextureSurfaceOwner独立自动清理保持 |
| C10 / FILE 剩余原调用 | 核心/业务及filebuf库点已部分迁移 | 按下表逐个原点找父和阻断原因，只补当前差集 | [stdio](../doc/cg2d_stdio_contract.md)；不能把所有余点当可立即迁移，也不因计数齐而宣称文件验收 |
| C11 / 应用分配域 | 21应用calloc及原窄裁决保持；NSH01/EXH01字节与LCH01实现对象是独立CRT域 | 逐调用登记批准范围、配对/失败消费；base facet/向量与其他对象不外推 | 保持open；[内存合同](../doc/cg2d_crt_memory_contract.md)、[LCH01分域](../doc/cg2d_locale_object_heap_migration_spec.md#locale-object-heap-decision)，不把21点统一改原堆或当未实现 |
| C12 / 字符串与格式剩余 | 已接点及分类计数见§2 | 长度余1与格式余23逐原点处理；范围排除/音频/替换/待事实或实施不混算 | [字符串](../doc/cg2d_legacy_string_compare.md)、[stdio](../doc/cg2d_stdio_contract.md)；格式余1沿P07补事实，旧点不重做 |

FILE 数量沿现有合同，原点去重，库点与业务点分列；“余”仅为该函数原引用差集，不是新任务总数：

| 操作 | 已接业务 | 已接库点 | 原引用分母 | 尚余原点 |
|---|---:|---:|---:|---:|
| 打开 | 64 | 0 | 67 | 3 |
| 关闭 | 90 | 2 | 96 | 4 |
| 读行 | 7 | 0 | 8 | 1 |
| 批量读取 | 55 | 0 | 55 | 0 |
| 批量写入 | 26 | 2 | 29 | 1 |
| 定位 | 20 | 2 | 25 | 3 |
| 刷新 | 2 | 1 | 5 | 2 |
| 单字节读取 | 22 | 3 | 39 | 14 |
| fprintf | 11 | 0 | 11 | 0 |

余点可能落在暂停/待决/排除父中；本表未新增逐点归类结论，C10 必须先沿原合同逐项确认。此表不包含已另记的 rewind 等入口，不用于推断整个 FILE 分母。

### 6.2 世界、UI、网络数据与路由

| 编号 / 范围 | 当前已具备 | 剩余任务 | 完成条件 / 证据 |
|---|---|---|---|
| U01 / 宠物技能目录 | 页面、共享图号CC3E64/页数C8AF84零初值与真实存储、页号D45F74已有 | CA0478数组定义/容量/生产；两标量运行时生产 | 生产记录→预览/翻页→使用/回复，不能补空目录；[旧58–61](../doc/ui_windows/legacy_pet_skill_catalog.md) |
| U02 / 交易名单 | 4E4EA0消费、CC3208页号与D30134/CF46D0存储已有 | CB1D1C的17B名单容量/定义/生产及数量/页数写者 | 实际名单→选择→请求完整同源；[旧51](../doc/ui_windows/legacy_trade_target_list.md) |
| U03 / version payload | 创建/升级消费者已有 | 9A5C10实际容量、内容、存活期与生产者 | 不造空正文或默认版本；[创建](../doc/cg2d_character_select_scene.md)、[声明](../src/CharacterSelectSceneDependencies.hpp) |
| U04 / 旧表与键表真实分派 | 旧99条handler证据、许多页面/短入口已有 | 57C368外层间接路由、键表真实分派及输入焦点 | 地址token不当可调用函数；取证实际签名/状态/调用顺序；[旧表索引](../doc/cg2d_ui_window_runtime.md) |
| U05 / 商店/修理/鉴定 | 旧62初始化、63–71主要页本体已有 | 独立商品/图号/页数/初页生产与P07父 | 生产→显示→确认→发送→更新，初始化本体不重复写；[旧62](../doc/ui_windows/legacy_shop_choice.md)、[修理](../doc/ui_windows/legacy_repair_list.md) |
| U06 / 仓库/交换/交出 | 旧72–76、83–85多页消费和格式已有 | D30404/CB1DDC容量、宠物展示与独立商品/页数；P05 | 共用owner和实际数据来源闭合；[仓库](../doc/ui_windows/legacy_storage_items.md)、[交换](../doc/ui_windows/legacy_barter_list.md) |
| U07 / 旧55技能正文 | 对话/技能选择/学习/遗忘本体已有 | 五行正文完整生产、独立状态/图号与外层路由 | 正文来源/容量/布局/请求对应；[技能选择](../doc/ui_windows/legacy_skill_choice.md) |
| U08 / 战斗执行闭环 | 257表项/200动作及43CBB0→427470链已有 | P01攻击生产、载荷完整字段/归属、附属清理及事件执行 | 生产→动作→回调→释放顺序；[战斗](../doc/ui_windows/shared_battle.md) |
| U09 / Yobi 生命周期 | New/四相更新/Clone/终止删除及8个新增身份已有 | 全部creator/cleanup、载荷浅复制后的共享寿命 | 保留Clone原失败返回，不能以基础生命周期存在称所有对象完成；[Yobi](../doc/cg2d_yobi_runtime.md) |
| U10 / 跨页持久化与请求 | 邮件、公会、公开交易、黑名单、存档等大量内部链已有 | 逐链核保存/回复/回滚/权限/共享数据；依赖U04与P09/P10 | 每条链给真实起点、结果和退出；[邮件](../doc/ui_windows/legacy_mail_compose.md)、[公会](../doc/ui_windows/legacy_guild_contact_list.md) |
| U11 / 场景/主循环传递依赖 | MainLoop、普通22状态、部分场景重置与切换已有 | 逐状态核所有传递依赖、退出/断线/重进与资源清理 | 不重建通用分发框架；[主循环](../doc/cg2d_main_loop.md)、[场景](../doc/cg2d_scene_state.md) |

### 6.3 渲染、资源与文本

| 编号 / 范围 | 当前已具备 | 剩余任务 | 完成条件 / 证据 |
|---|---|---|---|
| G01 / 图形资源完整生命周期 | 资源/容器已有；GDL01装载/建链已修，GUP01上传条件取样/同record双色表借用已修，原owner保持 | 全部父参数、文件/加载/上传/重建/场景/退出清理的owner/失效与标准流依赖 | 保持open；[GUP01](../doc/cg2d_graphic_upload_alignment_spec.md#graphic-upload-source-closure)、[GDL01](../doc/cg2d_graphic_load_alignment_spec.md#graphic-load-source-closure)、[DX SPEC](../doc/cg2d_graphic_dx_cache_x64_spec.md)；局部生产/上传不代全寿命/运行 |
| G02 / 队列与命中 | 原队列/绘制/命中已有；GQS01排序/GDL01装载与建链已修，GUP01原条件索引/尺寸/表与上传取样已修 | 全场景/父参数、命中/文本token/对象与节点借用、间接alias及清理后访问 | 保持open；[GUP01](../doc/cg2d_graphic_upload_alignment_spec.md#graphic-upload-source-closure)、[GDL01](../doc/cg2d_graphic_load_alignment_spec.md#graphic-load-source-closure)、[GQS01](../doc/cg2d_graphic_sort_alignment_spec.md#graphic-sort-source-closure)；两helper/三原点不代完整父/闭环 |
| G03 / 模式2/3/4 | MakeSurface尺寸/格式门禁、接口/设备生产与探测已有 | 模式切换/回退、裁剪、采样/混合、提交及重建真实流程 | [纹理](../doc/cg2d_texture_rendering.md)、[呈现](../doc/cg2d_sdl_presenter.md)；像素/实际driver留V04 |
| G04 / 渲染替换差异 | SDL/CPU与DX原生机制豁免已批；GUP01上传根上层条件取样/尺寸/表/写序已修，CPU核心保持 | 其他上层RAND/宽度/坐标/色键/提交序、完整模式/生命周期和像素/画面 | 保持open；[GUP01](../doc/cg2d_graphic_upload_alignment_spec.md#graphic-upload-source-closure)、[总SPEC §10](cg2d_full_reconstruction_plan.md#10-完成门禁)；原生豁免不代上层或画面验收 |
| G05 / 原图标/光标 | 使用关系有记录，原资源取得/权限未闭合 | 明确合法可用原资源与读取条件 | 不重开原EXE、不造替代图/下载；[启动](../doc/cg2d_startup_flow.md)、[资源来源](../doc/cg2d_resource_sources.md) |
| T01 / 文本生产消费合同 | 输入/文本生产及字体消费已局部对照；GQS01原钳制后首槽/每项共享取样/最终S16排序数量已修 | 各来源编码、容量/NUL、共享存储、字节/像素单位及全部上游/命中/排序父/资源/生命周期逐链归档 | 保持open；[GQS01](../doc/cg2d_graphic_sort_alignment_spec.md#graphic-sort-source-closure)、[FLC01](../doc/cg2d_font_lifecycle_alignment_spec.md#font-lifecycle-source-closure)、[文本SPEC §5](../doc/cg2d_text_encoding_spec.md#5-跨函数复核模板)；不推全工程代码页/验收 |
| T02 / 输入/IME差异 | 输入/队列生产与FLC01字体初始化/原消费/释放局部对齐；保存族号/width-face-height及unsigned派生钳制落实 | 全部上游/原新参数、排序/初始化退出/资源父、焦点/索引、原API候选/间接alias及完整寿命，SDK/画面运行未验 | 保持open；[FLC01](../doc/cg2d_font_lifecycle_alignment_spec.md#font-lifecycle-source-closure)、[TQP01](../doc/cg2d_text_queue_alignment_spec.md#text-queue-producer-source-closure)；D05/RPLAN.24保持 |
| T03 / 字体与显示 | 原字节TextOutA、共享Back DIB和case4已接 | 目标字体、字形、原字节显示边界及现有失败/超长处理的适用范围 | [字体](../doc/cg2d_font_measure.md)、[文本SPEC](../doc/cg2d_text_encoding_spec.md)；原数据不被显示副本反写，实际画面留V04 |

### 6.4 全量覆盖与最终验收

| 编号 / 范围 | 当前状态 | 下一步 | 关闭条件 |
|---|---|---|---|
| R01 / 函数分母 | 元数据和部分语义映射已有 | 按稳定ID归并直接定义、语义映射、范围排除和边界分类，标剩余未知 | 每个原条目有归属，重叠不重复计数；不以直接定位“未命中”当未实现 |
| R02 / 数据/间接目标 | 共享槽、局部表与回调证据已有 | 补剩余数据区间、常量、函数表、所有者/读写者及动态边 | 原身份/布局/来源/消费者/生命周期逐项可追溯 |
| R03 / 未归属区间 | 3061原区间快照与部分后置回调/数据/填充分组已有 | 沿候选清单处理尚未归属区间，不重做已证分类 | 区间用途及剩余未知明确；不把无直接xref当不可达 |
| R04 / 台账一致性 | 本表已识别总览、局部速查和机器自由文本的时效差 | 后续按任务更新同一记录的当前状态，保留历史字段证据 | 清单/源码/当前摘要相符；来源只有“关联”的项不升级成实现 |
| R05 / 例外收口 | 键盘/newest/两加密叶/低色深排除与窄替代已有 | 核共享callee、混合32位分支、参数/状态保留及排除接线 | 排除与替换有具体授权范围；音频/未知不混入；不降低总SPEC |
| V01 / 环境与输入 | 基线/部分来源已有；运行环境未完整冻结 | 在运行准备获准后记录实际工具链、SDK、系统/编码、参数/配置/资源 | 可复核实际来源/版本/输入与缺失项，不设文件摘要门禁 |
| V02 / 完整启动退出/链接 | 三静态库，父源码不等于可运行入口 | 先静态收口C/U依赖；获准后才配置可执行目标、构建/链接及验证启动退出 | 无假实现/重复构析/遗漏清理；不能拿历史静态库编译代当前完整链接 |
| V03 / 登录与网络真实流程 | 请求/分发及两帧父源码已有，完整链未验 | P08–P10源码工作项已闭；全部数据/依赖闭合后，在获准环境核登录/创建/升级/断线和实际字节 | 收发、状态、副作用与真实数据相符；保留已排除newest的明确接线 |
| V04 / 世界/UI/渲染/输入 | 局部源码/静态证据 | 获准后按同输入、资源、初值核地图/UI/战斗、焦点/IME、模式和画面 | 时间/RNG消费、坐标/裁剪/像素、状态及生命周期有对照证据 |
| V05 / 音频真实流程 | 暂停且未验 | 用户恢复P11–P17并允许运行后核播放、通知、音量、循环与退出 | 真实设备/资源/时序通过；暂停不得据此宣称整体完成 |
| V06 / 最终验收报告 | 尚无完整R8结果 | 汇总R01–R05和V01–V05，逐适用门禁列结果/未决/例外 | 总SPEC §10全部适用条件闭合；未知/未验尚在则保持整体未完成 |

## 7. 建议收尾顺序与任务记录

后续按本轮具名决议和子SPEC推进；本表登记设计状态，不把设计批准当源码实现。

| 顺序 | 任务组合 | 为什么先做 / 可以交付什么 |
|---|---|---|
| 1 | D01–D04已定方案的实施准备、P07补证条件 | 避免把需要局部适配或CRT职责迁移的问题拆成反复无效接线；未决不阻止其他独立取证 |
| 2 | C03、C05–C09、U01–U03、G05 | 补真实数据、布局、提供者及资源条件；这是下游接线的前提，缺事实时停受影响项 |
| 3 | C02/C04、C10–C12、U04–U11、G01–G04、T01–T03 | 沿真实父链与owner收口；按具名决议逐组迁移，事实缺口继续可见，不启用半套CRT |
| 4 | R01–R05 | 与前述小步同步登记，最后集中对全量差集/间接边/排除清单收口；不能仅检查这60项 |
| 5 | P11–P17、V01–V06 | 音频和运行分别等待用户恢复/授权；静态可做部分继续，最终运行门禁保留 |

每个小步至少记录：**任务号、原入口/数据身份、实际改动、证据链接、静态/接线/运行分别达到哪一级、尚余依赖、对应提交**。验收前不能只把“待实现”改成“完成”；若补了存储但未补生产，应明确拆开写。无充分依据不估工时，不给整体百分比。

## 8. 本轮检查范围

- 读取五份独立SPEC、总SPEC及唯一进度的全部后续增量；按关联读取启动/stdio/堆/字符串、UI及分类合同。
- 静态解析机器清单确认1136记录/18待决/4分类引用，核原库存计数；没有运行项目分析脚本或重新扫描生成全库定位。
- 定点核三静态库目标、创建无参调用与显式本体、两网络接线、七CRT槽、14标准流存储、四UI标量、堆三声明、异常抛出声明和绘制/命中源码。
- 其余细分实现数量沿已记录局部合同，未重新逐指令审计既有本体；本表是可执行的收尾索引，不是全EXE完成证明。
- 本轮仅改文档并做UTF-8、链接、任务号/待决映射及限定差异检查；用户已有六项工作区修改保留，不纳入提交。

本轮C02/C04/G01局部源码与限定证据见[四容器子SPEC](../doc/cg2d_resource_container_lifecycle_spec.md#resource-container-source-closure)；三项仍open，不能因接口落位关闭全应用顺序/资源生命周期。
