# InitLoading 完整阶段机

2026-09-27，原 `ida-17524` 只读核 `[457B10,4582FE)` 全部指令，
读取 `458300` 起 34 个 W32 跳表项并逐项核目标；未重开或保存数据库。
按用户工程决策委托，以下 SPEC 实施完整函数，不删除不可达阶段，
不把开场替换为直接跳登录。cg3a 可信参考不覆盖本节具体指令。

## 入口与跳过

入口局部 phase=0、image=0、继续标志=1；调用 `45C970` 后发布唯一
`render::gPaletteCapability`，取一次 U32 tick 同时初始化局部阶段时钟
和现有 double 帧截止。后续重探测不回写 capability。

每轮先泵消息，返回零才调用 ShutdownClient 并返回 0；否则依次清
图形 W16 计数、第二 W32 计数，再 CacheReset。依短路顺序检查
鼠标点击槽 `D68D9C` 的低 W8 bit0/1、Enter/Space/Escape 的 W8 bit1。
有输入且 phase<21 或 phase 在 30..33，改为 21；21..29 不改变。
超出跳表范围只走公共尾，不新增错误处理或退出。

## 分发表

`changing==0` 均读取同一 `628FEC`。表内“超时”先取当前 U32 tick，
再判断 `phaseTick+毫秒 < now` 的严格无符号比较，不换成差值计时。

| phase | 原目标 | 阶段行为与后继 |
|---:|---|---|
| 0 | 457BBF | ClearSurface；按原宽度/顺序初始化描述符；SelectPalette(19,1)；phase=1；YobiClearUp |
| 1 | 457C1F | image=22029；SelectPalette(23,30)；直接 phase=5，不修正成 2 |
| 2 | 457C3A | changing==0 时刷新 phaseTick，转 3 |
| 3 | 457C55 | 4000ms 超时才 SelectPalette(19,30)，转 4 |
| 4 | 457C7B | changing==0 时查询 238564，结果<1 再查询 22049；无论结果均转 5 |
| 5 | 457CBF | image=99103；SelectPalette(23,30)；转 6 |
| 6 | 457CDA | changing==0 时刷新 phaseTick，转 7 |
| 7 | 457CF5 | 4000ms 超时才 SelectPalette(19,30)，转 8 |
| 8 | 457D1B | changing==0 转 9 |
| 9 | 457D32 | image=22027；SelectPalette(34,30)；转 10 |
| 10 | 457D4D | changing==0 时刷新 phaseTick，转 11 |
| 11 | 457D68 | 4000ms 超时才 SelectPalette(19,30)，转 12 |
| 12 | 457D8E | changing==0 时查询 22049，结果<1 转 13，否则 17 |
| 13 | 457E3A | image=22028；SelectPalette(22,30)；转 14 |
| 14 | 457E55 | changing==0 时刷新 phaseTick，转 15 |
| 15 | 457E70 | 2000ms 超时才 SelectPalette(19,30)，转 16 |
| 16 | 457E96 | changing==0 转 26 |
| 17 | 457DC7 | image=22049；SelectPalette(26,60)；转 18 |
| 18 | 457DE2 | changing==0 时刷新 phaseTick，转 19 |
| 19 | 457DFD | 2000ms 超时才 SelectPalette(19,60)，转 20 |
| 20 | 457E23 | changing==0 转 13 |
| 21 | 4580FC | image=0；SelectPalette(0,1)；ClearSurface；先转 22，再探测 capability，负值改转 24，不发布探测结果 |
| 22 | 458123 | image=0；SelectPalette(19,1)；ClearSurface；转 23 |
| 23 | 45813D | image=0、继续标志=0；仍执行本轮公共尾 |
| 24 | 458145 | 先转 25，再 ResetLoginMessage |
| 25 | 458151 | UpdateLoginMessage(原提示) 恰返回 1 则立即返回 0，跳过公共尾且不 Shutdown；其他结果继续 |
| 26 | 457EAD | image=238564；有 3D 时 SelectPalette(0,0)，按 B/G/R 顺序清颜色；无 3D 时 SelectPalette(0,30)；转 27 |
| 27 | 457EFC | 有 3D 按下节增亮；无 3D 等 changing==0 后刷新 phaseTick、转 28 |
| 28 | 457F85 | 2000ms 超时后再查 3D；无 3D 才 SelectPalette(19,30)；两路均转 29 |
| 29 | 457FB4 | 有 3D 按下节减暗；无 3D 等 changing==0 转 30 |
| 30 | 458046 | drawingSuppression=1；image=99304；SelectPalette(38,60)；转 31 |
| 31 | 45806B | 先采样 changing，再 drawingSuppression=1；采样为零才刷新 phaseTick、转 32 |
| 32 | 458090 | drawingSuppression=1；2000ms 超时才 SelectPalette(19,30)，转 33 |
| 33 | 4580C0 | SelectPalette(19,0)；查询 22049；drawingSuppression=0；结果<1 转 13，否则 17 |

