# BNR01 技能19族累计与反弹取样

<a id="battle-m-skill19-family-sampling-baseline"></a>
## 原端与范围

只修复 `src/YobiRuntime.cpp#ProcessBattleMSkill19Family` 单根。正常主体43412E..434EDF（半开）914条3505B、十二页（每页最多80条）、67call，包含真实终结-Move/RID尾；无复用尾。全部直接出口在本段或父默认437A29，三个内部switch所有槽/默认也在本段。[证据](cg2d_battle_m_skill19_family_sampling_evidence.json)保存完整指令/字节/调用/出口、完整内部表、具名入口、26补丁/改前根及旧812完整前态，不以本根代父427470全部17298/SEH或运行。

三个内部表为4342AE十三W32槽、43438C七W32槽及十七W8索引、4343E6六W32槽，共26W32/104B+17W8/17B=121B。2055、2073、3037的内部外层比较和常量写入保持；内部default43442F不覆既有effectImage。十二S32图偏移553E58/48B沿[BMP01](cg2d_battle_m_skill61_sampling_evidence.json)，不重做其它图矩阵。

低19..31沿[BMO01原202B表](cg2d_battle_m_skill4_sampling_evidence.json)到43412E；2055..2057沿[BNP01原56B表](cg2d_battle_m_skill2028_2005_sampling_evidence.json)，2068..2079沿其432247/24D上界及432253/259下界到43412E，均先xor EDI为0。3037由434127的原四W32槽/二十W8索引（36B）直接到434130，跳过该xor，沿[BMN01](cg2d_battle_m_skill_entry_waiting_sampling_evidence.json)42AE2C已证EDI=1。现有header分别传entryValue0/1及flags9C/valueA0，四个入口业务字段和halfApplied沿该值保持；caster仍按原EDI比较，不另造统一判空。只核本族具名路由，不代其它技能/全路由。

<a id="battle-m-skill19-family-sampling-decision"></a>
## 工程裁决与26定义27处

依既有工程委托先SPEC后实施。单根26定义27替换处，第08定义两个遍历索引各一处；两选择索引另按原缩进分为07/09。四个纯业务索引保持S32。只显式原取样和字宽，不新增接口、owner、外部依赖或保护/异常模型。

| 补丁 | 改前差异 | 原端与修复 |
|---|---|---|
| 01..03 | caster发布后重读、基础skill/level先值 | 同parsed caster发布/actor；固定payload先live skill；另live caster位置/actor先level，再payload |
| 04 | 初图先表后caster位置 | 低level：位置→表→加110052→actor/payload；高另live位置→actor/payload固定111001 |
| 05 | target比较在pub后、比较和判空另读global | GetPos返回先255比较后pub，沿同parsed target比caster/取actor判空 |
| 06 | 先累计发布再取反弹target | 先previousTotal→target，后正常S32加lastAmount发布；其它图号/flags保存与列表-count顺序保持 |
| 07..09 | 两选择/两遍历索引拓成size_t | 四局部恢复S32，原0、上界2、找到首正图后退出保持，不扩容量 |
| 10..11 | 普通Effect/Aux参数取样隐含 | Effect固定flags/amount/image、target→caster→actor；Aux selected→secondary→primary→live flags→target→caster→actor |
| 12..14 | 两EffectList先actor、目标比较先current | 两处各fixed source/target→live flags→actor；已固定target先current比较，256/52用同target，51另live |
| 15..16 | 半额门禁flags宽读、反馈值先actor | 434705实际W8 flags→target→caster；117/160后caster→actor→live halfAmount→payload/1030 |
| 17..19 | 反弹基础字段先值、copy提前取source payload | skill/level各reflected位置→actor→live值→payload；copy source位置→actor→reflected位置→source payload→dest actor/payload→source102C写dest |
| 20..22 | 反弹最初flags宽读、34后另读caster、末半额门禁宽读 | 434BE2按W8；34后caster先current/256-52同位置、51另live；434D09按W8，20/21同侧判定按各分支才读target |
| 23..24 | 末反弹Effect/Aux全局参数取样隐含 | Effect image→total→saved W32 flags→caster→reflected source→actor；Aux second→primary→saved W32 flags→caster→reflected source→actor，selected固定 |
| 25..26 | 末8/20门禁宽读 | 434E0A/434E6A实际W8，前者原短路读取保持；邻近100门禁仍W32/高字节语义 |

