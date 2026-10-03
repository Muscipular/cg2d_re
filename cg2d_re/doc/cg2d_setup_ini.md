# ReadSetupIni 原函数还原

## 证据基线

- 单项：R2.1.2.C；实现为 `src/platform/SetupIni.cpp` 中的 `cg::platform::ReadSetupIni()`。
- 实例：`instance_list` 确认的 `cg2d.exe` / `ida-30904`。
- 原 IDB 输入 SHA-256：`ida_nalt.retrieve_input_file_sha256()` 返回 `dad3f292c6df28e9b7208ad1d339f56ab68c1e728f6595e849e37296551c0f08`。
- `survey_binary` 返回的 `e7c2790f479862635932e043b2d7e93b9e791a1edc276a960af05f1454c54010` 是已知磁盘副本差异，不替代 IDB 基线。
- 实际函数范围为 `[0x465A80, 0x465EFC)`，大小 `0x47C`，完整反汇编共 347 条指令；`0x465EF3` 是 `pop esi`，`0x465EFB` 才是 `ret`。
- 实现同时核对完整反汇编与 Hex-Rays 伪代码，未修改 IDA 数据库。

## 接口与字节边界

接口沿用共享头 `include/cg/platform/Startup.hpp`，不新增全局定义：

| 共享状态 | 原地址 | 本函数行为 |
| --- | --- | --- |
| `gCommandLine`，`char*` | `0x9A6EC4` | 读取输入，按分支暂时指向局部缓冲区，最后指向持久化缓冲区 |
| `gCommandLineStorage[4096]` | `0x9A6F48` | 最后一次 `sprintf` 的输出 |
| `gDataPath[128]` | `0x55CF84` | 按 NUL 结尾字节串读取，用于拼接 INI 路径 |
| `gMonitorKeyboard`，`std::int32_t` | `0x9A6EC8` | 入口预设 1，精确命中 `CLOSE` 才写 0 |
| `gRequestedRenderMode`，`std::uint8_t` | `0x628FE0` | 活动模式处理后清零，再按 Effect 优先级写入 1 至 4 |

`GetCurrentDirectoryA`、`GetPrivateProfileStringA` 和 CRT 字符串调用保持原字节边界。不转换为宽字符或 UTF-8，不猜测 INI、路径或命令行的编码；源文件与本文档使用 UTF-8 不代表这些运行时字节也采用 UTF-8。

返回值是最后 `sprintf(gCommandLineStorage, "%s", gCommandLine)` 的 `int` 计数，不含终止 NUL；不是配置读取成功标志。两个 Win32 API 的返回值与原函数一样被忽略，无新增失败回退或假成功返回。

## 原顺序与分支

