# BNA01 战斗技能98 mode17、状态目标与默认分派

日期：2026-10-06。沿[BMZ01两伤害路径](cg2d_battle_m_skill98_mode15_16_sampling_spec.md)补完最后原端分支；按既有工程委托先SPEC后实施，两既有根11组11定义11处（17五/状态六）。原wrapper和默认正确核心仅纳管、不修改，不扩业务或降低验收。

<a id="battle-m-skill98-mode17-status-sampling-baseline"></a>
## 原端有界基线

| 范围 | 指令/字节 | call |
| --- | ---: | ---: |
| mode17主体42E6A7..42E7F6 | 99/335 | 10 |
| 状态目标42E80D..42EAE6 | 179/729 | 17 |
| mode17终结参数尾42E7F6..42E80D | 7/23 | 0 |
| 状态目标空Finish/终结尾42EAE6..42EB02，沿BMX范围 | 9/28 | 1 |
| 共用消费尾42EB02..42EB32，沿BMX范围 | 12/48 | 3 |

五范围306条1163B八页31call，见[完整字节、分支与源码前态](cg2d_battle_m_skill98_mode17_status_sampling_evidence.json)。全部直接目标落本组或已证437A29父回存；mode17哨兵先两EBX正常零压栈，再caster/-1/actor，状态目标255走EAEF、空actor先EAE6 Finish后仍EAEF；都入EB02终结/Move-RID-增58。尾中E7F8..E80D五条21B沿BMZ，两个BMX尾21条76B沿当前证据，共26条97B/4call非新增；本步新280条1066B/27call分列。

最后模式比较42E69E..42E6A7两条9B/0call另列：17相等进E6A7，不等直接EB0A，所以**默认只有Move-RID-增58，不走效果终结**。既有wrapper默认正确保留。一次mode读取/比较链和全部19模式入口沿BMW/BMX/BMY/BMZ，状态目标0..5/9..11/18在BMW前缀EDI=5时提前转E80D，不能误用其它模式后续EDI=6；原SKL202B表和BMO八W32正常零仅上下文。

## mode17五处

目标Take/GetPosition→255比较先pub同target；原无actor判空/Finish。Take flags固定W32返回值先pub global，800000条件沿返回值，不能发布后重新读取全局当首次条件；有辅助图取包才按原重新读flags，否则只将firstImage原业务值置0，不重置secondImage或三数组。

两个选择计数恢复原S32，首正firstImages索引/默认0保持。先aux参数selectedImage→secondImage→firstImage→live caster→固定flags→live target→actor，flags是Take返回或取辅助图后重读所得，不改成此刻live全局；aux调用后再firstImage→live flags→target→caster→actor追加普通Effect(固定amount0)。不沿15/16的“普通效果先于aux”顺序，不跨调用固定图值/角色。

111015图号、250/251/58/36各次live目标保持；原八W32正常零一次/图值数组跨目标及无辅助图时firstImage=0均保留，不新异常补零/空保护。哨兵走caster终结/Move-RID-增58，两个正确尾helper不改。

## 状态目标六处

目标Take/GetPosition→255比较先pub、同解析位置actor空门禁，空Finish break仍走caster尾。下一Take所得固定resultFlags→**先live caster，再pub global flags**→live target→caster actor→AppendEffectList(固定resultFlags,0)，不先pub后取caster，也不重新取global flags供初次效果。

之后两个基础字段各live target→actor→live skill或level→payload/pub；随后另live mode用于本次状态分支，不沿wrapper初始mode永久缓存。0..5 Append119后才读live flags80000，命中则另target-actor-live skill-payload，再另target-actor-live mode-payload/pub1048，Append141；该字段的mode不能替为本次分支所用mode快照。mode9为134/80000后47-45-46；各调用后live角色保持。

mode10原已正确：先live caster/target，两者固定供side条件与首次actor；caster20且target<10或caster21且target>=10为183，其余133；调用后live flags80000才142，不改条件/缓存。mode11额外Take amount→live target Reset→再live target phase原W32=0→148→live flags80000，命中则value54原F32 8.0/79/160/固定amount1030和原5写1034，末40；均调用后重新借目标，不新owner/默认保护。

