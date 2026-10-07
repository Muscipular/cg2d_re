# 人物删除进度动态几何子SPEC（BSM01）

2026-10-07。沿人物选择删除分支推进，按AGENTS第8条工程委托先记录裁决。仅修改CharacterDeletionProgress.cpp既有UpdateCharacterDeletionProgress_45DC00，恢复原文本长度和布局消费算法，不调整删除请求或人物父业务。

<a id="character-deletion-progress-geometry-baseline"></a>
## 原端基线与当前差异

原45DC00范围[45DC00,45DD45)，单块102条325B／两页，6个直接调用／6目标，无间接，7跳转／2RET；唯一人物选择父460FF0@46132A。原55ABE8只读提示正文10B／含NUL11B，已逐字节取证为a8a4a6e2a752b0a3a4a400。两次扫描都读取该静态文本，不新增或借用别的进度窗口256B缓冲。

| 局部 | 原指令数据流 | 当前源码差异 | 裁决 |
| --- | --- | --- | --- |
| 阶段0长度 | 45DC1B扫描提示，45DC29长度W32×9，45DC2C发布stage1，之后45DC35逻辑右移6／加2 | 固定10长度，发布stage1后直接以常量计算 | 当前只读文本取长度，以U32×9后发布stage，再右移／加2；不套用列表进度×8和其发布位置 |
| 窗口创建 | rows2；10-columns计算后发布columns；centeredWidth；y192；width与x；发布x，5044E0借寄存器参数 | 使用已发布字段再计算及传参，height由rows现场读取计算 | 普通S32按原阶段，以本地x／192／width和原常量96／4／1创建 |
| 非零轮询 | 45DD24一次取action，非空才MarkDeath并45DD38清槽；空时直接返回 | 清槽在非空分支外 | 移回既有非空分支，保取样和返回，不新增保护 |
| 绘制尾部 | ready后45DCB1取rows、45DCB6取y，再算textY；45DCE2扫描文本，45DCE4才取columns，算(columns*8-length)*8；45DCF7取x，算textX | 固定kTextX=280／kTextY=232，未消费布局或现场长度 | 恢复rows→y→计算textY→扫描长度→columns→计算宽度→x→textX，最后仅两S16参数窄化 |

原普通10B文案和初始几何仍得到已有显示值；本步撤掉固定结果、恢复真实算法，不声称运行故障。删除进度绘制先扫描文本再取columns，与列表进度先取columns再扫描不同，严格保各自原序。

邻接真X:0005DD45-0005DD50共11B全90，逐BYTE decode确认NOP、无入向；前45DD44 RET、后45DD50真入口。仅登记原对齐用途，不造Reset、私有布局或块内F。

<a id="character-deletion-progress-geometry-decision"></a>
## 实施裁决与保持项

单源六精确片段：声明标准cstring以使用strlen，移除固定长度常量、移除两固定坐标、阶段0长度与本地创建参数、非空清槽、绘制算法。只一个既有业务体变更，不新增工具、owner、外部依赖或公共接口。标准头不是外部依赖。

保原11B静态文本／独立地址身份和生命周期，两个长度均读取同原只读文本，不造可变缓冲或影子状态。初次U32承接原逻辑SHR，其余普通S32计算及最终两S16绘制边界保持；不引入异常模拟、预算／限制／钳制／补零或故障门禁。

保Reset正文、入口一次W16阶段、创建后阶段重读、阶段1启动删除请求后现场W16++／仅绘制、阶段2轮询非零早退、一次非拥有action取样与死亡标记、readiness／DrawText3六参数和结果。删除请求／回复、两角色record owner、dispatcher及人物父正文保持，既定排除与网络原格式明文／音频暂停、x64真指针承接／无RTTI、RPLAN.24和执行限制保持。

<a id="character-deletion-progress-geometry-acceptance"></a>
## 可度量验收

1. 六from→to各唯一命中HEAD整源，构造expected与当前源逐字相等；两只读文本strlen、初次U32×9和stage发布位置、原S32创建／绘制阶段、非空才清槽分别对应原指令。Reset、字节／容量、阶段和其余正文保持。
2. 原102条325B／两页连续字节、6直接／6目标、7跳转／2RET、唯一父、两扫描与原字段阶段静态关联；11B NOP逐字节仅用途分类。
3. 新映射1592／真实F:0005DC00和单adapted，CHARACTER7→8，两局部I-S；语义1592→1593／有效1585→1586、文档1703→1704（46.30%）／3680、处置727→728、E844→846；R7 298→299／剩2762。
4. U11／V03／R04只current和引用仍open；P05原closed、closed11／完整叶0／globalfalse、所有完整门禁、冻结定位／无F代码区各7／pending18／差队列保。源码／原证据／完整机器／文档集中静态，再measure与-Check同句柄至终态，核完整派生快照及12文件／UTF-8无BOM-LF／引用和差异。

完整460FF0父与六依赖、真实删除请求回复／人物创建-主地图、输入图像生产消费、正常alias-owner寿命、CRT-SDK／机器ABI及链接、原格式明文往返／显示／运行R8仍待。局部源实修和原根取证不代完整人物或显示验收，不构建、测试、运行或计算文件摘要限制继续。

<a id="character-deletion-progress-geometry-source-state"></a>
## 实施追踪

预定固定命名快照：cg2d-re-character-deletion-progress-geometry-20261007。对应[原证据](cg2d_character_deletion_progress_geometry_evidence.json)、[人物场景](cg2d_character_select_scene.md)、[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。集中静态及派生终态通过后追加实际结果，不预记整体验收。

实际结果（2026-10-07）：单源六片段／一既有业务体已实施，固定坐标和长度已撤，恢复两原只读扫描、原阶段／S32动态几何及非空清槽；原11B字节／生命周期保持。原102条325B／两页及11B对齐证据落地；四集中静态、measure与-Check同句柄终态0、完整派生快照与12文件编码／引用／差异通过。新1592／F:0005DC00与单adapted、CHARACTER一成员、单11B用途和两局部I-S落实；语义1593有效1586／文档1704（46.30%）／处置728／E846／R7 299余2762。U11／V03／R04仍open，P05原closed及全部完整门禁保；链接、真实人物链、明文往返／显示、运行与整体验收未达成。
