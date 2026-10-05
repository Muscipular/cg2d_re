# YLC01 Yobi 摘链与回收取样 SPEC

<a id="yobi-cleanup-sampling-baseline"></a>
## 原端基线与具体缺口

沿[ACP01实体/附件生产](cg2d_actor_coordinate_producer_spec.md#actor-coordinate-producer-source-state)和既有[Yobi生命周期合同](cg2d_yobi_runtime.md)继续，不从“标死”推定立即释放或清所有附件。

原会话只读核三完整根，各一chunk：
| 原根/完整区间 | 指令/字节 | 原入向 |
|---|---:|---|
| 404C50=[404C50,404CF7) | 60/167B | 23 type17 call，5父 |
| 404D10=[404D10,404D30) | 13/32B | 1 type17 call，父404C50@404CCC |
| 519D50=[519D50,519E45) | 87/245B | 7 type17 call，6父 |

[三根160条444B](cg2d_yobi_cleanup_sampling_evidence.json)共31直接call/10不同父。完整指令保存只支持本组取样/回收及已有玩家清理保持合同；全部父实参、callback/资源生产和完整场景寿命不随三根保存判验收。

具体源码差异：
1. 404CB9先取current.prev，404CBB取current.next，404CBF写前驱next；404CC2在这次写入之后重读current.next，404CC5再重读current.prev，404CC7写后继prev。现两条复合赋值按C++先右值、后左值求值，第一次先next后prev、第二次先prev后next，均与原取读相反。
2. 404CD8取S32阶段计数，404CDE加1、404CE1按局部结果与4作有符号比较、404CE4发布局部结果；达到4时404CEC再发布0，但完整返回仍是加1结果。源现自增发布后重新读取共享计数来判断，未保持原单次读/局部判定。
3. 404D15仅一次取action.payload，404D18判非空；非空才把该快照传free，然后无条件free主action。源现条件及free实参分别取action.payload，额外复读。
4. 519D50已有八槽/文本、玩家指针重读和尾返回合同正确，不改其源码；补真实分工、身份和来源。八槽顺序04→08→0C→10→2C→4C→30→44，逐槽非空MarkDeath后清槽；不清34/38、不free主/payload。随后text1C非空free再清，返回主入口后重读玩家槽MarkDeath并清玩家槽；初玩家空时全部跳过，仅走51A5B0→51BEC0及非零时51BE50(0)。

<a id="yobi-cleanup-sampling-decision"></a>
## 工程裁决

依AGENTS.md第8条委托实施三个具名源片段：
- UpdateAll摘链显式previous→firstNext读取后发布previous.next；发布后重新取currentNext→currentPrevious，再发布currentNext.prev。不得把前/后指针在两次写入前统一缓存，也不交换两个链接写入。两次链接发布后仍独立读取current.next到释放后继续遍历的next，然后Delete当前节点。
- 末端用正常S32 result=计数+1、resetPhase=result>=4，先比较局部结果再发布result；满足时再写0，完整返回result。保原正常1/2/3/4返回与0/1/2/3计数循环，不把返回改为清零后的槽值、布尔或低字节；不添加输入限制/回绕模拟/溢出保护。
- Delete保存一次void* payload，非空free快照，再free实际action。payload为通用异构载荷的已有类型擦除字段，不新建owner/影子表/句柄或改布局。它不负责附件MarkDeath、清聊天或摘链；不新增action空检查或提前清payload。
- 519D50/ClearMapActorAttachments及MarkDeath源码保持；原玩家空/载荷空分支、八槽跳空与先标死后清、文本/玩家槽释放和原尾低S8返回保持。局部清理不据此扩成“清所有孩子”或改变34/38挂件责任。
- 两个四相门禁及flags40000、模式1和非零计数的原短路读取、空callback跳过、callback后current.next重取、唯一head/end和原生prev/next、所有其它源码/头/汇编不改。沿已有应用calloc/free域，不新引入原私有堆或异常保护。

<a id="yobi-cleanup-sampling-acceptance"></a>
## 可度量验收

1. 三完整160条444B、31type17call/10父、库存/chunk/真实404C50→404D10及519D50分工有证；保存与局部验收区分，不称全部caller/callback/资源/场景寿命已验。
2. 源全文仅精确重放三片段，两个摘链写之间prev/next按原重取，释放后用保存next；计数一次取读/先局部判定再发布/可选清零但返回完整result；payload一次取读/条件释放后主释放。
3. 原八槽/文本清理、玩家槽重读/清零、尾真实依赖和返回等未变；声明、非拥有原生指针、通用payload类型和宽度保持。全部其它源/头/汇编、AAS01/ACP01保持。
4. 新增519D50一语义；两既有UpdateAll/Delete记录仅追加源状态/证据，保全部旧字段。三adapted及MAP/LOOP同三ID纳管，四局部I/S共用同一源修正/原证据，不多计函数。
5. G01/G02/U11/R01/R04/V04仍open、所有完整门禁/功能/R1/R7/R8及整体保持未闭；仅获准文本汇总/-Check、新引用/UTF-8无BOM/LF/精确提交，不能代编译/运行。

<a id="yobi-cleanup-sampling-source-state"></a>
## 源状态与剩余

真实来源为YobiRuntime.cpp中的YobiUpdateAll_404C50/DeleteYobiAction_404D10及未改ClearMapActorAttachments/MarkDeath；519D50实际入口在world/SceneState.cpp，复用同一gPlayerRuntime.action，现调用关系保持。共用函数在地图/普通场景已有真实调用，不把跨域同ID当不同原函数。

本组静态结果归档唯一进度与登记簿。全部原语义/完整父与callback/callee、资源与数据生产、所有链表别名/owner重进退出/借用失效、实际ABI/SDK/时钟RNG/PC53/完整场景画面运行仍待。异常不保护、不模拟/称等价；音频暂停、原格式明文和不构建/测试/运行/摘要限制保持。固定快照`cg2d-re-yobi-cleanup-sampling-20261005`。
