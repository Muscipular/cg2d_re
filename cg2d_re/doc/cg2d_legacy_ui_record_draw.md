# cg2d 旧 UI 记录绘制证据

## 范围与调用者

本文只记录当前 `ida-17524/D:\SOFTSTAR\cg2d.exe` 中
`sub_45EFD0@[0x45EFD0,0x45F08C)`：大小 `0xBC`，共 80 条指令、13 个基本块，
原型为 `void(record,layerW32,overlayModeW32,overlayImageW32,styleW32)`。
17 个直接调用点完整如下：

| caller | 调用点 |
|---|---|
| `sub_45FA70` | `0x45FE4D`、`0x45FE5F` |
| `LoginScene::Process@0x45FEA0` | `0x4609E0`、`0x4609F2`、`0x460A3C`、`0x460A7B`、`0x460A8D`、`0x460B4F`、`0x460B9E`、`0x460C2B` |
| `sub_460FF0` | `0x46196F`、`0x4619F9` |
| `sub_461B30` | `0x461DDE`、`0x461E1A`、`0x462026` |
| `sub_464E30` | `0x4652D2`、`0x4652E4` |

当前实现不改这些 caller 的调用表达式。登录场景继续使用
`login::detail::Sub_45EFD0(const void*,...)`，角色选择使用强类型
`character_select::detail::Sub_45EFD0(const CharacterSelectUiRecord*,...)`；后者只
适配 `Sub_45F100` 参数类型并转发到同一份核心控制流。

## 记录字段

本函数访问记录前六个 W32：

| 偏移 | 用途 |
|---:|---|
| `+0x00` | X |
| `+0x04` | Y |
| `+0x08` | 宽度 |
| `+0x0C` | 高度 |
| `+0x10` | 常态图号 |
| `+0x14` | 激活图号 |

角色选择的 `CharacterSelectUiRecord` 仍为 7×W32、共 `0x1C` 字节；末项
`+0x18` 不由本函数读取。登录场景还会传入原始表中可能不适合强类型解引用的
记录，因此共享实现不复制整条记录，也不强转为 W32 数组；每一次定宽字段读取均
使用 `memcpy`，避免未对齐和严格别名未定义行为。

## 基础图选择与提交

空记录在 `0x45EFDA` 直接返回。非空记录严格先读取 `+0x10`：

- `style==0`：在 `0x45EFEC` 调用当前命名空间对应的 `Sub_45F100`，只检查
  返回值低 8 位的 `0x40`；置位时改读 `+0x14`，否则保留先前图号。
- `style==2`：不调用 `Sub_45F100`，强制读取 `+0x14`。
- 其他 style：不调用 `Sub_45F100`，继续使用 `+0x10`。

随后重新读取 Y、X，并在 `0x45F01A` 调用当前命名空间对应的
`Sub_45F090(x,y,low8(layer),imageNo,1,-1)`。两个公开入口的 layer 参数统一为
`std::uint8_t`；两处场景命名空间均引用唯一的 `cg::render::Sub_45F090`。

## 命中与覆盖图顺序

基础图提交后，`overlayMode==0` 直接返回。否则按
`Y→height→X→width` 重新读取四个字段，以 W32 无符号位型回绕计算
`bottom=Y+height`、`right=X+width`，并在 `0x45F03F` 调用
`Sub_5049A0(X,Y,right,bottom,-1)`。

未命中直接返回。命中后才在 `0x45F04B` 检查 style；任何非零 style 都返回，
但不能省略前面的命中调用及其鼠标读取副作用。只有 `style==0` 继续，再按
`Y→X→height→width` 第三次读取字段，以 W32 回绕计算：

```text
left   = X - 2
top    = Y - 2
right  = X + width + 2
bottom = Y + height + 2
```

`0x45F07F` 最终提交
`DrawGraphicHover(low16(left),low16(top),low16(right),low16(bottom),low8(layer),overlayImageW32,step)`；
`overlayImageW32` 保留完整 32 位，`overlayMode==2` 时 `step=1`，其他任意非零
值均为 0。

基础提交、命中和覆盖提交各自重新读取所需坐标字段，不跨 callee 复用先前阶段的
字段值。所有 signed W32 加减均先转为 `uint32_t` 位型运算，再恢复 signed W32，
不触发 C++ signed overflow。

## 实现与依赖

共享核心位于 `src/LegacyUiRecord.cpp`，加入 `cg2d_re_startup`；没有新增公共
结构、状态或外部依赖。`Sub_45F090` 已收敛到唯一渲染入口，证据见
[`cg2d_legacy_graphic_wrapper.md`](cg2d_legacy_graphic_wrapper.md)。两处
`Sub_45F100` 也已转发到同一份状态查询实现，证据见
[`cg2d_legacy_ui_record_state.md`](cg2d_legacy_ui_record_state.md)。本函数当前
没有未决直接依赖。
