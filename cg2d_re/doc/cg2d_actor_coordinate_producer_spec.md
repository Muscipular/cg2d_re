# ACP01 实体与挂件坐标生产 SPEC

<a id="actor-coordinate-producer-baseline"></a>
## 原端基线与缺口

沿[AAS01实体附件链](cg2d_actor_attachment_sampling_spec.md#actor-attachment-sampling-source-state)的真实生产依赖推进。既有[Yobi创建合同](cg2d_yobi_runtime.md)已区分441DB0和444000的字段发布顺序；本轮聚焦临时载体取样，不扩通用内存数值载体算法。

原会话只读确认两完整根：441DB0=[441DB0,441F53)，109条419B，七个type17调用七父；444000=[444000,44419A)，103条410B，五个type17调用同一442F70父。合计212条829B、12直接call、8个不同父，每根一chunk，见[原指令与源码差异](cg2d_actor_coordinate_producer_evidence.json)。全部父实参/正文不随两叶取证重新验收。444000的首附件两种图号分支在新端共一个条件图号调用表达式，其余三点分别调用：五个原call对应四个现表达式，不能少记原点或多计原函数。

每根四次分别ConstructFromInt_401CC0→复制临时字段→Destroy_401CF0，不复制vft/reserved，不合并RNG消费：
- 441DB0：各次先读临时U8 readCount，再读U32 encoded、mask；三值全部取齐后才按readCount→encoded→mask写同一实际目标。目标顺序mapX200、mapY210、清field270低W16、mapX1E0、mapY1F0。源现局部lambda交替读写三字段，未保存原三个取样值。
- 444000：各次先读临时U32 encoded、mask，再读U8 readCount；三值全部取齐后才按encoded→mask→readCount写同一目标。目标顺序mapX200、mapY210、mapX1E0、mapY1F0。源现调用通用AssignFromInt/CopyPayload，发布序正确但临时字段交替读写。

这些字段承载原内存数值载体，不是网络封包加解密；网络仍原格式明文，既定两网络叶排除不扩展。

<a id="actor-coordinate-producer-decision"></a>
## 工程裁决与实施边界

依AGENTS.md第8条委托裁决：
1. 仅两个源片段。441DB0既有局部lambda增加U8 readCount、U32 encoded和U32 mask快照，再依原顺序发布；444000将四次通用赋值调用改为本入口局部lambda，先U32 encoded/mask与U8 readCount取齐，再发布。四目标调用保持原序，不外提共用helper，也不改通用CopyPayload/AssignFromInt或其他同形创建入口。
2. 临时构造/析构仍各四次，依次消费真实RNG；目标仍现有原生LegacyCryptInt字段，只写三载荷字段，vft、reserved保持。readCount仍U8；encoded/mask仍W32位型，不撤为S32算术，不拓宽非地址字段。
3. 441DB0保NewYobiAction(2,sizeof(MapActorPayload))、分配失败立即空返回；方向→真实4417E0回调→图号→动作0→priority10→flags34，以及四目标间的field270低W16清、移动/phase清零、世界/屏幕投影和同一原生指针返回保持。不擅补payload.index或未知附件初值，不发布玩家全局槽。
4. 444000保NewYobiAction(2,0)、失败立即空返回；图号→动作→方向→低W8优先级→flags20、四坐标生产、移动清零、F32世界坐标/真实46CE30投影及同一原生指针返回。不设置callback/completionSlot，不借444260代替或额外清路径数。
5. 原X乘64先计算再移动分量清零，世界X后世界Y、两F64+0.5/S64截断和低S32屏幕写序保持；本次不新增数值叶、保护/回绕/故障模拟或异常等价门禁，不外推为完整数值/场景验收。

<a id="actor-coordinate-producer-acceptance"></a>
## 可度量验收

- 212条829B按真实完整边界保存，两个ID/入口/chunk和12个type17引用/8父可追；不把静态记录当所有父实参或寿命验收。
- 本文件全文只按两个具名片段从提交前精确重放；各局部lambda先三源快照、再三目标发布，四个真实目标及441DB0中间低W16清保持。构析/RNG四次、失败/真实回调/投影/指针返回与原生宽度保持；其它源/头/汇编及AAS01不变。
- 两语义/来源/adapted新增，MAP/LOOP各同两ID，不重复计函数；四局部I/S共用同一源修正/原证据，全部完整门禁不提升。
- 六任务G01/G02/U11/R01/R04/V04仍open；获准静态文本生成/-Check、相关文件UTF-8无BOM/LF/链接/精确提交通过，不代编译/运行。

<a id="actor-coordinate-producer-source-state"></a>
## 源状态与剩余

真实来源为YobiRuntime.cpp的Sub_441DB0/Sub_444000和既有include/cg/YobiRuntime.hpp声明；局部lambda不是新的原函数ID。441DB0的七父与444000的真实442F70消费、AAS01骑乘/附件回调关系保留。

静态结果写唯一进度和登记簿。两完整根的其它原语义/全部父及callee、原生对象/附件owner和借用寿命、上游所有数据生产/重进退出、真实ABI/SDK/CPU浮点/RNG/画面运行仍未闭。音频继续暂停，原格式明文及不构建/测试/运行/摘要限制保持。固定快照`cg2d-re-actor-coordinate-producer-20261005`。
