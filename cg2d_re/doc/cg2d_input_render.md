# cg2d 输入法说明、组合串与候选绘制还原

## 函数边界与直接证据

- `World::RenderInput@[0x4590E0,0x45957E)`，大小 `0x49E`（1182 字节），IDA
  统计 357 条指令、39 个基本块，原型为 `void __cdecl()`；当前数据库共有 21 个
  直接调用点。
- 直接目标包括 `sub_459090@0x459090`、`GetKeyboardState`、`GetKeyboardLayout`、
  `ImmGetDescriptionA`、`Renderer::DrawText@0x456780`、
  `Renderer::DrawText2@0x456670`、`TextMeasure@0x44B2A0`、
  `sub_44B250@0x44B250`、十进制转换和
  `Renderer::DrawGraphic_Hover@0x522180`。
- `sub_459090@[0x459090,0x45909F)` 把 `GetKeyboardLayout(0)` 的结果传给
  `ImmIsIME`。共享适配保留原 Win32 `BOOL` 的完整 32 位返回值；本函数和光标函数
  只按非零判断，不把它压缩成 `bool`。
- `sub_44B250@[0x44B250,0x44B25E)` 仅检查输入字节 bit7，置位返回 2，否则返回 1；
  属性扫描据此同步推进组合串和属性指针，不作字符集转换。

## 入口与输入法说明

入口 `0x4590E6..0x459130` 保持如下采样顺序：先读取
`gImeComposition[0]`，把组合串、属性、宽度、目标起点和命中标记初始化为零，再把
入口首字节镜像到 `gImeState73EFF0`；随后调用共享 IME 判定，无条件调用
`GetKeyboardState`，最后依次采样 `gValueD460C8`、Caps 字节和
`gCurrentInputData`。`gValueD460C8` 是唯一零初始化 W32，只保留原
`dword_D460C8@0xD460C8` 的地址型名称。

仅当说明门禁为零且该次当前上下文非空时绘制输入法说明：

- IME 路径在 `0x45914F` 再独立读取一次键盘布局，调用
  `ImmGetDescriptionA(description,100)`，无条件把 `description[10]` 写成 NUL。
- 非 IME 路径按入口保存的 Caps 字节复制固定 12 字节。小写为
  `AD 5E BC C6 2F A2 E9 A2 EA A2 EB 00`，大写为
  `AD 5E BC C6 2F A2 CF A2 D0 A2 D1 00`。
- 原4591AF先读5595F0低S16 Y、4591B6再读5595EC低S16 X，已加载值分别461/542；新端由真实S16存储gImeDescriptionY5595F0/gImeDescriptionX5595EC承接。调用DrawText_456780(x,y,3,1,8,description,0,0)返回弃，随后重取当前对象；不补未知writer或称间接alias已闭合。说明门禁不阻断后续组合/候选。

