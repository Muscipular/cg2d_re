# ATP01 动画载荷具名指针与清理身份

<a id="animation-pointer-baseline"></a>
## 已证载荷与缺口

[动画合同](cg2d_animation_runtime.md)已有装载、坐标和清理本体。主记录的listNode/actionList/coordinates以及主动作frames均有已证载荷，当前仍用void*；P4清理借用主表ActionEntry字段布局，也容易把未知前缀/帧误当已知动作格式。本步按[原生指针规则/工程委托](../AGENTS.md)收口类型合同，不重造已存在的业务实现。

[原证据](cg2d_animation_pointer_evidence.json)保存四体565条1786B、七直接call身份，分属六父：

| 原根 | 指令/字节 | 本步边界 |
|---|---|---|
| 45B760 AnimeLoadAction | 293/959B | 已有生产者，三具名分配之一及帧分配承接 |
| 45C150 CoordInit | 116/375B | 已有坐标生产者，具名分配承接 |
| 45B440 主表清理 | 78/226B | 本体不改，补稳定原身份/真实来源 |
| 45B650 P4清理 | 78/226B | 只换已证子项类型，不改free/重取/清空序 |

原主表+10在45B84C发布实际MainSlot，复用路径45B78B→45B790消费该槽；+18在45B82D发布动作表，子项原步长28B，+18在45B9DC发布帧；帧磁盘10B/运行12B及坐标8B沿既有合同。45C22E发布主表+1C坐标，45B440先逐帧free/重读清槽，再释放动作表/清+18/+10，最后释放坐标/清+1C/槽index=−1。原字段和失败门禁不因类型更新变动。

P4的45B650同样仅证明子项+18的可释放指针、旧步长28B；前24B字段及帧载荷生产/格式未证。不能将主动作direction/duration/frameCount等语义移给P4，也不把P4缓冲或9A4A54清理槽猜作坐标表。

<a id="animation-pointer-decision"></a>
## 裁决与实际开发

1. 在既有AnimationRuntime.hpp前置声明私有MainSlot/ActionEntry/CoordinateEntry/CacheActionEntry。AnimateRecord三指针改对应具名T*，ActionEntry.frames改FrameEntry*；不公开载荷定义、不加新接口或持有副本。
2. P4 actionList改CacheActionEntry*。同一私有头新增该已证承载：24B原字节前缀加原生void* frames，大小32B、frames+18；只暴露清理已证引用。未知帧载荷、P4.buffer及9A4A54继续void*，类型擦除有明确未知边界。
3. AnimeLoadAction/CoordInit三个malloc结果以对应static_cast承接，大小、次数、发布及失败/清零顺序完全保持。45B650两个读取/释放后重取改用CacheActionEntry；其它清理/初始化/播放/排序正文和已有合法同型cast不改。
4. 主表56B/P4表48B、主动作和P4子项32B、帧12B/坐标8B及所有既有offset断言保持；只改类型不改标量/容量。P4新增断言是源码要求，未执行编译。
5. 复用/领槽、count=0早退、计数门槛/2000B共享缓冲、短读/失败清理、28/12/8的原逻辑字节累计、活动计数和真实Yobi消费保持。指针承载不创建或解决未知owner/alias/寿命，也不新增保护/拒绝/默认值或异常模拟。
6. 两清理入口各新增唯一语义，两个既有生产语义补类型与清理依据；四根均以x64载荷承载adapted登记正常合同，不宣称原机器ABI/故障等价。未知P4生产继续开放，不按只读/未见writer推恒空。

<a id="animation-pointer-acceptance"></a>
## 限定验收

- 四文件十三源片段精确重放；具名五指针/三malloc承接和P4独立子项已落码，所有其它源/头及业务表达式不变。
- 四体565条1786B及原库存/七call六父身份可追，仅载荷写入/消费/清理局部静态；不重新验两个生产者全部语义、完整调用父、资源内容/SDK/实际运行。
- 语义1263→1265/有效ID1259→1261，处置354→358；ANIMATION4→8明确成员。E198→201只本次ANIMATION I/W/S局部，I是类型实修，W仅具名指针/原局部连接，不能记所有owner寿命通过。
- G01/U09/R01/R04保持open；完整初始化/加载/播放/场景/退出、P4前缀/帧/缓冲生产、间接alias/借用失效和所有owner寿命、实际分配失败/机器ABI/SDK/像素运行未验。R1/R7/R8及整体SPEC未闭，RPLAN.24异常不专项保护或模拟/称等价。

<a id="animation-pointer-source-state"></a>
## 当前交付

五个已证载荷引用改具名指针，P4子项与主动作明确分开、未知前缀/帧原字节保持；三分配结果和两清理读取完成类型承接。清理稳定身份与生产合同纳管，真实加载/播放/清理流程沿既有源保持。

固定命名快照：`cg2d-re-animation-pointer-contract-20261005`。
