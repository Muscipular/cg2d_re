# cg2d 输入上下文切换与初始化还原

## 函数边界

- `Input::SetContext@[0x44B330,0x44B33A)`，3 条指令、1 个基本块，原型为 `void(InputContext*)`。函数只把参数写入原 `gCurrentInputData@0x626C04`，允许写入空指针。
- `sub_449CF0@[0x449CF0,0x449D92)`，41 条指令、3 个基本块，原型为 `void(InputContext*, int, int, int, int, int, const char*, int, int, int, int)`。
- `sub_44A160` 是初始化器使用的私有文本装载辅助函数，不公开为新端接口。

## 固定字段写入

`sub_449CF0` 按以下顺序写入，未对顺序或目标字段作归并：

1. `field130@0x130`
2. `renderMode@0x138`，完整写入 32 位
3. `color@0x140`
4. `field134@0x134`
5. `maxCharPerLine@0x148`
6. `fontType@0x13C`
7. `encrypt@0x150`
8. `maxLine@0x144`
9. `field14C@0x14C`
10. `text[0]@0x000 = NUL`
11. `totalByteLength@0x12C = 0`
12. `caretByteOffset@0x128 = 0`
13. `field154@0x154 = 0`
14. `field158@0x158 = 0`

`field138` 按 `std::int32_t` 保存完整 32 位渲染模式，`InputContext` 以同类型引用关联自身存储；初始化直接赋值，绘字边界才显式取低八位，不通过不兼容指针别名访问。固定字段初始化阶段不清除 `text[1..287]`，也不写 `x@0x120`、`y@0x124` 或其他字节；非空初始文本造成的后续文本与计数变化只来自下述追加路径。

### 2026-10-16：field138 类型核对（ICT01）

ICT01 按用户要求确认类型，只读核对当前 cg2d.exe 会话及源码。`+0x138` 承载完整 32 位渲染模式；取证时的四字节数组可以保留位模式，但建议源码将 `InputFields::field138` 改为 `std::int32_t`，`InputContext::field138` 对应改为 `std::int32_t&`。这属于源码类型选择，不声称恢复了原始 C 声明；ICT01 阶段仅记录取证结论，后续实施见 ICT02。

| 来源／位置 | 已读取事实 | 类型判断 |
| --- | --- | --- |
| ICT01 取证时源码 `Input.hpp` | 存储为 `std::uint8_t[4]`，公开成员为该数组的引用 | 原始字节承载，不表达整数含义 |
| IDA 已有 `InputContext` 类型 | `renderMode@0x138` 为一字节 `char`，`0x139..0x13B` 另列三个 `char`；声明来源未知 | 与实际整字读写冲突，不能据此认定字段只有一字节 |
| `0x449D07`／`0x45D74E`／`0x45D78A` | 分别将 ECX／EDX／EDX 完整写入对象 `+0x138` | 三个入口均写 32 位，不是仅写低字节 |
| `0x456BED`／`0x456C0B`／`0x456C2F` | 独立读取 `+0x138` 到 ECX／EDX／EDX，并用于队列或字体层索引 | 必须保留完整 32 位及各次读取位置 |
| `0x4591F7`／`0x4591FD` | 先以 EBX 读取完整字段判断零／非零，再以 DL 独立读取低字节；`0x45921C` 执行 `inc dl` | 整数字段存在低字节消费者，不是四个独立业务字段的证据 |
| `0x456A00` | 读取 `+0x138` 到 CL 后用于绘字参数 | 仅该消费者取低八位，不决定字段存储宽度 |

上述 MOV、零值判断和索引指令不能独立证明有符号性。建议的 `std::int32_t` 与三个写入入口的 `renderMode` 参数及取证时 `ReadRenderMode` 的完整 S32 读取一致。迁移时可用普通赋值／直接读取替代相关 `memcpy`，绘制边界继续显式取低 S8／U8；低字节加一先转 U8 再加 `1U`，不把完整 S32 加一引入该路径。完整值零判断与三次队列取样仍各自保持，不共用入口快照；ICT01 未修改 IDA 类型，未构建、测试、运行或生成 JSON 台账。

### 2026-10-16：field138 定型实施 SPEC（ICT02）

