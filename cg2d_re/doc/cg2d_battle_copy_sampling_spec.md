# BCS01 战斗克隆字组复制与创建/残影取样 SPEC

<a id="battle-copy-sampling-baseline"></a>
## 原基线与缺口

沿[YAS01共用分配](cg2d_yobi_allocation_sampling_spec.md#yobi-allocation-sampling-source-state)及既有Yobi克隆/战斗正文推进；[原指令/引用和差异](cg2d_battle_copy_sampling_evidence.json)保存五完整入口。

| 入口 | 范围 | 指令/字节 | 入向 |
|---|---|---|---|
| 41EAB0克隆 | [41EAB0,41EB07) | 38/87B | 4个type17 call/4父 |
| 41EB10成员复制 | [41EB10,41F106) | 335/1526B | 41EAE7唯一call/父41EAB0 |
| 41E5C0残影更新 | [41E5C0,41E5E1) | 11/33B | 41E603一个type1回调地址发布 |
| 41E5F0残影创建 | [41E5F0,41E636) | 23/70B | 2个type17 call/2父 |
| 438290基础角色创建 | [438290,4383A1) | 71/273B | 10个type17 call/3父 |

合计478条1989B，各一chunk；17普通call与1地址发布、10个已知父分列，不将回调绑定计调用。Clone/Copy旧语义1134/1135保持，另外三已有真实正文的原身份缺失。

本次确认正确、必须保持：Clone在41EABD先捕获source.payload，分配失败原空返保留；成功保存新prev/next/payload、调用成员复制、按next→prev→payload恢复再按原生BattleActorPayload sizeof浅复制入口载荷快照。41EB10十个CryptInt仅+04 encoded/+08 mask/+0C readCount读后发布，不复制token/reserved，不调用读取/构造/重新编码或增加RNG。不能以整对象memcpy/重构CryptInt修正不存在的缺口。

五具名实际差异：
1. 41EF44–41EF86原+290..2A3为五W32逐读逐写；源std::array<byte,20>整体赋值，未表达字组的原取样/宽度。
2. 41F082–41F099原16B descriptor为四W32逐读逐写；源bytes描述符整体赋值，未表达各字组读后对应发布。
3. 41E5CD递增局部count、41E5CE先判>=12、41E5D1才发布到payload.FF0并分流；源先发布再比较。
4. 4382B9取flags、4382BC写callback、4382C3先OR60054，再4382C9写image100500、4382D3发布flags；源OR在image写后。
5. 4382FB先读取首pointIndex，438302才固定payload，再读取point.x转F32发布offset40；源先固定payload再首次索引读取。后五次pointIndex原独立读取，不能一并缓存。

<a id="battle-copy-sampling-decision"></a>
## 裁决与实施边界

依AGENTS.md第8条委托采用，仅同CPP三根五片段：
- Copy私有helper的20B bytes290及16B drawMode各以4B步进，在每次迭代先memcpy到局部U32 word、再memcpy到同偏移目标，分别五/四字组；不整段读取后一次发布。源/目标字段类型和边界不改，不添公共工具；局部word不预置0，不把这些非地址字节当指针或拷到全对象。原同对象读写不靠源/目标整段memcpy，两个传输均经独立局部字组。
- 残影先保存expired=(count>=12)，再发布count，原分支MarkDeath(action)和其它全部正文保持；正常S32+1，不加上限/默认或回绕/故障模拟。
- 基础角色原flags读取/callback发布之后先保存configuredFlags=flags|60054，再image100500和flags发布；首pointIndex先读取再固定payload供第一个X转换，仅首一次用该快照，其余五读取仍独立。
- Clone/残影创建全文与十CryptInt三字段复制、原所有标量/指针联合槽/文本数组和字段序保持；原W8 priority/显示层、低W16 field270、F32 offset/value54、PlayFrame后X截断/Y取样/X发布/Y截断发布、已有calloc/free域及原生T*浅复制/owner不改。不新增Ctor/Destroy/Read/RNG调用，不新加Clone caller空检查或改变失败路径。

原x86 W32是本组非地址字段宽度，已证指针仍按原生x64承接；并不把原0x330整个机器布局直接复制，也不把原载荷13E8计数强压成x64旧大小。正常字组/字段拷贝不是异常补零；不设数学溢出/越界保护、钳制/拒绝或模拟，沿RPLAN.24。网络原格式明文、两加解密叶范围排除与音频暂停保持；内存数值载体不属于本轮网络加解密。

<a id="battle-copy-sampling-acceptance"></a>
## 可度量验收

1. 五完整478条1989B/chunk与17普通call+1地址发布/10父可追；仅本组相关字组与取样静态，所有完整父/调用参数、资源和寿命未据此验收。
2. 全CPP只能精确重放五片段；两固定20/16B字组范围/4B步进/各读后同偏移发布及九字组，expired先判后pub、OR先于image和首索引先于payload成立；其它Clone/Afterimage创建、十CryptInt/旧字段与所有其它源/头/汇编保持。
3. 两旧语义只追加，三新记录纳管已有正文：1305→1308/有效ID1300→1303、代码片段5保持；五adapted处置405→410。YOBI4→9/BATTLE1→6同五F不双计；E277→281只双域局部I/S共用同五实修/原基线，LOOP31/R7区间67/队列和完整门禁保持。
4. U08/U09/G01/G02/R01/R04/V04及R1/R7/R8/整体不关闭；全部战斗状态与目标/资源/数据生产、owner重进退出/浅别名/借用失效/全部callback、真实SDK/ABI/PC53/RNG/画面运行仍需对应证据。
5. 仅集中必要静态与获准文本汇总/-Check/引用/UTF-8无BOM/LF及精确提交；不构建/测试/算法样例/客户端/摘要，不做全面复查或例行改总SPEC/索引和压历史。

<a id="battle-copy-sampling-source-state"></a>
## 源状态与剩余

src/YobiRuntime.cpp私有Copy helper及公共Clone/Afterimage/基础角色入口继续单一实际对象/载荷与队列；include/cg/YobiRuntime.hpp已有公开声明和LegacyCryptInt.hpp布局/CopyPayload不改。本步不改变内存数值生成或战斗分发，也不虚构原RNG结果。

五原身份与局部I/S可追，完整战斗/全部原实参和目标、所有数据资源与owner/浅别名/借用寿命、SDK/ABI/PC53/时钟RNG和实际画面仍未闭；源步进形式不证明本机逐指令/原内存重叠异常或并发等价。固定快照`cg2d-re-battle-copy-sampling-20261006`。
