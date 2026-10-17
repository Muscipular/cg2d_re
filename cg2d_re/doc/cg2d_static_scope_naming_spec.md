# static 作用域与语义命名整理 SPEC

日期：2026-10-17。状态：调查及方案完成，待用户确认；尚未修改源码。

## 目标与边界

承接已完成的命名空间整理，落实用户追加的两项要求：合理安排文件静态变量与函数局部静态变量；对作用明确的函数、变量合理命名，保留原出处地址注释。

本轮根据当前源码、就近注释及实际引用判断归属，不维护历史函数表或 JSON 台账。仅调整下列具名范围及其声明、调用方；不批量替换全部地址名称，不改变业务分支、字段宽度、初值、随机数消费、对象数量及资源所有权。历史 tools 不在范围内。

## 安排原则

| 实际使用情况 | 处理方式 |
| --- | --- |
| 仅一次调用所需的临时值 | 普通局部变量；不能为了减少栈变量而增加 static |
| 只在一个函数按值读取的标量常量 | 局部 constexpr；确认没有地址、引用身份依赖后去除多余 static |
| 只在一个函数使用的固定数组或表 | 函数内 static constexpr，保留静态存储，避免每次重建数组 |
| 单函数独占且跨调用保留的简单状态 | 函数内 static；仅在常量初始化、平凡析构、无外部地址依赖及复位入口时迁入 |
| 同文件多个函数共享的状态、常量或辅助函数 | 文件私有作用域；函数和可变对象显式 static，避免用 detail 名称代替内部链接 |
| 跨文件真实共享的对象、接口 | 保留唯一外部定义及对应模块头，不能各文件复制成 static |
| 构造、析构或首次使用有时序要求的对象 | 保留原位置及触发入口，不能在文件静态与局部静态之间机械转换 |

语义名称只表达已证用途。移动时携带原注释；原地址只存在于旧标识符的，在移除地址后缀前补到声明或定义旁。常量表需要区分原数据地址和原指令中的立即数出处，不把指令地址写成原全局地址。新增适配代码没有原端地址时不编造地址。

## SS01：单函数状态及固定表收拢

1. `Yobi/environment.cpp`：`gEffectCounter5F9755` 仅在 `Sub_449160` 中递增和判断低六位。迁为该函数局部 `static std::uint8_t effectSpawnCounter = 0`，保留原全局 `0x005F9755` 注释、U8回绕、门禁后递增及失败后的计数行为。函数具体效果类别尚无足够命名依据，本步保留函数名。
2. `platform/MainLoopSupport.cpp`：`gProtectionCheckCounter` 仅在 `CheckProtection_4043B0` 中读写。迁为局部 `static volatile std::uint32_t checkCounter = 0`，保留原 `0x0059E564`、volatile访问、1380次门槛及清零顺序。与 `ResetProtection_4048D0` 操作的另两个共享槽区分，不改变复位合同。
3. `Yobi/battle_attack.cpp`：回旋镖命中特效表仅供 `UpdateBattleAction22_4087C0` 使用，迁入该函数并命名 `kBoomerangHitImages`，保留 `0x0054D43C`。
4. `Yobi/battle_cast.cpp`：`kBattleStatusImages54D4C0` 仅供 `UpdateBattleAction143_410FC0` 使用，迁入函数并命名 `kBattleStatusImages`，保留原常量出处。
5. `Yobi/battle_status.cpp`：进入阶段的声音及图号表只在 `UpdateBattleAction163_4113A0` 使用，迁入该函数并保留 `0x004116DE`、`0x00411708` 的立即数出处。退出阶段两表由163、164两个处理函数共享，继续留在文件私有作用域，不复制。
6. `ui/windows/WindowBattleItems.cpp`：`WindowBattleItems_HandleInventory` 中 `kBombType`、`kMedicineType` 只有按值比较，改为局部 constexpr；等级数组和提示文本数组保留 static constexpr。

以上迁入的可变对象均为零常量初始化、平凡析构的标量，保持整个进程中的单一静态实例。不顺带移动写入用途尚不明确的 `AnimationRuntime.cpp::gValue9A4A48`，不删除仅写状态。

## SS02：文件内共享范围

`script/Copilot.cpp` 的五个寻路函数各有相同的八方向 `dx/dy` 局部 static constexpr 数组。合并为本文件 `copilot_detail` 内的 `kNeighborOffsetsX/kNeighborOffsetsY` 两张常量表；逐处只替换数组引用，保留邻居遍历顺序、斜向阻挡判断、代价及同优先级选择行为。其他函数中的坐标差值 `dx/dy` 是普通局部值，不参与替换。

