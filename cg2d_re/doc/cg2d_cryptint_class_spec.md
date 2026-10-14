# LegacyCryptInt class 与整数运算符 SPEC（LCI01）

日期：2026-10-14。状态：LCI-FA0整数左侧布尔比较已收敛、Raw接口退休；LCI06原生构造／析构、LCI07首项直接定义、LCI08无调用分配重载退休、LCI09简单赋值及LCI10的46AC90赋值收敛已落实。仅静态核对，未编译或运行；各阶段历史保留，当前按后续裁决覆盖。

## 目标与源码基线

用户要求将 `cg2d_re` 的 `LegacyCryptInt` 转为 class，通过 operator 与
`int32_t`、`uint32_t` 等整数类型互操作。
当前实现位于 `include/cg/platform/LegacyCryptInt.hpp` 与
`src/platform/LegacyCryptInt.cpp`；原函数状态沿
[全量函数实现表](../plan/cg2d_ida_function_implementation_table.md)，
存储和读取依据[既有合同](cg2d_legacy_cryptint.md)，
有符号算术依据[CQA01](cg2d_cryptint_arithmetic_spec.md)。
本次仅核当前源码及已有合同，不增加原端事实或宣称运行已验收。

当前布局固定16字节；读取更新W8计数，达到原门槛时刷新mask并消费共享RNG。
整数赋值通过临时构造、复制载荷、原生析构消费一次RNG；`SetNorVal`只改encoded。
鼠标、世界对象、场景状态和Yobi有显式构销、不同字段发布顺序及原始字节搬移。
`YobiRuntime.hpp`另有struct前置声明，需要同步改为class。

## LCI01已确认实施方案（阶段历史，生命周期由LCI06覆盖）

1. 改为 `class LegacyCryptInt`，保留五个原字段及public访问、16字节大小和全部偏移。
   本步封装数值操作；字段私有化需要同时迁移已有原始载荷消费者，另行确认范围。
   不新增虚函数、RTTI、额外状态或依赖。
2. 默认构造、析构、拷贝构造和同类赋值保持平凡行为；现有显式Construct／Destroy及
   原始载荷搬移继续有效。增加显式整数构造，直接调用既有有值构造入口一次。
   全局及聚合成员默认构造不额外取RNG，已有显式构造不重复执行。
3. 整数 `operator=` 调用现有 `AssignFromInt`，保持目标token与reserved、一次RNG消费。
   同类普通赋值仍沿当前整对象复制；原 `CopyPayload` 保留仅复制三字段的独立合同。
4. 仅提供一个隐式数值转换 `operator std::int32_t()`；提供显式
   `operator std::uint32_t()`。每次转换只调用一次现有Read，避免多个隐式转换引起歧义。
   读取运算符采用非const接口，明确计数／mask会改变；真正const对象只供载荷快照。
5. 支持 `int8_t/uint8_t/int16_t/uint16_t/int32_t/uint32_t` 的整数构造、赋值及两侧运算。
   本方案不承诺更宽整数、浮点或枚举的专用互操作；如用户需要，先补转换合同。
6. 运算符范围：双向 `+ - *`、六种比较、`+= -=`、前后置 `++`，以及同类的
   `+ - *` 和六种比较。普通算术返回整数，复合赋值／前置自增返回对象引用，
   后置自增返回递增前的int32值。不扩展除法、取余、位运算等尚未确认的运算。
7. 每次整数运算只读取对象一次；同类双对象运算明确先右后左各读一次，
   即使两个引用指向同一对象也不合并。已有同义入口继续保留，避免改动原返回合同；
   尤其 `IntLessCryptRaw_401FA0` 的完整原返回不能直接替换成bool运算符。
8. `int32_t` 运算／比较继续有符号；较窄整数按通常整数提升参与。
   `uint32_t` 混合运算按C++通常转换使用无符号32位值，普通算术返回uint32，比较返回bool。
   uint32构造／赋值按同一32位位模式存入；复合赋值将结果位模式写回同一载荷，
   复用Read与SetNorVal，不额外构造临时加密对象或改变RNG消费。
   例如明文为-1时，`x < 1` 为true，`x < uint32_t{1}` 为false；
   `static_cast<uint32_t>(x)` 得到0xFFFFFFFF。该无符号合同属于新增接口提案。
9. 同步class前置声明，并迁移MainLoopSupport中三个仅初始化token的聚合初始化式，避免新增整数构造误将token当作明文并提前取RNG。其余数值运算调用不批量替换。
   获得确认后实现类接口、必要说明与定位更新，再记录实际静态验证级别。

## CRT 初始化包装的类构造替换

用户追加询问 `InitializeLegacyStartupValues` 这类CRT初始化模拟是否也改为class构造函数。
建议纳入本方案：由真实启动拥有者类的构造／析构接管，不再保留手写CRT构造包装与退出登记。
数值类提供整数构造与operator；启动拥有者类负责这些对象在应用启动／退出序列中的位置。
原始载荷类型的平凡默认构造／析构与启动拥有者的非平凡构造／析构分开约定。

当前 `LegacyMouse.cpp::StartupProtectionValuesStartup` 按顺序调用四个初始化包装，
第一个在LegacyCryptInt.cpp构造两个私有对象，后三个在MainLoopSupport.cpp构造保护对象。
既有合同明确五对象构造序列为5939C8、5939D8、594070、594080、59E548，退出反序；
本次用原生类生命周期承接这一序列，原构造／销毁行为仍实际执行，五次初始RNG消费保持。
按该中央翻译单元中的实际位置构造各组拥有者，与后续世界对象／流配置／场景初始化的相对序保持。
不将对象分散到不同翻译单元的自动初始化，以免依赖链接器排序。

