# 战斗场景分字/死亡状态与取样SPEC（BPY01）

日期：2026-10-07。按既有工程委托先記取舍再实施；上承战斗收尾、原字段/别名和追踪验收体系。

<a id="battle-scene-words-death-sampling-baseline"></a>
## 原端基线与源码缺口

| 原入口 | 当前源符号 | 条数/字节/页 | call/jump/ret |
| --- | --- | --- | --- |
| 0x00412F70 | UpdateBattleAction210_412F70 | 71/237/1 | 10/3/4 |
| 0x00413070 | ApplyBattleSceneEffects_413070 | 42/156/1 | 2/4/1 |
| 0x00413110 | UpdateBattleAction212_413110 | 49/157/1 | 4/2/1 |
| 0x004131B0 | UpdateBattleAction213_4131B0 | 71/251/1 | 5/8/3 |
| 0x004132B0 | UpdateBattleAction214_4132B0 | 89/275/2 | 6/6/5 |
| 0x004133D0 | UpdateBattleAction215_4133D0 | 9/34/1 | 1/1/1 |
| 0x00413400 | UpdateBattleAction216_413400 | 108/335/2 | 12/13/2 |

七完整根439条1445B/九页、40call/16入口、37普通内部jump/17ret，一原四项switch；七唯一404DB0父call的21条邻窗。210真实rand一次后&3，413012四项跳转表→413019/302A/303B/304C，图110004..110007；源连续图表达式与借用返回正确，不改RNG序或造新表源。

211四原W8 TEST在413084/30A5/30C6/30E7，四W16 MOV在41308D/30AE/30CF/30F0，发布在413092/30B3/30D4/30F5。源W32旗位表达式和S32 position3转W16扩大读取宽度；恢复各cached index的低W8/低W16对象表示。四目标发布位置保C8A768/+2/C8A76C/+2，共享W32引用仍别名gMapCharaSceneSlots同owner，不拆高低槽或复制新state。

212在坐标发布后41315C是MOV AL低W8，保60门控；214原4132D9先读flags/4132DC先读X，4132E2再清field58/4132E9发布flags，源原先先清field58才读取；216每轮4134A8先currentIndex、4134AE再自身position18、4134B4后目标position，源查目标后才读自身位置。恢复三个实际读取阶段，所有业务/回收原分支保持。

**同依赖R7区间**：X:0001305D-00013070全部19B逐字节owner/xref核对；前41305C实际RET/函数块恰结束305D，下一413070真实函数起点。305D..3060为8d4900，解码LEA ECX,[ECX+0]三字节对齐，不能称纯NOP；3060..3070为四W32地址表，原413012唯一地址引用同210所有实际项。无函数owner字节，表项落在已捕210实际case；按两部分mapped字节用途，不增F/运行验收或范围排除。

<a id="battle-scene-words-death-sampling-decision"></a>
## 实施裁决与正常完整合同

只四体七阶段：211各W8读取后条件成立才memcpy低W16源值再原memcpy目标发布；212低W8；214先捕flags和X；216每轮按原索引/自身/目标序捕获。保持W32/S32原字段和写者，W16读用memcpy避免不兼容类型别名。正常旧值相同；原byte/word消费和读取/发布顺序恢复不宣称正常数值已错。

1. 210：characterAction3/speed16/rate50；currentIndex→共享目标先Turn(action,target,1,0)，再fresh currentIndex/共享目标供Distance；有符号距离<70才439340后speed8/rate100，否则Move。共同PlayFrame0弃返，然后一次rand53B4C9 &3，经四实际表项生成110004..110007的PositionEffect，返回借用T*供父弃用。保调用/RNG消费序，不把两个共享目标读取合并缓存。

2. 211：payload固定；四阶段各fresh currentIndex W32，分别current.flags实际低W8的1/2/4/8决定是否同一阶段cached index.position3低W16读并发布到C8A768低/高及C8A76C低/高。W32字段/其它16位和共享场景owner不变；每次值读用memcpy对象表示，避免把S32转成新W16类型字段或错用uint16_t类型别名。最后439470弃返→439340弃返，四条件各独立不串else。

3. 212：current.position供RandomEffect(action,image,0)，原返effect借用，无额外创建失败守卫；fresh index.position3 W32后effectX读加写，再fresh index.position4 W32后effectY读加写；之后FDC低W8/60清才PlayFrame0弃返。fresh FF0先加1发布、fresh current.flags W32按S32阈值比较，count>=threshold才FF0清0→439470→439340；保坐标/索引重取及正常业务阈值。