用户已明确要求执行 ICT01 的建议。限定 `Input.hpp`、`Input.cpp`、`InputCaret.cpp`、`render/InputText.cpp`、`world/RenderInput.cpp` 五份源／头：存储改为 `std::int32_t`，公开成员改为其引用，保留字段名和原偏移说明，并注明渲染模式含义。四处整字段复制／写入直接赋值；删除两个原始字节读取包装，三个队列索引各在原位置直接读取完整字段，组合串完整零判断与低 U8 模式仍独立取样。

绘字和文本记录边界分别显式转 S8／U8；两处光标模式先取低 U8，再加 `1U`，最后转 S8，避免新增完整 S32 加一。只清理因此失效的 `cstring` 包含及空匿名命名空间，不改其他字段、调用／读写顺序、输入文本、队列或 IDA 类型。沿 ICT01 的接口一致性选择 S32，不把该选择改写为已证原始有符号性。源码直接 apply_patch，仅做限定差异、类型／取样及 UTF-8 无 BOM／LF 静态检查，进度同步后提交；不构建、测试、运行、计算摘要或生成／更新 JSON 台账。

ICT02 完成：五份源／头改为 S32 存储／引用，四处整字段复制／写入直接赋值，两个字节读取包装和一份失效 `cstring` 包含及空匿名命名空间删除。三次队列完整读取与一次组合串完整读取、五处低字节转换保持原源码取样位置，两处光标保先低 U8 再加一。构造引用绑定、零初始化／reset 及其他字段和业务差异静态核对通过；本说明同步当前表示，ICT01 证据保留。未编译、测试、运行或生成／更新 JSON 台账。

## 初始文本路径

- `initialText == nullptr` 时，初始化器在固定字段写入后直接结束，不进入文本装载路径，当前上下文指针保持原值。
- `initialText != nullptr` 时，初始化器先保存当前上下文，再通过 `SetContext_44B330` 切换到目标上下文，调用私有 `sub_44A160`，最后通过同一函数恢复保存值；保存值允许为空指针。
- `sub_44A160` 先检查当前上下文；为空时直接返回，非空才以 `strlen` 取得初始文本长度。长度大于等于 `0x120` 时截为 `0x11F`。随后逐字节调用现有 `AppendChar_44ADB0`，不直接复制文本，因此追加函数的过滤、行限制及伴随状态写入仍然生效。

本项不建立账号、密码或角色名上下文实例，不建立焦点表，不实现 `sub_45C7C0`、`sub_45CA20`、输入渲染或 IME。

## 完整回车提交44AB60实施 SPEC

2026-09-28，原会话只读核[44AB60,44ADAF)全部指令和四个原字节提示，
沿既有ProcessKeyInput_4596D0的detail::Sub_44AB60调用落实完整入口。
按工程委托，定义放在既有Input.cpp后部，直接复用同文件已有的
world::detail::gChatHistory621FF8及gCursor62683C；不搬owner、不新建
历史表或为本函数公开其结构。玩家名用既有BorrowPlayerDisplayText(1)
借用，输入上下文均为原生T*身份，不把原地址常量用作数值句柄。

### 普通聊天与历史

IME组合首字节非零立即退出，不写输入相位。IME为空才读取当前context。
context严格为626AA0时，检查+12C totalByteLength严格0立即退出；
不误用反编译的cursor名称，也不改为检查光标或text[0]。
读取历史尾槽完整W32，另保留其低S8符号扩展为旧比较索引；
完整值按S32加1、先写尾槽，结果>=64才写0，不额外处理负值。
原44ABB3为movsx ebp,al，旧比较索引不是完整尾槽或无符号U8。

以原字节strcmp比较固定输入与旧索引历史：
相同则把旧S8符号扩展值写回完整尾槽，不持久化且不重读context；
不同则把固定输入含NUL复制到新的历史槽，再取共享尾槽作为S32调用
449FA0，忽略写文件结果并重读gCurrentInputData。
不先验证历史文件尾槽、不截短复制、不以写文件失败阻断发送。
随后先读取626AA0.field140颜色，再把历史游标62683C=-1，
调用完整50C940(context,color)。无论发送是否被限流或失败，
逐次重读当前context并按totalByteLength、caretByteOffset、text[0]
顺序清零，再重读context进入公共后半段。

