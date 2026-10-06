# 战斗M技能114取样SPEC（BNG01）

<a id="battle-m-skill114-sampling-baseline"></a>
## 原端基线与范围

本步只修复既有 `ProcessBattleMSkill114`。原主体 `42F739..42F9BF` 为153条646B两页18call，真实尾 `436FA1..436FD8` 为13条55B一页2call；两范围同属父427470，合166条701B三页20call。全部直接目标落本组或父默认437A29。

255/循环空Finish均在42F9B6压0/0，再跳436FA1读live caster/actor、压-1并EffectList终结，另live caster Move、58加一。13条尾与[BMM01已证item主体内同区段](cg2d_battle_m_item_combo_sampling_evidence.json)逐字段一致，既有13条55B/2call非新增，本步新153条646B/18call分列。[BMO01实际低段202B表](cg2d_battle_m_skill4_sampling_evidence.json)定位case114→42F739，21 W32跳槽84B/118 W8索引118B仅定位；十二S32偏移48B沿[BMP01原表](cg2d_battle_m_skill61_sampling_evidence.json)保持。

[证据](cg2d_battle_m_skill114_sampling_evidence.json)保存两范围全部实际字节/分页、原图号/flags发布和成功字段路径、调用/跳转、改前源码/八补丁及旧812完整前态。完整父17298/SEH、其它技能及运行另验；网络原格式明文、音频暂停，显式UTF-8。

<a id="battle-m-skill114-sampling-decision"></a>
## 工程裁决与八处差异

依既有工程委托先SPEC后实施，单根八组八定义八处，不新增接口/owner、依赖、类型或保护。

| 项 | 改前差异 | 原端顺序与修复 |
|---|---|---|
| 1 | caster发布后重读全局位置 | GetPos返回位置先pub，actor沿同位置 |
| 2 | 初skill先于payload | 固定caster的payload先取，再live skill |
| 3 | 初level先于actor | live caster、actor、live level、payload |
| 4 | 低图号加法全部早于caster位置 | level比较后原S32偏移表→live caster位置→S32加110056→actor/payload；高路径原111018 |
| 5 | target255比较在pub之后/判空又读位置 | 返回target先比较再pub，同位置actor判空 |
| 6 | Take flags发布/EffectList全局实参顺序隐含 | 固定返回W32 flags→live target→flags pub→live caster→actor，传固定flags/0 |
| 7 | 基础target skill值先于actor | live target、actor、live skill、payload |
| 8 | 基础target level值先于actor | live target、actor、live level、payload |

第4项原42F7FA先读表、42F801读caster、42F806才加偏移；不能套用其它技能的caster先表顺序。第6项原42F8A7 Take后42F8AC读target，42F8B4 pub flags，42F8BA读caster，42F8C0取actor，再42F8C8 EffectList。与107 caster先flags pub/112 flags先target各自不同，不统一套模板；初caster/target局部用parsed前缀。

正确行为全部保留：入口两Take首弃/次GetPos，初caster空Finish直接返回；skill/level/fpUsed，25/55/40，level<12按原表+110056否则111018，139/64。每target Take/GetPos，255退出，actor空Finish退出；正常再Take flags与EffectList(caster,target,固定flags,0)，随后两基础字段和157，调用后读取live flags80000。失败继续下一target，不新状态字段生产或动作。

成功沿该次live target的固定payload写13DC=7、13D8=1，143；调用后另live target写1018=253，80、37，循环。原不做144或状态相等比较，固定payload只供两正常字段写，不冻结给143之后所有动作。循环结束caster EffectList(-1/0/0)→另live caster Move→58加一→父默认，初空直接返回不消费该尾。原无RID、无额外末55/74。

全部其它根/声明/分派及正确尾保持。已证地址native非拥有指针，非地址S32/W32等不拓宽；RPLAN.24不新补零/钳制/越界溢出、回绕或故障模型，不宣异常或编译后指令形态等价。

<a id="battle-m-skill114-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证明 |
|---|---|
| I | 单根八定义八处落码，整CPP从HEAD精确重放补丁，其它正文/属性与声明保持 |
| S | 两范围166条701B三页20call连续实际字节/全部出口，含真实436FA1终结尾；原202B/48B表和八处原EA |
| 消费 | 原低图表-caster-加法/高111018、target先flags pub及固定参数、157后live flags门禁/7-1与253生产和全部取包-动作/失败/终结Move增58 |
| 复用 | BMM尾13条55B/2call非新增，新153条646B/18call分列，不增RID/144或额外55/74 |
| 登记 | 旧812仅五引用/一symbol/本步对象，双域四局部I/S，七open进展/唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH/所有消费者和接线-owner失效/SDK-ABI-PC53/时钟RNG画面及R1/R7/R8继续 |

E441→445仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60任务已闭10/完整验收0保持。不新F/X/adapted、分类或完整成员，不以本根/状态生产/真实尾或静态汇总代完整父/功能/整体验收。

<a id="battle-m-skill114-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill114`，旧812有界追加，快照 `cg2d-re-battle-m-skill114-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)和[看板](../plan/cg2d_measurement_dashboard.md)同步事实/剩余，不例行改总SPEC/索引或压历史。本步发现的八处正常差异当步修复，继续117等其它技能-全路由/完整父、全部消费/生命周期和运行门禁。
