# cg2d 还原台账

> 基线：当前 IDA，原始输入 SHA-256 为 `dad3f292c6df28e9b7208ad1d339f56ab68c1e728f6595e849e37296551c0f08`。
> 本文件从实际取得的局部证据开始登记，不是完整函数或数据清单。
> 当前条目：R3.1.1 资源名连续存储区初值已实现并通过独立验证。

关联：[基线](cg2d_reconstruction_baseline.md)、[资源来源](cg2d_resource_sources.md)、[独立进度](../plan/cg2d_full_reconstruction_progress.md)。

## 1. R0.6.1 任务卡

| 项目 | 本步约定 |
|---|---|
| 单项任务 | R3.1.1，仅还原资源名连续存储区及初始字节 |
| 原范围 | VA `[0x556F84,0x558A84)`，RVA `[0x156F84,0x158A84)` |
| 已满足证据 | R0.1.1 输入识别、R0.1.2.1 六表完整指纹；R3.1.1.A 再核对全部 6912 字节、原地写入和取址读用 |
| 输入与输出 | 不执行函数；静态初始化后取得与原区间相同的可写字符存储 |
| 布局 | 六表连续，每表九槽，每槽 128 字节；54 槽均为 ASCII、有 NUL，首 NUL 至槽尾全零 |
| 允许修改 | `include/cg/ResourceNames.hpp`、`src/ResourceNames.cpp`、`tests/verify_resource_names.ps1`，以及本线必要证据、计划与进度 |
| 不增加 | 参数解析、别名展开、路径拼接、文件加载、重置、Getter、指针表或 SDL 对象 |
| 局部门禁 | 此范围不含指针或外部接口；ASCII 字节不触发旧文本编码裁决，布局不依赖 SDL 驱动 |
| 验证方法 | 只编译该数据文件为 x86、x64 COFF 对象；检查存储长度、可写属性、无代码和无数据重定位，并逐表比较全部字节的 SHA-256 |
| 非验收范围 | 新客户端目标 ABI、函数调用约定、绝对地址、外围对齐/相邻数据、越界行为、完整生命周期及运行流程 |

只编译数据对象用于证明本数据项不依赖指针宽度，不为新客户端选择 x86 或 x64 构建目标。
不调用旧工程构建，不链接或运行原客户端；不下载依赖，不修改数据库。

## 2. 数据映射

新端用一个可写 `char[6][9][128]` 存储区保持已证实的六表相邻关系。
`ResourceNameTable` 是复刻实现的存储区下标，`gResourceNames` 是复刻别名，不声称恢复了原源码声明或同名业务枚举。
不使用字符串指针数组、动态容器或提前解析后的坐标文件名。

| 稳定标识（RVA） | 原 VA | 存储区内偏移 | 新端映射 | 状态 |
|---|---|---|---|---|
| `D:00156F84` | `0x556F84` | `0x0000` | `cg::gResourceNames[cg::GraphicNames]` | 初值与布局已验证 |
| `D:00157404` | `0x557404` | `0x0480` | `cg::gResourceNames[cg::GraphicInfoNames]` | 初值与布局已验证 |
| `D:00157884` | `0x557884` | `0x0900` | `cg::gResourceNames[cg::AnimeNames]` | 初值与布局已验证 |
| `D:00157D04` | `0x557D04` | `0x0D80` | `cg::gResourceNames[cg::AnimeInfoNames]` | 初值与布局已验证 |
| `D:00158184` | `0x558184` | `0x1200` | `cg::gResourceNames[cg::CoordinateNames]` | 初值与布局已验证 |
| `D:00158604` | `0x558604` | `0x1680` | `cg::gResourceNames[cg::CoordinateInfoNames]` | 初值与布局已验证 |

完整字符串和六个哈希沿用资源来源文档，不由新代码生成测试期望。
`ParseCmdOpt@0x465F00` 的 `sprintf` 写入点为 `0x46653B/0x466590/0x4665E8/0x46663D/0x466692/0x46670F/0x466775`；
坐标表另有 `0x4667DA/0x4667E1/0x46682C/0x466833` 原地复制。
`sub_456050@0x456050` 取内嵌字符地址读取，源步长和终界见 `0x45616E/0x45617A`。
因此可写性及槽内零字节属于本步契约，不能将表改为 `const` 或在初始化时展开 `*3`。

实现：[ResourceNames.hpp](../include/cg/ResourceNames.hpp)、[ResourceNames.cpp](../src/ResourceNames.cpp)；
字节检查：[verify_resource_names.ps1](../tests/verify_resource_names.ps1)。

## 3. 对象验证记录

2026-09-14 主线程使用本机 MSVC `14.50.35717`、PowerShell `7.6.5`，只编译上述数据文件。
对 `Hostx64\x86\cl.exe` 与 `Hostx64\x64\cl.exe` 分别使用下列参数，输出到已忽略的 `cg2d_re/build/r3_1_1/`：

```text
/nologo /c /GR- /utf-8 /W4 /WX
/ID:\Source\cgmsv\cg3a\cg2d_re\include
/FoD:\Source\cgmsv\cg3a\cg2d_re\build\r3_1_1\resource_names_<架构>.obj
D:\Source\cgmsv\cg3a\cg2d_re\src\ResourceNames.cpp
```

在仓库目录使用 PowerShell 7 执行验证，不会链接或执行对象代码：

```powershell
.\cg2d_re\tests\verify_resource_names.ps1 -ObjectPath .\cg2d_re\build\r3_1_1\resource_names_x86.obj -Architecture x86
.\cg2d_re\tests\verify_resource_names.ps1 -ObjectPath .\cg2d_re\build\r3_1_1\resource_names_x64.obj -Architecture x64
```

- 两次编译均通过，三个尺寸断言通过；两份对象的六个完整表指纹均匹配 IDA。
- `.data` 长 `0x1B00`、已初始化且可写、数据重定位数为零；没有非空可执行节。
- `dumpbin /symbols` 核对 x64 对象，`cg::gResourceNames` 为 `.data` 内偏移 0 的外部符号；不据此声称最终 EXE 的绝对地址或外围布局已还原。
- 主线程另生成仅位于忽略目录的单字节篡改、只读属性及截断头对象，并以错误架构检查原对象；四类负例均按对应预期原因拒绝。
- 独立代理重新编译 `verify_x86.obj` 与 `verify_x64.obj`，直接读取当前 IDA 全部 6912 字节后与两对象逐字节比较，两次均为 0 差异；六个测试期望哈希亦与直接读取的 IDA 六表一致。
- 独立复核同时确认两架构的唯一外部数据符号、尺寸断言、连续可写存储、零重定位及没有可执行代码，R3.1.1.V 通过。

## 4. 状态边界

- 本步仅初值、布局及可写存储已通过主线程和独立验证。
- 表内静态初值与布局是本步验收对象；上述消费者函数均未实现，不计为已还原函数。
- 相邻区之外的存储、原端无界写入及其他未决机器行为留在待裁决范围，不以本步测试豁免。
- 未登记的函数与数据仍是未知项；旧工程测试数、IDA 命名和局部通过均不构成全量完成证据。
