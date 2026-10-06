# BNS01 技能267累计反馈取样

<a id="battle-m-skill267-sampling-baseline"></a>
## 原端与范围

只修复 `src/YobiRuntime.cpp#ProcessBattleMSkill267` 单根，正常主体434EDF..43573F（半开）576条2144B、八页（每页至多80条）、45call，无内部switch或复用尾。全部直接出口在本段或父默认437A29，真实Move/RID尾在段内。[证据](cg2d_battle_m_skill267_sampling_evidence.json)保存完整指令/字节/调用/出口、25补丁/改前根和旧812完整前态，不代父427470全部17298/SEH或运行。

267沿[BNM01](cg2d_battle_m_skill263_400_401_sampling_evidence.json)原42FC32九W32槽/144W8索引180B表；3029沿[BNR01](cg2d_battle_m_skill19_family_sampling_evidence.json)434127四W32/二十W8索引36B表，均实际到434EDF。原十二S32偏移553E58/48B沿[BMP01](cg2d_battle_m_skill61_sampling_evidence.json)具名保持，只核本组入口不代全路由。

<a id="battle-m-skill267-sampling-decision"></a>
## 工程裁决与25定义27处

依既有工程委托先SPEC后实施。25定义27替换处，两个遍历索引和两处W8门禁的定义各替换两处。只恢复具体取样顺序/原字宽，不新接口、owner、外部依赖、默认/保护或异常模型。

| 补丁 | 原端与修复 |
|---|---|
| 01 | 同parsed caster发布和取actor |
| 02 | 固定caster payload先live skill |
| 03 | 另live caster actor先level |
| 04 | effectCount level-position-table-actor-S32 subtraction-payload |
| 05 | 初低图位置先表/加法后actor，高图独立live |
| 06 | parsed target比较先pub/同位置actor |
| 07 | saved flags-previousTotal-target-secondImage先合并/加法 |
| 08 | 累计附图选择索引恢复S32 |
| 09 | 两个纯业务遍历索引恢复S32 |
| 10 | 普通附图选择索引恢复S32 |
| 11 | 普通Effect target先caster位置/actor，固定flags金额图 |
| 12 | 普通Aux selected-secondary-primary-flags-target-caster-actor |
| 13 | target先current/256-52同位置，51另live |
| 14 | 反射基础skill actor先值 |
| 15 | 反射基础level actor先值 |
| 16 | 累积copy source actor后target位置，再sourcepayload-destactor |
| 17 | 循环累计Effect先取flags-primary-total，清flags发布前先取reflected |
| 18 | 循环累计Aux primary-flags-caster-source-actor |
| 19 | 循环state先读取previous state再caster/actor |
| 20 | 首244和最终46门禁实际W8 |
| 21 | 反馈actor先总量 |
| 22 | 反射34后caster先current/256-52用同位置，51另live |
| 23 | 末普通响应flags先caster/选状态再actor |
| 24 | 末Effect image-total-caster-source-actor，沿固定flags |
| 25 | 末Aux image-liveflags-caster-source-actor |

关键分工：effectCount先level→caster位置→表→actor→正常S32 4-offset→payload/102C；初低图先位置→表→加110052→actor/payload，高固定111001。累计先saved flags→previousTotal→target→secondImage，后OR/add再发布；原firstImage保存、FC1FFFDF清位和列表/count保持。循环累计Effect先saved flags→primary→total→清12000→reflected位置→flags发布→caster→actor；该Effect用固定清后flags，后Aux另live flags。末Effect沿此前flags样本，image→total→caster→reflected→actor；末Aux另live flags，各固定第二图/附图保持，不统一成同一次flags。

原生产、消费与退出保持：

- 首Take弃/第二Take/GetPos；初caster空Finish直接return，skill/level/fpUsed及25/55/40/240、effectCount、初图、64，各次live角色；5ABA7C和5ABA84入口两个W32业务零、图/三数组八W32业务零沿既有父基线。未指定lastAmount不补值，累计第二/附图沿flags9C bit_cast/valueA0。
- 每target255退出/空Finish后退出；Take flags再金额，800000才AuxGraphics(mode0)并在调用后重读flags；12000才合并savedflags/累计、保存图/target、首正图或原0项、清FC1FFFDF与列表-count，清后另live flags供普通Effect。两选择/两遍历索引S32，原0/上界2保持。
- 普通Effect/Aux后0，8/100门禁；40且无12000才34后target先current，256/52沿同target，51另live。金额非零且flags3按8000→123、40000→126、4000→122、20000→125、2000→121、10000→124优先；20000000挡普通165/27，所有调用后flags重读保持。金额或8/100且无2007E000才11/可76；20才46，46后重读flags再12000门禁。
- 12000反馈时反射skill/level各actor先值，source102C→accumulated102C的actor/payload各次取样；清saved12000、累计Effect/Aux、242→EffectList(-1/0/0)，5ABA7C先判再caster/actor动作26或1后该值写1。循环继续，不新增149/半额/附加状态。
- 共同casterEffectList(-1/0/0)；总量零直接Move/RID固定58=1。非零：首saved20门禁435504按W8/可244，actor先总量写1030、另live1034=6；W32 saved10000000决定159/调用后重读flags；其8/40/3仍同W32寄存器分工。34后caster先current/256或52同位置、51另live；普通响应先live resultflags→caster位置→按bit2状态165/27→actor。末Effect固定flags、Aux另live，74后另live savedflags，10000000/金额/8/100门禁决定caster11/可能**另live target**76；最终4356F2 saved20按W8才caster46。
- 两W8仅435504/4356F2；其它saved/result W32与AL/AH测试、入口业务零、全部调用/图号/门禁-字段/状态和真实Move→RID→58固定1保持，不增58。初空不消费共同尾，循环255/空共同消费真实尾；不加旧栈模拟或未写字段默认。

其它根/属性、图表/关联声明和实际header保持。已证native非拥有地址不收窄，非地址S32不扩宽；RPLAN.24只保持正常S32算术/原业务零，不加拒绝/补零/钳制/越界溢出/回绕或故障模型，不宣异常/编译指令形态等价。网络原格式明文，音频暂停。

<a id="battle-m-skill267-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证明 |
|---|---|
| I | 单根25定义27处，四S32业务索引和两W8门禁；整CPP从HEAD精确重放，其它根头/header和关联声明保持 |
| S | 576条2144B八页45call连续字节/全部直接出口、27处原EA；267/3029原180B/36B入口表和48B偏移仅上下文 |
| 消费与保持 | effectCount/初图、累计-普通和循环/末Effect-Aux各取样、字段-copy/总量反馈和state26/1；原全部门禁/字段/动作与Move-RID固定1 |
| 登记 | 旧812五引用/一既有源码symbol登记/本步对象，双域四局部I/S、七open和唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH-全部consumer/接线/owner、SDK/ABI/PC53/时钟RNG画面及R1/R7/R8继续 |

E491→495仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed及全部完整门禁保持，不新F/X/adapted、分类或完整成员；不以本根/静态汇总代整体验收。

<a id="battle-m-skill267-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill267`，旧812有界追加，快照 `cg2d-re-battle-m-skill267-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)和[看板](../plan/cg2d_measurement_dashboard.md)同步实际状态。25定义27处当步解决，其它技能/全路由、完整父/全部生命周期和适用运行门禁继续。
