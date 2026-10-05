# 战斗动作226退场、位置与角色恢复取样SPEC（BRA01）

2026-10-06。依cg2d_re/AGENTS.md第8条的工程委托先落本SPEC再实施。仅处理真实动作226；[BAR01](cg2d_battle_attachment_recovery_spec.md)两个helper的七槽清理/三附件恢复继续承接，不重新改动或制造独立原函数身份。

<a id="battle-ride-exit-sampling-baseline"></a>
## 原端基线与实际范围

已打开cg2d.exe当前数据库经MCP只读取证：[4154C0,415939)单chunk、277条指令/1145B，按80/80/80/37四页完整读取，覆盖全部分支及返回。唯一入向405578、type17普通call、父404DB0；当前UpdateBattleActor_404DB0的case226实际调用本根，既有include/cg/YobiRuntime.hpp声明保持。

28个原call逐点记录：404D00标死14、406260两、52C450音效两、53B498截断两、517DA0帧三、422A30装备创建一、439380转态两、422370状态两。源码的两个既有helper展开承接七标死及三标死/一次条件创建；两__ftol由现有F32到S32转换承接，SDK/ABI/PC53仍需实际门禁。音效463/457原额外压栈Y未被既有两参PlaySe_52C450消费，保留既有两参调用与音频暂停，不扩API或恢复音频。

BAR01已读本根两窗100条420B及比较前置8条34B；本轮完整277条1145B中新增169条691B，不再重复计入旧窗。BAR01三父真正未核1385条5495B减本根新增后，224/227剩1216条4804B；完整metadata不能当正文已核。详见[原指令、引用及源差异](cg2d_battle_ride_exit_sampling_evidence.json)。

<a id="battle-ride-exit-sampling-decision"></a>
## 差异裁决与实施

仅修改src/YobiRuntime.cpp中的UpdateBattleAction226_4154C0，十二个具名源片段（入口读取与门禁分列）；不改节点/载荷布局、公开接口或其它根：

1. 入口先固定action.payload、effectRunningC9C和rider13C8，再取得effect1060的载荷、position3/4；field58==0使用该入口rider，原无额外空门禁。
2. second04/first00恢复时，取一次角色指针、发布实际全局槽，再用同指针写callback。随后phase/flags及其它阶段仍依原点读取全局槽，不贯穿缓存。
3. 两角色各offset40/44复制先取当轴目标对象，再读取action当轴F32并发布；四目标指针独立取，不合并两轴/两角色。
4. second方向复制先取全局角色，再取当时rider，然后读角色direction并发布到该rider。
5. 两轴从second.offset40/44截断、各读原volatile U32屏幕偏移并按现有S32使用，再取当轴rider并写deltaX/Y。之后各一次取rider供同一对象的delta读取和x/y发布；X写后再取Y，不共用整个阶段入口。
6. PlayFrame(second,0)后固定movingRider，取field58，保留正常S32的time*time/5及frameY加法、movingRider.y加法。然后重新取counterRider，读其field58+1，先保存offset>=0判定，再发布计数，按同判定进入角色恢复。
7. 首槽旧对象只取一次供判空/MarkDeath，调用后仍按原点清全局槽，再取carrier.first00恢复。
8. 保留entry payload/carrier固定借用、原全部其它调用/字段序及跨调用复读、carrier.second04/first00清空时点。末段rider先phase=0、field58=0、标死，再读取effect1060、清rider、标死同effect、清effect，最后各重新取角色做状态/音效/转态、标死action与共用帧尾。
9. 不立即free，不加分配/空值/索引/溢出保护、钳制/补零/回绕模拟；不为取样修复引入新owner、RNG、公共helper或不可达排除。原正常S32运算与F32字段不扩宽，只有已证指针保留原生非拥有T*。

<a id="battle-ride-exit-sampling-acceptance"></a>
## 可度量验收

- 原证据：完整277条1145B逐页连续、首尾/chunk/入向及28call一致，十二片段对应实际指令；不得以仅元数据、局部窗口或源形式证明运行等价。
- 源静态：从HEAD精确重放本根十二片段即当前整个CPP；验证入口/两个恢复发布、四目标取样/方向/两volatile轴/两个位置借用/计数判定/旧首槽指针及原退出尾顺序；case226/两helper/其它根和已有声明保持。
- 台账：在已有语义index1322、F:000154C0上追加BRA01当前完整根依据，不增语义/库存身份或disposition。BAR01字段为当时局部范围，保留历史；当前源关系/状态追加新说明，两个域成员/完整门禁保持。
- 仅新增YOBI/BATTLE共用本根的四局部I/S，E297→301；语义1324/有效ID1319、disposition426、YOBI25/BATTLE22、非函数代码片段5、LOOP31、R7区间67/3061、队列175及60任务/已闭10和全部完整验收不提升。文档关联1437/3680=39.05%保持，不是整体完成率。
- U08/U09/G01/G02/R01/R04/V04继续open，只追加本根证据。完整所有父/动作参数/数据资源、共享owner和重进退出/浅别名/借用失效、SDK/ABI/PC53/时钟RNG/画面运行仍需对应证据。
- 集中必要静态、UTF-8无BOM/LF、引用/差异、获准文本汇总及-Check后精确提交。遵守不构建/调试/测试/客户端/算法样例/摘要，原格式明文和音频暂停，不例行改总SPEC/索引或压缩历史。

<a id="battle-ride-exit-sampling-source-state"></a>
## 源状态与剩余

本根十二源片段按原实际读取/发布位置修复，入口、case226、公开声明及两个helper实际调用保持。277条1145B为本根完整原指令静态覆盖；不据此宣布战斗或Yobi完整验收。224/227还剩1216条4804B本体未核，全部数据生产消费与owner寿命及运行门禁沿总SPEC继续。固定命名快照`cg2d-re-battle-ride-exit-sampling-20261006`。
