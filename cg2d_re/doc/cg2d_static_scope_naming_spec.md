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
| 单函数独占且跨调用保留的简单状态 | 函数内 static；仅在常量初始化、平凡析构、无外部地址依赖及外部复位入口时迁入；经引用传给同步辅助函数的，另核对是否保存引用 |
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
7. 二次复查补充 `render/SdlPresenter.cpp`：`gScreenshotTickD68A78` 仅由 `TakeScreenshotThrottled_503C80` 读写，迁为局部static `lastScreenshotTick`，保留原 `0x00D68A78`、初值0、500毫秒门槛及截图调用后的再次取时与发布。
8. 二次复查补充 `render/SurfaceState.cpp`：索引色与真彩AutoMap各自的blink/tick迁入各自绘制入口；继续以引用传给同步 `DrawAutoMap32`，后者不保存引用。保留四个原地址 `0x0062B07C/0x0062B080/0x0062B094/0x0062B098`，两组状态不能合并，renderer重建不清零。
9. 二次复查补充 `platform/LegacyCursor.cpp`：`gValueD68BE8` 仅由 `UpdateNativeCursor` 比较、更新，可迁为局部static `previousCursorState`；保留 `0x00D68BE8`、初值0及原更新位置，状态变化判断不增加窗口重建复位。
10. 二次复查补充 `ui/windows/WindowInventory.cpp`：背包和装备悬停槽各由一个处理函数读写，分别迁为各自函数内的static `previousHoveredSlot`，保留 `0x0057DA2C/0x0057DA68`、初值-2、悬停变化触发计时复位及各自发布顺序，不能合并成共享悬停槽。

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
- 二次复查补充 `WindowStall.cpp::WindowStall_ConstructBrowsePetValues` 的局部 `StallBrowseData`：由RuntimeInitialization中第三个窗口相关启动对象在既定位置触发，五项各六成员构造及退出逆序析构保持；不因函数名含Construct而改成每次新建对象。
- 二次复查补充 `ClientEntry.cpp::ResolveWindowResourceInstance` 的局部静态模块句柄：在原资源实例缺少目标资源时首次映射，调用处来自WinMain；资源映射与借用图标、光标保持进程寿命。不能改为每次LoadLibrary，也不能在函数返回时FreeLibrary，首次失败后缓存空值的行为保持。

每步完成后核对：唯一实体及使用范围；初值、存储期、构析触发及顺序；常量表逐项相同；函数正文除已列作用域与名称替换外的差异；声明、定义、调用及包含一致；原地址和合同注释保留；UTF-8、换行与git diff --check。必要时补寿命注释，不生成通用状态管理框架或新增测试。

按本次整理延续用户“完成后build.bat”的要求，源码完成后执行 `build.bat Debug` 验证配置、编译和链接；不运行客户端或测试，不检查产物、部署或计算文件摘要。此前构建通过不代替本轮结果。

本线程串行推进已确认条目，每个可验证小步更新唯一进度并精确提交。保留且排除用户既有AGENTS.md修改、采样目录及Python缓存。按本会话复杂优化先给SPEC确认的要求，本方案确认前不修改源码。

## SS05：二次审查发现的覆盖遗漏（2026-10-17，待确认）

本次按用户“再review一次看看有没有遗留”复查当前客户端349份源码、头及包含片段，排除历史tools。词法扫描用于定位候选，再按实际源码核对具名例子，不把扫描数量等同缺陷数量或实施清单。源码未改，以下为方案补充。

### 1. detail 内文件私有函数的链接整理不能只覆盖 Copilot

按名字在当前客户端源码中的出现文件初筛，并按声明词法排除属性后的static，12份CPP中存在146处没有static声明、名字仅在本CPP出现的detail函数定义候选，其中包含SS02已列的Copilot。该筛法也会漏掉不同命名空间的同名私有函数，例如LuaIO的LuaAbsIndex；不能直接按名字跨文件出现就认定共享。前置声明、重载、模板、回调和真实调用必须按所属作用域核实。LoginSavedState等处的 `[[maybe_unused]] static` 已具备内部链接，不属于遗漏。

