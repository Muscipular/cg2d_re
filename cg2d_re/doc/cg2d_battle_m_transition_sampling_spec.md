# BMI01 战斗转场与宠物消息取样

日期：2026-10-06；按既有工程委托先记录后实施。沿[BMH01数据消息](cg2d_battle_m_data_sampling_spec.md)继续427470的六个真实转场/宠物目标，恢复七项正常宽度/取样差异，保留所有原动作、借用和随机消费。

<a id="battle-m-transition-sampling-baseline"></a>
## 原端有界基线

| case / 消息 | 原区间 | 指令 / 字节 | call |
| --- | --- | ---: | ---: |
| 1 / Escape | 427F98..428209 | 163 / 625 | 21 |
| 2 / Clear | 428209..4282CD | 52 / 196 | 7 |
| 3 / Support | 4282CD..4283B4 | 58 / 231 | 7 |
| 18 / PetDown | 4283B4..428441 | 36 / 141 | 5 |
| 15 / PetNotice | 42A34B..42A3D8 | 36 / 141 | 5 |
| 16 / PetLeave | 42A3D8..42A520 | 81 / 328 | 11 |
| 宠物共用就绪尾 | 429FD9..42A014 | 16 / 59 | 3 |
| 哨兵共用58增量 | 42A2FB..42A303 | 2 / 8 | 0 |
| 空角色共用完成尾 | 42AAFE..42AB0C | 4 / 14 | 1 |

九个同属427470的有界区间448条1743B、十二指令页/60call见[实际字节、分支、源前态](cg2d_battle_m_transition_sampling_evidence.json)。前六块426条1662B/56call，必须计入外部共用尾才覆盖本组六消息的真实结束/就绪路由。两个增量/完成尾复用BMH已证字节，非新函数或未归属X。复用BMG25头表/43B63D data绑定和正确4392A0/439380/Finish/就绪/RNG依赖，不验全17298父或其它消息。

## 六类消息的真实合同

入口固定payload、substate58一次S32取样。非零只在substate1继续就绪：Clear用ready参数1，其余0；Escape/Support/PetDown就绪后Finish，PetNotice/PetLeave在Finish前live取caster角色406260。其它substate或未就绪不作消息动作，helper返回后总入口仍13项回存。原宠物共用尾429FD9和Clear跳入427FAD的ready1不能漏掉或误称独立回调。

Escape状态0取位置/角色，actor存在条件先于caster发布；空即Finish且不再取result。正常取result后**先读caster再发布resultFlags**，按该caster角色追加68；之后reactionImage=22320和追加12各live重取角色，随后才取resultFlags。result0追加50/37/40；result1/2写value1018=211并追加80/52/13，2还先追加213。末先caster→玩家位置比较，再按同caster借角色追加256或51，不能依赖C++参数求值次序。其它result不追加这些列表，所有正常路径live角色MoveState后再live读58递增。

Clear/Support逐取位置，原255比较先caster发布；即使255也发布caster并走共用58增量。非255借角色，空仅continue、不增58。每角色先追加68；Clear live写reaction22320，追加12/13/51；Support分别live写reaction22323/value1018=206，追加80后重新取caster/角色/载荷，以FDC**W8**位2门禁决定57/64，位为0时57用当前已借角色、64再live取角色。两类末live MoveState再取下一位置，完整列表顺序保持。

PetDown状态0先caster发布再借角色，空Finish；存在写value1018=205，live追加80/84→MoveState→live58递增。PetNotice同样先caster发布再借角色，存在固定该actor载荷写reaction22334，然后live追加68/65→MoveState→58增量。两者不套Escape/PetLeave的先actor条件后casterpub，不能统一全部取样。

PetLeave状态0先借actor/存在条件再caster发布，空Finish且不seed/随机。正常用**同解析position缓存**按W32播种一次，然后恰两次RandomRange(0,19)。X原顺序为Rand→live caster→借actor→读点X→读该actor载荷→写x08；Y则Rand→live caster→借actor及载荷→读点Y→写y0C，不归一化成同一工具或提前跨Rand借角色。随后live追加12→reaction22324→追加68→value1018=211→追加80/13/51→MoveState→58增量；原整数图号/参数/失败和随机消费保持。

<a id="battle-m-transition-sampling-decision"></a>
## 七项差异裁决

| 差异 | 恢复 |
| --- | --- |
| Escape/PetLeave两门禁 | 原测试actor在caster发布前，保存actorPresent再发布、按同条件空返；两个源码替换点，不影响另外两宠物的先pub后借角色合同。 |
| Escape结果发布 | 427FF6 caster读取先于FFC resultFlags pub，第一68使用同caster；显式resultFlags局部、caster取样、发布、借角色追加，后续仍live读取。 |
| Escape末玩家分支 | 原两个result分支均先caster/玩家比较再借角色，显式playerPosition/castState后才查角色表，保1/2不同列表。 |
| Clear/Support哨兵 | 原428237/4282F6比较255先于两次caster pub，缓存endPosition，发布后消费；哨兵、空槽continue与58增量保持。 |
| Support位门禁 | 42836E真实byte test，源W32读改unsigned char对象字节别名，不修改FDC字段/布局/其它消费者。 |
| PetLeave X取样 | caster→actor→点X→actor载荷/pub，分开保存非拥有actor和坐标，仅当前X阶段有效。 |
| PetLeave Y取样 | caster→actor→载荷→点Y/pub，分别重取，不复用X角色/载荷跨第二Rand。 |

一个既有源码根七项逻辑/八处替换，只有YobiRuntime.cpp改变；所有其它正文、头、共享布局、RNG/动作/附件核心和现代DX字段接口保持。原生非拥有T*仅拓宽地址，位置/坐标/状态仍S32、flag位按原W8。不加保护/新零写/钳制/回绕或异常模型，不新原F/X、ABI桥、owner或注册；网络原格式明文、音频暂停继续。

<a id="battle-m-transition-sampling-acceptance"></a>
## 可度量验收

| 项 | 标准 |
| --- | --- |
| 原范围 | 九区间448条1743B十二页连续实际字节/60call，六目标的全部原跳转落入本组区间或已证437A29回存尾；共用尾分列。 |
| 源码 | HEAD精确重放一个根七逻辑八处，条件先pub、byte门禁、两个不同Rand后借用序与原列表/就绪/失败/58增量有限静态。 |
| 台账 | 旧812只追加转场/宠物目标合同，无新原身份、分类或完整成员；仅YOBI/BATTLE四局部I/S，七任务open。 |
| 验证 | 必要静态及获准文本汇总/Check，不构建/测试/运行/摘要，不称编译指令或异常等价。 |

<a id="battle-m-transition-sampling-source-state"></a>
## 当前状态与后续

七项差异落码，六转场/宠物目标及外部共用就绪/增量/完成尾有界纳管。完整427470其余消息、所有列表状态消费者和资源owner重进退出/借用失效、SDK/SEH/ABI/PC53/时钟RNG/画面仍须沿实际分支继续开发验收；本组不称全17298父通过。

语义1350/有效ID1345、片段5/adapted456、YOBI55/BATTLE52/RENDER26/LOOP32与关联1463/3680=39.76%保持；E353→357只双域四局部I/S。R7区间67/3061、队列175、60任务及已闭10和所有完整门禁保持，关联率不是完成率。本组14文件，固定快照：`cg2d-re-battle-m-transition-sampling-20261006`。
