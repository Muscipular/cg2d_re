# UI与输入五项活动构析 SPEC

> 2026-10-04；C02/C04局部实施，依[D02/D03与工程委托](cg2d_closure_decisions_spec.md)、[启动生命周期](cg2d_crt_startup_lifecycle_spec.md)。规范两个选择器、浏览宠物、鼠标和数字步进的实际模块路径，不启用全C++90表。

<a id="ui-input-lifecycle-baseline"></a>
## 1. 原槽、固定owner与当前差异

[只读原证据](cg2d_ui_input_lifecycle_evidence.json)保留五根17条/五槽及直接入向。4CD240/506340各两条，尾入的四条登记分别属4CD250/506350，不增原身份或扩大root chunks。另七原聚合/叶身份283条作布局/归属证据，总300唯一指令；其存在不证明完整成员EH/数组SEH或SDK已验。

| 原槽→Init | 构造/动作提供者 | 退出提供者 |
|---|---|---|
| 0x553098→0x477160 | 0x477170 / InitializeNumberSpacing_477170 | 无退出登记 |
| 0x55309C→0x4883F0 | 0x488400 / ConstructLegacyFirstItemSelector_488400 | 0x488420 / ReleaseLegacyFirstItemSelectorAtExit_488420 |
| 0x5530A0→0x4998F0 | 0x499900 / ConstructLegacySecondItemSelector_499900 | 0x499920 / ReleaseLegacySecondItemSelectorAtExit_499920 |
| 0x5530A4→0x4CD240 | 0x4CD250 / ConstructStallBrowsePetValues | 0x4CD270 / DestroyStallBrowsePetValues |
| 0x5530F8→0x506340 | 0x506350 / ConstructLegacyMouseRuntime_506350 | 0x506370 / ReleaseLegacyMouseRuntimeAtExit_506370 |

477160只有尾跳477170，原无登记。选择器每项先构造再立即登记；浏览宠物沿独立五×六值数组，鼠标沿X/Y/八按钮坐标同一十值。各退出保原逆序，不清其他字段，不与主宠物/玩家或窗口动作owner合并。

488420先固定CF3B10、499920固定D2F338，二者进入488430；488447直接取this+78为内嵌预览，依已存成员逆序退出。原没有从可改preview字段重取目标。现有固定退出函数已经用BorrowWindowSelectorPreview(0/1)表达该原生owner；但InitializeItemSelectors的活动lambda会在退出时重读gItemSelector*.preview，存在固定owner合同差异，不能继续保留为活动退出。

目前浏览宠物/鼠标模块直接构造并以自身析构退出，具名Init另沿旧登记；选择器公开包装重复写构造/lambda登记，数字步进模块直接调用叶。全表简单绑定不能消除这些并行路径。

<a id="ui-input-lifecycle-decision"></a>
## 2. 本组执行决定

1. 两选择器、浏览宠物和鼠标四Init保持原构造完成后，改即时std::atexit各自既有固定无参退出，并传出实际宿主S32。宿主仅依D02/D03承接私有退出表；原53B406数值调用保留为基线，不计原体等价/当前源级调用。
2. InitializeItemSelectors保持void接口及模块caller，主体仅依55309C→5530A0转调两个真实Init并逐项丢弃结果，不短路。删除重复构造与可改preview槽lambda；实际退出固定借用两个原owner，不以新清零/拒绝/回滚补差异。
3. LegacyMouse.cpp浏览宠物/鼠标两个适配保类型与位置，构造改调各自Init/丢弃结果，删除自身析构并写平凡析构断言。其他字段、布局、构析/值/RNG、数组helper/typed回调和窗口业务源码不改。
4. 数字步进模块改调用既有477160具名入口，仍只有同一477170动作，不新增构造/登记/析构或RNG。选择器模块仍经规范公开包装调用，无第二路径。
5. 全应用原序接入时须同小步移除这四个当前模块调用，再从原序执行五入口；当前位置只保已有前置条件，不证明跨模块顺序。禁止双执行、once/懒调用、额外owner、补值/保护/回滚/重试/默认成功、自动析构补偿失败或额外随机消费。
6. 本组不重写十成员/鼠标完整EH与数组SEH，原取证保留且完整失败清理/SDK/运行继续待；不以原聚合指令数量计总体覆盖或功能已验收。标准流活动归属、其余表项与音频暂停不由此解除。

