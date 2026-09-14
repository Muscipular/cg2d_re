# 图形元信息结构与查询函数

日期：2026-09-14。任务：R3.2.2。状态：实现完成，独立静态复核及 x64 Debug/Release 编译通过。

范围仅为 `include/cg/GraphicInfo.hpp`、`src/GraphicInfo.cpp` 中的两种布局、三个全局状态及七个查询函数，命名空间为 `cg::resource`。
布局依据主线程已逐指令核对的 `GraphicInit@0x459960` 证据；本次在 `ida-30904`（`cg2d.exe`）只读复查了下列七个查询函数的完整指令范围，未改动 IDA。

## 运行时布局

`GraphicInfo` 步长为 `0x20`，不含指针，迁移到 x64 后不扩大记录。头文件对尺寸、全部字段偏移、标准布局及可平凡复制属性设置 `static_assert`。
`seqNo/length` 使用无符号 32 位类型承载原位模式；运行时 `offset` 按主线程 `GraphicRead` 的调用约定使用 `int32_t`，不据此扩展加载时的算术或业务规则。

| 偏移 | 字段 | 存储 | GraphicInit 写入指令 VA |
|---|---|---|---|
| `0x00` | `seqNo` | `uint32_t` | `0x459D05` |
| `0x04` | `offset` | `int32_t` | `0x459D12` |
| `0x08` | `length` | `uint32_t` | `0x459D20` |
| `0x0C` | `x` | `int16_t` | `0x459D2F` |
| `0x0E` | `y` | `int16_t` | `0x459D3F` |
| `0x10` | `w` | `int16_t` | `0x459D4F` |
| `0x12` | `h` | `int16_t` | `0x459D5F` |
| `0x14` | `fileIndex` | `int16_t` | `0x459DA4` |
| `0x16` | `reserved` | 两字节 | 用途未明，保持保留区 |
| `0x18` | `sx` | `uint8_t` | `0x459D6E` |
| `0x19` | `sy` | `uint8_t` | `0x459D7C` |
| `0x1A` | `flag` | `uint16_t` | `0x459D8B` |
| `0x1C` | `mapNo` | `int32_t` | `0x459D9A` |

`fileIndex` 的有符号性另由 `0x45A935` 的 `movsx` 确认。`mapNo` 使用 `int32_t`，查询直接返回字段，不通过数值转换改变原 32 位位模式。

## 磁盘布局

`GraphicInfoRecord` 单独命名，尺寸为 `0x28`（40 字节），同样对尺寸与全部字段偏移设置 `static_assert`。

| 偏移 | 字段 | 存储 |
|---|---|---|
| `0x00/0x04/0x08` | `seqNo/offset/length` | 各 `uint32_t` |
| `0x0C/0x10/0x14/0x18` | `x/y/w/h` | 各 `int32_t` |
| `0x1C/0x1D` | `sx/sy` | 各 `uint8_t` |
| `0x1E` | `flag` | `uint16_t` |
| `0x20` | `reserved` | 四字节 |
| `0x24` | `mapNo` | `int32_t` |

磁盘布局不等于运行时布局：坐标与宽高字段宽度不同，磁盘没有运行时的 `fileIndex`，两个保留区不能等同。本步未实现文件读取、逐字段转换或完整 `GraphicInit`，未引用旧 `ResourceTypes`。

## 全局状态

| 原 VA | 实现 | 未初始化值 |
|---|---|---|
| `0x103CFA0` | `int32_t gMaxGraphicCount` | `0` |
| `0x9A2970` | `GraphicInfo* gGraphicInfo` | `nullptr` |
| `0x9A2974` | `int32_t* gGraphicMapNoIndex` | `nullptr` |

本模块不分配、不加载、不绑定伪造的数据，也不声明初始化已成功。全局指针采用 x64 原生宽度，指向的图形记录及索引元素仍分别为 32 字节、4 字节。

## 原函数映射

| 实现 | 原指令范围（右端不含） | 对应行为 |
|---|---|---|
| `GetGraphicXY` | `[0x45A710, 0x45A760)` | 无符号索引比较；依次输出 `int16_t x/y` |
| `GetGraphicWH` | `[0x45A760, 0x45A7B0)` | 无符号索引比较；依次输出 `int16_t w/h` |
| `GetGraphicSxSy` | `[0x45A7B0, 0x45A802)` | 无符号索引比较；将 `sx/sy` 零扩展为 `uint16_t` 后依次输出 |
| `GetGraphicFlag1` | `[0x45A810, 0x45A851)` | 无符号索引比较；零扩展 `flag` 后 `% 100`，输出 `uint16_t` |
| `GetGraphicFlag2` | `[0x45A860, 0x45A8A8)` | 无符号索引比较；零扩展 `flag` 后 `/ 100`，输出 `uint16_t` |
| `GetGraphicMapNoIndex` | `[0x45A8B0, 0x45A8C9)` | 不检查索引，直接写 `gGraphicMapNoIndex[index]`，返回 `1` |
| `GetGraphicMapNo` | `[0x45A8D0, 0x45A8F2)` | 有符号负索引或不小于有符号上界时返回 `-1`，否则返回 `mapNo` |

前五个函数使用 `index < static_cast<uint32_t>(gMaxGraphicCount)`，成功返回 `1`；失败按参数顺序写零并返回 `0`，不把负计数修复为零。
双输出函数不预取两个字段或缓存整条记录，先完成首个输出写入，再从全局表表达式读取第二字段，对应 `0x45A733/0x45A783/0x45A7D4`。
`sx/sy` 的零扩展对应 `0x45A7C7/0x45A7DE`。Flag1 的 `xor ecx, ecx`、`mov cx` 与 `idiv 100` 对应 `0x45A824/0x45A829/0x45A836`；Flag2 对应 `0x45A874/0x45A879` 的零扩展及 `0x45A87E` 起的乘常数除以 100。

## 调用前提与未覆盖项

- 输出指针必须非空且可写；实际读取表项前，相应全局状态必须已经绑定有效存储。
- `GetGraphicMapNoIndex` 由调用者保证所给索引可访问，不借用图形计数增加边界检查。
- 不新增原函数没有的空指针检查、容量检查或其他修复逻辑。
- 非法地址、实际存储越界，以及原 32 位索引乘步长和地址加法的回绕行为仍未覆盖，不声称这些情形与 x64 指针运算等价。
- 独立代理核对两种布局和七个函数；首次复核发现文档中五个写入地址错配，主线程按原指令修正后再次复核通过，源码无需修改。
- 主线程独立 x64 Debug/Release 编译通过，布局断言已通过编译；未新增或执行测试用例，未启动客户端，不据此宣称完整加载流程已验证。
