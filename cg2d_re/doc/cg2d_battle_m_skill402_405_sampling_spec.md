# BNN01 技能402、405位置、图号与效果取样

<a id="battle-m-skill402-405-sampling-baseline"></a>
## 原端与范围

只修复 `src/YobiRuntime.cpp` 中 `ProcessBattleMSkill402/405` 两根，完整父427470、其它技能/全路由和运行另验。原端两连续正常范围如下：

| 半开范围 | 指令 | 实际字节 | 每页最多80条 | call |
|---|---:|---:|---:|---:|
| 430463..43062E：402 | 115 | 459 | 2 | 13 |
| 43062E..43097A：405 | 227 | 844 | 3 | 31 |
| 合计 | 342 | 1303 | 5 | 44 |

[证据](cg2d_battle_m_skill402_405_sampling_evidence.json)保存两范围全部实际指令/字节、调用/直接出口、改前两根、20补丁及旧812完整前态。全部直接目标在本组或父默认437A29；无复用尾，不以342条代父17298全部核完。

[BNM01原180B高段分派](cg2d_battle_m_skill263_400_401_sampling_evidence.json)仅复用定位402→430463、405→43062E，实际9W32跳槽36B/144W8索引144B、lowcase262；不重复计新增表取证，不宣其它case全部已开发。[BMP01十二S32/48B偏移](cg2d_battle_m_skill61_sampling_evidence.json)保持原值3/3/3/2/2/2/1/1/1/0/0/0，只作图号上下文。

<a id="battle-m-skill402-405-sampling-decision"></a>
## 工程裁决与20处差异

依既有工程委托，先SPEC后实施。两根20定义20处，402六处、405十四处；只显式表达已证取样，不新增接口/owner、类型、依赖、保护或异常模型。

| 根/补丁定义 | 改前差异 | 原端顺序与修复 |
|---|---|---|
| 402/01 | 第三Take后caster读取/side保存隐含 | side Take返回→live caster位置→固定side→actor；不能沿第二Take的GetPos旧位置 |
| 402/02..03、405/04..05 | skill先payload、level先actor | 固定caster payload先live skill；另live caster位置→actor→live level→payload |
| 402/06 | 低图号加法早于caster取样 | level低路径caster位置→S32表→加110052→actor/payload；高111001 |
| 402/07 | 目标遍历EffectList实参未显式取样 | live caster位置→固定position→caster actor，记录position/0/0 |
| 402/08 | 递增后的边界比较在目标pub后 | 初目标pub后比较；每次共同递增→比较position<固定end→目标pub→循环分支 |
| 405/09 | 初caster发布后再读全局 | GetPos返回同位置pub/actor |
| 405/10 | 初Effect caster/记录位置各自取样 | Take返回pub flags、固定image→live caster位置→actor，记录沿同caster样本 |
| 405/11 | 第一低图号加法在caster取样前 | S32表→live caster位置→加110052→actor/payload，高111001 |
| 405/12 | 第二低图号加法在caster取样前 | live caster位置→S32表→加110044→actor/payload，高111011；与第一幅不同 |
| 405/13..14 | target255比较在pub后/判空再读全局 | GetPos返回target先比较255再pub，同parsed target取actor判空 |
| 405/15 | 第一个目标Effect隐含顺序 | 第四值Take返回→caster位置→target位置→固定第四值/firstImage→caster actor，记录target/0/amount/firstImage |
| 405/16 | 第二个目标Effect隐含顺序 | secondImage→firstImage→target位置→caster位置→actor，记录target/0/firstImage/secondImage |
| 405/17 | 两个非地址索引被拓成size_t | imageIndex/index恢复原正常S32，起点0/最多2项、首正项/默认0保持 |
| 405/18 | 第三个目标Effect隐含顺序 | 所选images值→caster位置→target位置→actor，记录target/0/所选值/0 |
| 405/19 | 第四个目标Effect隐含顺序 | caster位置→target位置→固定secondValue/firstValue→actor，记录target/0/firstValue/secondValue |
| 405/20 | 第五个目标Effect隐含顺序 | fourthValue→thirdValue→target位置→caster位置→actor，记录target/0/thirdValue/fourthValue |

