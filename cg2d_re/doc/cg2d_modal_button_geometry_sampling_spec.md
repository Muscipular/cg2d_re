# 公共模态按钮几何与取样子SPEC（BSP01）

2026-10-07。沿删除确认和人物模态共用503990／503A20推进；依AGENTS第8条委托先记裁决，再实施两个已有原语。公共接口／记录布局／人物父不调整。

<a id="modal-button-geometry-baseline"></a>
## 原端基线与实际缺口

| 原根 | 完整原范围 | 调用与入向 |
| --- | --- | --- |
| 503990命中／鼠标标志 | [503990,503A15)，54条133B／一页、9跳转／2RET | 1直接5049A0；472个CALL，102个实际父 |
| 503A20按钮绘制 | [503A20,503AAE)，62条142B／一页、2跳转／1RET | 3直接5049A0／522180／456780；48个CALL，30个实际父 |

合116条275B／两指令页、4直接／3唯一目标、11跳转／3RET，无间接。入向独立七页读全，原调用多不等于全部父验收。人物本链45E0C0@45E1A2／45E1BD命中、@45E214／45E22F绘制，462090@462147命中、@46219E绘制均实际存在。

原503998..5039A6按top→left→height→width读记录，先bottom=height+top，再right=width+left；当前先用AddWrapped计算right、bottom，又在命中实参读取left／top。绘制命中前503A2E..503A42则按top→height→left→width、bottom→right，不能套同一读取次序。当前回绕辅助器也与RPLAN.24撤普通几何回绕模型的裁决冲突。

绘制命中后503A58..503A78再次读top，hoverMode-1，然后left／height，计算bottom=height+top+1、top-3，再读width、right=left+width+2及left-2；必须保命中调用之后独立取样，不能缓存命中前记录跨调用。四边最终U16是真边界，保留。

503A86先读文本T*，503A96读textY低WORD，503A9B读textX低WORD，最后456780。当前把三处读取直接放在C++实参，求值顺序不明确；恢复text→Y→X三个本地借用／定宽样本，可选hover完成后才取，不能提前跨绘制调用读文本。

503990原三次低BYTE样本D68D9C→D68D98→D68DA0，click样本末尾继续复用；0x40基值和八位映射现源码正确，全部保持。原普通值正确，本步修数据流与撤模型，不称运行故障。

两相邻X:00103A15-00103A20十一字节、X:00103AAE-00103AB0两字节共13B，全90逐BYTE解码NOP，无入向，前RET后真函数入口，只原对齐用途，不造helper／块／父身份。

<a id="modal-button-geometry-decision"></a>
## 实施裁决与保持项

单源五片段：删除ToBits／AddWrapped，LowInt16仅调整为接受S32并保最终U16位型转S16；命中和绘制分别按原顺序取齐记录字段；几何与step采用普通S32；命中后hover重新取记录；可选hover后text→textY→textX取齐再画字。不专项处理溢出／越界／回绕，不加保护、钳制、补零或输入限制。

保持const CharacterModalUiRecord*调用期非拥有，六个S32字段偏移0..14和text@18既有x64 T*／sizeof20不变；不新增owner／全局／句柄表。保持鼠标同源alias、低W8三次取样与全部结果位、未命中0、hoverMode为0不查询、不hover，非零且命中才hover、所有路径继续文字、文字返回、renderMode／color／layer低W8与所有零实参／imageSequence，四角U16及文字S16。其它完整源码、声明、所有父与三个callee正文保持，不新增业务／依赖／RTTI。

网络原格式只明文、音频暂停、既定排除与执行限制保。公共入向父的广泛性单独记录；本次仅纳管两个原语及人物实际消费关系，不外推其它父、输入事件owner或渲染队列整链验收。

<a id="modal-button-geometry-acceptance"></a>
## 可度量验收

1. HEAD整源以五唯一from→to构expected，逐字匹配；命中、绘制前／后取样和文字取样分阶段保，鼠标结果整段原文不动，最终窄边界不删。
2. 两原根116条275B／两页连续字节、4直接／3目标、11跳转／3RET与520实际CALL入向完整归档，七引用页不截断；两相邻13B只用途登记。
3. 新1596／1597分别对应F:00103990／F:00103A20、两adapted，CHARACTER11→13；语义1596→1598、有效1589→1591、文档1707→1709／3680（46.44%）、处置731→733、E850→852；R7 302→304／剩2757。
4. U11／V03／R04只current／引用仍open，P05原closed、closed11／完整叶0／globalfalse及全部完整门禁、冻结定位／pending18／差队列保。四集中静态后汇总器与-Check同句柄至终态，再核完整派生快照、12文件UTF-8无BOM-LF／16实际来源引用和差异。

全部实际父／三个依赖完整生产消费、所有正常alias-owner寿命／输入事件／文字图像接线、CRT-SDK／机器ABI／链接、人物创建-主地图与原格式明文往返／画面／运行R8未验。局部静态不代整体验收，不构建、测试、运行或计算文件摘要。

<a id="modal-button-geometry-source-state"></a>
## 实施追踪

预定Git命名快照：cg2d-re-modal-button-geometry-sampling-20261007。对应[原证据](cg2d_modal_button_geometry_sampling_evidence.json)、[人物场景](cg2d_character_select_scene.md)、[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。通过集中静态与派生终态后追加实际结果，不预记运行或整体完成。

实际结果（2026-10-07）：两个原语／五片段已实施，普通S32几何与各阶段字段、hover后text-Y-X取样恢复，真实U16／S16和鼠标结果保持。两根116条275B／两页、520CALL七引用页与双13B用途证据落地；四集中静态、measure与-Check同句柄终态0、完整派生快照和12文件编码／16实来源引用／差异通过。新1596／1597两F与adapted／CHARACTER两成员、两个用途与局部I-S落实；语义1598有效1591／文档1709（46.44%）／处置733／E852／R7 304余2757。U11／V03／R04仍open，P05原closed及全部完整门禁保持；完整事件／调用父／三依赖／正常alias-owner寿命、人物创建-主地图／显示／链接／明文互通／运行和整体验收未达成。
