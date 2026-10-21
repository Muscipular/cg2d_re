# Yobi 模糊变量命名（YVN01）

日期：2026-10-21。用户要求继续分析并重命名valueXXXX等不明确名称；本项按当前源码读写链落实纯命名调整，不改变类型、布局、初始化、控制流或读取时序。

## 命名与证据

保留原偏移／地址后缀以便追踪历史证据。以下字段均核对当前源码全部直接引用；相同词形但属于world、UI等其他结构的成员不作全文替换。代码依据优先于名称猜测，不补推测业务含义。

| 所属类型／位置 | 旧名 | 新名 | 当前源码依据 |
| --- | --- | --- | --- |
| YobiAction | value4CBits | verticalVelocityBits4C | 上升特效／浮动图标按F32计算竖直速度并更新offset44；保留原U32位表示 |
| YobiAction | value54 | motionMagnitude54 | battle_base／battle_display按方向分解位移；environment同时用作速度和振幅，不能只叫moveSpeed |
| BattleActorPayload | valueFCC | actionFrameCountFCC | battle_actor／battle_attack／battle_cast／battle_status每回调递增并比较阶段阈值，原低字节读法保留 |
| 同上 | value1018 | soundId1018 | 动作80读取并传PlaySe；BM及技能链生产 |
| 同上 | value101C | soundId101C | 动作81读取并传PlaySe，独立音效槽 |
| 同上 | value1020 | soundId1020 | 动作82读取并传PlaySe，独立音效槽 |
| 同上 | value1024 | effectPlaybackState1024 | NewBattleEffect关联时置1，效果回调置2，等待动作消费；部分动作继续使用3／4，保持整数状态 |
| 同上 | value1034 | numberDisplayType1034 | 动作159／160／204传NewBattleNumber并按类型更新HP／FP；不是纯视觉开关 |
| 同上 | value1048 | skill98Mode1048 | 技能98模式写入；动作139判断12／13、141按图号偏移消费 |
| 同上 | value1050 | attackDebuffType1050 | 技能94读取M整数；命中链非零时以110074加类型生成减益 |
| 同上 | value13A4 | queuedTargetIndex13A4 | AdvanceBattleQueuedTarget递增，动作166读取queuedTargets1084游标 |
| 同上 | value13B0 | appearanceOverrideActive13B0 | 动作218记录图像替换，退出恢复baseImage后清零；独立于FDC变身／REB位 |
| 同上 | value13D8 | statusEffectMode13D8 | 状态消息写模式；动作141／143及NewBattleStatusEffect区分模式1／3 |
| 同上 | value13DC | statusEffectType13DC | 上述模式下的状态类型与图号选择，不限于六种减益 |
| 同上 | value13E0 | statusEffectImageIndex13E0 | 模式3、类型7时索引kBattleStatusImages，生产为技能等级余数 |
| BattleScenePayload | value00 | completedEntryCount00 | 攻击／命中特效完成递增；合击消息比较参与数量并作为顺序游标 |
| 同上 | value84 | itemMode84 | ITM消息模式0／6／7，保存和恢复gBattleMValue59FA10 |
| 同上 | value88 | skill98Mode88 | 技能98模式输入，保存和恢复gBattleMValue5A4A34 |
| 同上 | value8C | feedbackSourcePosition8C | 反射／累计回馈路径保存来源角色位置，恢复后用于图像／技能派发 |
| 同上 | value98 | feedbackEffectImage98 | 同路径保存首个附加图号，回馈效果消费 |
| 同上 | valueA0 | feedbackAlternateImageA0 | 初始备用图号传入技能19族／267的回馈路径；保持只恢复、不回存 |
| MapTransitionPayload | value02 | searchDirection02 | 行走候选方向与失败转向，保留Normalize调用和原宽度 |
| 同上 | value04 | previousStepDirection04 | 与本步方向比较后回存，用于下一步比较 |
| 同上 | value06 | directionChangeCount06 | 方向连续变化时递增，达到阈值转向后清零，保留有符号视图判断 |
| 同上 | value20 | orbitAngle20 | 过渡模式2／3传入角度三角函数，逐次增加6 |
| 同上 | value24 | orbitRadius24 | 三点圆周绘制乘正弦／余弦，模式2／3增减4 |
| MapActorPayload | value18 | itemCount18 | WorldActorDrawing地面物品路径发布；悬停绘制名称后的数量 |