本组对选择器的固定退出是明确恢复原合同；宿主私有登记差异与这项修复分别记录，不将恢复原目标描述为新的业务方案或降低原正常验收。

<a id="ui-input-lifecycle-acceptance"></a>
## 3. 局部门禁

| 门禁 | 观察/标准 |
|---|---|
| I | 四真实即时宿主登记与原固定退出；数字步进无新登记；原存储/数值与构析、数组/成员源码不变 |
| W | 四模块位置只接同一provider；选择器按原两项序转调，不重读可改preview作为退出目标；两自动析构取消 |
| S | 五槽/17根+8所属登记指令、固定this/+78与合并归属可追踪；限定变化之外源码全文保持，不拆原合并F身份 |
| R | 禁构建/测试/运行；全应用/快速退出、登记失败、完整成员EH/数组SEH/SDK未验 |

只归档本组I/W/S，C02/C04、完整功能门禁及R8继续开放。

<a id="ui-input-lifecycle-source-closure"></a>
## 4. 本组源码收口

WindowRuntime.cpp两个选择器/浏览宠物及LegacyMouse.cpp鼠标四Init已改实际std::atexit既有固定退出，仍构造完成后立即登记并传出真实宿主S32。InitializeItemSelectors保void接口，依55309C→5530A0只转调两Init/逐项丢弃结果；重复构造与可改preview槽lambda取消。两个原固定退出函数源码未改，活动退出现在固定借用原两个内嵌owner。

LegacyMouse.cpp浏览宠物/鼠标适配保类型和位置，改调用对应Init并取消自动析构，平凡析构以静态断言约束，未编译；数字步进模块改调477160具名入口，原477170动作保持且无新登记。选择器仍经同一公开包装调用。本组四模块位置对应五provider，整表接通须同小步移除调用。

五根、两固定选择器退出、两合并登记载体4CD250/506350及53C8CD共10已有记录同步，1152语义/1149有效ID、18pending/4分类引用保持。五根adapted使已审68→73/3680；300原唯一指令只支撑本组固定owner/归属，完整十成员/鼠标EH与数组SEH/SDK仍未闭，合并块不新增F身份。

归档E-UI-INPUT-LIFECYCLE-I/W/S-20261004三限定局部证据，累计27条，实现固定至Git命名快照cg2d-re-ui-input-lifecycle-source-20261004。C02/C04仍open，关闭源码任务10项、叶验收0/182及完整功能门禁不提升。全应用原序/其余owner/快速退出、完整成员EH/数组SEH/SDK运行仍待；无构建/产物检查/新增或执行测试/运行或文件摘要，音频暂停及原网络格式明文不变。

## 2026-10-10 SIC01：完整正常成员与EH边界

[SIC01](cg2d_selector_construction_exit_spec.md#selector-construction-source-state)补齐选择器完整构造及两个固定构造/退出包装五原根，220条1049B/八页、20E8两目标/24跳两RET。构造/共享退出各有29条136B EH；488420主chunk另有六B间隙，指令字节与跨度分开，488430为其内部共享尾，不造F。同预览十成员构造和逆序析构、flags/开关/图号/三descriptor发布保持，仅一组一处在构造入口持址供Construct/Finish。两静态对象0/1独立，退出固定借用，不重读可改preview；四包装/四helper及两独立std::atexit正确body保持，十注释。四旧历史追加、一新F五处置、四限定S零I/十open；零模板初值有据，空Xrefs不代不可变或alias闭包，完整EH/ABI/全生产消费、全应用正常owner与快速退出/真实生命周期和运行仍待，整体未完成。

既有具名构造后独立MSVC atexit、固定退出对象和First→Second顺序源码保持，不恢复私有CRT/主动新增析构。完整异常/快速退出、全应用正常顺序与SDK/运行仍待；两个helper的原代码属于同构造或共享析构，不新增原函数身份。
