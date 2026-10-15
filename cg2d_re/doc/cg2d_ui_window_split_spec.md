# UI 按窗口类型拆分 SPEC（UWS01）

日期：2026-10-15。用户要求建立子目录，按窗口类型拆分 WindowRuntime.cpp，并将 Input.cpp 中窗口所属的输入上下文一并纳入；沿用户“完成 SPEC 后执行”的授权实施。

## 目录与职责

- `src/ui/windows/WindowBank.cpp`、`WindowShop.cpp` 等按已有 Window<类型>_ 前缀承接窗口函数、专属状态、常量与辅助类型。同族状态栏、聊天、摊位和战斗结果的变体放在同一文件；共享邮件／资料辅助各有对应文件。
- `src/ui/WindowRuntime.cpp` 保留通用窗口创建、事件、绘制、销毁以及控件、拖动、物品提示等共享机制和跨窗口可变状态。
- `src/ui/WindowRuntimeInternal.hpp` 仅供 UI 实现使用，集中共享依赖和拆分后必需的声明。私有辅助在只被一个文件消费时保持文件内可见；确有跨文件调用才提升到内部 namespace 的声明。共享类型与模板按 C++ 定义可见性放在内部头，跨文件常量用 inline constexpr 保持单一实体。
- `WindowRuntimeState.hpp`、公开接口及 `WindowPrototypes.cpp` 的原型对象、顺序和绑定保留；在子项目 CMake 源列表显式登记各独立 cpp，不使用 cpp 包含或扩大源码通配。

## 数据与输入上下文

逐项按实际引用分配窗口专属状态，跨窗口状态保留单一定义和原有 namespace、引用身份及地址注释。变量初始化与同一文件内的相对顺序保持；有跨对象初始化依赖的定义保留在同一所有者文件。类型布局、构造／析构函数和原有启动入口不改。

InputContext 的默认构造只绑定自身字段和自有正文，析构为默认析构；窗口原型只保存其地址。已核到现代窗口实际消费者的上下文移入相应窗口 cpp，`cg::input` namespace 和既有 extern 声明不变。D66CA8 归共享邮件文件，保留普通／宠物发信及旧面板共用；CB03F0 是旧发信面板的独立对象，同归邮件文件而不合并。聊天、快捷语和宠物名称上下文按其业务所有者归位，所有输入焦点切换仍指向原对象。仅旧面板消费、尚无本次窗口文件承接的其余上下文留在 Input.cpp，不推测窗口归属。

## 保持与验证

保留每个函数完整正文、签名、原地址注释、调用与发布顺序，保留所有全局初值、常量文字、字段宽度、数组容量和别名。只允许文件迁移、必要 include／内部声明／链接可见性调整及共享常量 inline 化；不新增业务、包装或依赖。

静态核对每个迁移单元恰好保留一次，逐项对照原函数／类型／变量正文和全部原地址注释，检查跨文件符号声明、文件内私有引用、输入对象唯一定义、CMake 源列表、UTF-8 无 BOM／LF 和 Git 差异。同步唯一函数表、当前源码定位、UI 符号对应表和本次限定追踪，原身份及验收状态保持。未发生本次迁移的历史证据保留，UI 符号对应表记录新位置，旧源码通过 Git 保留。

本线程完成分析、实施、静态核对及提交，不使用子代理；不编译、测试、运行客户端或计算摘要。既有台账历史附件缺失单独记录，不将静态检查计为链接或运行通过。

## 实施结果

已形成 68 个独立窗口 cpp，公共 WindowRuntime.cpp 为 5065 行，内部共享头为 425 行。2168 个原顶层单元逐项迁移，997 个函数定义移出公共 cpp（含共享模板）；仅跨文件必需的 35 项内部声明新增。两处称号字段引用与共享聚合所有者继续留在公共 cpp，避免新增初始化依赖；其它跨文件初值仅为原地址引用或编译期常量。

1138 项 UI 符号对应表、当前函数表及结构化位置已同步；Input.cpp 直接定位中的既有行号偏移和三处旧名称依据唯一函数表与现源码更正，原身份／状态保持。输入上下文定义迁移如下，原地址、对象名称、数量和 cg::input 作用域保留；其余八处仅旧面板拥有的定义继续留在 Input.cpp。