替换时按每组真实存储和消费者成组迁移；保留同一对象身份，移除重复构造／退出登记。
`std::atexit`登记返回及库内失败／异常清理行为改由MSVC对象生命周期承接，需要在实施记录具名，
不能宣称原登记失败语义或机器ABI完全等价。原函数表保留入口身份并更新实际承接关系。
本次范围为CryptInt相关启动拥有者；其他类型的CRT初始化保持原有实现。

## 已确认取舍

建议保留原始载荷的显式生命周期，避免现有构造和全局初始化额外消费RNG。
若要求完整自动构造析构／私有字段，则必须成组迁移相关owner、原始存储与退出接线，
属于更大范围重构，不能只改类型声明。

用户以“ok”确认本方案：原始载荷保持平凡默认生命周期，启动拥有者使用原生构造／析构；uint32采用上述无符号混合语义。

## 验证与交付

核对大小／偏移、标准布局、平凡默认构造／析构／拷贝属性、
重载选择、读取次数、同类读取顺序及整数赋值／复合赋值的RNG合同。
按现行限制只做必要源码静态检查和Git差异检查；不构建、测试、运行或计算摘要。
静态核对不代替编译、链接及运行验收；每个可验证小步精确提交，排除既有外部改动。

<a id="cryptint-class-source-state"></a>
## 当前接口实现

LCI-FA0将原401FA0的八个低字节条件收敛为S32整数<LegacyCryptInt；Raw声明／正文已退休，原地址归标量转换注释，高24位保BSB01历史。

LegacyCryptInt已改为class；整数构造／赋值、单一隐式int32及显式uint32转换、
加减复合赋值、前后自增以及九个同类算术／比较运算符已落实。
整数两侧的普通运算由有副作用的单次int32转换接C++内建运算，较窄整数无需重复重载；
uint32按通常转换参与无符号运算。算术溢出沿CQA01，不新增异常模拟或保护。
默认／整数构造和析构按LCI06实现；同类拷贝构造复制四字段，同类赋值沿LCI13仅复制三个载荷字段，均不读取／取RNG；四字段偏移与16B大小沿LCI12保持。
原始16B载荷单独以RawPayload承接；首项两值在原中央位置直接定义初始化，其余延后值通过std::optional在原位置构造。
LCI08退休无当前动态消费者的类专用operator new/delete；原401CA0销毁引用归入析构，未来动态释放沿默认MSVC服务。
LCI09将98个简单单目标载荷段接入整数operator=，移除60个无其他使用的局部optional；其余延后／多目标／跨段存储保持。
LCI10进一步将46AC90内无观察的临时reserved初始化一并收敛到整数赋值，后续标志／坐标读取／发送原序保持。
LCI11继续将82个直接字段／CopyPayload单目标段接入整数赋值，移除75个无其他使用的局部optional。

<a id="cryptint-startup-source-state"></a>
## 当前启动生命周期实现

原24个相关CRT包装中，首项按LCI07由两个LegacyCryptInt全局定义承接，另23组沿不可拷贝启动拥有者；世界千项表沿LCI06原生构造／析构。
首项两值直接默认构造、退出逆序析构；其余组同一optional为空时不构造值，原位置emplace／reset，16B值布局保持。
两定义与其余拥有者实例均位于LegacyMouse.cpp同一有序初始化位置；组内、组间及相邻非CryptInt初始化序保持。
退出登记、登记失败和C++异常机制沿已确认方案交由MSVC，未宣称旧登记结果或异常域等价。
五个原初始化汇总桥已退休；LCI06将成员／数组完成前缀清理由C++与STL承接，原地址保留。

