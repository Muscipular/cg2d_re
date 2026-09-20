# 登录、角色列表与角色删除请求启动、轮询及响应证据

## 范围

本文证据只来自当前 `ida-32980` 中的 `cg2d.exe`，不使用其他工程推定行为。

- `sub_508AF0@[0x508AF0,0x508B42)`：19 条指令、1 个基本块，无参数，返回 32 位 tick；唯一调用点为 `sub_45EA60@0x45EAEB`。
- `sub_508B50@[0x508B50,0x508BB2)`：25 条指令、8 个基本块，无参数，返回 32 位有符号结果；唯一调用点为 `sub_45EA60@0x45EB75`。
- `sub_512B00@0x512B00` 的当前 IDA 原型为四个 32 位参数、返回 32 位整数；唯一调用点 `0x508B24` 忽略其返回值。
- `sub_508910@[0x508910,0x508950)`：18 条指令、3 个基本块，无参数，返回完整 32 位 tick；唯一调用点为 `sub_45DD60@0x45DE16`。
- `sub_508950@[0x508950,0x5089B0)`：26 条指令、7 个基本块，无参数，返回 32 位有符号结果；唯一调用点为 `sub_45DD60@0x45DEA9`。

## 角色记录选择

原连续区间 `[0xFF03E0,0xFF0508)` 共 `0x128` 字节，固定为 2 条、每条 `0x94` 字节，映像初值全零。该区域存在多个跨场景引用，因此新端公开为定长原始字节数组，不声明尚未证实的完整结构布局。

`gSelectedChara@0xFF0C08` 是有符号 16 位值，映像初值为 0。正常角色选择流程中的三个写点为 `0x461510`、`0x461578`、`0x461654`，写入值限于 0 或 1；其他大量调用点按 `movsx` 读取。`sub_508AF0` 自身没有范围判断，因此新端只声明有效输入前提 `[0,1]`，不增加失败返回或钳制逻辑，并保持有符号值直到 `records.data() + selected` 的记录选择边界。

所需三个 32 位字段只在请求启动处按下列偏移读取，新端均使用 `memcpy`，不通过重解释指针读取：

| 记录内偏移 | 原指令 | 传入 `sub_512B00` 的位置 |
|---:|---|---:|
| `0x18` | `0x508B09` | 第 4 个参数 |
| `0x4C` | `0x508B16` | 第 2 个参数 |
| `0x50` | `0x508B0F` | 第 3 个参数 |

## `sub_508AF0` 顺序

1. `0x508AF0` 以 `movsx` 读取有符号选择值。
2. `0x508AF7` 以 16 位宽度把 `0xDA3B2C` 清零。
3. 选择对应的 `0x94` 字节记录并读取 `+0x18/+0x50/+0x4C` 三个 32 位字段。
4. `0x508B24` 调用 `sub_512B00(socket, field4C, field50, field18)`，忽略返回值。原 socket 来自 `0xD9B2B8`；新端复用 `platform::gConnectionSocket` 的 `uintptr_t` 边界，不在 x64 截断。
5. `0x508B2C` 以 32 位宽度写请求状态 1。
6. `0x508B36` 读取 32 位 `GetTickCount`，`0x508B3C` 保存到请求起始 tick，并原样返回。新端复用 `platform::ReadTickCount32()`。

## `sub_508B50` 顺序

1. `0x508B50` 首先只读一次 `0xD9F2C9` 的低 8 位；为 0 时立即返回 0。
2. 请求状态 `0xDA3B34` 等于 2 时，`0x508B67` 先将其清零，再以 16 位读取 `0xDA3B2C`：等于 1 时把 `0xDA3B30` 写为 1 并返回 1，否则返回 -2。
3. 状态不等于 2 时读取当前 32 位 tick，以无符号 32 位减法计算 `now - start`。结果严格大于 600000 才超时；恰好 600000 仍返回 0，减法自然保留 32 位回绕语义。
4. 超时时原端把 `0x55C9F8` 的旧编码字符串写入 `0xDA34DC` 并返回 -1，不清请求状态。原字节为 `B3 42 B2 7A B5 6E A4 4A B9 4F AE C9 A1 43 00`。新端消息使用 UTF-8 “處理登入逾時。”，有界复制 22 字节（含 NUL）到 1024 字节缓冲，不清空余部。