同文件 `copilot_detail` 当前含未显式限制链接的私有函数和可变状态。实施前按声明、定义及全项目引用逐项核实，对确属本文件的实体补 static，前置声明与定义同步；现有 constexpr 常量已经有内部链接，不为形式统一重复修饰。共享容器不搬进函数，保持原构造、析构及复位调用关系。公开的 `autoCopilot` 等模块入口保留外部链接。

## SS03：作用明确的名称

| 当前标识符 | 拟用名称 | 当前源码依据及同步范围 |
| --- | --- | --- |
| `Yobi/environment.cpp::Sub_448370` | `FindLiveActionByCallback` | 遍历普通节点，返回首个callback相同且deathFlag为零的节点；保留 `0x00448370` 和有效链、空callback、非拥有返回等注释。全部调用均在该CPP，改为static，原仅含此声明的environment.hpp及其唯一include一并移除 |
| `Yobi/preview.cpp::Sub_45E6E0` | `AdvanceCharacterPreviewAnimation` | 角色创建预览16阶段推进；同步Yobi公开声明与CharacterSelectScene两处调用，保留 `0x0045E6E0` |
| `world/SceneState.cpp::Sub_519840` | `FindTitleIndexById` | 按id查原称号表首个匹配索引，未命中返回-1；同步PlayerState.hpp及WindowPlayerTitles.cpp，保留 `0x00519840` |
| 同预览函数的 `sequence` | `kPreviewAnimationSequence` | 每项为动作、持续次数及图号偏移，保留原表 `0x0055B2C0` 注释 |
| `WindowRuntime.cpp::images54F564` | `kPreviewToggleImages` | 与TogglePreview写入的imageIndex3F0及绘制消费对应，补原常量 `0x0054F564` 注释 |
| `WindowRuntime.cpp::images54F574` | `kEquipmentToggleImages` | 与CycleEquipment写入的imageIndex3F4及绘制消费对应，补原常量 `0x0054F574` 注释 |
| `WindowRuntime.cpp::images54F5F4` | `kEquipmentTabImages` | 装备页签按tab与事件状态取图，补原常量 `0x0054F5F4` 注释 |
| `CenterBattleLayoutToWindow` 的 `previousX/previousY` | `appliedOffsetX/appliedOffsetY` | 保存已施加到阵位表的居中偏移，用于计算本次增量；保持局部static及更新位置，属于适配状态，不编造原地址 |

不顺带改字段布局、函数签名、回调类型、返回约定或脚本可见名称。不为旧名称添加兼容别名；同步全部当前源码引用，旧分析文档保留历史名字。

## SS04：保留有意设计的静态寿命并验收

已核对的以下对象保留原安排：

- `MainLoopSupport.cpp::SharedRandomEngine` 的唯一引擎及 `ReadTickCount64` 的首次调用基准；`AutoBattle.cpp` 的局部随机引擎仍在多目标分支首次播种，两次shuffle保持。
- `Yobi/preview.cpp` 两个lambda各自持有的YobiAction预览对象：分别在原入口构造、消费RNG并登记析构，不合并成预先构造的对象数组。
- `GraphicInfo.cpp`、`GraphicRead.cpp`、`GraphicDxCache.cpp` 由初始化入口创建的局部静态容器及其文件指针：与RuntimeInitialization的先后顺序一起保留。
- `PngImage.cpp::GetPngRuntime` 的局部静态缓存：保留首次业务调用构造及先于TextureSurfaceOwner析构的关系。
- `MainLoop.cpp` 帧统计、`WindowLifecycle.cpp` 布局初始化标志、`WindowProcedure.cpp` 跨WM_CHAR的代理项、`WindowShop.cpp` 连按计时、`WindowRuntime.cpp` 诊断去重状态：均有跨调用用途，保留持久状态和现有复位规则。没有用户要求或业务证据时不新增窗口重建、切场景等复位行为。
- `RuntimeInitialization.cpp` 具有构造副作用的静态对象保持原定义顺序，不按名称或模块重新排序。

每步完成后核对：唯一实体及使用范围；初值、存储期、构析触发及顺序；常量表逐项相同；函数正文除已列作用域与名称替换外的差异；声明、定义、调用及包含一致；原地址和合同注释保留；UTF-8、换行与git diff --check。必要时补寿命注释，不生成通用状态管理框架或新增测试。

按本次整理延续用户“完成后build.bat”的要求，源码完成后执行 `build.bat Debug` 验证配置、编译和链接；不运行客户端或测试，不检查产物、部署或计算文件摘要。此前构建通过不代替本轮结果。

本线程串行推进SS01～SS04，每个可验证小步更新唯一进度并精确提交。保留且排除用户既有AGENTS.md修改、采样目录及Python缓存。按本会话复杂优化先给SPEC确认的要求，本方案确认前不修改源码。
