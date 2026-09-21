# cg2d 基础场景状态与 World::Update

日期：2026-09-20。任务：R6.1.1a、R6.1.1b。证据源仅为当前 `ida-32980` 的 `cg2d.exe` IDB 与当前实现源码。

## 1. 函数边界

`World::Update@0x51C430` 的范围为 `[0x51C430,0x51C726)`，共 188 条指令、36 个基本块、74 个 `call` 指令和 49 个唯一直接目标。当前 IDA 原型为 `void()`；唯一代码调用点是 `MainLoop@0x457117`，调用者不消费返回值。

入口严格按以下顺序执行：

1. 单次读取 `gPendingSceneType@0xFF0D6C`，按有符号值判断是否 `>=0`。
2. 成立时依次写当前类型、读取待提交状态、把待提交类型写成 `-1`、写当前状态。
3. 无条件将 `W8[0xC82204]` 清零。
4. 单次读取当前类型，进入主分发。

## 2. 状态槽与切换原语

| 原地址/函数 | 当前实现 | 边界 |
| --- | --- | --- |
| `0xFF0D40` | `cg::world::gCurrentSceneType` | 引用 `cg::platform::gValueFF0D40`，不建立第二份存储 |
| `0xFF0D60` | `gPendingSceneState` | 独立 `int32_t`，映像初值 0 |
| `0xFF0D64` | `gCurrentSceneState` | 独立 `int32_t`，映像初值 0 |
| `0xFF0D6C` | `gPendingSceneType` | 独立 `int32_t`，映像初值 0；`-1` 由启动/消费流程显式写入 |
| `sub_51C770` | `SetCurrentSceneType` | 先清当前状态，再写当前类型，返回输入类型 |
| `sub_51C790` | `SetCurrentSceneTypeAndState` | 依次写当前类型、当前状态，返回输入类型 |
| `sub_51C7B0` | `SetPendingSceneType` | 先清待提交状态，再写待提交类型，返回输入类型 |

`ParseCmdOpt@0x4669D1` 也写 `0xFF0D40=1`，因此当前类型继续复用参数模块的唯一存储。

## 3. 两级分发表

主表从 `0x51C728` 开始，按 `u32(currentType-1)` 索引 13 个 DWORD；超范围直接返回。

| 当前类型 | 目标 | 当前实现路径 |
| ---: | ---: | --- |
| 1 | `0x51C477` | 清 `0x73ECC0`、`0x4596D0`、五项次表、`0x51C4AD` 四调用尾部 |
| 2 | `0x51C4C6` | 清 `0x73ECC0`、`0x45FEA0`、`0x404C50`、`0x521F20` |
| 3 | `0x51C4E0` | 清 `0x73ECC0`、`0x460FF0`、`0x404C50`、`0x521F20` |
| 4 | `0x51C4FA` | 清 `0x73ECC0` 后执行独立六调用链 |
| 5 | `0x51C55C` | 内嵌完整初始化链，然后切到类型 6 |
| 6 | `0x51C613` | 清 `0x73ECC0`、`0x45E960`、两调用公共尾部 |
| 7 | `0x51C62D` | `0x45EBB0`、`0x404C50` |
| 8 | `0x51C721` | 直接返回 |
| 9 | `0x51C63C` | `World::DrawNormalScene@0x51C7D0` |
| 10 | `0x51C71C` | `World::DrawBattleScene@0x43CBB0` |
| 11 | `0x51C646` | 内嵌状态 0/1 及确认分支 |
| 12 | `0x51C721` | 直接返回 |
| 13 | `0x51C523` | 清 `0x73ECC0`、`0x4596D0`、按有符号 `<3` 选择目标、`0x51C543` 四调用尾部 |

状态 1 的次表从 `0x51C75C` 开始，索引来自单次读取的 `gPaletteCapability@0x9A7F64`。实际五项为：`0/1→0x51C493`、`2→0x51C49A`、`3→0x51C4A1`、`4→0x51C4A8`；无符号越界时跳过次表目标，但仍执行 `0x51C4AD` 起的 `0x404C50→0x521F20→0x4590E0→0x44A410` 尾部。状态 13 使用内容相同但位于 `0x51C543` 的独立四调用尾部，不在新端合并两处原调用点。

