# MEP01 效果池初始化写宽与链表取样

<a id="map-effect-pool-baseline"></a>
## 实际缺口及既有链

[场景§20.4–20.5/20.9](cg2d_scene_state.md)已有唯一500项效果池、原生prev/next和领取/回收/清理本体，[MFP01](cg2d_map_effect_producer_spec.md)已修载荷取样。当前46E2A0用U32读改写清C8A744低半字；原46E2DE只写W16且不读该字段。46E340/46E390部分头/链接用重复字段表达式或在原写点后才取样，本步按原指令落实读取点及写序，沿原别名关系，不建立新池或保护。

[原证据](cg2d_map_effect_pool_evidence.json)保存四体105条400B、25直接call/10父。原根/计数分别：46E2A0 27条157B、46E340 19条67B、46E390 27条79B、46F160 32条97B。父身份仅指直接引用，不代全部父或间接调用验收。

- **初始化**：首项发布空闲头、首prev清零；499次逐项next→下项prev交替写，末next清零，活动头清零。46E2DE仅写C8A744低W16，邻C8A746保持；各独立W16/F32槽按原序初始化，返回末记录。不清记录载荷、启动/计时/投影/动作槽或其他未知字段。
- **领取**：空闲头为0直接返回0；46E34A保存record.next，46E352发布新空闲头，再只清保存的nextFree.prev；46E361先读旧活动头，46E367清record.prev、46E36E写record.next，46E370再读活动头，非空才写其prev=record，最后发布活动头并返回record。
- **回收**：空参原样返回；先读prev，若非空再读当时next、写prev.next；46E3A8在前写后再读next，非空再读当前prev写next.prev。活动头等于record时再读当前next并改头；46E3C9先取空闲头，再清record.prev、写record.next、发布空闲头、清kind低W16并返回record。原不改旧空闲头prev，不追加反向链修补。
- **已有清理**：46F160从活动头取种类，非6/7/8只沿next；kind8先MarkDeath、清共享动作槽；再缓存next、回收record、W16计数减一、用缓存next继续。该本体当前源码符合上述局部顺序，本次不修改或宣称新实现。

<a id="map-effect-pool-decision"></a>
## 委托裁决与源改动

1. 仅SceneState.cpp三模式/三局部块：46E2A0以既有memcpy方式准确写2B零半字，保packed U32承载和高半字；不读改写整U32，不新分裂共享owner。
2. 46E340保存nextFree并用于发布/条件清prev；先捕获activeHead再清record.prev并接next；接next后重新取currentActiveHead，用于条件回链。两个活动头取样点保留，不合并快照或添互斥。
3. 46E390保存用于第一条件/目标的prev；第一次条件写后才读取next并用于第二条件/目标；第二写中的record.prev保持当时读取，不提前把prev/next一次性快照。原活动头检查/条件取next和尾段空闲头先读后清prev/接next/发布/kind写保持。
4. 原500项、未知0x44载荷、原生非拥有prev/next、头owner/寿命、空参/空池、返回和数值宽度保持。不给self/交叉alias、重复回收、负计数或未知载荷加拒绝/修复/保护；RPLAN.24异常不专项模拟或称故障等价。
5. 四已有本体补唯一原身份/真实来源及adapted；三根为本次源实修，46F160仅已有清理消费纳管。原25call/10父保存不代完整调用者、全部alias/owner重进退出与借用失效、真实ABI/SDK/绘制/运行。MFP01/MFA01/MCA01及四绘制消费均不改。

<a id="map-effect-pool-acceptance"></a>
## 可核标准

- 当前完整源码等于HEAD加三模式/三块；低半字写为sizeof(uint16_t)且原高半字保持，领取/回收读写按上述点可对照；其余源/头/汇编及46F160本体保持。
- 四原105条400B/库存范围及25call/10父可追；500项499轮逐项链接/末返回、既有生产与清理消费局部原生指针接口可核，不扩大完整场景或所有间接别名结论。
- 1275→1279语义、1271→1275有效ID、368→372处置，MAP31→35明确成员；E207→209仅本次局部I/S。G01/G02/U11/R01/R04/V04保持open，不关闭完整地图生命周期或队列门禁。
- 全部场景/调度/初始化重进退出/加载绘制、多种记录或动作borrow失效、实际ABI/SDK/画面/RNG运行、R1/R7/R8及整体SPEC继续未闭。不编译、调试、测试、运行样例或计算摘要。

<a id="map-effect-pool-source-state"></a>
## 当前交付

效果池初始化恢复原低W16写，领取和回收落实原头/链接读取及写入顺序；四真实本体/声明获得原身份，其中已有清理本体未修改。局部池合同静态不等于所有效果或完整生命周期已验收。

固定命名快照：`cg2d-re-map-effect-pool-20261005`。

## 关联快照口径（MSS01，2026-10-05）

[场景快照SPEC](cg2d_scene_snapshot_width_spec.md#scene-snapshot-width-decision)将51D5B0的同一C8A744低半读取和高半缓存发布恢复原2B宽度；本SPEC初始化仍只清低半、高半保持，不增加复制高半或全W32清零。场景快照与池/全W32消费者继续共享唯一owner，500项/领取/回收/清理及其它数值槽不改；局部快照门禁不代完整池或场景寿命/运行。