| 指令范围 | 实现对应 |
| --- | --- |
| `0x465A8D..0x465AD0` | 清零目录、文件名、ScreenMode、屏幕模式命令行和 Effect 缓冲区 |
| `0x465AD9..0x465B49` | 键盘监视预设 1；取得当前目录；检查固定 `buffer[255]` 后按需追加 `\`；用 `%s%s\%s` 拼接 `CG-Setup.ini` |
| `0x465B51..0x465BDC` | 按 ScreenMode、Effect、ActiveMode、MonitorKeyboard 顺序读取 `DISPLAY` 节，默认参数均为 NULL |
| `0x465BDE..0x465CF0` | 屏幕模式处理，必要时切换 `gCommandLine` |
| `0x465CF2..0x465DFF` | 保留屏幕模式处理后的指针，再处理活动模式 |
| `0x465E05..0x465E8A` | 将模式字节清零后按 Effect 子串优先级赋值 |
| `0x465E91..0x465ECA` | MonitorKeyboard 精确比较 `CLOSE` |
| `0x465ED0..0x465EFB` | `sprintf` 持久化、切换全局指针、保留其返回计数并退栈 |

ScreenMode 与 ActiveMode 的值使用区分大小写的完整比较；命令行检查使用区分大小写的 `strstr`，不要求词边界，也不解析引号：

- `NORMAL`：当前命令行不含 `windowmode` 时，写入 `windowmode %s` 并指向屏幕模式缓冲区。
- `FULLSCREEN`：只要命令行任意位置含 `windowmode`，先复制整个字符串，再从复制缓冲区头跳过 `strlen("windowmode ")`，即 11 字节；不删除实际命中位置。
- `ONLYACTIVE`：在屏幕模式处理后的指针中不含 `onlyactive` 时，写入 `onlyactive %s` 并切换指针。两项均需添加时，最终前缀顺序是 `onlyactive windowmode `。
- `NOACTIVE`：只要上述指针任意位置含 `onlyactive`，先复制当前 `gCommandLine`，再从活动模式缓冲区头跳过 `strlen("onlyactive ")`，即 11 字节。
- 其余值不改变对应阶段的命令行指针；不去空格、不去重、不检查完整选项。

Effect 按 `GENERALEFF`、`SIMULATEEFF`、`ALPHAEFF`、`HARDEFF` 的顺序执行区分大小写的子串查找，首个命中分别写 1、2、3、4，全部未命中保持 0。MonitorKeyboard 只有完整值 `CLOSE` 写 0，其他值保留入口预设 1。

## 缓冲区与有效范围

| 局部缓冲区 | 字节数 | 原预清零 |
| --- | --- | --- |
| ScreenMode | 128 | 是 |
| 文件名、当前目录、Effect | 各 256 | 是 |
| ActiveMode、MonitorKeyboard | 各 256 | 否 |
| 屏幕模式命令行 | 4096 | 是 |
| 活动模式命令行 | 4096 | 否 |

实现保留以上容量和预清零差异，不引入截断、动态扩容或额外初始化。可定义并核对的有效调用范围要求：

1. `gCommandLine` 非空，并指向有效、可读、NUL 结尾的字节串；`gDataPath` 的终止 NUL 位于其 128 字节容量内。调用期间无并发修改这些全局状态。
2. 当前目录 API 成功产生 NUL 结尾字符串，原目录字节数不超过 254，使追加反斜杠及 NUL 均位于 256 字节内。完整格式化路径的字节数不超过 255；两个条件都要满足。
3. 每次命令行复制的源字符串不超过 4095 字节；添加 11 字节前缀时，源字符串不超过 4084 字节；两个前缀均添加时，初始命令行不超过 4073 字节。最终持久化字符串不超过 4095 字节。
4. 每次 CRT 字符串写入的实际源、目标存储区域互不重叠，且源在调用期间有效。尤其最后一次 `sprintf` 的源不能与 `gCommandLineStorage` 重叠；上次调用后的全局指针若未经中间分支转到独立局部缓冲区，再次持久化会违反这一条件。
5. API 返回的四个配置字符串在各自容量内 NUL 结尾；任何 11 字节指针跳转后，剩余读取区域必须包含已初始化的 NUL 终止字符串。

第 5 项有一个原差异不可掩盖：命令行仅为 10 字节的 `windowmode` 时，FULLSCREEN 复制后跳到下标 11，读到的是预清零的屏幕模式缓冲区，结果为空串；命令行仅为 10 字节的 `onlyactive` 时，NOACTIVE 的下标 11 位于未初始化的活动模式缓冲区，不属于这里定义的有效范围。实现未把该分支补成空串。

## 差异与未覆盖

- 新实现使用 x64 原生 `char*`；不复刻 x86 栈布局、寄存器分配和调用序言。CRT 内联复制、比较和扫描还原为对应的 `strcpy`、`strcmp`、`strcat`、`strlen` 调用，已在上述有效范围内核对控制流和状态写入。
- 原目录检查仍是固定下标 255，短目录已带反斜杠时仍会再追加一个；没有规范化路径。
- 未解决原长路径、命令行溢出、非 NUL 输入、NULL 命令行、CRT 源目标重叠或未初始化读取，也不承诺这些范围内与旧二进制的内存破坏结果相同。
- 本项仅做反汇编、伪代码与源码静态核对；未新增测试用例，未编译、运行客户端或提交；共享头、全局定义、CMake 和进度文件均不属于本项修改范围。

## 屏幕键盘排除接线

上述原端行为及历史检查记录保留。按已批准的屏幕键盘范围排除，
当前ReadSetupIni入口改为gMonitorKeyboard=0，仍读取
MonitorKeyboard及执行原CLOSE判定，配置不能重新启用键盘；
其余配置、缓冲及返回行为不变。两登录控制器与构建清单的明确
排除见[生产接线](cg2d_external_keyboard_wrapper.md#excluded-keyboard-production-wiring)。
这是用户排除差异，不是声称原端默认值为0；仅静态核对。


## 四处格式改接真实核心（2026-10-03）

本轮局部 SPEC 及裁决见 `cg2d_stdio_contract.md` 的配置四格式节。
当前原端完整 `[465A80,465EFC)` 为347条指令，唯一入向仍是
465790@4659B7。`SetupIni.cpp` 的465B49路径、465C43屏幕前缀、
465D5A活动前缀与465EE0持久化全部改接既有真实53B304，不再使用
宿主sprintf；增内部格式头并删除无消费者cstdio。
四组格式及CG-Setup.ini参数原字节保持，+11B从复制缓冲区头跳过、
屏幕后指针供活动判定、每次格式后才切换共享指针、末次S32计数返回
保持。原共享gCommandLine/gCommandLineStorage/gDataPath与A接口、
固定下标255、缓冲容量及预清差异、Effect顺序、CLOSE判定均不改；
屏幕键盘仍按已批范围排除，不恢复入口原值1。没有转码、截断、扩容、
补零或新增源目标重叠保护；历史有效范围说明不转记异常故障等价。
本轮只核新增接线与原347条/四格式/五字节组/唯一入向、全源码限定替换、
两文档追加、JSON限定差异及UTF-8无BOM/LF，不重新核API/字符串工具
内部。未构建、查看产物、新增/执行测试、运行算法/配置样例、读写INI、
运行客户端或校验哈希；完整配置及启动交互仍未运行验收。


## 配置八处查找直接接线（2026-10-03）

依[字符串局部SPEC](cg2d_legacy_string_compare.md#配置模式八处真实子串查找板块spec2026-10-03)，
本线程裁决八处窗口/活动/效果查找交真实53BE40。整份SetupIni仅八标识符与一个已有
公开头；同一gCommandLine及屏幕后EDI快照、NORMAL/FULLSCREEN与ONLYACTIVE/
NOACTIVE相反门禁、effect原0/1/2/3/4首命中次序保持，其余正文逐字不动。
原windowmode/onlyactive两组复用地址、四不同效果needle共六含NUL字节与八push已核；
347统计/112邻读取及唯一入向只核新增查找，余235条/既有四格式与工具核心不重验。
不纠正已存预清或复制头偏移、不恢复屏幕键盘、不添加拒绝/默认/保护，全部配置生产/
API与启动运行仍待。两旧JSON项限定增量、两合同及唯一进度只追加，未构建、测试、
运行算法/配置样例、读写INI、运行客户端或hash校验，音频/加解密保持。