| 输入上下文 | 所属实现文件 |
| --- | --- |
| `gInputContext626AA0` | `src/ui/windows/WindowChat.cpp` |
| `gInputContexts5F9758` | `src/ui/windows/WindowHotkeys.cpp` |
| `gInputContextD66CA8` | `src/ui/windows/WindowMail.cpp` |
| `gInputContextCB03F0` | `src/ui/windows/WindowMail.cpp` |
| `gInputContextCC2080` | `src/ui/windows/WindowMessageBoard.cpp` |
| `gInputContextD2D828` | `src/ui/windows/WindowOwnProfile.cpp` |
| `gInputContextCA0180` | `src/ui/windows/WindowMiniMail.cpp` |
| `gInputContextCB0610` | `src/ui/windows/WindowProfileSearch.cpp` |
| `gInputContextCB0028` | `src/ui/windows/WindowStall.cpp` |
| `gInputContextCC2378` | `src/ui/windows/WindowStall.cpp` |
| `gInputContextCF4E60` | `src/ui/windows/WindowStall.cpp` |
| `gInputContextD2DE70` | `src/ui/windows/WindowCraft.cpp` |
| `gInputContextD2DC10` | `src/ui/windows/WindowPlayerTitles.cpp` |
| `gInputContextD2D4A0` | `src/ui/windows/WindowPetDetail.cpp` |
| `gInputContextD64CB0` | `src/ui/windows/WindowOwnProfile.cpp` |
| `gInputContextD2D670` | `src/ui/windows/WindowOwnProfile.cpp` |
| `gInputContextCAFEC8` | `src/ui/windows/WindowOwnProfile.cpp` |
| `gInputContextC8B1B8` | `src/ui/windows/WindowBlacklist.cpp` |
| `gInputContextCC21E8` | `src/ui/windows/WindowTrade.cpp` |
| `gInputContextCB0878` | `src/ui/windows/WindowQuantity.cpp` |
| `gInputContextCA0018` | `src/ui/windows/WindowBank.cpp` |
| `gInputContextCC49C0` | `src/ui/windows/WindowTextList.cpp` |
| `gInputContextCA02E0` | `src/ui/windows/WindowDialog.cpp` |
| `gInputContextD2E130` | `src/ui/windows/WindowGuildDetails.cpp` |

逐单元正文／原地址、跨文件声明、输入唯一定义、CMake 源列表和当前函数定位静态核对通过；未编译、测试或运行。台账汇总器仍仅报两份既有外部键盘历史附件缺失，未写派生文件，不记全量台账通过。

## 原型与控件数据拆分补充（2026-10-15）

用户进一步要求 WindowPrototypes.cpp 也按窗口归位。本步把每个窗口的 WindowPrototype、WindowElementPrototype 表及专属图片／文本／鼠标／数字载荷定义放入同一个窗口 cpp，所有原地址注释保留。跨窗口共享载荷只保留一个定义，中央 WindowPrototypes.cpp 保存共享载荷、记录索引枚举与71槽窗口指针表。

原70条可变原型记录改为70个具名原型对象，按窗口归属定义；原内部记录数组改为 constexpr 指针索引表，WindowPrototypeIndex 的0～69和值70保持。该数组原本只有本文件的71槽绑定及3处引用别名消费，无连续内存消费或外部引用；引用别名改为直接引用对应具名对象。每个对象仍只有一份，36／37仍指向同一Dialog原型，全部初值和逻辑顺序保持。指针索引表只引用静态对象地址，不读取跨文件动态初值。

默认释放适配函数归入公共 WindowRuntime.cpp，新增内部声明供原型初始化使用；窗口私有载荷保持文件内可见，跨窗口载荷才补充内部 extern。静态核对70条原型初值、71槽映射、3处别名、全部控件绑定与载荷引用，逐项校验迁移单元和原地址；同步当前函数定位。本步同样不编译、测试或运行。

原型补充已完成：中央文件由4379行缩减为294行，70个唯一原型对象与69张控件表按窗口归位，18项共享载荷留在中央文件。1003个原顶层单元逐项核对；原型初值、71槽索引、36／37共享、3处引用别名、控件／载荷绑定及全部原地址保持。原有窗口实现仅尾部追加数据，函数正文和行号保持；默认释放适配移至公共运行文件，1138项符号和当前直接函数定位已核对。未编译、测试或运行。
