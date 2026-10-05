# BMT01 战斗技能12/82/97与共用目标循环取样

日期：2026-10-06。沿[BMQ01技能50族](cg2d_battle_m_skill50_sampling_spec.md)串行推进正常技能主体。82/97共用现代目标根，涉及它的修改同时核两原入口，按既有工程委托先SPEC后实施；四既有根二十二组/二十二定义/二十二处，不扩技能业务或降验收。

<a id="battle-m-skill12-82-97-sampling-baseline"></a>
## 原端有界基线

| 主体区间 | 指令/字节 | call |
| --- | ---: | ---: |
| 技能12：42C053..42C241 | 116/494 | 14 |
| 技能82：42C241..42C736 | 347/1269 | 33 |
| 技能97与共用尾：42C736..42CB5E | 286/1064 | 31 |

共749条2827B十一页78call，三连续非重叠范围完整保存实际字节/分支，见[证据及源码前态](cg2d_battle_m_skill12_82_97_sampling_evidence.json)。82目标空/255通向42CB2F/42CB37，均已在97范围，只计一次；所有直接目标落本组三范围或已证437A29父回存。沿[BMO实际低段表](cg2d_battle_m_skill4_sampling_evidence.json)202B/有效低case0定位12/82/97，表只作入口上下文，不代其它技能/高段路由。原553E58十二S32偏移48B沿[BMP已证值](cg2d_battle_m_skill61_sampling_evidence.json)保持，父17298统计不代全文。

## 技能12的四处

Take并丢弃mode→Take/GetPosition/pub caster→**Take amount后再live caster/actor**，三Take顺序保持，不能把caster改成跨amount调用的初始解析位置。空actor Finish直接回存，不Move/增58。初始固定caster payload先live skill/pub103C；level字段另live caster→actor→live level→payload/pub1044，再fpUsed1008/Append55、40、139、64。

live level比较12→live caster→低段偏移+110180→actor/payload/pub1028，高段原111017保持。自身4393A0一次caster同时作actor和target，flags0/固定amount；之后另live caster终结-1/0/0。value1018=256→Append80/160→value1030=amount/1034=5各次当前角色保持，末Move/++58，**没有RID**。四处为payload-skill、level字段、图号caster先偏移、自身效果同caster；其它取包/正常零和尾不改。

## 技能82的六处

Take并丢弃mode→Take/GetPosition/pub同caster→同解析位置actor，空actor Finish直接回存；payload先skill、level字段caster-actor-level-payload恢复，fpUsed与25/55/151/8/6/10动作原序保持。

后live position一次比较10，低侧选择base10/other15，高侧base5/other0；两组X各**两索引→baseX→同position actor→otherX→deltaX→该actor payload→中点/pubFE8**，不得把actor或payload延迟到完整中点后。正常S32差/向零除2/加base保留，不模拟越界或溢出。Y沿同侧选择分别重读两索引→baseY/otherY→中点，再live caster/actor/payload pubFEC，不跨轴缓存索引或把X actor用于Y。value1018=288及80/85/36/91/86/37/89各次live角色保持。live caster先player比较，首256/52沿同位置，52后51再live caster；随后既有共用目标根。六处为同解析caster、payload-skill、level、X交错取样、Y两索引、player首动作同caster。

## 技能97的四处

同82初始两Take与同解析caster借用/空Finish返回，payload-skill和level字段恢复。25/55/40/151/139/64之后live level比较12，低段原**偏移读取→live caster→偏移加110052→actor/payload/pub1028**；高段另live caster/原111001，不套用12的caster先偏移次序，也不提前完成偏移加法。现代低/高分支保存image与imageCasterPosition，不新增默认或保护。

value1018=5及80/88/87/86/89原顺序/每次当前角色保持，再进同现代目标根。四处为同解析caster、payload-skill、level及图号偏移-caster-加法。

## 82/97共用目标的八处

两原循环42C50C与42C905都完整核，核心读取/消费一致，保既有现代共用根，不新增原函数身份。原入口[BMG正常零](cg2d_battle_event_execution_gates_spec.md)初始化两图号/三二元素S32数组八W32槽；共用根只保一次初值，跨目标沿旧值，无每轮清理或新增默认。

