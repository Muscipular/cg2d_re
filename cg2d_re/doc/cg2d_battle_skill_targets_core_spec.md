# BNT01 十参技能目标生产与独立身份

<a id="battle-skill-targets-core-baseline"></a>
## 原端、身份与范围

原库存 `F:00026FC0` 为独立426FC0..427461（半开）单chunk：369条1185B、五页（每页至多80条）、32call、无switch，两条真实ret；全部直接出口在本函数。[证据](cg2d_battle_skill_targets_core_evidence.json)保存完整指令/字节/调用/出口、原十三条31B图号选择叶与两表40B、唯一实际4369CC父调用/26条94B调用上下文、四补丁和改前根。

当前 `TakeBattleMSkillTargets_426FC0` 已有完整源码/3101真实调用，但原语义表没有该ID、基线和独立分类；历史定位含此定义，因此它不在派生175项定位队列。仅补这项身份缺口，不把“不在队列”当作已验。源码行为四处差异本步修复；不将此独立函数并入父427470旧812，旧812整记录保持。

原十参按实际栈和4369CC对齐：scene、skill、level、technique、fpUsed、attackMode及四独立S32* source/initial/target/redirect。scene和输出地址由native非拥有T*承载，业务值/表项/位置仍S32，flags仍W32。原先GetPos返回先写initial再source，沿同返回位置取初actor；source/initial/target/redirect使用caller实际四个全局S32 storage，不复制为临时结构/单合并输出。原caller在函数返回后重读**initial**输出Move、RID、58=EDI1，不改为source；本步保原调用根/实参顺序与header。

图号54D710五S32为111251..111255、54D724五S32为111261..111265，共40B。404D90原十三条31B按count<=0/index<0/index>=count返回业务0，否则读values[index]；既有SelectIntValue实现/正常门禁保持，这个原门禁不是本步异常保护。初图用第一表/technique；每target按target<10第二表，否则第一表，调用后重读source写102C。427143原800000立即数虽显示成地址标签表达式，已按只读操作数确认，不当地址字段。

<a id="battle-skill-targets-core-decision"></a>
## 裁决与四处实修

沿用户工程委托先SPEC后实施。四处只恢复已证顺序；不新增API、owner、依赖、默认/保护或异常模型。

| 补丁 | 改前差异 | 原端与修复 |
|---|---|---|
| 01 | 先发布target输出再作255比较 | GetPos固定返回先255比较，再写同target输出，沿固定比较分派；初/target空和255退出仍原分工 |
| 02 | 普通Effect source/target输出取样隐含 | fixed图/金额/flags，target输出→source输出→actor，固定样本调用 |
| 03 | Aux source/target和附图取样隐含 | selected→secondary→primary→target输出→source输出→actor，沿此前固定flags |
| 04 | redirect发布后source/target参数顺序隐含 | 同GetPos返回写redirect输出后，target输出→source输出，固定本次flags/value/redirect调用 |

原生产、消费与返回保持：

- GetPos先initial/source双写、同返回actor判空；初空Finish直接return。基础103C/1040/1044/1008/FF4沿固定输入值、各次source/actor/payload；第一表选择写102C，25/55/151/64/255/36，各次live source。没有入口总量、复制队列或私有影子状态。
- 每target先GetPos固定返回，255发布后casterEffectList(-1/0/0)→另live source74→return；空actorFinish→return，均不加Move/RID或额外收尾；caller另消费原initial输出。其它target取fixed W32 flags再value；每轮first/second图及三个二项S32数组八业务零，800000才AuxGraphics(mode0)，没有发布或重读全局resultflags。
- 普通Effect后扫描values两槽，**取最后一个零**；两槽都非零沿原selected0，并用firstImages[selected]，不得改为首正图/首零/按firstImages扫描。两个选择/遍历索引已是S32，原逻辑保持；每轮初始化，不把父M技能跨target状态带入本函数。
- Aux后target<10选择第二表，否则第一表；SelectIntValue(5,technique)后重读source/actor写102C，再target25。fixed flags200才额外Take/GetPos/redirect输出发布→AppendRedirect，之后另live target11，调用后从redirect输出重读写回target。四输出地址/别名取样与固定value/flags分工保持，不回填initial或把redirect合并到source。
- fixed flags8/100优先target11；40/80分支分别34/35，之后target先current比较，256/52沿同target、51另live。否则8000→129、4000→128、2000→127、40000→126、20000→125、10000→124，flags3按bit2选择165/27；最后20才46，循环继续。全部flags沿本次W32参数，不增加新live全局读取。
- 正常255与初/target空两种返回保持，最终两个原ret/栈参数消费单列；源码void接口和caller忽略寄存器返回沿既有接线，不宣ABI/编译后指令形态或故障等价。其它根/属性、图表和全部关联声明保持。

## 身份与计量裁决

依据完整原库存范围/正常本体与唯一caller，新增语义记录1350绑定F:00026FC0，不新F/X库存、不造代码片段或替代身份。按已批准x64 native非拥有地址承载登记 `adapted`：原函数全部正常控制流/输出与图表纳管，原W32数据不拓宽；分类不代表运行通过。FUNC-BATTLE显式原成员52→53纳入该技能目标生产者，完整成员枚举仍未闭；YOBI55/RENDER26/LOOP32不改。仅BATTLE两条局部I/S，不把此函数冒充完整战斗或父427470。

RPLAN.24不新增补零/拒绝/钳制/越界溢出/回绕或故障模型；原404D90业务0/八每轮零保持。网络原格式明文/音频暂停，SDK/全部owner失效、父SEH/画面-时钟RNG与运行边界保持。

<a id="battle-skill-targets-core-acceptance"></a>
## 可度量验收

| 项 | 当前证据与限制 |
|---|---|
| 源码I | 单根四定义四处；整CPP从HEAD精确重放，全部其它根/属性、真实3101调用/header和声明保持 |
| 原端S | 完整369条1185B五页32call/全部出口与两ret；十三条31B选择叶/两40B图表及26条94Bcaller上下文分别计数 |
| 数据/接线 | 十参/四独立S32*及原取样-别名读写，fixed flags/value、八每轮零、最后零扫描/所有分支-两返回与caller消费initial保持 |
| 登记 | 新独立记录1350/F:00026FC0、adapted分类/战斗成员+1；旧812与全部其它记录不变，七open同步 |
| 运行与整体 | 不构建/测试/运行/摘要；局部I/S不代全部consumer/owner、ABI/SDK、父SEH/时钟RNG画面及R1/R7/R8/整体 |

预期语义1350→1351/有效ID1345→1346，分类456→457，文件关联1463→1464/3680=39.78%，E495→497仅BATTLE两局部I/S。定位队列175/174缺位置、55待范围映射/16暂停/103排除/1元数据、R7区间67/3061、片段5、60源码closed10/完整验收0/P01和七open与所有完整门禁保持；该ID已有历史直接定义，所以队列不减少。派生看板/快照由获准汇总器生成，不手改指标。

<a id="battle-skill-targets-core-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#TakeBattleMSkillTargets_426FC0`；caller `ProcessBattleMSkill3101` 只保持原接线，本步不改它。快照 `cg2d-re-battle-skill-target-core-20261006`，独立原ID、provider/十参/四输出和原字节可跟踪。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)及[看板](../plan/cg2d_measurement_dashboard.md)同步四处开发与身份收口；原函数源码登记不代完整业务/生命周期/运行验收，R1/R7/R8继续。