阶段 4/12/33 复用真实 GetGraphicMapNoIndex；该叶无条件写输出，
局部查询槽不额外初始化。阶段 33 原复用的入口 tick 栈槽此时已无活跃
读取者，新端按同一输出用途承接，不恢复或复制已过期栈值。

## 描述符与颜色

原 `[73ED28,73ED38)` 全部直接引用都在本函数，
使用私有 `GraphicDescriptor16`，PE 零初值；不复用淡变的 `73ECE8` 图号。
phase 0 先按 W16 清 +6/+4/+2/+0，再按 W8 清 +A/+9/+8，
写 +B=FF，清 +C/+D，最后按 W16 清 +E。
不在每次函数入口无条件重置，以保留输入提前跳过 phase 0 的路径。

phase 27 有 3D 时先读取 R(+A) 判 `R+4<=255`：
成立则重新依序读 R/G/B，再依序写各自低 W8 的 +4；
不成立则按 R/G/B 写 FF，刷新 phaseTick，转 28。
alpha、clip、mixMode 等其他字节不动。

phase 29 有 3D 时读取 R：非零且 R>=4，先读 G、写 R-4，
再读 B、写 G-4、B-4；R 在 1..3 时按 B/G/R 清零，
本轮仍停在 29。R 已为零时**再次**查询 3D，非零才
SelectPalette(19,0)，随后转 30；不合并两个查询。

消息 `559358` 含 NUL 共 21B：
`a853a6b3abf6b7d3a5bfb160b57ba7c7a677b8cb00`。
保留原字节交既有消息入口，不转为 UTF-8 或改写正文；
同步收准其头文件中与既有实现不符的 UTF-8 注释。

## 公共尾与帧处理

每轮阶段处理后：YobiUpdateAll；image 非零才 DrawGraphic(320,240,255,...)，
并按**更新后的** phase 是否在 26..29 决定传同一描述符或 nullptr。
随后 CursorUpdate→SortTextures→507140→UpdatePalette→ClearKeyFlags。
不重置 drawingSuppression；从 30..33 提前跳到 21 也不补清零。

帧阈值先计算，再取 tick；仅有序严格大于且追赶计数按 S32<20
才跳帧。跳帧先计算新截止，再写追赶计数+1、截止，
仍进入公共结束判断。实绘先查询 3D，为零才递增同一调色板版本；
窗口活动值恰为 1 才写 2，再 DrawAllQueued、按重载槽==1 写调色板并清槽、
Flip、Sleep(1) 等待严格大于截止，最后依序更新追赶计数/帧数/截止。
不加入淡变的逐帧 ClearSurface，也不借普通 MainLoop 的激活/防护门禁。
NaN/unordered 保留在非严格大于路径，不改成简单 <=。

phase 23 清继续标志后，跳帧或实绘都必须完成公共尾才返回 1。
这与消息泵退出的 Shutdown→0、phase 25 的无 Shutdown→0 是三种不同出口。

## 实施边界

完整入口放在 startup 目标的 `LoadingScreen.cpp`，namespace 保持既有
`platform::initialization_detail`，由 InitializeClient 原调用直接接入。
不把 34 个阶段拆成任意新业务框架；状态局部化，只有已证 16B 描述符
作为唯一私有存储。已实现的资源、Yobi、键鼠、消息、调色板和 Flip
直接复用，未完成的 HasLegacy3DDevice、507140、DrawAllQueued
仍是真实依赖，不提供固定返回或空实现。

本步静态核对不代替加载画面、资源齐备、完整链接或运行验收；
不构建、不新增或执行测试、不运行客户端，不用 subAgent。

## 当前落实

[LoadingScreen.cpp](../src/platform/LoadingScreen.cpp) 已实现完整入口，
CMake startup 已登记，InitializeClient 原声明及调用解析到该定义。
34 项阶段覆盖、三种出口、描述符写序和公共尾已逐项静态核对；
提示源码加终止 NUL 与上述 21B 一致。相关八文件严格 UTF-8 无 BOM/LF、
相对文件链接及限定差异检查通过；没有编译、测试或运行结论。
