# BOC01 命中与命中音效更新回调

<a id="battle-impact-callbacks-sampling-baseline"></a>
## 原端与实际差异

[完整证据](cg2d_battle_impact_callbacks_sampling_evidence.json)覆盖两个独立单chunk：41A9E0..41AEFF的362条1311B、41AF00..41B438的370条1336B，共732条2647B十页，57call/74普通内部分支/两ret，无switch。三入向全部来自已过[BNZ01共同创建](cg2d_battle_effect_factory_sampling_spec.md)：41A9E0两地址引用（一次传callback、一次直接绑定），41AF00一次传callback；两根不是普通call入向，不据引用数宣全部父/consumer/owner已验。

两原ID此前无独立语义/分类/战斗成员，历史定义已有，不在175定位队列。当前源码各有五处语言级取样差异：
1. 入口先读取payload再phase，原phase先payload。
2. 完成后先查世界表target再读flags，原position→flags→target→targetPayload。
3. 负HP归零分支先查copyTarget再由CopyPayload取临时encoded，原fresh C9C→临时encoded→position/世界表→W32 encoded发布→W32 mask发布→W8 readCount发布。
4. 治疗数字style5提前计算Y-80，targetX在调用实参中读取；原flags→targetY→记录position3→targetX→S32 Y-80。
5. 普通数字style6存在同样取样差异。

本步十补丁解决全部五类差异；原已正确的尾部offsetX→F32 Y→S32 X发布→Y截断与offsetY发布不重改，原伤害/治疗/分类与完成分支全部保留。

<a id="battle-impact-callbacks-sampling-decision"></a>
## 本线程裁决与完整合同

按既有用户授权先SPEC后实施，不合并两原回调。各保存入口initialPhase、完成initialPosition、负HP copyIndex/临时encoded，两个数字分支各显式取targetY、displayed、targetX，再Y-80调用。仅负HP内联原三字段复制以恢复encoded先于目标查表，不改通用CopyPayload或其它调用。LegacyCryptInt的token/reserved均保留；治疗上限CopyPayload原W32/W32/W8读写顺序已经正确，保持。

| 阶段 | 完整正常合同 |
|---|---|
| 入口/完成 | 无新action/payload/target判空。phase0取live当前目标，目标phase非零立即返回，连延迟/坐标尾也不执行；其它路径写phase1/Play(1)，严格返回1才MarkDeath并进入完成逻辑。标死不立即释放、不早退 |
| 图号/施加效果 | 固定完成游标position/flags/target及targetPayload；flags4选110000，800选110099，400选110098，否则才RNG(0,2)+110001。406260后从live源18写固定目标opponent；fresh当前flags写固定目标FE4，再406390(live源18,action)；之后重取resolved游标/flags |
| 伤害 | resolved8写固定目标phase31，跳过其它分类但仍finish；否则70000清且position3非零才随机效果→重生效果→fresh金额/当前对象SubtractAssign→fresh对象Less(0)。负值才原零临时构造，fresh游标先临时encoded、再当前对象三字段复制/析构；随后fresh flags20写固定targetPayload FDC OR2，fresh40写全局gate1/alive1 |
| 三优先分类 | category取一次，10000优先、40000其次，二者Advance固定target后场景phase严格8才payload00++；20000再次，Advance后fresh10000000只生成当前对象22459，否则金额先position对象AddAssign→fresh对象GreaterCrypt(maxHp)→fresh对象三字段上限复制；style5数字/flags2的22454/属性效果后两条路径均按场景phase8计数 |
| default | 100000才MarkDeath固定targetPayload1054对象，再取其FDC、清1054地址、按旧FDC清20发布；随后Advance固定target，fresh10000000只当前对象22459，否则style6数字/flags2的22454/属性效果。此分类不增加场景计数；41AF00额外132音效位于default公共末尾，41A9E0没有 |
| finish/尾 | fresh游标flags8清且position3非零，取固定targetPayload FDC的bit1生成14或28写源FD0，完成分支清W32 frameEvent。随后所有非入口早退路径先old58-1发布，仅old58<=0才检查源FD0>0并减一；截断X→volatile屏幕X偏移→取F32 Y→正常S32 X相加发布→Y截断/volatile Y偏移→S32 Y发布 |

原HP负值归零、上限复制、状态门禁和业务0是真实原行为，不能因RPLAN.24异常不专项处理而删掉，也不新增越界/溢出拒绝、回绕或故障模型。内存既有CryptInt沿获准固定布局/生命周期保持，不扩展网络加解密；网络继续原格式明文。两flags分类顺序、图号随机消费、scene计数、固定目标/载荷跨调用与源游标重读不能合并或改为统一缓存。

action/target/payload/1054及真实callback沿获准native非拥有地址，数值S32/U32/W32/W8与F32原宽，原__ftol沿CPH01 host，不宣异常输入、x87 PC53、编译器实际取样次序或EAX/机器ABI等价。41AF00音效原额外Y栈字沿既有接口说明不被52C450本体读取，保持现二参接口和暂停范围。

新增语义1378..1379分别绑定F:0001A9E0/F:0001AF00、adapted分类及FUNC-BATTLE成员80→82，枚举仍open。十源码差异本步解决，已过[BOB01五初始化助手](cg2d_battle_initializers_sampling_spec.md)不全面复验；尚未核实的FrameEvent/Moving/Homing回调沿实际依赖继续开发。

<a id="battle-impact-callbacks-sampling-acceptance"></a>
## 可度量验收

| 项 | 限定依据 |
|---|---|
| I | 两根十补丁/整CPP从HEAD精确重放；五类取样、HP W32/W32/W8复制/原业务上下限、固定/live引用、分类优先和scene计数、严格完成/标死后继续、old58-FD0及偏移坐标定向静态 |
| S | 两完整732条2647B十页、57call/74内部分支/两ret、三真实callback地址引用；原全部路径与业务宽度，现通用复制/固定布局/volatile偏移声明分列 |
| 追踪 | 两独立ID/分类、成员82，每函数局部I/S共四E；七open/唯一进度同步，所有旧记录和完整门禁保持 |
| 度量 | 语义1378→1380/有效ID1373→1375、分类484→486、关联1491→1493/3680=40.57%、E551→555；175定位队列保持 |
| 整体 | 不构建/测试/运行/摘要；全部父-callee-owner/资源-ABI-SDK-PC53-RNG画面及R1/R7/R8另验，整体未完成 |

YOBI55/RENDER26/LOOP32、片段5/R7区间67/3061、60源码closed10/完整验收0/P01与七open/全部完整门禁保持。只声明本步正常源码及完整原根的局部I/S，禁止以两根通过代整功能或全生命周期验收。

<a id="battle-impact-callbacks-sampling-source-state"></a>
## 当前收尾

两回调十处取样问题已修，HP消费、状态分类、场景计数与完成尾基线可独立追踪。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)/[看板](../plan/cg2d_measurement_dashboard.md)同步，固定快照 `cg2d-re-battle-impact-callbacks-sampling-20261006`。继续帧事件和运动回调中的开发缺口。
