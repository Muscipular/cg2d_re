# 旧光标逐帧更新

## 范围与证据

本项完整还原 `World::CursorUpdate@[0x506E00,0x507083)`，并补齐它直接依赖的
`CryptInt::add@[0x401E10,0x401E1C)` 与
`sub_506DD0@[0x506DD0,0x506DDB)`。主函数共有 174 条指令、33 个基本块，
两个 caller 是 `MainLoop@0x457124` 与 `InitLoading@0x45819D`；新端本步只接入
既有 MainLoop 调用，不接 `InitLoading`、`InitializeClient` 或 `WIN_PROC`。

取证使用只读 IDA 实例 `ida-17524` 的 `D:\SOFTSTAR\cg2d.exe`；`survey_binary`
报告 32 位 x86、小端、基址 `0x400000`，同名磁盘文件 SHA-256 为
`e7c2790f479862635932e043b2d7e93b9e791a1edc276a960af05f1454c54010`。
本项未修改 IDA，未读取或使用 STONAGE 作为证据。

## 请求状态写入 helper

`sub_506DE0@[0x506DE0,0x506DFB)` 共 9 条指令、4 个基本块，没有 callee，
11 个 caller 共用原 `dword_D68DC4@0xD68DC4`。新端直接复用唯一 owner
`platform::gValueD68DC4`，不建立第二份请求状态。

函数先在 `0x506DE0..0x506DE6` 检查 `force`。`force` 非零时不读取当前状态，
直接把 `requestedState` 的 W32 位型写入 owner 并返回原参数；`force` 为零时才在
`0x506DE8` 单次采样当前状态，非零则按相同 W32 位型返回旧值，为零则写入并返回
`requestedState`。owner 使用 `uint32_t`，接口返回 `int32_t`，两者之间只用
`bit_cast` 保留位型，不依赖域外无符号到有符号转换，也不增加状态范围门禁。

尚未实现的 `sub_507140` 在 `0x5079D7`、`0x507A1F`、`0x507A49`、
`0x507A74` 四次以 `(requestedState=1, force=0)` 调用并忽略返回值。本步只实现
这个真实叶函数，不截取或伪造 `sub_507140` 的文本、图形或 Yobi 路径。

## 按键边沿、计时与脉冲

入口 `0x506E14` 先把 `edgeButtons@0xD68D9C` 完整 W32 清零，再分别处理左、
右两套计时，顺序不可交换：

1. `leftDownTick@0xD68DA4` 非零才独立调用一次 `GetTickCount`，以 W32 回绕减法
   写 `leftElapsed@0xD68DA8`；为零时沿用旧 elapsed。
2. `rightDownTick@0xD68DAC` 非零才再次独立调用一次 `GetTickCount`，同样写
   `rightElapsed@0xD68DB0`；为零时沿用旧 elapsed。每帧因此可能调用 0、1 或
   2 次 tick，不能合并成统一的 `now`。
3. 左侧使用锚点 `0xD68DC8` 与 `pulseButtons` bit `0x01`，右侧使用锚点
   `0xD68BF0` 与 bit `0x02`。elapsed 不大于 500 时清锚和本位；大于 500 且
   锚为零时保存 elapsed 并置位；已有锚时，W32 回绕差小于 100 清本位，
   否则更新锚并置位。清位只影响对应 bit，保留同一 W32 的其余位。

## `bShow == 0` 的表驱动分支

`bShow@0xD67AE4` 与请求状态都是 W32。绘制抑制
`drawingSuppression@0xD68DBC` 非零时整段跳过；为零时构造固定 16 字节描述符：

```text
00 00 00 00 00 00 00 00 FF FF FF FF 00 04 00 00
```

描述符第 `+8..+11` 字节是颜色 `0xFFFFFFFF`，第 `+13` 字节是 mix mode 4，
其余为零。原函数三次独立读取 `gValueD68DC4`：先选图号，再在 Y 坐标读取后选
Y 偏移，最后在 X 坐标读取后选 X 偏移。两次坐标调用严格保持 Y 后 X，均为
`CryptInt::add(value,16)` 的单次读取与 W32 回绕结果；偏移再按有符号 16 位作
W32 回绕相加。最终调用
`DrawGraphic(x,y,0xE7,image,0,&descriptor)`。

IDA 逐项读取的 11 个状态如下：

| 状态 | 图号 | X 偏移 | Y 偏移 |
| ---: | ---: | ---: | ---: |
| 0 | `0x3BE98` | 0 | 0 |
| 1 | `0x3BE9B` | -20 | -30 |
| 2 | `0x3BE9C` | 0 | 0 |
| 3 | `0x3C3A8` | -23 | -22 |
| 4 | `0x3C3A1` | -16 | -25 |
| 5 | `0x3C3A2` | -9 | -22 |
| 6 | `0x3C3A3` | -7 | -16 |
| 7 | `0x3C3A4` | -9 | -10 |
| 8 | `0x3C3A5` | -16 | -7 |
| 9 | `0x3C3A6` | -23 | -10 |
| 10 | `0x3C3A7` | -25 | -16 |

状态 0 来自原 `[0x55CE80,0x55CE88)`，新端由匿名命名空间内唯一可写的 8 字节
定宽记录承载：W32 图号位模式初值为 `0x3BE98`，W32 packed offsets 初值为 0，
并对总大小作静态断言。图号、X 偏移、Y 偏移三个 helper 都从该对象读取状态 0，
不以内联常量或直接返回 0 代替存储。状态 1..10 不复制第二份常量表，而是直接
读取既有 `resource::gCursorPathStorage.cursor[state-1].value04/value08`；其中
`value08` 的低、高 16 位分别是 X、Y 偏移。

