# 通用 RAND 入口证据与待批普通除零 SPEC

## 任务范围与当前状态

- 单项：`R6.1.3.162.D`；主线程交接基线为 `HEAD 2720cd6`。
- 本文仅保存已交接事实及待批建议，不实现通用入口、不接 caller、不代用户确认。
- 分析代理 Bacon 的结论仍为 `NEEDS_CLARIFICATION`，不得改写为源码 `READY`。
- 名称、落位及普通除零错误映射尚待批准；完整目标或继续任务的要求不等于批准。
- 本次仅新增本文，不修改源码、旧文档或任何进度文件；不暂存、不提交。
- 独立 review 与文件验证尚待后续执行；本文不预写 `PASS` 或源码验收通过。

## 证据来源与继承边界

原端证据来自主线程完整交接：IDA 实例 `ida-17524/a1def68e`，数据库
`D:\SOFTSTAR\cg2d.exe.i64`，目标 `cg2d.exe`，x86 小端，映像基址 `0x400000`。
交接时 autoanalysis 已完成，分析查询已释放，用户实例未关闭。
本次文档整理未连接或操作 IDA，不进行 hash 核验、图谱分析、编译、测试或程序运行。

范围算法继承 `cg2d_re/doc/cg2d_yobi_runtime.md:198-201`：
W32 的 `upper+1` 为零时返回零，否则按 signed32 区间宽度取余后加下界。
本文补存通用入口 ABI、消费顺序、普通除零及 caller 证据；
不重审或修改已过的种子、私有 `RandomSymmetricW32`、`Sub_423640`。

除下文明确标为源码位置核对的内容外，原端指令与 caller 结论均是已交接证据，
不表示本次重新取证。现代 API 说明仅服务于待批建议，不作为 cg2d 行为证据。

## 原端入口与完整 ABI

| 项目 | 已交接证据 |
|---|---|
| 入口范围 | `RAND@0x46FAF0`，`[0x46FAF0,0x46FB13)`，单 chunk |
| 中间入口 | 未见外部引用指向该范围的中间地址；不据此扩大为其他入口的结论 |
| 调用约定 | `int __cdecl(int lower, int upper)`，两个 W32 实参 |
| 入口栈位置 | 入口时 `[ESP+4]` 为 lower，`[ESP+8]` 为 upper |
| 保存 ESI 后的读取 | `0x46FAF1` 从 `[ESP+0xC]` 读取 upper；`0x46FB03` 才从 `[ESP+8]` 读取 lower |
| 返回值 | 完整 W32 `EAX`，按 S32 使用；不是 bool，也不是窄值返回 |
| 返回与寄存器 | `0x46FAFD`、`0x46FB12` 均为无立即数 `ret`；ESI 保存并恢复 |
| 清栈证据 | caller 的 `0x44920B: add esp, 8` 清理两个实参 |
| 写入与状态 | 不写参数或对象；持久副作用只涉及共享 RNG 状态 |

访问和消费顺序不可互换：`0x46FAF1` 先读 upper，
`0x46FAF5-0x46FAFD` 判断并完成早退；未早退才在 `0x46FAFE`
调用 `0x53B4C9`，随后 `0x46FB03` 才读 lower。
不能把 lower 的读取、区间除零检查或 RNG 调用提前到早退判断之前。

## 既有随机源与源码 owner

以下位置在本次文档整理时按 UTF-8 只读核对；两个 owner 源码文件未列为 dirty。

| 当前位置 | 职责 |
|---|---|
| `cg2d_re/src/platform/MainLoopSupport.cpp:59` | `gRandomSeed`，原 `0x58F870` 的共享状态，初值为 1 |
| `cg2d_re/src/platform/MainLoopSupport.cpp:101` | `SeedRandom_53B4BF`，既有播种入口 |
| `cg2d_re/src/platform/MainLoopSupport.cpp:107` | `NextRandom_53B4C9`，既有随机数入口 |
| `cg2d_re/src/platform/MainLoopDependencies.hpp:46` | 既有 `gRandomSeed` 声明；播种及取数声明分别在第 47、49 行 |

既有取数按 W32 更新 `seed = seed * 0x343FD + 0x269EC3`，
返回 `(seed >> 16) & 0x7FFF`，故 `N` 为 `0..32767`。
这些是既有 owner 的位置及消费基础，不是本次新增算法或重新批准种子/helper。
通用入口候选只调用既有 `NextRandom_53B4C9`，不新增随机状态、不另建 seed、不重播种。

## W32 算法、早退与消费次数

记 `L`、`U` 为 lower、upper 的 W32 位模式；`W32(x)` 表示保留低 32 位，
`S32(x)` 表示将同一 W32 位模式按 signed32 解释。
区间除数为 `D = S32(W32(U - L + 1))`。
`N` 只表示非早退路径上一次 `NextRandom_53B4C9` 的结果，不表示提前取数。

| 条件 | 原端结果 | 本入口 RNG 消费 |
|---|---|---|
| `W32(U + 1) == 0` | `EAX = 0`；不读 lower、不取余 | 0 次 |
| `W32(U + 1) != 0` 且 `D != 0` | `EAX = W32(L + (N % D))`，按 S32 返回 | 1 次 |
| `W32(U + 1) != 0` 且 `D == 0` | `0x46FB0B: idiv esi` 整数除零，无正常返回 | 故障前已消费 1 次 |

非早退路径在 RNG 调用后读取 lower；`0x46FB0B-0x46FB12` 完成 signed
除法取余、W32 加下界及返回。表中的 `%` 表示 signed32 余数语义，
不是授权在 C++ 中对零除数执行 `%`。
负除数仍使用 signed 余数，不交换上下界，也不保证返回值落入所谓“修正区间”。

