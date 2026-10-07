# cg2d 登录控件绘制证据

## 范围

本文只记录当前 `ida-17524/D:\SOFTSTAR\cg2d.exe` 中
`sub_503780@[0x503780,0x5038A9)`：共 121 条指令、14 个基本块，原型为
`void(control, layerW32, stateW32, overlayImageW32, renderModeW32)`。唯一 caller 为
`sub_45CA20@0x45CA20`，共有两处调用。本项只提供
`DrawLoginControl_503780`，不实现或接线 `sub_45CA20`，也不实现其中的
`DrawInputText`。

## 记录布局

`LoginSceneControl32` 是固定 `0x20` 字节的 8×signed W32 记录：

| 偏移 | 字段 | 语义 |
|---|---|---|
| `+0x00` | `drawX` | 基础图绘制 X |
| `+0x04` | `drawY` | 基础图绘制 Y |
| `+0x08` | `left` | 命中矩形左边界 |
| `+0x0C` | `top` | 命中矩形上边界 |
| `+0x10` | `width` | 宽度 |
| `+0x14` | `height` | 高度 |
| `+0x18` | `normalImageNo` | 常态基础图号 |
| `+0x1C` | `activeImageNo` | 激活基础图号 |

八个偏移和总大小均有静态断言，x64 下不引入原生指针宽度。

## 入口与基础图

入口首先构造 16 字节 `render::GraphicDescriptor16`：
`sourceX/sourceY/width/height=0`、`color=-1`、
`clip/mixMode/paletteCacheIndex=0`。这一步位于空指针检查之前；
`control == nullptr` 时随后直接返回。

非空路径先对 `renderMode` 取一次快照：

- 严格等于 1：选择 `normalImageNo`，不命中测试；
- 严格等于 2：选择 `activeImageNo`，不命中测试；
- 其他值：按 `top`、`height`、`left`、`width` 顺序读取，先算
  `bottom=W32(top+height)`，再算 `right=W32(left+width)`，调用
  `Sub_5049A0(left,top,right,bottom,-1)`。命中选激活图，未命中选常态图。

三条路径汇合后恰好调用一次
`DrawGraphic(drawX,drawY,layerLow8,imageNo,1,&descriptor)`，不使用返回值。

## 状态、第二次命中与覆盖图

基础图提交后才读取 `state`。`state == 0` 直接返回。其他值按
`top`、`height`、`left`、`width` 重新读取矩形，独立再调用一次
`Sub_5049A0`；不复用基础图选择阶段的命中结果，未命中直接返回。

第二次命中后再取 `renderMode` 快照，仅严格等于 0 时继续；
1、2 以及其他非零值均不画覆盖图，但不省略此前的第二次命中。
`state == 2` 时 `step=1`，其他任意非零值均为 `step=0`。

覆盖图提交前第三次从记录读取矩形字段，以 W32 无符号运算自然
回绕后取低 16 位：

```text
left16   = low16(left - 2)
top16    = low16(top - 2)
right16  = low16(left + width + 2)
bottom16 = low16(top + height + 2)
```

最后调用
`DrawGraphicHover(left16,top16,right16,bottom16,layerLow8,overlayImageW32,step)`。
`overlayImageW32` 保留完整 32 位，不因边界中的 `-2` 提前过滤覆盖图。

## 算术与接线边界

所有 signed W32 加减都先用 `bit_cast<uint32_t>` 取位型，在无符号域运算，
需要 signed W32 参数时再 `bit_cast<int32_t>` 恢复，不触发 C++ signed overflow。
基础图与覆盖图阶段的两次命中调用保持独立，不缓存跨调用的鼠标结果。
`sub_45CA20` 两处 caller 接线留待其独立单项。

## 正常S32坐标与原取样顺序更新（BSD01，2026-10-07）

[BSD01 SPEC](cg2d_login_control_normal_geometry_spec.md#login-control-geometry-source-state)替代旧文中的无符号回绕模型。两个独立命中各取top、height、left，先算bottom再取width／算right；基础图各分支汇合后先drawY再drawX。覆盖取top、left、height，先算height+top+2和top-2，再取width／算width+left+2和left-2，均用普通S32，最后仍四次转低W16交给既有绘制边界。

descriptor16／NULL分支、mode1常态／2激活／其它命中选图、state非零才第二命中／mode严格0才覆盖及step规则、图号／layer保持；不缓存两次命中或跨调用记录。该入口四片段，其余整源保持。完整原根121条297B／两页、四直接调用三目标／45CA20两真实点纳管为F:00103780／映射1586，相邻7B只NOP用途。局部原顺序已修；完整绘制消费／像素、父、借用寿命和机器ABI／运行继续原验收标准。
