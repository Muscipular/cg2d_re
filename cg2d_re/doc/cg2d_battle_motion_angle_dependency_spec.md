# BOG01 战斗距离、转向与原表角度依赖 SPEC

> 2026-10-06。本线程按既有工程委托先登记方案再实施。沿已完成的 [BOF01 追踪命中](cg2d_battle_homing_impact_callback_sampling_spec.md) 和 [BOE01 移动命中](cg2d_battle_moving_impact_callback_sampling_spec.md) 的真实调用，聚焦此前未独立纳管的九个助手；不重验已过回调，不改网络原格式明文及音频暂停。

<a id="battle-motion-angle-dependency-baseline"></a>
## 1. 原端基线与范围

权威为已打开的 cg2d.exe 会话只读实际指令，不用反编译名代事实。九个函数均单 chunk，共 **441 条/1422B/11 页、14 个 call、60 个普通跳转、17 个 ret**；其中 406356 为到既有 __ftol53B498 的函数外尾跳，不能把它错计成根内出口或 call。46F970 另有一个实际 switch，八槽 W32 表32B；219条入向引用逐条归档，分类只证明引用来源，不证明这些全部父函数已验收。

| 原入口 / 库存 ID | 现有源码符号 | 指令/代码B | call/跳转/ret | 本步补丁 |
| --- | --- | --- | --- | --- |
| 0x405EA0 / F:00005EA0 | MoveBattleByAngle_405EA0 | 17/46 | 2/0/1 | 0 |
| 0x4060D0 / F:000060D0 | TurnBattleTowardPosition_4060D0 | 61/188 | 4/9/1 | 0 |
| 0x406190 / F:00006190 | TurnBattleToward_406190 | 66/206 | 4/11/1 | 1 |
| 0x406300 / F:00006300 | GetBattleDistance_406300 | 33/94 | 0/5/1 | 0 |
| 0x46F930 / F:0006F930 | Sub_46F930 | 8/30 | 2/0/1 | 0 |
| 0x46F950 / F:0006F950 | Sub_46F950 | 7/23 | 1/0/1 | 0 |
| 0x46F970 / F:0006F970 | Sub_46F970 | 114/351 | 1/10/9 | 1 |
| 0x46F8F0 / F:0006F8F0 | Sub_46F8F0 | 20/59 | 0/4/1 | 0 |
| 0x405CB0 / F:00005CB0 | Sub_405CB0 | 115/425 | 0/21/1 | 0 |

完整指令、入向、常量、改前原根及精确补丁见 [原证据](cg2d_battle_motion_angle_dependency_evidence.json)。18个标量常量共80B，360项 F64 可写表55DF10=2880B、65项 F64 可写表55EA50=520B，保留真实位型；加switch32B共3512B数据证据。表55DF10来源为 src/world/SceneAngleTable55DF10.inc，逐项对照原2880B；不运行三角函数生成替代表。已有 __ftol 宿主转换职责按 [转换边界](cg2d_compiler_helper_spec.md) 保留，不能借本步宣 ABI/PC53 一致。

## 2. 完整正常合同

### 1. MoveBattleByAngle_405EA0

两次角度/速度live读取：X角调用46F950→乘value54→另取Y角→加X/F32发布→46F930(Y角)→重新读value54/Y并F32发布；原无空门禁，无场外检查或death处理。

### 2. TurnBattleTowardPosition_4060D0

step先比较原零、Y差先于X差且分别F32；无空门禁；step零直接目标角F32，否则F64目标角-当前角，>180减360、<=-180加360各一次，正常S32截断后abs再与step比较。负差达到步长则当前角-step；正差则step+当前角；小余量则difference+当前角；先F32发布，仅updateDirection严格1调用405CB0。

### 3. TurnBattleToward_406190

action先空返回、target后空返回；其它与坐标转向同公式，目标Y差先X差/F32；原负向当前角-step、正向step+当前角、余量difference+当前角恢复，发布F32后仅updateDirection==1更新方向，不abs(step)或钳制。

### 4. GetBattleDistance_406300

first/second原空返回0；secondY-firstY先于X差，未先F32差值；X平方+Y平方后唯一F32平方和，原平方和正数才fsqrt/尾跳__ftol53B498，正常S32截断返回，其它0。

### 5. Sub_46F930

参数局部F32地址先交46F8F0重复圈归一→读F32角→正常S32截断→可写360项F64位型表55DF10[index]返回F64，不以sin重算。

### 6. Sub_46F950

F32角+原F32 90后落F32，交46F930并返回F64，原未额外取模/限制输入。

### 7. Sub_46F970

原两分量均0返回0；首/次分量负标记1/2，abs首<abs次标记4，以较小绝对分量/较大分量*64；正常S32截断index，index+1仅原业务最大64，fraction独立落F32；左表项先于1-fraction、左乘后右表项/右乘，和落F32；八selector依次angle/180-angle/360-angle/angle+180/90-angle/angle+90/angle+270/270-angle，返回F64，不能换atan2。

### 8. Sub_46F8F0

native非拥有float*，无新空门禁；每圈fresh上界>=360时减原F32 360并存F32重来，否则fresh下界<0加360/F32重来，达到[0,360)原返回；原重复循环保，不改fmod或一次圈调整。

### 9. Sub_405CB0

native非拥有action，无新空门禁；角>=360减360否则<0加360，仅调整一次F32；八区间逐条live比较/W32方向：15..60→4、60..120→5、120..165→6、165..195→7、195..240→0、240..300→1、300..345→2、345..360或0..15→3，下含上不含；不循环归一。

