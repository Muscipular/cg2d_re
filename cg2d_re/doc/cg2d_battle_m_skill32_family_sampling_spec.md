# BNQ01 技能32族状态生产取样

<a id="battle-m-skill32-family-sampling-baseline"></a>
## 原端与范围

只修复 `src/YobiRuntime.cpp#ProcessBattleMSkill32Family` 单根，原端正常主体432ECE..4340FE（半开）为1065条4656B、每页最多80条共14页、76call，真实End/Move/RID尾在段内，无复用尾。全部直接出口在本段或父默认437A29；十二实际switch的间接目标也在本段。[证据](cg2d_battle_m_skill32_family_sampling_evidence.json)保存全部指令/字节、调用/出口、十二完整表、状态图表、改前根/13补丁和旧812前态。不以本根代父427470全部17298/SEH或运行。

十二内部表共117 W32跳槽468B，四压缩表W8索引23/36/16/18共93B，合561B；原 `get_jtable_size()` 与 `get_lowcase()` 用于真实槽数/低case，不拿ncases当跳槽数。直接18槽debuff表4332B3将32..49映为1..6三组重复，越过该内部case范围的原默认4332E4明确xor EBX=0，现有type=0保留。99..114压缩状态表43354F：99..104→1..6、114→7，其它不覆原状态局部；现有未初始化statusType/跨target保持，不补默认。

图数据为八分档行5S32/20B共160B（首有效地址54D5E4/54D620/54D634/54D648/54D65C/54D670/54D6AC/54D6E8）、54D738的15×5 S32/300B和54D864的18×3 S32/216B，合169S32/676B。当前三个constexpr表与实际字节/值一致，重复列保留，IDA的flt标签不改变原W32图号读取含义。十二S32偏移553E58/48B沿[BMP01](cg2d_battle_m_skill61_sampling_evidence.json)具名保持。

入口按具名既有证据定位：低32..49/54..60/64..68/86沿[BMO01实际202B表](cg2d_battle_m_skill4_sampling_evidence.json)；2006/2010..2015沿[BNB01高段45B表](cg2d_battle_m_skill99_family_sampling_evidence.json)，2027沿该基线FBF4比较/FC00相等直跳；2050..2052/2058..2067沿[BNP01 2028..2067原56B表](cg2d_battle_m_skill2028_2005_sampling_evidence.json)，3002..3016沿该原43223B/241范围分支。2089..2095由本步432832..432850的范围分派直达432ECE（含外层43282A/830），仅本组具体路由，不代所有其它技能开发/全路由验收。入口之外的分派节点不计1065分母。

<a id="battle-m-skill32-family-sampling-decision"></a>
## 工程裁决与13组14处修复

依既有工程委托先SPEC后实施。单根13定义14替换处，第11定义两个成功skill写入共两处；不新增接口/owner、类型、依赖、保护或异常模型。

| 补丁 | 改前差异 | 原端与修复 |
|---|---|---|
| 01..03 | caster pub后重读、skill先payload/level先actor | GetPos返回同parsed位置pub/actor；固定caster payload先live skill；另live caster位置→actor→live level→payload |
| 04 | 三初图号先全部算完再取caster | 86低：表→caster位置→加110052→actor；64..66/68低：caster位置→表→加110081→actor；其余低：caster位置→表→加110056→actor；高111001/111014/111018各保持 |
| 05..06 | target255比较在pub后/判空另读global | GetPos返回target先255比较再pub，actor沿同parsed target判空 |
| 07 | AddAppendEffect全局参数取样隐含 | live target→level(5A4A2C)→skill，固定参数调用，随后live flags重读 |
| 08 | EffectList隐含caster/target取样 | caster位置→target位置→actor，传此前固定flags/0 |
| 09..10 | target基础字段先取值 | 各target位置→actor→live skill/level→payload |
| 11 | 32..49/99..104成功skill字段先值 | 两处均target位置→actor→live skill→payload，之后各自再次读skill作类型分派 |
| 12 | 分档行索引拓成size_t | 纯非地址行恢复S32，原row0及各技能分配保持 |
| 13 | 所有分档图表都在target/actor之前读取 | GetBand后先live target；按下表分别表先actor或payload先表，写各样本102C |

分档顺序：

| 技能 | 原正常生产顺序 |
|---|---|
| 2006、2011、2013、2027 | level+1→GetBand→target位置→表值→actor→payload/102C |
| 2010、2012、2015、2014 | level+1→GetBand→target位置→actor→payload→表值/102C |