| 原入口 | 当前原生承接 | 构造 / 清理正文 |
|---|---|---|
| 0x00401C10 | `gStartupValue5939C8` / `gStartupValue5939D8`直接全局定义 | `LegacyCryptInt()` / `~LegacyCryptInt()`；编译器生成有序初始化及逆序退出 |
| 0x00404120 | `cg::platform::ProtectionChecksum1Startup` | `ConstructProtectionChecksum1_404130` / `DestroyProtectionChecksum1_404150` |
| 0x00404160 | `cg::platform::ProtectionChecksum2Startup` | `ConstructProtectionChecksum2_404170` / `DestroyProtectionChecksum2_404190` |
| 0x004041A0 | `cg::platform::ProtectionProcessStateStartup` | `ConstructProtectionProcessState_4041B0` / `DestroyProtectionProcessState_4041D0` |
| 0x00456FD0 | `cg::world::SceneTransitionValueStartup` | `ConstructSceneTransitionValue` / `DestroySceneTransitionValue` |
| 0x00467B90 | `cg::world::PlayerCoordinatesStartup` | `ConstructLegacyPlayerCoordinates_467BA0` / `DestroyLegacyPlayerCoordinates_467BD0` |
| 0x00467BF0 | `cg::world::PlayerSnapshotXStartup` | `ConstructLegacyPlayerSnapshotX_467C00` / `DestroyLegacyPlayerSnapshotX_467C20` |
| 0x00467C30 | `cg::world::PlayerSnapshotYStartup` | `ConstructLegacyPlayerSnapshotY_467C40` / `DestroyLegacyPlayerSnapshotY_467C60` |
| 0x00467C70 | `cg::world::WorldCoordinateState9EE8D0Startup` | `ConstructLegacyWorldCoordinateState_467C80` / `DestroyLegacyWorldCoordinateState_467CA0` |
| 0x00467CB0 | `cg::world::WorldCoordinateState9EE8E0Startup` | `ConstructLegacyWorldCoordinateState_467CC0` / `DestroyLegacyWorldCoordinateState_467CE0` |
| 0x00467CF0 | `cg::world::WorldCoordinatePairStartup` | `ConstructLegacyWorldCoordinatePair_467D00` / `DestroyLegacyWorldCoordinatePair_467D30` |
| 0x00467D50 | `cg::world::WorldMoveTargetsStartup` | `ConstructLegacyWorldMoveTargets_467D60` / `DestroyLegacyWorldMoveTargets_467D90` |
| 0x00467DB0 | `cg::world::WorldMoveStateC821E8Startup` | `ConstructLegacyWorldMoveState_467DC0` / `DestroyLegacyWorldMoveState_467DE0` |
| 0x00467DF0 | `cg::world::WorldMoveState9E03C0Startup` | `ConstructLegacyWorldMoveState_467E00` / `DestroyLegacyWorldMoveState_467E20` |
| 0x00467E30 | `cg::world::WorldMoveState9FB918Startup` | `ConstructLegacyWorldMoveState_467E40` / `DestroyLegacyWorldMoveState_467E60` |
| 0x00467E70 | `cg::world::WorldMoveStateAF0FE8Startup` | `ConstructLegacyWorldMoveState_467E80` / `DestroyLegacyWorldMoveState_467EA0` |
| 0x00467EB0 | `cg::world::WorldMoveStateC821D8Startup` | `ConstructLegacyWorldMoveState_467EC0` / `DestroyLegacyWorldMoveState_467EE0` |
| 0x00467EF0 | `cg::world::RippleGateStartup` | `ConstructLegacyRippleGate_467F00` / `DestroyLegacyRippleGate_467F20` |
| 0x004883F0 | `cg::ui::FirstItemSelectorStartup` | `ConstructLegacyFirstItemSelector_488400` / `ReleaseLegacyFirstItemSelectorAtExit_488420` |
| 0x004998F0 | `cg::ui::SecondItemSelectorStartup` | `ConstructLegacySecondItemSelector_499900` / `ReleaseLegacySecondItemSelectorAtExit_499920` |
| 0x004CD240 | `cg::ui::StallBrowsePetValuesStartup` | `ConstructStallBrowsePetValues` / `DestroyStallBrowsePetValues` |
| 0x00506340 | `cg::platform::MouseRuntimeStartup` | `ConstructLegacyMouseRuntime_506350` / `ReleaseLegacyMouseRuntimeAtExit_506370` |
| 0x005183D0 | `cg::world::PlayerRuntimeValuesStartup` | `ConstructLegacyPlayerRuntime_5183E0` / `ReleaseLegacyPlayerRuntimeAtExit_518400` |
| 0x00518520 | `cg::world::PetRuntimeValuesStartup` | `ConstructPetRuntimeValues` / `DestroyPetRuntimeValues` |

## LCI01阶段静态核对结果（历史）

24组构造／析构各唯一接线、中央声明顺序和禁拷贝声明核对通过；48个原构造／清理正文
及CryptInt后端33个函数正文与改动前逐字保持。五字段声明／偏移断言仍在，新增类型属性断言供后续编译验证。
函数表保留3680个唯一原入口，24个包装更新为原生类承接，其余状态保持；当前源码定位同步。
相关语义记录与处置引用同步，旧CRT登记及生命周期资料明确保留为LCI01前态；当前原生寿命另记，
所有新增当前项的build_verified／runtime_verified均false，四项任务仍开放。
JSON／UTF-8无BOM／LF和Git差异核对、文本汇总器及其-Check完成；只说明源码／台账结构，
不替代编译、链接、数值／正常退出和实际运行验收。未构建、测试、运行、检查产物或计算摘要。

<a id="cryptint-class-merge"></a>
## 命名空间合并裁决（LCI02）

2026-10-14用户指出：`legacy_crypt_int`应合并进CryptInt类。
本项沿用户已确认的class封装方向和子项目第8条工程委托实施，覆盖LCI01第9项暂不迁移数值调用的阶段限制。

1. 保留现有`LegacyCryptInt`类型名；删除`legacy_crypt_int`命名空间，原数值／显式构销函数正文改为对象成员。
   原对象参数由`this`承接；双对象运算仍接收右对象，标量左侧入口由右对象接收左整数。
   保留地址后缀和特殊返回合同，不将全部旧入口替换成语义不同的operator。
2. operator与整数构造直接调用类内成员，不保留类外数值后端或同义兼容包装。
   位模式转换和mask生成改为私有静态成员；不新增数据成员、虚函数或RTTI。
3. 首项启动拥有者改为嵌套类型`LegacyCryptInt::StartupValues`，两对象成组构销入口为私有静态成员；
   中央翻译单元中的实例顺序、同一全局载荷及MSVC退出承接保持。其余23组拥有者职责与接线不变。
4. 所有源码消费者迁移到对象成员调用；参数表达式、读取次数、先右后左读取及字段发布顺序保持。
   原始存储、public字段、平凡默认构造／析构／同类复制及16字节布局仍沿已确认合同，不扩大为全自动生命周期。
5. 同步受影响源码定位、语义映射、验收登记与唯一进度，仅做必要静态核对和文本汇总。
   当前不构建、不测试、不运行、不检查产物、不计算摘要；不提高运行验收状态。

