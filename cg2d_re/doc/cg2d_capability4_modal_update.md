# 能力 4 删除确认模态更新证据

## 范围与调用关系

本文只记录当前 `ida-17524` 中 `D:\SOFTSTAR\cg2d.exe` 的
`sub_464450@[0x464450,0x464831)`：大小 `0x3E1`（993）字节、321 条指令、
35 个基本块，原型为 `int(const char*)`，返回值只可能为 0、1、2。唯一 caller
是 `sub_464840` 在 `0x464CF2` 的直接调用；本项不接该 caller。

函数没有键盘输入分支。13 个调用点对应 9 个唯一目标：

| 目标 | 调用地址 |
|---|---|
| `strchr` | `0x4644C5`、`0x4644F4` |
| `TextMeasure` | `0x4644D9`、`0x464509` |
| `sub_5044E0` | `0x46459D` |
| `sub_5049A0` | `0x4645F1`、`0x464642` |
| `World::PlaySe` | `0x4646A6` |
| `Yobi::MarkDeath` | `0x4646BC` |
| `CryptInt::gte` | `0x4646F2`，新端复用 `IsYobiActionReady` |
| `Renderer::DrawText` | `0x464752` |
| `Renderer::DrawGraphic` | `0x4647C3`、`0x46481A` |

## 文本与初始化

调用前提沿用既有 LoginMessage：`text` 非空、NUL 终止，且包含 NUL 的总长度不
超过 256 字节。函数每帧把全文复制到一个 256 字节缓冲，只把 LF (`0x0A`) 原地
改为 NUL；首、尾或连续 LF 均保留为空行。每行依次调用 `TextMeasure(line,0)`，
得到 `lineCount` 与 `maxWidth`。绘制阶段不保存行指针表，而是从缓冲起点开始，
每绘一行后重新 `strlen(line)+1` 前进。

本模块的匿名状态与原地址一一对应：

| 原地址 | 新端状态 | 初值/写入 |
|---:|---|---|
| `0x9A57E4` | 初始化锁 | PE 零填充；复位写 0，首次更新先写 1 |
| `0x55C580` | W32 哨兵 | 文件初值 -2；每次首次初始化再写 -2 |
| `0x9A4EA8` | width | PE 零填充 |
| `0x9A5448` | height | PE 零填充 |
| `0x9A4B78` | x | PE 零填充 |
| `0x9A4E7C` | y | PE 零填充 |
| `0x9A6EBC` | action | 原 x86 指针零填充；新端为原生 `YobiAction*` |

锁为 0 时严格先写锁 1、再写哨兵 -2；随后按 x86 W32 回绕计算：

```text
width  = ((maxWidth + 95) / 64) * 64
height = (lineCount - 1) * (fontHeight + 2) + 96
x      = (640 - width) / 2
y      = (480 - height) / 2
```

状态严格按 `width→height→x→y` 写入，再调用
`Sub_5044E0(x,y,width,height,4,1)` 并保存完整原生指针，包括 `nullptr`。创建失败
不回滚初始化锁。descriptor 固定为 16 字节：8 个 `00`、4 个 `FF`、
`00 04 00 00`。

## 命中、点击与返回矩阵

每帧始终先执行左矩形命中，再执行右矩形命中，两次均不短路。两边各自在调用前
重新读取字体高度和布局状态：

```text
baseY       = y + (lineCount - 1) * (fresh fontHeight + 2)
leftCenter  = x + width / 3
rightCenter = x + (2 * width) / 3
rect        = [center-33, center+33) × [baseY+61, baseY+78)
```

两次命中结束后才读取一次 `gValueD68D9C` 的低 8 位 bit 0。左点击先选内部值 0，
右点击随后覆盖为 1；因此双命中点击按右侧处理。命中点击先播放
`PlaySe(79,320,240)`，再重读 action；action 非空时先 `MarkDeath`，再清为
`nullptr` 并立即返回。action 为空时两者均跳过并直接返回；空 action 仍不阻止
点击返回。

| 条件 | 返回 |
|---|---:|
| 左命中且点击，右侧未覆盖 | 1 |
| 右命中且点击，包括双命中 | 2 |
| 无点击且 action 为空 | 0 |
| 无点击且 action 未 ready | 0 |
| 无点击且 ready，完成绘制 | 0 |

## ready 绘制与边界

ready 后正文横坐标为 `x+(width-maxWidth)/2`；每行纵坐标为
`y+30+i*(fresh fontHeight+2)`，文本坐标显式截取低 16 位后调用
`DrawText(x,y,2,0,0,line,0,0)`。正文全部完成后依次绘左、右按钮，图形层为
223、`value4=0`：左图号为悬停 `243050`/普通 `243048`，右图号为悬停
`245328`/普通 `245326`。每个按钮都重新读取字体高度和布局状态；正文、左图、
右图之间不跨外部调用缓存整套布局。

所有 W32 加、减、乘均通过本模块位型 helper 保留 x86 回绕；action 始终使用原生
指针宽度。固定 caller 必须保证文本前提，本函数不定义非法输入的拒绝、截断或
错误返回。旧编码到 UTF-8 后的字节容量与几何策略仍由未来能力 4 主场景负责，
本项不提前接 caller 或扩展转换策略。
