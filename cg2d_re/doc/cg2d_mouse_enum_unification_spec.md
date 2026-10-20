# 鼠标状态与 UI 事件枚举统一 SPEC（MEU01）

状态：待用户确认，尚未实施代码。日期：2026-10-20。

## 1. 目标

参考 LuaUI 已使用的 `MouseFlags`，统一鼠标相关常量的定义来源与命名，覆盖 `LegacyMouse.cpp`、`src/ui/` 及其 `windows/`、`controls/` 子目录。替换已确认的鼠标魔法数字，保持数值、字段布局、事件产生与消费行为。

本次统一分为底层鼠标状态与 UI 事件两层。两层位值不同，不能直接合并成同一枚举或将 UI 标志写入底层状态。

## 2. 当前源码依据

- `include/cg/platform/LegacyMouse.hpp`：`heldButtons`、`edgeButtons`、`pulseButtons` 均为 `std::uint32_t`。
- `src/platform/LegacyMouse.cpp`：按下写入左右按钮位；抬起清除持有和脉冲位并写入抬起边沿；双击同时写入按下和双击边沿。
- `src/platform/LegacyCursor.cpp`：维护按住后的重复脉冲，并按帧清除边沿状态。
- `src/ui/WindowComponentHost.hpp`：目前在 `cg::ui::component_detail` 定义 `MouseFlags`，同时提供底层状态别名。
- `src/ui/controls/UIControl.cpp` 和 `src/ui/WindowRuntime.cpp` 的 `WindowRuntime_AppendMouseEvents`：分别将底层状态映射为 UI 事件位。
- `src/script/LuaConstants.cpp`：将上述 UI 枚举导出为 Lua `MouseFlags` 表。
- `src/ui/windows/`：同时存在底层状态判断和 UI 回调事件判断，例如库存、银行、商店、交易、战斗及地图窗口；必须按数据来源分别替换。

以上依据为当前源码，不表示本次重新完成原端取证或运行验收。

## 3. 枚举定义方案

### 3.1 底层按钮与边沿

在既有 `include/cg/platform/LegacyMouse.hpp` 的 `cg::platform` 内新增两个非作用域枚举，底层类型均为 `std::uint32_t`。采用具名前缀，支持现有整数位运算，不增加运算符重载。

`LegacyMouseButton` 用于 `heldButtons` 与 `pulseButtons`：

| 名称 | 值 | 含义 |
|---|---:|---|
| `LegacyMouseButton_None` | `0x00` | 无按钮位 |
| `LegacyMouseButton_Left` | `0x01` | 左键位 |
| `LegacyMouseButton_Right` | `0x02` | 右键位 |

同一按钮位在 `heldButtons` 中表示持续按住，在 `pulseButtons` 中表示当前脉冲；两字段保留各自时序，不互相替代。

`LegacyMouseEdge` 仅用于 `edgeButtons`：

| 名称 | 值 | 含义 |
|---|---:|---|
| `LegacyMouseEdge_None` | `0x00` | 无边沿 |
| `LegacyMouseEdge_LDown` | `0x01` | 左键按下 |
| `LegacyMouseEdge_RDown` | `0x02` | 右键按下 |
| `LegacyMouseEdge_LUp` | `0x04` | 左键抬起 |
| `LegacyMouseEdge_RUp` | `0x08` | 右键抬起 |
| `LegacyMouseEdge_LDblClick` | `0x10` | 左键双击 |
| `LegacyMouseEdge_RDblClick` | `0x20` | 右键双击 |

双击更新中的 `0x11`、`0x22` 分别写为对应的 `Down | DblClick`，不能只保留双击位。三个状态字段继续采用原整数类型，不修改结构体布局或函数签名。

### 3.2 UI 与 Lua 共用事件枚举

将现有 `MouseFlags : std::uint32_t` 从 `WindowComponentHost.hpp` 移至既有公共头 `include/cg/ui/WindowRuntime.hpp`，归属 `cg::ui`。原生窗口、组件和 Lua 桥接使用同一份定义，不要求平台层依赖 UI 或脚本头文件。

| 现有名称 | 保持的值 |
|---|---:|
| `MouseFlag_None` | `0x000` |
| `MouseFlag_Hover` | `0x001` |
| `MouseFlag_LDownEvent` | `0x002` |
| `MouseFlag_RDownEvent` | `0x004` |
| `MouseFlag_LHoldPhysical` | `0x008` |
| `MouseFlag_RHoldPhysical` | `0x010` |
| `MouseFlag_Dragging` | `0x020` |
| `MouseFlag_DragFinish` | `0x040` |
| `MouseFlag_Leave` | `0x080` |
| `MouseFlag_LDblClickEvent` | `0x100` |
| `MouseFlag_RDblClickEvent` | `0x200` |
| `MouseFlag_LRepeatDown` | `0x400` |
| `MouseFlag_RRepeatDown` | `0x800` |

