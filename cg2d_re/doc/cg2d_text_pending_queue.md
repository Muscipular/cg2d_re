# 文本待提交链还原

## 目标与证据

- `Renderer::DrawText3@[0x456560,0x456668)`：大小 `0x108`，79 条指令、
  5 个基本块，cdecl 原型为
  `int32 DrawText3(int16 x,int16 y,int8 renderMode,int8 color,const char* text,int8 value7)`；
  15 个直接调用点来自 14 个 caller，唯一直接 callee 为
  `Renderer::QueueText@0x521A30`。
- `Renderer::DrawText2@[0x456670,0x45677B)`：大小 `0x10B`，80 条指令、
  5 个基本块，cdecl 原型为
  `(int16,int16,int8,int8,int8,const char*,int8)`。
- `Renderer::DrawText@[0x456780,0x456891)`：大小 `0x111`，81 条指令、
  5 个基本块，cdecl 原型为
  `(int16,int16,uint8,int8,int8,const char*,int8,int8)`。
- `Renderer::QueueText@[0x521A30,0x521A98)`：大小 `0x68`，22 条指令、
  3 个基本块。原 x86 首参数是文本记录指针；x64 适配为一基 `uint32_t` 令牌。
- 文本记录位于原 `GraphicCache4[4096]@0x62ECB8`，步长 `0x110`；六槽尾队列
  位于 `[0x62ECA0,0x62ECB8)`；字体层记录位于 `fontLayer@0x558F50`。
- `QueueText` 复用原 `[0xFF4EB8,0x1020EB8)` 的既有
  `LegacyPendingStorage`，不建立第二份图形队列。

## 文本记录与令牌

`TextPendingRecord` 严格保持原偏移：`+0/+2` 为 S16 坐标，`+4/+5/+6/+7`
依次为颜色、字体、模式和值 7，`+8` 由 `DrawText2/DrawText3` 固定写 `0xF9`、
由 `DrawText` 写入第八参数的低 8 位，`+9` 为 255 字节文本，`+0x108` 为本目标
不写的 S32 字段，`+0x10C` 为下一项令牌。记录总长 `0x110`。

原 `dword_D68BEC@0xD68BEC` 是独立 W32 悬停文本索引：PE 映像无文件后备并
零填充，故唯一新端 owner `gHoveredTextPendingIndex` 初值为 0。全程序只有
`sub_507140` 的 `0x507264` 写 -1、`0x507341` 写当前零基文本记录下标，以及
`sub_45D7A0` 的 `0x45D7B8` 读取。生产者 `sub_507140` 尚未实现，本项不从
鼠标坐标或文本记录反推该值。

`FindClickedTextIndex_45D7A0` 对应原 `[0x45D7A0,0x45D7CF)`。它严格先
单次采样旧鼠标 `edgeButtons` 的低字节，左键 bit 0 未置位时返回 -1；再判断
signed W32 `count <= 0`；通过后单次采样上述悬停索引，按顺序返回 `values`
中的首个相等下标，否则返回 -1。函数没有直接 callee、不会修改输入或悬停状态，
也不检查空指针与数组容量；bit 0 置位且正计数属于调用者必须满足的有效内存前提。

`gGraphicCache4Queue` 保留六个 W32 槽，但不存 x64 指针：0 表示空链，令牌
`i+1` 表示 `gTextPendingRecords[i]`。字体层六项仍各占四字节，映像初值依次为
`D2 00 00 00`、`D4 00 00 00`、`D8 00 00 00`、`DF 00 00 00`、
`E2 00 00 00`、`E6 00 00 00`。

## `DrawText2` 写入与排队

入口把 `platform::gPendingGraphicCount2` 的 W32 按 signed32 解释；合法范围为
`0..4095`。原 x86 会让负计数形成数组前地址，新端对负数和至少 4096 的值返回
`-2`。模式按 signed8 解释，只接受 `0..5`，避免原端域外索引。

合法调用严格按 `x、y、mode、color、font、value7、0xF9` 的顺序写字段，随后
只复制文本有效字节和 NUL，不清文本余部，不写 `field108`，也不整项清零。新端
额外要求文本指针非空，并从索引 0 开始逐字节读取、每次读取后立即判断 NUL；
最多读取索引 `0..254`，正文最多 254 字节。到长度 255 仍未遇到 NUL 时返回
`-2`，不要求首个 NUL 之后的内存可读，避免原内联 `strcpy` 的无界访问。

复制后读取当前模式的尾令牌，再把新项 `nextToken` 写 0。空链先把尾槽设为新项
令牌，再以对应字体层调用 `QueueText`，忽略其返回值；之后重新读取文本计数，
以该次 W32 的 signed32 值返回并按 W32 加一。非空链先把旧尾的 `nextToken`
连到新项，再重新读取文本计数，以该重读索引的一基令牌更新尾槽，返回重读值并
按 W32 加一。