已直接核对的遗漏包括 `LuaIO.cpp:15/22` 的LuaAbsIndex/IsValidLuaFileName、`LuaConstants.cpp:37` 的AddVkConst、`environment.cpp:113` 的Sub_449570。它们的定义及当前消费者在同一CPP，具名detail仍赋予默认外部链接，应纳入文件私有链接整理。Lua C回调经函数指针注册，不要求函数具备外部链接，保留注册名称及调用约定。

建议补入的范围：ClientInitialization、ClientShutdown、ResourcePaths、AutoBattle、LuaConstants、LuaIO，以及Yobi的environment、map_actor、map_observation、map_transition、window；另加原SS02的Copilot。逐项确认后只加必要的static及同步前置声明，不能整文件改成static。`RenderInput.cpp:31/32` 两个只在本文件读取的IME描述坐标也需明确文件私有归属，原地址与数值保持；`AutoBattle.cpp:104..111` 的匿名结构体实例checkboxList应单独核对，不能被只识别简单变量声明的扫描漏掉。

### 2. 跨模块 detail 依赖仍然存在

此前NS10的“零残留”指跨模块using namespace整域导入，不等于所有跨模块detail访问归零。当前直接限定调用仍有结构遗留，已核对三个实例：

| 提供者及消费者 | 遗留与建议 |
| --- | --- |
| `Yobi/battle_skill_effect.hpp:17/18` → `script/LuaRuntime.cpp:40/51` | InitializeBattleEffects/ShutdownBattleEffects是脚本启动、关闭实际调用的模块边界，仍位于detail且消费者包含战斗内部头。建议将这两个生命周期声明归cg::yobi模块接口，其他内部技能载荷接口继续留detail |
| `world/MailState.hpp:116..125` → `ui/windows/WindowMailRead.cpp:277`等 | MailMessageView及三种邮件读取入口是窗口真实使用的跨模块合同，仍位于world::detail。建议连同签名依赖的类型归cg::world；不能给这些函数加static或为每个窗口复制状态 |
| `scene/character_select/CharacterSelectSceneDependencies.hpp:106` → `Yobi/preview.cpp:73`及多个窗口 | Sub_51B740提供多个模块使用的图号映射，仍通过角色选择内部依赖头暴露。需把真实共享入口与场景编排私有声明分开，命名须先核对完整映射规则 |

UI、登录、角色选择、邮件及地图数学适配边界还有同类候选，跨模块使用本身不证明每个内部布局都应该公开。本次不把全部detail上提，也不以包装函数机械隐藏内部依赖；以上三个具名边界列为建议追加实施项，其余需继续按用途划分。现阶段不能把“detail滥用已全部解决”作为验收结论。

### 3. 单函数状态收拢与命名尚有漏项

已将截图节流、两组AutoMap闪烁、原生光标前态及背包／装备悬停补入SS01第7～10项。它们有跨调用读写用途，局部化时保留static；`gLastSeNo58D4FC`等仅写槽仍按原合同保留，不借本次整理删除。其他单函数命中项只作候选，逐项核对地址借用、重入、回调和复位后才能迁移。

### 4. 生命周期保留清单不完整

已将摆摊宠物静态对象与首次资源模块映射补入SS04。两者当前代码安排有依据，本项修正方案覆盖，不代表已发现这两处存在运行错误。可变局部static还需区分函数内lambda各自的实例，不能只按缩进或行首static搜寻。

### 本次复查结论

客户端词法检查未发现匿名或空namespace，也未发现头文件命名空间作用域的static实体；类静态成员不在该判断内。没有确认新的功能性故障；本次确认的是私有链接、跨模块detail合同、单函数状态和寿命保留清单的遗漏。未编译、运行或新增测试，静态审查不代替本轮实施后的构建及行为验收。此补充及SS01～SS04均仍待方案确认。