GetBand和level+1仍正常S32，row对应2006=0/2010=1/2011=2/2012=3/2013=4/2015=5/2014=6/2027=7；不造范围限制/回绕。初图/分档两组顺序各自保持，不用统一“表先角色”或“角色先表”模板。

正确生产、消费、比较与退出保持：

- 首Take弃、第二Take/GetPos，初caster空Finish直接返回；基础skill/level/fpUsed，25/55/40/三类初图/139/64。每target255退出、actor空Finish退出；Take flags，80000才AddAppendEffect，并在调用后重读flags，EffectList/两基础字段/value1034=-1；之后另live skill分派。
- 55..60同侧190/异侧120，64..66同侧180/异侧130，67同侧183/异侧133与成功142；同侧仍source20-target<10或source21-target>=10，各次fixed source/target样本不改。32..49：119后成功才再写skill/再次live skill选类型1..6或原默认0，固定target/payload比较2/type，142后仍同payload写debuff type/state，141另live actor。54的146/成功147，68的额外amount Take/Reset、phase业务零/148及成功8.0F-79/160-amount/type5，末40保持。
- 动态99..104入口157后成功、actor先skill再另live skill，内部114→7支持保持；原未指定statusType不补值，payload在144之前固定，跨该替换动作写13DC/type和13D8=1，143/37各live target。86 technique正常有符号除100，2053/2054选择135/136否则134，成功47/45/46保持。
- 2006/2010..2015/2027：2013先原`performIndex = %d`格式化/DebugString，失败直接继续，成功才GetBand；2011/2013只有图号/252后继续，不做额外状态。其余成功才状态3；2006 type0、2010 type4、2012 type5、2014 type9按原比较/可144→固定payload字段→143/37；2015 type8/2027 type10不做替换动作/143，只字段后37。payload仍在替换动作前固定，字段类型/顺序保持。
- 2050..2052/2058..2067/2089..2095/3002..3016按原常量图表/level列和类型映射，image先选/写102C→252后live成功门禁；固定payload的state3/type比较，可142或144，跨调用保留字段→141或143→37。2089/2090仍使用debuff动作；其余常规动作，类型30..47、11..20等原映射不改，不新增动态skill重读或类型默认值。
- 全部target取包/各分支成功失败、继续循环和原正确比较/状态写入保持；循环255/空退出共同caster EffectList(-1/0/0)→另live caster Move→RID→58增一，初空直接返不消费该尾，无额外55/74。原未指定局部不补值，不按正常门禁推异常模型。

其它根/属性/三个图数据表及关联声明/真实分派保持。已证native非拥有地址不收窄，非地址S32/W32不扩宽；RPLAN.24不新增补零/拒绝/钳制/越界溢出或回绕模型，不宣异常/编译后指令形态等价。网络原格式明文/音频暂停，CRT/DebugString承接保持既有边界，不扩大SDK运行等价结论。

<a id="battle-m-skill32-family-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证明 |
|---|---|
| I | 单根13定义14处；整CPP从HEAD精确重放，其它全部正文/属性、图表及关联声明/分派保持 |
| S | 1065条4656B十四页76call连续实际字节/全部直接及12表间接出口，561B内部表与169S32/676B图数据、14处原EA |
| 消费 | 三初图/八分档图各自取样、target-level-skill附加效果/固定flags、角色字段和两成功skill写入、一个S32行索引 |
| 保持 | 原每分支图号/状态类型-比较/固定payload跨替换动作、正确默认0和未指定状态局部、所有取包/调用/动作及初空-循环/真实End-Move-RID增58 |
| 登记 | 旧812仅五引用/本步对象，既有ProcessBattleMSkill32Family唯一symbol保持，双域四局部I/S，七open与唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH/全部consumer-接线-owner失效/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续 |

E483→487仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed及全部完整门禁保持。不新F/X/adapted、分类或完整成员，不以本状态根/1065条-图表或静态汇总代完整父/功能/整体通过。

<a id="battle-m-skill32-family-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill32Family`，旧812有界追加，快照 `cg2d-re-battle-m-skill32-family-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)与[看板](../plan/cg2d_measurement_dashboard.md)同步实际状态；不例行刷新总SPEC/索引或压历史。本步13组14处差异当步修复，继续其它技能真实消费者/全路由、完整父与全部生命周期/适用运行门禁。
