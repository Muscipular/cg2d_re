# 战斗M技能108取样SPEC（BND01）

<a id="battle-m-skill108-sampling-baseline"></a>
## 原端基线与范围

本步只修复既有 `ProcessBattleMSkill108`。原主体 `42EC95..42F035` 为226条928B三页24call，真实退出先压55再跳 `436DE2..436E17`：13条53B一页2call。两范围同属父427470，合239条981B四页26call；全部直接目标落本组或父默认437A29，不能只核局部线性主体。

该13条尾与[BMM01已有item主体内同区段](cg2d_battle_m_item_combo_sampling_evidence.json)逐指令/字节一致，既有尾不称新增，本步新取证226条928B/24call分列。case108由[BMO01实际低段202B入口表](cg2d_battle_m_skill4_sampling_evidence.json)定位42EC95；21 W32跳槽84B/118 W8索引118B只作为定位上下文。十二S32图号偏移48B沿[BMP01原表](cg2d_battle_m_skill61_sampling_evidence.json)保持。

[证据](cg2d_battle_m_skill108_sampling_evidence.json)保存正文/实际共用尾全部指令、两个正常效果分支、原表、改前源码/十处补丁及旧812完整前态。完整父17298条/SEH、所有其它技能与运行另验；网络原格式明文、音频暂停，显式UTF-8。

<a id="battle-m-skill108-sampling-decision"></a>
## 工程裁决与十组差异

依既有工程委托先SPEC后实施；单根十组十定义十处，不新增接口/依赖、owner、类型或保护。

| 项 | 改前差异 | 原端顺序与修复 |
|---|---|---|
| 1 | caster发布后重读全局位置 | GetPos返回位置先pub，actor沿同位置 |
| 2 | 初skill先于payload读取 | 固定caster的payload先取，再live skill |
| 3 | 初level先于actor读取 | live caster位置、actor、live level、payload |
| 4 | target比较在pub之后/判空又读位置 | 同解析target先255比较再pub，同位置actor判空 |
| 5 | Take flags发布早于target采样，skill值早于actor | 固定返回flags→live target位置→flags pub→同位置actor→live skill→payload |
| 6 | target level值先于actor | live target位置、actor、live level、payload |
| 7 | 比较caster/target和0动作取样不固定 | 先live target再读caster比较，0动作沿已比较target；后续7/9/10仍分别live caster |
| 8 | 106/57比较和caster actor顺序隐含 | live caster位置、live target比较、沿同caster位置actor，再对应动作 |
| 9 | 图号偏移表先于target位置 | 149/184各原路径完整核后保共用；level比较后低路径先target再S32表+110081、actor/payload，高路径111014 |
| 10 | EffectList live实参顺序隐含 | live W32 flags、live caster位置、live target位置、caster actor，amount沿固定返回值 |

第5项原42ED79 Take后42ED7E先读target、42ED84再pub flags；不把pub提前，也不改为所有后续沿同一target。基础level、位置比较、动作、图号、金额两字段与最后EffectList分别按原次重新live读取。初caster与loop target新局部名使用parsed前缀，保原成功分支casterPosition/targetPosition，避免新增遮蔽。

保留全部正确行为：入口两Take首弃/次GetPos，初caster空Finish直接返回；写skill/level/fpUsed，25/151。每目标Take/GetPos，255退出，空actor Finish退出；正常再Take amount并固定S32、Take flags，基础两字段，再caster/target不同的0、7、9、10，随后105。

105调用后读live flags80000：成功caster同target则106否则57；之后另读caster/target，20且target<10或21且target≥10走184，其余149。两分支各自调用后重新live level，图1028低段offset+110081或高段111014；仅149分支在写图后159。该分支选择沿已作出的原判断，不能在149/184之后因全局变化重新选分支。成功尾分别live target写1030=固定amount、1034=原5。失败原68、live caster写reactionImage1010=22331、再59，不改为成功字段生产。之后另live flags-caster-target-actor调用EffectList，74，再下一target。

循环255/空Finish末55→共用尾中的live caster Append→另live caster Move→packetSubState58加一→父默认；初空直接返回不走该尾。原无RID、无额外caster终结EffectList(-1)，现代正确尾保持。S32/W32/F32等非地址宽度不拓宽，已证地址native非拥有指针；RPLAN.24不新补零/钳制/保护、越界/溢出/回绕或故障模型，不宣异常或编译后指令形态等价。其它根/声明/分派保持。

<a id="battle-m-skill108-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步可接受证明 |
|---|---|
| I | 单根十定义十处落码，整CPP从HEAD重放精确补丁，全部其它正文/属性及声明保持 |
| S | 两范围239条981B四页26call连续实际字节/全部直接出口，含实际436DE2尾；原202B表/48B偏移与每处原EA，两个图号分支各有证据 |
| 消费保持 | 原调用/取包、amount固定/各次live字段、149/184与159门禁、68/22331/59、EffectList-74、末55/Move增58且无RID |
| 复用 | 既有BMM尾13条53B/2call分列，新226条928B/24call，不加入定位表或旧尾为新增原指令 |
| 登记 | 旧812仅增五引用/一symbol/本步对象，双域四局部I/S、七open进展/唯一进度同步 |
| 运行和整体 | 不构建/测试/运行/计算摘要；完整父SEH/所有消费者与接线-owner失效/SDK-ABI-PC53/时钟RNG画面及R1/R7/R8继续 |

E429→433仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60任务已闭10/完整验收0保持。不新增F/X/adapted、分类或完整成员，不以单根/两原分支/静态汇总代完整父、技能或整体验收。

<a id="battle-m-skill108-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill108`，旧812有界追加，快照 `cg2d-re-battle-m-skill108-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)及[看板](../plan/cg2d_measurement_dashboard.md)同步事实和剩余；不例行改总SPEC/索引或压历史。本步发现的十处正常差异当步修复，继续109等其它技能/全路由、完整父及全部消费/生命周期和运行门禁。
