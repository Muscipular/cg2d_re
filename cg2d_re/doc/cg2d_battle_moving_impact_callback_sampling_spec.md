# BOE01 移动命中与运动尾回调

<a id="battle-moving-impact-callback-sampling-baseline"></a>
## 原端与实际差异

[完整证据](cg2d_battle_moving_impact_callback_sampling_evidence.json)覆盖41BB00..41C3A9独立单chunk，636条2217B八页45call/78普通内部分支/一ret，无switch。原F32 50(54D5A4=00004842)/0.5(54D48C=0000003F)共8B。单callback地址引用41DFCF由已过[BNZ01共同创建](cg2d_battle_effect_factory_sampling_spec.md)传给Offset初始化，不据单引用宣所有消费者/owner已验。

F:0001BB00此前无独立语义/分类/战斗成员，历史定义已有、不在175定位队列。当前五处语言级取样差异：入口payload先phase；首次Load实参图号晚于targetPayload取样；负HP临时encoded晚于copyTarget查表；治疗style5提前Y-80且targetX在调用实参读取；末优先级发布早于F32 X取样。

本根default style6已按flags→targetX→position3→targetY→S32 Y-80正确执行；old58为0的多条原音效路径全部收敛到262，现单调用正确。阈值原FILD S32半高→FIADD对象2CC S32→FADD目标F32 Y，本体double中间加法正确，不改成S32先求和。这些路径与[BOC01](cg2d_battle_impact_callbacks_sampling_spec.md)/[BOD01](cg2d_battle_frame_event_callback_sampling_spec.md)不同，保实际证据，不照搬。

<a id="battle-moving-impact-callback-sampling-decision"></a>
## 本线程裁决与完整合同

按既有用户授权先SPEC后实施：保存initialPhase、首次targetGraphic、负HP fresh copyIndex/临时encoded和治疗targetY/position3/targetX，再正常S32 Y-80。末尾保存目标U8 priority、F32 projectedX，再原W8 priority+20发布，随后截断X。仅五补丁，首次图号样本只用于该Load，后面三次半高继续按live target.graphicIndex读取；通用复制/固定内存CryptInt/DX读取与声明不改。

| 阶段 | 完整正常合同 |
|---|---|
| 入口/固定引用 | phase0仅当前目标phase非零才早退，所有尾也跳过；无新action/payload/target空门禁。其它路径phase1，固定当前target/targetPayload贯穿本次回调，原graphic取样先payload再Load，后续效果游标与目标字段按原重读 |
| 提前视觉标死 | Load后fresh flags20000000才读live图号DX低S16半高，对象2CC按S32参与double，目标F32 Y加后减原F32 50，再比较action Y。threshold>=actionY不执行，原通过且frameEvent0才依次屏幕X/Y偏移S32各/2加action各F32坐标并发布，然后MarkDeath自身、以自身为parent生成110469、固定target生成110474、Advance固定target。原未设置frameEvent或return，继续普通中心判断/延迟/运动尾 |
| 普通命中 | 重新读live target图号DX半高/对象2CC/Y，独立threshold/frameEvent0门禁。flags4/800/400选110000/110099/110098，否则才RNG0..2+110001；406260后live源18写opponent/fresh flags写固定targetPayload FE4，再406390。resolved8只target phase31跳分类；否则70000清且position3非零才金额先当前对象SubtractAssign、fresh对象Less(0)/原零临时三字段复制/析构，fresh flags20写固定FDC OR2/gate1/alive1，无40门禁 |
| 分类/完成 | category一次取样，10000→40000→20000→default。前三种Advance后场景phase严格8才payload00++；20000按fresh10000000只当前对象22459，否则AddAssign/原上限CopyPayload、style5数字/fresh2的22454/属性效果。default可100000标死1054/旧FDC清20-地址清零，再fresh2的110008/主图随机效果/重生效果、Advance及fresh图标或style6数字/属性，无scene计数；按action image110032..034播263再132，035..037播263、038..040播132，其余131，音频暂停。fresh delay游标8清且position3非零才FD0=14或28，然后W32 frameEvent=1 |
| 延迟/屏幕震幅 | 所有非早退路径old58-1发布；old58>0直接末尾，old58<=0继续。old58严格0才音效262，再RNG0..32；仅正帧数按固定次数Play(0)，0不播放。随后正FD0：先fresh C9C、FD0-1发布，仅该项70000清才取技能level、rising1、S32 level/2+1发布原W32最大震幅。之后必须fresh重读FD0严格0才运动 |
| 运动/尾迹 | FD0零时Play(0)，重新live图号DX低S16半高、取target F32 X，然后对象2CC/Y计算double中心并F32传TurnTowardPosition(action,X,Y,0,0)，再MoveByAngle。速度F32读+原0.5后先取frameEvent，再F32速度发布。旧hit0才图号032..034/035..037在nameX0生成110007尾迹，038..040在nameX0生成110006，并此族无论是否生成都额外nameX++。之后fresh oldTrail<2写+1，否则写业务0；fresh frameEvent严格1则标死自身、rising0，仍执行末尾 |
| 优先级/投影 | 固定target U8 priority取样→action F32 X取样→原W8 priority+20发布→X截断→volatile屏幕X偏移→F32 Y取样→正常S32 X发布→Y截断/volatile Y偏移相加发布。原__ftol沿CPH01 host；所有提前或最终标死都不立即free/摘链 |