### 其他输入与黑名单

context分别等于D66CA8/CB03F0/CC46E0/CC2080/D2D828/CA0180之一，
则AppendChar(CR)，之后重读context；其他context不追加字符。
非上述对象且为空的context仍能进入公共相位写入，不新增空指针早退。

后半段context为C8B1B8才处理黑名单；原对静态对象地址再次非零判断
恒成立，不替换成检查文本是否为空。
先strcmp输入与同源玩家名，相同用原554938提示；
否则ContainsBlacklistRecord_4013D0非零用554928重复提示；
未命中才AddBlacklistRecord_401270，低S8非零选5532C4成功格式，
为0选5532B0失败格式。保留sprintf到未初始化256B局部区，
格式中的%s使用C8B1B8原文本，不转码、不复制名称或补长度门禁。
四个原字节格式（正文长度，不含NUL）为：

- 554938，26B：`B1 7A A1 4B A6 70 A6 B9 B9 BD B4 63 A6 DB A4 76 AA BA C1 6E AD B5 B6 DC A1 48`。
- 554928，14B：`25 73 A4 77 A6 62 B6 C2 A6 57 B3 E6 A4 A4`。
- 5532C4，16B：`A4 77 B1 4E 25 73 A5 5B A4 4A B6 C2 A6 57 B3 E6`。
- 5532B0，16B：`25 73 A5 5B A4 4A B6 C2 A6 57 B3 E6 A5 A2 B1 D1`。

生成提示后按text[0]、totalByteLength、caretByteOffset顺序清黑名单
输入；44AD7D实际地址C8B2E4(+12C)、44AD83为C8B2E0(+128)，
不信其反编译字段名。随后AppendMsg(buffer,4,3)、SetContext(626AA0)。
所有未早退路径最后将626C0C写20，不添加本函数没有的音效。

仅静态核对字段、索引解释、完整分支、原字节、持久化/发包与清空次序，
不构建、测试、运行或检查产物。接通已有回车调用不等于初始化/全部
编辑叶、窗口原型或完整聊天收发已验收；50CAE0回复链仍须继续。

实施状态：44AB60已在Input.cpp完整落位，现有InputKeyDispatch回车
调用解析到该本体，并接已实现50C940、449FA0及黑名单查询/插入。
同文件直接复用历史区，未新增公开存储或改变InputContext的15C布局。
四原字节提示、低S8/完整S32索引、长度字段及上下文重读/清空次序、
既有源登记与接口仅静态核对，未构建、测试或运行。

## 初始装载与字节分类数量宽度 SPEC（2026-10-02）

在追加本体修复后，沿其真实装载/分类依赖完整核五个入口：44A160共31条、
44B1D0共44条、44B190共21条、449CF0共41条、44B340共79条，合计216条。
按工程委托保留既有函数及调用，只收回两处被宿主size_t扩宽的原W32数量：

- 44A160在44A17E用signed比较长度与288，44A18D/44A19E用signed控制循环。
  strlen结果在边界转S32，length/index均保持S32；入口context为空直接返回，
  只取一次初始长度，>=288时取287，逐字节重新读取initialText并调用AppendChar。
  不提前复制源文本，也不按追加是否成功改变循环次数。
- 44B1D0的Count为W32；strncpy数量只在库调用边界按U32传递，不能把负S32
  直接转为x64 size_t。终止位置保持原S32偏移，扫描数量按S32比较。
  IED01已撤销旧288B局部预清，原复制填充/终止保持；负值、越界不专项模拟，不增加门禁或声称故障等价。

44B190完整高位双字节/孤立高位尾的2/3分类不变，不替换为系统代码页或UTF-8。
449CF0字段写序、仅text[0]清零及临时切换/恢复当前对象保持；不清整块上下文。
44B340仍优先Shift、其次Ctrl，依次读取F1至F8，最后一个命中决定0/8/16组索引，
先装载再调用原音效入口。原调用传51/320/240；现有音频窄接口消费前两项，
不在本步改造该暂停板块或把声音播放记为已验收。