两个容量彼此独立：即使空链调用 `QueueText` 因图形队列已满而返回 `-2`，尾槽
仍已建立，文本计数仍递增。正常单线程下重读值等于入口索引；实现不提供并发
同步。损坏尾令牌大于 4096 时安全返回 `-2`，不会用它访问数组。非空链连接后若
重读文本计数已越出合法索引域，也安全返回 `-2`，不再生成域外尾令牌；此前已按
原顺序完成的新记录写入和旧尾连接保持不回滚。

`DrawInputText_4568A0` 的多行路径复用本入口逐段入队；单行路径则按原
`0x456A8D..0x456C4C` 直接写同一 `gTextPendingRecords`、六个尾令牌和字体层状态，
不写 `marker` 与 `field108`。初始记录用入口计数，裁宽两分支及非空续尾各按原点
重读计数，末尾递增共享当前值；三个完整模式索引分别读取当前字段。
当前行为见[ITD01](cg2d_input_text_alignment_spec.md#input-text-draw-source-closure)；
本组只改输入绘制，DrawText2/QueueText自身差异与完整消费/寿命仍待。

## `DrawText3` 固定字体包装

`DrawText3_456560` 与 `DrawText2_456670` 的唯一语义差异是少一个字体参数，并把
记录的 `fontType` 固定写为 0。其唯一实现严格收敛为
`return DrawText2_456670(x, y, renderMode, 0, color, text, value7);`，不复制队列
逻辑，也不转发到按无符号模式索引的 `DrawText_456780`。

因此它完整继承 `DrawText2` 的写入与排队顺序：
`x、y、mode、color、font0、value7、0xF9、含 NUL 文本、next`，尾槽按 signed8
模式选择，空链调用
`QueueText`，非空链续接旧尾，末尾重读 W32 文本计数、加一并返回旧值；也继承
负计数、非法模式、空或超长文本、损坏尾令牌与续链后损坏重读计数的安全门禁。

## `DrawText` 差异

`DrawText_456780` 与 `DrawText2_456670` 使用同一组记录、尾队列、字体层和两个
计数器，字段写入、文本复制、首链调用 `QueueText`、续链、计数重读与返回旧零基
索引的顺序一致。严格差异只有两处：模式参数按无符号低 8 位解释并只接受 `0..5`；
记录 `+8` 写入第八参数 `marker` 的低 8 位，而不是固定 `0xF9`。

其安全门禁与 `DrawText2` 相同：入口文本计数按 signed32 仅接受 `0..4095`，文本
指针不得为空，逐字节只扫描索引 `0..254` 寻找 NUL，尾令牌必须为合法的一基令牌
或空令牌。字体参数不作范围门禁；`QueueText` 失败也不回滚已经建立的文本链和随后
发生的文本计数推进。`login::detail::DrawText` 仅保留为调用该唯一实现的薄适配。

## `QueueText` 写入

入口先把 `platform::gPendingGraphicCount` 的 W16 按 signed16 解释；负值或至少
4096 时返回 `-2`。容量检查通过后，再要求 `headToken` 严格位于 `1..4096`；
损坏令牌返回 `-2`，且不写任何队列字段。合法路径只按以下顺序写入初始索引
对应项：

1. 写 `GraphicCache3.layer`；
2. 重读 W16 计数，把位模式写入同一 `GraphicCache3.cacheIndex`；
3. 写 `imageNo=-1`、`value4=0`、`batchType=4`、`reference.textToken=headToken`（后续 x64 联合字段）；
4. 再次读取 W16 计数，以 signed16 扩展返回，并把该 W16 加一写回。

它不写图形记录的坐标、描述符或尾字节，也不写 `GraphicCache3+3`。通过范围
检查的 `headToken` 按 W32 位模式原样保存。

## 当前未闭合项

主循环依赖的 `Renderer::CacheReset@[0x456D30,0x456D41)`（声明名
`ResetGraphicCache_456D30`）已闭合：原函数 17 字节、7 条指令、1 个基本块，
字节为 `57 B9 06 00 00 00 33 C0 BF A0 EC 62 00 F3 AB 5F C3`；
`ECX=6`、`EAX=0`、`EDI=0x62ECA0` 后执行 `rep stosd`，恰清
`[0x62ECA0,0x62ECB8)` 的六个 W32 尾槽并返回 W32 零。新端只清
`gGraphicCache4Queue`，不清文本记录或任何计数。

最终队列消费入口 `Renderer::DrawAllQueued@0x520DD0` 仍只有真实声明，
其 case 4 父分发未接；后续 `RenderTextToSurface_456D70` 已完整实现，
沿同一一基 token 遍历记录，不改变本节生产者或门禁。
具体两分支、字体和共享 Back DIB 见[文本消费 SPEC](cg2d_sdl_presenter.md)。
文本提交仅静态接通，实际字形、最终画面和整个客户端仍未运行验收。
