# 登录模块摘要证据

## 边界与调用

本文只记录当前 `ida-17524/cg2d.exe` 中 `sub_5202E0` 及其直接摘要链。

- `sub_5202E0` 精确边界为 `[0x5202E0,0x5203A3)`，共 75 条指令、5 个
  基本块；原型为 `char* __cdecl(char*)`。
- 唯一 caller 是 `LoginScene::Process@0x45FEA0` 的 `0x460167`，传入
  `0xDA3A1C`；返回值未消费。
- 直接 callee 是 `GetModuleFileNameA`、文件 MD5 包装 `sub_403660` 和小写
  hex 编码 `sub_403750`。
- 新端唯一公开边界为
  `cg::platform::ComputeExecutableDigest_5202E0(char*)`；公开头文件不暴露
  Win32 类型。`LoginScene` 继续复用自身唯一持有的 `gValueDA3A1C[64]`，
  不建立第二份摘要状态。
- 原 `[0xDA3A1C,0xDA3A5C)` 恰为 64 字节 PE 零填充区，只有 `0x460162`
  和 `0x50873D` 两个直接 xref；新端不修改其容量、定义或初值，也不把它
  归入 `LoginSavedState`。

## 路径与文件读取

`0x520321` 以 `(nullptr, path, 0xFF)` 调用 `GetModuleFileNameA`，忽略返回值。
原物理缓冲为 256 字节；新端仍传 `0xFF`，使用零初始化的 `char path[256]`，
不改用宽字符、`filesystem`、`argv` 或当前工作目录。

`sub_403660@[0x403660,0x40374F)` 的文件参数严格保留：

| 参数 | 值 |
|---|---|
| 访问 | `GENERIC_READ` |
| 共享 | `FILE_SHARE_READ | FILE_SHARE_WRITE` |
| 安全属性 | `nullptr` |
| 创建方式 | `OPEN_EXISTING` |
| 标志 | `FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN` |
| 模板 | `nullptr` |

打开成功后，`GetFileSize` 仍接收高 DWORD 地址，但只有低 DWORD 控制
`remaining`。循环每次固定请求 `0x8000` 字节，不按 `remaining` 缩短；
`ReadFile` 返回值仍被忽略，只用 `bytesRead` 更新 MD5 并执行
`remaining -= bytesRead`，直到低 DWORD 为零。原端只在进入循环前清一次
`bytesRead`；此后该 DWORD 始终有定义，但若后续 `ReadFile` 失败且未更新输出
参数，原端会沿用上一轮值。新端每轮清零以避免这种沿用；稳定成功读取域中
`ReadFile` 每次都会更新 `bytesRead`，两者无差异。读取失败且
`remaining != 0` 时，新端会不前进并可能挂起。正常打开路径在摘要收尾后调用
`CloseHandle`。

## MD5 与文本编码

`sub_402B70/402C50/402BC0/402D30/402BA0` 构成标准流式 MD5：初值依次为
`67452301/EFCDAB89/98BADCFE/10325476`，使用 F/G/H/I 四轮、标准 64 项
常量、索引与移位，并以 `uint32_t` 模加。位长度以低/高两个 W32 维护；收尾
追加 `0x80`、零填充和 64 位 little-endian 原消息位长度。四个 state 按
little-endian 输出 16 字节。

`sub_403750@[0x403750,0x4037CD)` 对每个摘要字节先写高半字节、再写低半字节，
查表严格为 `0123456789abcdef`，最终追加 NUL。成功时新端向调用方复制恰好
33 字节并返回原 `output`。

## 合法域与风险

- `output == nullptr` 是明确定义域：直接返回 `nullptr`，不调用路径或文件
  API，也不产生写入。
- 成功路径要求 `output` 指向至少 33 个可写字节；打开失败路径只要求至少
  1 个可写字节。`CreateFileA` 失败时只写 `output[0] = '\0'`，不清后续
  字节并返回 `nullptr`。
- 稳定成功域要求 `GetModuleFileNameA` 返回完整、可打开的 ANSI 模块路径，
  文件大小可由低 DWORD 表达，且读取期间文件内容与长度稳定。新端在此域不
  改变原结果。
- 零初始化路径只为 API 失败或未终止等原非法域提供有定义的字符串边界：
  若 API 失败时未写入缓冲，路径保持为空并自然进入打开失败分支；IDA 未证明
  失败时是否会改写 `path[0]`。无论 API 返回如何，物理第 256 字节的零哨兵都
  保证传给 `CreateFileA` 的字符串有界。它不增加回退路径，也不改变稳定成功域。
  路径截断后仍可能打开失败，或碰巧打开另一条截断路径，均不伪造成功结果。
- `GetFileSize` 失败、文件大于 4 GiB、`ReadFile` 失败，以及共享打开后文件被
  并发截短或增长，仍可能导致少读、低 DWORD 下溢、摘要不一致或循环挂起。
  实现不擅自 break、重试或返回新错误。
- 实现本身只使用局部 MD5 状态；但多个线程若同时写同一个 `output`（当前
  caller 的全局 `gValueDA3A1C`）仍有数据竞争，调用方需保证串行。
