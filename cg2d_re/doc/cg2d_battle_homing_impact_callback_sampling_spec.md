# BOF01 追踪命中与出界运动尾回调

<a id="battle-homing-impact-callback-sampling-baseline"></a>
## 原端与实际差异

[完整证据](cg2d_battle_homing_impact_callback_sampling_evidence.json)覆盖41C3B0..41CCFD独立单chunk，676条2381B九页51call/83普通内部分支/一ret，无switch。原四F32常量共16B：54D4B4=CDCCCC3D（0.1）、54D478=000048C3（-200）、54D474=00005244（840）、54D470=00002A44（680）。单callback地址引用41DF9E由已过[BNZ01共同创建](cg2d_battle_effect_factory_sampling_spec.md)传Center初始化，不据单引用宣全consumer/owner已验。

F:0001C3B0此前无独立语义/分类/战斗成员，历史定义已有、不在175定位队列。当前四处语言级差异：入口payload先phase；负HP临时encoded晚于copyTarget查表；治疗style5提前Y-80且targetX在调用实参读取；default style6在读取金额/X之前提前Y-80。

本根治疗style5是flags→position3→targetY→targetX→S32 Y-80，default是flags→targetY→position3→targetX→S32 Y-80，与[BOE01移动回调](cg2d_battle_moving_impact_callback_sampling_spec.md)不同。普通命中已按position/flags先世界表target、特殊全屏偏移/目标取样及尾部原浮点/RNG顺序正确，保持；本步四补丁解决全部上述差异。

<a id="battle-homing-impact-callback-sampling-decision"></a>
## 本线程裁决与完整合同

按既有用户授权先SPEC后实施：保存initialPhase、负HP fresh copyIndex/临时encoded后查对象并按W32 encoded/W32 mask/W8 readCount发布，两个数字分支显式targetY/X后正常S32 Y-80，治疗金额仍先Y，default保原X变量和caller参数。通用复制/固定内存CryptInt/接口不改，token/reserved保留。

| 阶段 | 完整正常合同 |
|---|---|
| 入口/特殊距离 | phase0仅当前目标phase非零才早退，所有尾也跳过；原无action/payload/target新空门禁。phase1后固定initialIndex，flags20000000及frameEvent0才用该项目标GetDistance<=70；成功后fresh游标、volatile全屏X偏移与action X的double先算、再查此时target、F32 X发布，然后全屏Y偏移加action Y/F32发布，MarkDeath自身/自身parent110475/固定此时target110474/Advance。不用半偏移，不设置frameEvent或return，仍普通距离与运动尾 |
| 普通距离/固定目标 | 再fresh frameEvent0才重取当前目标GetDistance<=8；成功后fresh游标position/flags→世界表target→固定targetPayload，flags4/800/400选110000/110099/110098，否则才RNG0..2+110001。原不调用406260，不补目标动作重置；直接live源18写opponent/fresh当前flags写FE4，再406390，之后fresh resolved |
| HP/四分类 | resolved8仅target phase31跳分类；否则70000清且position3非零才金额先当前对象SubtractAssign、fresh对象Less(0)/原零临时三字段复制/析构，fresh flags20直接固定FDC OR2/gate1/alive1，无40门禁。category一次10000→40000→20000→default，前三种Advance后scene phase严格8才payload00++；20000按fresh10000000生成22459或AddAssign/原上限复制、style5数字/fresh2的22454/属性效果 |
| default/命中尾 | 可100000标死1054/旧FDC清20和地址零，再fresh2的110008/主图随机效果/重生效果，随后原TurnToward(target,action,1,0)才Advance；fresh10000000图标或style6数字/22454/属性，无scene计数。音效仅image111011/110044/110045播269、110046/110047播268，其它无音效。fresh delay游标flags8清且position3非零才FD0=14或28，然后W32 frameEvent=1 |
| 延迟/跟踪与出界 | 所有非早退路径old58-1先发布，old58>0直接投影；old58严格0启动音效只111011/110044播269，110045/110046/110047播268，其它无。原无随机预播放/震幅写入。随后正FD0减一，再fresh重读FD0严格0才Play(0)，Play后fresh frameEvent0才B4 F32+原F32 0.1，先F32临时和B4发布，再fresh当前目标TurnToward(action,target,0,step)。event任意非零则依原X/Y四次独立读判断X<-200/X>840/Y<-200/Y>680才MarkDeath；边界含端点保，标死后仍MoveByAngle及尾迹/节拍/投影 |
| 五族尾迹 | Move后固定image，只有各族nameX0才Y RNG→double加action Y/F32保存→X RNG→double加action X/F32保存→NewTrail(mode1,lifetime202)。111011与110044的Y/X范围-10..10、110007；110045范围-6..6、110007；110046范围-4..4、110006并无论是否生成额外nameX++；110047范围-2..2、110006。其它四族共用原尾调用，110046独立；随机消费/舍入/坐标读取顺序保，不新增或移走RNG |
| 节拍/投影 | 尾迹后fresh oldTrail<2写+1，否则业务0；无论特殊提前标死、出界标死或普通非运动路径，最终X截断→volatile屏幕X偏移→F32 Y→正常S32 X发布→Y截断/volatile Y偏移相加发布。原没有额外优先级改写、不立即free/摘链，__ftol沿CPH01 host |

