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
原物理缓冲为256字节，调用前没有清零Filename。现有新端仍传`0xFF`，
但额外使用零初始化的`char path[256]`；这一失败/未终止路径差异尚待明确裁决，
不能因正常路径有结果便记为原端保真。不改用宽字符、filesystem、argv或当前目录。

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
参数，原端会沿用上一轮值。2026-10-02修正新端此前每轮清零的偏差：
只在循环前初始化一次，后续调用沿同一DWORD；不根据ReadFile返回值额外退出。
原读取缓冲物理跨度为0x8080，首字节先取共享593850，余0x807F字节清零；
读取请求仍为0x8000。正常打开路径在摘要收尾后调用CloseHandle。

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
- 成功路径要求output至少33个可写字节；打开失败时原端复制共享593850的
  完整NUL文本并返回nullptr，所需空间由当时源字符串决定。共享owner当前虚拟
  初值为空不证明永远为空；不能将失败行为固定成只写output[0]。
- 稳定成功域要求 `GetModuleFileNameA` 返回完整、可打开的 ANSI 模块路径，
  文件大小可由低 DWORD 表达，且读取期间文件内容与长度稳定。新端在此域不
  改变原结果。
- 现有额外零初始化路径为API失败或未终止情况提供了不同于原栈槽的字符串边界：
  若 API 失败时未写入缓冲，路径保持为空并自然进入打开失败分支；IDA 未证明
  失败时是否会改写 `path[0]`。无论 API 返回如何，物理第 256 字节的零哨兵都
  保证传给CreateFileA的字符串有界。此前把这项差异概括为“原非法域”不足以
  获得适配授权；本项保留待决，尚不声称失败路径等价。路径截断也可能改变打开对象。
- `GetFileSize` 失败、文件大于 4 GiB、`ReadFile` 失败，以及共享打开后文件被
  并发截短或增长，仍可能导致少读、低 DWORD 下溢、摘要不一致或循环挂起。
  实现不擅自 break、重试或返回新错误。
- 实现本身只使用局部 MD5 状态；但多个线程若同时写同一个 `output`（当前
  caller 的全局 `gValueDA3A1C`）仍有数据竞争，调用方需保证串行。

## 读取状态与失败文本纠正 SPEC（2026-10-02）

当前只读核403660全部74条、5202E0全部75条实际指令，修正三个已有实现差异：

1. 4036E6在循环前将NumberOfBytesRead清零；403725回到4036EE，绕过清零。
   将ComputeFileDigest的bytesRead声明/初始化移到do之前，保持固定0x8000读取、
   不检查API返回、Update后再从remaining减去同一槽以及原循环条件。
2. 4036C1/4036C6先读取共享593850首字节写Buffer，之后201F次stosd、
   stosw、stosb清余0x807F字节，合计0x8080。新端按此物理容量与顺序初始化，
   通过已有input::LegacyText593850借唯一owner，不建立新空字符串或私有副本。
3. 52033D..52035D按strlen加NUL复制同一共享文本；失败分支改为strcpy
   而不是仅写首零，返回nullptr不变。正常编码成功路径、大小计数、文件参数不改。

本线程按既有工程委托实施这三项已证修复，只改LegacyModuleDigest.cpp及本证据/进度。
新增cg/Input.hpp仅供实现借用已存在的原字节owner，不把Input或Win32类型扩散到公开接口。
不执行模块摘要、文件读取函数、算法样例或任何文件哈希计算/比对；仅静态检查。

### 路径缓冲的独立待决适配

520300..520319清的是局部摘要文本首33字节，不是随后传给GetModuleFileNameA
的Filename[256]。原函数忽略API返回，未预写路径尾哨兵。
现有path[256]补零在API失败或未终止时选择了确定字节，这不是取证恢复出的值。
建议明确保留一次入口补零，作为仅限这个256B路径缓冲的适配；其他读取、
失败文本、返回和调用顺序按原合同。其差异是失败/未写路径按零初始化内容继续
打开，不模拟原栈残留。该建议尚待用户针对性确认，不能由一般委托自动批准。
本步暂不改动这行既存补零，也不将其计为已验收；它不授权其他未写局部值补零。
