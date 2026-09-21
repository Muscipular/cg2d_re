# 能力 3 登录输入控制器

## 范围与返回域

本文只依据当前 `ida-17524 / D:\SOFTSTAR\cg2d.exe`，目标为
`sub_45FA70@[0x45FA70,0x45FE9C)`。函数大小 `0x42C`（1068）字节，共 49 个
基本块，圈复杂度 33；唯一直接 caller 是 `sub_45F190`，调用点为 `0x45F699`。
新端入口为 `cg::login::ProcessCapability3Input_45FA70(std::int32_t mode)`。

正常返回域为 `{-1,0,1,2}`：主按钮在 `mode==0` 时命中且任一输入为空返回
`-1`，两项均非空返回 `1`；副按钮在 `mode==0` 时命中返回 `2`；其余路径保留
`0`。后续 Enter 或外部键盘提交成功可把先前的 `-1`、`0` 或 `2` 覆盖为 `1`；
失败路径不清除已有结果。

## 按钮阶段

入口建立两个独立的 28 字节栈记录，固定布局为
`{x,y,width,height,normalImage,hoverImage,clickedImage}`：

| 按钮 | 七个 W32 |
|---|---|
| 主按钮 | `{240,438,64,16,243048,243050,243049}` |
| 副按钮 | `{332,438,64,16,245153,245155,245154}` |

严格先调用主按钮的 `Sub_45F100`。只有 bit 0 命中且 `mode==0` 才进入主按钮
分支：先把 `normalImage` 改为 `clickedImage`，再按密码
`gInputContext9A5678`、账号 `gInputContext9A5960` 顺序执行无界 `strlen`。任一
为空产生 `-1`；两项均非空产生 `1`，并严格先按账号、再按密码各自的
`totalByteLength@+0x12C` 写 NUL。未进入主按钮分支时才查询副按钮；其 bit 0
命中且 `mode==0` 时更新常态图号并产生 `2`。因此 `mode!=0` 时仍会查询两个
按钮，但不提交按钮结果。

## 文本与 IME 绘制

按钮之后仅当 `mode<2` 执行本段：

1. 从当时的 `gCurrentInputData+0x13C` 读取字体类型并调用
   `TextMeasure(gImeComposition,fontType)`。
2. 测量返回后重新读取 `gCurrentInputData`；`0x45FB9E..0x45FBB2` 的字段读取
   顺序严格为 Y、X、字体类型。
3. 以 W32 回绕计算 `x-1`、`y-1`、`x+width`、`y+doubleHeight`，把四项低
   16 位传给 `DrawGraphicHover(...,211,0,1)`。
4. 严格依次绘制 `gInputContext9A4D20`、`gInputContext9A5960`、
   `gInputContext9A5678`。第三个上下文不是可省略的共享副本。

本段不为当前上下文空值或字体索引增加门禁。

## Tab、Enter 与外部键盘

键序严格为先读取 Tab 9，仅当未触发时才读取 Enter 13。任一触发后按本函数
私有当前索引 `dword_9A6EA0` 分派：

- 索引 1：再次读取 Enter。只有账号非空、密码非空、账号合法、密码合法时才
  覆盖结果为 `1`，再按账号、密码顺序在各自 `totalByteLength@+0x12C` 写
  NUL，并保持请求值为 `-1`；Tab 单独触发或 Enter 校验任一步失败时请求索引
  0。
- 索引 0 且 `gMonitorKeyboard!=1`：请求索引 1。
- 索引 0 且 `gMonitorKeyboard==1`：以实时窗口和未初始化的 `char[68]` 调用
  `Sub_534AB0`。仅返回 1 时，按标准 `strncpy(...,0x120)` 语义把缓冲复制到
  密码 `gInputContext9A5678.text`，随后用无界 `strlen` 只更新密码
  `totalByteLength@+0x12C`，不更新 `caretByteOffset@+0x128`。再按账号非空、
  密码非空、账号合法、密码合法的顺序检查；成功时覆盖结果为 1，并按账号、
  密码顺序写终止 NUL。无论外部键盘返回值及校验结果如何，本分支最终请求索引
  0；返回 -1 也不关闭 `gMonitorKeyboard`。
- 其他索引不处理，也不归一化。

`Sub_534AB0` 的外部键盘包装层已在 `src/platform/LegacyExternalKeyboard.cpp`
还原，仍沿用既有地址中性接口；具体证据见
[`cg2d_external_keyboard_wrapper.md`](cg2d_external_keyboard_wrapper.md)。
`DialogFunc@0x534B80` 消息回调也已还原；未闭合项为四个真实对象依赖所承接的
`JoyparkKeyboard` 对象闭包，以及外部 DLL 的真实资源和行为。外部键盘路径及
最终链接仍未闭合。

## 鼠标、上下文与尾部

键盘阶段之后，只有 `gMonitorKeyboard==0 && mode==0` 执行鼠标命中，且严格先
密码、后账号：

1. 密码使用 `gInputContext9A5678X/Y` 的 signed W16 值，调用
   `Sub_5049A0(x-5,y-4,x+227,y+22,206)`；命中请求索引 1。
2. 账号使用 `gInputContext9A5960X/Y` 调用同一偏移；命中请求索引 0，允许覆盖
   前一个请求。

上下文更新保持原状态机：`mode>0` 时调用 `SetContext(nullptr)` 并把本函数私有
latch `dword_9A6E9C` 写 1，不改当前索引；`mode<=0` 时，若 latch 非零则忽略
本帧请求并恢复当前索引，否则只接受 `[0,2]` 的请求。实际切换严格按焦点表取
指针、调用 `SetContext`、清 latch、写当前索引的顺序执行。两个私有 W32 均由
本实现单元唯一持有并按 PE 虚拟零填充语义初始化为 0；不与能力 4 状态合并。

尾部无条件依次执行：

1. `Sub_45EFD0(primary,207,0,0,mode!=0)`；
2. `Sub_45EFD0(secondary,207,0,0,mode!=0)`；
3. `DrawText3(540,424,0,gPaletteCapability>=1?3:0,"7.1.00",0)`；
4. 返回当前结果。

## 异常域与证据边界

- 所有 `strlen` 保留无界读取；`strncpy(...,0x120)` 保留标准填充和源过长时不
  补终止零的语义，随后的 `strlen` 不增加容量限制。
- 原 68 字节外部键盘局部缓冲不预清；有效性完全依赖 `Sub_534AB0` 契约。
- 所有按 `totalByteLength` 的写入不检查负值、容量或与实际 NUL 的一致性；绝不
  错用 `caretByteOffset`。
- 当前上下文、字体索引、焦点索引及焦点表访问不增加空值或范围门禁；坐标加减
  保持 W32 回绕，IME 框坐标只转交低 16 位。
- 本实现复用唯一的三个输入上下文、焦点表、`gMonitorKeyboard`、IME、字体、
  `Sub_534AB0`、`Sub_45F100`、`Sub_45EFD0`、`Sub_5049A0`、
  `DrawGraphicHover`、`DrawInputText` 与 `DrawText3` 边界，没有建立副本。
- `D:\Source\STONAGE` 不参与本文地址、字段、顺序、状态或验收结论，不能作为
  本函数的行为证据。