## 共享存储

下列存储均有跨函数引用，按证据宽度公开，映像初值均为 0：

| 原地址 | 新端类型 | 本函数访问 |
|---|---|---|
| `0xDA3B2C` | `uint16_t` | 启动 W16=0；轮询 R16 |
| `0xDA3B34` | `int32_t` | 启动 W32=1；轮询 R/W32 |
| `0xDA3B38` | `uint32_t` | 启动 W32 tick；轮询 R32 |
| `0xD9F2C9` | `uint8_t` | 轮询首个 R8 |
| `0xDA3B30` | `int32_t` | 成功路径 W32=1 |
| `0xDA34DC` | `char[1024]` | 超时路径写入字符串与 NUL |

登录请求部分未实现的唯一直接依赖为 `sub_512B00`；只保留真实声明，不提供空体或固定返回值。

## 角色列表请求 `sub_508910/sub_508950`

### 公开入口与角色记录清零

`StartCharacterListRequest()` 与 `PollCharacterListRequest()` 分别直接承接
`sub_508910/sub_508950`，角色列表进度不再经过旧地址声明。原连续记录仍复用
`gCharacterRecords`：固定为 2 条，每条 `0x94` 字节，总大小 `0x128`，这些大小
均有静态断言。启动入口显式先对记录 0 的整个 byte-array `fill(0)`，再对记录 1
执行相同操作，严格保持原 `sub_51B3B0(0)` 后 `sub_51B3B0(1)` 的顺序；直接清零
已取证的现有存储，不新增未实现链接。

启动顺序严格为：

1. 依次清空记录 0、记录 1。
2. 从 `platform::gConnectionSocket` 快照本机 socket。
3. 以 W16 把独立回复码 `gCharacterListReplyCode@0xDA3B26` 写为 0。
4. 调用 `sub_512B90(socket)` 并忽略返回；发送失败也继续后续状态推进。
5. 以 W32 把共享 `gRequestStatus@0xDA3B34` 写为 1。
6. 调用 `ReadTickCount32()`，把完整 `uint32` tick 写入共享
   `gRequestStartTick@0xDA3B38` 并原样返回。

`sub_512B90@0x512B90` 的唯一参数是原 socket。其名义请求为无字段 `Gp`，协议
标识字节为 `47 70 00`；新端只新增
`Sub_512B90(std::uintptr_t socket)` 真实声明，不实现发送目标，也不因返回值
增加分支。

### 轮询状态表

| 条件 | 顺序与返回 |
|---|---|
| `gRequestStatus == 2` 且回复码 1 | 先快照 W16 回复码，再把状态写 0，返回 1。 |
| `gRequestStatus == 2` 且回复码 2 | 同样先快照再清状态，返回 -3。 |
| `gRequestStatus == 2` 且回复码为其他值 | 同样先快照再清状态，返回 -2。回复码本身不清。 |
| 状态不是 2，`uint32(now-start) <= 600000` | 无条件读取当前 tick，以无符号回绕减法计算 elapsed，返回 0。 |
| 状态不是 2，`uint32(now-start) > 600000` | 复制超时 UTF-8 文本及 NUL，返回 -1；不清状态、回复码或起始 tick。 |

此轮询不读取 `gRequestPollingEnabled`，也不写 `gRequestOutcomeFlag`；不能复用
登录请求轮询的这两项分支。

### 共享与独立存储

