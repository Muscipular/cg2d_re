# BNM01 技能263、400、401角色、图号与效果取样

<a id="battle-m-skill263-400-401-sampling-baseline"></a>
## 原端与范围

本步只修复 `src/YobiRuntime.cpp` 的 `ProcessBattleMSkill263/400/401` 三根，完整父427470、其它技能/全路由和运行门禁另验。不扩至402/405。

| 原端正常范围（半开） | 指令 | 实际字节 | 每页最多80条 | call |
|---|---:|---:|---:|---:|
| 42FC39..42FDCF：263 | 107 | 406 | 2 | 16 |
| 42FDCF..430162：400 | 237 | 915 | 3 | 26 |
| 430162..430463：401 | 192 | 769 | 3 | 24 |
| 合计 | 536 | 2090 | 8 | 66 |

三段连续实际指令/字节、调用和全部直接出口保存在[证据](cg2d_battle_m_skill263_400_401_sampling_evidence.json)。三段同属父427470，直接目标在本组或父默认437A29；本步没有复用尾段，不据536条声称父17298全部核完。

42FC32实际压缩分派：9 W32跳槽36B、144 W8索引144B，`get_lowcase()` 为262，默认437A29。原字节定位263→42FC39、400→42FDCF、401→430162，180B只证明本组路由，不代其它144项开发/全路由验收。十二S32图偏移553E58/48B沿[BMP01基线](cg2d_battle_m_skill61_sampling_evidence.json)保留，值为3/3/3/2/2/2/1/1/1/0/0/0，仅具名复用不重复计新增表证据。

<a id="battle-m-skill263-400-401-sampling-decision"></a>
## 工程裁决与23处修复

依既有工程委托先SPEC后落码，三根23定义23处：263四处、400十处、401九处。只显式表达已证取样，不新增接口/owner、类型、依赖、保护或异常模型。

| 根/补丁 | 改前差异 | 原端及落码要求 |
|---|---|---|
| 三根01..09 | caster发布后又读全局；skill先payload；level先actor | GetPos返回同位置pub/actor；固定caster payload先live skill；live caster位置→actor→live level→payload |
| 263/10 | Effect实参取样顺序隐含 | live target→固定image→live caster位置→actor，传固定target/0/0/image |
| 400/11 | 初Effect caster和记录位置分别重读 | 一次live caster位置→actor，记录沿同位置，类型400 |
| 400/12..13 | 低图加法在caster取样前 | 两次独立live level，各低路径caster位置→S32原表→加110052/110044→actor/payload；高111001/111011 |
| 400/14、16；401/15、17 | target255比较在pub之后，判空再读全局 | GetPos返回target先比较255，再pub；actor沿同parsedTarget判空 |
| 400/18 | flags发布/Effect隐含重读 | Take固定W32 flags→live target→flags pub→live caster位置→actor，Effect沿固定target/flags |
| 400/19 | 两次Cast0无明确共享/重取样 | 先取target再caster比较；不同时第一次Cast0沿该target，调用后重读target；第二次沿保留/重读所得位置 |
| 401/20 | 低图号在caster取样前计算 | live level低路径caster位置→S32表→加110081→actor/payload；高111014 |
| 401/21..22 | target基础字段先读值 | 每字段分别live target位置→actor→live skill/level→payload |
| 401/23 | Effect全局实参顺序隐含 | 两字段写完后另live W32 flags→live caster位置→live target位置→caster actor，再Effect |

263在原42FD07比较target=-1后有两条机器调用支路，参数和动作相同：42FD14..42FD2C、42FD5B..42FD73均传已取target、固定image、0/0和随后live caster/actor。现共用表达式保留target=-1/其它值的实际参数，不增没有业务差异的人为分支。

400的42FFAA Take返回flags经42FFAF取target、42FFB7 pub，再42FFBD取caster/42FFC7 actor；**不能改为Effect前重新读取flags**。401的flags先pub，随后两基础字段各自取样，4302F0才重新读取flags、4302F6 caster、4302FC target、430303 actor；**不能套400的Take返回固定flags**。两者正常数据相同不构成取样合同相同。

400的42FFD4先取target、42FFD9取caster；不同位置才42FFF0第一次Cast0，42FFF5调用后重读target，430007共同第二次Cast0。相同位置时保留第一次样本供共同第二次，不能在共同入口无条件重读target。三项低图路径均caster先表，保S32和原高段常量，不套114的表先caster次序。

正确生产/消费和生命周期尾全部保留：

- 263：初caster空Finish直接返回；skill/level/fpUsed、25/55/40，target Take/GetPos pub、image Take及另一Take丢弃，Effect、218/74/Move，58固定1，无RID。
- 400：初空同样直接返回；初Effect类型400、246/25/55/40/216/64，两图独立level。每target255给caster Effect记录255后退出；循环actor空Finish后退出；两Cast0、不同角色的target类型400效果/246及各次live target保持。原一次W32 flags/CL位视图、8/10000100/80000反馈与current比较、256/52同位置-51另live、11/76原门禁保持；正常循环尾Move→58固定2，无RID。
- 401：初空直接返回；25/55/40/216/图号，每target255同Echo记录，循环空Finish，flags pub在两target基础字段之前。Effect后target Cast0，再live flags80000成功37/212及target Effect固定参数110074/30/0/23。循环尾caster211→EffectList(0,1,3)→64→37→211→EffectList(0,1,0)→Move→58固定2，无RID；初空不消费该尾。

其它根/头文件/真实分派保持。已证地址用native非拥有指针，非地址S32/W32不拓宽；RPLAN.24不新增补零/拒绝/钳制/回绕、溢出越界或故障模拟，不宣编译后指令形态或异常等价。网络原格式明文，音频继续暂停。

<a id="battle-m-skill263-400-401-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步验收项 |
|---|---|
| I | 三根23定义23处；整CPP从HEAD精确补丁重放，其它正文/属性、关联声明和分派保持 |
| S | 三段536条2090B八页66call的连续实际字节/直接出口与23组原EA，180B实际路由/48B既有偏移上下文 |
| 消费 | 三根角色/字段、各图低高路径、400固定Take flags与两Cast0样本、401两字段后live flags、263两原同参支路 |
| 尾 | 原取包/动作次数、初空直接返回、循环255/空两出口及Move、263固定1/400-401固定2，无RID/新保护 |
| 登记 | 旧812仅五引用/三个symbol/本步对象，双域四局部I/S，七open与唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH/所有消费者-接线-owner失效/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续 |

E467→471仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60任务源码closed10/完整验收0保持。不新F/X/adapted、分类或完整成员，不以本组三根/正常指令/路由表/静态汇总代完整功能或整体通过。

<a id="battle-m-skill263-400-401-sampling-source-state"></a>
## 源码、进度与剩余

三个入口见 `src/YobiRuntime.cpp#ProcessBattleMSkill263/400/401`，旧812有界追加，快照 `cg2d-re-battle-m-skill263-400-401-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)与[看板](../plan/cg2d_measurement_dashboard.md)同步实际状态；不例行刷新总SPEC/索引或压历史。本步23处差异当步修复，继续其它技能真实消费者/全路由、完整父/全部生命周期与适用运行门禁。