Take/GetPosition→255比较→pub target→同解析位置查actor；255退出，空target Finish后仍共用Move/RID/++58尾。非空Take flags/pub→Take amount，再live flags800000才43BF30(scene/类型0/两图三数组)。439400原firstImage→live flags→target→caster→actor，保存原序再传既有16B非拥有效果记录。两firstImages索引原S32，取两元素首个>0，否则索引0。之后43BC10原selectedImage→secondImage→firstImage→live flags→target→caster→actor，不跨两次效果合并读取。

之后两个target字段分别target→actor→live skill或level→payload/pub103C/1044，然后live W32 flags入寄存器。原AL/AH局部108/40门禁、amount与3位、40路径Append34后target先player比较/首256-52同位置、51重新live target、165/27先kind再target/actor保持，动作后原live flags重取。amount非0或flags108才11并重取flags；flags20且非10000才46。所有局部bit均W32寄存器载体，不把全局flags改成byte。八处为target条件/pub同位置、主效果参数、两S32索引、辅效果参数、两个target字段、player、kind取样。

## 共用尾与边界

42CB2F空目标Finish后42CB37与哨兵路径汇合，live caster Move→RID→固定scene payload58递增→437A29。97范围已包含全部共用尾，不重复计数。三初始空caster都Finish直接返回，不套入尾；12正常尾不RID，82/97正常尾RID。取包/原正常零、各常数/位宽/调用后live及列表重复保持，不新增owner/资源释放/公开接口或F/X/adapted/分类/完整成员。网络原格式明文/音频暂停持续。

<a id="battle-m-skill12-82-97-sampling-decision"></a>
## 二十二组差异裁决

| 根 | 修改数/裁决 |
| --- | --- |
| ProcessBattleMSkill12 | 四：payload先skill、actor先level、图号caster先偏移、自身效果同caster；amount调用后的live caster保持。 |
| ProcessBattleMSkill82 | 六：同解析caster、payload-skill、level字段、X交错索引/actor/payload、Y两索引、player同位置首动作。 |
| ProcessBattleMSkill97 | 四：同解析caster、payload-skill、level字段、低段偏移-caster-加法；与12不同图号次序保持。 |
| ProcessBattleMSkill82And97Targets | 八：两原循环均核；target比较pub/同位置、主辅参数、两S32索引、两字段、player、165/27 kind；其余消费保持。 |

所有其它源/头/主体/正确核心保持；native非拥有T*只局部借用，正常S32数学/原业务门禁与原零保持，不新增输入拒绝/补零/钳制/越界保护、故障或回绕模型，不宣异常等价。

<a id="battle-m-skill12-82-97-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 三主体749条2827B十一页78call连续字节/直接目标闭合，共用尾只计一次；202B入口映射三case仅上下文，48B偏移和正常八W32初值沿已证来源。 |
| 实现 | HEAD整CPP四根二十二组二十二定义二十二处精确差异；字段/两种图号序、82 X/Y交错读取与两循环参数/索引/条件pub有限静态，其它正文/头保持。 |
| 消费 | 12三Take/amount后live caster/无RID、82/97两Take与两循环/不同准备/正常尾RID，初始空与目标空不同出口、图值跨目标与所有调用后live/原零/重复列表保持。 |
| 台账 | 旧812仅有界追加四根，不新身份/adapted/分类/完整成员；双域四局部I/S/七任务open、全部完整门禁/其它指标保持。 |
| 限制 | 必要静态与获准measure/Check，不构建/测试/运行/摘要，不宣编译指令/SDK/SEH/ABI或异常等价。 |

<a id="battle-m-skill12-82-97-sampling-source-state"></a>
## 当前状态与后续

三技能正常主体/四现代根二十二处实修，完整有界749条2827B纳管；两原目标循环及共用尾纳入同一步，不把82的修复未经取证外推97。其它各技能/全路由、全部列表消费者和资源owner重进退出/借用失效、完整父与SEH、SDK/ABI/PC53/时钟RNG/画面及整体R1/R7/R8继续，不以有界完整主体代完整函数或整体。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E389→393仅双域四局部I/S。关联率不是完成率，本组14文件；固定快照：`cg2d-re-battle-m-skill12-82-97-sampling-20261006`。
