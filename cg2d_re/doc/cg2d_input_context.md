# cg2d 输入上下文切换与初始化还原

## 函数边界

- `Input::SetContext@[0x44B330,0x44B33A)`，3 条指令、1 个基本块，原型为 `void(InputContext*)`。函数只把参数写入原 `gCurrentInputData@0x626C04`，允许写入空指针。
- `sub_449CF0@[0x449CF0,0x449D92)`，41 条指令、3 个基本块，原型为 `void(InputContext*, int, int, int, int, int, const char*, int, int, int, int)`。
- `sub_44A160` 是初始化器使用的私有文本装载辅助函数，不公开为新端接口。

## 固定字段写入

`sub_449CF0` 按以下顺序写入，未对顺序或目标字段作归并：

1. `field130@0x130`
2. `renderMode@0x138`，完整写入 32 位
3. `color@0x140`
4. `field134@0x134`
5. `maxCharPerLine@0x148`
6. `fontType@0x13C`
7. `encrypt@0x150`
8. `maxLine@0x144`
9. `field14C@0x14C`
10. `text[0]@0x000 = NUL`
11. `totalByteLength@0x12C = 0`
12. `caretByteOffset@0x128 = 0`
13. `field154@0x154 = 0`
14. `field158@0x158 = 0`

`field138` 在结构中保留为 4 字节原始存储；实现使用 `memcpy` 完整写入 `renderMode`，不通过不兼容指针别名访问。固定字段初始化阶段不清除 `text[1..287]`，也不写 `x@0x120`、`y@0x124` 或其他字节；非空初始文本造成的后续文本与计数变化只来自下述追加路径。

## 初始文本路径

- `initialText == nullptr` 时，初始化器在固定字段写入后直接结束，不进入文本装载路径，当前上下文指针保持原值。
- `initialText != nullptr` 时，初始化器先保存当前上下文，再通过 `SetContext_44B330` 切换到目标上下文，调用私有 `sub_44A160`，最后通过同一函数恢复保存值；保存值允许为空指针。
- `sub_44A160` 先检查当前上下文；为空时直接返回，非空才以 `strlen` 取得初始文本长度。长度大于等于 `0x120` 时截为 `0x11F`。随后逐字节调用现有 `AppendChar_44ADB0`，不直接复制文本，因此追加函数的过滤、行限制及伴随状态写入仍然生效。

本项不建立账号、密码或角色名上下文实例，不建立焦点表，不实现 `sub_45C7C0`、`sub_45CA20`、输入渲染或 IME。
