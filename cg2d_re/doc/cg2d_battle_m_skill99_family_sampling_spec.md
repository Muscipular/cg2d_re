# 战斗M技能99族状态取样SPEC（BNB01）

<a id="battle-m-skill99-family-sampling-baseline"></a>
## 原端基线与范围

本步只修复既有 `ProcessBattleMSkill99Family` 的正常取样顺序。原主体 `432867..432ECE` 属 `UpdateBattleEvent_427470`：400条、1639B、五页、40个call；包含99..104、2019、3001三种状态生产路径和实际失败/共用终结尾。末 `432E99..432ECE` 的12条53B/2call与BMU01既有93尾重叠，不称新增；其余388条1586B/38call本步取证。完整父17298条和全部技能仍分别验收。

入口依据：
- [BMO01低段实际表](cg2d_battle_m_skill4_sampling_evidence.json)：21个W32跳槽84B与118个W8索引118B，99..104实际均到432867。
- 2019经原 `42FBF4..42FC12` 比较，`431C9C..431CBA` 的原归一化/索引/跳槽进入432867；高表437ED4六W32/24B及437EEC二十一W8/21B，lowcase2002，2019所取槽4。其它高表目标仅上下文，不据此宣各主体已验。
- 3001经原43222F的3000/3002/2079比较到43282A，再2101比较到43285B，3001等值正常落432867。上下文与正文分列。
- 原432B7D是直接六W32跳表437F3C/24B、lowcase99，六目标432B84/432B8B/432B92/432B99/432BA0/432BA7分别设置S32状态1..6；默认432BAC不覆盖跨目标的EDI。联合域values=0x63不是索引表地址。
- 2019图号原54D684五S32/20B为111126..111130；原以54D680按有效档1..5取值，现代数组按band-1对应，保正常边界，不加钳制。
- 原438230/438250各10条26B只比较状态组第0/1字段是否为1/type或3/7，以AL返回、无写入/调用；现代短路条件正确。该52B上下文不加入主体分母。

完整指令/字节、表、有限上下文、改前源码和补丁见[证据](cg2d_battle_m_skill99_family_sampling_evidence.json)。内部文本UTF-8；网络保持原格式明文；音频暂停。

<a id="battle-m-skill99-family-sampling-decision"></a>
## 工程裁决与具体差异

按用户既有“未决议由本线程决定、先SPEC后开发”授权，将下列14组/14定义/15处落实到单根；不新增公共类型、owner或依赖。

| 项 | 改前差异 | 原端要求与修复 |
|---|---|---|
| 1 | caster发布后从全局重读位置 | GetPos返回位置先发布，再沿该位置取caster |
| 2 | skill右值先于caster载荷 | 固定caster的payload先取，再live skill |
| 3 | level右值先于角色读取 | live caster位置、actor、live level、payload、字段 |
| 4 | 2019图号表先于caster位置 | band调用后先live位置，再图号表、actor/payload |
| 5 | 普通图号先算完再读caster | level比较后，低路径位置先于S32偏移表；高路径原111018常量 |
| 6 | target发布后再做255比较并重读判空位置 | 返回位置先比较再发布，actor判空沿同位置 |
| 7 | Take flags发布后无条件全局重读 | 固定返回W32 flags供初次80000判定；附加效果调用后才live重读 |
| 8 | AddAppendEffect三个全局实参顺序隐含 | target、level、skill依原序明确取样后调用 |
| 9 | EffectList角色和target顺序隐含 | live target、live caster、actor；flags沿已固定值 |
| 10 | 基础target skill右值先于actor | target位置、actor、live skill、payload |
| 11 | 基础target level右值先于actor | target位置、actor、live level、payload |
| 12 | 99族和3001成功后的skill重写次序 | 两处分别位置、actor、live skill、payload |
| 13 | 99族状态payload先于用于switch的skill | 位置、live skill、actor、payload，再按固定skill选择1..6 |
| 14 | 2019先取payload再读level和减一 | 位置、live level、actor、S32减一、payload、S32余数 |

保留所有正确消费：两个初始Take首值弃用/次值caster，初caster空Finish后直接返回；castSkill/level/fpUsed，25/55、2019额外253/254、39/40、图1028/139/64/40。每target先Take/GetPos，255退出，空actor Finish后退出，Take flags/可选AddAppendEffect/EffectList、基础两字段、调用后按live skill选择分支；未命中只继续消费下个target。

99..104先157，调用后80000失败继续；成功重写skill，固定状态payload跨144保留，再写13DC/type与13D8=1，143后再live skill选择1018=253或274，80/37。3001先158，成功重写skill，固定payload跨144，写0/1、143、1018=274、80/37。2019先157，成功采样固定状态payload和余数，与3/7/余数短路比较；144后保持固定payload写7/3，再**重新读取live level计算余数**写13E0，143、1018=253、80/37。原EDI跨目标默认不覆盖、正常99..104先赋1..6；现代未初始化statusType保持，不造业务默认。

循环255/空actor最终均caster效果终结(-1/0/0)、Move、packetSubState58加一、回父默认；初caster空的直接返回不消费该尾。原末无RID；不改任何其它根、准备/分派、字段类型和正确状态载荷借用。

按RPLAN.24不新增溢出/越界、补零、钳制、回绕、故障或异常等价模型。除已证地址使用native指针，S32/W32/F32等非地址宽度保持；不据C++取样表达证明编译后的指令形态。

<a id="battle-m-skill99-family-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步可接受证据 |
|---|---|
| I | 单根14定义15处落码；整CPP从HEAD重放补丁，全部其它正文/属性及声明保持 |
| S | 400条1639B五页/40call连续实际字节；全部直接出口落本主体或父默认437A29；实际六槽24B与入口表/图号/状态helper上下文逐字段；旧尾12条53B不算新增 |
| 登记 | 仅旧812追加五引用/一symbol/本步对象；双域四局部I/S，七任务进展和唯一进度同步 |
| 有界保持 | 原取包/条件重读/三个状态路径、固定payload跨144、不同初始失败与循环失败、无RID终结、非地址宽度和正确正常零/未初始化边界 |
| 运行及整体 | 本步不构建/测试/运行/计算摘要；父SEH、完整消费者/接线-owner重进退出失效、SDK/ABI、PC53、时钟RNG/画面及全功能/R1/R7/R8继续，不因本文或静态汇总变pass |

本步完成后E421→425，仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60任务已闭10/完整验收0保持。新增图号与状态helper/高入口取证是有界上下文，不新增F/X/adapted或完整成员。

<a id="battle-m-skill99-family-sampling-source-state"></a>
## 源码、进度与剩余

源码：`src/YobiRuntime.cpp#ProcessBattleMSkill99Family`；原新关系追加到既有812，不把helper注册为独立原函数。快照 `cg2d-re-battle-m-skill99-family-sampling-20261006`。唯一进度[收尾推进记录](../plan/cg2d_full_reconstruction_progress.md)，[看板](../plan/cg2d_measurement_dashboard.md)只汇总证据与缺口。

后续沿107等尚未核对主体/完整分派推进；本步发现的14组正常取样差异当步修复。完整父、所有消费和运行门禁仍是独立工作范围，不把这些尚未执行范围降为“已完成”。
