# BNU01 三方转向与辅助效果独立基线

<a id="battle-redirect-aux-baseline-baseline"></a>
## 原端与追踪缺口

原库存F:000242B0为4242B0..4244F1单chunk180条577B三页28call、无分支/switch、一ret；F:0003BC10为43BC10..43BC84单chunk49条116B一页三call、一条件分支/一ret。两完整函数合229条693B四页31call。[证据](cg2d_battle_redirect_aux_baseline_evidence.json)保存全部实际指令/字节、调用/出口、两个完整改前根、入向/返回消费上下文和当前独立ID追踪前态。

两根当前源码与原正常合同一致，本步保持，不为计数制造改动。缺口是两原ID无语义记录/基线/分类和战斗成员；历史直接定义均存在，原175定位队列不含它们。不能以源码定义存在或不在队列代替可跟踪基线。

转向普通call五处/两个父：424620内424C47/424D38/425155/42583A，426FC0内42727F；原返回消费窗口分别保存。前两和42727F重读其它位置后续动作、未消费EAX；425155/42583A把返回EAX保存EBX。当前源P01四表达式前两忽略、后两赋secondaryPosition，BNT调用明确忽略，原分工保持；父全文沿[完整攻击生产](cg2d_battle_attack_targets_spec.md)和[独立技能目标](cg2d_battle_skill_targets_core_spec.md)，本步不重验父全文。

辅助效果普通call37处/14父，全部地址/父身份列证据，包含P01、BNT和父427470。源表达式与原共享路径分开，不按字面数推原路径覆盖；本步不宣14父全部接线/生命周期完成。动作追加/效果列表追加和普通Effect既有接口保持，沿[BQA01](cg2d_battle_reset_append_baseline_spec.md)及[BQR01](cg2d_battle_queue_readiness_sampling_spec.md)已证基线；不重复复验未变生产叶。

<a id="battle-redirect-aux-baseline-decision"></a>
## 原生产与消费合同

### 三方转向

六参：scene native非拥有T*、source/target/redirect S32、flags W32、value S32；S32返回redirect。source、redirect、target可相同，保持原完整序列，不合并/过滤别名。各次gBattleEntries取actor保持，不将首actor缓存贯穿所有动作；scene借用写入由既有AppendCast承担。

| 阶段 | 原完整消费 |
|---|---|
| source | 74→EffectList(redirect,flags,value)→25→73→0→25 |
| redirect起段 | 0→EffectList(target,flags,value)→另live actor/payload写100C=target→41 |
| redirect字段 | 41后另live actor/payload写1018=252→80；80后另live actor/payload写1010=0x5748→69 |
| redirect剩余 | 36→75→7→4→44→73→10→76→74→EffectList(target,flags,value) |
| target | 0→28→EffectList(source,flags,value)→73→74→64→1 |
| 返回 | 原EDI/源码S32 redirect返回；两父各次调用和两个消费返回的位置保持 |

24次Cast/四次EffectList合28call。三字段均原W32，当前位置/flags/value是固定输入值；每次actor/payload取样依原，不将字段指针跨41/80等动作保留。原没有本函数actor/source范围guard，不新增判空、容量保护、默认字段、RNG/Move/RID或owner/free。

### 辅助效果

六参：action native非拥有T*、position/value3/value4/value5 S32和flags W32，void接口沿既有源。原flags800000无则直接return，先判flags、不访问action；有则取得固定payload、S32 previous=CA0-1后跨两次Append保留。

| 阶段 | 原完整消费 |
|---|---|
| 门禁/前态 | fixed flags800000；payload先取得，CA0-1固定，不重新取当前count |
| 第一追加 | Effect(action,{position,flags,value3,value4}) |
| 第二追加 | Effect(action,{position,flags,value5,0})，0是原业务值 |
| 第三取样 | 两次追加后，原旧槽position4→flags→position3→position读取到固定16B值记录 |
| 第三追加 | Effect(action,旧完整记录)，不是再次传输入flags或同一target |
| 返回 | 真支共三call，假支零call，共用原ret；无额外列表End/状态动作 |

原43BC6A flags从payload+((previous+2)*16)读；按原effects1C/四W32记录的正常S32位移关系，正是旧槽flags(+0x20)，与position4(+0x28)/position3(+0x24)/position(+0x1C)分列。当前typed旧槽四字段读取和固定payload/previous保持；不把乘/位移关系改成回绕模型，不造异常/负索引故障门禁。action真支没有新判空；八每轮业务零属于上游取包器，不能移入此叶或复制整个列表。

## 登记裁决

沿既有工程委托先SPEC后登记：新增语义记录1351/F:000242B0、1352/F:0003BC10绑定已有原库存，不新F/X/片段；各native provider/source_body_implemented记录既有正确源码，不称本步源码新增。两项按获准x64 native非拥有地址承载登记adapted，S32/W32/16B值语义不拓宽，ABI/全部owner和运行仍未验。

FUNC-BATTLE显式成员53→55纳入两生产者，完整枚举继续open；其它域成员及全部旧语义记录保持。每函数各一条局部I/S共四E，仅纳当前已实现合同及完整原根，不代完整战斗/14父/所有owner或运行。原RPLAN.24、网络原格式明文与音频暂停保持，不增依赖/API/保护/补零/钳制/溢出/回绕或故障模型。

<a id="battle-redirect-aux-baseline-acceptance"></a>
## 可度量验收

| 门禁 | 本步限定证明 |
|---|---|
| I | 两既有完整正确根和全部CPP/头文件等于HEAD，24Cast/四EffectList与三Aux Effect原调用/三字段、两返回和调用者分工静态一致 |
| S | 两函数229条693B四页31call/全部实际字节、两ret与一条件出口；五转向入向/返回窗口和37辅助入向分列，父全文不计通过 |
| 数据/生命周期边界 | 固定六参、每次live actor/三字段与三方别名序列，辅助固定payload/CA0-1跨两Append/旧四字段顺序；不新owner/free或补值 |
| 登记 | 两独立ID/语义-分类、战斗成员+2及每函数I/S；七open与唯一进度同步，旧记录保持 |
| 运行/整体 | 不构建/测试/运行/摘要；完整父/所有consumer-owner、SDK/ABI/PC53/时钟RNG画面及R1/R7/R8继续 |

预期语义1351→1353/有效ID1346→1348、分类457→459、文件关联1464→1466/3680=39.84%、E497→501四局部I/S、战斗成员55。YOBI55/RENDER26/LOOP32、R7区间67/3061、175定位队列/174缺位置-55待范围映射/16暂停/103排除/1元数据、片段5、60源码closed10/完整验收0/P01及七open-全部完整门禁保持。两ID原有历史定义不在队列，不虚减定位队列或宣R1闭合。

<a id="battle-redirect-aux-baseline-source-state"></a>
## 源码、进度与剩余

源码 `AppendBattleRedirect_4242B0`/`AppendBattleAuxEffects_43BC10` 及所有caller保持，缺失独立合同/身份与分类本步解决。快照 `cg2d-re-battle-redirect-aux-baseline-20261006`。[唯一进度](../plan/cg2d_full_reconstruction_progress.md)与[看板](../plan/cg2d_measurement_dashboard.md)同步实际登记，保持“已实现/已登记/已验收”分开；完整父/全部owner-资源生命周期、运行与R1/R7/R8继续。