坐标转向与对象转向的原零比较包含 x87 unordered 状态，既有源码的 step==0 或 isnan 表达保持；本步不新增非有限值/溢出/越界处理，不建立异常等价或输入门禁。原负步长不被改为绝对步长；difference 正180保留、负180加360成为正180，角差最多调整一圈。原 nextIndex 最大64属于正常插值业务，不扩大为通用钳制。距离、对象转向的原空对象早退保，坐标转向、运动、方向及角度归一的原无空门禁保。

<a id="battle-motion-angle-dependency-decision"></a>
## 3. 差异与本线程裁决

| 差异 | 原证据 | 裁决与实施 |
| --- | --- | --- |
| 对象转向把负向限步预先取负后合入统一加法；原体分别执行当前角度-step、step+当前角度，余量分支再相加 | 40621B/40621F/406228/406235/406237/40623A/406240/406244/40624B | 恢复三条原公式和唯一 F32 发布分支，保S32截断幅度与比较。此为源表达还原，不凭静态推断已有画面差异或宿主浮点完全等价 |
| 46F970先算1-fraction后读左表；原先读可写F64左表，再权重/左乘，后读右表/右乘 | 46FA31/46FA38/46FA3E/46FA45/46FA47/46FA52/46FA56/46FA58 | 显式左样本→权重→左乘→右样本→右乘→加和落F32；表、index、fraction与八象限公式保持 |
| 九助手已有实现和公开声明，但没有各自原ID/完整函数独立基线 | 库存与当前计量快照，九ID语义/分类/成员/文档为空且各有一个直接定义 | 新增九语义记录、分类及BATTLE依赖成员，原可写表同时纳入本基线；其它父消费者未全验，完整成员枚举和门禁保持open |

裁决仅按两处原指令差异落地，不重构成共用转向工具、不用 atan2/sin/cos/fmod、不缓存两次运动的角度/速度、不扩大指针或其它非地址字段。native非拥有action/target/float*按既有x64承载；值参数F32、角差/表返回F64、方向W32和正常转换S32保。方向助手只一次圈调整，表角归一则重复循环，两职责不得合并。

<a id="battle-motion-angle-dependency-acceptance"></a>
## 4. 可度量局部验收

| 项 | 本步通过证据 | 完整验收界限 |
| --- | --- | --- |
| 原函数覆盖 | 九根完整441条1422B，连续全字节/边界、14call、60跳转含一外尾跳、17ret与switch八槽 | 不代219引用的全部父行为 |
| 数值与表数据 | 80B常量、两表3400B与switch32B原字节；两F64表源项逐一对照，零/±180/360、方向八区间及原插值64上界按指令落源码 | 不执行算法样例，不宣异常、PC53、宿主生成指令或跨架构ABI一致 |
| 源码实施I | 两处补丁在完整两CPP精确重放；其余七根、声明、两表/其它源码保持；原空门禁、F32落点与真实调用序定向静态 | 未编译/链接/运行；__ftol与上游owner/共享表修改者、全生命周期仍按总体门禁 |
| 映射/分类S | 九ID独立原库存身份、九adapted分类、BATTLE85→94成员，九组各I/S局部证据18条 | member_inventory_closed=false，全部完整功能gates保持；R1/R7/R8继续 |
| 台账/进度 | 语义1383→1392、有效ID1378→1387、分类489→498、文档关联1496→1505/3680=40.90%、E561→579；七open追加本步引用 | 175待处置队列、60任务源码closed10/全验收0等旧状态保；关联比不是总体完成率 |

按现行限制只做必要文本静态与 measure_cg2d.ps1/-Check，不构建、不调试、不新增或执行测试、不运行客户端/算法、不计算hash。通过内容不得代整体SPEC验收。

<a id="battle-motion-angle-dependency-source-state"></a>
## 5. 实施状态与接续

先落本SPEC再实施两差异、归档原证据，并同步运行文档、语义/分类/登记簿、七项收尾与唯一进度。命名快照：`cg2d-re-battle-motion-angle-dependency-20261006`。九个局部正常函数已纳管；原表/原调用/宽度保持。共同特效父41CD00与各命中回调已有独立记录，不能反向把本步219条引用等同全部父完成。

继续沿真实未完成依赖推进，完整callee/owner、表的全部生产消费、资源重进退出、宿主数值/实际画面及R1/R7/R8仍按总SPEC。网络仅原格式明文，音频暂停；不降低验收标准。

<a id="world-direction-current-20261010"></a>
## 2026-10-10 WDG01：归一化更新前读取的当前补正

[WDG01](cg2d_world_direction_normalization_sampling_spec.md#world-direction-decision)/[原指令和当前source](cg2d_world_direction_normalization_sampling_spec.md#evidence-archive-20261011)明确46F8F0上界比较后46F903另读float，下界比较后46F91E另读float，再加减/F32写回；BOG01实现只保每圈比较采样，却复用了比较值到更新。本次恢复两个局部更新采样，原重复循环、native非拥有float*、F32字段和void返回保持。BOG当时“正确/0补丁/其余七根保持”及旧计量属于历史，保原文/原记录，不沿历史误称当前正确。实际当前声明位于src/world/WorldUpdateDependencies.hpp，1390追加当前来源/纠正证据。

46CDF0新增完整独立身份，两个E8复用BOG插值1389和归一化1390，尾跳ftol旧1252由明确本机S64cast承担；primary正确body、插值114条351B及65项可写原表/旧记录保。两本步根合34条112B/两页，与复用依赖证据不重复计数。BATTLE既有成员422保持，只追加1390当前限定S，MAP/Yobi各加primary及normalizer；三S零I、七任务open/完整gates保。源码重新采样不保证优化后机器加载、x87状态/PC53/ABI或全父/表owner/运行；现ACP01/MC01正常数值边界及WAV-only仍沿最新决议，旧音频暂停措辞仅历史。
