# 旧鼠标按下追踪器

## 范围与证据

本项还原 `sub_506580`、`sub_5067F0` 及两者依赖的五个短链表 helper。取证只使用
当前只读 IDA 实例 `ida-17524` 的 `D:\SOFTSTAR\cg2d.exe`、对应二进制数据和
当前源码；`survey_binary` 确认目标为 32 位 x86、小端、基址 `0x400000`，
SHA-256 为 `e7c2790f479862635932e043b2d7e93b9e791a1edc276a960af05f1454c54010`。
本项未修改 IDA，也未使用 STONAGE 作为行为证据。

函数边界及 IDA 统计如下：

| 原函数 | 半开边界 | 指令/基本块 | 真实语义 |
| --- | --- | ---: | --- |
| `sub_506580` | `[0x506580,0x5066C1)` | 91/23 | 释放旧动态池，选择并清空新池，重建两条链 |
| `sub_5066F0` | `[0x5066F0,0x506720)` | 20/7 | 把第二个节点插到第一个节点之后；成功返回 1 |
| `sub_506720` | `[0x506720,0x506757)` | 20/7 | 摘链，按原顺序清 `prev`、`next`；成功返回 1 |
| `sub_506760` | `[0x506760,0x506780)` | 13/3 | 取空闲链首，空链返回 0 |
| `sub_506780` | `[0x506780,0x5067A0)` | 13/3 | 淘汰活动链尾，空链返回 0 |
| `sub_5067A0` | `[0x5067A0,0x5067E2)` | 20/4 | 空闲优先、否则淘汰，写入记录并插活动头 |
| `sub_5067F0` | `[0x5067F0,0x50686C)` | 55/11 | 查找 `(x,y,tag)`、更新 LRU 次序并返回计数 |

`sub_5067F0` 的两个直接 caller 仅为
`MouseEvent::UpdateLDown@0x506900` 与
`MouseEvent::UpdateRDown@0x506B50`。

五个 helper 的返回控制流也按反汇编保留：`sub_5066F0` 在任一输入为空或锚点
`next` 为空时返回 0，否则按 `next.prev`、`anchor.next`、`node.prev`、
`node.next` 顺序写入并返回 1；`sub_506720` 要求节点、`next`、`prev` 都非空，
再按 `next.prev`、`prev.next`、`node.prev=0`、`node.next=0` 顺序写入并返回 1。
`sub_506760/sub_506780` 先保存候选，命中各自空链哨兵才返回 0，否则忽略摘链
返回值并返回已保存候选。`sub_5067A0` 先取空闲首项，失败才取活动尾；两者都失败
返回 0，取得节点后忽略插链返回值并固定返回 1。

## 配置、节点池与重置

原 `[0x58ACBC,0x58ACC8)` 三个 W32 初值依次为 signed threshold `50`、signed
capacity `100`、池指针 `0xD68DD0`。新端以
`gLegacyMouseTrackerThreshold/Capacity` 各保留唯一 signed32 配置存储，并在追踪器
翻译单元中保留唯一原生池所有权。配置存储供后续六个事件和网络包装器复用，但本项
不接这些调用点；外部修改 capacity 后必须先调用重置，才可进入追踪入口。

原固定池 `[0xD68DD0,0xD69730)` 恰为 `100 * 24` 字节且映像初值全零。新端固定池
同样是 100 个节点，并对 `2400` 字节总长作断言。节点布局为：

| 偏移 | 类型 | 字段 |
| ---: | --- | --- |
| `+0x00` | I32 | `x` |
| `+0x04` | I32 | `y` |
| `+0x08` | I32 | `tag` |
| `+0x0C` | I32 | `count` |
| `+0x10` | W32 | `next` |
| `+0x14` | W32 | `prev` |

`LegacyMouseTrackerNode32` 对标准布局、可平凡复制、24 字节总长及六个字段偏移均
有静态断言。

`sub_506580` 先在旧池非空且不是固定池时释放它。仅当 capacity 不等于 100，且
按 signed32 比较处于 `[10,5000)` 时，才申请精确的 `24 * capacity` 字节；该范围
同时保证原 x86 乘法不溢出。申请失败、capacity 为 100 或范围无效时均退回固定池，
并把 capacity 强制恢复为 100。随后清空最终池；threshold 按 signed32 小于 5 时
恢复为 50；最后重建空闲链和空活动链，返回最终 capacity。

