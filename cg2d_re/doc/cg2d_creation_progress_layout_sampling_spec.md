# 创建请求进度布局取样子SPEC（BSR01）

2026-10-07。沿人物创建controller真实45E7D0推进，按AGENTS第8条工程委托先裁决再实施；仅调整进度框三个阶段取样，不改请求、controller或创建业务。

<a id="creation-progress-layout-baseline"></a>
## 原端基线与实际缺口

45E7D0完整[45E7D0,45E95F)，123条399B／两页、6直接／6目标、10跳转／两RET，无间接；唯一创建controller4626A0@4627DF。六callee分别5044E0／508E00／508E90／404D00／401D70／456560。

45E83E..45E854先算(10-columns)*64、width=columns*64和x/2，再45E85D／45E867／45E86C发布rows2／x／y192，45E876创建借本地宽度。当前width直到创建实参才计算，在共享布局发布之后；恢复原几何计算阶段和本地借值。

绘制前45E8F9取rows，45E905..45E913算48*rows-16，45E915再取y，45E91B／45E91D完成有符号/2和相加。当前rows后先读top，再算高度；恢复计算在y取样之前。45E929第二次现场strlen后，45E92D先读columns，然后45E932／45E933得到len和逻辑SHR1文字units、45E937..45E942算横向未居中宽度，再45E944取x并/2相加；当前先算units再取columns、先取left再算横向几何。按原取样／计算分阶段恢复，最终两S16保持。

原55C9C0正文10B+NUL共11B已逐字节取证；12B栈分配只初始化11B，未使用的第12字节不推为文本逻辑容量。现char text[sizeof原11B]逐字节复制11B、两次现场strlen正确，保持，不增256B缓冲／owner／零填充。

原9A50E0、9A565C、9A566C、9A4E90、9A50D8均S32，9A6E88是非拥有观察槽；不套列表／删除的W16阶段。原45E811先phase1发布再第一次strlen，(len+7)*9用U32／逻辑SHR6、初次columns发布及<2再发布真实门槛保持。create后现场phase、启动508E00后现场++、轮询508E90状态1／负数与非空死亡标记／清槽当前正确，保持。

特别保45E8F0的401D70 GE1与既有IsYobiActionReady(action,1)，这里就是原入口，不套BSQ01的LTE0。普通值和正常分支原正确，本步修时序而不称运行故障。

X:0005E95F-0005E960单字节90，真实NOP decode、无入向、前RET后45E960真入口，仅对齐用途，不造布局／Reset／局部块F。

<a id="creation-progress-layout-decision"></a>
## 实施裁决与保持项

单源三精确片段、一既有业务体：先创建unscaledX／width／x再按原写rows／x／y、创建借本地width；绘制先rows／unscaledY再y样本和textY；第二strlen得U32长度后取columns，再逻辑SHR1文字units／普通S32横向几何，最后x样本／textX。普通S32四则沿RPLAN.24，不模拟或专项处理异常／溢出／回绕，不新增保护／限制／钳制／补零。

保原11B复制／动态长度、U32长度算法和真实<2门槛、五离散S32及同native非拥有观察槽、阶段发布／创建后重读／启动后的++和原请求结果分类／非空标死清槽、原GE1入口、六参数DrawText3与两S16、Reset45E7B0、整个其它源码／所有头／controller／请求回复／六callee。不新增业务／owner／全局／外依赖／RTTI。网络原格式仅明文，音频暂停及既定排除／执行限制保持。

<a id="creation-progress-layout-acceptance"></a>
## 可度量验收

1. HEAD整源以三唯一from→to构expected逐字相等，三个阶段计算／取样顺序对应原点，原阶段／请求／GE1门禁／返回／文本复制及所有其它正文保持。
2. 123条399B／两页连续字节、6直接／6目标／10跳转／两RET／唯一真实caller，原11B文本与单字节对齐用途归档。
3. 新1599对应F:0005E7D0／单adapted，CHARACTER14→15；语义1599→1600／有效1592→1593、文档1710→1711／3680（46.49%）、处置734→735、E854→856，R7 305→306／剩2755；两局部I-S各18实际来源。
4. U11／V03／R04只current／引用仍open，P05原closed／closed11／完整叶0／globalfalse及全部完整门禁、冻结定位／pending18／差队列保。四集中静态后measure与-Check同句柄到终态，再核完整派生快照及12文件UTF-8无BOM-LF／四锚点／真实来源／差异。

完整创建controller／六依赖、请求回复／图像文字生产消费／全部正常alias-owner寿命、CRT-SDK／机器ABI／链接、人物创建-主地图／明文互通／画面／运行R8未验；局部不代整体验收，不构建／测试／运行或计算摘要。

<a id="creation-progress-layout-source-state"></a>
## 实施追踪

预定Git命名快照：cg2d-re-creation-progress-layout-sampling-20261007。对应[原证据](cg2d_creation_progress_layout_sampling_evidence.json)、[人物场景](cg2d_character_select_scene.md)、[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。集中静态与派生终态后追加实际结果，不预记整体门禁。

实际结果（2026-10-07）：单源三片段／一创建请求进度体已实施，创建几何先算后发布、显示rows／长度与列／坐标分阶段取样落实；原GE1入口／S32阶段／请求／返回、11B文案／两现场长度及整个其它源码保持。123条399B／两页、六直接／六目标／唯一创建caller和单字节用途证据落地；补充六原存储地址19实际直接引用，均在Reset45E7B0／本进度体，不外推未知间接别名或全部owner寿命。四集中静态、measure与-Check同句柄终态0、完整派生快照、12文件编码／四锚点／两局部I-S各18来源引用与差异通过。新1599单F／adapted／CHARACTER单成员、单用途与两局部I-S落实；语义1600有效1593／文档1711（46.49%）／735处置／E856／R7 306余2755。U11／V03／R04仍open，P05原closed和全部完整门禁保持；完整创建controller／六依赖／全部正常alias-owner寿命与生产消费、人物创建-主地图／链接／明文互通／画面／运行和整体验收未达成。