## LCI02合并阶段源码状态（普通运算后续按LCI03／LCI04收敛）

旧命名空间的28个对象操作、2个成组构销和3个私有辅助正文已移入LegacyCryptInt；
operator直接复用成员，所有旧限定／别名／using调用已迁移，源码与头文件无legacy_crypt_int引用。
静态核对33个正文仅改变接收方式，五字段与既有大小／偏移／平凡属性断言保持；
特殊401FA0返回、读计数、RNG及双对象先右后左保持，布局断言尚未经编译器执行。

<a id="cryptint-comparison-operators"></a>
## 普通比较入口收敛为operator（LCI03）

2026-10-14用户继续指出IntGreaterCrypt_401F80这类入口应使用operator。
本项只收敛11个普通bool比较成员及其消费者；标量两侧比较复用已批准的单次int32转换和C++内建operator，
同类比较沿已有friend operator，保先右后左独立读取。原S32形参的转换边界仍保持，不能因调用改写使旧S32比较变成U32比较。
IntLessCryptRaw_401FA0保留完整U32高24位返回合同，作为特殊成员保留；不扩展算术或自动生命周期范围。
移除同义命名比较声明／正文，更新全量表当前承接和语义定位，历史入口身份与验收门禁保持。

<a id="cryptint-native-operators"></a>
## 自增与算术收敛为operator（LCI04）

按用户追加指令，移除Increment、加减乘、复合赋值和整数赋值等9个同义命名成员。
原前置自增、复合赋值、赋值及同类差值正文直接进入对应operator；标量普通算术继续单次int32转换与内建operator。
旧S32参数边界保持；前置自增按已批准接口返回对象引用，当前源码没有旧整数返回消费者。
原地址注释迁移到实际operator／转换承接处，不删除原身份；低层载荷复制／显式构销与特殊原返回保留独立合同。

<a id="cryptint-native-lifecycle-proposal"></a>
## 原生析构与分配释放方案（LCI05历史提案，已由LCI06覆盖）

用户指出DestroyAndFree_401CA0应采用析构函数。原401CF0只恢复token；401CA0先执行401CF0、再按flags bit0决定free。
方案为将401CF0正文放入~LegacyCryptInt，401CA0的释放责任由delete／operator delete承接，仍配对CRT malloc／free，
原地址分别注释在析构与释放边界；不在析构中无条件free(this)，防止销毁栈／嵌入对象时错误释放。
新增非平凡析构与LCI01原“平凡载荷”合同有范围冲突；须同步迁移裸内存／数组和24组启动拥有者的手工构销，
明确每个实际对象何时开始／结束寿命、同一存储重构与前缀异常清理，避免手工与自动析构重复。
在范围确认及这些生产／消费迁移完成前，保持当前显式构销代码，不仅添加析构／删除静态断言以掩盖未迁移存储。
仍保留16字节／字段宽度／地址注释、读取及RNG顺序；不自动恢复构建、测试、运行或文件摘要权限。

## LCI03／LCI04当前实现

普通比较178处、自增／算术／赋值185处消费者已使用operator，20个同义命名成员声明与正文退休。
前置自增、复合赋值、整数赋值和同类运算直接复用原正文；标量运算使用单次int32转换，保留旧S32参数转换。
该阶段保留原地址注释；其后的401CA0／401CF0迁移及当前生命周期沿LCI06。

<a id="cryptint-native-lifecycle-decision"></a>
## 原生构造／析构统一裁决（LCI06）

用户继续明确ConstructZero应为构造函数；结合此前析构与operator指令，本轮统一落实原生数值对象生命周期，
覆盖LCI01平凡默认构造／析构及LCI05范围待确认的阶段限制，不再只改名字或保留同义数值构销成员。

1. 原0x00401C70正文进入默认构造；原0x00401CC0进入整数构造，默认构造委托整数0构造、每次仅消费一次共享RNG。
   原0x00401CF0正文进入析构；原0x00401CA0的释放责任由operator delete配对CRT free，原flags分支由销毁或delete的调用形式承接。
   类使用配套operator new分配，原16字节／五字段偏移与非虚类型保持；原ABI返回／flags不作为新增接口，地址注释保留。
2. 新构析不再平凡，不能保留旧平凡断言作为已通过证据。原原始字节边界用嵌套RawPayload表达同一16字节，
   其构造／恢复只复制原字段、不消费RNG；只用于已有零填充Yobi内存与原始载荷，不代替普通零值构造。
3. 需要延后构造的全局值、数组和可复用临时量采用标准std::optional承载，原初始化位置emplace、原清理位置reset；
   全局承载为函数内静态optional：首次中央构造时先登记空容器，拥有者随后登记，退出先由拥有者reset、后销毁空容器；避免跨翻译单元析构登记顺序造成提前清理。
   空承载不构造CryptInt、不取RNG，访问函数只借同一存储、不懒构造业务值。玩家物品成员别名同步改为运行时引用访问，消除构造前的全局引用绑定。
   普通聚合／数组原生成员构析保持正序构造、逆序清理；移除原逐成员重复构销与数组签名适配，不扩大其他CRT范围。
4. Yobi动态calloc只提供存储；失败分支沿原源码，成功后以RawPayload成员初始化开始对象寿命、不新增十次RNG，
   释放前结束原生对象寿命。两静态预览在原两组位置独立原生构造十成员，不在翻译单元加载时预构造。
5. 原全对象清零与载荷搬移按实际数据范围处理，保字段发布序、计数／mask／RNG和非CryptInt字段；
   不把有值重构改为普通operator=以免改变token或发布序。源码消费者同步指向同一个承载中的活动对象。
