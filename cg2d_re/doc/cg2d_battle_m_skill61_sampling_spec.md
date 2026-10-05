# BMP01 战斗技能61/62/63/83共用主体取样

日期：2026-10-06。沿[BMO01技能4](cg2d_battle_m_skill4_sampling_spec.md)串行推进下一正常主体；按既有工程委托先SPEC后实施，单既有ProcessBattleMSkill61Family十九组/十九定义/十九处，不扩技能业务或降验收。

<a id="battle-m-skill61-sampling-baseline"></a>
## 原端有界基线

原42AE60低段表cases61/62/63/83→42B600，沿[BMO实际低段表](cg2d_battle_m_skill4_sampling_evidence.json)的有效低case0/21W32跳槽84B+118W8索引118B=202B保存路由上下文，不把间接表raw lowcase共享值当有效低case，也不代全路由。主体42B600..42BEAC完整566条2220B八页56call，全部直接跳转落本块或已证437A29父回存，见[字节、分支、前态及表](cg2d_battle_m_skill61_sampling_evidence.json)。原553E58十二S32效果偏移表48B为3/3/3/2/2/2/1/1/1/0/0/0，与既有源码一致，不改表；父17298统计不代全文。

### 入口和83准备

先Take并丢弃mode，再固定scene payload value00=0/count04=0、message59FA14=0，原正常零写保持。Take/GetPosition→pub caster同解析位置→查同actor；空caster Finish后直接回存，无RID/58。初始caster payload先live skill/pub103C；下一字段各次live caster→actor→live level→payload/pub1044，fpUsed1008及Append25/55/40原序/每次当前角色保持。

83的X/Y各先读pointIndices[15]和[5]，再依次读两坐标，以原正常S32 x15+(x5-x15)/2及y15+(y5-y15)/2向零截断，再live caster/actor/payload写FE8/FEC。两坐标分别重新取索引，不跨轴合并。Append8/6/10/64/137后live position比较20，再同position actor/90或198；之后另live caster→player比较，首256/52同位置，52后51重新live caster。非83 Append139/64后live level比较12，再live caster位置，低段才读553E58[level]+110081，actor/payload pub1028；高段原111014保持，不新钳制。

### 目标、累计与反向效果

每目标Take/GetPosition→比较255→pub同target→同解析位置查actor。255退出；空target Finish后退出循环，后续仍效果-1终结/累计/Move/RID/58=2。非空Take flags/pub、Take amount S32，再live flags；10000命中先读累计value59FA14，再target，pub flags5ABA84后才作S32累计加法/寄存器AL清20，依次pub lastTarget5ABA90、total59FA14和flags，再固定payload参与者槽[count]/live count递增与live flags重读，原重复位置/列表保持。

正向4393A0先live caster→target→actor，沿已取flags/amount调用。之后两个target字段分别target→actor→live skill/level→payload/pub103C/1044，不跨字段复用actor。sourcePosition读取后先比较20，再targetPosition；149门禁的各不同<10/20/21路径与调用后source→target重取保持。

live flags选响应优先40000→20000→10000→无；sameSide仅source20且target<10或source21且target>=10。sameSide沿已取source/target/flags/amount反向4393A0，再每次live target Append184/74，74后重取target；不跨调用重取flags或改变已判响应。40000 Append126后循环，10000 Append124后循环；20000依125/159/32、value1030=amount/value1034=6、live W32 flags10000000时正常零写，然后**42BB9A原W8全局flags20门禁**才各次live target Append46/47。无响应非sameSide才160；sameSide反向184/74后仍pub amount/1034=5，live W32 flags10000000才零。原AL/AH寄存器局部运算保持W32载体，只有42BB9A改原真实字节读取，不能误改所有flags。

### 终结与累计回送

效果-1/0/0终结当前caster后live累计非0才回送。分别live lastTarget→actor→live skill/level→payload/pub，flags5ABA84原W32清10000，再各次live last/caster依Append1/140/0/149/160。value1030按live caster→actor→live total→payload/pub；1034=5另借当前caster。最后反向4393A0原**live total→flags5ABA84→caster→lastTarget→actor**，之后再live lastTarget Append74。末live caster Move→RID→固定scene payload58=2；初始空caster不走此尾，目标空与哨兵走此尾。全部正常零、值宽/图表/重复列表、调用后live和父回存保持。

<a id="battle-m-skill61-sampling-decision"></a>
## 十九组差异裁决

| 组 | 恢复 |
| --- | --- |
| 1–3 入口 | 同解析caster pub/查actor；payload先skill；caster-actor-level-payload。 |
| 4–5 中点 | X/Y各两索引先两坐标，不跨轴缓存；正常S32 /2保持。 |
| 6–7 83动作 | 90/198条件先actor；caster先player比较/首256-52同位置、51仍live。 |
| 8 图号 | level比较12后caster先低段偏移表取值，原高段常数保持。 |
| 9 目标 | 255条件先pub/同解析target查actor。 |
| 10 累计 | 旧total先target，flags pub后累计加法；原后续pub/参与者顺序保持。 |
| 11 正向效果 | caster-target-actor读取顺序固定。 |
| 12–13 目标字段 | 两次各target-actor-skill或level-payload。 |
| 14 149门禁 | source比较20先target取样，原后续重借与<10/20/21保持。 |
| 15 后续46/47 | 20000路径全局flags20原W8对象字节，其他W32/寄存器局部位不改。 |
| 16–17 累计目标字段 | 各lastTarget-actor-skill或level-payload。 |
| 18 累计caster字段 | caster-actor-live total-payload/pub1030。 |
| 19 最后效果 | total-flags-caster-lastTarget-actor，不复用此前字段的total/actor。 |

十九处落码，其它源/主体/Header/Waiting/五整数、布局/所有正确核心保持；不新增原F/X/adapted/分类/完整成员、公开接口或owner，native非拥有T*仅局部借用。网络原格式明文/音频暂停持续，不设新越界/数学溢出、输入拒绝、补零/钳制或异常确定值模型。

<a id="battle-m-skill61-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 566条2220B八页56call连续实际字节/直接目标闭合；低段202B表逐case真实映射和四技能定位，仅上下文；48B十二S32偏移与源码对应。 |
| 实现 | HEAD整CPP单根十九组十九定义十九处精确差异，字段/中点/参数与条件pub序和一W8有限静态，其它正文/头保持。 |
| 消费 | 三正常入口零、83与非83准备、10000累计参与者、149各门禁/响应优先级与sameSide反向消费、初始空caster/目标空不同出口、终结/累计回送/Move-RID-58=2与调用后live保持。 |
| 台账 | 旧812仅有界追加，不新身份/adapted/分类/完整成员；双域四局部I/S/七任务open及所有完整门禁/其它指标保持。 |
| 限制 | 必要静态与获准measure/Check，不构建/测试/运行/摘要，不宣编译指令/SDK/SEH/ABI或异常等价。 |

<a id="battle-m-skill61-sampling-source-state"></a>
## 当前状态与后续

四技能共用正常主体十九处实修，完整有界566条2220B纳管。入口202B仅四case定位，原48B偏移表保持；其它各技能主体/全路由、全部列表消费者和资源owner重进退出/借用失效、完整427470父与SEH、SDK/ABI/PC53/时钟RNG/画面及整体R1/R7/R8继续按真实缺口开发验收，不将有界完整主体代完整函数或整体。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E381→385仅双域四局部I/S。关联率不是完成率，本组14文件；固定快照：`cg2d-re-battle-m-skill61-sampling-20261006`。
