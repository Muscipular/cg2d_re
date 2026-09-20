# cg2d 输入光标闪烁与排队还原

## 函数边界与证据

- `sub_44A410@[0x44A410,0x44A535)`，大小 `0x125`（293 字节），IDA 统计 83 条
  指令、12 个基本块，原型为 `void __cdecl()`。
- 三个直接目标分别为 `sub_459090@0x459090`、`Font::measure@0x44B2E0` 与
  `Renderer::DrawText2@0x456670`。`sub_459090@[0x459090,0x45909F)` 固定先调用
  `GetKeyboardLayout(0)`，再把返回布局传给 `ImmIsIME`。
- `asc_554924@0x554924` 的原始字节为 `A2 78 00`；实现按该三字节常量排队，不作
  Unicode 或 UTF-8 替换。
- `stru_626AA0@0x626AA0` 与 `dword_C9A96C@0xC9A96C` 的映像初值均为零；新端分别
  以唯一 `gInputContext626AA0` 与 `gValueC9A96C` 承接，只保留地址型命名。

## 控制流与采样顺序

入口在 `0x44A410..0x44A42B` 读取当前上下文：空指针立即返回；当前上下文恰为
`gInputContext626AA0` 且 `gValueC9A96C==0` 时也立即返回。这两条路径都不推进闪烁
计数。其余路径按 signed W32 检查 `gValue626C0C`：小于 20 时不绘制，仅走尾部计数；
至少 20 时按以下顺序选择绘制：

1. `0x44A43E` 查询当前键盘布局是否为 IME。
2. 非 IME，或 IME 但 `gImeComposition[0]==0`，绘制基础光标。
3. IME 组合串非空时再读 `gImeAttributes[0]`；首属性非零不绘制，首属性为零才进入
   组合光标路径。
4. 组合路径在测宽前只采样一次当前上下文，依次保存颜色、完整字体、模式低字节加一
   和 Y；随后读取完整 `gImeCompositionCursorPosition` 并取得组合串前缀宽度。
5. 测宽阶段之后重新读取 `gCurrentInputData`，使用该次上下文的完整字体和 X 计算最终
   X；排队参数中的 Y、模式、字体、颜色仍使用测宽前保存值。新端若重读得到空指针，
   只跳过本次绘制，仍推进计数。

基础路径只采样一次绘制时的当前上下文。两条绘制路径都把坐标截为低 16 位，把模式、
字体、颜色截为低 8 位，`value7=0`，并忽略 `DrawText2_456670` 返回值。模式的低字节
先按 W8 加一，保留 `0xFF+1 -> 0` 的回绕。

## 坐标与计数

- 基础 X 为 `W32(x - trunc_zero(doubleWidth/2))`。
- IME X 为
  `W32(measuredWidth + W32(postMeasureX - trunc_zero(postMeasureDoubleWidth/2)))`；
  `doubleWidth` 来自测宽后重新读取上下文的完整字体槽。
- 每条通过入口门禁的路径最终都在 `0x44A51A` 重新读取计数，按 W32 加一并先写回；
  新值按 signed W32 至少为 40 时，再清零。空上下文和特殊上下文禁用路径不执行该
  尾部逻辑。

## 新端安全边界

- 基础路径字体和 IME 测宽后字体会直接索引 `gFontMetrics`，因此必须位于 `0..13`；
  无效时只跳过本次绘制，尾部计数照常推进。IME 测宽前字体不直接索引该表：字体
  无效或光标位置非正时沿用 `Font::measure` 的零宽语义，不探测组合缓冲。
- IME 测宽只在光标位置为正且测宽前字体有效时调用 `Font::measure`。调用前在固定
  `gImeComposition[256]` 内按相同的单/双字节推进模拟实际读取，避免缺失 NUL 或末尾
  高位字节导致越界；其余路径宽度为零，不要求 NUL。
- 所有 X 运算及闪烁计数递增均使用无符号位模式实现 W32 环绕，避免 C++ signed
  overflow；有符号宽度除以二使用向零截断。

本项不实现 `World::RenderInput`、IME 候选列表、输入事件或 SDL 消费器。
