# MQC01 地图观察链查找取样与已有清理身份

<a id="map-chara-lookup-baseline"></a>
## 当前缺口与实际链

[MSS01](cg2d_scene_snapshot_width_spec.md)的51D5B0调用43F1F0查找、尾接43F230清理，二者在[场景§74/116](cg2d_scene_state.md)已有本体/原生链承载。当前43F1F0在五编号比较结束后才读payload.next；原43F204先保存next，再比较编号。本步修该取样点，并将已有清理消费绑定原身份，不改三种地图角色载荷布局或头owner。

[原证据](cg2d_map_chara_lookup_evidence.json)保存两体57条134B：43F1F0为22条47B，43F230为35条87B；七引用是六直接call、一尾jump，五父。43F230@51D635为type19尾jmp，不写成第七call，不代完整五父或间接调用。

- 43F1F0先读5F9750头，空链返回0；每节点43F1FF取action+0C载荷，43F204读取其+2C下一节点，43F207..43F212依序比较五个原W32编号。任一匹配返回当前action；全不匹配才使用之前缓存next继续，遍历结束返回0。编号只作W32相等，不变成指针。
- 43F230先读同一头；非空每节点先取载荷、缓存next，依序读五个+14..+24孩子槽，非空才MarkDeath，最后MarkDeath当前节点、沿缓存next继续，尾清头。原空链只清头且EAX未定义，现有返回空值承载及注释保持，不作为机器返回等价通过。
- 当前MapCharaPayload保持既有values五W32、partyActions五原生非拥有指针、frame原S32和next原生指针；action/payload/孩子寿命仍归原Yobi runtime，查找/清理不新建或释放载荷，不补陌生字段/影子表。

<a id="map-chara-lookup-decision"></a>
## 委托裁决与源码

1. 仅YobiRuntime.cpp/43F1F0一模式一块：载荷读取后立即保存YobiAction* next，再遍历原五编号；无匹配时用保存next前进。保原头读取、空/未命中0、命中返回当前action、U32相等及顺序；不重读下一节点或把所有记录预先快照。
2. 43F230已有本体不修改或计新清理实现；其当前缓存next→五孩子逐槽条件死亡→当前节点死亡→缓存next继续→清头合同保持。原非统一返回承载/空链注释不改，不据未知EAX升级稳定action返回合同。
3. 两个已有本体/真实声明追加唯一原身份/来源及adapted，分别关联FUNC-MAP与FUNC-LOOP的实际传递依赖。两域共用同一原ID/源码/证据，不将双关联当四函数/两份开发。
4. 既有MapCharaPayload布局/五项容量、非地址宽度、原生指针、append/unlink/生产/清理/调度与其他callee保持。未见的新alias/地址逃逸、所有生产者及清理后borrow失效/owner重进退出、五完整父、实际ABI/SDK/场景画面RNG运行继续未闭。
5. 不加payload空值或编号范围拒绝、钳制、补零、修链、自循环保护或异常模拟；RPLAN.24不称数学/越界异常安全或故障等价。MSS01及效果池/所有场景和原格式明文/音频暂停保持。

<a id="map-chara-lookup-acceptance"></a>
## 限定验收

- 当前完整源等于HEAD加一模式一块；当前节点载荷→保存next→五编号顺序比较→命中/缓存next前进与原可核，已有43F230全文及全部其余源/头/汇编保持。
- 两原57条134B/库存及七引用六call一jump五父可追，原载荷/编号/next读点及现有清理顺序、两真实本体/声明存在；不代完整五父或所有链producer/alias/owner寿命和原空链机器返回。
- 语义1280→1282/有效ID1276→1278、处置373→375；MAP36→38、LOOP1→3成员。E213→217只两域各局部I/S，四记录共享一源码改动/两原证据，不计清理新增本体或完整功能。
- G01/G02/U11/R01/R04/V04继续open，全部B/D/W/R/完整功能、R1/R7/R8/60任务关闭与整体SPEC不提升。不编译/调试/测试/运行/检查产物或计算摘要。

<a id="map-chara-lookup-source-state"></a>
## 当前交付

观察链查找恢复原提前保存下一节点的读取点；已有五孩子清理消费只补身份，原生载荷/头owner和原返回边界保持。局部源和身份静态不代整个观察链或场景生命周期完成。

固定命名快照：`cg2d-re-map-chara-lookup-20261005`。
