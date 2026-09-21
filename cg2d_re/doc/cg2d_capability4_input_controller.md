# 能力 4 登录输入控制器

## 范围、调用者与返回域

本文只依据当前 `ida-17524 / D:\SOFTSTAR\cg2d.exe`，目标为
`sub_464E30@[0x464E30,0x46531E)`。函数大小 `0x4EE`（1262）字节，共 350 条
指令、50 个基本块，圈复杂度 36。

唯一直接 caller 是 `sub_464840@[0x464840,0x464DDD)`，唯一 callsite 为
`0x464ADF`。caller 在 `0x464AE4` 对返回值加 2 后执行五路分派；目标函数正常
返回域严格为 `{-2,-1,0,1,2}`：

| 返回值 | 产生条件 | caller 目标 |
|---:|---|---|
| `-2` | 点击登录且账号、密码均为空 | `0x464B86` |
| `-1` | 点击登录且两项恰有一项非空 | `0x464B8F` |
| `0` | 本帧没有提交或退出结果 | `0x464DA4` 默认分支 |
| `1` | 按钮、Enter 或外部键盘路径提交 | `0x464AF7` |
| `2` | 点击退出 | `0x464B5A` |

返回 0 不表示无副作用；键盘监控、焦点、输入上下文、鼠标音效和全部尾部绘制仍
可能发生。

## 完整直接目标

| 目标 | callsite |
|---|---|
| `sub_45F100` | `0x464EA9`、`0x464F29` |
| `TextMeasure` | `0x464F50` |
| `Renderer::DrawGraphic_Hover` | `0x464F89` |
| `Renderer::DrawInputText` | `0x464F93`、`0x464F9D` |
| `sub_4597B0` | `0x464FA4`、`0x464FB2`、`0x464FD1` |
| `sub_404060` | `0x46500E`、`0x465023`、`0x4650D4`、`0x4650E5` |
| `sub_534AB0` | `0x465077` |
| `_strncpy` | `0x465094` |
| `sub_401FA0` | `0x46514F`、`0x465183`、`0x4651DC`、`0x465211` |
| `CryptInt::lt` | `0x46516D`、`0x46519F`、`0x4651FB`、`0x46522D` |
| `World::PlaySe` | `0x4651BD`、`0x46524B` |
| `Input::SetContext` | `0x465267` |
| `sub_5049A0` | `0x465292`、`0x4652BD` |
| `sub_45EFD0` | `0x4652D2`、`0x4652E4` |
| `Renderer::DrawText3` | `0x46530C` |

新端鼠标坐标通过既有 `ReadLegacyMouseX/ReadLegacyMouseY` 边界读取；该边界承接
原 `sub_401FA0` 与 `CryptInt::lt` 的地址中性适配，不为本函数新建鼠标状态。

## 栈上按钮与按钮结果

入口建立两个独立的 7×W32 栈记录，布局为
`{x,y,width,height,normalImage,activeImage,clickedImage}`：

| 按钮 | 七个 W32 |
|---|---|
| 登录 | `{240,421,64,16,243048,243050,243049}` |
| 退出 | `{332,421,64,16,245153,245155,245154}` |

先调用登录按钮的 `sub_45F100`，只检查返回值 bit 0；命中后把登录记录
`normalImage` 改为 `243049`，且本帧不再查询退出按钮。两项均非空时直接返回候选
1，不执行非法字符检查；恰一项非空为 -1，两项均空为 -2。只有登录按钮未命中
时才查询退出按钮；命中后把退出记录 `normalImage` 改为 `245154` 并产生结果 2。

登录按钮两项均非空时，`0x464ED9..0x464EF0` 分别读取账号
`gInputContext9A5678+0x12C`、密码 `gInputContext9A5960+0x12C`，并在对应文本
下标写 NUL。IDA 当前把该字段误标为 `cursor`，但结构布局已经确认：`+0x128` 是
`caretByteOffset`，`+0x12C` 才是 `totalByteLength`。实现只使用后者，绝不以
`+0x128` 代替。

## 无条件 IME 与输入框绘制

按钮阶段之后不设任何门禁：

1. 从当时的 `gCurrentInputData+0x13C` 读取完整 W32 字体类型，以
   `TextMeasure(gImeComposition,fontType)` 测宽。
2. 测宽返回后重新读取 `gCurrentInputData`，依次取得 Y、X 和字体类型；从唯一
   `gFontMetrics` 读取该字体的 `doubleHeight`。
3. 以 W32 回绕计算 `x-1`、`y-1`、`x+compositionWidth`、
   `y+doubleHeight`，只把四项低 16 位传给
   `DrawGraphicHover(...,211,0,1)`。
4. 严格先绘制密码 `gInputContext9A5960`，再绘制账号
   `gInputContext9A5678`。

当前上下文空值、字体索引域外等异常输入均没有 caller 门禁；实现不增加钳制或
替代上下文。

## 键盘顺序与外部键盘