补丁编号按实际证据定义顺序，跨根公共字段先列，不重新编号造成证据歧义。402先Take flags pub、第二Take/GetPos caster pub、第三Take side，在430485才读取live caster位置，43048B保side返回值、430490 actor；此处不能套405的同GetPos返回actor，第三次Take可能影响共享位置，原正常合同必须保留。

402原4305F7仅在EffectList和Cast217后重取全局target；空槽无调用/重读，沿保留position直接共同递增。4305FF递增、430600先比较、430602 pub、430607按该次比较分支；用局部bool保存正常比较结果，不新增持久状态/owner或限制遍历。初范围仍side0→0..9，其它side→10..19，固定end=start+10，调用后global变更沿原重读，不能强制限定十次消费或钳制全局。

405第一图低段43073D读表、430744 caster、430749加；第二430785 caster、43078B读表、430792加。分别落码，不套402/400两图都位置先表。五目标Effect各自重新取共享位置，固定值只供本次记录，不冻结全循环caster/target。

正确行为保持：

- 402：前三Take/flags和caster pub，初空Finish直接返回；skill/level/fpUsed、25/55/40/139/64、原低/高图号。十槽正常遍历只对非空角色EffectList(caster,position,0,0)、target Cast217，调用后重读global target；空槽直接递增，无新增Finish/Take/flags消费。尾另live caster Move→58固定2，无RID。
- 405：caster Take/GetPos，初空Finish直接返回；skill/level/fpUsed、25/55/40/139/64。初值Take pub flags并作为初Effect image，随后两次独立level和图号。每target Take/GetPos，255退出不额外Echo，actor空Finish退出；amount、两个单图转换、两项数组转换及四值Take原序，五Effect flags均固定0，不使用初global flags替换0。
- 405数组仍两项未初始化局部、逐项覆盖；首正值优先、两项均非正保索引0，不补默认数据/负值拒绝。不造Aux helper或增加取包/转换调用。循环退出caster Effect(-1,0,0,0)→另live caster Move→58固定1，无RID；初空不消费此尾。

其它根/头文件/真实分派保持。native非拥有地址和非地址S32/W32边界保持；RPLAN.24不新拒绝/补零/钳制/回绕、溢出越界或故障模型，不宣异常或编译后指令形态等价。网络原格式明文，音频继续暂停。

<a id="battle-m-skill402-405-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步验收 |
|---|---|
| I | 两根20定义20处；整CPP从HEAD精确重放，其它全部正文/属性、关联声明/分派保持 |
| S | 两范围342条1303B五页44call连续实际字节/全部直接出口与20组原EA；既有180B/48B表具名上下文 |
| 消费 | 402第三Take后live caster、图号/遍历效果及比较先pub；405两幅不同图顺序、五效果独立取样和两S32索引 |
| 尾 | 所有原取包/转换与动作次数、初空直接返、各循环退出/原真实尾-402固定58=2和405固定58=1无RID |
| 登记 | 旧812仅五引用/两个symbol/本步对象，双域四局部I/S，七open与唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH/全部consumer-接线-owner失效/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续 |

E471→475仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed保持。不新F/X/adapted、分类或完整成员，不以两入口/正常遍历和342条指令或静态汇总代完整父/功能/整体通过。

<a id="battle-m-skill402-405-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill402/405`，旧812有界追加，快照 `cg2d-re-battle-m-skill402-405-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)与[看板](../plan/cg2d_measurement_dashboard.md)同步本组事实和剩余，不例行刷新总SPEC/索引或压历史。发现的20处差异当步修复，继续其它技能真实消费者/全路由、完整父与全部生命周期/适用运行门禁。