4. 213：FDC W32/10000000置位时捕原phase写FF0、phase164并返回旧phase；其它characterAction10/rate100→PlayFrame1，返回不等1直接返回。等1才fresh FDC W32/1000决定scaleX/Y F32各1.0，之后fresh FDC清1000；依序一次借105C/1054/1068/1058，非空MarkDeath后清对应槽，空槽不额外清写。最后fresh13B0非0phase220，否则214，返回13B0；保真实回收次序/条件清槽和返回消费。

5. 214：先Has3D。无3D时FF0先增发布，field58>=8才先读W32 action.flags和X、field58清0、flags|8000发布、声218(x)和type36效果(action,0,0,36,0,1)弃返、phase215；小于8且count>4时FF0清0，fresh flags及field58捕获后flags^2/step+1依序发布；其它等待。存在3D时fresh FF0非255先255发布/声218(x)一次，低W8 alpha先取后308写8；先前alpha0则field58/FF0清0后439340，否则RisingEffect后fresh alpha W8小于4置0、其它减4。保这个原业务下限、W8存储/首读后fresh读；原声第三Y不消费，音频暂停。

6. 215：fresh payload1024恰2才写0→439340；其它等待，原无新effect槽操作。

7. 216：characterAction6；payload1024恰0才type34效果(action,0,0,34,0,1)返回写借用+10槽/FF0清0。fresh frameEvent处于[10000,20000)时，一次借+10指针，非空MarkDeath后清+10、循环freshcurrent.position直到255；每非255项Seed(position+ownPosition)，fresh skill400才type35效果(action,0,0,35,0,0)，随后每轮先fresh currentIndex、再自身position18、后该index.position，若不同供共享59E980目标439340；本动作439470推进后循环，255出口再439470。无论+10是否空均frameEvent清0，随后PlayFrame1；恰1时一次借+10非空回收/清槽，payload1024清0→439340。共同尾fresh +10非空才fresh skill400 draw3或401 draw1；保两回收点缓存、借用寿命/原循环推进与seed消费，不加固定次数或未知容量保护。

七完整439条1445B/九页、40call/16入口、37普通内部jump/一四项switch/17ret、七唯一父21条邻窗及210原19B未分配区间。四源体七阶段：211四W8条件/四W16值、212一W8门控、214先flags/X后发布及216每轮索引/自身位置/目标位置读取序；正常旧值一致，不称数值/并发或生成机器故障。三其它正确体和其它字段W32/F32/T*、原业务/返回/RNG/seed与现有场景共享owner保持，R7只本区间实际LEA三字节/四项表16B用途mapped，不代软件或排除。只当前完整正常源接线静态，不外推递归父/全部子依赖/其它生产和owner寿命、SDK/生成机器ABI/画面运行；音频暂停与RPLAN.24不专项异常/保护/回绕/故障等价保。

<a id="battle-scene-words-death-sampling-acceptance"></a>
## 限定验收

- 七完整原根、实际字节连续边界、40call/16入口、37普通内部jump/四项switch/17ret、七入向21邻窗；210四图及同依赖19B实际边界/LEA解码/全字节owner-xref和唯一表引用可追。
- 整YobiRuntime只四体七阶段；211四W8/四W16、212一W8、214缓存先发布和216每轮读取序，与原条件/循环推进/回收/共享场景owner/业务上下限及RNG/seed一致。三正确体、其它全源和owner声明/消费保持。
- 新增1462..1468七原已有定义正常语义/equivalent：语义1462→1469/有效1457→1464、关联1575→1582/3680（42.99%）、BATTLE139→146枚举open、分类587→594、E748→750仅两局部I/S，各十六实来源。R7 259→260/3061、未处置2802→2801只当前19B用途；差队列172/待归属43和全部完整门禁不变。
- U08/R01/R03/R04/V04仅当前/引用仍open，60源码closed10/完整验收0/叶0/global_accepted=false保持；完整战斗/生命周期/运行及R8未关闭。

<a id="battle-scene-words-death-sampling-source-state"></a>
## 当前交付

211/212/214/216七读取阶段恢复，七完整动作/共享场景发布与R7同依赖区间纳管；快照 `cg2d-re-battle-scene-words-death-sampling-20261007`。未编译/调试/测试/运行/摘要，音频暂停、原格式明文及原验收标准保。
