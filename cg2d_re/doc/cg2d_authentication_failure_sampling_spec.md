# 认证失败提示取样子SPEC（BSN01）

2026-10-07。沿人物选择失败提示分支推进，按AGENTS第8条工程委托先记裁决。仅改CharacterAuthenticationFailure.cpp既有UpdateAuthenticationFailure_45D7E0及该文件提示注释；原事实来自已打开cg2d.exe只读指令和字节。

<a id="authentication-failure-sampling-baseline"></a>
## 原端基线与当前差异

原45D7E0范围[45D7E0,45D97D)，单块113条413B／两页，7直接调用／7目标、无间接，6跳转／2RET；唯一人物选择父460FF0@46140C。提示已核为55C5E8，14B正文＋NUL共15B：b56ea44abb7bc3d2a5a2b1d1a14300。移除原未证CP950名称与“地址未交接”陈旧注释，保字节，不命名未证来源编码。

| 局部 | 原指令数据流 | 当前源码差异 | 裁决 |
| --- | --- | --- | --- |
| 初次创建 | 原常量64／168／512／144／4／1准备，之后发布stage／unknown／width／height／x／y，5044E0使用原常量参数 | 发布后从四布局字段重新取创建参数 | 直接传原常量，所有既有字段发布和调用位置保 |
| 命中矩形 | 45D876 width、45D87C x；算width-66后45D887读y一次，再SAR1＋x得到left；先bottom=y+132、top=y+115，再right=left+66 | 跨表达式取字段，y在两实参中重复读取 | width→x→减66→y→left→bottom→top→right，保五参数和同命中调用 |
| 点击清理 | 先PlaySe51，45D8CE单读action；非空才MarkDeath／清槽，空也返回result1 | if与MarkDeath实参重复取action | 声音后一次非拥有指针样本，复用判断与死亡标记 |
| 文字 | readiness后45D913 y、45D918 x，先y+20再x+140；两最终S16 | 参数表达式中读取x／y | 明确y→x→textY→textX，颜色6／层2和原文字保持 |
| 图像 | 文字调用后45D939 width、45D93F y，选择图号，45D959 x，再45D95F SAR1、y+123、halfWidth+x | x／width／y在实参中读取，width用除2 | 现场width→y→图号→x，保原S32算术右移1与后续计算；不复用先前按钮或文字样本 |

当前原初始值下命中和显示数值已正确，本步恢复原读取／发布合同，不声称运行故障。SAR1属于原算式，不引入回绕或异常模拟。图像各字段在文字提交之后重新取，不能为了减少读数合并跨调用样本。

原16B graphicDescriptor初始化已对照：前8B零、8..11为FF、12为0／13为4／14..15为0，现同16B值类型保持。原0xDF低BYTE、243048／243050图号、0参数及调用期descriptor借用不改。

邻接X:0005D97D-0005D980共3B全90，逐BYTE decode确认NOP，无入向；前45D97C RET，后45D980真入口。只登记原对齐用途，不造未变Reset、常量、descriptor或内部块F。

<a id="authentication-failure-sampling-decision"></a>
## 实施裁决与保持项

单源六精确片段：纠正文案注释／地址、初次常量参数、命中矩形取样、声音后action单取样、文字阶段、图像阶段与SAR。只一个既有业务体变更，无新工具／owner／接口、外依赖或RTTI。保所有原数据宽度：W16 stage、S32布局／result、低W8 mouse与graphic layer、两个文字S16及真非拥有指针，非地址不拓宽。

保Reset正文、原首次六字段发布、-2／512／144／64／168值、同16B descriptor、命中和hover／低W8 bit1、声音51、结果1早退／非空才死亡清槽、后续独立action取样／readiness与两绘制、原15B正文和全部其它源码。人物父／共享UI命中／Yobi／渲染和音频正文不改，不将局部参数取样记为callee验收。

既定屏幕键盘／newest／两加解密叶排除、网络原格式明文、音频暂停、x64已证指针承接与RPLAN.24保持。不新异常门禁／保护／补零／钳制或输入限制，不专项验证异常。

<a id="authentication-failure-sampling-acceptance"></a>
## 可度量验收

1. 六from→to各唯一命中HEAD整源，构造expected与现源逐字相等；原常量创建、矩形单y样本／顺序、声音后一次action、文字与图像独立字段阶段及SAR分别对应原点。Reset／发布／descriptor／字段类型／字节与其余正文保。
2. 原113条413B／两页连续字节、7直接／7目标、6跳转／2RET、唯一父和原文案15B／descriptor字段指令静态关联；3B NOP仅原用途。
3. 新映射1593／真实F:0005D7E0与单adapted，CHARACTER8→9，两局部I-S；语义1593→1594／有效1586→1587、文档1704→1705（46.33%）／3680、处置728→729、E846→848；R7 299→300／剩2761。
4. U11／V03／R04只current和引用仍open；P05原closed、closed11／完整叶0／globalfalse、全完整门禁、冻结定位／无F代码区各7／pending18／差队列保持。四集中静态后measure与-Check同句柄至终态，再核完整派生快照／12文件、UTF-8无BOM-LF／有效引用和差异。

完整460FF0父与七依赖、原事件／输入／渲染消费、正常alias-owner寿命、CRT-SDK／机器ABI、链接、人物创建-主地图／明文互通／实际画面与运行R8仍待。原根取证和本步局部静态不代完整提示或人物验收；不构建／测试／运行或计算文件摘要限制保持。

<a id="authentication-failure-sampling-source-state"></a>
## 实施追踪

预定固定命名快照：cg2d-re-authentication-failure-sampling-20261007。对应[原证据](cg2d_authentication_failure_sampling_evidence.json)、[人物场景](cg2d_character_select_scene.md)、[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。集中静态和派生终态后追加实际结果，不预记整体验收。

实际结果（2026-10-07）：单源六片段／一既有业务体已实施；命中单y／声音后单action／文字图像独立现场取样与原SAR恢复，提示地址和原15B字节已核，撤未证编码名。原113条413B／两页及3B对齐证据落地；四集中静态、measure与-Check同句柄终态0、完整派生快照和12文件编码／引用／差异通过。新1593／F:0005D7E0与单adapted、CHARACTER一成员、单3B用途和两局部I-S落实；语义1594有效1587／文档1705（46.33%）／处置729／E848／R7 300余2761。U11／V03／R04仍open，P05原closed及全部完整门禁保；完整人物／事件显示链、明文互通、链接／运行和整体验收未达成。