原HP归零/上限、距离70/8/flags门禁、FD0/frameEvent/nameX业务0/1和出界标死保，不因RPLAN.24异常不专项处理删业务规则，也不新增越界/溢出保护、钳制、回绕或故障模型。地址沿获准native非拥有action/target/payload/1054/真实void callback，其余S32/U32/F32/W32/W8原宽；0.1F取真实原F32位型，正常double中间/F32发布和Y先X RNG保。仅语言级取样及正常源码，编译器次序、x87 PC53、原EAX/机器ABI或实际画面未运行。

网络原格式明文；既有内存CryptInt保、不扩展网络加解密。音效额外Y栈字沿既有52C450说明不被本体读取，现二参接口/暂停范围保，不宣音频验收。原距离/转向/运动生产消费还有独立基线与全部父-owner收口责任。

新增语义1382绑定单原ID/adapted分类，FUNC-BATTLE成员84→85，枚举仍open；四源码问题当步解决，已过共同创建/初始化/命中/帧事件/移动父叶不全面复验，继续实际距离/转向/运动依赖开发。

<a id="battle-homing-impact-callback-sampling-acceptance"></a>
## 可度量验收

| 项 | 限定依据 |
|---|---|
| I | 单根四补丁/整CPP从HEAD重放；独立70/8门禁/全屏偏移-提前标死继续、无406260/HP原上下限和四分类-scene/default转向、不同命中/启动音效映射、old58/正FD0/fresh零-F32步长发布/出界仍移动、五族Y先X RNG和F32/110046额外计数-共同节拍/偏移尾定向静态 |
| S | 完整676条2381B九页51call/83内部分支/一ret、四F32常量16B、单callback地址引用；真实字段取样/业务宽度与既有复制/固定布局/偏移声明分列 |
| 追踪 | 单ID/分类及成员85、局部I/S两E；七open/唯一进度同步，全部旧记录与完整门禁保持 |
| 度量 | 语义1382→1383/有效ID1377→1378、分类488→489、关联1495→1496/3680=40.65%、E559→561；175定位队列保持 |
| 整体 | 不构建/测试/运行/摘要；全部父-callee-owner/资源-ABI-SDK-PC53-RNG画面及R1/R7/R8另验，整体未完成 |

YOBI55/RENDER26/LOOP32、片段5/R7区间67/3061、60源码closed10/完整验收0/P01与七open/所有完整门禁保持。仅本步正常源码与完整原根局部I/S，不代战斗整体或全生命周期/运行验收。

<a id="battle-homing-impact-callback-sampling-source-state"></a>
## 当前收尾

追踪命中四处取样已修，距离命中/HP消费/跟踪与出界尾独立可追踪。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)/[看板](../plan/cg2d_measurement_dashboard.md)同步，固定快照 `cg2d-re-battle-homing-impact-callback-sampling-20261006`。沿真实距离/转向/运动依赖继续。