同时统一四个对应全局名：gBattleMValue59FA10→gBattleMItemMode59FA10、gBattleMValue5A4A34→gBattleMSkill98Mode5A4A34、gBattleMValue5ABA90→gBattleMFeedbackSourcePosition5ABA90、gBattleMValue5A4A28→gBattleMFeedbackEffectImage5A4A28。只改其定义、声明与引用，不改保存恢复顺序。

同源局部变量／形参同步名称：BM读取value1018、技能结束路径value88、valueA0传递链及地图过渡value02。观察组value170改为travelDuration，其值存入readiness，SavePlayerObservationTravelSnapshot按原式乘60；不据此推断时间单位。公开函数参数顺序、类型及调用保持。

## 暂不命名的边界

按用户补充要求，以下保留字段的原因同时写入声明旁注释，避免只在文档中说明。

- YobiAction::value190／value1A0：当前仅见构造和复制，缺少业务消费依据。value1D4被附件位置、观察队列索引／轨迹计数及效果反向标志共用，不能统一叫索引或帧数。
- BattleActorPayload::valueFD0兼任等待倒计时、一次性启动状态及碰撞边界编号；valueFD4兼任等待帧与阶段；valueFF0兼任计数、延时与保存的phase。value1030兼任HP／FP数值和选图／模式参数；value1038兼任强制命中门禁和动作阶段。保留现存名称，不拆字段、不新增union或访问器。
- BattleScenePayload::value70既存角色位置又作累计技能的重复标记，不能命名为单纯的casterPosition。
- WindowOpeningPayload::value1C当前仅初始化／布局断言，窗口value14在不同接口里转交，map_actor的value188／value1D8仍承接未具名field1D8；本项不凭相邻偏移推断。
- animation、map_actor、map_render读取的其他value成员属于外部world／render数据结构，本项不改其定义或业务。既有历史SPEC和进度中旧名保留，使用本表对应新名。

## 验证边界

对修改前后逐词核对，只允许表内标识符及字段用途注释变化；核对声明、定义、调用与布局断言引用，无新名冲突、限定范围内旧名残留或字符串变化。显式UTF-8，保留各文件换行。仅做必要源码静态检查，不编译、不运行、不新增测试、不计算摘要。

## 实施结果

本项已完成27个字段、4个全局量及同源局部变量／形参的683处标识符替换，涉及30个源码／头文件；另两份源码仅补保留原因注释。10个保留原名的字段均在声明旁解释原因，共享全局量及透传参数也补充说明。

32个源码／头文件静态逐词核对通过：逆向映射新名称并剔除注释后，与修改前源码一致；其余标识符、类型、布局、常量、字符串、运算和控制流保持。旧唯一名称引用清理完成，换行及UTF-8保持；未编译、链接或运行验收。

## statusEffectType13DC 候选值分析（2026-10-21）

结论：该字段是按statusEffectMode13D8及当前动作解释的类型编号，不是位掩码，也不是全局唯一的状态枚举。以下数值均为十进制；只记录当前源码能够证明的语义。本次未修改运行源码。

### 普通减益与能力变化

| type | 动作141的普通减益路径（mode不为3） | 动作143的能力变化路径（mode=1） |
| --- | --- | --- |
| 1 | Poison，中毒；110075 | DefenseUp，防御上升；103018 |
| 2 | Sleep，睡眠；110076 | DefenseDown，防御下降；103019 |
| 3 | Stone，石化；110077 | AttackUp，攻击上升；103020 |
| 4 | Inebriety，醉酒；110078 | AttackDown，攻击下降；103021 |
| 5 | Confusion，混乱；110079 | AgilityUp，敏捷上升；103022 |
| 6 | Forget，遗忘；110080 | AgilityDown，敏捷下降；103023 |
| 7 | 不在六种普通减益范围内 | RecoveryUp，回复能力上升；请求103033，创建器置RecoveryUp位并改用103018显示 |

