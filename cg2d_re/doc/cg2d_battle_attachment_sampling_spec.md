# BAS01 三战斗附件同步的字节门禁与取样 SPEC

<a id="battle-attachment-sampling-baseline"></a>
## 原基线与实际差异

沿[BTS01主回调公共尾](cg2d_battle_tail_sampling_spec.md#battle-tail-sampling-source-state)的三个真实消费入口推进；[完整原指令/四表/源差异](cg2d_battle_attachment_sampling_evidence.json)保存原事实，[既有战斗回调](yobi_updates/battle_actor.md)保留历史分工。

| 入口 | 完整范围 | 指令/字节 | 实际入向 |
|---|---|---|---|
| 421FB0骑乘同步 | [421FB0,4221C0) | 139/528B | 4056B3唯一call |
| 4225D0主装备同步 | [4225D0,42277A) | 126/426B | 4056C6唯一call |
| 422780辅助装备同步 | [422780,4229A9) | 156/553B | 4056E5/4056FA两个call |

合计421条1507B，各一chunk、4普通call且同一已知父404DB0；三个原身份均已有真实正文但未登记语义。四原表另列：421FFF→[4221C0,4221EC)11槽、422806→[4229AC,4229C0)5槽、422818→[4229C0,422A10)20槽、422880→[422A10,422A30)8槽，合44槽176B，不混入函数指令字节。后三表分别为type-2、parentAction及direction分派；保有界unsigned默认分支，不加范围保护。

十三具名片段按三根分列：
- **骑乘五片段**：原先取action.value1D4再取parent、表项及payload；源先parent后位置。原422067/422079各读payload.FDC低W8、422073读parent.flags低W8；源按W32读取后掩码。原422087先读取parent.direction并用同值比较/写入，源判定与赋值重复读。原三次flags发布沿EAX→ECX→EDX保存前次值，各阶段独立取parent.flags；源复合赋值重复读取目标。原每轴先读取S32 frame，再F32 scale，再按既有double乘积截断/取当前父坐标/相加发布；源把两读取合在乘法表达式。
- **主装备五片段**：原取位置→parent→表项并比较→固定payload→据比较分流，源顺序不同。原422666新读payload.FDC低W8、422672读parent.flags低W8；另422678确用最初W32 flags的AL快照，源该快照正确不可改成重新读取。parent.direction与三阶段flags缓存同上；投影四个帧/缩放读取需明确顺序，原骑乘路径两轴均用parent.scaleX、X发布后重新取rider供Y，源该区别正确保留，不统一为scaleY或同一rider快照。
- **辅助装备三片段**：原先取parent.direction→action.value1D4→写direction再查表，源写direction后取位置。type3/4/5原只读取一次parentAction分派，再写对应常量；源命中后再读取parentAction。原坐标截断后重新取当前parent.deltaX/Y，再按原S32加法次序发布；源未明确读取相对于加法的时点，不得复用先前delta字段复制的快照。

原三根失败合同不能统一：骑乘/主装备无额外action/parent/payload空保护，身份不符MarkDeath并返回；辅助装备原确有action→parent→payload三空返回，方向在身份检查前发布，不可借RPLAN.24删除。三根只标死不立即free；原非拥有parent、载荷、注册表和主装备两阶段rider借用保持。

<a id="battle-attachment-sampling-decision"></a>
## 裁决与实施边界

依AGENTS.md第8条委托采用，仅src/YobiRuntime.cpp三根十三片段：
1. 骑乘/装备先保存位置再读取parent及表项；装备另先保存hasRegisteredParent，再固定payload并按同判定分流，骑乘保原payload读取在比较前。
2. 五处原W8读取直接经const unsigned char别名读取已有W32字段的首字节，沿原x86/x64小端低U8，不读高三字节、不写或改变字段布局；不使用当前按整W32截取的GetYobiActionFlagsLowByte，不改其其它消费者，不添加公共helper。
3. 两root保存parentDirection后按同值比较/发布；辅助方向/位置按原序各保存一次，保后续类型分派独立重新读取parent方向。
4. 两root以局部U32 drawFlags保存三阶段值，每阶段仍发布到action.flags后才读取下一parent.flags；主装备最后mask→OR前阶段值→OR extraFlags→发布，原完整flags快照/低字节重新取值门禁各自保持，不一次取完所有flags。
5. 骑乘与装备各轴显式取frame S32，再取scale F32，再沿既有double乘积正常截断后取当前坐标/相加发布；不增加F32乘积舍入，不改缩放来源，装备X发布后原rider复读和自己scale不变保持。PC53/x87生成指令及运行仍另验。
6. 辅助type3/4/5在独立case作用域保存一次parentCharacterAction供分派/赋值，不跳过其方向分派；type2/6/default、graphicIndex=-1/stopAtEnd差异和全部八方向偏移保持。坐标各自截断后独立重新取parent.deltaX/Y，再按X+deltaX+xOffset、Y+yOffset+deltaY原正常S32次序发布；保之前delta/offset属性复制，不合并复读。

原常量FFEA7FFD/820000/118000/40002/2000000、batchType与两rebirth字节/完整color2、phase非零的骑乘路径、动作映射与播放门禁/实参/次数/extraFlags和MarkDeath顺序保持；四表/所有其它正文、主404DB0调用点及三个已有声明不改。只拓原已证非拥有指针，非地址字段宽度不改；不新owner/句柄/共享副本/guard/钳制/拒绝、回绕或故障模拟，不新增Read/Ctor/Destroy/RNG或改变音频暂停与原格式明文。

<a id="battle-attachment-sampling-acceptance"></a>
## 可度量验收

1. 原完整421条1507B/各一chunk/连续末界及4普通call/同一父可追；四表44槽176B字节/表项与原分派另列，六页原指令全文可对照。
2. 全CPP从HEAD仅精确重放三根十三片段；五W8门禁、位置/方向/父身份取样、flags三阶段读后发布、每轴frame→scale与装备rider复读/两轴scaleX、辅助类型作用域与坐标复读成立。原失败/表映射/播放及属性宽度和所有其它源/头/汇编保持，不称本机指令已经验收。
3. 三已有真实本体身份新增语义1309→1312/有效ID1304→1307、代码片段5保持；三adapted411→414。YOBI10→13/BATTLE7→10同三F不双计，E285→289仅双域局部I/S共用同十三片段/原证据，LOOP31/R7区间67/队列与全部完整门禁保持。
4. U08/U09/G01/G02/R01/R04/V04及完整R1/R7/R8/整体保持open；全部附件生产/主回调/各动作和数据资源、owner重进退出/浅别名/借用失效、SDK/ABI/PC53/时钟RNG/画面运行仍需对应证据。
5. 仅必要集中静态和获准文本汇总/-Check/新引用/UTF-8无BOM/LF及精确提交，不构建/测试/算法样例/客户端/摘要，不全面复查、总SPEC/索引例行更新或压历史。

<a id="battle-attachment-sampling-source-state"></a>
## 源状态与剩余

SyncBattleRider_421FB0/SyncBattleGear_4225D0/SyncBattleAuxGear_422780继续由既有404DB0公共尾的1/1/2个真实调用消费，同一节点/载荷/注册表owner与原生非拥有引用保持；include/cg/YobiRuntime.hpp已有声明不改。本组修取样和字节门禁，不新建附件工厂或删去真实失败分支。

只本组局部静态，全部生产者/父与动作实参、数据资源/owner寿命、PC53和本机逐指令/异常重叠或并发等价、SDK/ABI/时钟RNG/实际画面未验，沿RPLAN.24不设异常模拟门禁。网络原格式明文、音频暂停保持。固定快照`cg2d-re-battle-attachment-sampling-20261006`。
