# 战斗M技能73非零模式取样SPEC（BNI01）

<a id="battle-m-skill73-sampling-baseline"></a>
## 原端基线与范围

本步只修复既有 `ProcessBattleMSkill73NonzeroMode`，真实技能73分派保持。原case73完整区段 `42AE67..42B282` 同属父427470，共305条1051B四页31call，全部直接出口在本区段或父默认437A29。[证据](cg2d_battle_m_skill73_sampling_evidence.json)完整保存实际字节/分页、调用/跳转、六补丁、改前源码和旧812完整前态。

305条中入口模式分派九条34B（42AE67..42AE89）、非零主体272条928B（42AE89..42B229）、零模式接线上下文18条68B（42B229..42B26D）、RID/增58尾六条21B（42B26D..42B282）。mode0调用424620及随后value5ABA7C角色Move仅作已有接线的原端上下文，不验其完整消费者；本步源码只非零根，不能以case区段存在声明mode0功能完成。

[BMO实际202B低段表](cg2d_battle_m_skill4_sampling_evidence.json)确定case73→42AE67，21个W32跳槽84B/118个W8索引118B仅定位上下文，不当正文指令或全路由验收。原八个正常W32零写由BMO/BMG具名初始化证据承接，只在事件入口一次；两图号/三组二元素数组跨target保留，不每target补零。完整父17298/SEH、其它技能及运行另验；网络原格式明文、音频暂停，显式UTF-8。

<a id="battle-m-skill73-sampling-decision"></a>
## 工程裁决与六处差异

依既有工程委托先SPEC后实施，单根六组六定义六处，不新增接口、owner、依赖、类型或保护。

| 项 | 改前差异 | 原端顺序与修复 |
|---|---|---|
| 1 | 初level值早于actor | live caster位置→actor→live level→payload/1044写 |
| 2 | target255比较在pub之后，判空又读全局位置 | GetPos返回target先比较再pub，actor沿同解析位置判空 |
| 3 | 普通Effect参数读取隐含 | firstImage→live flags→live target→live caster→actor，固定amount与该次取样 |
| 4 | 两非地址索引使用size_t | imageIndex/index保原S32，两元素选首个正值/默认索引0 |
| 5 | AuxEffects参数读取隐含 | 选中firstImages→secondImage→firstImage→live flags→live target→live caster→actor |
| 6 | 34后比较位置与分支角色重读 | 先live target再current比较，256/52沿同位置；52后51另live target |

第3项原42B020/B024/B02A/B031/B039，第5项原42B063/B067/B06B/B070/B077/B07D/B085，不套相邻技能flags发布顺序。第6项原42B0DA先target、B0DF current、B0E8比较，两分支分别沿EAX位置取actor；51在B10E另live target。角色仅借用native指针；非地址S32/W32不拓宽。

正确行为保持：原入口Take mode，零另分支，非零原<=0/>4直接RID/增58；本私有根只由非零入口调用，已有mode<0/>4早返保持。有效1..4先Take/GetPos→live skill（原95早于pub）→caster/value位置发布→同位置actor/payload写103C，然后level写；之后另live caster判空，空Finish→RID→增58返回，**不把判空搬到两字段写之前**。非空attackModeFF4=mode；1无初Cast，2为25/15、3为25/15/43/24、4为25/18，各次live caster。

每target Take/GetPos与255/actor空退出；空先Finish，之后Move/RID/增58。正常Take flags即pub，再Take固定S32 amount，**随后读live W32 flags**判断800000 AuxGraphics；不把初门禁改为Take返回值。先普通Effect再S32二元素索引选择/AuxEffects，target0后原一次live W32 flags，原AL/AH只是该值的位视图，不改为内存W8读取。

flags108零时优先40→target34/位置分支256或52/51；否则amount非零且flags3有值按8000/4000/2000选123/122/121，再按bit2选165/27。动作后原位置读取和flags重新取样保持。amount或flags108有值且flagsE000零时11；11后另live flags100时76并再live flags；最后20时46。循环末live caster Move→RID→58加一，无caster终结EffectList/额外Cast。所有正确分派、其它正文/属性/声明和调用次数保持。

RPLAN.24不新补零/钳制/越界溢出、回绕或故障模型，不宣异常或编译后指令形态等价。图数组现有八个业务初始化保留，size_t只在本根两个非地址索引替回S32，既有SDK/ABI/PC53与所有consumer-owner不外推。

<a id="battle-m-skill73-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证明 |
|---|---|
| I | 单根六定义六处落码，整CPP从HEAD精确重放补丁，其它正文/属性与声明保持 |
| S | case区段305条1051B四页31call连续实际字节/全部出口及六差异原EA；入口/零模式上下文和非零主体、RID尾分列 |
| 消费 | 初skill早于位置pub/两字段先判空、flags pub后第二Take/live门禁、两Effect具体顺序、S32索引和34后位置比较 |
| 保持 | 原1..4初动作、跨target八W32初始化、cached W32位选择/具名重新取样及Move-RID-增58，原mode0分派与其它根保持 |
| 登记 | 旧812只五引用/一symbol/本步对象，双域四局部I/S，七open/唯一进度同步 |
| 运行和整体 | 不构建/测试/运行/摘要；mode0完整消费者、其它技能/全路由、完整父SEH/所有消费者与接线-owner失效/SDK-ABI-PC53/时钟RNG画面及R1/R7/R8继续 |

E449→453仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60任务已闭10/完整验收0保持。不新F/X/adapted/分类/完整成员，不以区段静态/零模式接线上下文或本非零根代完整case73/父/功能/整体验收。

<a id="battle-m-skill73-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill73NonzeroMode`，旧812追加，快照 `cg2d-re-battle-m-skill73-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)及[看板](../plan/cg2d_measurement_dashboard.md)同步事实/剩余。本步六处差异当步修复；后续106族、73零模式/其它技能的真实消费者和全路由、完整父/生命周期与运行门禁继续。