| 原地址 | 新端存储 | 所有权与本请求访问 |
|---|---|---|
| `[0xFF03E0,0xFF0508)` | `gCharacterRecords` | 复用登录模块的两条固定记录；启动时按 0、1 顺序全量清零。 |
| `0xDA3B26` | `gCharacterListReplyCode`，`uint16_t` | 角色列表请求独立公开存储；启动 W16=0，轮询只快照读取。 |
| `0xDA3B34` | `gRequestStatus`，`int32_t` | 与登录及其他网络请求共享；启动写 1，完成轮询写 0。 |
| `0xDA3B38` | `gRequestStartTick`，`uint32_t` | 共享 32 位起始 tick；保留自然回绕。 |
| `0xDA34DC` | `gRequestMessage[1024]` | 共享消息；仅超时路径写入文本及 NUL，不清余部。 |
| `0xD9B2B8` | `platform::gConnectionSocket`，`uintptr_t` | 调用发送目标前快照，不把 x64 socket 截断到 W32。 |

角色列表回复码不能与登录回复码 `0xDA3B2C` 合并；其他四项则必须复用既有
定义，不能建立第二份 status、tick、消息或记录数组。

### 超时文本与未决异步响应

原超时文本 `0x58AF94` 的 CP950 字节（含首个 NUL）为
`A8 FA B1 6F A8 A4 A6 E2 A6 43 AA ED B9 4F AE C9 A1 43 00`。源码使用
UTF-8“取得角色列表逾時。”，正文为 27 字节、含 NUL 为 28 字节；静态断言
确认其不超过共享 1024 字节缓冲，复制长度固定包含 NUL，不依赖旧编码长度。

异步 `Daj` 响应写者 `sub_5089B0@[0x5089B0,0x508AE9)` 本步不实现。已确认的
控制流仅在状态为 1 时接受响应，并先写状态 2：成功响应先写回复码 1，再固定
解析两组角色记录；错误文本 `OUTOFSERVICE` 写回复码 2；其他错误保持启动时的
回复码 0。该响应写者是当前角色列表请求唯一未决接线，不能在启动或轮询函数中
伪造状态 2 或成功回复。下层发送实现 `sub_512B90` 也仍只保留真实声明，不提供
空体或固定成功返回。

## 角色删除请求 `sub_508F70/sub_508FC0`

`sub_508F70@[0x508F70,0x508FB3)` 为 14 条指令、1 个基本块；
`sub_508FC0@[0x508FC0,0x509011)` 为 21 条指令、5 个基本块。新端分别以
`StartCharacterDeletionRequest()` 与 `PollCharacterDeletionRequest()` 承接，角色删除
进度的阶段 1 发起请求并递增阶段，阶段 2 才开始轮询，因此不会在发起请求的同次
更新中轮询。场景仍只处理返回 1 和 -2，-1 超时保持忽略；超时后状态不清，后续
轮询仍可接受迟到响应。

### 启动顺序与前置条件

1. 以有符号 `int16_t` 读取 `gSelectedCharacterIndex`。有效值位于 `[0,1]` 是调用
   前提；原函数不检查范围，新端不增加分支或钳制。
2. 以 W16 把独立回复码 `gCharacterDeletionReplyCode@0xDA3B28` 写为 0。
3. 先从 `platform::gConnectionSocket` 快照 `uintptr_t` socket，再选择对应的 `0x94`
   字节角色记录，以 `memcpy` 从记录 `+0x4C` 读取有符号 `int32_t registNumber`。
4. 调用 `sub_512AB0(socket, registNumber)` 并忽略返回值；发送失败也继续推进状态。
   该发送目标的名义协议为 `Godf`，唯一字段是 signed `Int62`；协议标识字节为
   `47 6F 64 66 00`。新端只新增保留 x64 指针宽度的真实声明，不实现发送目标。
5. 以 W32 把共享 `gRequestStatus@0xDA3B34` 写为 1，调用
   `ReadTickCount32()`，把完整 `uint32_t` tick 写入共享
   `gRequestStartTick@0xDA3B38` 并原样返回。

启动不清角色记录、共享消息、登录回复码或角色列表回复码。

### 共享与独立存储

