# cg2d_re 文本编码与跨函数契约 SPEC

> 2026-10-10当前多字节/参数准备边界：[MC01-C5](cg2d_crt_host_migration_spec.md#msvc-crt-multibyte-source-state-20261010)已退休无现业务调用的旧ctype/多字节/NLS/转换表与私有argv/环境、双BYTE桥、重复前置及专用致命入口；Input两项ASCII非零分类直接MSVC。应用命令四正文/发布槽、真实WinMain尾、数组terminate与所有业务/字段/字节/owner保持；未更改字体/GBK、协议/资源/存档编码或开全局locale。下文旧库表/状态/handler/9/9/8分支和其实施前态只作固定历史，不要求复刻，也不判原未知alias/邮件重叠已闭。全体实际CRT提供者与业务边界继续审计，全部适用SDK/正常结果/运行门禁保持，未编译/运行。

> RPLAN.16；原则于2026-09-22获批，2026-10-04压缩更新。压缩前全文见Git命名快照 `cg2d-re-spec-history-20261004-before-closure-compression` 的本路径。
> 连接提示原字节、指定绘制接线已有；整体编码/输入/字形与运行仍未验。状态见[唯一进度](../plan/cg2d_full_reconstruction_progress.md)，后续见[收尾T01–T03/V04](../plan/cg2d_spec_closure_checklist.md)。

> 2026-10-09用户更新：源码静态Big5文字改为GBK，字体统一宋体、光标采用ASCII `|`；按[本次更新](#big5-to-gbk-source-state)覆盖旧静态原字节/字体/光标限制，原端证据仍保留。

## 1. 批准范围与非目标

cg2d_re承接原端行为的兼容数据以原字节为权威，覆盖父级内部UTF-8约定在此范围内的要求；源码/文档仍显式UTF-8。Unicode仅在真实现代输入/显示边界有需求时集中转换，不预建通用框架、不跨改旧src/text；2026-10-09仅按用户要求转换已确认的静态Big5文字，不扩展为协议/资源/存档或API迁移。

历史“新端内部UTF-8”只说明当时现状，不作为必须转码或已完成修复的依据；原则批准不等于源码/整链验收。来源编码分别取证，不能由一条繁体提示推出全工程Big5/CP950，也不能使用猜测默认代码页；未知编码可保留原字节，依赖该编码的转换不得猜测推进。

## 2. 核心与显示契约

| 范围 | 必须保持 |
|---|---|
| 静态文本 | 原地址/字节保留作基线；按2026-10-09要求以显式GBK字节常量承载已确认的Big5文字，明确记录字体/光标及不可表示标点差异；未授权来源保持原字节 |
| Input/协议/存档 | 原表示、容量、字段偏移、字节索引、NUL及读写顺序；不按Unicode字符数重排 |
| 测宽/换行/截断/入队 | 原字节算法、预算、边界推进、处理序及布局；不整体改写TextMeasure或在调用点反复转码 |
| 必需的Unicode显示边界 | 只派生显示副本，不反写原缓冲/布局/光标/协议/存档；明确转换位置与失败策略 |
| 最终呈现 | SdlPresenter只收像素帧，不是文本转换点；[公开接口](../include/cg/render/SdlPresenter.hpp)不新增文本职责 |

每条链分别记录字节索引、字符索引和像素；Unicode索引注明码元/码点等具体单位。权威缓冲和副本须有owner、容量是否含NUL、借用/复制、有效期及释放责任；临时副本不替代共享状态。

## 3. 输入侧与未决行为

覆盖键盘文本、IME组合/提交、光标/选区/属性偏移及剪贴板，明确Unicode→原字节转换和双向索引；[Input](../include/cg/Input.hpp)的字节光标、容量和原序保持，Unicode索引不能直接写入字节字段。

2026-10-04本轮决议采用现有原字节/A系输入与GDI边界，不新增Unicode/API迁移或字体替代；来源编码/实际时序继续取证，不把未来可选转换列为当前必须开发。确需改变现有接口时另写具名SPEC。原算法接受的非法编码字节不得被新增严格解码门槛拒绝；显示失败不改原数据/路径。不可表示字符、超长输入、转换失败和字体差异须有证据/裁决，不擅换?、丢字或截断；原已证截断仍保留。此SPEC不扩大有效内存域、不声称越界可移植，异常执行口径沿总SPEC。

## 4. 首个实施任务 R6.1.2.8

服务器连接提示：常量→复制→TextMeasure→窗口布局→文字入队。证据见[登录状态](cg2d_login_state_controller.md)、[字体度量](cg2d_font_measure.md)、[文本队列](cg2d_text_pending_queue.md)；源码为[LoginState](../src/LoginState.cpp)、[Font](../src/render/Font.cpp)。

原55C7F0的正文/NUL为 `A6 F8 AA 41 BE B9 B3 73 B1 B5 A4 A4 00`。原基线字节保留，kServerConnectingText现以GBK表示同一「伺服器連接中」文字，memcpy(sizeof(...))仍只覆盖13B，不清256B余部；首次与绘制前再次测宽、窗口公式及DrawText不改。

| 固定font0.doubleWidth=17的静态推导 | 当前GBK字节 | 修复前UTF-8历史差异 |
|---|---:|---:|
| 正文/含NUL字节 | 12/13 | 18/19 |
| 双字节累计次数 / 测宽像素 | 6/102 | 9/153 |
| 列数max(2,(width+111)/64) | 3 | 4 |
| 窗口X / 宽 | 224/192 | 192/256 |

本输入推导DrawText(269,232,2,0,0,text,0,0)，源码未硬编码布局。DrawText_456780按0..textLength复制正文及NUL到独立记录，不借用连接缓冲。此为局部源码/静态对齐，不是新测试或连接/客户端运行通过。

## 5. 跨函数复核模板

`基线/原地址 → 生产者表示/单位 → 转换 → 消费者状态 → 预期输出 → 批准差异 → 版本/验证范围`

| 字段 | 必填 |
|---|---|
| 基线与原证据 | 可定位原地址/字节/函数/数据及既有证据链接；不新增文件摘要门禁 |
| 生产者 | 来源编码或未知、原字节、容量/NUL、索引单位、owner/有效期 |
| 转换 | 无转换或明确位置/方向/表示、失败策略、副本生命周期及索引映射 |
| 消费者 | 实际调用者、字节/字符预算、像素度量、共享存储与读写序 |
| 输出 | 同输入/初值下的原新数值、入队字节、布局或其他可核结果 |
| 差异 | 裁决、日期及适用范围；未决不靠披露转为批准 |
| 版本/验证 | 源码版本、证据版本、实际核对范围和等级；不沿用至未核新版本 |

由本线程按新增差异核整链，不安排独立复核。单函数静态对应、符号接通或声明限制不能代整条文本合同，更不能代运行/视觉验收。

## 6. 实施边界与待决项

当前不新增Unicode转换；仍核各来源编码、既有输入/IME索引、失败/超长处理和实际字体/完整链，未知事实不以本轮工程决定代替；只阻断受影响适配，不自动改异常域或批准差异。连接提示已对齐不意味着全部生产者都对齐，既存输入预清/门禁/快照差异保持可见。

执行沿[总SPEC](../plan/cg2d_full_reconstruction_plan.md)，不构建/测试/运行或计算摘要。已批准原则、已落源码、已接父和实际验收分列。

## 7. 原字节文本消费接线

RenderTextToSurface_456D70已在Font.cpp完整实现，原NUL字节串直接交同一GDI字体TextOutA；现路径无需Unicode副本，不指定猜测代码页，不反写旧缓冲/测宽/坐标。阴影和正文分别重取长度，字体选择及两种链遍历原序保持。

GDI和SDL共享同一Back mapping，render私有边界管理DC/bitmap，队列公开接口不含Windows类型，见[共享Back合同](cg2d_sdl_presenter.md)。DrawAllQueued case4已有真实调用并保留设备场景结束/重开；没有据此改变Font创建、IME、协议/存档编码。目标字体/字形、设备模式和最终画面仍未验。

## 2026-10-05：IME组合/提交局部差异落码

[EIC01源码收口](cg2d_ime_composition_alignment_spec.md#ime-composition-source-closure)撤销459580三处额外负长度拒绝与局部结果预清，恢复S32结果数量/循环及signed LONG终止下标；原255上限、四标志独立序、七API每次重读HIMC、逐字节AppendChar和游标/相位保留。继续IMM32 A原字节与同一共享256B存储，不新增Unicode转换/owner或用API失败补值。

原负长度/未写局部访问依RPLAN.24，不专项模拟故障、不记安全/故障等价通过。此项只归档适用路径的限定I/W/S；输入绘制/编辑等其他门禁、预清/快照差异、完整焦点/生命周期及SDK/运行仍待T01–T03，不把该本体修正当完整文本验收。

## 2026-10-05：输入光标消费取样对齐

[ICA01源码收口](cg2d_input_caret_alignment_spec.md#input-caret-source-closure)沿EIC01组合/属性/游标生产，撤销44A410绘制辅助空对象/字体门禁和256B预扫描，恢复始终真实测宽；测宽前后两个共享当前对象样本保持，原光标字节A2 78 00静态借用入队，不复制/转码。相位用尾部本次S32计算值比较，发布后不另读共享槽。

仅该本体及16直接caller/四callee点的局部静态；完整输入绘制/编辑、焦点/共享寿命和SDK/运行仍待T01–T03。RPLAN.24异常访问不作安全/故障等价结论，不把删保护或127原指令当整链验收。

## 2026-10-05：组合/属性及候选绘制消费对齐

[RIN01](cg2d_input_render_alignment_spec.md#input-render-source-closure)沿EIC01/ICA01继续完成4590E0本体差异：撤销额外预扫描/拒绝/局部预清，实际S16说明坐标与Itoa返回消费；候选有效项/跳过项按原条件读取表，页大小先读再递增。组合/属性原字节/索引、两测宽及文字/框取样、DWORD候选偏移和owner保持，无Unicode迁移或新默认文本。

364原指令、21直接caller及当前存储为局部I/W/S，完整焦点/IME重入、候选寿命/间接alias与SDK/画面运行未验；未知坐标writer仍待，不把原异常域、文档/本体或局部数量当全链验收。

## 2026-10-05：方向/删除编辑的字节数量与取样

[IED01](cg2d_input_edit_alignment_spec.md#input-edit-source-closure)恢复上下扫描S32行宽先保存、余串低U32比较/分类，终行在目标/列确定后重新扫描；五局部预清撤销，实际复制填充/显式NUL和原高位字节规则保持。退格原钳制后重新取result，左右保存字段别名不改，无Unicode索引、默认正文或新owner。

521原指令/六caller及七分类点只归档局部I/W/S，AppendChar/其他编辑、完整焦点/IME/寿命和SDK/运行继续待T01/T02；原异常域不模拟或记安全/故障等价，不以七ID处置当全文本门禁通过。

## 2026-10-05：追加副本与原字节边界

[IAP01](cg2d_input_append_alignment_spec.md#input-append-source-closure)只撤销额外追加状态拒绝及289B tail/288B oldText预清，真实复制padding/NUL与Count289/288/288保持；拼接仍287减strlen低U32。原S8过滤、bit7末单元0～3、CR折叠/行预算与条件回滚保持，00接受/更新计数不新增一致性拒绝。完整文本绘制/输入父、共享寿命/间接alias、SDK与运行继续待验；不作异常域安全或故障等价声明。

## 2026-10-05：输入绘制原字节生产与重读

[ITD01](cg2d_input_text_alignment_spec.md#input-text-draw-source-closure)恢复多行逐字节预算、复制前/后和分类后各阶段取值及逐段低W8模式；单行先写记录，星号每步重读数量，普通后缀含NUL低U32/逐W32再W8复制。原DBCS/CR、完全不截断分支和字节/像素单位保持，不新增Unicode转换或容量保护。

裁宽测宽返回后才取宽度限制，每次超宽重新取原后缀；三完整模式索引/队列计数与两锚点真实测宽按原点。仅根332/测宽支持33条局部I/W/S，不提升完整输入/文本/队列寿命或运行，RPLAN.24异常域不记安全/故障等价。网络继续原格式明文，星号仅原输入遮显。

## 2026-10-05：文本队列生产复制原阶段

[TQP01](cg2d_text_queue_alignment_spec.md#text-queue-producer-source-closure)恢复记录字段先写、原文本扫描/含NUL低U32数量及逐W32后余W8复制。signed模式复制完取尾，unsigned模式在W32后/W8前取尾；空链先读实际layer再更新尾槽，原计数重读/返回和满队列失败保持，额外拒绝撤销。

DrawText3仍真实复用font0核心，输入绘制/所有caller和消费不改，token/唯一owner保持。四完整262条/854原点只局部静态与库存归档，不计全部上游/间接alias/消费/寿命或SDK/运行通过；异常域不称安全/故障等价。网络原格式明文不变，无Unicode或新补NUL/默认正文。

## 2026-10-05：字体族字节/度量取样与原文本消费

[FLC01](cg2d_font_lifecycle_alignment_spec.md#font-lifecycle-source-closure)恢复初始化的原本地族选择、保存S8族号/width-face-height及unsigned派生比较；族名字节、字体度量/实际句柄/39颜色和原API参数保持，不转换代码页/Unicode或新建字体fallback。当前TextMeasure早期保护描述按后续源码修正解读，不新增空指针或非法字体返回0。

558条原体只局部静态；原TextOutA每次取原字节长度、Y→X/next和两消费分支未改，完整源/父/Back-DC与字体生命周期、SDK/画面运行继续待验。

<a id="big5-to-gbk-source-state"></a>

## 2026-10-09：静态Big5转GBK、宋体与ASCII光标

用户明确要求静态Big5字符串改为GBK，随后要求字体统一宋体并处理 `|` 光标。本次共处理43个源码文件、1663处静态文本：1661处按CP950严格解码并编码为GBK，保留繁体文字、ASCII/格式占位、正文长度和NUL位置；其余两处按用户指定更换字体名和光标。源码文件仍UTF-8，运行字节仍显式转义，不依赖源码字面量编码生成GBK。

GBK不支持U+2027 `‧`，五处处理中提示改用U+00B7 `·`，保留双字节占位和原个数，不用问号或丢字。字体名「細明體」改为GBK「宋体」`CB CE CC E5 00`，对应sizeof由7改5；四族和全部现有创建分支共用该名，CreateFontA采用GB2312_CHARSET，字号、字重、字体创建序和度量表保持。光标原CP950 `A2 78`「│」改为ASCII `7C 00`「|」，普通/IME两条绘制路径按sWidth/2居中；IME文字测宽、字节游标、前后对象取样及闪烁不改。

68处非Big5或混合非法字节不能整体按CP950解释，包含日文Shift-JIS及原不完整字节，保持原样；不改原端数据库、协议格式/组包、资源、存档、外部输入或音频二进制。十处既有UTF-8启动提示保持其独立显示边界。已确认当前系统ACP为936，原A系窗口/输入/显示接口保持；不同系统代码页及未安装宋体的环境未验证。

[结构化记录](cg2d_big5_to_gbk_evidence.json)列逐文件数量、原源码提交、特殊字符裁决及边界；完整逐字符串差异由Git固定快照 `cg2d-re-big5-to-gbk-20261009` 对照提交 `09f64ac8` 追踪。43个文件的全部字面量逐组静态核对通过，未批准代码/原ASCII变化为0，C++转义无越界或吞并；未编译或启动客户端。旧文档中的原字节/族名/光标描述继续作为原端基线，不代表本次更新后的字节一致结论，也不提升完整文本或UI验收。

<a id="text-unescape-current-20261010"></a>
## 2026-10-10 UTE01：文本反转义原位取样

[UTE01完整原根](cg2d_text_unescape_sampling_spec.md#text-unescape-source-state)/[原码与映射](cg2d_text_unescape_sampling_spec.md#evidence-archive-20261011)覆盖52D120/52D150/52D2F0/509080，共66条147B/四页、三E8/三个内部目标/七跳五RET；316入向内部3/外313只证明来源。原58E944四对BYTE映射与未知字母低BYTE返回保持，恢复S32映射计数、每轮首BYTE持有及后继读取先于索引递增/共尾写。原高bit两BYTE、反斜杠字母、固定入口长度和两S32索引、末NUL及同text返回保；不切换编码或增加空值/容量/末字节检查。包装弃返→真实单RET空操作→同text返回，两正确body保，不新增通知/成功值/API或owner。四新语义/四F、INPUT/UIRUNTIME各四成员/NETWORK三个文本根、三限定S零I/九任务open；全部外调用、文本生产消费/容量与alias-owner寿命、SDK/机器ABI/真实字符消息画面运行待。

<a id="text-escape-current-20261010"></a>
## 2026-10-10 ETE01：文本转义生产端取样

[ETE01完整原根](cg2d_text_escape_sampling_spec.md#text-escape-source-state)/[原码与映射](cg2d_text_escape_sampling_spec.md#evidence-archive-20261011)覆盖52D1B0转义生产/52D2D0包装，共108条310B/三页、两E8/两目标/11跳两RET；112入向内部1/外111仅来源。恢复非地址S32表计数、已读首BYTE持有/后继与索引及计数更新序、匹配字母早退前读取/两写后更新索引三组。原四对BYTE映射、固定入口长度和两S32索引、原容量/调整长度门槛与截断、末NUL/同destination返回保持，不切换编码或加保护。正确包装和InputContext文本便利适配保持，不新增原根/通知/成功值或owner；UTE01四根和所有其它源保。两个新语义/F、INPUT/UIRUNTIME/NETWORK各两成员、三限定S零I/九任务open；全外调用、文本生产消费/容量与alias-owner寿命、SDK/机器ABI/真实截断和字符消息画面运行待。

<a id="gbk-readable-literals-20261014"></a>
## 2026-10-14：可读GBK字面量方案（GLT01，待确定范围）

用户要求kModeChangeText等静态文字直接采用字面量，不以转义字符拼写正文。已核现有kModeChangeText的18B GBK正文为“正在改變描繪模式。”；拟改为：

```cpp
static constexpr char kModeChangeText[] = "正在改變描繪模式。";
```

源码继续UTF-8，相关目标普通窄字面量执行字符集采用GBK（MSVC /source-charset:utf-8、/execution-charset:.936）；当前/utf-8同时指定UTF-8源／执行字符集，不能仅替换拼写。带引号、反斜杠或换行的可读文本采用C++原始字符串字面量。已有UTF-8提示显式保留UTF-8，沿原char接口适配，不能随普通字面量改成GBK。
范围选择待用户回复：统一处理已有证据确认的同类GBK静态界面文字，或仅kModeChangeText；非Big5／混合／日文字节按既有证据保留，不能按可解码就猜为GBK。保留原地址注释、繁体文字、格式占位、正文与NUL字节、sizeof／缓冲容量及全部消费者，不改变显示、协议、资源或输入编码边界，不新增依赖或运行时转码。
实施前后逐组静态核对CP936编码回读与完整字节／容量，仅必要静态及台账文本汇总；不编译、测试、运行或计算摘要，既有编译／画面／整体门禁保持。当前仅记录方案，尚未修改源码或CMake。
MSVC选项依据：[执行字符集](https://learn.microsoft.com/en-us/cpp/build/reference/execution-charset-set-execution-character-set?view=msvc-170)、[/utf-8](https://learn.microsoft.com/en-us/cpp/build/reference/utf-8-set-source-and-executable-character-sets-to-utf-8?view=msvc-170)。

<a id="unicode-ui-source-20261014"></a>
## 2026-10-14：Unicode界面迁移方案（UTL01，待范围确认）

用户已确认同类静态界面文字统一处理，随后提出Unicode及绘制也采用Unicode；GLT01的GBK字面量方案暂停实施，尚未改源码或CMake。本方案建议内部UTF-8、Win32文字边界UTF-16，保留当前/utf-8；正文使用直接或原始字符串字面量，不用十六进制转义拼写已确认文字。

1. 依据现有GBK迁移证据将已确认静态界面文字还原为Unicode文字，保繁体、格式占位、原地址注释与字体宋体；既有UTF-8提示继续UTF-8。非Big5／混合字节不猜编码，按既有来源逐项明确转换或保留其旧编码边界。
2. Font.cpp现TextOutA改为TextOutW，字体创建采用CreateFontW；UTF-8转UTF-16在Win32调用边界完成。沿CPU文本DC、既有队列顺序、颜色、阴影、坐标和surface生命周期，不更换渲染后端或引外部依赖。
3. 绘制、测宽、换行、截取及光标必须同步采用Unicode字符边界，不能继续用高bit判为两字节。以实际字体的W文字度量为依据；由旧单／双字节固定宽度改为Unicode文字度量的布局差异须明确记录。
4. 现TextPendingRecord::text为255B，DrawText／DrawText2直接按strlen+1复制；UTF-8扩长不能继续沿旧sizeof或固定GBK长度。迁移内部文本owner与必要调用，保留原字段偏移作注释／历史；固定协议与资源字节域不随UI文本扩宽，逐项核容量、终止符、长度单位和寿命。
5. 若确认完整Unicode界面范围，窗口注册／创建／默认消息处理及IME同步采用W接口；输入缓冲、组合串、属性／光标索引、删除及候选文字统一明确UTF-8字节与UTF-16索引转换，不混接A／W字符消息。
6. 网络协议、资源和存档按已确认原GBK字节合同在边界转码，不改变格式／字段容量／外部数据；不将字面量自动改变视为整个边界迁移完成。

范围待用户答复：是否将当前静态字面量任务扩展为上述完整Unicode界面迁移，或仍只改变GBK字面量拼写。获确认后先沿具体生产／消费／owner落实全部必要调整，保原端事实及具名差异；仅必要静态与文本汇总，不编译、测试、运行、查产物或计算摘要。当前只完成源码边界分析与方案记录，未声称已实现或运行通过。