原HP负值归零、上限三字段复制、帧数0不播放/nameX节拍零/frameEvent1与震幅0都是实际业务合同，保token/reserved，不能因RPLAN.24删业务门禁，也不新增越界/溢出保护、回绕、钳制或故障模型。地址沿获准native非拥有action/target/payload/1054/真实void callback，其它S32/U32/F32/W32/W8原宽；原S16 DX字段每阶段重读，正常double中间值/F32中心与速度发布保。语言级取样修复不代编译器次序、x87 PC53、原EAX/机器ABI或实际画面运行证明。

网络原格式明文；内存既有CryptInt保，不扩展网络加解密。音效原额外Y栈字沿既有52C450说明不被本体读取，现二参接口与暂停范围保持，不宣音频验收。

新增语义1381绑定单原ID/adapted分类、FUNC-BATTLE成员83→84，枚举仍open。五源码问题本步解决，已过共同创建/初始化/命中/帧事件父叶不全面复验，Homing实际回调与其真实依赖继续开发。

<a id="battle-moving-impact-callback-sampling-acceptance"></a>
## 可度量验收

| 项 | 限定依据 |
|---|---|
| I | 单根五补丁/整CPP从HEAD重放；原default X-数值-Y与三次live DX读、独立特殊/普通门禁-提前标死继续、HP上下限/分类场景计数-条件RNG、old58/正FD0-震幅/fresh零/中心F32转向-运动-速度-hit/尾迹第三族额外自增/最终标死与W8优先级-投影定向静态 |
| S | 完整636条2217B八页45call/78内部分支/一ret、F32常量8B、单callback地址引用，真实字段取样/宽度与既有复制/DX读取/偏移声明分列 |
| 追踪 | 单ID/分类和成员84、局部I/S两E；七open/唯一进度同步，全部旧记录和完整门禁保持 |
| 度量 | 语义1381→1382/有效ID1376→1377、分类487→488、关联1494→1495/3680=40.62%、E557→559；沿现Percent的Round至两位，175定位队列保持 |
| 整体 | 不构建/测试/运行/摘要；全部父-callee-owner/资源-ABI-SDK-PC53-RNG画面及R1/R7/R8另验，整体未完成 |

YOBI55/RENDER26/LOOP32、片段5/R7区间67/3061、60源码closed10/完整验收0/P01与七open/所有完整门禁保持。仅本步正常源码与完整原根局部I/S，不代完整战斗/owner或运行门禁。

<a id="battle-moving-impact-callback-sampling-source-state"></a>
## 当前收尾

移动命中回调五处取样已修，命中/HP消费/延迟与运动尾独立可追踪。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)/[看板](../plan/cg2d_measurement_dashboard.md)同步，固定快照 `cg2d-re-battle-moving-impact-callback-sampling-20261006`。继续Homing真实回调缺口。