四个原哨兵分别位于 `0xD68AD0`（空闲头）、`0xD69730`（空闲尾）、
`0xD69750`（活动头）、`0xD69770`（活动尾）。重置保持原
`0x506628..0x50665A` 写序：先活动头尾，再写空闲头 `prev`、空闲尾
`next/prev`，最后写空闲头 `next`。节点再按索引写 `prev/next`；与原端一致，
重置时空闲尾的 `prev` 先指空闲头，不额外改成最后一个节点。

## 查找、计数与淘汰

活动链按 `(x,y,tag)` 顺序比较。命中后先按需增加 `count`，再摘链并插回活动头。
计数条件保留原非对称行为：

- 活动首项且 `tag == 0` 时不增加；
- 活动首项但 `tag != 0` 时增加；
- 搜索只要后移过一次，后续命中无论 tag 都增加。

计数增加使用 W32 位模式加一再还原 I32，明确保持 x86 `inc dword` 回绕而不触发
C++ signed overflow。未命中时先取空闲首项；空闲耗尽则摘活动尾复用；新记录写
`x/y/tag/count=1` 后插活动头。外层 `sub_5067F0` 与原 `0x506836..0x506841`
一致，忽略分配 helper 的返回值并固定返回 1。

## x64 handle 适配与安全边界

原节点的两个 W32 链接是 32 位进程内指针。x64 新端不把原槽扩大为 64 位，也不
截断本机指针，而使用以下定宽 handle：

| handle | 映射 |
| ---: | --- |
| `0` | 空链接 |
| `1..nativePoolNodeCount` | 当前原生池的 `handle-1` 号节点；重置完成后等于最终 capacity |
| `0xFFFFFFFC` | 空闲头哨兵 |
| `0xFFFFFFFD` | 空闲尾哨兵 |
| `0xFFFFFFFE` | 活动头哨兵 |
| `0xFFFFFFFF` | 活动尾哨兵 |

动态池只由本模块的本机 `Node*` 持有，W32 链接槽从不保存该指针。最窄安全偏差
位于 handle 到节点的解析边界：除当前池实际节点数、四个哨兵及空值外的损坏
handle 一律解析失败；helper 在首次需要解引用时返回 0，公开查找入口在当前遍历项
无法解析时返回 0，不执行越界本机内存访问。原函数本来就忽略摘链、插链及分配
helper 的返回值，这些路径仍保持各自原返回控制流，不统一改写。正常链路的四次
插入写序、四次摘链写序、返回值及 LRU 结果均不变。`nativePoolNodeCount` 只记录
当前本机分配范围，供 handle 边界检查，不是第二份 legacy capacity 配置；不额外
把链表扩展为通用容器。

本模块不注册静态析构；进程退出时的动态池释放留给后续还原的原显式退出链。

## 5066D0 默认配置包装

2026-09-24，本线程在当前 `ida-17524/cg2d.exe.i64` 核对完整
`[5066D0,5066E9)`：`5066D0` 向唯一阈值 W32 `58ACBC` 写 50，
`5066DA` 向唯一容量 W32 `58ACC0` 写 100，然后 `5066E4`
尾跳 `506580`；没有参数、其他读写或独立成功返回。唯一直接
caller 是退出链 `457A8B`，返回值在父函数中被丢弃。

新端在追踪器 owner 中实现
`platform::ResetLegacyMouseTrackerDefaults_5066D0()`，按原序写两个
既有 signed32 配置并直接返回
`ResetLegacyMouseTracker_506580()` 的完整 S32 值；退出链改为
直接调用。该重置函数负责原有池释放/选择、清零与链表重建，本步
不修改其内部，也不新建配置或节点池副本。此项只闭合这一直接
callee，不计 `ShutdownClient_457A40` 的其他未落位清理、完整链接或运行。

## 未接线边界

本项只把新实现加入 `cg2d_re_startup`，没有接入六个鼠标事件、
`InitializeClient`、网络 TQ 配置包装器，也没有提供 Report/no-op。
`sub_5066D0` 已由上节在后续小步接入；本节原先的未接线范围为历史状态。
已有旧鼠标核心状态和逐帧光标逻辑均未改动。按用户约束，本项只做人工静态自查，
未编译、未测试、未 lint、未格式化、未生成代码、未运行客户端，也未读取任何
`.obj`、静态库或其他编译产物。

## 50E7B0 网络配置包装

2026-09-24，当前 IDA `[50E7B0,50E7C8)`：
`50E7B0/50E7B4` 依次读取第二、第三个 W32
参数，`50E7B8` 先写容量 `58ACC0`，
`50E7BD` 再写阈值 `58ACBC`，`50E7C3`
尾跳 `506580`；首个参数没有被读取，
返回值完整沿用重建入口。唯一直接 caller
`network_dispatch@516749–51677A` 对
`protocolArgs[1]/[2]` 分别调用 `517190`，
再按第三参数、第二参数、原 `fd` 的压栈顺序
调用本包装，随后丢弃返回值并返回零。