6. 同步函数实现表、当前语义／验收登记与唯一进度，原地址与历史资料保持。
   只做必要静态核对与文本汇总；不构建、测试、运行、查产物或计算摘要，不引外部依赖或委派。

## LCI06阶段源码与静态结果（首项两值由LCI07、分配重载由LCI08覆盖）

默认／有值构造、析构和CRT分配释放已进入LegacyCryptInt；四个同义显式构销成员及首项两汇总成员已退休。
165个可复用临时承载在原214个有值构造位置emplace、原销毁位置reset；实参沿原S32转换，不改为整数赋值。
原24个启动组与世界千项表仍在同一中央顺序管理；成员和数组原生正构逆析，初始2118次RNG消费按原组数量对应，未执行数值样例。
鼠标／主玩家／主宠物／浏览宠物的原整块清零改为普通字段零字节和CryptInt::RestoreRaw，保留token为零的原状态，不新增构造RNG。
浏览宠物左移由原生记录复制承接，显式同类复制包含全部字段、不读取或取RNG；非业务填充字节不作为新ABI合同。

Yobi的三次零填充分配在原分配／失败位置接std::construct_at(RawFields)，四处释放分支先std::destroy_at再free。
两个预览独立optional在原调用位置各构造十成员；已核Sub_4C61B0只有两个固定启动调用。
preview非拥有指针改为对象完成构造后发布，flags|2仍在其他选择器字段写入前持样；这项绑定时点变化由原生对象寿命承接。
普通动态节点的十CryptInt由RawPayload初始化，不新增十次RNG；分配失败消息、payload释放和其余业务发布顺序保持。

必要静态核对覆盖旧构销引用归零、214个临时有值构造数量／次序、成员及中央序列、五字段／偏移、
原地址与周边注释、源码括号配对、编码及差异；布局断言等待后续明确恢复编译后验证。
函数表、当前语义记录和登记引用同步；历史字段／验证门禁保留。文本汇总与-Check仅核台账，不代替编译、链接或运行。

<a id="cryptint-direct-global-definition"></a>
## 首项两值直接定义初始化（LCI07）

用户指出gStartupValue5939C8适合直接在定义处初始化，本步收敛同组5939C8／5939D8两值。
把两项LegacyCryptInt定义放在LegacyMouse.cpp原gStartupValues的中央位置，先5939C8、后5939D8，后续保护值／世界表顺序保持；
默认构造各取一次共享RNG，退出由MSVC全局对象析构按相反声明序承接。共享种子gRandomSeed=1仍为常量初始化。
若把有副作用的定义留在LegacyCryptInt.cpp，就会依赖跨翻译单元的动态初始化顺序；本步将定义统一留在既有有序中央翻译单元。

移除本组optional存储、BorrowStartupValuesStorage及LegacyCryptInt::StartupValues包装，不复制第二份载荷，不扩其他23组或可复用临时量。
原0x00401C10／0x00401C20／0x00401C50及两原全局地址注释移到实际定义旁，原入口仍以MSVC生成的全局构造／析构承接记录。
覆盖LCI06对这两值延后承载的阶段实现；其余原生类、16B布局、读取、RNG和全部门禁保持。
同步当前函数表／语义与验收引用、唯一进度，仅静态核对及文本汇总，不构建／测试／运行／查产物／计算摘要，不引依赖或委派。

### LCI07源码与静态结果

两直接定义位于LegacyMouse.cpp原中央位置，默认构造各消费一次RNG，退出按5939D8→5939C8析构；后续声明／正文保持。
本组optional／BorrowStartupValuesStorage／StartupValues已退休，原地址注释无丢失，数值构造后的核心正文保持。
三原入口仍有承接，504处受影响源码行号同步；五条语义、五条处置、两功能域及C02／C04两开放任务引用更新。
3680入口身份／状态、既有历史和编译／运行门禁保持；UTF-8无BOM／LF、Git差异及文本汇总器／-Check通过。仅静态核对，不构建、测试或运行。

<a id="cryptint-unused-allocation-overloads"></a>
## 移除无调用的分配重载（LCI08）

用户要求无调用时归入构造／析构并保留注释引用。当前src／include中只有类operator new/delete声明与定义，
未见LegacyCryptInt的动态new/delete、智能指针拥有或派生类型；指针使用沿既有成员／启动存储借用。
按工程委托裁决移除两个无消费者重载及仅供它们使用的头文件；对象初始化／清理继续由现有构造／析构承接，
原0x00401CA0销毁引用归到~LegacyCryptInt声明和定义注释，原401C70／401CC0／401CF0与其他地址保持。
分配内存与对象寿命分别由标准new/delete和构造／析构负责；不向构造函数添加malloc，不在析构函数free(this)。
本步覆盖LCI06专用malloc/free重载合同：类未来动态分配沿默认MSVC分配／释放，不承诺原flags／返回ABI或显式free行为。
当前没有该类动态分配消费者，不改变已有对象存储、生命周期、16B布局、RNG或数值正文，不扩其他对象的分配域。
同步原401CA0当前承接、语义／登记引用与受影响源定位；原端事实、阶段历史和全部运行门禁保持。
仅必要静态核对与文本汇总，不构建、测试、运行、查产物、计算摘要或委派，不引依赖。

### LCI08源码与静态结果