早期源码曾在调用GetKeyboardState前清零256B局部；现按[RIN01](cg2d_input_render_alignment_spec.md#input-render-source-closure)撤销。原缓冲未预清，API失败不补默认键态，未写字节访问按RPLAN.24，不声明安全/故障等价。

## 组合串与属性范围

组合路径只在入口 IME 判定非零、重新读取的 `gImeComposition[0]` 非零且当前上下文
非空时执行。完整 `field138` 为零时基础层为 `0xD2`，否则为 `0xE0`；文本模式为
`field138` 低字节按 W8 加一，字体取完整 `field13C` 的低 8 位，颜色和值 7 均为零。
组合文本以当前上下文 X/Y 调用 `DrawText2_456670`。

组合头保留原独立字段读取：`0x4591F7` 先读取完整 `field138` 判基础层，
`0x4591FD` 再读取一次 `field138` 低字节计算模式，不能从同一快照派生；其后按
`0x459203/0x45921F/0x459226` 的 `font -> y -> x` 顺序各读取一次并锁定为局部值。

文本调用后在 `0x459237` 重新读取当前上下文及完整字体并调用 `TextMeasure`；测宽后
又在 `0x45924D` 重新读取上下文，以该次 X/Y、字体 `doubleHeight` 和测得宽度绘制
整体框：`(x-1,y-1,x+width,y+doubleHeight,baseLayer,1,1)`。各调用边界的当前
上下文和字体读取互不合并；整体框字段严格按 `0x45925B/0x459261/0x459267` 的
`y -> x -> font` 顺序各读取一次，坐标表达式只使用局部快照。

属性扫描从两个缓冲的索引 0 开始，宽度、目标起点和命中标记均为零：

- 属性 1 或 3 首次出现时记录当前宽度为目标起点并置命中。
- 命中后遇属性 0 或 2，在处理该字节前结束；命中前属性 2 正常处理，属性 0 结束。
- 每轮只按组合字节 bit7 选择 1/2 字节步长，两个索引同步推进；随后重新读取当前
  上下文及完整字体，分别累加 `sWidth` 或 `doubleWidth`。
- 命中后再重读上下文和字体，绘制
  `(x+start-1,y-1,x+end,y+doubleHeight,baseLayer+1,252,1)`。

两个步长块不合并采样顺序：双字节块在 `0x4592EB/0x4592ED` 先计算两个索引加 2，
再于 `0x4592EF` 重读当前上下文；单字节块先在 `0x45931A` 重读上下文，随后按
`0x459323` 推进组合索引、`0x459324` 读取字体、`0x45932A` 推进属性索引。属性框在
`0x459350` 再重读上下文，并按 `0x45935E/0x459364/0x45936A` 的
`y -> x -> font` 顺序各读取一次。

## 候选列表

候选仅位于上述组合分支内，要求 `gImeCandidateList` 非空且 `dwPageSize` 非零。
实现按 Win32 `CANDIDATELIST` 的 DWORD 布局读取 `dwCount@+8`、
`dwSelection@+0xC`、`dwPageStart@+0x10`、`dwPageSize@+0x14` 和
`dwOffset[]@+0x18`。循环索引为无符号W32，只有W32(pageStart+i)<count才处理该项。有效项绘框返回后459556重取候选，跳过项沿本轮表；459563先读取pageSize，再递增索引/无符号比较，不合并为每轮无条件重读或尾空指针拒绝。

每项先保存旧单位数，并按原调用边界反复读取当前上下文、完整字体和
`doubleHeight`：

1. 把 `i+1` 的 W32 按 signed32 安全转换为十进制，序号位置为
   `x+targetStart+oldUnits*height+5, y+height`，模式 4、颜色 0。
2. 正文位置为 `x+targetStart+(oldUnits+1)*height, y+height`，模式 4、颜色 0；
   文本来自当前候选表的 `dwOffset[pageStart+i]`。
3. 对再次读取的正文指针执行原始 `strlen`，长度按 signed W32 小于 2 时取 2，再按
   `units = oldUnits + 1 + trunc_zero(length/2)` 更新布局。
4. 重新读取当前上下文及字体调用 `TextMeasure`，再重读候选选择项和当前上下文绘框。
   框为
   `(x+targetStart+oldUnits*h, y+h,
   x+targetStart+(oldUnits+1)*h+width, y+2h)`；选中项使用层 `0xD9`、图号 0，
   其余使用层 `0xD8`、图号 252，step 均为 1。

候选段同样不合并跨调用快照：序号先读取一份 `current -> font -> doubleHeight`，十进制
转换后重新读取 `current -> y -> x`；序号调用后，正文重新读取
`current -> font -> doubleHeight`，保留两次候选指针读取，再读取
`current -> y -> x`。正文测宽前另读 `current -> font`；选择状态读取后，框再按
`current -> font -> y -> x` 采样。每个字段在对应采样点只读一次，后续表达式均使用
该次局部快照。

所有文本和图形调用返回值均忽略；文本坐标最终取低 S16，模式和字体取低 8 位，
图框坐标取低 U16，层取低 8 位。

## 当前源码边界（2026-10-05）

- 旧额外空对象/字体/重复Count拒绝、组合/属性预扫描及影子valid/宽度补0已按[RIN01](cg2d_input_render_alignment_spec.md#input-render-source-closure)撤销；旧状态见Git命名快照cg2d-re-input-caret-source-20261005的本页/源码，不继续作为当前批准差异。
- 原说明/组合三门禁、首次候选非空/页大小非0与每项U32 count门禁保持。各阶段原取样/调用与参数不改，正文两候选读取、strlen缓存给同一TextMeasure，内部不加默认文本/提前返回。
- 键态256B和序号12B局部不预清；序号消费实际Itoa返回指针，原S32长度<2取2、除2/units与选中D9/0、未选D8/252保持。有效项框返回才重取候选，跳过项保留本轮表，页大小读取先于递增。
- 两S16说明坐标按原已加载值实际承接/读取，未知间接writer仍待R02；全当前对象/候选分配大小、API重入与寿命不由直接xref或本体齐备证明。
- Win32 CANDIDATELIST头及DWORD偏移保持，ReadCandidateOffset读可变尾一个W32 offset并在地址边界拓宽；不把dwOffset[1]推为容量，不造未知分配边界检查。API候选owner、shared状态与释放未改变。
- 已有位宽/截断运算保留，无新增异常/溢出模拟。API未写局部、空对象/列表、域外字体/offset及越界等沿RPLAN.24，不保证x64安全/原故障等价，不记为已验证或修复。
- [证据](cg2d_input_render_alignment_evidence.json)保存357+7共364唯一原指令、21caller及已加载字节/坐标；只归档限定I/W/S，完整父/间接边、焦点、SDK/最终画面与运行未验，不构建、测试、运行或摘要。
## 数字格式化依赖 SPEC（2026-10-02）

原itoa@[54ABDD,54AC0A)18条、xtoa@[54AC0A,54AC66)47条，
共同完成输入法候选序号和协议校验值的文本转换。按现有工程委托，在
LegacyTime.cpp的已有旧字符串边界实现Itoa_54ABDD及内部Xtoa_54AC0A，
通过LegacyTime.hpp提供Itoa入口，不引入库或第二套数字格式化状态。

- itoa只有4593E7（RenderInput）和517625（BuildProtocolCommand）两个直接调用。
  前者传i+1的完整W32、目标局部、radix10并使用返回指针；
  后者传已形成的校验值、栈槽、radix16并弃用返回。
- itoa仅在radix==10且原S32值<0时传负号标志1；其他情况传0。
  不把非十进制负数转成带负号文本，不扩大原数值位宽。
- xtoa先按负号标志写'-'并对W32取负；随后至少一轮无符号余数/商运算。
  余数0..9写'0'+digit，其他写低U8的0x57+digit，保留小写数字字母。
- 数字先按逆序直接写入目标，随后写NUL，再就地交换数字首尾；
  负号不参与反转。原do循环在仅一个数字时也执行两次同位置写入。
  新端用S32左右索引保留交换顺序，避免为这一正常单数字路径构造数组前指针。
- xtoa仅有54ABFD一个直接调用，caller不消费其寄存器残值；内部接口返回void。
  itoa独立返回原目标char*，原端两处返回消费合同保持。
- 不新增radix范围、容量、空指针或失败校验，不补零整个目标；
  非法进制与越界仍按RPLAN.24，不专项模拟错误，也不称故障等价。

既有FormatDecimalW32保留接口，内部改为Itoa(valueBits按S32解释, destination,10)；
不改候选页号取样、原有外层12B局部清零、字体门禁或坐标/文本调用顺序。
BuildProtocolCommand只替换原宿主_itoa调用，继续16进制、原校验值和目标栈槽，
后续strlen/复制及共享状态次序保持；不据此解决待决wire或恢复加密链。

本步收敛原真实共享数字转换依赖，不重构输入法或协议构造父流程。
静态核两个入口65条和两个实际调用；不构建、测试、运行或校验哈希。

2026-10-05当前说明：上节数字叶当时未改父的12B预清/字体条件；RIN01已根据原完整父撤销这些旧源码差异并消费真实返回，数字叶/协议调用不变。历史步骤不作为当前批准例外。

## IMA01 组合与候选布局算术（2026-10-05）

[当前具名SPEC](cg2d_ime_display_arithmetic_spec.md#ime-display-arithmetic-decision)将RenderInput.cpp三个工具恢复正常S32加减乘；保组合/高亮框、属性宽度、候选序号/正文/框的32处既有表达式及原取样/调用写序。真实低S16/U16/S8、SDK DWORD、有效项重读/skip和signed长度下限2保持，不新增owner或拒绝。

仅三个body变化；357条1182B和21call/三父保存不代全部原语义/完整父/坐标writer/间接alias/候选容量及完整寿命/实际SDK/画面运行。RPLAN.24异常不保护或模拟/称等价。

## 2026-10-10 CIG01：滚动到当前插入点并绘制

[CIG01](cg2d_input_scroll_draw_spec.md#input-scroll-source-state)补齐4A7590/4A7640两个完整原根92条327B及三真实入向；明确聊天宽度先字体9 S32宽度、再聊天单位的一处读取序。全局当前context测IME与实参context滚动分离，固定1024入口/既有ScrollInputAndDraw body和所有其他源保持；原caret/offset/width发布、每轮新偏移及低BYTE bit7步长、绘制同一实参有据。两新语义F、INPUT/UIRUNTIME各两成员、两限定S零I，旧依赖与点击父证据复用；完整绘制父、字体/IME/输入/偏移全生产消费和alias-owner寿命、机器ABI/优化后读取/实际交互文字画面与运行仍待，整体未完成。

## 2026-10-10 CLD01：聊天日志字节分行及逆序绘制

[CLD01](cg2d_chat_log_line_drawing_spec.md#chat-log-draw-source-state)补齐4A7750完整265条935B原根及一真实入向；普通宽度和每行高度上限两处按原顺序独立取样。512项134H日志、三测宽/原BYTE分行/121H行与逆序绘制、字体回写条件及index0借24项InputContext末field158别名保持，不回绕日志511或新造字段。两处取样/三注释、helper与其它源不改；一新语义F、INPUT/UIRUNTIME/MAP各一成员、三限定S零I。旧四依赖/绘制父身份不改，完整父/全生产消费与alias-owner寿命、机器ABI/实际分行文字颜色画面和运行仍待，整体未完成。

## 2026-10-10 WDP01：完整窗口输入/日志绘制分发

[WDP01](cg2d_window_element_draw_pipeline_spec.md#window-draw-source-state)补齐4729B0完整277条846B及七槽分发表/51CALL与34父来源。六组十三处恢复入口坐标、入口图号缓存、四低W16裁剪、两图形坐标路径、数字载荷四字段和四动画颜色BYTE；所有原visible/图号/焦点/context/text/数字null门槛、六业务分发/每元素新窗口count/尾层3清队列与范围提示保持。共用Yobi helper改读选中BYTE，S32存储/其他调用源/GetField20正确body不改，不外推其他父；四整源/四注释精确静态通过。旧1932全部CAC08来源/表/范围子记录保后追加，一F处置/四限定S零I，INPUT/UIRUNTIME/RENDER/YOBI各一成员；51外父/元素载荷/资源动画字体输入日志提示全生产消费及owner寿命、机器ABI/真实像素文字动画与运行仍待，整体未完成。

## 2026-10-10 WND01：数字文本步进与图片消费

[WND01](cg2d_window_number_draw_spec.md#number-draw-source-state)补齐4771B0/477460/477170三完整原根255条774B、两E8与38CALL/21父来源，120W32/480B四表与源常量吻合。旧86初始化尾跳、553098原槽及现MSVC构造接线复用，不新建同一身份或恢复私有CRT机制。测宽明确先运行步进乘字节长度再读扣宽；原S8样式/颜色、flags正反字节绘制/四符号/未知BYTE仍步进、descriptor、四S32初始化序和所有其他源保持，七关联注释。三新语义F、UIRUNTIME/RENDER各三成员、两限定S零I；全父/数字载荷字符串图号步进队列生产消费及owner寿命、构造退出时序/机器ABI/真实数字符号像素与运行仍待，整体未完成。

## 2026-10-10 WPL01：窗口输入载荷的外层owner与借用context

[WPL01](cg2d_window_payload_creation_spec.md#window-payload-source-state)补齐471440完整384条1159B、13E8/五目标/40跳五RET及一次update间接、52CALL/21父来源和七槽旧表。七类载荷是既有真实结构：外层由UI管理，type3另own256B文本、type7借字符串、type5借context、type2 action由Yobi管理；旧1797/BVR释放正文保。现有唯一helper改借初始action及现有action/图号槽，恢复首flags目标、延迟图号指针和图号指针→action槽→值的交错，一组三处/四注释；声明/定义/唯一调用同步，其余创建/释放/其它source和类型保持。旧1929全CAC08历史保后追加、一F处置/四域各一成员/四限定S零I；全父/七载荷及嵌套对象生产消费与owner寿命、SDK/机器ABI/实际创建更新释放和运行仍待，整体未完成。

## 2026-10-10 WSB01：鼠标滚动载荷消费

[WSB01](cg2d_window_scroll_callback_spec.md#window-scroll-source-state)补齐4741A0/4B0F10两完整原根76条211B、一E8/六跳两RET，七实际CALL和十九代码地址发布有据；四滚动原父未归属不猜。整数组xref按实际偏移核58项57536B、16滚动原型（15type4/1type1）和1125零回调槽，未把整表引用当CALL。滚动根上下界和差值三个原阶段读取顺序已明确，初始data持样/低BYTE与W32标志/每阶段现场数组和原边界调整保持；零回调确为33C0/C3只返0，正确body保。两整源三段/四注释及三个结构静态差异通过；两新F/两adapted处置，INPUT/UIRUNTIME各两成员、两限定S零I、九任务open。全父/间接绑定/跨类型载荷生产消费与alias-owner寿命、实际滚动画面、机器ABI和运行仍待，整体未完成。

## 2026-10-10 WSP01：滚动状态与位置换算

[WSP01](cg2d_window_scroll_position_spec.md#window-scroll-position-source-state)补齐两个父及位置换算三完整F原根，复用四既有X代码体，共162条431B/十二E8/十一跳八RET。四曾无IDA父的滚动调用已按旧1959/1961/1992/1999 code_identity解析，保空inventory_id和原X用途，不造F。六父各原非零/恰1/statebit3条件、参数58/950/count/count-10/90和保存S32返回保持；聊天换算后新取count，既有焦点子不改。474270两整数除法和非正maximum分支保持，仅明确先当前/前项height一取样段；两整源十四注释/六父与共享子正文保持。两个新原型和31换算入向内五/外26有据；三新F/三处置、INPUT/UIRUNTIME各三成员、两限定S零I。全部间接绑定/外父/数据生产消费与owner、机器ABI/实际拖动画面和运行仍待，整体未完成。

## 2026-10-10 WWS01：滚轮输入的像素生产

[WWS01](cg2d_window_wheel_scroll_spec.md#window-wheel-source-state)补齐4743B0完整89条231B、四E8/两目标/六跳两RET与29实际CALL；F32因子原四BYTE、已批宿主数学/原double-F32落存和转换图保持。三段修正原余量/像素Y持样跨转换、高度/上下界以及尾部flags取样和data+8再+0发布；原入口data借用/wheel零、max及position钳制/余量零、S32返回保持。两整源三段/两注释及三个类型静态差异通过；旧1533 setter/CRT/聊天父复用不改。一新F/一处置，INPUT/UIRUNTIME各一成员，两限定S零I；完整29父/间接绑定/生产消费与owner、宿主数值和滚轮手感/画面、机器ABI及运行仍待，整体未完成。

## 2026-10-10 WDF01：命中到拖动/焦点掩码

[WDF01](cg2d_window_drag_focus_spec.md#window-drag-focus-source-state)补齐472D20完整152条466B、九E8/三目标/十八跳两RET和唯一更新父CALL。四组明确原一次W32 state/phase门槛、四低BYTE消费、矩形字段/focus/count和两全窗height-width-y-x取样；W32储宽和正确坐标调用后值/held目标地址/new窗口写回保持。Read_401EC0对应原401EA0，旧219/矩形命中/更新父复用不改。两整源四组/两注释与两个类型静态差异通过，原bit800/2000和结果1/2、无新增十项上限保持；一新F/一处置，INPUT/UIRUNTIME各一成员，两限定S零I/九任务open。完整输入生产消费/所有Crypto和父/owner、机器ABI与生成BYTE访问、实际拖动焦点及运行仍待，整体未完成。

## 2026-10-10 RHL01：矩形命中到描边

[RHL01](cg2d_rectangle_hit_hover_spec.md#rectangle-hit-hover-source-state)补齐5049A0/401FC0两完整原根60条150B、六实际E8/四目标、五跳三RET。矩形根135实际CALL/102父纳盘；比较叶16CALL/三父均仅消费AL，不提升全父验收。单组四处接回现有比较器，四次原短路/读取及RNG、半开边界、负layer仍命中和可描边后S32 1保持；比较叶正确body不改，四整源仅两已有头/四调用/四注释。旧Less/绘框和悬停父复用不改；原SETLE只覆盖AL，高24位不记等价。两新F/两处置，INPUT两成员、RENDER/UIRUNTIME各一成员，三限定S零I、十任务open。完整父/所有图像和坐标生产消费、队列及alias-owner寿命、机器ABI/像素与运行仍待，整体未完成。

## 2026-10-10 HWT01：悬停文本与IME消费

[HWT01](cg2d_hover_text_consumption_spec.md#hover-text-consumption-source-state)补齐窗口6悬停消费4731E0及IME查询/描述借用459090/4590D0三完整原根237条910B、五实际E8/四目标、两IAT调用、十跳三RET。4734F0是旧822根内共用尾，仅现平台helper，不造原F；窗口6原型+36真实main绑定复用并保持，五IME实际CALL有据。两处颜色源明确低BYTE访问，存储仍S32/W32；主/副标题含NUL复制、原payload借用跨复制、逐项fresh窗口显示、IME尾和四槽清理及S32 1保持。三IME正文正确不改，四整源只两BYTE源/八注释；旧822保全历史追加完整合同，两新F/三处置，INPUT/UIRUNTIME各三成员，两限定S零I/十任务open。三API进口仍unassessed，不据调用点提升完整进口验收；全生产消费/容量和alias-owner寿命、SDK/机器ABI/实际输入显示与运行仍待，整体未完成。

## 2026-10-10 DLP01：拖动会话状态生产与清理

[DLP01](cg2d_drag_session_production_lifecycle_spec.md#drag-session-source-state)补齐475DA0/476490两个会话生产、4763B0焦点绑定、4763E0五槽重置、476410取消及476500完成六完整原根129条483B/六页、零E8/两实际寄存器回调、十三跳十一RET；116直接CALL/50父只核点与四条后邻，不代完整外父/返回消费。阶段1/3生产的原焦点/回调/来源及S16坐标写序、已有kind的两槽绑定、五槽重置保来源/坐标、取消回调后新phase到2/6及完成入口4/5到0保持。六正文正确不改，三整源仅十二定义/声明注释；四S32结果与两void当前适配保持，原残留EAX/全部父机器ABI仍待。六新F/六处置，INPUT/UIRUNTIME各六成员，两限定S零I/十任务open；全事件生产消费/全部回调和payload/alias-owner寿命、真实拖动/退出与运行待，整体未完成。

## 2026-10-10 ISP01：选物发布到回调/显示

[ISP01](cg2d_item_selection_event_pipeline_spec.md#item-selection-source-state)补齐对话29选物入口、两显示回调、列表借用与两坐标六完整原根828条2692B/十六页，43实际E8/22目标、64跳12RET。26直接CALL和八代码回调发布有据，发布不是直接CALL；旧920完整598条取代本轮仅相邻读取范围，但全部旧历史保持。五组八处对齐：主四BYTE源、伴随state BYTE/循环count先取再推进、X/Y元素与窗口取序；入口及列表正确body保，两整源六正文其余/四类型和原字段储宽保持，十二注释。原mode1/2门槛、两独立退出/占用递减/恢复回调、未清256B/独立16B cap15及原发送和绘制/结果保持。五新F/旧920追加、六处置，INPUT/UIRUNTIME各六成员、RENDER两成员，三限定S零I/十任务open；全父/间接回调、物品/宠物/占用/协议与alias-owner寿命、机器ABI/像素与运行待，整体未完成。

## 2026-10-10 SLC01：选择器局部坐标及悬停

[SLC01](cg2d_selector_local_coordinate_hover_spec.md#selector-local-source-state)补齐局部X/Y、宠物范围/索引借用及连续悬停显示五完整原根243条771B/八页、九E8五目标/26跳12RET与26直接CALL。四S32坐标表112B与现源逐项一致；八宠物判定消费者七TEST AL/一AND FF，只证明低W8，不记原EAX高24位等价。悬停父四组五处对齐：两轴window坐标先element、两flags BYTE及未饱和alpha计数BYTE；原5*W32>=256饱和255/三descriptor、第一选择器宠物号/原非负门槛与计数/索引尾序保持，四叶正确body不改。四整源仅四组五处/十注释，四类型/表项/字段储宽保持。4C6790一父两CALL首点尾RET转发原残留EAX，当前void保持，完整上行/机器ABI未据本步等价。五新F/五处置，INPUT/UIRUNTIME各五成员、RENDER一成员，三限定S零I/十任务open；全父/callee/生产消费与alias-owner寿命、实际悬停/像素与运行待，整体未完成。

## 2026-10-10 SFD01：选择器逐帧分发与装配

[SFD01](cg2d_selector_frame_binding_spec.md#selector-frame-source-state)补齐逐帧分发、两模式元素/回调绑定及首次初始化三完整原根362条1298B/七页，20实际E8/十目标、20跳十RET，五入CALL与三表48B有据。两次按钮绘制明确各自四字段取序、先Y后X及descriptor0共借，三组三处/六注释；绑定与初始化正文正确保持。原四页/step递减/-182业务钳制、非零模式槽19不写及callback0先slot18保持。已知悬停残留EAX经分发/初始化返回，两窗口回调立即MOV EAX,1/RET，局部上行不消费已有依据；全机器ABI/所有callee及alias-owner寿命、实际像素与运行仍待。旧1988/CAC18历史与旧2285/SLC01历史保持追加，两新F/三处置、三限定S零I/十任务open，整体未完成。

## 2026-10-10 SPG01：选择器页状态及显示

[SPG01](cg2d_selector_page_visibility_spec.md#selector-page-source-state)补齐玩家/宠物显示、隐藏、五页状态切换、位置发布及mode切换六完整原根323条937B/八页，19E8九目标/35跳十一RET，24入CALL与20B五case表有据。两组五处明确CC40D4低BYTE和四回调目标先于回调读取；其余五正文与原动态slot/每轮fresh宠物count、页发布/隐藏/图号及step21、模式页3切换保持。两整源只上述差异/十二注释，三类型与原六坐标表不改。旧1987/CAC18保历史追加、五新F/六处置、三限定S零I/十open；原残留EAX及7340仅AL=1不外推全ABI弃用，全外父/callee、生产消费/alias-owner寿命、生成读取/机器ABI/像素与运行待，整体未完成。

## 2026-10-10 PSP01：宠物索引到文本/预览

[PSP01](cg2d_selector_pet_preview_spec.md#selector-pet-source-state)补齐S8序号发布、五排序对映射、名称负重及同宠物预览四完整原根238条690B/六页，九E8七目标/21跳六RET，八入CALL有据。两测宽分别按原字体→文本、文本→字体取样；预览沿原replacement→previous读取，现detail helper增加取序参数，四个既有单参调用保previous→replacement。四组四处/十注释，四整源其它正文与两正确叶/五类型保持。复用BOT01已核layer BYTE消费，高24位不记整ABI等价。旧892格式与历史追加、三新F/四处置、四限定S零I/十open；全部排序对/宠物/文本/descriptor生产消费、callee/外父及alias-owner寿命、生成取样/机器ABI/预览像素与运行待，整体未完成。

## 2026-10-10 SIC01：选择器构造与对象来源

[SIC01](cg2d_selector_construction_exit_spec.md#selector-construction-source-state)补齐选择器完整构造及两个固定构造/退出包装五原根，220条1049B/八页、20E8两目标/24跳两RET。构造/共享退出各有29条136B EH；488420主chunk另有六B间隙，指令字节与跨度分开，488430为其内部共享尾，不造F。同预览十成员构造和逆序析构、flags/开关/图号/三descriptor发布保持，仅一组一处在构造入口持址供Construct/Finish。两静态对象0/1独立，退出固定借用，不重读可改preview；四包装/四helper及两独立std::atexit正确body保持，十注释。四旧历史追加、一新F五处置、四限定S零I/十open；零模板初值有据，空Xrefs不代不可变或alias闭包，完整EH/ABI/全生产消费、全应用正常owner与快速退出/真实生命周期和运行仍待，整体未完成。

## 2026-10-10 SEV01：选择器按钮事件

[SEV01](cg2d_selector_button_event_spec.md#selector-event-source-state)补齐页签、预览开关、装备开关及双方向旋转四完整原根352条1028B/八页，九E8五目标/46跳九RET，十入CALL/64B图表有据。四组四处对齐中心取样/descriptor先图号、目标先回调和循环尾selected先推进/count后取；两正确body/五类型/十caller和既有声效接口保持，八注释。原失效宠物索引false早返保开关/回调部分写、双旋转条件独立及两声/方向原余数保持。六CALL仅消费AL、四CALL不消费，未记高位或全机器ABI等价。四新F四处置/四限定S零I/十open；全生产消费、所有外父/callee/alias-owner寿命、实际按钮/声音像素及运行仍待，整体未完成。

## 2026-10-10 SAP01：属性显示与宠物页签

[SAP01](cg2d_selector_attribute_pet_tab_spec.md#selector-attribute-source-state)补齐附加属性显示与两宠物页签三完整原根391条1176B/八页、十三E8五目标/39跳六RET。24项384B属性表、两22B包装父及六原型callback数据槽有据。两组四处明确descriptor持址和三种文字坐标读取序，原24项/14行/第8换列/512B未清及GBK/MSVC格式保持；两页签正确正文保持，库存成功低AL归一、战斗成功恒0分界明确。六外层注释及一局部枚举说明，不改公开接口、类型、caller及原型绑定。旧921格式历史追加、两新F/三处置/三限定S零I十open；完整生产消费/回调分发、callee/alias-owner寿命、生成取样/实际画面交互与运行待，整体未完成。

## 2026-10-10 IER01：装备事件与回复消费

[IER01](cg2d_equipment_attribute_reply_spec.md#equipment-reply-source-state)补齐库存装备与属性回复两完整根662条2141B/十二页，61E8二十七目标/84跳五RET、七装备data引用及一个回复入CALL有据。四组四处落实两物品低BYTE/动作BYTE接口与selected先于hover写回；原拖放/使用/特殊回调/occupancy/分页/绘图及已批GBK/MSVC格式保持。属性回复正确正文保持，首field失败本根仅清used，既有提取器写首文本首字节NUL、两文本/单空格门槛与field4..27逐项写24值，原TgX分发/当前LF明文沿既有合同。四源限定差异/四注释，旧919格式历史追加、一新F两处置/四限定S零I八open；完整生产消费、callback/callee/网络发送、alias-owner寿命/生成取样与实际操作运行待，整体未完成。

## 2026-10-10 BEI01：战斗装备事件

[BEI01](cg2d_battle_equipment_interaction_spec.md#battle-equipment-source-state)补齐战斗装备与背包空槽两个完整原根764条2531B/十三页，56E8十八目标/103跳五RET。四组四处明确三BYTE及区域描边颜色/窗口/元素读取序；保原宠物禁用页、玩家拖放/卸武器、容量/未鉴定/无武器失败提示、两个Q命令和发布序，末两次独立禁图不合并。空槽正确正文保持，slot8..27/used W16，仅统计唯一玩家区。两data回调与十九空槽入CALL/后邻有据，其余十七调用点/十五外父仅来源登记。四源限定diff/四注释、旧924格式历史追加、一新F两处置/五限定S零I九open；全数据生产/动态callback/19外父和命令、callee/alias-owner寿命/SDK/生成取样、实际交互像素声音与运行待，整体未完成。

## 2026-10-10 BIP01：战斗背包事件

[BIP01](cg2d_battle_item_interaction_spec.md#battle-item-source-state)补齐战斗背包完整1175条4105B/十九页，62E8十九目标/154跳两RET和两data绑定有据。三逻辑组十二片段落实六BYTE、五grid及一区域取序，保未加8的容量标志索引/level0经验成本、十五技能/技术/级限、两Q一I格式及目标pending/直接使用发布序和原失败分支/重复禁图。BQI01目标选择/BEI01空槽真实提供者复用，正文字节不改、不重复取完整根；两整源只十二片段/两注释。旧900格式/CAB30及既有adapted/既有UI成员保历史追加，无新增F/处置；四功能新增本根、五限定S零I九open。完整生产消费/后续目标消费者/所有callee与alias-owner寿命、SDK/生成取样/实际操作运行待，整体未完成。

## 2026-10-10 IDE01：物品拖动回调

[IDE01](cg2d_item_drag_entry_callback_spec.md#item-drag-source-state)补齐物品拖动入口/回调/分类三完整根219条719B/五页，23E8十一目标/33跳八RET。十七CALL及四后邻、两个callback data持址有据，只登记外父来源。两组六处明确动作和两点击BYTE/三绘制分支descriptor持址；mode先发布/原结果透传、两次fresh分类、失效reset/取消后仍继续绘制、玩家/宠物/C44来源及两轴CryptInt序保持。入口/分类正确正文保，两整源仅六片段/六注释，DLP01真实会话及既有目标消费者保持。三新F三处置/五限定S零I九open；完整调用方/动态回调/数据生产/alias-owner寿命、SDK/生成取样/实际拖放与运行待，整体未完成。

## 2026-10-11 IMT01：物品移动及拆分

[IMT01](cg2d_item_move_request_spec.md#item-move-source-state)补齐丢物请求/直接拆分/模式间移动/数量消费四完整原根310条957B/七页，30E8十五目标/28跳八RET，十七CALL/四后邻及一个callback data持址有据。四正文正确保持，仅八合同注释；原四容量先于格式/输出重读、同mode两次fresh组读取、Ctrl和反转开关、直接source先target/拆分target先source、窗口41/38优先及转义后状态/Y-X发送序保持。原S32返回透传与数量lambda只具名副作用边界，不推完整callback ABI/互通。旧934格式历史追加、三新F四处置/三限定S零I八open；完整输入/数量/数据/缓冲区生产、callback/callee/所有外父和alias-owner寿命、SDK/生成读取/实际操作及运行待，整体未完成。

## 2026-10-11 QVC01：数量输入与确认

[QVC01](cg2d_quantity_window_callbacks_spec.md#quantity-callback-source-state)补齐数量初始化、输入激活、确认面板和增减按钮五完整原根230条714B/六页；三读取合同对齐，三正确正文保持，五定义/五声明十注释。原callback非空调用后重取窗口，关闭/声效后再次重取窗口并格式化；保两次原上限钳回及增减门槛。旧937格式历史追加、四新F五adapted，INPUT/UI各五成员/RENDER四成员，三限定S零I八open；完整取消/关闭/释放owner及全callback/ABI/运行待。

## 2026-10-11 WCL01：共享关闭/前置按钮

[WCL01](cg2d_window_close_front_release_spec.md#window-close-source-state)补齐共享关闭按钮、原端返回叶和前置元素三完整原根70条212B/三页，三正确正文保持，仅七合同注释。窗口42关闭只置bit15/声57，前置只置bit13；真实资源回收复用窗口整理→FreeWindow，release返回叶原端只返回1，不能记作完整释放成功。三新F三adapted，INPUT两/UI三/RENDER两成员，三限定S零I九open；完整取消/共享调度/owner、ABI及运行仍待。

## 2026-10-11 NPI01：数字键与Backspace优先级

[NPI01](cg2d_numeric_panel_input_spec.md#numeric-input-source-state)补齐数值面板474550完整540条1675B/九页，逐键四次CryptInt短路、数字/Backspace优先级、四鼠标操作与AL0/1/3返回有据。两逻辑组五处坐标及绘制取样对齐；42图号和既有业务正文/原上限钳回保持。本根没有Enter/Esc处理，不添加退出键行为。一新F一adapted、三限定S零I八open，完整输入生产消费/owner、ABI及运行待。

## 2026-10-11 TQN01：交易面板滚动与数值结果

[TQN01](cg2d_trade_quantity_panel_spec.md#trade-quantity-source-state)旧949格式历史追加共享交易面板4B1260完整524条1693B/九页，31E8十一目标/一个间接业务callback、41跳一RET。五逻辑组六处低宽坐标/滚动标志、滚动值先元素与前一滚动框坐标取样对齐；原数值确认结果、callback哨兵适配、四格式及S32 0/2返回保持。一处置、INPUT/UI/RENDER各一成员，三限定S零I八open；完整共享生产消费/业务callback、owner/ABI及交易运行待。

## 2026-10-11 SDM01：存取金额确认与文本

[SDM01](cg2d_storage_amount_exchange_spec.md#storage-amount-source-state)补齐存取金额4ABB20及xD发送512620两个完整原根310条1075B/六页；两正确正文保持，仅四合同注释。原存/取上限、AL确认和钳回、转义后字段/Y-X发送及发送后fresh金额记录/清槽/双文本有据，xD五整数+文本各保六次容量和输出取样。旧894格式历史追加、一新F两adapted、三限定S零I八open；69CALL/58父仅局部来源，完整金额/协议生产消费与owner、ABI及互通运行待。
