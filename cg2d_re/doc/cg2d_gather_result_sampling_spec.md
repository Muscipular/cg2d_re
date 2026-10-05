# GRS01 采集结果页描述符与状态/坐标取样 SPEC

<a id="gather-result-sampling-baseline"></a>
## 原基线与缺口

沿[UPS01共用预览](cg2d_ui_preview_sampling_spec.md#ui-preview-sampling-source-state)推进实际旧选择39采集结果页；[既有完整合同](ui_windows/legacy_gather_result.md)与[原指令/差异](cg2d_gather_result_sampling_evidence.json)分列。4EDB20完整[4EDB20,4EE122)，437条1538B，一chunk，当前IDA入向引用集合空；不据此称函数不可达或旧57C368表路由已经接通。既有独立语义1035/F:000EDB20只追加，本步不新造父或空业务体。

四具名当前差异：
1. 原16B局部描述符以偏移2/6/10三W32零、0/14两W16零、8处W32 FFFFFFFF、13处W8 4依序显式写入，后两写覆盖前面部分零字节。源现只有四W8 FF/一W8 4，其余借零值初始化，没有原宽度/写序。原descriptor写入与局部button/压栈交错，本步只保descriptor七写相对顺序及首次503160前完成，不宣原机器布局/所有栈发布已等价。
2. 原4EDC84先比较mapped<110350，再4EDC8A发布对象image，4EDC90按前次标志分流；未小于才4EDC92比较上界。源先SetYobiActionField2A8才判下界，省略原先判标志。
3. 原4EDFC2取结果image、4EDFC8取panel.y，4EDFD2加184，4EDFD9再取panel.x，4EDFE6加192后绘图。源先取Y/X，再在实参处计算两轴。
4. 原详情行503AB0非零时，4EE040取panel.y，4EE051计算Y+20*row+237，4EE058再取panel.x，4EE064加10后绘文本。源先计算row偏移调用drawAt，再由其取Y/X，最后在DrawText实参处加Y偏移。

本组完整保存仅证明相关原字段/分支有基线，不代旧实际路由、数据生产、资源/对象寿命或运行。

<a id="gather-result-sampling-decision"></a>
## 裁决与实施边界

依AGENTS.md第8条工程委托采用，仅4EDB20四源码片段：
- 保既有16B bytes值类型/零值初始化，局部U32零/U16零/U32 FFFFFFFF，通过memcpy分别显式发布2/6/10、0/14、8，再直接W8写13=4；不把三重叠零写合并，原正常显式零写不是异常补零。其它按钮构造/坐标/首次503160与其它描述符保持，不新建通用初始化工具或改公开布局。
- 先保存bool belowRebirthRange=(mapped<110350)，再写当前action.image，随后按!belowRebirthRange及mapped<=110373走原两分支。两rebirth写仍逐次取共享D67CD4，非族仅写309=0，最后再读共享槽供原创建门禁；不补310/改对象复读与owner。
- 图片显式image→panel.y→imageY=Y+184→panel.x，再按X+192/imageY绘图。原layer213/mode0/descriptor和后续suffix绘制保持，不迁移其它drawAt。
- 详情八次ExtractLegacyLine(buffer,256,原借详情,row,28)保持；仅非零块显式Y→lineY=Y+20*row+237→X，再DrawText低S16(X+10,lineY),layer2/font0/color0/三个原尾参数。空行仍继续下一次，不补buffer/默认正文或新增输入门槛。

完整S32正常算术、低S16文本坐标与原U32时钟门禁保持；不新增异常保护/钳制/回绕或故障模拟，RPLAN.24不取消原显式正常写。原state0/2分支、两明文62发送条件/Y后X、原MarkDeath清槽/混合指针返回、五族504C50与创建失败发布、原结果输入、pair低S8→skill低S16、按钮bit0/bit40与repeat门槛、FP值/倍率正常乘除和比较、背包/消息/音效/清repeat、成功504180(38)后清CF46C4、原格式/字体/详情/悬停和5035E0→503440尾返回全部保持。音频暂停，不扩大为相关业务排除；网络原格式明文不改变。

<a id="gather-result-sampling-acceptance"></a>
## 可度量验收

1. 完整437条1538B及原chunk/末界/空入向集合可追；只四相关取样片段验收，空引用不当业务不可达。
2. 全CPP只能精确重放四片段；descriptor七写/重叠/宽度、mapped下界先判再发布与后上界、图片和非空详情行Y计算早于X取样及原低S16/调用参数成立。所有其它源/头/汇编与原字段/分支/owner不变。
3. 旧语义1304/有效ID1299与非函数片段5保持；一adapted处置403→404，公共UI成员13→14及UI-L-039成员0→1为同一F，不多计原函数。E269→273仅双域局部I/S共用一源修正/原证据，不提升整门禁或叶验收，R7区间67/队列保持。
4. U04/U10/R01/R04/V04及R1/R7/R8/整体保持未闭；整表路由/本页完整生产消费与原调用者、owner重进退出/借用失效、真实ABI/SDK/PC53/时钟RNG/完整画面运行仍需相应证据。
5. 仅必要集中静态、文本汇总/-Check、引用/UTF-8无BOM/LF和精确提交；不构建/测试/算法样例/客户端/摘要，不全项目复检或例行改总SPEC/索引/压历史。

<a id="gather-result-sampling-source-state"></a>
## 源状态与剩余

真实src/LegacyUiRecord.cpp的Sub_4EDB20及src/LegacyPanelRuntime.hpp既有声明/非拥有panel和对象槽继续；全部四改动仅该根，既有cstring/cstdint足够。共享预览4FAAA0/4FACE0/4EF5F0、其它采集/技能/物品/制造父及全部源/头/汇编保持。

本组只当前源码与局部I/S。旧57C368外层实际分派、玩家/技能/回复和资源的完整真实生产、原未验数据/owner寿命/ABI/SDK/PC53/时钟RNG/完整采集往返与实际画面仍未闭；不据一旧身份补成员关闭全功能。固定快照`cg2d-re-gather-result-sampling-20261006`。