## 4. 类型 5 初始化链

`0x51C55C..0x51C612` 保持原顺序：

1. `gValue73ECC0=0`。
2. 依次调用 `4590A0→449DA0→46A9E0→467F30→5199A0→446470→4D3170→455BD0→46E2A0→46BD40`。
3. 清 `W32[5ABFC4]`、`W32[9A6E0C]`。
4. 依次调用 `504A30→43C8F0→504100→518C80→518CB0`。
5. 写 `W32[FF0D5C]=0xFFFFFFFF`，清 `W32[FF0D58]`、`W32[D67E78]`。
6. 依次调用 `509080→518780→503E80→43F220`。
7. 清 `W32[FF0D74]` 与 `W8[9DAD30]`。
8. `SetCurrentSceneType(6)`，调用 `52B990`，最后严格清零 `D67B08` 起始的 7 个 `uint32_t`。

## 5. 类型 11 内嵌状态链

函数单次读取当前状态：

- 状态 0：`519D50→446470→404D30→503E80→43F220`；写 `W32[58B6D0]=1`；调用 `51D750`；写 `gValue73ECC0=2`；调用 `52B990`；读取 32 位 tick，把其无符号数值转换为 `double` 后写 `gLastRenderDt`；最后递增当前状态。
- 状态 1：调用 `51FDE0(2,0,0x3F19999A)`；只有返回值严格等于 1 时才清 `gValue73ECC0` 并递增当前状态。第三参数在新端保持 `uint32_t` 位载荷，不使用截断十进制浮点字面量。
- 其他状态：不执行上述两段。

所有路径随后恰调用一次 `sub_455C90@0x455C90`。返回 0 时进入类型 6 的 `0x404C50→0x521F20` 公共尾部；返回非零时执行 `520160→519D50→446470→5081A0`，调用 `resource::ProbePaletteCapability_45C970`，先按 `0x51C6EA` 比较返回值是否为 3，再由 caller 在 `0x51C6ED` 对应位置把返回值写入 `gPaletteCapability`，调用 `SelectPalette(capability==3 ? 27 : 0, 0)`，切到类型 2，调用 `404D30` 并清 `W8[D9F2CA]`。

## 6. 新增原全局存储

| 原地址 | 新端类型 | 映像初值 |
| --- | --- | ---: |
| `C82204` | `uint8_t` | 0 |
| `5ABFC4/9A6E0C/FF0D5C/FF0D58/D67E78/FF0D74` | 各自独立 `uint32_t` | 0 |
| `9DAD30/D9F2CA` | 各自独立 `uint8_t` | 0 |
| `D67B08` | `std::array<uint32_t,7>`，严格 28 字节 | 全 0 |
| `58B6D0` | `uint32_t` | 1 |

`gPaletteCapability@0x9A7F64`、`gValue73ECC0@0x73ECC0` 与 `gLastRenderDt@0x73ECC8` 均复用既有唯一存储。读取 tick 通过返回 `uint32_t` 的最小平台适配函数完成，world 公开接口不暴露 Win32 类型。

## 7. 依赖与当前完成边界

49 个唯一直接目标中，`SetCurrentSceneType`、`SelectPalette` 与 `ProbePaletteCapability_45C970` 已有实现，`GetTickCount` 由平台边界适配；`nullsub_1@0x509080` 已按唯一 `retn` 改接 `cg::NullSub_509080()`，其余未实现目标只在 `WorldUpdateDependencies.hpp` 保留带原地址与当前 IDA 调用契约的声明，不提供固定返回实现。`WorldUpdate` 已直接调用唯一公共探测入口并移除重复占位声明；`MainLoop` 的原唯一调用点已直接改接 `cg::world::Update()`，旧 `cg::platform::UpdateWorld_51C430` 声明已移除。

本步实现的是基础状态消费、分发和函数内两条内嵌链；`LoginScene::Process@0x45FEA0`、`sub_460FF0`、`sub_4626A0`、`sub_45E960`、`sub_45EBB0` 以及正常世界、战斗目标都仍未实现。登录相关 scene 继续属于 R6.1.2；静态库允许保留这些未解析依赖，完整链接和运行不属于本步完成范围。
