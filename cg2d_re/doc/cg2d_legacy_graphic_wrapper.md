# cg2d legacy 图形提交包装证据

## 范围与引用

本文只记录当前 `ida-17524/D:\SOFTSTAR\cg2d.exe` 中
`sub_45F090@[0x45F090,0x45F0FA)`：大小 `0x6A`，共 32 条指令、1 个基本块，
唯一直接目标为 `Renderer::DrawGraphic3@0x520970`。

IDA 共确认 53 个直接 callsite，按 caller 汇总如下：

| caller | callsite 数 |
|---|---:|
| `sub_45EFD0` | 1 |
| `sub_45F190` | 14 |
| `LoginScene::Process@0x45FEA0` | 2 |
| `sub_460FF0` | 3 |
| `sub_461B30` | 3 |
| `sub_462330` | 1 |
| `sub_464840` | 16 |
| `sub_465430` | 13 |

当前实现不改任何 caller 表达式。唯一实体为
`cg::render::Sub_45F090`；登录与角色选择依赖头只用 `using` 将该实体引入各自
`detail` 命名空间，不再保留重复函数声明。

## 参数、描述符与返回

入口参数依次为：

| 参数 | 新端类型 | 原处理 |
|---|---|---|
| X、Y | `std::int32_t` | 完整 W32 原样转发 |
| layer | `std::uint8_t` | 只使用原参数低 8 位 |
| imageNo | `std::int32_t` | 完整 W32 原样转发 |
| value4 | `std::int32_t` | 完整 W32 原样转发 |
| color | `std::int32_t` | 写入描述符 `+0x08` |

函数构造全零的 16 字节 `GraphicModeFields`，只把第六参数写入 `color@+0x08`，
并把 `mixMode@+0x0D` 写为 4。`sourceX/sourceY/width/height`、`clip` 与
`paletteCacheIndex` 保持零。随后原样返回：

```text
DrawGraphic3_520970(x,y,layer,imageNo,value4,1,0,&descriptor)
```

其中 `isGraphicNo=1`、`batchType=0`；不读取、转换或覆盖底层返回值。

## 安全边界

新端复用 `DrawGraphic.cpp` 已有、经过大小和偏移断言的私有
`GraphicModeFields`，再按相同 16 字节位型形成 `GraphicDescriptor16`，不依赖
类型别名或未初始化栈字节。除 layer 的既定低 8 位边界外，不增加图号、坐标、
颜色、队列容量或描述符门禁；队列检查和图号映射仍完全由唯一
`DrawGraphic3_520970` 处理。
