# BTS01 战斗主回调公共尾与附件消费取样 SPEC

<a id="battle-tail-sampling-baseline"></a>
## 原基线与实际差异

沿[BCS01克隆/残影创建](cg2d_battle_copy_sampling_spec.md#battle-copy-sampling-source-state)推进[既有战斗回调](yobi_updates/battle_actor.md)的真实消费者404DB0；[原完整指令、257槽表与源差异](cg2d_battle_tail_sampling_evidence.json)分列保存。

404DB0完整[404DB0,40573C)为684条2444B、单chunk，13个type1回调地址发布/10个已知父，没有直接call入向。入口取scene、先比较7、再固定action.payload，scene7直接返回；非7读取完整S32 phase，原unsigned >256走公共尾。表[40573C,405B40)另为1028B/257个W32非拥有代码地址，全部指向200个已知动作入口；当前源257个case/200个调用目标逐槽已对应，没有分派缺口，不重造或变更表/动作本体。

原共用尾沿入口载荷：1004严格等1才递增1000；405652保存递增值到EAX、405654发布count、40565A用保存值与2比较，>=2置0；405669再读取当前1000与1比较，等1才调用41E5F0。800000标志清才投影：X截断→取S32屏幕X→读取Y的F32→正常S32 X相加/发布→Y截断→取S32屏幕Y/相加发布；不提前缓存两屏幕偏移或Y。

附件路径的真实次序：4056A8一次读取rider13C8并用同值判空/调用421FB0；调用后4056BB一次读取gear13B4并用同值调用4225D0；调用后4056CE重新读取rider并先判非空，4056D6读取firstAux13B8后才按该骑乘判定分流。无骑乘走aux0同步→再取aux1同步；有骑乘走aux0 MarkDeath→清0→再取aux1 MarkDeath→清1。第二槽必须在第一槽调用及可能清除之后读取，不跨调用缓存全部槽；不改附件类型13C0/13C4、不立即free或重绑owner。

四具名源差异：
1. scene比较写在固定payload之后，未表达原先判定后固定载荷。
2. 残影计数用++直接发布后再次读取判>=2，丢失原递增值寄存器快照；后续==1原确需重新读取，不能一并缓存。
3. Y截断和屏幕Y读取放在同一个+表达式，未表达原先截断再取屏幕Y的次序。
4. rider/gear/aux的判空与传参重复读槽，分流后才取firstAux，未表达调用前指针快照和原third-rider判定→firstAux读取→分流；原不同阶段重新读取rider必须保留。

<a id="battle-tail-sampling-decision"></a>
## 裁决与实施边界

依AGENTS.md第8条工程委托采用，仅src/YobiRuntime.cpp一根四片段：
- scene读取后先保存isSceneSeven，再固定payload，按同判定返回；不增scene/对象/载荷空保护。
- 先保存正常S32递增count，再发布1000，以count判>=2，保原>=2置0及后续直接重读1000==1；不把本步误套成BCS01残影“判定先于发布”，两原序不同。
- 保X/Y原取样/发布次序，仅X发布后显式保存Y截断的S32，再读取同源屏幕Y并相加发布。无钳制/回绕/故障模拟，不替换原正常算术。
- 首次rider、gear各保存原生非拥有YobiAction*供原判空与调用；之后独立重读rider并保存hasRider，再读取firstAux供两个原分支。secondAux分别在该分支第一槽消费结束后读取一次；原MarkDeath后清槽、无骑乘两同步索引0/1与全部其它字段/动作分派/头文件/载荷布局保持。不跨阶段复用rider，不遍历合并两aux，不新建owner、句柄、状态或公共工具。

此前257case/200动作已落位，原表逐槽与源静态对应不验收200个完整本体/参数、13绑定父或整个战斗。只纳管404DB0原身份与本组实修；不借函数数量或局部修正关闭功能。

<a id="battle-tail-sampling-acceptance"></a>
## 可度量验收

1. 原684条2444B完整/单chunk/连续末界、13地址绑定与10父分列；257槽/1028B单列，不计入函数代码字节。九页原指令及四页表记录可追，不把回调绑定计普通call。
2. 全CPP从HEAD仅精确重放四片段；原257case/200目标保持并逐槽对应、原入向角色与声明可追。场景判定/固定payload、count发布后用递增快照且==1重新取值、Y截断/偏移顺序、各指针单次读取与第二槽在第一槽消费后读取成立。
3. 原函数新语义1308→1309/有效ID1303→1304、非函数片段5保持；一adapted410→411。YOBI9→10/BATTLE6→7同F不双计；E281→285仅双域局部I/S共用同四实修/原证据，LOOP31/R7区间67/队列/完整门禁保持。
4. U08/U09/G01/G02/R01/R04/V04及全R1/R7/R8/整体保持open；全部动作、父/callback参数、战斗数据和资源生产、owner/浅别名/重进退出/借用失效、SDK/ABI/PC53/时钟RNG和画面运行仍需相应证据。
5. 仅必要集中静态、获准文本汇总/-Check/新引用/UTF-8无BOM/LF与精确提交；不构建/测试/客户端/算法样例/摘要，不全面复查、例行改总SPEC/索引或压历史。

<a id="battle-tail-sampling-source-state"></a>
## 源状态与剩余

既有真实UpdateBattleActor_404DB0与include/cg/YobiRuntime.hpp声明保持单一action/入口payload和共享节点槽；实际NewBattleActor_438290等13原绑定仍按各自父合同，本步不修改绑定或子动作。内存数值载体不属于网络cipher叶排除；原格式明文及音频暂停保持。

只本组消费取样与静态表对应通过，不证明本机逐指令/原异常重叠或并发等价，不放宽RPLAN.24。完整所有父/动作及资源owner寿命、SDK/ABI/PC53/时钟RNG/实际画面仍未闭。固定快照`cg2d-re-battle-tail-sampling-20261006`。