键序严格为：先读键 9；只有其结果为零才读键 13。两者均未触发时不进入焦点键盘
分支。任一触发后读取 `gValue9A6E20`：

- 焦点为 0 时再次读取键 13。只有密码、账号依次非空，且按密码、账号顺序调用
  `IsRejectedInputText_404060` 后均合法，才把结果覆盖为 1，并依次按两项各自
  `totalByteLength@+0x12C` 写 NUL；失败时把焦点改为 1。
- 焦点为 1 且 `gMonitorKeyboard==1` 时，以实时
  `platform::gLegacyWindow` 和一个未初始化的 `char[68]` 调用
  `sub_534AB0`。返回 1 时按普通 `strncpy` 语义向密码 `text` 复制 288 字节，
  随即以无界 `strlen` 只更新密码 `totalByteLength@+0x12C`；密码
  `caretByteOffset@+0x128` 保持原值。随后按密码、账号顺序执行非空和非法字符
  检查；成功时覆盖结果为 1，按密码、账号各自 `+0x12C` 写 NUL，把焦点写 1
  后跳到键处理尾部。返回 -1 时把 `gMonitorKeyboard` 写 0，其他返回值不改监控
  状态；该外部键盘分支的公共尾始终把焦点写 1。
- 焦点为 1 且 `gMonitorKeyboard!=1` 时，把焦点写 0。
- 其他焦点值不进入上述两路，且不被安全归一化。

按钮先产生的 -2、-1、0、1 或 2 都可能被键盘成功路径覆盖为 1。

`sub_534AB0@[0x534AB0,0x534B7E)` 的外部键盘包装层已在
`src/platform/LegacyExternalKeyboard.cpp` 还原，沿用既有地址中性边界
`std::int32_t Sub_534AB0(std::uintptr_t window,char* output)`。包装层通过外部
`idguardlibr.dll` 对话框返回 -1、0 或 1；仅返回 1 时无界复制结果旧字节串。
证据见 [`cg2d_external_keyboard_wrapper.md`](cg2d_external_keyboard_wrapper.md)。
`DialogFunc@0x534B80` 消息回调也已还原；未闭合项为四个真实对象依赖所承接的
`JoyparkKeyboard` 对象闭包，以及外部 DLL 的真实资源和行为。未提供替代键盘
界面，外部键盘路径及最终链接仍未闭合。

## 鼠标、焦点与尾部绘制

键处理之后只在 `gMonitorKeyboard==0` 时执行以下块：

1. 账号命中范围为严格开区间 `(187,559)×(358,384)`。X 下界、X 上界、Y 下界、
   Y 上界各自独立调用一次 `ReadLegacyMouseX/Y`；再检查 `edgeButtons` 低字节 bit 0。
   命中时调用 `PlaySe(51,320,240)` 并把焦点写 1。
2. 密码命中范围为严格开区间 `(187,559)×(389,415)`，保持同样四次独立鼠标读取
   与 bit 0 检查；命中时播放同一音效并把焦点写 0。两个命中判断是相邻的独立
   `if`，不是 `else if`。
3. 不检查焦点范围，直接执行
   `SetContext(gInputContextFocusTable[gValue9A6E20])`。
4. 依次调用账号 `Sub_5049A0(182,354,554,380,206)` 和密码
   `Sub_5049A0(182,385,554,411,206)`。坐标仍从原 W16 全局符号扩展后按 W32
   加减得出；表中数值是能力 4 初始化后的正常值。

无论监控状态如何，尾部都严格执行：

1. `sub_45EFD0(login,207,0,0,0)`；
2. `sub_45EFD0(exit,207,0,0,0)`；
3. `DrawText3(460,462,0,gPaletteCapability>=1?3:0,"7.1.00",0)`；
4. 返回当前结果。

## 异常域与实现边界

- 所有 `strlen` 都保留无界 C 字符串读取；`strncpy(...,288)` 保留标准填充及可能
  不补终止零的语义，后续 `strlen` 不加容量上限。
- 所有按 `totalByteLength` 的文本写入都不检查负值、容量或与实际 NUL 的一致性。
- `gCurrentInputData`、字体类型、焦点表索引均不增加空值或范围门禁。
- 坐标加减按无符号位型执行 W32 回绕；IME 框提交时只保留低 16 位。
- 未初始化 `char[68]` 不预清零；输出有效性完全沿用 `sub_534AB0` 契约。
- 本项复用 `LoginInput`、`LoginMessage/SceneDependencies`、`KeyState`、
  `LegacyWindow`、`Startup`、`WindowProcedureState`、`Font`、`Palette` 与
  `TextPendingQueue` 的唯一状态和入口；没有新增翻译单元或外部依赖。
- `D:\Source\STONAGE` 仅允许帮助理解“表单状态与外部输入分层”的架构候选；本文
  的地址、顺序、字段、返回值和验收结论均不以 STONAGE 为行为证据。