### 普通除零与数值排除边界

非早退普通除零的精确条件为：
`W32(upper + 1) == lower` 且 `upper != -1`；
等式两侧按 W32 位模式比较，`-1` 表示 signed32 的全 1 位模式。

- `(lower, upper) = (1, 0)` 无需发生 overflow 就会除零；原入口先消费 1 次 RNG，
  再在 `idiv esi` 故障。这只是入口条件示例，不是已证 caller 输入。
- `(0, -1)` 虽然区间除数按位计算为零，但先命中 upper 早退，返回 0，不消费 RNG。
- 上下界倒置不必导致除零，不能用“倒置区间”替代上述精确条件。
- `N` 仅为 `0..32767`，所以此处不存在 IDIV 商溢出；除数为 `INT32_MIN`
  或 `-1` 也不造成商溢出。不设置数值极值门槛。
- overflow、专项回绕及令牌耗尽不处理的既定排除保持不变；正常位宽、掩码、
  取模语义仍须保留。一般内存、栈及除零不因此获得豁免。

不得擅自 `return 0`、拒绝范围、交换边界或 clamp；也不得以 C++ `% 0` 的
未定义行为声称复现了原端异常。普通除零的适配策略必须单独获得批准。

## 已知 caller 与未审边界

必要 caller `Sub_449160` 的已核直接调用如下：

| 调用地址 | `(lower, upper)` | signed32 除数 |
|---|---|---:|
| `0x449206` | `(0, 1)` 或 `(2, 3)` | 2 |
| `0x44921B` | `(0, 200)` | 201 |
| `0x44923E` | `(0, 2048)` | 2049 |
| `0x44926C`、`0x4492BB`、`0x44930D`、`0x44935F` | `(0, 40)` | 41 |

这些实参均不早退，也不发生零除数。已核分支表 `0x4493AC`；
成功进入创建后的相关路径直接调用 RAND 4 次，直接消费 RNG 4 次。
该计数不包含其他 callee 的 RNG 消费，不代表整个调用链只消费 4 次。

另外，`0x475AE5`、`0x475AFC` 的实参为 `(-15, 15)`；
`0x475B6B`、`0x475B9A` 是动态区间，宽度恒为 21。
不补写交接中没有给出的动态上下界。

共见 129 个直接引用、43 个 caller，尚未全部审查。
本文不宣称所有 caller 正常，不断言其他零除数路径可达或不可达，
也不把本单项扩成对全部 129 个引用的重审。

## 通用入口名称与落位候选（待批准）

候选签名为
`std::int32_t cg::platform::RandomRange_46FAF0(std::int32_t lower, std::int32_t upper)`。
候选定义位置为既有 `cg2d_re/src/platform/MainLoopSupport.cpp`，
声明位置为既有 `cg2d_re/src/platform/MainLoopDependencies.hpp`。
名称与落位均尚未获批，不是现有定义或声明。

候选合同仅复用现有随机源：中间计算使用 W32，除数与返回解释使用 S32；
upper 早退判断保持在 RNG 前，非早退路径保持先取随机数、后读 lower 的顺序。
无新状态、不重播种，不修改私有对称 helper 或 `Sub_423640`。

当前通用入口尚未实现或接线；不提前接入未批准的 `Sub_449160`，
不修改 dirty 公有头、CMake、AGENTS 或登录文本分析代理的工作。
普通除零如何映射仍未决，不能由签名、落位候选或正常 caller 样本自动补齐。

## 主线程提出的待批异常映射（不是原端证据）

### 候选内容与适配差异

主线程提出的候选为：非早退且 `D == 0` 时，先消费 1 次共享 RNG，
再通过 Windows `RaiseException`，以 `EXCEPTION_NONCONTINUABLE` 标志
发出整数除零的软件异常。此候选不是已经实现的代码。

`EXCEPTION_NONCONTINUABLE` 仅禁止在发生异常的位置恢复继续执行，
不表示“进程无条件终止”；依据同一参考文档对 frame-based exception handler
的说明，Windows 的外层异常处理与栈展开仍可发生。
原分析中的“无正常返回”也不等于已证明所有调用都会杀死进程。

该候选不保证原 x86 异常地址、寄存器/栈上下文，或在原异常点恢复执行的语义。
它属于软件生成异常的现代适配，有明确差异，不能冒称 hardware IDIV、
原生故障等价或已还原原异常上下文。
上述内容仅准确限定已交接候选，不新增批准或扩大源码范围。

### 仅用于现代 API 的参考

主线程提供的参考为 Microsoft Learn `RaiseException` 官方文档，
文档日期 `2024-02-02`：
`https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-raiseexception`。

该参考说明 `RaiseException` 产生 software-generated exception，
`dwExceptionFlags` 可为 `EXCEPTION_NONCONTINUABLE`；
尝试在发生异常的位置恢复继续执行会引发 `EXCEPTION_NONCONTINUABLE_EXCEPTION`。
它只支持现代 API 机制说明，不证明 cg2d 使用了该 API，不属于原端取证。
本次仅保存交接提供的参考，未重新联网查询。

### 批准状态与后续门禁

主线程已向用户请求明确批准，但尚未得到答复。
本文不代用户批准该异常候选，也不将 Bacon 的 `NEEDS_CLARIFICATION`
变更为源码 `READY`；暂不写代码或上线新异常策略。

仍需用户明确批准名称、落位与异常映射，并接受上述适配差异。
后续具体实施须重新进行分析及主线程门禁，覆盖批准后的完整合同；
不能把本文、独立文档 review 或文件验证当作源码实施授权。
本单项后续只安排独立 review 和文件验证，进度由主线程记录。
