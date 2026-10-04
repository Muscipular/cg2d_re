# 旧鼠标核心状态

## 范围

本项还原原 `cg2d.exe` 的旧鼠标核心存储、CRT 静态构造、启动清零、当前位置
赋值，以及登录/角色选择代码已经依赖的坐标读取和三个位域引用。
`World::CursorUpdate` 已在后续单项还原，见 `cg2d_legacy_cursor_update.md`；
六个按键更新函数也已在后续单项还原，见 `cg2d_legacy_mouse_events.md`；
`WindowProcedure@466DC0` 后续已有本体，并由 `ClientEntry.cpp` 的窗口类接入；
完整平台事件/鼠标输入仍未运行验收，见[启动合同](cg2d_startup_flow.md)。

## 连续布局

原连续区间 `[0xD68CF8,0xD68DC4)` 长 `0xCC` 字节。前 `0xA0` 字节是十个
连续的 16 字节 `CryptInt`：索引 0、1 分别是当前鼠标 X、Y；其余八个只按
已知布局保留为按键坐标，不继续猜测各槽业务名。随后字段如下：

| 原地址 | 新端字段 | 宽度 |
| --- | --- | --- |
| `0xD68D98` | `heldButtons` | W32 |
| `0xD68D9C` | `edgeButtons` | W32 |
| `0xD68DA0` | `pulseButtons` | W32 |
| `0xD68DA4` | `leftDownTick` | W32 |
| `0xD68DA8` | `leftElapsed` | W32 |
| `0xD68DAC` | `rightDownTick` | W32 |
| `0xD68DB0` | `rightElapsed` | W32 |
| `0xD68DB4` | `valueD68DB4` | W32，含义未决 |
| `0xD68DB8` | `valueD68DB8` | W32，启动清零后写 `-1` |
| `0xD68DBC` | `drawingSuppression` | W32 |
| `0xD68DC0` | `tailReset` | W32 |

`LegacyMouseState` 对标准布局、可平凡复制、总大小和每个字段偏移均有静态断言。
紧随其后的原 `0xD68DC4` 继续使用 `StartupState.cpp` 中的唯一
`platform::gValueD68DC4`，没有建立第二份存储。原右锚 `0xD68BF0` 与左锚
`0xD68DC8` 各自是独立零初值 W32，不并入连续结构。

## 构造与重置证据

- `unknown_libname_47@[0x506350,0x50652E)` 的构造路径
  `0x506480..0x506516` 按地址递增顺序对十个槽调用
  `sub_401C70`。新端同一翻译单元的动态初始化器保持 X、Y、其余八槽的顺序，
  因而每槽仍各消耗一次共享随机序列。
- `sub_506870@[0x506870,0x50688B)` 共 8 条指令、1 个基本块；
  `0x506871` 设置 52 个双字，`0x50687D` 从 `0xD68CF8` 执行
  `rep stosd`，即清零 `0xD0` 字节，覆盖 `LegacyMouseState` 和既有
  `gValueD68DC4`；`0x50687F` 再把 `0xD68DB8` 写为 `-1`。两个外侧锚点
  不在清零范围内。
- `MouseEvent::Reset_0@[0x506890,0x5068FF)` 共 27 条指令、1 个基本块；
  `0x50689C` 先从第一个参数构造临时 `CryptInt` 并复制到 X，
  `0x5068D0` 再从第二个参数构造并复制到 Y。新端依次调用既有
  `AssignFromInt`，保留每次赋值各消耗一次共享随机数的行为。

## 已接入依赖与未决边界

`login::detail::ReadLegacyMouseX/Y` 现在分别对 X、Y 执行一次既有
`Read_401EC0`，没有缓存读取副作用。`gValueD68D98`、`gValueD68D9C`、
`gValueD68DA0` 改为引用并绑定连续状态中的对应字段，因此既有低 8 位读取不再
依赖虚构的第二份全局量。本项没有定义 `gValueD69768`。

尚未把 `ResetLegacyMouseState_506870` 接入 `InitializeClient`，也未把
`ResetLegacyMousePosition_506890` 接入 `WIN_PROC`。六个按键事件入口已经直接
复用本结构和既有按下追踪器，但仍未接入 `WIN_PROC`；窗口激活门禁、坐标缩放与
拖拽状态继续留在后续入口还原范围。逐帧光标更新已经直接复用本结构的计时、
脉冲、抑制和尾字段，没有建立第二份鼠标状态。
