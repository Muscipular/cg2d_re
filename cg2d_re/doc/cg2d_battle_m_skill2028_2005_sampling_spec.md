# BNP01 技能2028累计反馈与2005/4003攻击取样

<a id="battle-m-skill2028-2005-sampling-baseline"></a>
## 原端与范围

只修复 `src/YobiRuntime.cpp#ProcessBattleMSkill2028/ProcessBattleMSkill2005Family` 两根，完整父427470/其它技能和运行另验。[证据](cg2d_battle_m_skill2028_2005_sampling_evidence.json)保存下列四范围全部实际指令/字节、调用/直接出口、改前两根/24补丁及旧812完整前态：

| 半开范围 | 指令 | 实际字节 | 每页最多80条 | call |
|---|---:|---:|---:|---:|
| 43227D..43282A：2028主 | 377 | 1453 | 5 | 37 |
| 42BD50..42BEAC：2028真实累计尾 | 79 | 348 | 1 | 9 |
| 4357BA..435B25：2005/4003主 | 250 | 875 | 4 | 24 |
| 42CB44..42CB5E：2005族真实Move/RID尾 | 7 | 26 | 1 | 2 |
| 合计 | 713 | 2702 | 11 | 72 |

全部直接目标在本组或父默认437A29。累计值为零时2028跳42BE84直接Move/RID/固定58=2；非零跳42BD50两字段-反馈后同尾，不能额外消费前序其它技能动作。2005族435B12先取caster/actor压参，435B20跳42CB44直接Move/RID/58增一，无End(-1)/额外74。

2028尾与[BMP01第一范围](cg2d_battle_m_skill61_sampling_evidence.json)偏移487起79条逐字段复用；2005尾与[BMT01第三范围](cg2d_battle_m_skill12_82_97_sampling_evidence.json)偏移279起7条相同。既有86条374B/11call非新增，本步新两个主627条2328B/61call分列，不以713代父17298全覆盖。

2028五S32图表54D6FC..54D710共20B，实际值111147..111151；低level有符号除3选前四项，高>=12取第五项。原432276实际4W32跳槽16B/40W8索引40B、lowcase2028定位2028→43227D；`switch_info.startea` 43225F是表达式起点，实际间接跳转EA为432276，分开记录。56B表和78B前缀只本组定位。

2005沿[BNB01既有高段45B表](cg2d_battle_m_skill99_family_sampling_evidence.json)定位4357BA；当前选择明确2005，不沿用2019选择元数据。4003沿43222F的>3016分支到4340FE、>3100到43579B，依次减3101/901/1，避开3101与4002跳转、最后零值沿指令顺序进入4357BA；55B具体路由节点只定位4003，不以代码引用表缺少独立jump认定入口缺失。BMO八W32入口一次业务零具名保留，2005两图/三组数组跨target保持，不新补零。

<a id="battle-m-skill2028-2005-sampling-decision"></a>
## 工程裁决与24处修复

依既有工程委托先SPEC后实施，两根24定义24替换处（2028十五、2005族九），仅正常取样/宽度，不新增接口/owner、类型、依赖、保护或异常模型。

| 补丁 | 根/改前差异 | 原端与修复 |
|---|---|---|
| 01..06 | 两根caster发布后重读、skill先payload/level先actor | GetPos返回同parsed位置pub/actor；固定caster payload先live skill；另live caster位置→actor→live level→payload |
| 07 | 2028图表及除3先于caster/actor | 低level：caster位置→正常S32除3最终索引→actor→payload→图表；高：caster位置→actor→第五图表→payload，各写1028 |
| 08..11 | 两根target255比较在pub后/判空再读global | GetPos返回target先比较255再pub，actor沿同parsed target |
| 12 | 2028先target/总和，后保存flags | previousTotal先读取→target→保存固定flags→正常S32累加；后清20/位置-total-flags pub及两次count读写保持 |
| 13 | 2028首EffectList隐含caster/target顺序 | caster位置→target位置→actor，传此前固定flags/amount |
| 14..15 | 2028 target基础字段先取值 | 各live target位置→actor→live skill/level→payload |
| 16 | 2028 response20000 bit20读取W32 | 43266C实际W8内存，显式字节读取；相邻10000000门禁仍W32 |
| 17..18 | 累计尾反射角色字段先取值 | 各live reflected位置→actor→live skill/level→payload |
| 19 | 尾caster金额先读累计值 | caster位置→actor→live累计值→payload 1030 |
| 20 | 尾EffectList全局实参顺序隐含 | live累计amount→保存flags→caster target位置→reflected source位置→actor |
| 21 | 2005族普通Effect参数隐含 | firstImage→live flags→live target→live caster位置→actor，amount固定 |
| 22 | 2005族两非地址索引拓为size_t | imageIndex/index恢复S32，首正项/默认0和两项界限保持 |
| 23 | 2005族Aux参数隐含 | 所选图→secondImage→firstImage→live flags→target→caster位置→actor |
| 24 | 34后current先target/256-52重新读global | target先current比较，256/52同位置，52之后51另live |

