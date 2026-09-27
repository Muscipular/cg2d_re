# cg2d legacy 图形提交包装证据

## 范围与引用

本文初始记录当前 `ida-17524/D:\SOFTSTAR\cg2d.exe` 中
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

## 三种普通入队入口增量

2026-09-27，本线程在现有 `ida-17524` 只读核完整
`521AA0@[521AA0,521B0F)`、`521C80@[521C80,521DC6)`、
`DrawGraphic2@[521DD0,521F14)` 的全部指令、反编译及直接调用点。
三者均只在原 +0C 写零，不需要新对象身份表，故不等待
[DX §15](cg2d_graphic_dx_cache_x64_spec.md) 的 48B 联合字段布局。
本步继续使用现有唯一队列，不提前改变记录大小或已批文本 token。

### 入口差异

| 原入口 / 新端 | 图号与偏移处理 | 类型及特殊标志 |
|---|---|---|
| `521AA0` / `QueueMap_521AA0` | X/Y 原样发布，图号固定 -1，value4=0，dataBits=0；不写 descriptor 或尾字节 | 固定 batchType=5；不写 `gSpecialGraphicPending` |
| `521C80` / `Sub_521C80` | 所有 <=999 的输入均保留并入队，偏移为零；>999 才 mapNo 查询并对结果取 XY，再以 S16 偏移加原 W32 坐标 | 先写类型 0，clip 严格为 1 时覆为 1；检查映射后图号 C18..C21 或原图号 1DF0..1DFF |
| `521DD0` / `DrawGraphic2_521DD0` | 拒绝 -1..999；<-1 保留原值、偏移零；>999 直接对输入 graphic index 取 XY，**不做 mapNo 映射** | 先写 caller 的低 W8 batchType，clip 严格为 1 时覆为 1；两段特殊范围均检查原 graphic index |

因此 `DrawGraphic2` 不能直接转发到现有 DrawGraphic，也不能用
DrawGraphic3 代替：前者会增加 mapNo 映射，后者处理 descriptor 的
sourceX/sourceY 的方式不同。`521C80` 的低图号保留同样不能套用
已有普通 DrawGraphic 的拒绝门禁。

### 共同写入与返回

- 三入口均先以 **S16** 解释共享待提交计数，仅在其 >=4096 时返回 -2；
  不新增负计数恢复或扩大既有其他入口的局部适配。负数形成的越界访问
  按 RPLAN.24 不专项处理、不宣称原故障现场等价。
- 容量与图号门禁通过后，先写排序槽 layer W8，再重读计数写
  cacheIndex W16；随后写主记录。只有 `521C80/521DD0` 再处理描述符。
- 描述符非空时复制原 16B；为空时写零位置、零尺寸、FFFFFFFF 颜色、
  零 clip/mixMode/paletteIndex。它们不改调用者描述符，也不做
  DrawGraphic3 的源坐标减偏移操作。
- 两图形入口的 value4 原样发布，dataBits 只写零；其他字段按本节
  原分支写入，不注册 Yobi、不清未知尾字节。
- 尾部重新读取共享计数，按 S16 扩展作为返回值，再将加一结果写回
  原 W16；不返回最初捕获的索引来省掉该次读取。

`GetGraphicXY` 的既有实现无论有效/无效图号都会写两个 S16 输出，
因此局部偏移只承接其写出，不使用原栈槽高 W16，也不新增加载失败门禁。
坐标相加保留正常 S32/S16 语义；溢出不专项模拟或增加拒绝。

### 接线边界

`QueueMap` 在既有 `GraphicPendingQueue.cpp` 落位，另两入口在
既有 `DrawGraphic.cpp`；公开声明统一在 `GraphicPendingQueue.hpp`。
原 `521C80` 唯一直接 caller `476680@4766DF` 用于低图号 3 的覆盖绘制；
DrawGraphic2 有地图 `46C1C0`、窗口绘制的 `472C16` 调用点及地图角色、
UI 等 25 个直接调用点；`521AA0` 的直接调用位于 `495E47/502B44`。
这些父流程尚未因此实现或接入新端，不能把入口落位记为画面已显示。

本步未改任何已有入队入口的算法，未实施 DX §15，没有新增来源不明的
共享状态。只做限定差异、接口、编码和文档文件链接静态检查；
不构建、测试、运行客户端或使用 subAgent。