mode18在初始路由进入状态目标，仍消费target/flags、效果与两个基础字段；随后live mode若仍18，不追加专属动作而继续循环。其它初始状态路径也以字段后live mode选择，不新mode18假体或不可达排除。内存flags原W32读取/80000保持，不误改成内存W8。

<a id="battle-m-skill98-mode17-status-sampling-decision"></a>
## 11组落码

| 根 | 处数 | 恢复 |
| --- | ---: | --- |
| mode17 | 5 | 目标比较pub、Take返回flags条件、两S32选择计数、aux固定flags参数、aux之后普通效果live参数。 |
| 状态目标 | 6 | 目标比较pub/同位置actor、Take flags-caster先pub及固定参数、两个基础字段、0..5两个重写字段actor先值。 |

默认、mode10/11、0..5范围与状态动态mode正确核心保持；两根之外的wrapper/其它mode/准备/两个尾helper、布局与声明不变。非拥有native T*仅局部，不新F/X/adapted/分类/完整成员或公开接口/owner。保正常S32/W32和原范围/常数，网络原格式仅明文/音频暂停；不新越界/数学溢出/输入拒绝/异常补零/钳制/回绕/确定值或异常等价模型。

## 技能98原指令覆盖合并

只合并已提交的[准备段](cg2d_battle_m_skill98_prepare_sampling_evidence.json)、[6及首比较](cg2d_battle_m_skill98_mode6_sampling_evidence.json)、[7/8](cg2d_battle_m_skill98_mode7_8_sampling_evidence.json)、[12/13/14](cg2d_battle_m_skill98_mode12_13_14_sampling_evidence.json)、[15/16](cg2d_battle_m_skill98_mode15_16_sampling_evidence.json)和本步证据，不重新全面核对其它主体。按EA去重并要求重叠原字节/尺寸一致，42CF66..42EB32连续1833条7116B/170call，十九模式0..18和默认比较均有原端入口/消费来源。既有记录中的“其它98模式待”是各步当时边界，保持历史，不倒改成已验。

此合并只闭合技能98原区段指令证据/分派及有界源码关系；不是完整427470父函数17298条、SEH/SDK/ABI、全部消费者-owner重进退出/借用失效、编译指令或画面运行验收，不调整完整分类/成员或运行门禁。

<a id="battle-m-skill98-mode17-status-sampling-acceptance"></a>
## 验收标准

| 项 | 要求 |
| --- | --- |
| 原范围 | 本步五范围306条1163B八页31call连续字节/直接目标闭合，最后比较两条9B及默认EB0A消费明确；既有26条97B与当前BMX/BMZ相同，不称新增。 |
| 实现 | HEAD整CPP两根11组11定义11处精确差异，flags发布/固定与live、参数/字段及S32选择有限静态，正确wrapper/默认和其它根头保持。 |
| 消费 | 17的aux先Effect、无空门禁/业务firstImage零，状态0..5/9/10/11/18原动态mode与正常返回/Finish尾保持；默认只Move-RID-增58。 |
| 合并 | 五既有证据与HEAD一致、本步字节与捕获一致，EA去重冲突为0、原98全区段1833条7116B/170call无缺口；不据此验完整父或运行。 |
| 台账 | 旧812有界追加，Mode17/StatusTargets实修与正确ProcessBattleMSkill98 wrapper分列，不新身份/adapted/分类/完整成员；双域四局部I/S、七open/完整门禁及其它指标保持。 |
| 限制 | 仅必要静态/获准measure/Check，不构建/测试/运行/摘要。 |

<a id="battle-m-skill98-mode17-status-sampling-source-state"></a>
## 当前状态与后续

最后mode17/状态目标两根11处实修，默认正确核心及十九模式分派纳管；技能98原区段1833条7116B无缺口覆盖由具名子SPEC合并闭合，不等于完整功能验收。下一步按低段真实依赖进入99..104等剩余主体；其它技能-全路由/完整父-SEH/全部消费者-owner/SDK/ABI/PC53/时钟RNG/画面及R1/R7/R8/整体继续。

语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%、R7区间67/3061/队列175、60任务已闭10保持，E417→421仅双域四局部I/S。关联率不是完成率，本组14文件，快照`cg2d-re-battle-m-skill98-mode17-status-sampling-20261006`。
