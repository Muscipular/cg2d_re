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