Lua `MouseFlags` 表的键名、值及回调参数保持现状。当前 `onClick` 按左键按下事件派发，不改为抬起触发；不新增 Lua 抬起事件。

## 4. 修改范围

| 范围 | 拟调整内容 |
|---|---|
| `include/cg/platform/LegacyMouse.hpp` | 定义底层枚举，补充三个状态字段的语义注释 |
| `src/platform/LegacyMouse.cpp` | 替换按下、抬起、双击更新中的位常量 |
| `src/platform/LegacyCursor.cpp` | 替换重复脉冲按钮参数、右键判断及边沿清零常量 |
| `include/cg/ui/WindowRuntime.hpp` | 承接唯一 UI 事件枚举及用途注释 |
| `src/ui/WindowComponentHost.hpp` | 移除原枚举定义；既有 `CG_MBTN_LEFTDOWN` 保留为底层左键枚举的兼容常量，避免扩大脚本调用方修改 |
| `src/ui/` 全部源码及头文件 | 按来源替换底层鼠标位、UI 事件位及组合掩码，包括事件生产、转换、回调分发与消费 |
| `src/ui/windows/`、`src/ui/controls/` | 包含在上一项中，覆盖窗口和控件的直接读取、局部副本及组合事件判断 |
| `src/script/` | 核对枚举迁移后的名称可见性与 Lua 导出；仅在必要时调整限定名或包含关系 |

UI 内 `CG_MBTN_LEFTDOWN` 调用按来源改用按钮位或边沿位。其他模块既有调用可继续通过兼容常量取到相同数值，不保留第二份数字定义。

不扩大到 `scene/`、`world/`、`render/` 及其他平台文件的批量清理。本次不迁移键盘状态、窗口状态、拖放业务阶段、物品属性或鼠标区域载荷的独立控制标志。

## 5. 替换与行为保持规则

1. 根据变量赋值来源和调用链判断位域，不能仅凭 `events`、`flags`、`buttons` 等变量名替换。底层状态局部副本同样使用底层枚举。
2. 保留 `WindowRuntime_AppendMouseEvents` 和 `UIControl` 当前转换位置与执行条件，只替换输入掩码与输出事件常量，不新增通用转换函数或抽象层。
3. 已确认的 UI 组合掩码展开为现有枚举组合，例如 `0x21` 为 `Hover | Dragging`，`0x60` 为 `Dragging | DragFinish`；保持括号、运算符和比较方式。
4. 保留按低字节读取、强制转换、取反宽度、字段读写次序及重复读取。不得借枚举整理合并取样、移动条件或调整分支。
5. 普通数值零、回调返回值、布尔比较不机械替换为 `None`；仅实际鼠标状态或事件初始化、清零使用对应常量。
6. 不改变命中检测、焦点、拖动捕获、重复脉冲间隔、双击时序、回调顺序及鼠标消费规则。
7. 同值但不同语义的标志不能复用鼠标枚举。遇到来源无法确认的表达式，列出位置与疑点，暂停对应修改并向用户澄清，不猜测含义或把遗漏记为完成。

## 6. 验证与完成条件

实施后仅进行静态检查：

- 核对枚举取值、字段类型和双击组合保持不变。
- 核对原生窗口与控件的底层到 UI 映射逐项一致，Lua 导出名称和值不变。
- 检索范围内直接读取、别名、局部副本、事件赋值与组合判断；逐项确认遗留数字是否属于无关位域，不以简单全文数字替换代替核查。
- 核对公共头依赖及命名空间可见性，平台层不引入 UI 或 Lua 依赖。
- 检查差异只包含上述常量、必要头文件/限定名和注释调整，执行 `git diff --check`。
- 更新唯一进度并精确提交本任务修改，排除用户既有文件。

不编译、不新增或运行测试、不运行客户端、不计算文件摘要。完成结论仅为源码迁移与静态核对完成；编译、链接及实际点击/双击/拖放交互未验证，待用户另行授权。

## 7. 待确认事项

请确认按本 SPEC 实施：底层保留按钮、边沿两个枚举，UI/Lua 共用一份事件枚举；覆盖 `ui/` 全目录，保持现有数值和行为。确认前只提交本设计与进度记录，不修改运行代码。