新端在既有 `cg::platform` 追踪器 owner
落位 `ConfigureLegacyMouseTracker_50E7B0`，
沿原顺序写唯一 signed32 容量与阈值，
直接返回 `ResetLegacyMouseTracker_506580()`
的结果；不复写池分配、阈值归一和链表重建。
本节取代上述“网络 TQ 配置包装器未接线”
的旧状态中仅本体缺失部分；原网络分派仍未
落位，也没有本函数的新端调用点，不计
网络配置链、完整链接或运行通过。

<a id="fixed-pool-byte-observer-current"></a>
## 登录负分类的固定池BYTE观察（BRU01，2026-10-07）

既有模块新增ReadLegacyMouseTrackerFixedByte只读桥，见[局部裁决](cg2d_login_negative_selection_byte_owner_spec.md#login-negative-byte-decision)。前16B坐标等值字段从fixedPool现场取BYTE；next/prev保持原W32 handle，仅在观察边界按原固定节点D68DD0+(index×24)及四哨兵身份提取原BYTE。投影是不可解引用的原标量身份，不是新端地址，不截断native指针，不写回链接或增加影子池/反查表。固定池切入动态nativePool后仍观察其原存储，不能反算当前池地址。已有24B布局、配置、重置、申请释放、链逻辑/原边界检查与RNG全文保持；本步没有新异常保护或故障/回绕模型。仅登录12点现场读取和身份字节源码合同，不代完整运行验收。

## 原生节点链接、正常失败与取样修复（BUV01，2026-10-08）

[新原生链接基线](cg2d_mouse_tracker_native_links_spec.md#mouse-tracker-native-links-baseline)与[裁决](cg2d_mouse_tracker_native_links_spec.md#mouse-tracker-native-links-decision)固定追踪/配置/PGXE十完整根262条799B/十一页、14directcall/34jump/19RET、25引用13父身份、原字串/默认值17B和七65B NN_NOP用途。原next/prev通过pool+index×24和四哨兵地址实际解引用，现旧W32 handle及nativePoolNodeCount解析保护由已证地址优先T*裁决覆盖。

现LegacyMouseTrackerNode仅两链接为非拥有T*，四S32/唯一阈值容量和同一gTracker owner保，物理大小随本机地址拓宽；原24B只有只读字节视图，不再等于x64物理sizeof。Insert/Detach的原nullptr失败及四写序保；Take/Evict只原哨兵比较，保存候选调用Detach后返候选；Allocate保空闲/淘汰/仍空失败，Track保首项tag0抑制、x-y-tag短路/先摘后插/读count返，普通++count替代回绕helper。三个handle/解析/计数helper退休，不造新helper/owner或异常模型。

Reset恢复保存旧pool、capacity一次门禁/申请，malloc先发布返回含null再回固定池/100；保存清理指针和对应容量，阈值<5原业务门禁保。原头尾发布序/首轮保存pool、后轮fresh pool、prev后fresh容量/next用fresh pool、保存循环容量后增index、最后返index保。默认/网络配置三句/五句及PGXE Report正确全保，不计I。

固定BYTE桥仍原步长24/字段16与20，前16B同源S32现场读；链接T*只按固定池索引/四哨兵投影原W32身份，不截native地址/写回或造影子池，动态pool切换后仍观察fixedPool旧内容。旧handle/24B物理布局/损坏解析和计数回绕段仅历史；RPLAN.24异常不专项，真实null/分配失败不豁免。完整父-callee/owner-alias寿命/ABI/运行和R8继续。

### BUV01 实际限定静态结果

十完整原根262条799B/十一页、14directcall/34jump/19RET及25引用13父身份、原17B/七65B用途、两源码仅十二批准片段、原T*字段/原nullptr-哨兵/链写序及失败发布和pool-capacity采样/index返回、固定24B只读视图移交、完整机器及派生差异模型、十六文件UTF8/LF/四锚点及六限定证据各25来源已通过。文本汇总与Check均退出0。
实际语义1741/有效1734、文档1852（50.33%）、F882/E1032、R7已审444/余2617、LOOP154/INPUT47/NETWORK63/LOGIN13；原分母/其他域/差队列/pending18与ECX pending7/closed11和完整门禁保。七原根和BYTE桥实修/三旧helper退休，默认/网络配置/Report三正确正文保持不计新增I；未编译、链接、运行或证明完整窗口/登录/配置/明文互通验收，整体目标继续active。
