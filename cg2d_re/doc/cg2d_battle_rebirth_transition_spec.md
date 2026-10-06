# 战斗图像切换、淡入淡出与恢复 SPEC（BPU01）

2026-10-07；推进动作163/164的完整进入/退出状态链。原事实以已打开IDA只读会话为准，工程委托有效，先记方案后实施。

<a id="battle-rebirth-transition-baseline"></a>
## 完整原根与实际差异

| 原根 | 完整本体 | 实际出向/分支/返回 | 唯一入向 |
|---|---|---|---|
| 4113A0..411A46 | 417条1702B/六页 | 24call/47普通jump/4ret | 4053D6 |
| 411A50..411CA8 | 153条600B/两页 | 8call/16普通jump/3ret | 4053E1 |

合570条2302B/八页、32call/八被调入口、63普通内部jump/七ret；两普通点同父404DB0，只六条压action/弃返/公共尾邻窗。[完整证据](cg2d_battle_rebirth_transition_evidence.json)保所有实际指令和五源常量组的七原栈初始化组，不把代码地址名称误当全局数据表。

原411487是MOV AL BYTE读取位置，41148B INC AL后落S8 type；源先读整个S32再截低U8，恢复直接unsigned byte对象表示读取，原W8增量/存储保。原八效果回收指针点都先取指针供判断/MarkDeath（两个gear槽在源同一循环），源重复字段/slot表达式，改对应七源取样片段。合七模式/八片段，只源读阶段与宽度恢复，正常旧值一致，不冒称数值/并发或生成机器错误。

原持久图16、进入声4/图8、退出声4/图8均为指令即时初始化栈值；164重复退出声4/图8。共七原组52元素映射源五组40元素，源数组及其容量保持，原栈空间复用不合并为虚构全局物理容量，不新增R7表归属。

<a id="battle-rebirth-transition-decision"></a>
## 实施裁决与完整正常合同

动作163：fresh effects[initialEffect].flags W32 bit80000决定进入/退出。进入FD4=0按原direction分支选side、当前位置声425/427/429/431→308=8/30F=255→device查询无设备时W32 flags|8000→fresh index/FD4=1→按四类型两图选择419C10→106C发布/返回非空才309=4后fresh槽310=4→character6/PlayFrame1。FD4=1若imageNo==原initialEffect.position4，则106C空或phase非0才FD4=2、PlayFrame0；不相等时alpha W8减7，降到0后保13B0门禁/baseImage、fresh当前图发布及原各W8draw字段/flags10000000和PlayFrame0，否则character6/PlayFrame1。FD4=2同阶段106C指针条件回收→fresh index/清106C→原position低W8加1落S8 type，按四类型四图依序419CF0的偏移2/1/-1/-2及相应槽，非零图创建；第四创建仍按第三槽非空判定，不改为本次返回判定。各创建309=4/fresh槽310=4后，fresh当前图/flags与字段发布、PlayFrame0→439470弃返→439340→frameEvent0/FD40。所有进入阶段/default共同按13B8/13BC两借用槽条件MarkDeath并仅非空时清槽。退出先character6/PlayFrame1再fresh FD4：1时alpha+7未超过255只W8发布，超过时条件回收106C/清槽、flags清10000和字段/type0、439470→439340→frameEvent/FD4清0；0时S8 type>0才播426/428/430/432、条件回收主效果并清、三辅助槽逐个取指针/条件MarkDeath后均清槽、按原side/fresh当前图与W8字段/flags发布→PlayFrame1→device无设备设10000→fresh S8 type>0才419C10退出图→发布106C并按非空写309和fresh槽310→FD4=1。其它退出FD4返回。

动作164：先character0/PlayFrame1再fresh S32 stage。stage1/2时W8 alpha+7未超过255只发布并返回；超过时仅stage1无额外空门禁MarkDeath(106C)，然后清106C/flags清10000、3080/30F255、fresh保存phaseFF0→type0→phase恢复→FF0/frameEvent/FD4清0。其它非0 stage返回。stage0沿S8 type正才播退出声、主效果/三辅助槽按原条件回收清除→side与baseImage发布、3088/30F0/3090、flags清10000000/13B00→PlayFrame1→device无设备设10000→fresh S8 type正才退出图419C10并写106C，之后按非空写309/fresh槽310，FD4=1。负direction按原分支side1、0..2或>=6为side0、3..5为side1；现有方向表达式保持。

两个源本体只在既有类型读取/效果回收阶段取样，不把指针缓存提前跨音效/播放/创建/MarkDeath；三辅助槽每迭代重新取其当前指针，gear两槽各迭代取样且空槽原本不写。stage1的164原无空门禁回收保持。309写后原再次取对应槽写310，不缓存该真实重读；第四持久创建原按第三槽判定保持，不用新保护更改分支。

S32位置字段和所有写者保持；读取低W8后实际INC/落S8由已有U8加1/存储承载，不新造S32溢出或异常模型。原alpha业务门槛、负方向side、S8 type正判定和全部正确状态发布/创建偏移/返回消费保持。仅内存图像状态，网络原格式明文和音频暂停继续。

<a id="battle-rebirth-transition-acceptance"></a>
## 限定验收

- 两根全部570条2302B/八页、32call/八入口、63普通内部jump/七ret与两唯一父六条邻窗可追；七原栈初始化组52即时值逐项对五源常量组，无独立原表地址/新R7。
- 整个YobiRuntime只两个真实定义内七模式/八片段；一W8类型消费、四主指针取样/两个三辅助槽循环/一个gear循环恢复，字段及其余源码/常量/第三槽判定/真实重读保持。
- 新增1438/1439两明确原身份及正常equivalent；语义1438→1440/有效1433→1435、关联1551→1553/3680（42.20%）、BATTLE115→117仍枚举open、分类563→565、E740→742只两局部I/S。R7 259/3061/余2802、差队列172/待归属43与全部完整门禁保。
- U08/R01/R04/V04只当前/引用仍open，原60任务关闭条件保。两完整原根570条2302B/八页、32实际call/63普通内部jump/七ret、两个普通唯一入向同一404DB0父六条邻窗，七组原指令即时构造的52栈表元素对应源五组40常量，只当前完整正常源/状态发布/字段/调用及同阶段回收静态。七替换模式共八源片段，恢复一实际低W8类型消费与七源回收取样（gear一循环承两个原点），正常旧值无介入相同、不称数值/并发或生成机器bug。正确原第三槽判断、全部case/default/负方向、字段W32/W8/S8、真实创建与alias借用保；不造同名原全局数据表或新增R7处置。全父/全部子依赖/其它生产与间接alias-owner寿命、SDK/生成机器ABI/RNG/时钟/画面运行未新验；音频暂停和原格式明文、RPLAN.24实际W8存储且不专项异常/保护/模拟/故障等价保。

<a id="battle-rebirth-transition-source-state"></a>
## 当前交付

两完整图像切换/恢复源与正常状态链纳管，低W8类型读取和原回收同一指针取样恢复；快照 `cg2d-re-battle-rebirth-transition-sampling-20261007`。未编译/调试/测试/运行/摘要，完整战斗/生命周期/实际画面及R8未关闭。