44A160有七处原调用（历史浏览两处）；44B1D0有七处编辑调用，44B190有五处
输入/文本调用；44B340唯一caller473F80已接。源码关系仅登记真实现有消费者，
不以这五项齐备宣称整个输入板块、所有UI父链或全客户端完成。
只核本次数量修改、既有流程与映射，不构建、测试、运行或哈希校验。

## 2026-10-05：分类与编辑局部存储当前状态

[IED01](cg2d_input_edit_alignment_spec.md#input-edit-source-closure)已撤销44B1D0额外整块预清：真实StrNCpy_53BEC0低U32 Count、signed终止位置和S32扫描/0～3返回保持，不删原padding/NUL或加Count范围拒绝。七原点绑定与六编辑本体局部已对照，上下扫描预算/终行重新取长、退格钳制后对象重读按原序。此前日期保留预清是旧源码状态，固定历史见Git命名快照cg2d-re-input-render-source-20261005；完整输入/别名/SDK/运行仍未验。

## 2026-10-05：追加前置状态差异当前对齐

[IAP01](cg2d_input_append_alignment_spec.md#input-append-source-closure)撤销44ADB0非原有HasSafeLegacyState与两临时副本预清，原context为空/控制字节早退及当前对象逐阶段重取保持。三复制289/288/288、实际补零/NUL、低U32拼接余量和原截断/光标钳制/回滚不改；00仍按原接受，不能用可见串长/计数一致性补门禁。超行maxLine/626C20一次共同取样已正确；203条/实际直接引用仅局部静态，完整输入/别名/生命周期和运行未验。

## BPH01 预设键索引宽度与真实文本消费（2026-10-06）

[BPH01 SPEC](cg2d_key_index_preset_source_spec.md#key-index-preset-decision)/[原证据](cg2d_key_index_preset_source_evidence.json)。44B340只将0x70+index直接交完整S32查询，原Shift优先/否则Ctrl、F1..F8全扫/最后非零覆盖、未选返回最后查询保持。选中仍借同一24项15CH上下文文本，先44A160装载再52C450并透传；现有现代473F80唯一调用保持，不能当旧99路由已补。

原音效caller压51/320/240，164条551B支持体仅入口参数+4/+8读取，额外第三值未消费，现有两参调用保持。支持只参数消费，独立于三根87条分母、不纳音频验收或恢复播放。旧373全文映射归档后续记，正常旧输入结果不变。语义1422→1424条/有效ID1417→1419、关联1535→1537/3680（41.71%→41.77%）、分类543→546，INPUT14→17/TEXT22→23枚举未闭；E707→715仅8条正常局部I/S。60任务源码closed10/完整验收0、叶验收0/global_accepted=false及全部完整门禁保持。 所有其它输入正文/焦点/生命周期、SDK/ABI与实际输入画面运行未验。

## 旧分隔符扫描单BYTE取样更新（BSX01，2026-10-07）

按[BSX01裁决](cg2d_legacy_separator_byte_sampling_spec.md#legacy-separator-byte-decision)，45E770首BYTE一次取current、同样本供NUL和分类，末尾cursor[1]先取再++cursor。撤for门禁／循环体重复读取，不改0x81 unsigned两字节状态／尾字节跳过、非尾7C或5C返回完整1、空串／末lead遇NUL完整0，const文本不写且不建owner／新规则。

原25条57B／一页、无call／八jump／两RET，十实际CALL在七父；新增1602单真F、INPUT26→27及CHARACTER17→18真实消费绑定，两域各局部I-S。7B相邻NOP只用途。其它整个Input、所有消费者及接口保持；原姓名20 00／81 40 00字节正确保，不据编码猜测合并字节规则。全部原父／输入文本生产消费／正常alias-owner寿命与运行未验。

<a id="text-unescape-current-20261010"></a>
## 2026-10-10 UTE01：文本反转义原位取样

[UTE01完整原根](cg2d_text_unescape_sampling_spec.md#text-unescape-source-state)/[原码与映射](cg2d_text_unescape_sampling_spec.md#evidence-archive-20261011)覆盖52D120/52D150/52D2F0/509080，共66条147B/四页、三E8/三个内部目标/七跳五RET；316入向内部3/外313只证明来源。原58E944四对BYTE映射与未知字母低BYTE返回保持，恢复S32映射计数、每轮首BYTE持有及后继读取先于索引递增/共尾写。原高bit两BYTE、反斜杠字母、固定入口长度和两S32索引、末NUL及同text返回保；不切换编码或增加空值/容量/末字节检查。包装弃返→真实单RET空操作→同text返回，两正确body保，不新增通知/成功值/API或owner。四新语义/四F、INPUT/UIRUNTIME各四成员/NETWORK三个文本根、三限定S零I/九任务open；全部外调用、文本生产消费/容量与alias-owner寿命、SDK/机器ABI/真实字符消息画面运行待。

<a id="text-escape-current-20261010"></a>
## 2026-10-10 ETE01：文本转义生产端取样

[ETE01完整原根](cg2d_text_escape_sampling_spec.md#text-escape-source-state)/[原码与映射](cg2d_text_escape_sampling_spec.md#evidence-archive-20261011)覆盖52D1B0转义生产/52D2D0包装，共108条310B/三页、两E8/两目标/11跳两RET；112入向内部1/外111仅来源。恢复非地址S32表计数、已读首BYTE持有/后继与索引及计数更新序、匹配字母早退前读取/两写后更新索引三组。原四对BYTE映射、固定入口长度和两S32索引、原容量/调整长度门槛与截断、末NUL/同destination返回保持，不切换编码或加保护。正确包装和InputContext文本便利适配保持，不新增原根/通知/成功值或owner；UTE01四根和所有其它源保。两个新语义/F、INPUT/UIRUNTIME/NETWORK各两成员、三限定S零I/九任务open；全外调用、文本生产消费/容量与alias-owner寿命、SDK/机器ABI/真实截断和字符消息画面运行待。

<a id="chat-history-current-20261010"></a>
## 2026-10-10 HBN01：聊天历史与输入字节滚动

[HBN01完整原根](cg2d_chat_history_navigation_spec.md#chat-history-source-state)/[原码与接线](cg2d_chat_history_navigation_spec.md#evidence-archive-20261011)覆盖44A1B0历史浏览、44B440字节类别和4A80E0宽度，共225条707B/五页、七E8/四目标/34跳八RET；26入向内部4/外22仅来源。原64项289B/尾W32与独立游标S32、Up/Down门槛差异及游标发布/环/恢复、入口context清length→新context清text/caret→当前游标装入→新游标复制未清256B→字节长度/宽度→S16类别→原负滚动归零→新context写field158保持。类别仍按原固定BYTE范围返低BYTE0/1/2，不按GBK重写；宽度同一W32两倍、旧707/708文件与BTG-CTS提交保持。三个正确body不改，仅六注释/零逻辑组，三新语义/F、INPUT/UIRUNTIME各三成员/MAP一个成员、三限定S零I/九任务open；struct/断言文本不代编译布局验收，全输入/文件/宽度owner、SDK/机器ABI/真实上下翻页和字符消息画面运行待。

<a id="chat-geometry-current-20261010"></a>
## 2026-10-10 CWP01：聊天尺寸生产与设置取样

[CWP01完整原根](cg2d_chat_window_geometry_production_spec.md#chat-geometry-source-state)/[原码与接线](cg2d_chat_window_geometry_production_spec.md#evidence-archive-20261011)覆盖4C9240默认、4C9170设置恢复和4A7C40聊天尺寸/几何，共329条1484B/七页、两E8/两目标/11跳三RET；六外入向仅来源。71项原地址寻址与当前原生表/数组承载保；恢复记录14两首BYTE改真实unsigned char读取、S32字段不变，两请求和及八字体/高度对逐点先取样再运算。三组12处/六注释，默认正确body、所有其它源与尺寸原环/26倍位置补偿/请求归零、现场窗口元素载荷及滑块callee后发布序和返回保持。三新语义/F、INPUT/UIRUNTIME各三成员/MAP两设置根、三限定S零I/九任务open；全文件/窗口父及默认回填/滑块callee、字体载荷请求/宽高/历史绘制owner、SDK/机器ABI/实际尺寸切换文字画面运行待。

<a id="chat-control-current-20261010"></a>
## 2026-10-10 CWC01：聊天输入与按钮/滑块控制

[CWC01完整原根](cg2d_chat_window_control_pipeline_spec.md#chat-control-source-state)/[原码与真实绑定](cg2d_chat_window_control_pipeline_spec.md#evidence-archive-20261011)覆盖4A6DD0输入创建、4A6FC0按钮和4A8140滑块，共492条1769B/九页、29E8/14目标/56跳14RET；四CALL入向内部3/外1。原窗口35/20项元素与11实际callback保持，旧12槽表/CAC18范围不重复处置，11/12/19默认行为不新增。两组四处恢复三个hover窗口/元素及scroll→count取样；两个正确body、原创建/焦点/可见/返1、按钮环/提示/SE/颜色发布/scroll边界、滑块浮点与低S32位置及GBK/明文/MSVC/WAV-only保。旧1969保历史追加/两新语义三F、INPUT/UIRUNTIME各三成员/NETWORK仅配置生产按钮、三限定S零I/九任务open；全窗口父/创建绘制释放及输入字体日志请求/音效与CTS配置生产消费owner、SDK/FPU/机器ABI/实际输入文字声音画面运行待。

## 2026-10-10 CFD01：聊天焦点、滚轮与绘制门禁

[CFD01](cg2d_chat_focus_wheel_draw_spec.md#chat-focus-source-state)补齐4A6EA0/4A81D0/4A82C0三完整原根112条420B及窗口35/57五处真实绑定。创建原AL与现BYTE返回、348B复制、窗口35槽焦点/当前窗context分离、首窗口滚轮与调用后新count、共用绘制门禁和原返回有据。三个正确body、旧1788切换和原型保持；零逻辑组/六注释，三新语义F、INPUT两/UIRUNTIME三成员、两限定S零I。全门禁写方、窗口父/创建释放绘制、4A7590/4743B0、输入/鼠标/日志/元素全生产消费和owner寿命、机器ABI/实际交互画面与运行仍待，整体未完成。

## 2026-10-10 CIG01：输入横向滚动合同

[CIG01](cg2d_input_scroll_draw_spec.md#input-scroll-source-state)补齐4A7590/4A7640两个完整原根92条327B及三真实入向；明确聊天宽度先字体9 S32宽度、再聊天单位的一处读取序。全局当前context测IME与实参context滚动分离，固定1024入口/既有ScrollInputAndDraw body和所有其他源保持；原caret/offset/width发布、每轮新偏移及低BYTE bit7步长、绘制同一实参有据。两新语义F、INPUT/UIRUNTIME各两成员、两限定S零I，旧依赖与点击父证据复用；完整绘制父、字体/IME/输入/偏移全生产消费和alias-owner寿命、机器ABI/优化后读取/实际交互文字画面与运行仍待，整体未完成。

## 2026-10-10 CLD01：日志绘制与输入末字段别名

[CLD01](cg2d_chat_log_line_drawing_spec.md#chat-log-draw-source-state)补齐4A7750完整265条935B原根及一真实入向；普通宽度和每行高度上限两处按原顺序独立取样。512项134H日志、三测宽/原BYTE分行/121H行与逆序绘制、字体回写条件及index0借24项InputContext末field158别名保持，不回绕日志511或新造字段。两处取样/三注释、helper与其它源不改；一新语义F、INPUT/UIRUNTIME/MAP各一成员、三限定S零I。旧四依赖/绘制父身份不改，完整父/全生产消费与alias-owner寿命、机器ABI/实际分行文字颜色画面和运行仍待，整体未完成。

## 2026-10-10 WIC01：原型模式和context发布

[WIC01](cg2d_window_input_context_dispatch_spec.md#input-create-source-state)补齐476570/45D720/45D760三完整原根128条367B、三E8/四跳七RET及80CALL内2/外78/53父来源和四槽旧表。三个正确正文保持：既有x64带标记引用承接原高字节拒绝值，mode1落2并两次新取原型context、mode3颜色取入口context，原坐标/滚动零/四BYTE模式/颜色/字体发布与真实context返回有据；本链不分配新载荷。零逻辑组/六注释，旧1935全CAC08来源/表/范围保后追加，两新语义三F处置、INPUT/UIRUNTIME各三成员/两限定S零I。53外父/原型和context/输入载荷全生产消费与owner寿命、其余依赖/机器ABI/实际初始化切换焦点编辑和运行仍待，整体未完成。
