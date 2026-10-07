# 人物列表进度文本长度与布局取样子SPEC（BSL01）

2026-10-07。沿连接成功后的角色列表请求／选择流程推进，按AGENTS第8条工程委托先记录裁决。仅改CharacterListProgress.cpp既有UpdateCharacterListProgress_45DD60及移除其不再使用的固定长度常量。

<a id="character-list-progress-length-baseline"></a>
## 原端与当前差异

原45DD60范围[45DD60,45DED9)，单块118条377B／两页，6个直接调用／6目标、无间接、7跳转／2RET；唯一人物选择父460FF0@461041。原入口W16阶段0／1／2，阶段0创建后45DE05重读阶段，阶段1只启动请求并INC W16后绘制、阶段2轮询非零就返回，不把启动和轮询融合为同帧。

| 局部 | 原指令数据流 | 当前源码差异 | 裁决 |
| --- | --- | --- | --- |
| 阶段0长度与列数 | 复制原21B含NUL提示后，45DDA9扫描当前9A5554缓冲；长度W32×8、逻辑右移6、加2，在45DDBC发布阶段1之前算出列数 | 固定长度20，阶段先写1，未扫描当前缓冲 | 从同256B缓冲取一次长度；保持原U32逻辑右移公式与发布位置，不照搬登录进度的×9公式 |
| 创建参数 | 列数样本计算10-columns，发布columns，算centeredWidth，发布rows2，算width和x；发布x／y192，以本地参数调用5044E0 | 用已发布全局列数／x／y再次构造参数 | 保原S32阶段与各字段写序，以本地x／192／width创建，height96及4／1保 |
| 非零轮询清理 | 45DEB8一次取action；非空才MarkDeath并45DECC清槽，空指针直接返回 | 清槽在非空分支外，空时也写null | 移回已有非空分支内；不新增保护、不改变取样和返回 |
| 绘制尾部 | readiness后45DE3C取rows／算高，45DE5D取y／算textY；45DE6D先取columns，再45DE73扫描当前缓冲，算(columns*8-length)*8；45DE87取x／算textX | 固定20长度且先算textX再textY，字段顺序不同 | 明确rows→y→columns→现场长度→x及原算式阶段；两最终S16和DrawText3六参数保 |

当前原文案正常长度仍为20B、含NUL21B，原普通几何值已正确；本步恢复原算法与读取顺序，不声称运行故障。首次与绘制长度都取当前缓冲，columns必须在绘制长度扫描之前取样，不能套用别的进度窗口顺序。

邻接真X:0005DED9-0005DEE0共7B，全部90逐BYTE decode确认为NOP，无入向；前45DED8 RET、后45DEE0真入口。仅登记原对齐用途，不造表内或块内F。

<a id="character-list-progress-length-decision"></a>
## 实施裁决与保持项

单源四精确片段：移除固定kLegacyTextLength、阶段0长度与创建参数、非零轮询清槽位置、绘制长度与字段顺序；只一个既有业务体变更，不新增工具／owner或公共接口。原21B提示memcpy及256B缓冲余部保持；初次U32计算承接原逻辑SHR，布局与绘制用普通S32，不新增故障／回绕模型、扫描预算、补零、严格校验或钳制。

保Reset正文、入口一次W16快照、创建后阶段重读、阶段1请求启动与现场W16++、阶段2轮询／非零早退、同非拥有YobiAction观察者取样／死亡标记、readiness及两S16参数和调用序。角色列表请求／回复、两record owner、公共解析／解转义、dispatcher及人物父正文不改。本步不恢复屏幕键盘／newest／两加解密叶，网络兼容原格式只明文；音频暂停、RPLAN.24与x64既有指针承接、无RTTI／外依赖约束保持。

<a id="character-list-progress-length-acceptance"></a>
## 可度量验收

1. HEAD整源由四唯一from→to构造expected，与现源逐字相等；两次当前缓冲strlen、初次U32×8逻辑移位、绘制先columns后strlen及普通S32阶段、清槽在原非空分支分别对应指令。Reset／声明／其余正文不变。
2. 完整118条377B／两页连续字节、6直接／6目标、7跳转／2RET、唯一父与正常原点静态关联；7B NOP逐字节仅用途分类。
3. 新映射1591／真实F:0005DD60及单adapted，不给未变Reset或私有布局另造身份；CHARACTER6→7，两局部I-S。语义1591→1592／有效1584→1585、文档1702→1703（46.28%）／3680、处置726→727、E842→844；单X使R7 297→298／剩2763。
4. U11／V03／R04只current和引用仍open；P05原closed、closed11／完整叶0／globalfalse、所有完整门禁、冻结定位／无F代码区各7／pending18／差队列全部保持。源码／原证据／完整机器／文档集中静态后运行文本measure与-Check同句柄至终态，再核完整派生快照、12文件集合／UTF-8无BOM-LF／有效引用与差异。

完整460FF0父与六依赖、真实请求／回复／人物创建-主地图、输入图像生产消费、全部正常alias-owner与寿命、CRT-SDK／机器ABI及链接、原格式明文往返／显示／运行R8仍待。函数身份纳管及本步局部I-S不代完整人物或显示链验收；不构建、测试、运行或计算文件摘要限制保持。

<a id="character-list-progress-length-source-state"></a>
## 实施追踪

预定固定命名快照：cg2d-re-character-list-progress-length-sampling-20261007。对应[原证据](cg2d_character_list_progress_length_sampling_evidence.json)、[人物场景](cg2d_character_select_scene.md)、[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。集中静态与派生终态通过后追加实际结果，不预记运行或整体验收。

实际结果（2026-10-07）：单源四片段／一既有业务体已实施，原118条377B／两页和7B真实NOP证据落地。源码／原证据／完整机器／文档集中静态、measure与-Check同句柄终态0、完整派生快照与12文件编码／引用／差异通过。新1591／真实F:0005DD60与单adapted、CHARACTER一成员／7B原用途和两局部I-S已落实；语义1592有效1585／文档1703（46.28%）／处置727／E844／R7 298余2763。U11／V03／R04仍open，P05原closed及全完整门禁保；真实人物链、明文往返／显示、链接／运行及整体验收未达成。
