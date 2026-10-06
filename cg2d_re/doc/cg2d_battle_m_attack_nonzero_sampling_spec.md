# 普通攻击非零模式取样SPEC（BNL01）

<a id="battle-m-attack-nonzero-sampling-baseline"></a>
## 原端范围与证据

本步只修复既有 `ProcessBattleMSkillAttackNonzeroMode`，模式1..4真实header分流保持。[证据](cg2d_battle_m_attack_nonzero_sampling_evidence.json)保存主435B63..43695E的979条3579B十三页94call，以及真实RID尾42B26D..42B282六条21B一页1call；同属父427470，合985条3600B十四页95call，全部直接出口本组或父默认437A29。原模式0/72/113/2017交公共九参攻击，已由[BNK三父桥](cg2d_battle_m_attack_targets_call_sampling_spec.md#battle-m-attack-targets-call-sampling-source-state)纳管，不在本步重新核对。

RID尾与[BNK第一范围](cg2d_battle_m_attack_targets_call_sampling_evidence.json)偏移18起六条逐字段一致，六条21B/1call非新增；主979条3579B/94call本步取证。八个普通图号/数组业务W32零写沿BMO/BMG具名基线；三个自身二元素数组六个W32零写沿BMG42749F..4274BF，各一次初始化、跨目标复用。2007常量表基址54D5F4六S32/24B原字节111067..111072，与现有数组逐项保持，不改索引或加范围门禁。

redirectPosition原读取未在该路径指定初值的父栈槽，selfSecondImage仅由条件Aux写入；本步保留源未初始化边界，不补值、不模拟历史栈，不宣故障域等价。完整父17298/SEH及跨消息生命周期另验。原会话首次整段输出截断后，按80条分页完整保存；不重开/写IDA数据库、客户端或算法，不计算摘要。网络原格式明文、音频暂停，显式UTF-8。

<a id="battle-m-attack-nonzero-sampling-decision"></a>
## 工程裁决与23组修复

按已有委托先SPEC后实施，单根23定义24处；位置比较同一文本补丁同时修普通目标和mode3后继两处。只修具体正常取样/非地址宽度，不新owner、类型、接口、依赖或保护。

### 普通入口、坐标与目标

| 组 | 原取样与修复 |
|---|---|
| 1–3 | GetPos同返回caster/value pub及actor；固定payload先live skill；caster位置-actor先live level |
| 4 | 2007 band helper之后caster位置→actor→payload→原表项→image102C；不先读表 |
| 5 | 258 X：索引15/5→X15→caster位置→X5→S32和→同位置actor→/2→payload |
| 6 | Y：重新索引15/5→两Y与S32和→caster位置→/2→同位置actor/payload；不冻结X采样给Y |
| 7–8 | GetPos target先255比较再pub；同解析位置比caster、写selfPosition或取普通actor判空 |
| 9–11 | 普通Effect firstImage-flags-target-caster-actor；两个S32索引；Aux选中图-second-first-flags-target-caster-actor |
| 12 | 两处34后先target再current比较；256/52沿同位置，51调用前另live target |

原GetPos后435B7E/B83双pub沿EAX；2007的435D42/D4D/D54先角色再D57表项。X原435DC1/DC7先两索引，435DD3 X15、435DD9 caster、435DDF X5，原435DEC取actor先于435DF3右移，源在S32和之后取actor再用/2表达向零平均，不模拟CDQ/SUB寄存器算术；Y在X写后重新读取索引/数据，435E1F位置取样在435E25平均之前以显式局部保留。平均只保正常S32相加/向零截断，不新增溢出或回绕模型。

原mode无效/初caster空Finish沿RID增58早返，无Move；有效路径字段skill/level/fpUsed/mode、40，live skill条件151，105 flash=1，2007原band+图/252，广域技能151及258 X/Y-8/6/10保持。levelIndex>=9的266/259/258原EffectList0/5/3与245保持；55后mode4为25/18、3为25/15/43、2先缓存skill再25/19或15，1无额外动作，全部角色各次live。

self target在普通空门禁前处理：保存selfPosition/固定selfFlags与amount，selfFlags800000决定自身Aux，200决定Take/GetPos转向；继续而不生产普通效果。普通target取flags即pub、再取amount后live W32门禁800000决定Aux，普通Effect→S32选首正图/默认0→Aux→target0；0后一次W32 flags供AL/AH位视图，不改为内存W8。

原108/40/8000/4000/2000与20000000门禁、95/3019的29及4002/3038的30、默认165/27，动作后具名flags重读保持；11/76与特殊反馈20零直接继续、末46的消费/分流不变，不统一套其它攻击技能模板。

### mode3自身与转向后继

| 组 | 原取样与修复 |
|---|---|
| 13 | live caster位置→selfFirstImage→actor，然后pub target/flags与firstImage，再固定selfFlags的Effect(-1) |
| 14、17、22 | 转向先Cast0后取selfAmount，非转向在原汇合前取selfAmount；固定tailAmount沿原EDI用于所有后继和非零判断，不在调用后重读 |
| 14–15 | 转向EffectList amount-flags-target-redirectActor；写100C先redirectActor→live target→payload |
| 16 | live W32 flags→firstImage→清200/pub固定flags→caster位置/actor→Effect；调用后pub转向target |
| 18–19 | 24后Effect一：image-flags-caster-target-actor；二：image-flags-target-actor、目标与actor沿同位置，均用固定tailAmount |
| 20–21 | 两个S32索引；仍选普通firstImages（436719），Aux选中图-second-first-flags-target/同位置actor |
| 23 | 4368A2末20门禁显式原低W8读取；此前43679E W32的AL/AH分支保原一次样本 |

mode3 finalSkill266/2022且selfPosition>=0才走此后继；原条件/未有自身记录回落保持。43643F actor在43644A/44F pub之前，旧源曾反序；金额原转向436494、非转向4366B2沿同一EDI，显式tailAmount两分支赋值，不新默认数值。转向100C/1018=252/1010=22344与所有Cast原序保持，清200为完整W32只清该位，不宣内存W8写。

末Aux必须使用普通图数组/普通secondImage，不误换刚保存的self数组。221后原caster终结EffectList(-1)-74及目标反馈、最后46保持；W8读取用现有x64小端承载的std::uint8_t借用指针，仅此真实原字节门禁，不外推其它flags内存访问，不新增字节影子状态。其余模式/模式3回落原终结-74（模式3再24）→live caster Move→RID→增58保持。公开声明/header/其它全部正文不改；native地址与非拥有借用保持，非地址S32/W32不拓宽，禁RTTI。

<a id="battle-m-attack-nonzero-sampling-acceptance"></a>
## 可度量验收

| 阶段 | 本步证明 |
|---|---|
| I | 单根23定义24处，从HEAD精确重放整个CPP；两处比较/四S32索引、所有新取样与W8门禁限定落码 |
| S | 两范围985条3600B十四页95call连续字节/全部直接出口、23组原EA、RID复用及六S32表/原业务零写上下文 |
| 消费 | 原技能/模式/取包、自身缓存和条件转向、普通/后继图数组分工、cached/live flags与amount、原字段/动作/早返及Move-RID增58 |
| 复用 | RID尾六条21B/1call非新增，新979条3579B/94call分列；初始化仅业务上下文，原未指定局部不补值 |
| 登记 | 旧812五引用/一symbol/本步对象、双域四局部I/S及七open/唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH/其它技能-全路由、所有consumer-owner/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续 |

E463→467仅YOBI/BATTLE四局部I/S；1350/有效ID1345、片段5/分类456、原成员55/52/26/32、关联1463/3680、60源码closed10/完整验收0保持。不新F/X/adapted/分类或完整成员，不以本非零根/985条或原字节门禁代完整父/全部技能/功能运行；不宣编译指令形态或异常等价。

<a id="battle-m-attack-nonzero-sampling-source-state"></a>
## 源码与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkillAttackNonzeroMode`，快照 `cg2d-re-battle-m-attack-nonzero-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)及[看板](../plan/cg2d_measurement_dashboard.md)同步；本步全部23组差异当步修复，继续其它技能真实消费者/全路由、完整父与生命周期/运行门禁。