## `bShow != 0` 的原生光标分支

私有缓存 `0xD68BE8` 是映像零值 W32，全部两处 xref 都在本函数内，因此只在
`LegacyCursor.cpp` 建立唯一私有存储。请求状态与缓存不同时严格按顺序调用
`GetCursorPos`、`ScreenToClient(gLegacyWindow)`、
`GetClientRect(gLegacyWindow)`，忽略三者返回值。点坐标按 signed32 检查
`x>=0 && x<right && y>=0 && y<bottom`；命中后重新读取请求状态，再从
`gCursorHandles` 取原生句柄调用 `SetCursor`。无论状态是否变化或点是否命中，
随后都重新读取请求状态并更新缓存。

抑制值为零时仍按 Y 后 X 各调用一次 `CryptInt::add(...,16)`，随后提交
`DrawGraphic(x,y,0xE7,0x55F0,0,nullptr)`；抑制非零时不读坐标、不提交。

## 公共尾部与定宽 helper

公共尾部在 `0x507066` 重新读取请求状态，随后严格执行：

```text
consumed = gValueD68DC4
gValueD68DC4 = 0
gValue9A6F44 = consumed
ResetLegacyMouseTail_506DD0()
```

`ResetLegacyMouseTail_506DD0` 对应原两条指令，只清
`tailReset@0xD68DC0`。`CryptInt::add` 对应原三条指令，只调用一次
`Read_401EC0`，再作 W32 回绕加法，不引入额外随机或读取副作用。

## x64 安全边界与接入状态

原端直接以请求状态索引表和 11 项句柄数组，没有 `0..10` 检查；损坏状态会
越界。新端在每个数组读取点做最窄范围拒绝：表驱动分支仍在三个原时点各读取
一次请求状态，无效值只跳过对应表访问并标记本次不可绘制；Y、X 两次
`CryptInt::add` 无论前一状态是否无效都照常执行，第三次状态读取完成后才按标记
跳过 `DrawGraphic`。正常 0..10 路径的读取次数、次序与结果不变，原生光标分支
的损坏值只跳过不安全的句柄读取，公共尾部仍执行。

原端忽略三个 Win32 API 返回值，失败时可能继续读取未初始化栈局部。新端仍
调用全部 API 并忽略返回值，但把 `POINT/RECT` 零初始化以避免 C++ 未定义行为；
这是失败路径上的明确安全偏差，成功路径不变。原生句柄继续用 `uintptr_t`
承载，仅在 Win32 边界转换，不把指针压回 W32。

`MainLoop.cpp` 现在通过公开 `LegacyCursor.hpp` 调用唯一实现，旧内部未实现声明
已移除。CMake 只把该实现加入既有 x64 启动静态库；本步不运行客户端，不实现
窗口消息生产者，也不补 `InitLoading` 的第二调用点。

## 光标计时/脉冲取样与额外策略撤销（BUW01，2026-10-08）

[本步数值原证](cg2d_cursor_flag_sampling_restore_spec.md#cursor-flag-sampling-baseline)与[纠正裁决](cg2d_cursor_flag_sampling_restore_spec.md#cursor-flag-sampling-decision)记录逐帧/请求/尾清/鼠标清理四完整根193条708B/六页、六directcall/六indirectcall/27jump/四RET、18引用15父身份，原11标量88B及四28B NN_NOP用途。原数值地址held D68D98、edge D68D9C、pulse D68DA0已核，不按IDA名称猜字段。

入口先保存leftDownTick再清edge；左tick后fresh down做差/发布或保存旧elapsed，右tick后fresh down做差、重新保存左elapsed再发布右elapsed，否则保存旧右值；两保存elapsed左右传入pulse。各清锚/首脉冲分支先保存pulse后写anchor/flag，右已有anchor还须在100判断前保存pulse；helper已恢复这些阶段，不合并两tick或重读两elapsed替代保存值。

撤本文早期state0..10拒绝/canDraw与AddSigned16Wrapping，正常offset改普通S32加法；读图号→Y后独立state/offset→X后独立state/offset、原88B可写/共享表、描述符与RNG次数保。系统分支撤状态域拒绝，原矩形条件/API顺序和POINT/RECT已批零初始化适配、fresh状态缓存及公共尾保，bit_cast位型不属算术wrapper，继续保。
当前LoadingScreen也有真实UpdateCursor调用、ClientInitialization已调用鼠标清理，旧“未接入”仅历史；本步不重新验完整父。三正确请求/尾/状态清理根保持仅S，额外异常策略不保留、不模拟或记等价，完整ABI/像素/owner寿命/输入运行与R8继续。

### BUW01 当前修复限定静态结果

四完整原根193条708B/六页、六数值旗位36条214B编码/地址双证、原11标量88B/四28B用途、两cpp仅十三片段/正确配对与tick-tracker尾链保、光标原计时-pulse-请求-Y-X-API/公共尾阶段、完整机器/派生差异模型、十五文件编码/四锚点及七新限定证据各25来源均通过。文本汇总与Check均退出0。
四旧BUU01过宽I/S已明确fail，previous pass/独立正确坐标事实和更正引用保留，当前修复七新限定证据pass；这是旧snapshot判断失效的历史记录，不是当前代码故障遗留，未删除失败或用新修复改写旧通过。
实际语义1745/有效1738、文档1856（50.43%）、F886/E1039、R7已审448/余2613、LOOP158/INPUT51/RENDER59/STARTUP118；其他域/原分母/差队列/pending18与ECX pending7/closed11和完整门禁保持。未编译、链接、运行或证明完整原父/别名/ABI/显示验收，整体目标继续active。