正确生产、消费、分支和退出保持：

- 首Take弃、入口entryValue写value00/count04/messageTotal/halfTotal与halfApplied，第二Take/GetPos，初caster比较失败Finish直接return。基础skill/level/fpUsed，25/55/40/139/64，各次live actor；effectImage指针在技能分派前固定，五低图组19/23/27→3×offset+110032或111002、20/24/28→110020或111005、21/25/29→110140或111008、22/26/30→offset+110044或111011、31/3037→offset+110063或111012，其它高技能各固定111164..111190及default不写保持。
- seenCaster业务false、first/second图和三数组共八W32业务零保持既有父基线；lastAmount未指定不补值，reflectedSecondImage沿flags9C bit_cast、reflectedAlternateImage沿valueA0。逐target255退出、空actor Finish后退出；Take固定W32 flags再Take金额，800000才AuxGraphics(mode0)，调用后live flags重读；10000才累计/保存target-图/flags、首正附图或原第0项、清FC1FFFDF与列表/count，清后另live flags供普通Effect。
- 每target普通Effect/Aux后动作0，flags8/100门禁；40且无10000时，31/3037按C00可33→34、source/target比较可EffectList→25，73→双位置重读可74→再双位置重读，0后半额20门禁/同侧182及halfApplied=1和正常S32 lastAmount/2累计，最终target-current选择256或52/另live51。其它skill只有34和该选择；所有各次位置重读和固定flags分工保持。
- 金额非零且flags3按40000→126、20000→125、10000→124优先，20000000阻止普通响应；其它skill按flags2选择165/27；31/3037按C00选33/32、可EffectList/25/73/74、0后原20门禁/同侧182/半额累计。金额或8/100满足且无20070000、非31/3037才11/可76；末20且无10000才46。
- halfApplied=1才caster117/160，固定每次actor写halfTotal/1030与另live1034=5；共同EffectList(-1/0/0)。总量零直接真实Move/RID固定58=2；总量非零才反弹字段、source102C→reflected102C、清saved10000、seenCaster才37、1/140/caster0。反弹W8 flags8/40门禁、34后256或52/另live51；金额非零、非31/3037按saved低flags3与live resultflags2选择165/27；31/3037按live C00选33/32→64、saved W8 20无才同侧182或117/160及正常S32 lastAmount/2写1030/5。
- 末反弹Effect/Aux后reflected74，金额或saved W8 8或live100门禁且非31/3037才caster11/可能**另live target**76；saved W8 20才caster46。真实Move→RID→58固定2无增一；初caster比较失败不消费尾，target255/空共消费真实尾。不增加额外55、默认字段/旧栈模拟或改包格式。

其它根/属性、全部图表/关联声明和真实header保持；只已证非拥有地址native，不扩宽非地址。RPLAN.24保持正常S32/原业务零，不加拒绝/补零/钳制/越界溢出/回绕或故障模型，不宣异常或编译后指令形态等价。网络仍原格式明文，音频暂停。

<a id="battle-m-skill19-family-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证据 |
|---|---|
| I | 单根26定义27处、四S32索引/五处W8读取；整CPP从HEAD精确重放，其它正文/属性与关联声明/header保持 |
| S | 914条3505B十二页67call连续字节及全部直接/三个表间接出口；121B内部表、36B 3037入口表及具名低/中入口、48B偏移只上下文；27处原EA |
| 消费与保持 | 初图/两普通效果、半额/累计与反弹字段/图copy-反馈取样；全部取包/固定与live分工、原图号/金额门禁-动作/各失败及真实Move-RID固定2 |
| 登记 | 旧812仅五引用/一既有源码symbol登记/本步对象；双域四局部I/S、七open与唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH、全部consumer/接线/owner失效、SDK/ABI/PC53/时钟RNG画面及R1/R7/R8继续 |

五处W8为434705、434BE2、434D09、434E0A、434E6A；其它W32金额/flags/数组及原业务零保持。E487→491仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed及全部完整门禁保持，不新F/X/adapted、分类或完整成员，不以本根/静态汇总代整体通过。

<a id="battle-m-skill19-family-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill19Family`，旧812有界追加，快照 `cg2d-re-battle-m-skill19-family-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)与[看板](../plan/cg2d_measurement_dashboard.md)同步实际开发状态。本步26定义27处当步解决，其它技能/全路由、完整父与全部生命周期/适用运行门禁继续。