2028低图原432383乘法用于有符号除3，432385已读caster，43238F完成正常除3结果，432391 actor/432398 payload，43239B才表值；保C++正常S32除3，不模拟乘高位/回绕或编译指令。高路径4323AA位置→4323B0 actor→4323B7表→4323BD payload，不能套同一个payload先表模板。

累计门禁按live flags10000；43241B先总量、432421 target、432427保存flags、43242C加amount。清20仅完整flags缓存的低位变更，W32保存/写回和target列表索引、entryCount两次读取-递增保持；不新增容量/溢出保护，不把固定flags替换成Effect前live重读。

正确消费/字段/动作保持：

- 2028：首Take弃并将scene value00/entryCount04及message总值业务置0，第二Take/GetPos，初空Finish直接返；基础字段/fpUsed和25/55/40/139/64、低高图号。每target255退出，空Finish退出，flags/amount；10000才累计、保存flags/反射位置、清20和记录entryPositions/count。首EffectList后两基础字段，各角色位置比较；非同侧149后必须重读caster/target，随后flags重新取样。
- 同侧条件仍(source20且target<10)或(source21且target>=10)；响应优先40000→20000→10000，否则0，在184/74前固定；同侧反向EffectList/184/74后live target，40000→126，10000→124，20000→125/159/32、1030 amount/1034=6、live10000000可改0、W8 bit20才46/47；无响应时非同侧160、1030 amount/1034=5、live10000000可改0。原复用固定source/target与调用后重读保持，不增加取包/动作。
- 2028退出caster EffectList(-1)，总量非零才反射skill/level、保存flags清10000，反射1/140、caster0/149/160、总量1030与类型5，再累计反馈EffectList和反射74；两路径共同live caster Move→RID→58固定2。总量可能正常累计抵消为0，沿原零路径，不新强制反弹。
- 2005/4003：首Take固定attackMode，第二GetPos caster，初空直接Finish返；基础字段/fpUsed、FF4 mode、25/55/151/249；入口两图/三组数组一次业务零。target255或空Finish退，flags/amount、800000才Aux取包，Effect/Aux→target0，原cached W32的AL/AH分流、34/256或52-51、123/122/121或165/27、11/76/46及各live flags重读保持。共同尾Move→RID→58增一，没有额外End(-1)/74；两索引恢复不重置数组/默认值。

其它根/属性/关联声明/真实分派保持。native非拥有地址/非地址S32-W32及实际W8边界保留；RPLAN.24不新补零/拒绝/钳制/回绕、越界溢出或故障模型，不宣异常或编译后指令形态等价。网络原格式明文，音频继续暂停。

<a id="battle-m-skill2028-2005-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证明 |
|---|---|
| I | 两根24定义24处；整CPP从HEAD精确重放，其它全部正文/属性、关联声明/分派保持 |
| S | 四范围713条2702B十一页72call连续实际字节/全部出口与24组EA、五S32/20B图表及2028/2005/4003实际路由 |
| 复用 | BMP79条348B/9call与BMT7条26B/2call逐字段复用非新增；新627条2328B/61call分列 |
| 消费 | 2028各图路径、累计/固定flags和响应优先/同侧双位置重读、W8门禁/真实两尾；2005族Effect/Aux-两S32索引/位置比较与原反馈尾 |
| 登记 | 旧812仅五引用/两个symbol/本步对象，双域四局部I/S，七open与唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH/所有consumer-接线-owner失效/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续 |

E479→483仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed保持。不新F/X/adapted、分类或完整成员，不以本组/累计反馈-713条或静态汇总代完整父/功能/整体通过。

<a id="battle-m-skill2028-2005-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill2028/ProcessBattleMSkill2005Family`，旧812有界追加，快照 `cg2d-re-battle-m-skill2028-2005-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)与[看板](../plan/cg2d_measurement_dashboard.md)同步实际状态；不例行刷新总SPEC/索引或压历史。发现的24处差异当步修复，继续其它技能真实消费者/全路由、完整父/全部生命周期与适用运行门禁。