两个类专用重载及cstdlib／new头文件已删除，401CA0引用保留在析构声明／定义；构造、析构、读取、RNG和其余数值正文保持。
27处受影响源行号同步，原401CA0当前提供者改指析构；3680入口身份／状态保持。
一条语义与一条处置、两功能域和C04开放任务引用同步；旧专用分配绑定保留为历史，原身份与编译／运行门禁保持。
UTF-8无BOM／LF、Git差异、完整JSON与文本汇总器／-Check核对通过，仅静态，未构建或运行。

<a id="cryptint-runtime-integer-assignment"></a>
## 运行期整数赋值收敛（LCI09）

用户要求将简单临时构造／载荷发布改为原生对象或直接赋值，并询问定义处{0}初始化。
当前目标已在启动拥有者中完成构造；定义处初始化不能替代运行期重新赋值。同类operator=沿整对象复制，
会覆盖目标token／reserved；整数operator=已在其内部构造临时值并CopyPayload，只写encoded／mask／readCount、一次共享RNG，不解密目标。
因此直接使用目标=原S32值，保留原value求值及每次构造的RNG序，不改同类拷贝／原始载荷／多目标持值合同。

本步识别6源中98个完整连续段：emplace整数→三个字段持样→单一已存目标的三个对应字段写入→reset，
捕获值在该段后不再消费；目标只为现有成员／借用指针或无参Borrow存储访问，段内没有插入其他业务调用。
转换仅将这些段接入既有整数operator=；若某容器所有后续使用都在这些段内，再删除其声明，共60个。
其余延后存储、多目标／分支／跨段捕获保持，不把所有optional一概去除；原整数转换、字段宽度、RNG次数及赋值位置保持。
采用CopyPayload统一源读／目标字段写序（encoded→mask→readCount），替代局部原三字段存储序；
同一段内无业务观察或其他字段发布，目标token／reserved不写，完成后才继续原业务。Borrow访问收敛一次，沿已有原生存储，初始化／退出位置不动。
这是用户要求的operator封装范围内的源码次序收敛，原端逐字段次序／地址注释保留为历史，不宣称原机器指令或完整运行已验。
原构造／析构／CopyPayload／operator正文及16B布局不改，无新增RNG或原始零载荷构造。
源码、全量函数表定位、当前语义／登记及唯一进度同步；仅必要静态核对与文本汇总，不编译、测试、运行、查产物、计算摘要或委派。

### LCI09源码与静态结果

6源98个连续单目标赋值段已使用整数operator=；60个无剩余使用的局部optional退休，另105个持值容器保持。
精确C++作用域核对捕获值不再消费、容器无遗漏使用；15个Borrow访问正文仅返回既有optional存储，不附带业务或RNG；空容器首次登记及目标构造仍沿原启动位置。
原类正文／16B布局、现有构造／析构、RNG及启动／退出保持；原地址和局部旧次序注释保留，变化范围仅识别段、无用optional头文件及一处失去用途的lambda捕获；该lambda仅被本函数四次直接调用。
全量表55个受影响原入口按现有提供者定位，1049处源码行号同步；54条既有语义、53条既有处置、两功能域与C04开放任务引用同步。
没有既有语义／处置项的入口仍以全量表为准，不猜原端事实或新增完成门禁；3680入口身份／状态、全部历史和验收门禁保持。
UTF-8无BOM／LF、Git差异、完整JSON及文本汇总器／最终-Check通过；仅静态，不构建、测试或运行。

<a id="cryptint-46ac90-integer-assignment"></a>
## 46AC90临时保留字节与整数赋值（LCI10）

用户要求继续简化Sub_46AC90。原[0x46AC90,0x46AD00)当前先调用4681B0，再构造整数1临时值、
清零临时reserved并仅发布mask／encoded／readCount，销毁后按标志→Y读取→X读取→510810发送。
临时reserved的三次零写没有被目标复制、读取或析构消费；整数operator=只复制三个载荷字段，仍一次共享RNG。
按工程委托裁决将该段收敛为*BorrowValueC821D8Storage()=std::int32_t{1}，临时保留字节无观察，目标token／reserved保持。
沿LCI09将局部三字段写序交既有CopyPayload，Borrow访问一次；该段内无其他业务观察，初始化／退出与后续发布／读取／请求原序不动。
仅优化此函数的局部赋值；不改变原生构造、析构、RawPayload或其他函数逻辑，保留原地址和临时reserved的历史说明。
当前整数赋值、CopyPayload和析构正文静态核对；用户在LegacyCryptInt.cpp中的既有未提交修改保留且不纳入本步提交。
同步本入口语义／处置、受影响SceneState源码定位和唯一进度，原身份／状态及全部验收门禁保持。
仅静态核对与文本汇总，不构建、测试、运行、查产物、计算摘要或委派，不引依赖、不打tag。

### LCI10源码与静态结果

46AC90的8行临时段已改为注释＋整数赋值两行，移除一个局部optional与三次无观察的临时reserved零写。
原源码前后缀逐字保持，整数赋值／CopyPayload／析构依赖按当前正文静态核对，赋值一次RNG、目标token／reserved与后续Y→X读取／发送保持。
107处SceneState源码行号同步；本入口沿全量表仍有正文，3680入口身份／状态保持。
新增一条仅当前源码重构的语义和一条有界adapted处置，身份沿既有清单，不补原端指令事实；两功能域与C04开放引用同步，旧语义／处置及验收门禁保持。
用户LegacyCryptInt.cpp改动保留并排除，不将其来源变化计为本步；UTF-8无BOM／LF、Git差异、完整JSON及文本汇总器／-Check通过，仅静态，未编译或运行。

