# cg2d 登录控件命中与鼠标位打包证据

## 范围

本文只记录当前 `ida-17524/D:\SOFTSTAR\cg2d.exe` 中
`sub_5036F0@[0x5036F0,0x50377E)`：共 55 条指令、20 个基本块。其唯一
caller 为 `sub_45CA20@0x45CA20`，共有两处调用；本项不实现或接线
`sub_45CA20`。相邻 `sub_503780` 的独立还原见
[`cg2d_login_control_draw.md`](cg2d_login_control_draw.md)。

## 参数与固定布局

函数接收一个记录指针并返回 W32 位域。空指针在读取记录前直接返回 0。记录
严格由 8 个 signed W32 字段组成，总长 `0x20`：

| 偏移 | 新端字段 | 语义 |
|---|---|---|
| `+0x00` | `drawX` | 基础图绘制 X |
| `+0x04` | `drawY` | 基础图绘制 Y |
| `+0x08` | `left` | 命中矩形左边界 |
| `+0x0C` | `top` | 命中矩形上边界 |
| `+0x10` | `width` | 宽度 |
| `+0x14` | `height` | 高度 |
| `+0x18` | `normalImageNo` | 常态基础图号 |
| `+0x1C` | `activeImageNo` | 激活基础图号 |

`LoginSceneControl32` 的每个字段偏移和总大小均有静态断言。它不含原生指针，
所以 x64 构建仍保持原 `0x20` 布局；实现不把它强转为包含文本指针的
`CharacterModalUiRecord`。

## 命中计算与读取顺序

非空记录严格依次读取 `top`、`left`、`height`、`width`。随后先计算
`bottom=W32(top+height)`，再计算 `right=W32(left+width)`，并调用既有唯一
命中边界：

```text
login::detail::Sub_5049A0(left, top, right, bottom, -1)
```

两次加法均先把操作数位型解释为 `uint32_t`，以无符号加法自然回绕，再用
`bit_cast<int32_t>` 恢复 signed W32 位型，避免 C++ signed overflow。实现不钳制、
排序或拒绝负宽高。未命中立即返回 0，且不读取鼠标按钮状态。

## 鼠标快照与返回位域

命中后只读取三个既有唯一状态别名的低 8 位，顺序和次数严格为：

1. `gValueD68D9C@0xD68D9C`：`edge`，一次；
2. `gValueD68D98@0xD68D98`：`held`，一次；
3. `gValueD68DA0@0xD68DA0`：`pulse`，一次。

返回值以 `0x40` 为基值，按原控制流顺序累计：

| 快照位 | 返回位 |
|---|---|
| `edge & 0x01` | `0x001` |
| `edge & 0x02` | `0x004` |
| `held & 0x01` | `0x002` |
| `held & 0x02` | `0x008` |
| `pulse & 0x01` | `0x010` |
| `pulse & 0x02` | `0x020` |
| `edge & 0x10` | `0x080` |
| `edge & 0x20` | `0x100` |

普通按键位和双击位复用同一份 `edge` 快照，不进行第二次全局读取。三个全局
值的高 24 位均不参与结果。

## 新端落点与接线边界

- 公共固定布局与入口：`include/cg/LoginInput.hpp`；
- 实现：`src/LoginInput.cpp`；
- 命中测试与三项鼠标状态均复用 `src/LoginMessageDependencies.hpp` 的既有声明，
  不复制共享状态；
- 当前提供 `QueryLoginControlState_5036F0`；`sub_45CA20` 场景接线留待其
  独立单项。