- 减益依据：battle_cast.cpp的UpdateBattleAction141_410E90及battle_effect.cpp的NewBattleDebuffEffect_41F660，图号、debuffType13D0与FDC异常位对应。ANR输入state=2会写mode=2；动作141实际检查的是mode!=3，不能将其消费者条件改写为mode==2。技能98和2002在此路径有独立选图，不直接使用13DC。
- 能力变化依据：UpdateBattleAction143_410FC0及NewBattleStatusEffect_41FE00；技能99～104分别生产1～6，技能114的独立路径生产7。服务器include/battle.h的BATTLE_PARAMETER_SKILL_DEFUP等宏也对应99～104／114，支持这组业务命名。battle_skill_status.cpp中99～104分支内部虽还有case114，但该case不满足外层条件，不能将它记为可达生产证据。
- mode=1、type=0：技能3001成功路径写入，先安排动作144清旧效果，再安排143；143不创建新状态图。候选名可用NoParameterEffect，不应外推为所有模式的None。
- mode=0、type=-1：ANR清理请求暂存值；动作142结束时又把debuffType13D0写回13DC并清mode。因此mode=0不保证type始终为-1或0，不能据此新增无效值过滤。

### 扩展模式（mode=3）

当前可确认如下生产与表现映射。没有可靠业务名称的值保留编号，不依据图号相近或技能顺序猜作护盾、反射、抗性等。图号相同也不代表业务效果相同。

| type | 客户端已证技能来源 | 图号／消费路径 |
| --- | --- | --- |
| 0 | 2006 | 111100，动作143 |
| 4 | 2010 | 111099，动作143 |
| 5 | 2012 | 111098，动作143 |
| 7 | 2019 | 按13E0索引103018／103022／103020，动作143；索引为(技能等级-1)%3，对应防御／敏捷／攻击上升的图像变体 |
| 8 | 2015 | 写入此类型，但该生产路径不安排143；143本身也无此case |
| 9 | 2014 | 111152，动作143 |
| 10 | 2027 | 写入此类型，但该生产路径不安排143；143本身也无此case |
| 11 | 2058／2059 | 98254，动作143 |
| 12 | 2060／2061 | 98252，动作143 |
| 13 | 2062／2063 | 98257，动作143 |
| 14 | 2064／2065 | 98255，动作143 |
| 15 | 2066／2067 | 98258，动作143 |
| 16 | 2092／2093 | 111098，动作143 |
| 17 | 2094／2095 | 111098，动作143 |
| 18 | 2091 | 111098，动作143 |
| 19 | 2089 | 111192，动作141，走减益特效创建器但不属于六种普通减益位映射 |
| 20 | 2090 | 111193，动作141，同上 |
| 30／31／32 | 2050／2051／2052 | 均111099，动作143 |
| 33 | 3002 | 98273 |
| 34 | 3003 | 98261 |
| 35 | 3004 | 98259 |
| 36 | 3005 | 98260 |
| 37 | 3006 | 98264 |
| 38 | 3007 | 98266 |
| 39 | 3008 | 98272 |
| 40 | 3009 | 98263 |
| 41 | 3010 | 98267 |
| 42 | 3011 | 98269 |
| 43 | 3012 | 98270 |
| 44 | 3013 | 98271 |
| 45 | 3014 | 98259，与35共用显示资源 |
| 46 | 3015 | 98265 |
| 47 | 3016 | 98262 |

33～47均由动作143消费。主要生产在battle_skill_status.cpp的技能状态分支；2019的特殊索引在battle_skill_special.cpp。NewBattleStatusEffect_41FE00在mode==3时跳过按103018等图号设置六种能力位的逻辑，因此type=7不能直接等同于能力变化模式的某一位。

客户端PUE读取首个数值<=0时选mode=1，否则选mode=3，并将后续完整S32作为type；ANR也可直接传type。上表是已识别生产／消费集合，不是输入合法值全集。未命中的值在现有图像分派中不创建效果，不据此丢弃协议值。

服务器对照限制：本机battle/battle.c的BATTLE_ParameterUpDownCheck只生产PUE加位置的旧式清理消息；其源码与客户端此处读取的完整模式／类型格式存在差异。本次所查服务器battle／skill源码未找到上述扩展技能数字的直接对应，未读取配置中的显示名称来猜业务语义，不把本机服务器旧格式当作扩展类型的定义。

枚举候选边界：分别使用BattleDebuffType、BattleParameterEffectType和BattleExtendedEffectType描述三个域；前两组可用明确业务名，第三组未证部分保留编号或采用Skill2006等来源名。字段仍保留S32以承接-1、未知值和跨域复用，不能直接改成单一业务enum，也不能复用BattleActorFlag的位值。本次仅分析，尚未实施这些枚举。
