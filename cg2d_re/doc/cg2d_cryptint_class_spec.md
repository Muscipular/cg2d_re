# LegacyCryptInt class 与整数运算符 SPEC（LCI01）

日期：2026-10-14。状态：用户已确认；接口与CryptInt启动拥有者已实现，当前只做静态核对。

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
整数赋值通过临时构造、复制载荷、显式销毁消费一次RNG；`SetNorVal`只改encoded。
鼠标、世界对象、场景状态和Yobi有显式构销、不同字段发布顺序及原始字节搬移。
`YobiRuntime.hpp`另有struct前置声明，需要同步改为class。

## 已确认实施方案

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

LegacyCryptInt已改为class；整数构造／赋值、单一隐式int32及显式uint32转换、
加减复合赋值、前后自增以及九个同类算术／比较运算符已落实。
整数两侧的普通运算由有副作用的单次int32转换接C++内建运算，较窄整数无需重复重载；
uint32按通常转换参与无符号运算。算术溢出沿CQA01，不新增异常模拟或保护。
默认构造／析构／同类拷贝仍平凡，五字段与偏移保持；新增对应类型属性静态断言。
三个保护对象的仅token聚合初值改为constinit字面lambda，保持原零载荷且不提前取RNG。

<a id="cryptint-startup-source-state"></a>
## 当前启动生命周期实现

24个相关CRT包装由不可拷贝的启动拥有者类构造／析构替代；每个类沿原真实全局存储调用
原构造和逆序清理正文，无第二份载荷。各组原始载荷仍保持16字节和原有显式操作。
全部拥有者实例仍位于LegacyMouse.cpp同一有序初始化位置；组内、组间及相邻非CryptInt初始化序保持。
退出登记、登记失败和C++异常机制沿已确认方案交由MSVC，未宣称旧登记结果或异常域等价。
五个原初始化汇总桥已随直接拥有者实例退休；构造前缀／数组异常清理正文保持。

| 原入口 | 原生启动拥有者 | 构造 / 清理正文 |
|---|---|---|
| 0x00401C10 | `cg::platform::legacy_crypt_int::StartupValues` | `ConstructStartupValues_401C20` / `DestroyStartupValues_401C50` |
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

## 本步静态核对结果

24组构造／析构各唯一接线、中央声明顺序和禁拷贝声明核对通过；48个原构造／清理正文
及CryptInt后端33个函数正文与改动前逐字保持。五字段声明／偏移断言仍在，新增类型属性断言供后续编译验证。
函数表保留3680个唯一原入口，24个包装更新为原生类承接，其余状态保持；当前源码定位同步。
相关语义记录与处置引用同步，旧CRT登记及生命周期资料明确保留为LCI01前态；当前原生寿命另记，
所有新增当前项的build_verified／runtime_verified均false，四项任务仍开放。
JSON／UTF-8无BOM／LF和Git差异核对、文本汇总器及其-Check完成；只说明源码／台账结构，
不替代编译、链接、数值／正常退出和实际运行验收。未构建、测试、运行、检查产物或计算摘要。
