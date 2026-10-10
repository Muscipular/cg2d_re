# 世界对象记录生产与更新 SPEC（WOP01）

2026-10-10。沿MOP01/WOL01/WLR01核三个完整记录生产入口：无action记录445A60、类别2更新445D20、类别4更新445F40。当前真实接线已在，只局部修复六次CryptInt三成员的先取样后发布，并明确三个原文本缓冲身份；不改变共享CryptInt工具或其它生产者。

<a id="world-object-record-producers-baseline"></a>
## 完整原根与接线

| 原身份 / 当前符号 | 完整范围（尾不含） | 指令 / 字节 / E8 | 实际入向CALL |
|---|---|---|---|
| F:00045A60 / Sub_445A60 | 445A60–445D1E | 206 / 702B / 13 | 50E036，父50DD80 |
| F:00045D20 / Sub_445D20 | 445D20–445F3E | 160 / 542B / 7 | 50BB16，父50B380 |
| F:00045F40 / Sub_445F40 | 445F40–44612E | 150 / 494B / 7 | 50BCA7，父50B380 |

合516条1738B/十页、27 E8/九callee、22跳/五RET，无switch。三入向/两父只核每CALL前24条、后12条指令短窗，不称完整父或将跨分支指令当动态后继。445A60返回0/1实际参与父分支，另两入口父弃尾EAX，保现void观察边界，不把IDA推断返回当新API。当前消费者为NetworkRuntime.cpp带记录路径及SceneState.cpp类别2/4分流。完整指令、短窗、前后源码和差集见[证据](cg2d_world_object_record_producers_evidence.json)。

九callee均绑定当前真实实现；444FA0/446130/446210/4461E0、441DB0、52CED0复用六已有身份，401CC0/401CF0/52D2F0没有独立语义登记，保当前源码依赖边界，不伪称本步完整叶取证或新增已验身份。原两Construct/Destroy、读取/通知及RNG消费保持。

<a id="world-object-record-producers-decision"></a>
## 工程裁决与六组读取

依子AGENTS第8条先记本SPEC，再局部修复。每次从临时量先读完三个成员，再按同序写目标：

| 根 / 原段 | 临时成员读取及目标发布顺序 | 目标 |
|---|---|---|
| 445A60 / 445B0F–445B2B | readCount W8、encoded W32、mask W32 | coordinate0C |
| 445A60 / 445B47–445B63 | mask W32、encoded W32、readCount W8 | coordinate1C |
| 445D20 / 445DD9–445DF5 | mask W32、encoded W32、readCount W8 | coordinate0C |
| 445D20 / 445E0A–445E26 | mask W32、encoded W32、readCount W8 | coordinate1C |
| 445F40 / 445FF8–446014 | encoded W32、mask W32、readCount W8 | coordinate0C |
| 445F40 / 446029–446045 | encoded W32、mask W32、readCount W8 | coordinate1C |

每入口仍按mapX/coordinate0C，再mapY/coordinate1C构造、发布和销毁。仅持样三个既有非地址字段，不复制token/reserved或整结构。445D20既有局部lambda保留，445F40两次AssignFromInt改为本入口局部lambda，原共享CopyPayload/AssignFromInt及全部其它消费者保持；两个lambda各服务两个原展开点，无新公开函数/原根或API。六组指执行点，不按共用源码表达式数漏计。原DB gameX/gameY名称不独立作换轴依据。

## 无action记录445A60

先Find，已存在返0；再TakeEmpty，负返0。成功依次action空、used W16=2、category取446210低W16、编号/image W32、value40 W16、level W32，再两坐标、方向；不初始化value34或增加领取回滚。flag1/flag2非零分别仅flags42低BYTE OR40/80，高BYTE保持。

name原16门槛、含NUL原字节复制保持；titles按序解析1/2/3、每次容量255。三个未初始化256B独立缓冲明确为text100/text200/text300，对应原var_100/var_200/var_300：ordinal1→text100/title65；ordinal2→text300，Unescape后32门槛→text76；ordinal3→text200，Unescape后32门槛→text97。标题ordinal1无额外Unescape，失败或过长不补零/清旧字段。最后4461E0完整W32写flags114，返1；不创建action、改帧或重解析其它记录。

## 类别2/4记录更新445D20、445F40

Find命中直接更新，不补建已存在记录的空action；未命中才TakeEmpty与441DB0(image,mapX,mapY,direction)。先发布创建返回至record.action，空结束，保领取副作用，无回滚。成功used W16=2、fresh action.payload.index、category分别2/4、编号及value34=-1，再进入共同更新。

两入口分别先imageNo W32与value48/value4C，再两坐标、方向、nameC9原60门槛、flags114分别400h/800h、value118=0；类别2再value50。借action判空，非空先写action.imageNo。名字分别28/16门槛，成功重取action.nameC4目标后再取第二strlen；过长用原"???"含NUL四字节，不增门槛或替代值。末尾fresh action OR原flags；类别2再fresh action.field1D8=value48、fresh payload.value18=value50，类别4不补这些字段。全部既有正常/失败与字段宽度、借用/写序保持。

<a id="world-object-record-producers-acceptance"></a>
## 限定静态验收

核三完整连续机器码/27E8/九callee/22跳五RET、三实际入向及短窗；六组成员宽度、全部读取在首发布前、两轴构造/销毁与原RNG次数、两种局部lambda展开及三个文本缓冲身份，整两CPP/一头按唯一计划差异精确重放。其它原字段/结构/存储、共享CryptInt工具、原生产者、两个当前父与六旧语义身份保持。

三新语义/F、MAP/YOBI各三成员、TEXT一成员及三限定S，零I；七任务仍open，完整gates/全部member枚举、184叶验收0、60任务关闭11与六closure开放、原政策不变。两完整父/传递callee/完整输入与生产/正常失败/容量/alias/owner/SDK/机器ABI/画面声音与运行仍待；不以局部取样修复称整体生产闭环。

RPLAN.24不专项处理或模拟异常、越界、溢出，不加保护、输入限制、默认值、补零、钳制或回滚。保原文本字节/协议与现明文接线、WAV-only、SDL和MSVC直接CRT及数学批准边界。只读当前IDA，不重开/改库，不构建/测试/运行客户端或算法/检查产物/摘要，不引依赖；总SPEC§10不降低。

<a id="world-object-record-producers-source-state"></a>
## 当前状态

本步前Git提交说明“[#] 对齐世界对象删除清理与空引用恢复合同”，完成提交说明“[#] 对齐世界对象记录生产与坐标取样”。统一cg2d_re分支、不打tag，先记SPEC再按工程委托修六组读取并明确三文本缓冲；核心静态、登记、文本汇总、Check及派生范围核对通过。语义2198=2150F+48X，文档关联2260/3680=61.41%仅关联率，F处置1629/证据1432，MAP149/Yobi79/TEXT30枚举及验收仍false；派生仅六metrics/三F/三功能/七任务变化，184叶验收0、60任务关闭11、六closure开放及全部完整gates/其它字段守恒。外部七项及删除暂存保留排除，整体目标active/未完成。