<a id="cryptint-additional-integer-assignment"></a>
## 其他简单临时赋值收敛（LCI11）

用户询问其他类似情况；继续既有operator与原生生命周期迁移，按工程委托裁决处理当前3源82个完整单目标段：
38段为整数emplace→CopyPayload→reset，44段为对应三字段直接／局部持样发布→reset。
目标均为已有成员或无参Borrow既有存储，三个载荷字段只发布一次；局部捕获在段后无消费，段内没有其他业务调用／分支／多目标发布。
整数operator=保留原S32表达式求值，每段内部构造／一次共享RNG／CopyPayload／析构；目标token／reserved保持，目标不解密。
含TakeBattleMInteger的表达式按赋值右侧先于左侧求值，原字段依次赋值与取包顺序保持；
目标寻址仍为既有指针／索引读取，NextMask仅调用共享RNG，不改目标引用。
沿LCI09统一局部三字段发布为CopyPayload的encoded→mask→readCount顺序；旧地址及旧逐字段次序注释保留并标明当前承接，不能宣称原机器指令序等价。
仅当容器在精确C++作用域中的全部引用均落在已识别段内时移除声明，共75个；另29个局部容器的中途目标取样、分支发布或跨段持值继续保持。
若复用容器尚有其他段，仅改已验证为空后emplace的本段，保留其余声明／构造／销毁时点；本步此类仅SceneState的first末次赋值。
不改同类整对象赋值、原始载荷恢复、启动／退出或用户已有LegacyCryptInt.cpp修改。
同步受影响函数定位／既有语义与处置／任务引用和唯一进度，只作静态核对及文本汇总，不构建、测试、运行、查产物、计算摘要或委派，不引依赖、不打tag。

### LCI11源码与静态结果

3源82个完整单目标段已改为整数operator=：YobiRuntime76段／71容器、SceneState5段／3容器、WindowRuntime1段／1容器；38段原用CopyPayload，44段原直接／持样复制三个字段。
75个无其他引用的局部optional退休，另29个局部容器保持；SceneState的first初始构销跨后一个赋值，其原时点保持，仅末次空容器赋值收敛。
精确作用域核对三个字段对应、捕获不再消费、全部容器引用覆盖；原注释和数值／地址字面量保持，三处旧存储序注释后补当前整数赋值说明。
取包表达式仍右侧先于目标求值，每段一次共享RNG、目标token／reserved不写，后续字段取样／业务语句保持；类正文和用户既有修改未动。
1401处受影响源行号同步，3680入口身份／状态不变；7条既有根语义和6条既有处置、两功能域与C04开放引用同步。
没有既有语义或完整父处置的入口沿全量表，不补原端指令事实或新的父完成处置；当前源段静态记录不替代构建／链接／运行门禁。
UTF-8无BOM／LF、完整JSON、历史／门禁及Git差异核对通过；文本汇总器及-Check通过3680函数／218导入／3061区间／184叶功能／60任务。
按同一cg2d_re分支精确提交，排除用户既有改动与暂存删除；仅静态，不构建、测试、运行、查产物、计算摘要或委派，不引依赖、不打tag。

<a id="cryptint-integer-left-bool-comparison"></a>
## 整数左侧比较去除寄存器模拟（LCI-FA0）

用户确认将401FA0的现有低字节条件改为普通operator比较。当前源码八处调用（4417E0两处、464E30四处、465430两处）全部显式取uint8后判非零，完整返回的高24位没有当前消费者；既有BSB01原叶及八TEST AL证据保留，不重复取证或新增原端事实。
八处改为原S32左整数<右LegacyCryptInt，沿既有隐式int32转换单次Read_401EC0；右对象、原S32转换、短路顺序及计数／按阈值刷新RNG保持。左值仅为已取样局部整数／几何字段／无读取副作用的输入坐标，右侧存储借用沿现有owner。
删除IntLessCryptRaw_401FA0声明与正文，原0x00401FA0归入标量转换／内建比较注释；不新增标量重载，不改同类比较、布局、构析、整数赋值或Read正文。原完整EAX返回作为BSB01历史保留，本次明确只承接已知布尔消费者，不宣称未知机器ABI或完整返回仍等价。
该裁决覆盖LCI01／LCI03及BSB01阶段保留Raw接口的要求；同步当前全量函数表承接、语义／登记与唯一进度，历史原事实和运行门禁保持。用户既有RestoreRaw修改保留且排除提交，本次仅必要静态核对与文本汇总，不编译、测试、运行、计算摘要、引依赖或委派。

### LCI-FA0源码与静态结果

五个源码文件仅改八个条件、Raw声明／正文和相关注释；src／include无Raw接口残留，原0x00401FA0保留在转换说明。Read、转换及其它数值正文、字段／布局断言、已有RestoreRaw修改逐字保持。
518处源码定位同步，原401FA0改为有承接；3680入口身份保持，当前2864有正文／688有承接，其余状态不变。四条既有语义、一条处置、两功能域与四个开放任务引用同步，BSB01原事实及阶段历史保留，全部编译／运行门禁保持。
UTF-8无BOM／LF、八处条件方向／读取、完整JSON、原身份／历史／门禁及Git差异静态核对通过；文本汇总器及最终-Check通过3680函数／218导入／3061区间／184叶功能／60任务，派生文本一致。仅静态，未编译、测试或运行。

<a id="cryptint-implicit-tail-padding"></a>
## 显式reserved改为尾部填充（LCI12）