| 原地址 | 新端存储 | 所有权与本请求访问 |
|---|---|---|
| `[0xFF03E0,0xFF0508)` | `gCharacterRecords` | 复用两条固定角色记录；只读取所选记录的 `+0x4C`，不清记录。 |
| `0xFF0C08` | `gSelectedCharacterIndex`，`int16_t` | 复用有符号选择值；启动首步读取。 |
| `0xDA3B28` | `gCharacterDeletionReplyCode`，`uint16_t` | 删除请求独立公开存储；启动 W16=0，完成轮询先快照再清共享状态。 |
| `0xDA3B34` | `gRequestStatus`，`int32_t` | 与其他网络请求共享；启动写 1，完成轮询写 0。 |
| `0xDA3B38` | `gRequestStartTick`，`uint32_t` | 共享完整 32 位 tick；轮询保留自然回绕。 |
| `0xDA34DC` | `gRequestMessage[1024]` | 共享消息；仅超时路径复制文本与 NUL。 |
| `0xD9B2B8` | `platform::gConnectionSocket`，`uintptr_t` | 读取字段前快照；x64 不截断为 32 位。 |

### 轮询状态与返回

| 条件 | 顺序与返回 |
|---|---|
| `gRequestStatus == 2` 且删除回复码非 0 | 先快照 W16 回复码，再把状态写 0，返回 1。 |
| `gRequestStatus == 2` 且删除回复码为 0 | 同样先快照再清状态，返回 -2；回复码本身不清。 |
| 状态不是 2，`uint32(now-start) <= 600000` | 无条件读取当前 tick，以无符号回绕减法计算 elapsed，返回 0。 |
| 状态不是 2，`uint32(now-start) > 600000` | 复制超时 UTF-8 文本及 NUL，返回 -1；不清状态、回复码或起始 tick。 |

删除轮询不检查 `gRequestPollingEnabled`，也不写 `gRequestOutcomeFlag`。它复用
`gCharacterRecords`、`gSelectedCharacterIndex`、`gRequestStatus`、
`gRequestStartTick`、`gRequestMessage`，只为原 `0xDA3B28` 建立独立 `uint16_t`
回复码存储，不建立第二份共享状态。

原超时文本的 CP950 字节（含 NUL）为
`A7 52 B0 A3 A8 A4 A6 E2 B9 4F AE C9 A1 43 00`。源码使用 UTF-8
“刪除角色逾時。”，正文为 21 字节、含 NUL 为 22 字节；静态断言确认长度并保证
不超过共享 1024 字节缓冲，复制固定包含 NUL，不清空余部。

### 删除异步响应 `sub_509020`

`sub_509020@[0x509020,0x50907B)` 共 33 条指令、11 个基本块，没有直接调用；
字符串比较由原函数内联完成。唯一调用者是
`network_dispatch@0x516269`。调用点按 `data/result/fd` 的顺序压入三个 cdecl
参数，因此公开入口采用
`HandleCharacterDeletionResponse_509020(std::uintptr_t fd, const char* result,
const char* data)`。IDA 当前显示的两参数原型遗漏了完全未读取的 `data`，不作为
实际调用契约；x64 新端以 `uintptr_t` 承接 `fd`，不截断指针宽度。

处理顺序严格为：

1. `gRequestStatus != 1` 时立即返回，不写状态或删除回复码。
2. 状态为 1 时先把 `gRequestStatus` 写为 2，再对 `result` 与 ASCII
   `successful` 做区分大小写、包含 NUL 终止位置的逐字节精确比较。
3. 完全相等时才把 `gCharacterDeletionReplyCode` 写为 1；不相等时保持
   `StartCharacterDeletionRequest()` 已清零的原值，不再次清回复码。

函数不检查 `result` 空指针，不读取 `fd/data`，不复制 `data` 或错误消息。完成后
的重复响应，以及轮询清状态后的迟到响应，都会因状态不为 1 而忽略；超时路径不
清状态，因此超时后的首个迟到响应仍可接受。名义响应标识 `oy` 的字节为
`6F 79 00`，成功文本字节为
`73 75 63 63 65 73 73 66 75 6C 00`。当前只提供语义 handler，尚未接入现代
`NetworkLoop` 或 dispatcher。
