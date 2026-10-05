# AAS01 实体附件槽位取样 SPEC

<a id="actor-attachment-sampling-baseline"></a>
## 原端基线与具体缺口

范围为实体绘制链中已实现的附件取样；上游见[Yobi实体阶段/附加绘制合同](cg2d_yobi_runtime.md)。本轮沿真实依赖，原会话只读确认：
- 4417E0全体[4417E0,441DAB)，466条1483B，一chunk；唯一441DB0@441DEA type1是回调地址写入，不是直接call。
- 442F70全体元数据[442F70,443FF1)，1220条，一chunk；唯一4417E0@441D9B type17直接call。本轮只保存入口[442F70,442FC9)24条89B和首附件[443230,4433C6)119条406B，共143条495B；不以两个局部窗声称其余1077条复验。
- 43E490仅保存四个依赖窗：[43E66D,43E6B0)26条67B、[43E708,43E74E)27条70B、[43EBA1,43EBE4)26条67B、[43EC3C,43EC82)27条70B，共106条274B；不登记为该完整原根的基线或验收。

[本组原指令与源码差异](cg2d_actor_attachment_sampling_evidence.json)共715条2252B。完整保存4417E0只支持本组调用/取样关系，不代全部原语义、数值叶、callee、producer或运行验收。

现源码共用DrawActorPhaseAttachment把五条路径合为“Y相加后重读X子槽、X相加后重读图形子槽”；原五条路径实际有四种取样时点。两偏移查询均可能访问真实共享图形状态，应保留逐次借用和已证读取点，不能把初次指针贯穿到全部后续读取。442F70入口先读payload、再读frameX/Y、反复读骑乘槽；原先frameX→payload→frameY→骑乘指针→index，同一骑乘指针供两个帧偏移。首附件四次写入用复合赋值，未固定原目标子槽和主字段的先后；绘后偏移阶段也未保存当次子指针。

<a id="actor-attachment-sampling-decision"></a>
## 工程裁决

依AGENTS.md第8条委托采用：在既有私有共用函数增加四值ActorAttachmentReadOrder，所有调用点显式选择已证顺序。此枚举仅固定局部读序，不增加业务分支、公开接口、owner、保护或新依赖。原生YobiAction*及GraphicDescriptor16*均为借用，初次子对象的描述符在本次绘制内保持，后续槽取样不能改用新子对象的描述符。

每条路径：先初次子槽→descriptor→5182B0轴1→主Y；依下表在Y加法之前/之后取当次X查询子槽；调用5182B0轴0后取主X，再依表在X加法之前/之后取当次图形子槽；最后取该子graphicIndex，低S16屏幕坐标入46C060。每个后续槽只取一次；不缓存完整绘制周期或统一五路径。

| 原路径/入队点 | X查询子槽取样 | 图形子槽取样 | 选择值 | before |
|---|---|---|---|---:|
| 阶段1骑乘43E6AB | Y相加后，43E68D | X相加后，43E6A0 | AfterYAndXAddition | 0 |
| 阶段1效果43E746 | Y相加前，43E728 | X相加前，43E736 | BeforeYAndXAddition | 1 |
| 阶段2骑乘43EBDF | Y相加前，43EBBB | X相加后，43EBD5 | BeforeYAddition | 0 |
| 阶段2效果43EC7A | Y相加前，43EC56 | X相加后，43EC70 | BeforeYAddition | 1 |
| 普通骑乘441C03 | Y相加后，441BE5 | X相加前，441BF3 | BeforeXAddition | 0 |

DrawActorPhaseEffect保留两阶段实际初次创建/空值路径和FrameMove，只透传本阶段的明确枚举；图号仍阶段1的1AE10、阶段2的1AE11，不从图号猜读序。阶段1/2的其它动画、恢复、清槽、音效和数值不改。

442F70：
1. 保留原442F7E/442F85空action早退。非空路径按frameX→payload→frameY→initialRide→stateIndex读取；骑乘非空时只用initialRide读取其frameX、frameY。快照仍只供action30尾绘制。
2. 首附件创建条件、kind取样/刷新清零、Y→X CryptInt读取、返回发布及本次跳过绘制保持；原MarkDeath→清槽也保持，不新增失败回退/释放旧对象。
3. 仅绘制分支把最后一次非空判断的子指针保存为initialChild，随后取主X并写入initialChild.x；后续三次各先重读目标槽，再取主Y、worldX、worldY并依序写入。然后重新取动画子槽交FrameMove。
4. FrameMove后取主图号→高度查询→当次Y子槽→轴1查询→主Y→高度加偏移→当次X子槽→加主Y→轴0查询→主X→当次图形子槽→X相加→图形索引。仍before1、descriptor空、batch0入队，坐标保低W16的S16解释。
5. 首附件分支之外的条数值、聊天截止U32、文字长度/索引别名、名称十原格式点、状态图标、两个挂件和action30尾部保持；不复查/改写这些既有正文或扩展范围。

<a id="actor-attachment-sampling-acceptance"></a>
## 可度量验收

本组10个具名源码片段：共用主体1、效果透传2、两个阶段选择4、普通骑乘选择1、附加入口/首附件2。必须静态证明：
- 715条2252B按真实指令边界连续保存；原4417E0全体及442F70元数据、两入向引用类型不混淆，四43E490窗不冒充完整根。
- 五调用路径均选择表中对应顺序；每路径3次子槽读取、两真实偏移查询Y→X、主Y/X读点、初次descriptor/当次graphicIndex、before/batch及S16窄化保持。效果透传不根据图号猜顺序，不改变动画次数或失败处理。
- 442F70原空值返回、创建/死亡分支、首附件逐次指针读取与同步、初骑乘同指针帧快照；本文件全文仅精确重放10片段，所有其它源码/头/汇编无改动。
- 语义4417E0新增一条；既有442F70只追加本组证据/来源和边界，保留名称10点/一源码表达式及原记录。两ID为adapted、与MAP/LOOP同ID关联；四局部I/S共用一份源修正/原证据。
- 六open任务、全部完整门禁/叶功能、R1/R7/R8和整体未验收保持；生成器仅获准文本汇总，生成及-Check通过不代编译/运行。相关文件UTF-8无BOM/LF、引用及精确提交须通过。

<a id="actor-attachment-sampling-source-state"></a>
## 源状态与剩余

真实来源为YobiRuntime.cpp中的DrawActorPhaseAttachment、DrawActorPhaseEffect、UpdateActorMountPhase、UpdateActorDismountPhase、Sub_4417E0及Sub_442F70；现441DB0回调绑定和4417E0末端Sub_442F70调用保持。新增枚举不成为原函数ID；43E490四局部窗不提升该完整根的分类/门禁。

本组静态结果归档到唯一进度和验收登记簿。整条实体场景、全部原语义/父与callee、上游生产、重进退出与借用失效、真实ABI/SDK/CPU浮点环境/时钟RNG/画面运行仍未闭合。异常不新增保护、回绕或故障模拟，不称等价；音频暂停、原格式明文及不构建/测试/运行/摘要限制保持。固定源快照`cg2d-re-actor-attachment-sampling-20261005`。
