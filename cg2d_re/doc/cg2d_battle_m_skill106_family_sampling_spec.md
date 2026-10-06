# 战斗M技能106族取样SPEC（BNJ01）

<a id="battle-m-skill106-family-sampling-baseline"></a>
## 原端基线与范围

本步只修复既有 `ProcessBattleMSkill106Family`，技能106/111/2002真实分派保持。原主体431CBA..43222F为399条1397B五页46call，实际尾42CB37..42CB5E十条39B一页2call；同属父427470，合409条1436B六页48call，全部直接出口在本组或父默认437A29。[证据](cg2d_battle_m_skill106_family_sampling_evidence.json)保存实际字节/分页、两类效果原序/2002消费、八补丁、改前源码与旧812完整前态。

[BMT技能97主体](cg2d_battle_m_skill12_82_97_sampling_evidence.json)末十条39B/2call与本步尾逐字段相同，既有十条非新增，新取证399条1397B/46call分列。尾live caster/actor Move→RID→58加一；初caster空Finish直接返回不消费该尾，无末caster终结EffectList。

[BMO低段202B表](cg2d_battle_m_skill4_sampling_evidence.json)定位106/111→431CBA；[BNB高段原45B表](cg2d_battle_m_skill99_family_sampling_evidence.json)定位2002→431CBA，六W32跳槽24B/21W8索引21B，低case2002，前缀/外比较只定位。两表及原八W32一次初始化仅上下文，不当正文新增或全路由验收；不把原2019示例选择字段复制为2002事实。完整父17298/SEH、其它技能/所有consumer-owner与运行另验；网络原格式明文、音频暂停，显式UTF-8。

<a id="battle-m-skill106-family-sampling-decision"></a>
## 工程裁决与八处差异

依既有工程委托先SPEC后实施，单根八组八定义八处，不新增接口、owner、依赖、类型或保护。

| 项 | 改前差异 | 原端顺序与修复 |
|---|---|---|
| 1 | caster pub后重读位置取actor | GetPos返回位置先pub，actor沿同解析位置 |
| 2 | 初skill值早于payload | 固定caster payload先取，再live skill |
| 3 | 初level值早于actor | live caster位置→actor→live level→payload |
| 4 | target255比较在pub之后/判空重读位置 | 返回target先比较再pub，同解析actor判空 |
| 5 | 普通Effect实参读取隐含 | firstImage→live flags→live target→live caster→actor，固定amount |
| 6 | 两非地址索引为size_t | imageIndex/index保原S32，二元素首正值/默认索引0 |
| 7 | AuxEffects实参读取隐含 | 选中firstImages→secondImage→firstImage→live flags→live target→live caster→actor |
| 8 | 34后位置比较与actor重读 | 先live target再current比较，256/52沿同位置，52后51另live target |

普通Effect原431F50/F54/F5A/F60/F69，Aux原431F93/F97/F9B/FA0/FA6/FAD/FB5；位置比较原43205F先target、2064 current、206D比较、2071/2083沿同位置，51在2093另取。各样本只供该次具体消费，不冻结全部后续动作/target。非地址S32/W32保宽，已证地址native非拥有指针。

原正确行为保持：首Take attackMode、次Take/GetPos及caster判空；初空Finish直接返回。skill/level/fpUsed/attackMode，25/55/161；161后一次live skill供初分支106→77/36/91/103、111→7/9/10/75/108、2002且live level==90→112/64。每个Cast各次live caster。

每target Take/GetPos、255退出或actor空Finish后退出；正常Take flags即pub，再Take固定S32 amount。之后一次live skill==2002时再Take两整数、TakeStr2容量29、再Take一整数（名称缓冲未初始化保持，不新增清零）；之后读取live W32 flags800000决定AuxGraphics。普通Effect、二S32索引、AuxEffects，target0后另live skill==2002且live level<90时caster112/74/64；随后原一次live W32 flags供AL/AH位视图，不改内存W8读取/提前冻结门禁。

flags108零时优先40→target34/256或52-51；否则amount非零且flags3有值按8000/4000/2000取123/122/121，默认bit2取165/27。原动作后flags重新取样保持。amount或108有值且E000零时11，11后live flags100时76并再live flags；最后20时46。循环退出另一次live skill：111→caster76/7/11/10，2002→64，106/其它直接共用尾；每次角色位置另live，之后实际Move→RID→58加一，无额外55/末终结效果。

两图号/三组二元素数组原八W32事件入口初始化一次，跨target保留；全部原取包/字段/动作/flags重读、其它根头/声明/分派保持。RPLAN.24不新补零/钳制/越界溢出、回绕或故障模型，不宣异常或编译后指令形态等价。

<a id="battle-m-skill106-family-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证明 |
|---|---|
| I | 单根八定义八处落码，整CPP从HEAD精确重放补丁，其它正文/属性与声明保持 |
| S | 两范围409条1436B六页48call连续实际字节/全部出口、八差异原EA及实际Move-RID尾 |
| 消费 | 角色字段/同解析target、两Effect具体顺序/双S32索引、34后比较位置；2002原额外取包/29容量与后续live门禁 |
| 复用 | BMT末十条39B/2call非新增，新399条1397B/46call分列；原202B/45B两表与八W32初始化仅上下文 |
| 登记 | 旧812只五引用/一symbol/本步对象，双域四局部I/S，七open/唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；其它技能/全路由、完整父SEH/所有消费者与接线-owner失效/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续 |

E453→457仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60任务已闭10/完整验收0保持。不新F/X/adapted/分类/完整成员，不以本族/具体消费/实际尾或静态汇总代全路由/完整父/功能/整体验收。

<a id="battle-m-skill106-family-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill106Family`，旧812追加，快照 `cg2d-re-battle-m-skill106-family-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)及[看板](../plan/cg2d_measurement_dashboard.md)同步事实/剩余。本步八处差异当步修复；继续其它技能真实消费者、73零模式及全路由/完整父/生命周期与运行门禁。