用户明确只删除LegacyCryptInt与RawPayload中的两个reserved[3]，保留类、RawPayload构造和RestoreRaw。
当前src／include没有这两个成员的读写，RawPayload消费者仅以空值恢复四个有效字段，既有RestoreRaw已不复制reserved。
删除两处显式数组和对应offsetof断言；原+0x0D～+0x0F以尾部padding注释保留，四个有效字段与sizeof==16断言继续约束现有MSVC默认对齐。
没有增加打包指令或改变对齐，大小与数组步长预计保持16；此次不构建，不能将既有断言计为当前编译已通过。
RestoreRaw仍恢复token／encoded／mask／readCount，无RNG与寿命操作；整数赋值和CopyPayload保持三字段发布、目标token不写。
覆盖LCI01／LCI06阶段五个显式字段及同类复制尾部字节的合同：尾部现为编译器padding，不再保证其内容、清零、复制或保留。
同类默认复制继续复制有效字段；此变化不作为协议／资源格式或其它类型reserved成员的删除许可。
当前源码构析／读取／运算正文不动；提交的必要一致性包含既有RestoreRaw中reserved复制循环已删除，避免声明退休后仍有失效成员引用。
已有比较重构及其它用户修改保持且隔离于本步提交；同步当前有效字段说明、源定位、既有语义／验收引用与唯一进度。
仅必要静态核对及文本汇总，不编译、测试、运行、查产物、计算摘要或委派，不引依赖、不打tag。

### LCI12源码与静态结果

两处reserved[3]和对应offsetof断言已移除，原+0x0D～+0x0F以尾部padding注释保留；sizeof==16及四个有效字段偏移断言保持。
RawPayload／原始载荷构造／RestoreRaw接口保留，RestoreRaw仅恢复四字段；当前构造、析构、复制载荷、读取与数值运算正文保持。
原地址注释保留，24处核心源定位同步；3680入口身份／状态保持，四条既有根语义／四条处置、两功能域和C04开放引用同步。
此前reserved复制循环的既有删除属于本步必要一致性；此前比较重构已独立进入基线，其余用户修改不纳入本步。
尾部padding的字节内容不再作为有效字段保证；当前大小／偏移仅保留断言并按MSVC默认对齐静态判断，本步不构建，不声称编译或运行通过。
UTF-8无BOM／LF、有效字段／尾部填充说明、原地址、布局断言、JSON／历史门禁和Git差异静态核对通过；文本汇总器及最终-Check通过3680函数／218导入／3061区间／184叶功能／60任务。
本步只作静态核对，不将大小／偏移断言视为当前编译通过；按同一cg2d_re分支精确提交，保留其它用户修改，不打tag。

<a id="cryptint-copy-assignment"></a>
## CopyPayload收敛为同类赋值（LCI13）

用户纠正目标为CopyPayload改operator=并确认可以；按工程委托裁决采用operator=(const LegacyCryptInt&) noexcept。
同类赋值仅按encoded→mask→readCount复制，不写目标legacyVftToken，不读取明文、不消费RNG，返回本对象引用；同址赋值沿原三字段读写，无额外分支。
替代原default同类赋值的四字段复制合同；同类拷贝构造仍default，原始FP快照返回／新对象构造继续复制token与载荷。
RestoreRaw／RawPayload和四字段16B布局保持，不把原始恢复改为同类赋值，不改SetNorVal／Read／算术构析正文。
83个外部调用（YobiRuntime79、SceneState4）按原指针／成员来源改为对象=对象；整数operator=内部的一处改为*this=temporary，临时构造及析构时点、一次共享RNG保持。
赋值语法右侧先求值，目标／来源表达式只沿既有指针／索引或五个无参Borrow存储读取；Borrow正文仅返回已由中央启动拥有者构造的存储，无业务／RNG副作用。
原整体类型与同类赋值／optional用途已作定点检查，当前同类直接拷贝场景为拷贝构造和原始快照，未发现需要赋值同时覆盖token的活动调用；成员赋值明确采用新三字段合同。
删除CopyPayload声明／定义／调用，当前源码注释改指同类operator=，原0x00438270与其它地址注释保留。
原438270保持有承接，更新实际operator提供者及源行号／既有语义／验收引用／唯一进度；原入口身份、阶段历史与全部编译／运行门禁保持。
用户已有SetNorVal移到private的头文件修改完整保留，提交仅含本步同类赋值调整；不引入依赖、不委派、不打tag。
仅必要静态核对及文本汇总，不构建、测试、运行、查产物或计算摘要。

### LCI13源码与静态结果

同类operator=(const LegacyCryptInt&) noexcept已承接encoded→mask→readCount复制与目标引用返回；83个外部调用和整数赋值内1处已迁移。
CopyPayload声明／定义／调用及当前源码注释已退休，原0x00438270与其它地址保留；同类拷贝构造、原始快照、RawPayload／RestoreRaw及读取／算术／构析正文保持。
23处核心源定位同步，438270仍有承接，3680入口身份／状态不变；一既有语义绑定更新、旧void绑定存历史、新增一条仅当前源码承接的有界处置、两功能域和C04开放引用同步。
没有新增原端取证或完整运行通过声明；同类赋值不再复制token，原整对象赋值合同由本步具名覆盖，原始恢复仍由既有接口承担。
UTF-8无BOM／LF、来源／目标对应、三字段／返回引用、整数赋值RNG、原地址、完整JSON／历史门禁及Git差异静态核对通过；文本汇总器及最终-Check通过3680函数／218导入／3061区间／184叶功能／60任务。
本步仅静态，不构建、测试或运行；精确提交本步差异，原private调整与其它既有修改保留。
