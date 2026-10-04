# cg2d IME 组合串处理还原

## 函数与调用证据

- 原函数为 `sub_459580@[0x459580,0x4596C8)`，大小 `0x148`，IDA 统计 97 条指令、19 个基本块，原型为 `char __cdecl(__int16)`。
- 唯一代码引用是 `WIN_PROC@0x46728D`。`0x467288/0x46728C` 把 x86 `lParam` 的完整 32 位压栈，被调函数按 `__int16` 只消费低 16 位；`0x467292` 只回收参数，随后 `0x467295` 清零返回寄存器，因此忽略本函数返回值。
- 四个标志按 `GCS_RESULTSTR(800h)`、`GCS_COMPSTR(8)`、`GCS_COMPATTR(10h)`、`GCS_CURSORPOS(80h)` 的顺序独立判断；前一分支不会阻断后一分支。

## 分支行为

- `0x459590..0x459605`：先以空缓冲查询结果串 ANSI 字节数，按 signed `LONG >= 256` 钳为 255，再读入局部 256 字节缓冲并写终止零；随后按 `strlen` 结果再次钳为 255，逐字节调用 `input::AppendChar_44ADB0`。
- `0x459610..0x459645`：同样查询并钳制组合串长度，第二次读取写入唯一 `gImeComposition[256]`，然后在所得下标写终止零。
- `0x459656..0x45968A`：同样查询并钳制属性长度，第二次读取写入唯一 `gImeAttributes[256]`，然后在所得下标写零。
- `0x45969D..0x4596B7`：游标位置只查询一次，把完整 `LONG` 写入原 `0x73EFE0` 对应的 `gImeCompositionCursorPosition`，并把 `input::gValue626C0C` 写为 20。
- 不含游标标志时返回 flags 低 8 位；含游标标志时返回该次 API 结果低 8 位。所有 `ImmGetCompositionStringA` 调用均读取唯一 `gImeContext`，不增加空句柄分支。

## 当前范围与RPLAN.24（2026-10-05）

- 早期源码曾拒绝负查询长度，并预清局部结果缓冲；当时差异见Git命名快照cg2d-re-active-initializer-order-source-20261005的本页/源码。该处理与现行RPLAN.24不符，已按[EIC01局部SPEC](cg2d_ime_composition_alignment_spec.md#ime-composition-source-closure)撤销，不再作为当前批准差异。
- 原指令使用signed jl跳过256上限钳制，没有负值拒绝。新端继续第二次API及第一次查询长度下标终止；局部结果不预清，全局两缓冲同样不额外清零。strlen边界/追加数量和循环索引为S32，终止下标保留signed LONG，不由宿主size_t扩宽。
- 原有255上限、四标志独立处理、七次共享HIMC读取、逐字节AppendChar、游标完整LONG/相位20及低U8返回保持。窗口既有低W16调用和弃返回不改。
- 负查询长度导致越界、读取失败留下未写局部字节等沿RPLAN.24不专项处理，不模拟原x86故障、不声称x64访问安全或故障等价；本组静态结论仅覆盖适用正常/原上限/多标志路径。
- 97条本体及10条窗口邻窗/共享槽证据见[EIC01证据](cg2d_ime_composition_alignment_evidence.json)。完整输入/IME消费者、候选、焦点/绘制、间接别名和SDK/运行未验；生命周期仍见[cg2d_ime_lifecycle](cg2d_ime_lifecycle.md)，不由本组提升父的完整门禁。
