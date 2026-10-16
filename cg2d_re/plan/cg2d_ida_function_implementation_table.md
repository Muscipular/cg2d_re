# IDA 函数全量实现表

> 2026-10-16 [NKF01](../doc/cg2d_network_plaintext_frame_spec.md)：按用户要求删除无当前调用的旧W16收发帧及其独占短状态，4037D0／403C50／403E50三入口由有正文改为已排除；当前LF明文提供者与接线保持，3680身份／名称保持。仅静态核对，未构建或运行。

> 2026-10-16 [COR10](../doc/cg2d_code_organization_review_spec.md)：按用户要求移除重复点击追踪及上报，9个独占入口由有正文转为已排除；普通鼠标按下入口仍有正文，tq配置按原格式解码后忽略。原3680身份／名称保持，服务器兼容性未运行验证。

> 2026-10-16 COR09：模块MD5核心改由Windows CNG提供，402B70／402BC0／402C50／402D30四行由有正文转为有承接，402BA0仍有承接；三个父入口保留有正文。原3680身份／名称保持。本次编译／链接通过，未运行摘要比对，详见[COR09 SPEC](../doc/cg2d_code_organization_review_spec.md)。

> 2026-10-16 [COR07](../doc/cg2d_code_organization_review_spec.md)：同步50文件的目录迁移路径；原行号为历史定位，不代表本轮重新定位。3680行函数身份、名称与状态保持，仅静态整理，不更新JSON台账。

> 2026-10-15 [UWO02](../doc/cg2d_ui_window_split_spec.md#ui-window-ownership-followup-20261015)：七个单窗口函数、三记录类型／54布局断言、十七状态和两常量归位；3680原身份／名称／状态与共享门禁保持，只作静态核对。

> 2026-10-15 [LWR03](../doc/cg2d_legacy_window_retirement_spec.md#legacy-window-simplification-lwr03)：用户接受取消无显示链旧选择及End残留，六个选择入口改已排除；原3680身份／名称和九个空方法保持，登录／角色共享绘制保留。仅静态核对。

> 2026-10-15 [LWR02](../doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02)：移除312个旧窗口无消费者函数及10组只生产无读者数据的启动初始化，226个有正文库存入口改为已排除；九个空方法状态保持；3680行原身份／名称保留，六个仍有消费者的选择接口和公共绘制保持。仅静态验证，不证明原端不可达或完整功能验收。

> 2026-10-15 [LWR01](../doc/cg2d_legacy_window_retirement_spec.md)：当前工程无消费者的97旧handler及3独占目录初始化已移除，对应99库存入口由94项有正文／5项部分实现改为已排除；4FC730非库存独立入口另记。选择状态和公共Yobi保留。下文旧目录五项缓行属于历史，当前状态以本次99行及实际汇总为准；非原端不可达证明，不提升其余门禁。

> 2026-10-12当前源码核对版。当前 cg2d.exe 已打开 IDA 会话共有 3680 个唯一函数入口；本表按地址升序逐行保留当前名称，名称只作 Markdown 显示转义。状态依据当前 CPP/HPP 定义、明确的共享／内联操作、实际宿主／SDK 服务及对应原入口事实；未继承旧台账的实施状态。
> 本表是后续判断函数实施状态和选择补码入口的唯一依据。旧资料只用于候选符号定位；正文存在、承接、接线／数据缺项和完整验收分开。状态口径沿[本阶段 SPEC](../doc/cg2d_ida_function_implementation_table_spec.md)。
> 2026-10-12，PCP01／TTP01 已明确保持只读静态分析，动态方案不执行、不再等待回复。[旧表内部地址补检](../doc/cg2d_pet_catalog_production_spec.md#legacy-table-interior-address-scan-20261012)仍仅见已知消费操作数，无新静态别名；五项部分实现的缺证内容和状态保持，不以未命中推原不可达。
> 2026-10-12 用户明确不确定项跳到下一函数；旧目录／名单五项继续缓行。随后用户决定“nullsub直接标记空方法，不用再核对”，当前九个nullsub统一标为空方法，已有定位保留，不再核职责、调用或承接，不补CPP包装；原两项待确认已结束核对。其他未证候选仍不猜新增调用。
> 随后用户要求补全五项并按IDA原版落地；当前四个宠物技能入口已将CA0478记录的名称和两行说明落实为[具名字段访问](../doc/cg2d_pet_catalog_production_spec.md#original-catalog-record-fields-20261012)。CPP定义行号不变，交易51源码未改；真实数组容量／定义／生产及原调度仍缺证，五项继续标部分实现，不能把字段整理计作功能补齐。
> 用户最新明确“继续严格按原端证据还原”，不采用保留原页面后新增现代数据兼容接线的提案；该选择已答复，不再待确认或重复询问。[EBP／ESP地址补查](../doc/cg2d_pet_catalog_production_spec.md#original-catalog-stack-base-flow-20261012)没有取得新写者或原调度，五项及其余表行状态保持。

> 2026-10-14 [LCI01](../doc/cg2d_cryptint_class_spec.md#cryptint-startup-source-state)：24个CryptInt CRT初始化包装改由原生启动拥有者构造／析构承接；相应24行由有正文改为有承接，其余入口身份／状态保持。五个改动源文件的定位同步当前源码；本轮只作静态核对，不计编译或运行验收。

> 2026-10-14 [LCI06](../doc/cg2d_cryptint_class_spec.md#cryptint-native-lifecycle-decision)：默认／整数构造和析构进入LegacyCryptInt，首项组／释放／世界表与四数组元素入口采用原生拥有者或成员生命周期；原身份和原地址保留。源定位同步，只作静态核对，完整编译／运行门禁保持。

> 2026-10-14 [LCI07](../doc/cg2d_cryptint_class_spec.md#cryptint-direct-global-definition)：首项5939C8／5939D8在原中央位置直接定义初始化，三原入口仍有承接；退休本组optional及StartupValues。源定位同步，原身份／数量／门禁保持，仅静态核对。

> 2026-10-14 [LCI08](../doc/cg2d_cryptint_class_spec.md#cryptint-unused-allocation-overloads)：无当前动态消费者的CryptInt类专用分配重载退休，原401CA0销毁引用归入析构、动态释放沿默认MSVC。受影响源码定位同步，3680入口身份／状态与验收门禁保持，仅静态核对。

> 2026-10-14 [LCI09](../doc/cg2d_cryptint_class_spec.md#cryptint-runtime-integer-assignment)：98个简单单目标临时载荷段收敛到既有整数operator=，保留一次RNG与目标token／reserved；移除60个无剩余使用的局部optional。源定位同步，原入口身份／状态与门禁保持，仅静态核对。

> 2026-10-14 [LCI10](../doc/cg2d_cryptint_class_spec.md#cryptint-46ac90-integer-assignment)：46AC90中无观察的临时reserved零写与载荷段收敛到整数赋值；后续原序保持。只同步本步SceneState源定位，原入口身份／状态与验收门禁保持，仅静态核对。

> 2026-10-14 [LCI11](../doc/cg2d_cryptint_class_spec.md#cryptint-additional-integer-assignment)：另82个连续单目标构造／载荷复制／销毁段收敛到既有整数operator=，移除75个无剩余使用的局部optional。中途取目标、分支和跨段持值保持；受影响源码定位同步，原入口身份／状态与门禁保持，仅静态核对。

> 2026-10-14 [LCI-FA0](../doc/cg2d_cryptint_class_spec.md#cryptint-integer-left-bool-comparison)：八个整数左侧低字节条件改为普通布尔比较，Raw接口退休，原401FA0改记单次转换／内建比较承接；原身份／历史与运行门禁保持，仅静态核对。

> 2026-10-14 [LCI12](../doc/cg2d_cryptint_class_spec.md#cryptint-implicit-tail-padding)：LegacyCryptInt／RawPayload的两处reserved数组移除，原+0x0D～+0x0F改为隐式尾部填充；四个有效字段、16B断言、构析／数值正文保持。RestoreRaw仅恢复有效字段，相关源定位同步；原入口身份／状态和运行门禁不变，仅静态核对。

> 2026-10-14 [LCI13](../doc/cg2d_cryptint_class_spec.md#cryptint-copy-assignment)：CopyPayload改由同类operator=承接，83个外部调用和整数赋值内1处迁移；三字段复制／目标token／RNG保持，拷贝构造与原始恢复不改。核心源定位同步，原入口身份／状态和运行门禁保持，仅静态核对。

> 2026-10-14 [UTL02](../doc/cg2d_text_encoding_spec.md#unicode-utf16-source-state-20261014)：界面输入/控件/队列/私有排版采用动态UTF-16，同类静态正文采用宽字面量；W绘制/IME与原GBK提交边界接线，旧窄只读接口使用具名兼容视图。同步当前定义及宿主承接定位，3680个原入口身份、名称和状态保持；仅静态核对，不计完整B/I/W/S/R通过。

| 状态 | 实际入口数 |
|---|---:|
| 有正文 | 2468 |
| 有承接 | 746 |
| 空方法 | 9 |
| 部分实现 | 0 |
| 未实现 | 0 |
| 待确认 | 0 |
| 已排除 | 456 |
| 不适用 | 1 |

2026-10-12 进度核对：3680行入口与当前IDA地址集合、名称此前经15页逐项核对零差异；3964处源码引用及9处CMake引用可读。当时2911个正文提供者可定位到定义；本次九个nullsub仅归类为空方法，其中3个已有CPP定义和4个已有承接定位保留，其余2908个有正文及644个有承接不变。序号、身份、名称、提供者均不变，未重新查询IDA；UTF-8无BOM/LF。这不提升全部逻辑、完整接线或运行验收，未实现0不能推为功能缺口归零。

所有3680行均已标记。九个空方法按用户决定结束核对，不计为业务正文、承接或范围排除；当前待确认0。未实现0只表示尚未确认独立缺正文入口，不表示不存在开发缺口；原五项部分实现已按LWR01废弃处理器，原容量／生产仍未证，不计实现通过。

提供者采用本线相对路径、实际符号／操作与定位行号。库内叶的“有承接”仅指当前真实 MSVC／STL／SDL／SDK 服务或已证内联操作，不声称复制旧私有算法或机器 ABI。现有正文只确认存在与非占位，未例行重做完整差异核对；所有完整业务、owner、接线及运行门禁仍按各自合同验收。未见调用仅为当前 CPP/HPP 定位事实，不声称原入口不可达。

| 序号 | IDA 地址 | IDA 函数名 | 是否有实现 | 当前提供者 | 接线／缺项 |
|---:|---|---|---|---|---|
| 1 | 0x00401000 | `sub_401000` | 有正文 | `src/world/Blacklist.cpp:139 — PersistAndReset_401000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2 | 0x00401030 | `sub_401030` | 有正文 | `src/world/Blacklist.cpp:79 — LoadBlacklistFile` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 3 | 0x00401170 | `sub_401170` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 4 | 0x00401190 | `sub_401190` | 有正文 | `src/world/Blacklist.cpp:151 — GetBlacklistCount_401190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 5 | 0x004011A0 | `sub_4011A0` | 有正文 | `src/world/Blacklist.cpp:44 — PersistBlacklistFile_4011A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 6 | 0x00401270 | `sub_401270` | 有正文 | `src/world/Blacklist.cpp:181 — AddBlacklistRecord_401270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 7 | 0x004012F0 | `sub_4012F0` | 有正文 | `src/world/Blacklist.cpp:227 — RemoveBlacklistRecord_4012F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 8 | 0x004013D0 | `sub_4013D0` | 有正文 | `src/world/Blacklist.cpp:157 — ContainsBlacklistRecord_4013D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 9 | 0x00401470 | `sub_401470` | 有正文 | `src/world/Blacklist.cpp:281 — ClearBlacklistRecords_401470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 10 | 0x004014B0 | `sub_4014B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 11 | 0x00401960 | `sub_401960` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 12 | 0x00401AB0 | `sub_401AB0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 13 | 0x00401C10 | `sub_401C10` | 有承接 | `src/platform/RuntimeInitialization.cpp:33 — gStartupValue5939C8 / gStartupValue5939D8` | LCI07在原中央位置直接定义两值，默认构造各一次RNG并自动逆序析构；原地址保持；仅静态核对，编译／运行未验 |
| 14 | 0x00401C20 | `unknown_libname_1` | 有承接 | `src/platform/RuntimeInitialization.cpp:33 — gStartupValue5939C8 / gStartupValue5939D8` | 原401C20两值正序构造由全局定义处默认构造承接；原中央位置与两次RNG序保持；SDK退出／完整运行未验 |
| 15 | 0x00401C50 | `unknown_libname_2` | 有承接 | `src/platform/RuntimeInitialization.cpp:34 — gStartupValue5939D8 / gStartupValue5939C8逆序析构` | 原401C50逆序退出由同一中央两值的原生全局析构承接；MSVC自动登记；完整运行未验 |
| 16 | 0x00401C70 | `sub_401C70` | 有正文 | `src/platform/LegacyCryptInt.cpp:25 — LegacyCryptInt::LegacyCryptInt()` | 零值默认构造委托整数0构造，仍一次共享RNG；原16B／偏移保持；布局／运行未验 |
| 17 | 0x00401CA0 | `sub_401CA0` | 有承接 | `src/platform/LegacyCryptInt.cpp:41 — LegacyCryptInt::~LegacyCryptInt；释放由默认MSVC delete承接` | LCI08无动态消费者，类专用分配重载退休；原401CA0销毁注释归入析构，未来动态释放沿默认MSVC；原flags／返回ABI保历史，运行未验 |
| 18 | 0x00401CC0 | `CryptInt_Init` | 有正文 | `src/platform/LegacyCryptInt.cpp:31 — LegacyCryptInt::LegacyCryptInt(int32_t)` | 整数构造接原发布顺序和一次RNG；不显式写尾部padding；完整运行未验 |
| 19 | 0x00401CF0 | `CryptInt_delete` | 有正文 | `src/platform/LegacyCryptInt.cpp:41 — LegacyCryptInt::~LegacyCryptInt` | 原生析构只恢复token；栈／成员／动态存储实际寿命已迁移；运行未验 |
| 20 | 0x00401D00 | `CryptInt::eq` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建==` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 21 | 0x00401D10 | `sub_401D10` | 有承接 | `src/Yobi/environment.cpp:33802 — action->mapY1F0.value() != floor` | != 比较内联；共享 value，不重建包装；完整业务／运行未验 |
| 22 | 0x00401D20 | `CryptInt::neq` | 有正文 | `src/platform/LegacyCryptInt.cpp:194 — cg::platform::operator!=` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 23 | 0x00401D40 | `sub_401D40` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建>` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 24 | 0x00401D50 | `sub_401D50` | 有正文 | `src/platform/LegacyCryptInt.cpp:201 — cg::platform::operator>` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 25 | 0x00401D70 | `CryptInt::gte` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建>=` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 26 | 0x00401D80 | `sub_401D80` | 有正文 | `src/platform/LegacyCryptInt.cpp:208 — cg::platform::operator>=` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 27 | 0x00401DA0 | `CryptInt::Lte` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建<=` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 28 | 0x00401DB0 | `CryptInt::lt` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建<` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 29 | 0x00401DC0 | `sub_401DC0` | 有正文 | `src/platform/LegacyCryptInt.cpp:215 — cg::platform::operator<` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 30 | 0x00401DE0 | `sub_401DE0` | 有正文 | `src/platform/LegacyCryptInt.cpp:155 — LegacyCryptInt::operator++` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 31 | 0x00401E00 | `sub_401E00` | 有承接 | `src/Yobi/map_transition.cpp:3333 — Sub_446B50` | S16 加值在实际坐标计算中内联；共享 value；完整业务／运行未验 |
| 32 | 0x00401E10 | `CryptInt::add` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建+` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 33 | 0x00401E20 | `sub_401E20` | 有正文 | `src/platform/LegacyCryptInt.cpp:125 — LegacyCryptInt::operator+=` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 34 | 0x00401E40 | `sub_401E40` | 有正文 | `src/platform/LegacyCryptInt.cpp:140 — LegacyCryptInt::operator-=` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 35 | 0x00401E60 | `CryptInt::dec` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建-` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 36 | 0x00401E70 | `CryptInt::dec_0` | 有正文 | `src/platform/LegacyCryptInt.cpp:176 — cg::platform::operator-` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 37 | 0x00401E90 | `CryptInt::mul` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建*` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 38 | 0x00401EA0 | `CryptInt::GetVal` | 有承接 | `src/platform/LegacyCryptInt.cpp:76 — LegacyCryptInt::value` | 原入口为跳转到 401EC0；共用同一读取正文；完整业务／运行未验 |
| 39 | 0x00401EB0 | `sub_401EB0` | 有承接 | `src/Yobi/map_actor.cpp:5257 — Sub_4424B0` | 原读取后 fild 转换；当前实际读取后转 float／double；完整业务／运行未验 |
| 40 | 0x00401EC0 | `sub_401EC0` | 有正文 | `src/platform/LegacyCryptInt.cpp:76 — LegacyCryptInt::value` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 41 | 0x00401F50 | `CryptInt::SetNorVal` | 有正文 | `src/platform/LegacyCryptInt.cpp:60 — LegacyCryptInt::SetNorVal_401F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 42 | 0x00401F60 | `sub_401F60` | 有承接 | `src/Yobi/environment.cpp:33802 — action->mapY1F0.value() != floor` | 标量与读取结果 != 在消费者中内联；完整业务／运行未验 |
| 43 | 0x00401F80 | `CryptInt::gt` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建>` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 44 | 0x00401FA0 | `sub_401FA0` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32左整数内建<` | LCI-FA0八处已知调用只消费AL，改为单次读取与布尔比较；高24位保历史，完整机器ABI／运行未验 |
| 45 | 0x00401FC0 | `CrypyInt::lte` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建<=` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 46 | 0x00401FE0 | `CryptInt::Dec2` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建-` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 47 | 0x00402000 | `sub_402000` | 有承接 | `src/world/WorldActor.cpp:35703 — action->mapX200 == static_cast<std::int32_t>(x)` | 原标量 == 读取值；当前由 CryptInt::operator== 承接；完整业务／运行未验 |
| 48 | 0x00402020 | `sub_402020` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 49 | 0x004021A0 | `sub_4021A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 50 | 0x00402B30 | `sub_402B30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 51 | 0x00402B70 | `sub_402B70` | 有承接 | `src/platform/LegacyModuleDigest.cpp:45 — BCryptCreateHash` | COR09：原手写MD5核心由Windows CNG提供；本次编译／链接通过，未执行摘要比对或性能测试 |
| 52 | 0x00402BA0 | `sub_402BA0` | 有承接 | `src/platform/LegacyModuleDigest.cpp:86 — BCryptFinishHash（16字节标准MD5输出）` | COR09：原手写MD5核心由Windows CNG提供；本次编译／链接通过，未执行摘要比对或性能测试 |
| 53 | 0x00402BC0 | `sub_402BC0` | 有承接 | `src/platform/LegacyModuleDigest.cpp:86 — BCryptFinishHash（填充／长度编码／最终化）` | COR09：原手写MD5核心由Windows CNG提供；本次编译／链接通过，未执行摘要比对或性能测试 |
| 54 | 0x00402C50 | `sub_402C50` | 有承接 | `src/platform/LegacyModuleDigest.cpp:77 — BCryptHashData` | COR09：原手写MD5核心由Windows CNG提供；本次编译／链接通过，未执行摘要比对或性能测试 |
| 55 | 0x00402D30 | `sub_402D30` | 有承接 | `src/platform/LegacyModuleDigest.cpp:77 — BCryptHashData的系统MD5变换` | COR09：原手写MD5核心由Windows CNG提供；本次编译／链接通过，未执行摘要比对或性能测试 |
| 56 | 0x00403660 | `sub_403660` | 有正文 | `src/platform/LegacyModuleDigest.cpp:37 — ComputeFileDigest` | COR09：保留文件读取／小写文本／模块接口正文，MD5计算交Windows CNG；本次编译／链接通过，未运行摘要链 |
| 57 | 0x00403750 | `sub_403750` | 有正文 | `src/platform/LegacyModuleDigest.cpp:93 — EncodeLowerHex` | COR09：保留文件读取／小写文本／模块接口正文，MD5计算交Windows CNG；本次编译／链接通过，未运行摘要链 |
| 58 | 0x004037D0 | `sub_4037D0` | 已排除 | `doc/cg2d_network_plaintext_frame_spec.md — NKF01：旧协议状态重置已退休` | 用户授权删除旧帧独占短状态、重置入口及登录／退出调用；当前LF明文链和valueD9F2D0独立重置保持；非原端不可达或运行验收证明 |
| 59 | 0x004038A0 | `sub_4038A0` | 有正文 | `src/network/NetworkRuntime.cpp:426 — Sub_4038A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 60 | 0x00403A70 | `sub_403A70` | 有正文 | `src/network/NetworkRuntime.cpp:509 — Sub_403A70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 61 | 0x00403C50 | `sub_403C50` | 已排除 | `doc/cg2d_network_plaintext_frame_spec.md — NKF01：旧W16收帧入口已退休` | 用户授权删除无当前调用的旧帧函数及声明；活动LF接收仍由ReceivePlaintextProtocolLine承接，当前队列／派发保持；非原端不可达或完整网络验收证明 |
| 62 | 0x00403E50 | `sub_403E50` | 已排除 | `doc/cg2d_network_plaintext_frame_spec.md — NKF01：旧W16发帧入口已退休` | 用户授权删除无当前调用的旧帧函数及声明；活动发送仍由QueuePlaintextProtocolBytes承接，窗口报告采样／清零及回调接线保持；非原端不可达或完整网络验收证明 |
| 63 | 0x00404060 | `sub_404060` | 有正文 | `src/input/Input.cpp:504 — IsRejectedInputText_404060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 64 | 0x00404120 | `sub_404120` | 有承接 | `src/platform/RuntimeInitialization.cpp:39 — cg::platform::gProtectionChecksum1` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 65 | 0x00404130 | `unknown_libname_3` | 有承接 | `src/platform/RuntimeInitialization.cpp:39 — cg::platform::gProtectionChecksum1` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 66 | 0x00404150 | `unknown_libname_4` | 有承接 | `src/platform/RuntimeInitialization.cpp:39 — cg::platform::gProtectionChecksum1` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 67 | 0x00404160 | `sub_404160` | 有承接 | `src/platform/RuntimeInitialization.cpp:42 — cg::platform::gProtectionChecksum2` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 68 | 0x00404170 | `unknown_libname_5` | 有承接 | `src/platform/RuntimeInitialization.cpp:42 — cg::platform::gProtectionChecksum2` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 69 | 0x00404190 | `unknown_libname_6` | 有承接 | `src/platform/RuntimeInitialization.cpp:42 — cg::platform::gProtectionChecksum2` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 70 | 0x004041A0 | `sub_4041A0` | 有承接 | `src/platform/RuntimeInitialization.cpp:45 — cg::platform::gProtectionProcessState` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 71 | 0x004041B0 | `unknown_libname_7` | 有承接 | `src/platform/RuntimeInitialization.cpp:45 — cg::platform::gProtectionProcessState` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 72 | 0x004041D0 | `unknown_libname_8` | 有承接 | `src/platform/RuntimeInitialization.cpp:45 — cg::platform::gProtectionProcessState` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 73 | 0x004041E0 | `sub_4041E0` | 有正文 | `src/platform/MainLoopSupport.cpp:252 — InitializeProtection_4041E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 74 | 0x004042B0 | `sub_4042B0` | 有正文 | `src/platform/MainLoopSupport.cpp:487 — ReleaseProtection_4042B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 75 | 0x004042D0 | `sub_4042D0` | 有正文 | `src/platform/MainLoopSupport.cpp:141 — CheckProcessModules_4042D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 76 | 0x004043B0 | `sub_4043B0` | 有正文 | `src/platform/MainLoopSupport.cpp:285 — CheckProtection_4043B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 77 | 0x00404600 | `sub_404600` | 有正文 | `src/platform/MainLoopSupport.cpp:402 — ParseProtectionTables_404600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 78 | 0x00404880 | `sub_404880` | 有正文 | `src/platform/MainLoopSupport.cpp:184 — CheckDebugger_404880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 79 | 0x004048D0 | `sub_4048D0` | 有正文 | `src/platform/MainLoopSupport.cpp:515 — ResetProtection_4048D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 80 | 0x004048E0 | `sub_4048E0` | 有正文 | `src/platform/MainLoopSupport.cpp:545 — CheckWindows_4048E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 81 | 0x00404940 | `EnumFunc` | 有正文 | `src/platform/MainLoopSupport.cpp:205 — CheckWindowsCallback_404940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 82 | 0x00404A40 | `sub_404A40` | 有正文 | `src/Yobi/base.cpp:1614 — Sub_404A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 83 | 0x00404AE0 | `YobiAction::new` | 有正文 | `src/Yobi/base.cpp:1645 — NewYobiAction_404AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 84 | 0x00404C50 | `Yobi::UpdateAll` | 有正文 | `src/Yobi/base.cpp:6564 — YobiUpdateAll_404C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 85 | 0x00404D00 | `Yobi::MarkDeath` | 有正文 | `src/Yobi/base.cpp:6603 — MarkDeath` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 86 | 0x00404D10 | `sub_404D10` | 有正文 | `src/Yobi/base.cpp:1436 — DeleteYobiAction_404D10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 87 | 0x00404D30 | `YobiClearUp` | 有正文 | `src/Yobi/base.cpp:33667 — YobiClearUp_404D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 88 | 0x00404D60 | `sub_404D60` | 有正文 | `src/Yobi/base.cpp:33678 — Sub_404D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 89 | 0x00404D90 | `SelectIntValue` | 有正文 | `src/Yobi/battle_base.cpp:11857 — SelectIntValue_404D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 90 | 0x00404DB0 | `BattleEntry::EffectUpdate` | 有正文 | `src/Yobi/battle_actor.cpp:23109 — UpdateBattleActor_404DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 91 | 0x00405B40 | `GetPetPosition` | 有正文 | `src/world/SceneState.cpp:7852 — GetBattlePetPosition_405B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 92 | 0x00405B60 | `sub_405B60` | 有正文 | `src/Yobi/battle_status.cpp:26486 — ConsumeBattleFpCost_405B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 93 | 0x00405BD0 | `CompareFunction` | 有正文 | `src/Yobi/battle_base.cpp:22802 — CompareBattleDrawActions_405BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 94 | 0x00405C30 | `Yobi::sort` | 有正文 | `src/Yobi/battle_base.cpp:22830 — SortBattleActions_405C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 95 | 0x00405CB0 | `sub_405CB0` | 有正文 | `src/Yobi/battle_base.cpp:22862 — Sub_405CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 96 | 0x00405E60 | `sub_405E60` | 有正文 | `src/Yobi/battle_base.cpp:22898 — NormalizeBattleAngle_405E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 97 | 0x00405EA0 | `YobiAction::UpdateXYByRad` | 有正文 | `src/Yobi/battle_base.cpp:22910 — MoveBattleByAngle_405EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 98 | 0x00405ED0 | `sub_405ED0` | 有正文 | `src/Yobi/battle_base.cpp:22921 — GetBattleRelativeDirection_405ED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 99 | 0x00406060 | `sub_406060` | 有正文 | `src/Yobi/battle_base.cpp:22964 — GetBattleAngleDifference_406060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 100 | 0x004060D0 | `sub_4060D0` | 有正文 | `src/Yobi/battle_base.cpp:22983 — TurnBattleTowardPosition_4060D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 101 | 0x00406190 | `YobiAction::UpdateRad` | 有正文 | `src/Yobi/battle_base.cpp:23015 — TurnBattleToward_406190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 102 | 0x00406260 | `sub_406260` | 有正文 | `src/Yobi/battle_base.cpp:29242 — Sub_406260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 103 | 0x004062A0 | `sub_4062A0` | 有正文 | `src/Yobi/battle_base.cpp:23051 — GetBattleDistanceToPosition_4062A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 104 | 0x00406300 | `sub_406300` | 有正文 | `src/Yobi/battle_base.cpp:23066 — GetBattleDistance_406300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 105 | 0x00406360 | `sub_406360` | 有正文 | `src/Yobi/battle_base.cpp:23082 — CountBattleActiveEntries_406360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 106 | 0x00406390 | `sub_406390` | 有正文 | `src/Yobi/battle_base.cpp:31637 — ApplyBattleEffectFlags_406390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 107 | 0x004065A0 | `sub_4065A0` | 有正文 | `src/Yobi/battle_base.cpp:23094 — SeedBattleRandom_4065A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 108 | 0x004065E0 | `sub_4065E0` | 有正文 | `src/Yobi/battle_attack.cpp:24775 — ProbeBattleAttackEvent_4065E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 109 | 0x00406630 | `sub_406630` | 有正文 | `src/Yobi/battle_actor.cpp:29233 — UpdateBattleAction236_406630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 110 | 0x00406650 | `sub_406650` | 有正文 | `src/Yobi/battle_actor.cpp:23831 — UpdateBattleAction0_406650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 111 | 0x00406720 | `sub_406720` | 有正文 | `src/Yobi/battle_actor.cpp:23870 — UpdateBattleAction1_406720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 112 | 0x00406760 | `sub_406760` | 有正文 | `src/Yobi/battle_actor.cpp:23886 — UpdateBattleAction2_406760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 113 | 0x004067F0 | `sub_4067F0` | 有正文 | `src/Yobi/battle_actor.cpp:24081 — UpdateBattleAction11_4067F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 114 | 0x00406930 | `sub_406930` | 有正文 | `src/Yobi/battle_actor.cpp:24138 — UpdateBattleAction12_406930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 115 | 0x00406A80 | `sub_406A80` | 有正文 | `src/Yobi/battle_actor.cpp:24182 — UpdateBattleAction13_406A80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 116 | 0x00406BA0 | `sub_406BA0` | 有正文 | `src/Yobi/battle_attack.cpp:25915 — UpdateBattleAction25_406BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 117 | 0x00406C30 | `sub_406C30` | 有正文 | `src/Yobi/battle_actor.cpp:23910 — UpdateBattleAction3_406C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 118 | 0x00406CD0 | `sub_406CD0` | 有正文 | `src/Yobi/battle_actor.cpp:23969 — UpdateBattleAction6_406CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 119 | 0x00406D70 | `sub_406D70` | 有正文 | `src/Yobi/battle_actor.cpp:23930 — UpdateBattleAction4_406D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 120 | 0x00406E10 | `Battle::Boomerange::runEffect` | 有正文 | `src/Yobi/battle_attack.cpp:24796 — UpdateBattleAction15_406E10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 121 | 0x004070E0 | `Battle::SpiracleShot::runEffect` | 有正文 | `src/Yobi/battle_attack.cpp:24961 — UpdateBattleAction70_4070E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 122 | 0x004071C0 | `Battle::BlastWave::runEffect` | 有正文 | `src/Yobi/battle_attack.cpp:24985 — UpdateBattleAction249_4071C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 123 | 0x004072E0 | `sub_4072E0` | 有正文 | `src/Yobi/battle_attack.cpp:25022 — UpdateBattleAction16_4072E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 124 | 0x00408170 | `Battle::BowShot::runEffect` | 有正文 | `src/Yobi/battle_attack.cpp:24880 — UpdateBattleAction18_408170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 125 | 0x004082F0 | `Battle::RandomShot::runEffect` | 有正文 | `src/Yobi/battle_attack.cpp:24922 — UpdateBattleAction19_4082F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 126 | 0x00408410 | `sub_408410` | 有正文 | `src/Yobi/battle_attack.cpp:25447 — UpdateBattleAction20_408410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 127 | 0x00408750 | `sub_408750` | 有正文 | `src/Yobi/battle_attack.cpp:25549 — UpdateBattleAction21_408750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 128 | 0x004087C0 | `sub_4087C0` | 有正文 | `src/Yobi/battle_attack.cpp:25567 — UpdateBattleAction22_4087C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 129 | 0x004090F0 | `sub_4090F0` | 有正文 | `src/Yobi/battle_attack.cpp:25820 — UpdateBattleAction23_4090F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 130 | 0x00409380 | `sub_409380` | 有正文 | `src/Yobi/battle_attack.cpp:25902 — UpdateBattleAction24_409380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 131 | 0x004093B0 | `sub_4093B0` | 有正文 | `src/Yobi/battle_attack.cpp:25436 — UpdateBattleAction17_4093B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 132 | 0x00409410 | `sub_409410` | 有正文 | `src/Yobi/battle_actor.cpp:23991 — UpdateBattleAction7_409410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 133 | 0x004094A0 | `sub_4094A0` | 有正文 | `src/Yobi/battle_actor.cpp:24014 — UpdateBattleAction8_4094A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 134 | 0x00409510 | `sub_409510` | 有正文 | `src/Yobi/battle_actor.cpp:24032 — UpdateBattleAction9_409510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 135 | 0x00409610 | `sub_409610` | 有正文 | `src/Yobi/battle_actor.cpp:24065 — UpdateBattleAction10_409610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 136 | 0x00409680 | `sub_409680` | 有正文 | `src/Yobi/battle_attack.cpp:24225 — UpdateBattleAction14_409680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 137 | 0x0040A9E0 | `sub_40A9E0` | 有正文 | `src/Yobi/battle_status.cpp:25947 — UpdateBattleAction26_40A9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 138 | 0x0040AA70 | `sub_40AA70` | 有正文 | `src/Yobi/battle_status.cpp:25967 — UpdateBattleAction27_40AA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 139 | 0x0040ACF0 | `sub_40ACF0` | 有正文 | `src/Yobi/battle_status.cpp:26053 — UpdateBattleAction28_40ACF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 140 | 0x0040AE50 | `sub_40AE50` | 有正文 | `src/Yobi/battle_status.cpp:26105 — UpdateBattleAction29_40AE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 141 | 0x0040AF30 | `sub_40AF30` | 有正文 | `src/Yobi/battle_status.cpp:26136 — UpdateBattleAction30_40AF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 142 | 0x0040B020 | `sub_40B020` | 有正文 | `src/Yobi/battle_status.cpp:26167 — UpdateBattleAction31_40B020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 143 | 0x0040B130 | `sub_40B130` | 有正文 | `src/Yobi/battle_cast.cpp:26553 — UpdateBattleAction45_40B130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 144 | 0x0040B180 | `sub_40B180` | 有正文 | `src/Yobi/battle_cast.cpp:33160 — UpdateBattleAction251_40B180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 145 | 0x0040B200 | `sub_40B200` | 有正文 | `src/Yobi/battle_cast.cpp:26563 — UpdateBattleAction46_40B200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 146 | 0x0040B3C0 | `sub_40B3C0` | 有正文 | `src/Yobi/battle_cast.cpp:26703 — UpdateBattleAction50_40B3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 147 | 0x0040B3F0 | `sub_40B3F0` | 有正文 | `src/Yobi/battle_cast.cpp:26641 — UpdateBattleAction47_40B3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 148 | 0x0040B410 | `sub_40B410` | 有正文 | `src/Yobi/battle_attack.cpp:26206 — UpdateBattleAction32_40B410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 149 | 0x0040B450 | `sub_40B450` | 有正文 | `src/Yobi/battle_attack.cpp:26217 — UpdateBattleAction33_40B450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 150 | 0x0040B490 | `sub_40B490` | 有正文 | `src/Yobi/battle_cast.cpp:26650 — UpdateBattleAction48_40B490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 151 | 0x0040B5B0 | `sub_40B5B0` | 有正文 | `src/Yobi/battle_cast.cpp:26692 — UpdateBattleAction49_40B5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 152 | 0x0040B5E0 | `sub_40B5E0` | 有正文 | `src/Yobi/battle_attack.cpp:26228 — UpdateBattleAction34_40B5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 153 | 0x0040B930 | `sub_40B930` | 有正文 | `src/Yobi/battle_attack.cpp:26320 — UpdateBattleAction35_40B930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 154 | 0x0040BAD0 | `sub_40BAD0` | 有正文 | `src/Yobi/battle_attack.cpp:26378 — UpdateBattleAction36_40BAD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 155 | 0x0040BB20 | `sub_40BB20` | 有正文 | `src/Yobi/battle_attack.cpp:26394 — UpdateBattleAction37_40BB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 156 | 0x0040BB70 | `sub_40BB70` | 有正文 | `src/Yobi/battle_attack.cpp:26410 — UpdateBattleAction38_40BB70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 157 | 0x0040BBC0 | `sub_40BBC0` | 有正文 | `src/Yobi/battle_attack.cpp:26426 — UpdateBattleAction39_40BBC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 158 | 0x0040BD40 | `sub_40BD40` | 有正文 | `src/Yobi/battle_attack.cpp:26478 — UpdateBattleAction40_40BD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 159 | 0x0040BD60 | `sub_40BD60` | 有正文 | `src/Yobi/battle_cast.cpp:26503 — UpdateBattleAction41_40BD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 160 | 0x0040BDB0 | `sub_40BDB0` | 有正文 | `src/Yobi/battle_cast.cpp:26517 — UpdateBattleAction42_40BDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 161 | 0x0040BDF0 | `sub_40BDF0` | 有正文 | `src/Yobi/battle_cast.cpp:26542 — UpdateBattleAction44_40BDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 162 | 0x0040BE30 | `sub_40BE30` | 有正文 | `src/Yobi/battle_cast.cpp:26529 — UpdateBattleAction43_40BE30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 163 | 0x0040BE70 | `sub_40BE70` | 有正文 | `src/Yobi/battle_cast.cpp:26713 — UpdateBattleAction51_40BE70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 164 | 0x0040BE90 | `sub_40BE90` | 有正文 | `src/Yobi/battle_cast.cpp:26722 — UpdateBattleAction52_40BE90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 165 | 0x0040BF20 | `sub_40BF20` | 有正文 | `src/Yobi/battle_cast.cpp:26745 — UpdateBattleAction53_40BF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 166 | 0x0040BFE0 | `sub_40BFE0` | 有正文 | `src/Yobi/battle_cast.cpp:26776 — UpdateBattleAction54_40BFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 167 | 0x0040C0C0 | `sub_40C0C0` | 有正文 | `src/Yobi/battle_cast.cpp:26809 — UpdateBattleAction55_40C0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 168 | 0x0040C0E0 | `sub_40C0E0` | 有正文 | `src/Yobi/battle_cast.cpp:27023 — UpdateBattleAction68_40C0E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 169 | 0x0040C120 | `sub_40C120` | 有正文 | `src/Yobi/battle_cast.cpp:27035 — UpdateBattleAction69_40C120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 170 | 0x0040C160 | `sub_40C160` | 有正文 | `src/Yobi/battle_cast.cpp:26818 — UpdateBattleAction56_40C160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 171 | 0x0040C1A0 | `sub_40C1A0` | 有正文 | `src/Yobi/battle_cast.cpp:26831 — UpdateBattleAction57_40C1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 172 | 0x0040C1E0 | `sub_40C1E0` | 有正文 | `src/Yobi/battle_cast.cpp:26843 — UpdateBattleAction58_40C1E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 173 | 0x0040C220 | `sub_40C220` | 有正文 | `src/Yobi/battle_cast.cpp:26867 — UpdateBattleAction60_40C220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 174 | 0x0040C240 | `sub_40C240` | 有正文 | `src/Yobi/battle_cast.cpp:26855 — UpdateBattleAction59_40C240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 175 | 0x0040C280 | `sub_40C280` | 有正文 | `src/Yobi/battle_cast.cpp:26875 — UpdateBattleAction61_40C280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 176 | 0x0040C2B0 | `sub_40C2B0` | 有正文 | `src/Yobi/battle_cast.cpp:26885 — UpdateBattleAction62_40C2B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 177 | 0x0040C2E0 | `sub_40C2E0` | 有正文 | `src/Yobi/battle_cast.cpp:26895 — UpdateBattleAction63_40C2E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 178 | 0x0040C380 | `sub_40C380` | 有正文 | `src/Yobi/battle_cast.cpp:26935 — UpdateBattleAction65_40C380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 179 | 0x0040C480 | `sub_40C480` | 有正文 | `src/Yobi/battle_cast.cpp:26985 — UpdateBattleAction66_40C480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 180 | 0x0040C4D0 | `sub_40C4D0` | 有正文 | `src/Yobi/battle_cast.cpp:27047 — UpdateBattleAction71_40C4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 181 | 0x0040C520 | `sub_40C520` | 有正文 | `src/Yobi/battle_cast.cpp:27254 — UpdateBattleAction86_40C520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 182 | 0x0040C780 | `sub_40C780` | 有正文 | `src/Yobi/battle_cast.cpp:27327 — UpdateBattleAction87_40C780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 183 | 0x0040C8D0 | `sub_40C8D0` | 有正文 | `src/Yobi/battle_cast.cpp:27368 — UpdateBattleAction88_40C8D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 184 | 0x0040C8F0 | `sub_40C8F0` | 有正文 | `src/Yobi/battle_cast.cpp:27377 — UpdateBattleAction89_40C8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 185 | 0x0040C910 | `sub_40C910` | 有正文 | `src/Yobi/battle_cast.cpp:27385 — UpdateBattleAction90_40C910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 186 | 0x0040CAA0 | `sub_40CAA0` | 有正文 | `src/Yobi/battle_cast.cpp:27065 — UpdateBattleAction72_40CAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 187 | 0x0040CAC0 | `sub_40CAC0` | 有正文 | `src/Yobi/battle_base.cpp:33359 — AdvanceBattleLinkedCast_40CAC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 188 | 0x0040CAF0 | `sub_40CAF0` | 有正文 | `src/Yobi/battle_base.cpp:33369 — AdvanceBattleEffectAndCast_40CAF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 189 | 0x0040CB10 | `sub_40CB10` | 有正文 | `src/Yobi/battle_cast.cpp:27075 — UpdateBattleAction75_40CB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 190 | 0x0040CB30 | `sub_40CB30` | 有正文 | `src/Yobi/battle_cast.cpp:27084 — UpdateBattleAction76_40CB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 191 | 0x0040CB50 | `sub_40CB50` | 有正文 | `src/Yobi/battle_cast.cpp:27093 — UpdateBattleAction77_40CB50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 192 | 0x0040CB80 | `sub_40CB80` | 有正文 | `src/Yobi/battle_cast.cpp:27102 — UpdateBattleAction78_40CB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 193 | 0x0040CBB0 | `sub_40CBB0` | 有正文 | `src/Yobi/battle_cast.cpp:26927 — UpdateBattleAction64_40CBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 194 | 0x0040CBD0 | `sub_40CBD0` | 有正文 | `src/Yobi/battle_cast.cpp:27111 — UpdateBattleAction79_40CBD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 195 | 0x0040CC80 | `sub_40CC80` | 有正文 | `src/Yobi/battle_cast.cpp:27141 — UpdateBattleAction80_40CC80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 196 | 0x0040CCB0 | `sub_40CCB0` | 有正文 | `src/Yobi/battle_cast.cpp:27152 — UpdateBattleAction81_40CCB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 197 | 0x0040CCE0 | `sub_40CCE0` | 有正文 | `src/Yobi/battle_cast.cpp:27163 — UpdateBattleAction82_40CCE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 198 | 0x0040CD10 | `sub_40CD10` | 有正文 | `src/Yobi/battle_cast.cpp:27174 — UpdateBattleAction83_40CD10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 199 | 0x0040CD70 | `sub_40CD70` | 有正文 | `src/Yobi/battle_cast.cpp:27191 — UpdateBattleAction84_40CD70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 200 | 0x0040CDE0 | `sub_40CDE0` | 有正文 | `src/Yobi/battle_cast.cpp:27212 — UpdateBattleAction85_40CDE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 201 | 0x0040CF30 | `sub_40CF30` | 有正文 | `src/Yobi/battle_cast.cpp:27435 — UpdateBattleAction91_40CF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 202 | 0x0040CF50 | `sub_40CF50` | 有正文 | `src/Yobi/battle_cast.cpp:27444 — UpdateBattleAction92_40CF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 203 | 0x0040CF70 | `sub_40CF70` | 有正文 | `src/Yobi/battle_cast.cpp:27453 — UpdateBattleAction93_40CF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 204 | 0x0040CFA0 | `sub_40CFA0` | 有正文 | `src/Yobi/battle_actor.cpp:23950 — UpdateBattleAction5_40CFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 205 | 0x0040D010 | `sub_40D010` | 有正文 | `src/Yobi/battle_cast.cpp:27465 — UpdateBattleAction94_40D010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 206 | 0x0040D0B0 | `sub_40D0B0` | 有正文 | `src/Yobi/battle_cast.cpp:27486 — UpdateBattleAction95_40D0B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 207 | 0x0040D170 | `sub_40D170` | 有正文 | `src/Yobi/battle_cast.cpp:27516 — UpdateBattleAction96_40D170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 208 | 0x0040D1A0 | `sub_40D1A0` | 有正文 | `src/Yobi/battle_cast.cpp:27526 — UpdateBattleAction97_40D1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 209 | 0x0040D1D0 | `sub_40D1D0` | 有正文 | `src/Yobi/battle_cast.cpp:27536 — UpdateBattleAction98_40D1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 210 | 0x0040D390 | `sub_40D390` | 有正文 | `src/Yobi/battle_cast.cpp:27588 — UpdateBattleAction99_40D390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 211 | 0x0040D3C0 | `sub_40D3C0` | 有正文 | `src/Yobi/battle_cast.cpp:27599 — UpdateBattleAction100_40D3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 212 | 0x0040D580 | `sub_40D580` | 有正文 | `src/Yobi/battle_cast.cpp:27653 — UpdateBattleAction101_40D580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 213 | 0x0040D6C0 | `sub_40D6C0` | 有正文 | `src/Yobi/battle_cast.cpp:27693 — UpdateBattleAction102_40D6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 214 | 0x0040DBE0 | `sub_40DBE0` | 有正文 | `src/Yobi/battle_cast.cpp:30132 — UpdateBattleAction103_40DBE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 215 | 0x0040DC90 | `sub_40DC90` | 有正文 | `src/Yobi/battle_cast.cpp:27840 — UpdateBattleAction104_40DC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 216 | 0x0040DE40 | `sub_40DE40` | 有正文 | `src/Yobi/battle_cast.cpp:27887 — UpdateBattleAction105_40DE40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 217 | 0x0040DF50 | `sub_40DF50` | 有正文 | `src/Yobi/battle_cast.cpp:27930 — UpdateBattleAction106_40DF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 218 | 0x0040DF70 | `sub_40DF70` | 有正文 | `src/Yobi/battle_cast.cpp:27938 — UpdateBattleAction107_40DF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 219 | 0x0040DF80 | `sub_40DF80` | 有正文 | `src/Yobi/battle_cast.cpp:27945 — UpdateBattleAction108_40DF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 220 | 0x0040E4B0 | `sub_40E4B0` | 有正文 | `src/Yobi/battle_cast.cpp:30158 — UpdateBattleAction109_40E4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 221 | 0x0040E560 | `sub_40E560` | 有正文 | `src/Yobi/battle_cast.cpp:28094 — UpdateBattleAction110_40E560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 222 | 0x0040E970 | `sub_40E970` | 有正文 | `src/Yobi/battle_cast.cpp:28225 — UpdateBattleAction111_40E970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 223 | 0x0040EB00 | `sub_40EB00` | 有正文 | `src/Yobi/battle_cast.cpp:30185 — UpdateBattleAction112_40EB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 224 | 0x0040EFE0 | `sub_40EFE0` | 有正文 | `src/Yobi/battle_cast.cpp:28277 — UpdateBattleAction113_40EFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 225 | 0x0040F0A0 | `sub_40F0A0` | 有正文 | `src/Yobi/battle_cast.cpp:28311 — UpdateBattleAction114_40F0A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 226 | 0x0040F790 | `sub_40F790` | 有正文 | `src/Yobi/battle_cast.cpp:28994 — UpdateBattleAction154_40F790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 227 | 0x0040F800 | `sub_40F800` | 有正文 | `src/Yobi/battle_cast.cpp:29010 — UpdateBattleAction155_40F800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 228 | 0x0040FB30 | `sub_40FB30` | 有正文 | `src/Yobi/battle_cast.cpp:29119 — UpdateBattleAction156_40FB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 229 | 0x0040FB80 | `sub_40FB80` | 有正文 | `src/Yobi/battle_cast.cpp:28495 — UpdateBattleAction115_40FB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 230 | 0x0040FC10 | `sub_40FC10` | 有正文 | `src/Yobi/battle_cast.cpp:28511 — UpdateBattleFactoryAction_40FC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 231 | 0x0040FE50 | `sub_40FE50` | 有正文 | `src/Yobi/battle_cast.cpp:33168 — UpdateBattleAction253_40FE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 232 | 0x0040FEB0 | `sub_40FEB0` | 有正文 | `src/Yobi/battle_cast.cpp:33181 — UpdateBattleAction254_40FEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 233 | 0x0040FEF0 | `sub_40FEF0` | 有正文 | `src/Yobi/battle_cast.cpp:28981 — UpdateBattleAction148_40FEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 234 | 0x0040FF40 | `sub_40FF40` | 有正文 | `src/Yobi/battle_cast.cpp:28580 — UpdateBattleAction138_40FF40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 235 | 0x0040FF90 | `sub_40FF90` | 有正文 | `src/Yobi/battle_cast.cpp:28562 — UpdateBattleActions127To129_40FF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 236 | 0x00410000 | `sub_410000` | 有正文 | `src/Yobi/battle_cast.cpp:28594 — UpdateBattleAction139_410000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 237 | 0x004106B0 | `sub_4106B0` | 有正文 | `src/Yobi/battle_cast.cpp:26998 — UpdateBattleAction67_4106B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 238 | 0x00410730 | `sub_410730` | 有正文 | `src/Yobi/battle_cast.cpp:28806 — UpdateBattleAction140_410730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 239 | 0x004107C0 | `sub_4107C0` | 有正文 | `src/Yobi/battle_status.cpp:29204 — UpdateBattleAction159_4107C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 240 | 0x00410B00 | `sub_410B00` | 有正文 | `src/Yobi/battle_status.cpp:29211 — UpdateBattleAction160_410B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 241 | 0x00410E30 | `sub_410E30` | 有正文 | `src/Yobi/battle_status.cpp:29218 — UpdateBattleAction162_410E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 242 | 0x00410E90 | `sub_410E90` | 有正文 | `src/Yobi/battle_cast.cpp:28821 — UpdateBattleAction141_410E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 243 | 0x00410F70 | `sub_410F70` | 有正文 | `src/Yobi/battle_cast.cpp:28861 — UpdateBattleAction142_410F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 244 | 0x00410FC0 | `sub_410FC0` | 有正文 | `src/Yobi/battle_cast.cpp:28873 — UpdateBattleAction143_410FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 245 | 0x00411240 | `sub_411240` | 有正文 | `src/Yobi/battle_cast.cpp:28932 — UpdateBattleAction144_411240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 246 | 0x00411290 | `sub_411290` | 有正文 | `src/Yobi/battle_cast.cpp:28945 — UpdateBattleAction145_411290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 247 | 0x00411320 | `sub_411320` | 有正文 | `src/Yobi/battle_cast.cpp:28964 — UpdateBattleAction147_411320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 248 | 0x004113A0 | `sub_4113A0` | 有正文 | `src/Yobi/battle_status.cpp:29411 — UpdateBattleAction163_4113A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 249 | 0x00411A50 | `sub_411A50` | 有正文 | `src/Yobi/battle_status.cpp:29619 — UpdateBattleAction164_411A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 250 | 0x00411CB0 | `sub_411CB0` | 有正文 | `src/Yobi/battle_status.cpp:29779 — UpdateBattleAction166_411CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 251 | 0x00411D30 | `sub_411D30` | 有正文 | `src/Yobi/battle_status.cpp:29692 — UpdateBattleAction165_411D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 252 | 0x00411FA0 | `sub_411FA0` | 有正文 | `src/Yobi/battle_status.cpp:29796 — UpdateBattleAction167_411FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 253 | 0x00411FC0 | `sub_411FC0` | 有正文 | `src/Yobi/battle_status.cpp:29803 — UpdateBattleAction168_411FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 254 | 0x00411FE0 | `sub_411FE0` | 有正文 | `src/Yobi/battle_status.cpp:29810 — UpdateBattleAction169_411FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 255 | 0x00412000 | `sub_412000` | 有正文 | `src/Yobi/battle_status.cpp:29855 — UpdateBattleAction170_412000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 256 | 0x00412020 | `sub_412020` | 有正文 | `src/Yobi/battle_status.cpp:29862 — UpdateBattleAction171_412020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 257 | 0x004120F0 | `sub_4120F0` | 有正文 | `src/Yobi/battle_status.cpp:29886 — UpdateBattleActions172_173_4120F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 258 | 0x004121D0 | `sub_4121D0` | 有正文 | `src/Yobi/battle_status.cpp:29911 — UpdateBattleAction174_4121D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 259 | 0x00412210 | `sub_412210` | 有正文 | `src/Yobi/battle_status.cpp:29923 — UpdateBattleAction175_412210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 260 | 0x00412260 | `sub_412260` | 有正文 | `src/Yobi/battle_status.cpp:29935 — UpdateBattleAction176_412260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 261 | 0x00412290 | `sub_412290` | 有正文 | `src/Yobi/battle_status.cpp:29947 — UpdateBattleAction177_412290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 262 | 0x004122C0 | `sub_4122C0` | 有正文 | `src/Yobi/battle_status.cpp:29956 — UpdateBattleAction178_4122C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 263 | 0x004122E0 | `sub_4122E0` | 有正文 | `src/Yobi/battle_status.cpp:29963 — UpdateBattleAction179_4122E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 264 | 0x00412310 | `sub_412310` | 有正文 | `src/Yobi/battle_status.cpp:29972 — UpdateBattleActions180_203_412310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 265 | 0x004124E0 | `sub_4124E0` | 有正文 | `src/Yobi/battle_status.cpp:30014 — UpdateBattleAction204_4124E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 266 | 0x004127B0 | `sub_4127B0` | 有正文 | `src/Yobi/battle_status.cpp:30099 — UpdateBattleAction205_4127B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 267 | 0x004127F0 | `sub_4127F0` | 有正文 | `src/Yobi/battle_status.cpp:30110 — UpdateBattleAction206_4127F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 268 | 0x00412830 | `sub_412830` | 有正文 | `src/Yobi/battle_status.cpp:30120 — UpdateBattleAction207_412830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 269 | 0x00412870 | `sub_412870` | 有正文 | `src/Yobi/battle_cast.cpp:31689 — SetBattleAction208Target_412870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 270 | 0x00412900 | `sub_412900` | 有正文 | `src/Yobi/battle_cast.cpp:31708 — UpdateBattleAction209_412900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 271 | 0x00412F70 | `sub_412F70` | 有正文 | `src/Yobi/battle_cast.cpp:31925 — UpdateBattleAction210_412F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 272 | 0x00413070 | `sub_413070` | 有正文 | `src/Yobi/battle_cast.cpp:31948 — ApplyBattleSceneEffects_413070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 273 | 0x00413110 | `sub_413110` | 有正文 | `src/Yobi/battle_cast.cpp:31990 — UpdateBattleAction212_413110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 274 | 0x004131B0 | `sub_4131B0` | 有正文 | `src/Yobi/battle_cast.cpp:32016 — UpdateBattleAction213_4131B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 275 | 0x004132B0 | `sub_4132B0` | 有正文 | `src/Yobi/battle_cast.cpp:32062 — UpdateBattleAction214_4132B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 276 | 0x004133D0 | `sub_4133D0` | 有正文 | `src/Yobi/battle_cast.cpp:32103 — UpdateBattleAction215_4133D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 277 | 0x00413400 | `sub_413400` | 有正文 | `src/Yobi/battle_cast.cpp:32113 — UpdateBattleAction216_413400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 278 | 0x00413550 | `sub_413550` | 有正文 | `src/Yobi/battle_cast.cpp:32169 — UpdateBattleAction218_413550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 279 | 0x00413750 | `sub_413750` | 有正文 | `src/Yobi/battle_cast.cpp:32301 — UpdateBattleAction219_413750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 280 | 0x00413980 | `sub_413980` | 有正文 | `src/Yobi/battle_cast.cpp:32326 — UpdateBattleAction220_413980` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 281 | 0x00413B40 | `sub_413B40` | 有正文 | `src/Yobi/battle_cast.cpp:32335 — UpdateBattleAction221_413B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 282 | 0x00413E90 | `sub_413E90` | 有正文 | `src/Yobi/battle_cast.cpp:32434 — UpdateBattleAction222_413E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 283 | 0x00413FE0 | `sub_413FE0` | 有正文 | `src/Yobi/battle_cast.cpp:32472 — UpdateBattleAction223_413FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 284 | 0x00414050 | `sub_414050` | 有正文 | `src/Yobi/battle_ride.cpp:30455 — UpdateBattleAction224_414050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 285 | 0x004150F0 | `sub_4150F0` | 有正文 | `src/Yobi/battle_ride.cpp:31070 — UpdateBattleAction225_4150F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 286 | 0x004154C0 | `sub_4154C0` | 有正文 | `src/Yobi/battle_ride.cpp:30827 — UpdateBattleAction226_4154C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 287 | 0x00415940 | `sub_415940` | 有正文 | `src/Yobi/battle_ride.cpp:30920 — UpdateBattleAction227_415940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 288 | 0x00416000 | `sub_416000` | 有正文 | `src/Yobi/battle_ride.cpp:31188 — UpdateBattleAction228_416000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 289 | 0x00416020 | `sub_416020` | 有正文 | `src/Yobi/battle_ride.cpp:31197 — UpdateBattleAction229_416020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 290 | 0x004160A0 | `sub_4160A0` | 有正文 | `src/Yobi/battle_ride.cpp:31213 — UpdateBattleAction230_4160A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 291 | 0x00416150 | `sub_416150` | 有正文 | `src/Yobi/battle_ride.cpp:31235 — UpdateBattleAction231_416150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 292 | 0x00416220 | `sub_416220` | 有正文 | `src/Yobi/battle_attack.cpp:31266 — UpdateBattleAction232_416220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 293 | 0x00416860 | `sub_416860` | 有正文 | `src/Yobi/battle_attack.cpp:31434 — UpdateBattleAction233_416860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 294 | 0x00416AB0 | `sub_416AB0` | 有正文 | `src/Yobi/battle_attack.cpp:31513 — UpdateBattleAction234_416AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 295 | 0x00416DB0 | `sub_416DB0` | 有正文 | `src/Yobi/battle_base.cpp:31681 — ApplyBattleCastEffects_416DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 296 | 0x00416DD0 | `sub_416DD0` | 有正文 | `src/Yobi/battle_attack.cpp:32494 — UpdateBattleAction237_416DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 297 | 0x00417410 | `sub_417410` | 有正文 | `src/Yobi/battle_attack.cpp:32672 — UpdateBattleActions238_239_417410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 298 | 0x00417AC0 | `sub_417AC0` | 有正文 | `src/Yobi/battle_attack.cpp:32870 — UpdateBattleAction240_417AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 299 | 0x00417B80 | `sub_417B80` | 有正文 | `src/Yobi/battle_attack.cpp:30318 — UpdateBattleAction241_417B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 300 | 0x00417CC0 | `sub_417CC0` | 有正文 | `src/Yobi/battle_attack.cpp:30359 — UpdateBattleAction242_417CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 301 | 0x00417D80 | `sub_417D80` | 有正文 | `src/Yobi/battle_attack.cpp:32898 — UpdateBattleAction243_417D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 302 | 0x00418550 | `sub_418550` | 有正文 | `src/Yobi/battle_attack.cpp:33097 — UpdateBattleAction244_418550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 303 | 0x00418580 | `sub_418580` | 有正文 | `src/Yobi/battle_attack.cpp:33107 — UpdateBattleAction245_418580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 304 | 0x004185C0 | `sub_4185C0` | 有正文 | `src/Yobi/battle_attack.cpp:33119 — UpdateBattleAction246_4185C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 305 | 0x004185F0 | `sub_4185F0` | 有正文 | `src/Yobi/battle_attack.cpp:33129 — UpdateBattleAction247_4185F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 306 | 0x00418620 | `sub_418620` | 有正文 | `src/Yobi/battle_attack.cpp:33139 — UpdateBattleAction248_418620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 307 | 0x004186B0 | `sub_4186B0` | 有正文 | `src/Yobi/battle_attack.cpp:33194 — UpdateBattleAction255_4186B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 308 | 0x00418B50 | `sub_418B50` | 有正文 | `src/Yobi/battle_attack.cpp:33352 — UpdateBattleAction256_418B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 309 | 0x00418B70 | `Battle::SpiracleShot::newEffect` | 有正文 | `src/Yobi/battle_effect.cpp:9415 — NewBattleSpiracleShot_418B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 310 | 0x00418D80 | `YobiBattleEntry::ProjectionUpdate` | 有正文 | `src/Yobi/battle_effect.cpp:9347 — UpdateBattleProjection_418D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 311 | 0x00418DF0 | `YobiBattleEntry::newProjection` | 有正文 | `src/Yobi/battle_effect.cpp:9368 — NewBattleProjection_418DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 312 | 0x00418F10 | `sub_418F10` | 有正文 | `src/Yobi/battle_effect.cpp:9256 — UpdateBattleArcProjection_418F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 313 | 0x00418FF0 | `sub_418FF0` | 有正文 | `src/Yobi/battle_effect.cpp:9296 — NewBattleArcProjection_418FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 314 | 0x00419180 | `Battle::BlastWave::newEffect` | 有正文 | `src/Yobi/battle_effect.cpp:9486 — NewBattleBlastWave_419180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 315 | 0x00419390 | `Battle::RangeHit::newEffect` | 有正文 | `src/Yobi/battle_effect.cpp:9557 — NewBattleRangeHit_419390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 316 | 0x004195F0 | `Battle::Boomerange::newEffect` | 有正文 | `src/Yobi/battle_effect.cpp:9724 — NewBattleBoomerang_4195F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 317 | 0x00419BE0 | `sub_419BE0` | 有正文 | `src/Yobi/battle_effect.cpp:10609 — UpdateBattleTransitionEffect_419BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 318 | 0x00419C10 | `sub_419C10` | 有正文 | `src/Yobi/battle_effect.cpp:10619 — NewBattleTransitionEffect_419C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 319 | 0x00419CB0 | `sub_419CB0` | 有正文 | `src/Yobi/battle_effect.cpp:10648 — UpdateBattleOffsetFollowEffect_419CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 320 | 0x00419CF0 | `sub_419CF0` | 有正文 | `src/Yobi/battle_effect.cpp:10665 — NewBattleOffsetFollowEffect_419CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 321 | 0x00419DA0 | `sub_419DA0` | 有正文 | `src/Yobi/battle_effect.cpp:9634 — NewBattleArcShot_419DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 322 | 0x00419FA0 | `sub_419FA0` | 有正文 | `src/Yobi/battle_effect.cpp:10047 — UpdateBattleOneShotEffect_419FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 323 | 0x00419FC0 | `sub_419FC0` | 有正文 | `src/Yobi/battle_effect.cpp:10055 — NewBattleRandomEffect_419FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 324 | 0x0041A0D0 | `sub_41A0D0` | 有正文 | `src/Yobi/battle_effect.cpp:10088 — NewBattleScreenEffect_41A0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 325 | 0x0041A160 | `sub_41A160` | 有正文 | `src/Yobi/battle_effect.cpp:6869 — UpdateBattleCompletingEffect_41A160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 326 | 0x0041A1C0 | `sub_41A1C0` | 有正文 | `src/Yobi/battle_effect.cpp:6888 — UpdateBattleParentEffect_41A1C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 327 | 0x0041A200 | `sub_41A200` | 有正文 | `src/Yobi/battle_effect.cpp:6902 — UpdateBattleRisingEffect_41A200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 328 | 0x0041A270 | `sub_41A270` | 有正文 | `src/Yobi/battle_effect.cpp:6926 — UpdateBattleScalingEffect_41A270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 329 | 0x0041A310 | `sub_41A310` | 有正文 | `src/Yobi/battle_effect.cpp:6956 — UpdateBattleRandomEmitter_41A310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 330 | 0x0041A530 | `sub_41A530` | 有正文 | `src/Yobi/battle_effect.cpp:7012 — UpdateBattleFallingEffect_41A530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 331 | 0x0041A590 | `sub_41A590` | 有正文 | `src/Yobi/battle_effect.cpp:7027 — UpdateBattleReverseEmitter_41A590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 332 | 0x0041A7D0 | `sub_41A7D0` | 有正文 | `src/Yobi/battle_effect.cpp:7080 — UpdateBattleIndexedEmitter_41A7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 333 | 0x0041A9E0 | `sub_41A9E0` | 有正文 | `src/Yobi/battle_impact.cpp:7156 — UpdateBattleImpactEffect_41A9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 334 | 0x0041AF00 | `sub_41AF00` | 有正文 | `src/Yobi/battle_impact.cpp:7322 — UpdateBattleImpactSoundEffect_41AF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 335 | 0x0041B440 | `sub_41B440` | 有正文 | `src/Yobi/battle_impact.cpp:7489 — UpdateBattleFrameEventEffect_41B440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 336 | 0x0041BB00 | `sub_41BB00` | 有正文 | `src/Yobi/battle_impact.cpp:7694 — UpdateBattleMovingImpactEffect_41BB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 337 | 0x0041C3B0 | `sub_41C3B0` | 有正文 | `src/Yobi/battle_impact.cpp:7944 — UpdateBattleHomingImpactEffect_41C3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 338 | 0x0041CD00 | `sub_41CD00` | 有正文 | `src/Yobi/battle_effect.cpp:8375 — NewBattleEffect_41CD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 339 | 0x0041E4D0 | `sub_41E4D0` | 有正文 | `src/Yobi/battle_effect.cpp:9911 — UpdateBattleScatterEffect_41E4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 340 | 0x0041E510 | `sub_41E510` | 有正文 | `src/Yobi/battle_effect.cpp:9925 — NewBattleScatterEffect_41E510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 341 | 0x0041E5C0 | `sub_41E5C0` | 有正文 | `src/Yobi/battle_effect.cpp:1853 — UpdateBattleAfterimage_41E5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 342 | 0x0041E5F0 | `sub_41E5F0` | 有正文 | `src/Yobi/battle_effect.cpp:1865 — NewBattleAfterimage_41E5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 343 | 0x0041E640 | `sub_41E640` | 有正文 | `src/Yobi/battle_effect.cpp:9950 — UpdateBattlePositionEffect_41E640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 344 | 0x0041E680 | `sub_41E680` | 有正文 | `src/Yobi/battle_effect.cpp:9962 — NewBattlePositionEffect_41E680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 345 | 0x0041E720 | `sub_41E720` | 有正文 | `src/Yobi/battle_effect.cpp:9989 — NewBattleCoordinateEffect_41E720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 346 | 0x0041E7C0 | `sub_41E7C0` | 有正文 | `src/Yobi/battle_effect.cpp:10015 — NewBattleTrailEffect_41E7C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 347 | 0x0041E890 | `sub_41E890` | 有正文 | `src/Yobi/battle_effect.cpp:8908 — DrawBattleTiledEffect_41E890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 348 | 0x0041E920 | `sub_41E920` | 有正文 | `src/Yobi/battle_effect.cpp:8935 — UpdateBattleTiledEffect_41E920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 349 | 0x0041E9D0 | `sub_41E9D0` | 有正文 | `src/Yobi/battle_effect.cpp:8968 — NewBattleTiledEffect_41E9D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 350 | 0x0041EAB0 | `sub_41EAB0` | 有正文 | `src/Yobi/battle_effect.cpp:1833 — CloneBattleAction_41EAB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 351 | 0x0041EB10 | `sub_41EB10` | 有正文 | `src/Yobi/base.cpp:1720 — YobiAction::operator=` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 352 | 0x0041F110 | `sub_41F110` | 有正文 | `src/Yobi/battle_display.cpp:10113 — UpdateBattleNumber_41F110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 353 | 0x0041F1A0 | `sub_41F1A0` | 有正文 | `src/Yobi/battle_display.cpp:10146 — NewBattleNumber_41F1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 354 | 0x0041F450 | `sub_41F450` | 有正文 | `src/Yobi/battle_display.cpp:10281 — UpdateBattleFloatingIcon_41F450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 355 | 0x0041F4C0 | `CreateSubYobi` | 有正文 | `src/Yobi/battle_display.cpp:10308 — NewBattleFloatingIcon_41F4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 356 | 0x0041F570 | `sub_41F570` | 有正文 | `src/Yobi/battle_impact.cpp:10447 — UpdateBattleHitEffect_41F570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 357 | 0x0041F5B0 | `Battle::Boomerange::newHitEffect` | 有正文 | `src/Yobi/battle_impact.cpp:10464 — NewBattleHitEffect_41F5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 358 | 0x0041F660 | `BattleYobi::Debuff::newEffect` | 有正文 | `src/Yobi/battle_effect.cpp:10497 — NewBattleDebuffEffect_41F660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 359 | 0x0041F800 | `BattleYobi::Debuff::UpdateEffect` | 有正文 | `src/Yobi/battle_effect.cpp:10550 — UpdateBattleDebuffEffect_41F800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 360 | 0x0041F860 | `sub_41F860` | 有正文 | `src/Yobi/battle_effect.cpp:10571 — NewBattleReverseEffect_41F860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 361 | 0x0041F950 | `sub_41F950` | 有正文 | `src/Yobi/battle_effect.cpp:10698 — UpdateBattleCenteredEffect_41F950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 362 | 0x0041FA00 | `sub_41FA00` | 有正文 | `src/Yobi/battle_effect.cpp:10722 — NewBattleCenteredEffect_41FA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 363 | 0x0041FB20 | `sub_41FB20` | 有正文 | `src/Yobi/battle_effect.cpp:10762 — UpdateBattleAttachedEffect_41FB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 364 | 0x0041FBE0 | `sub_41FBE0` | 有正文 | `src/Yobi/battle_effect.cpp:10801 — NewBattleAttachedEffect_41FBE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 365 | 0x0041FCD0 | `sub_41FCD0` | 有正文 | `src/Yobi/battle_effect.cpp:10833 — UpdateBattleStatusEffect_41FCD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 366 | 0x0041FE00 | `sub_41FE00` | 有正文 | `src/Yobi/battle_effect.cpp:10894 — NewBattleStatusEffect_41FE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 367 | 0x0041FF90 | `sub_41FF90` | 有正文 | `src/Yobi/battle_effect.cpp:10949 — UpdateBattleFieldEffect_41FF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 368 | 0x0041FFE0 | `sub_41FFE0` | 有正文 | `src/Yobi/battle_effect.cpp:10967 — NewBattleFieldEffect_41FFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 369 | 0x00420100 | `sub_420100` | 有正文 | `src/Yobi/battle_display.cpp:6711 — NewBattleSequence_420100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 370 | 0x00420300 | `sub_420300` | 有正文 | `src/Yobi/battle_display.cpp:6785 — UpdateBattleSequence_420300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 371 | 0x00420590 | `sub_420590` | 有正文 | `src/Yobi/battle_display.cpp:6768 — MoveBattleSequence_420590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 372 | 0x004205F0 | `sub_4205F0` | 有正文 | `src/Yobi/battle_effect.cpp:9691 — NewBattleCastCopy_4205F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 373 | 0x004206C0 | `sub_4206C0` | 有正文 | `src/Yobi/battle_display.cpp:10214 — NewBattleTimeNumber_4206C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 374 | 0x004208F0 | `sub_4208F0` | 有正文 | `src/Yobi/battle_effect.cpp:10340 — NewBattleRebirthEffects_4208F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 375 | 0x004209C0 | `sub_4209C0` | 有正文 | `src/Yobi/battle_effect.cpp:10367 — NewBattleAttributeEffects_4209C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 376 | 0x00420D50 | `sub_420D50` | 有正文 | `src/Yobi/battle_effect.cpp:7122 — NewBattleRisingEffect_420D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 377 | 0x00420F10 | `sub_420F10` | 有正文 | `src/Yobi/battle_attachment.cpp:29817 — SyncBattleAttachmentAlpha_420F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 378 | 0x00421480 | `sub_421480` | 有正文 | `src/Yobi/battle_effect.cpp:11014 — UpdateBattleBackgroundFade_421480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 379 | 0x00421570 | `sub_421570` | 有正文 | `src/Yobi/battle_effect.cpp:11072 — NewBattleBackgroundFade_421570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 380 | 0x00421610 | `sub_421610` | 有正文 | `src/Yobi/battle_effect.cpp:11196 — NewBattleScreenEffect_421610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 381 | 0x004216B0 | `sub_4216B0` | 有正文 | `src/Yobi/battle_effect.cpp:11224 — UpdateBattleScreenEmitter_4216B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 382 | 0x00421700 | `sub_421700` | 有正文 | `src/Yobi/battle_effect.cpp:11239 — NewBattleScreenEmitter_421700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 383 | 0x00421750 | `sub_421750` | 有正文 | `src/Yobi/battle_display.cpp:6617 — UpdateBattleText_421750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 384 | 0x004217F0 | `sub_4217F0` | 有正文 | `src/Yobi/battle_display.cpp:6646 — NewBattleText_4217F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 385 | 0x004219D0 | `sub_4219D0` | 有正文 | `src/Yobi/battle_impact.cpp:8204 — UpdateBattleCarryingEffect_4219D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 386 | 0x00421F90 | `sub_421F90` | 有正文 | `src/Yobi/battle_attachment.cpp:11257 — UpdateBattleAttachment_421F90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 387 | 0x00421FB0 | `sub_421FB0` | 有正文 | `src/Yobi/battle_attachment.cpp:11268 — SyncBattleRider_421FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 388 | 0x004221F0 | `sub_4221F0` | 有正文 | `src/Yobi/battle_attachment.cpp:11542 — NewBattleRider_4221F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 389 | 0x004222B0 | `YobiChara::SetRider` | 有正文 | `src/Yobi/battle_attachment.cpp:11641 — SetBattleRider_4222B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 390 | 0x00422370 | `sub_422370` | 有正文 | `src/Yobi/battle_effect.cpp:11105 — RestoreBattleStateEffects_422370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 391 | 0x004225D0 | `sub_4225D0` | 有正文 | `src/Yobi/battle_attachment.cpp:11353 — SyncBattleGear_4225D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 392 | 0x00422780 | `sub_422780` | 有正文 | `src/Yobi/battle_attachment.cpp:11434 — SyncBattleAuxGear_422780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 393 | 0x00422A30 | `sub_422A30` | 有正文 | `src/Yobi/battle_attachment.cpp:11573 — NewBattleGear_422A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 394 | 0x00422B00 | `sub_422B00` | 有正文 | `src/Yobi/battle_attachment.cpp:11605 — NewBattleAuxGear_422B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 395 | 0x00422BC0 | `sub_422BC0` | 有正文 | `src/Yobi/battle_attachment.cpp:11673 — SetBattleGear_422BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 396 | 0x00422C20 | `sub_422C20` | 有正文 | `src/Yobi/battle_attachment.cpp:11694 — SetBattleGear_422C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 397 | 0x00422C90 | `sub_422C90` | 有正文 | `src/Yobi/battle_attachment.cpp:11716 — SetBattleAuxGear_422C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 398 | 0x00422E50 | `sub_422E50` | 有正文 | `src/Yobi/battle_impact.cpp:8992 — InitBattleTargetEffect_422E50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 399 | 0x00423040 | `sub_423040` | 有正文 | `src/Yobi/battle_impact.cpp:9054 — InitBattleOffsetEffect_423040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 400 | 0x004231D0 | `sub_4231D0` | 有正文 | `src/Yobi/battle_impact.cpp:9104 — InitBattleCenterEffect_4231D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 401 | 0x00423350 | `sub_423350` | 有正文 | `src/Yobi/battle_impact.cpp:9154 — InitBattleLowerEffect_423350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 402 | 0x004234B0 | `sub_4234B0` | 有正文 | `src/Yobi/battle_impact.cpp:9201 — InitBattleScreenYEffect_4234B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 403 | 0x00423640 | `sub_423640` | 有正文 | `src/platform/MainLoopSupport.cpp:626 — Sub_423640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 404 | 0x004237E0 | `GetBattlePosition` | 有正文 | `src/Yobi/battle_base.cpp:22533 — GetBattlePosition_4237E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 405 | 0x00423800 | `sub_423800` | 有正文 | `src/Yobi/battle_actor.cpp:11802 — MatchBattleAlbum_423800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 406 | 0x004238F0 | `sub_4238F0` | 有正文 | `src/Yobi/battle_actor.cpp:11828 — UpdateBattleAlbumFlag_4238F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 407 | 0x00423960 | `Battle::ParsePacket_M::GetHeader2` | 有正文 | `src/Yobi/battle_message.cpp:14310 — GetBattleMObjectHeader_423960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 408 | 0x004239D0 | `Battle::ParsePacket_M_RID` | 有正文 | `src/Yobi/battle_message.cpp:21837 — ParseBattleMRide_4239D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 409 | 0x004242B0 | `sub_4242B0` | 有正文 | `src/Yobi/battle_skill.cpp:11867 — AppendBattleRedirect_4242B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 410 | 0x00424500 | `sub_424500` | 有正文 | `src/Yobi/battle_skill.cpp:11907 — AppendBattleSkillTail_424500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 411 | 0x00424620 | `sub_424620` | 有正文 | `src/Yobi/battle_skill_attack.cpp:11943 — TakeBattleMSkillAttackTargets_424620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 412 | 0x00426FC0 | `sub_426FC0` | 有正文 | `src/Yobi/battle_skill_attack.cpp:13060 — TakeBattleMSkillTargets_426FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 413 | 0x00427470 | `Yobi::BattleSceneUpdate` | 有正文 | `src/Yobi/battle_message.cpp:21643 — UpdateBattleEvent_427470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 414 | 0x00438230 | `sub_438230` | 有承接 | `src/Yobi/battle_skill_status.cpp:19731 — targetPayload->value13D8 != 1 \|\| targetPayload->value13DC != statusType` | 原 type=1 且 value 匹配；当前战斗 M 消费内联；完整业务／运行未验 |
| 415 | 0x00438250 | `sub_438250` | 有承接 | `src/Yobi/battle_skill_status.cpp:20066 — targetPayload->value13D8 != 3 \|\| targetPayload->value13DC != type` | 原 type=3 且 value 匹配；当前战斗 M 消费内联；完整业务／运行未验 |
| 416 | 0x00438270 | `sub_438270` | 有承接 | `src/platform/LegacyCryptInt.cpp:66 — LegacyCryptInt::operator=(const LegacyCryptInt&)` | 原三字段载荷复制由同类赋值承接，保留目标token、无读取／RNG，返回目标引用；完整业务／运行未验 |
| 417 | 0x00438290 | `YobiBattleChara::new` | 有正文 | `src/Yobi/battle_actor.cpp:21798 — NewBattleActor_438290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 418 | 0x004383B0 | `sub_4383B0` | 有正文 | `src/Yobi/battle_scene.cpp:21766 — NewBattleScene_4383B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 419 | 0x004383F0 | `sub_4383F0` | 有正文 | `src/Yobi/battle_scene.cpp:21782 — InitializeBattleScene_4383F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 420 | 0x00438450 | `getIntFromBattleBuffC1` | 有正文 | `src/Yobi/battle_message.cpp:22054 — TakeBattleCInteger_438450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 421 | 0x004384C0 | `getCharFromBattleBuffC1` | 有正文 | `src/Yobi/battle_message.cpp:22086 — TakeBattleCString_4384C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 422 | 0x00438550 | `Battle::ParsePacket_C` | 有正文 | `src/Yobi/battle_message.cpp:22128 — ParseBattleC_438550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 423 | 0x00438F70 | `Battle::ParsePacket_M::TakeInt` | 有正文 | `src/Yobi/battle_message.cpp:13210 — TakeBattleMInteger_438F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 424 | 0x00438FE0 | `Battle::ParsePacket_M::TakeStr2` | 有正文 | `src/Yobi/battle_message.cpp:13242 — TakeBattleMString_438FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 425 | 0x00439070 | `Battle::ParsePacket_M::GetHeader` | 有正文 | `src/Yobi/battle_message.cpp:13184 — GetBattleMHeader_439070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 426 | 0x004390E0 | `InitBattleEntry` | 有正文 | `src/Yobi/battle_base.cpp:29255 — InitBattleCastList_4390E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 427 | 0x004391E0 | `sub_4391E0` | 有正文 | `src/Yobi/battle_base.cpp:29287 — ResetBattleCastList_4391E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 428 | 0x004392A0 | `BattleScene::AppendCastList` | 有正文 | `src/Yobi/battle_base.cpp:29309 — AppendBattleCast_4392A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 429 | 0x004392F0 | `BattleScene::AppendCastList2` | 有正文 | `src/Yobi/battle_base.cpp:29325 — AppendBattleCastAt_4392F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 430 | 0x00439340 | `sub_439340` | 有正文 | `src/Yobi/battle_base.cpp:29342 — AdvanceBattleCastState_439340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 431 | 0x00439380 | `BattleEntry::MoveState` | 有正文 | `src/Yobi/battle_base.cpp:33376 — MoveBattleCastState_439380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 432 | 0x004393A0 | `BattleEntry::AppendEffectList` | 有正文 | `src/Yobi/battle_base.cpp:29360 — AppendBattleEffectList_4393A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 433 | 0x00439400 | `BattleEntry::AppendEffect` | 有正文 | `src/Yobi/battle_base.cpp:29376 — AppendBattleEffect_439400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 434 | 0x00439470 | `BattleEntry::MoveEffect` | 有正文 | `src/Yobi/battle_base.cpp:31609 — AdvanceBattleEffect_439470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 435 | 0x004394A0 | `sub_4394A0` | 有正文 | `src/Yobi/battle_base.cpp:33386 — CheckBattleTurnReady_4394A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 436 | 0x00439600 | `Battle::ParsePacket_M::TakeInt2` | 有正文 | `src/Yobi/battle_message.cpp:13288 — TakeBattleMPreviewInteger_439600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 437 | 0x00439670 | `Battle::ParsePacket_M::TakeStr3` | 有正文 | `src/Yobi/battle_message.cpp:13320 — TakeBattleMPreviewString_439670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 438 | 0x00439700 | `Battle::ParsePacket_M::GetHeader3` | 有正文 | `src/Yobi/battle_message.cpp:13365 — GetBattleMPreviewHeader_439700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 439 | 0x00439770 | `Battle::ParsePacket_M::handleRID` | 有正文 | `src/Yobi/battle_message.cpp:13503 — ScanBattleMRideParticipants_439770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 440 | 0x00439920 | `sub_439920` | 有正文 | `src/Yobi/battle_message.cpp:13578 — ScanBattleMAttackParticipants_439920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 441 | 0x00439B30 | `Battle::ParsePacket_M::Most` | 有正文 | `src/Yobi/battle_message.cpp:13650 — ScanBattleMParticipants_439B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 442 | 0x0043B290 | `sub_43B290` | 有正文 | `src/Yobi/battle_message.cpp:14109 — CheckBattleMParticipants_43B290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 443 | 0x0043B360 | `sub_43B360` | 有正文 | `src/Yobi/battle_scene.cpp:14151 — DispatchBattleEvents_43B360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 444 | 0x0043B610 | `sub_43B610` | 有正文 | `src/Yobi/battle_scene.cpp:21734 — NewBattleEvent_43B610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 445 | 0x0043B6C0 | `sub_43B6C0` | 有正文 | `src/Yobi/battle_scene.cpp:22445 — FinishBattleEvent_43B6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 446 | 0x0043B7F0 | `sub_43B7F0` | 有正文 | `src/Yobi/battle_base.cpp:22504 — AppendBattleQueuedTarget_43B7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 447 | 0x0043B830 | `sub_43B830` | 有正文 | `src/Yobi/battle_base.cpp:31597 — AdvanceBattleQueuedTarget_43B830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 448 | 0x0043B860 | `Battle::ParsePacket_M::TakeInt4` | 有正文 | `src/Yobi/battle_message.cpp:14281 — PeekBattleMInteger_43B860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 449 | 0x0043B8C0 | `Battle::ParsePacket_M::TakeStr` | 有正文 | `src/Yobi/battle_message.cpp:13458 — TakeBattleMStringAt_43B8C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 450 | 0x0043B940 | `Battle::ParsePacket_M::TakeInt3` | 有正文 | `src/Yobi/battle_message.cpp:13391 — TakeBattleMIntegerAt_43B940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 451 | 0x0043B9B0 | `sub_43B9B0` | 有正文 | `src/Yobi/battle_message.cpp:13422 — FindBattleMTarget_43B9B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 452 | 0x0043BAA0 | `sub_43BAA0` | 有正文 | `src/Yobi/battle_message.cpp:22546 — Sub_43BAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 453 | 0x0043BC10 | `sub_43BC10` | 有正文 | `src/Yobi/battle_base.cpp:29391 — AppendBattleAuxEffects_43BC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 454 | 0x0043BC90 | `sub_43BC90` | 有正文 | `src/Yobi/battle_base.cpp:31621 — TakeBattleAuxEffects_43BC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 455 | 0x0043BD00 | `BattleScene::HasAppendEffect` | 有正文 | `src/Yobi/battle_base.cpp:22587 — IsBattleAppendEffectSkill_43BD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 456 | 0x0043BE00 | `BattleScene::AddAppendEffect` | 有正文 | `src/Yobi/battle_base.cpp:22610 — AddBattleAppendEffect_43BE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 457 | 0x0043BEA0 | `sub_43BEA0` | 有正文 | `src/Yobi/battle_base.cpp:22648 — FindBattleAppendEffectTechnique_43BEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 458 | 0x0043BED0 | `sub_43BED0` | 有正文 | `src/Yobi/battle_base.cpp:22662 — ClearBattleAppendEffects_43BED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 459 | 0x0043BF00 | `sub_43BF00` | 有正文 | `src/Yobi/battle_base.cpp:22517 — GetBattleValueBand_43BF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 460 | 0x0043BF30 | `sub_43BF30` | 有正文 | `src/Yobi/battle_message.cpp:13553 — TakeBattleMAuxGraphics_43BF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 461 | 0x0043BFF0 | `sub_43BFF0` | 有正文 | `src/Yobi/battle_scene.cpp:22688 — Sub_43BFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 462 | 0x0043C040 | `sub_43C040` | 有正文 | `src/Yobi/battle_scene.cpp:22704 — Sub_43C040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 463 | 0x0043C060 | `BattleScene::FlushEntry` | 有正文 | `src/Yobi/battle_scene.cpp:33641 — FlushBattleEntries_43C060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 464 | 0x0043C0A0 | `sub_43C0A0` | 有正文 | `src/Yobi/battle_scene.cpp:22714 — DrawBattleEntryHover_43C0A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 465 | 0x0043C120 | `BattleScene::GetSelectedEntry` | 有正文 | `src/Yobi/battle_scene.cpp:22738 — GetSelectedBattleEntry_43C120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 466 | 0x0043C3E0 | `sub_43C3E0` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:1147 — WindowBattleCommand_DrawTargetHint` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 467 | 0x0043C490 | `sub_43C490` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:1171 — WindowBattleCommand_ResetTargets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 468 | 0x0043C4B0 | `Battle::GetBattlePetIndex` | 有正文 | `src/world/SceneState.cpp:3751 — Sub_43C4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 469 | 0x0043C4E0 | `BattleScene::InitCommand` | 有正文 | `src/world/SceneState.cpp:3763 — Sub_43C4E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 470 | 0x0043C560 | `BattleScene::CheckCommand` | 有正文 | `src/Yobi/battle_scene.cpp:33533 — CheckBattleCommand_43C560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 471 | 0x0043C8F0 | `sub_43C8F0` | 有正文 | `src/world/SceneState.cpp:9659 — Sub_43C8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 472 | 0x0043C910 | `sub_43C910` | 有正文 | `src/Yobi/battle_scene.cpp:33442 — Sub_43C910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 473 | 0x0043C950 | `sub_43C950` | 有正文 | `src/ui/windows/WindowBattleStateNotice.cpp:28 — WindowBattleStateNotice_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 474 | 0x0043C9A0 | `sub_43C9A0` | 有正文 | `src/Yobi/battle_scene.cpp:33457 — Sub_43C9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 475 | 0x0043CA50 | `sub_43CA50` | 有正文 | `src/Yobi/battle_scene.cpp:33491 — Sub_43CA50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 476 | 0x0043CA80 | `sub_43CA80` | 有正文 | `src/Yobi/battle_scene.cpp:33505 — Sub_43CA80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 477 | 0x0043CB10 | `sub_43CB10` | 有正文 | `src/world/MapSceneDrawing.cpp:336 — Sub_43CB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 478 | 0x0043CBB0 | `World::DrawBattleScene` | 有正文 | `src/world/SceneState.cpp:6378 — DrawBattleScene_43CBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 479 | 0x0043D760 | `sub_43D760` | 有承接 | `src/platform/RuntimeInitialization.cpp:54 — cg::world::detail::gWorldObjects` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 480 | 0x0043D770 | `sub_43D770` | 有承接 | `src/platform/RuntimeInitialization.cpp:54 — cg::world::detail::gWorldObjects` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 481 | 0x0043D7A0 | `sub_43D7A0` | 有承接 | `src/platform/RuntimeInitialization.cpp:54 — cg::world::detail::gWorldObjects` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 482 | 0x0043D7C0 | `sub_43D7C0` | 有承接 | `src/world/WorldObjectState.hpp:18 — WorldObjectRecord::WorldObjectRecord` | 两CryptInt成员按声明序自动构造；千项同一中央直接数组；C++完成前缀清理承接；运行未验 |
| 483 | 0x0043D810 | `sub_43D810` | 有承接 | `src/world/WorldObjectState.hpp:20 — WorldObjectRecord::~WorldObjectRecord` | 成员1C→0C及数组元素自动逆序析构，退休手工适配；原SEH／完整运行未验 |
| 484 | 0x0043D860 | `sub_43D860` | 有正文 | `src/world/WorldActorDrawing.cpp:35392 — Sub_43D860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 485 | 0x0043D970 | `sub_43D970` | 有正文 | `src/world/WorldActorDrawing.cpp:35557 — Sub_43D970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 486 | 0x0043D9A0 | `sub_43D9A0` | 有正文 | `src/world/WorldActorDrawing.cpp:35569 — Sub_43D9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 487 | 0x0043D9D0 | `sub_43D9D0` | 有正文 | `src/world/WorldActorDrawing.cpp:35581 — Sub_43D9D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 488 | 0x0043DA10 | `sub_43DA10` | 有正文 | `src/world/WorldActorDrawing.cpp:35596 — Sub_43DA10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 489 | 0x0043DA40 | `sub_43DA40` | 有正文 | `src/world/WorldActorDrawing.cpp:35608 — Sub_43DA40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 490 | 0x0043DA70 | `sub_43DA70` | 有正文 | `src/world/WorldActorDrawing.cpp:35620 — Sub_43DA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 491 | 0x0043DAA0 | `sub_43DAA0` | 有正文 | `src/world/WorldActorDrawing.cpp:35632 — Sub_43DAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 492 | 0x0043DAD0 | `sub_43DAD0` | 有正文 | `src/world/WorldActorDrawing.cpp:35644 — Sub_43DAD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 493 | 0x0043DB00 | `sub_43DB00` | 有正文 | `src/world/WorldActorDrawing.cpp:35656 — Sub_43DB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 494 | 0x0043DB30 | `sub_43DB30` | 有正文 | `src/world/WorldActorDrawing.cpp:35668 — Sub_43DB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 495 | 0x0043DB60 | `BattleEntry::showHpBar` | 有正文 | `src/Yobi/map_render.cpp:6281 — ShowHpBar_43DB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 496 | 0x0043DD00 | `sub_43DD00` | 有正文 | `src/Yobi/map_actor.cpp:4803 — Sub_43DD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 497 | 0x0043E490 | `sub_43E490` | 有正文 | `src/Yobi/map_actor.cpp:4975 — Sub_43E490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 498 | 0x0043EFC0 | `sub_43EFC0` | 有正文 | `src/Yobi/map_actor.cpp:5197 — Sub_43EFC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 499 | 0x0043F180 | `sub_43F180` | 有正文 | `src/Yobi/map_observation.cpp:5548 — Sub_43F180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 500 | 0x0043F1B0 | `sub_43F1B0` | 有正文 | `src/Yobi/map_observation.cpp:5567 — Sub_43F1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 501 | 0x0043F1F0 | `sub_43F1F0` | 有正文 | `src/Yobi/map_observation.cpp:5590 — Sub_43F1F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 502 | 0x0043F220 | `sub_43F220` | 有正文 | `src/Yobi/map_observation.cpp:5608 — Sub_43F220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 503 | 0x0043F230 | `sub_43F230` | 有正文 | `src/Yobi/map_observation.cpp:5614 — Sub_43F230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 504 | 0x0043F290 | `sub_43F290` | 有正文 | `src/Yobi/map_observation.cpp:2372 — Sub_43F290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 505 | 0x0043F330 | `YobiMapChara::new` | 有正文 | `src/Yobi/map_observation.cpp:2268 — Sub_43F330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 506 | 0x0043F520 | `YobiMapChara::Update` | 有正文 | `src/Yobi/map_observation.cpp:2718 — Sub_43F520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 507 | 0x0043F910 | `sub_43F910` | 有正文 | `src/Yobi/map_observation.cpp:2463 — Sub_43F910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 508 | 0x0043FD90 | `sub_43FD90` | 有正文 | `src/Yobi/map_observation.cpp:5636 — Sub_43FD90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 509 | 0x0043FE00 | `sub_43FE00` | 有正文 | `src/Yobi/map_observation.cpp:2404 — Sub_43FE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 510 | 0x0043FEB0 | `sub_43FEB0` | 有正文 | `src/Yobi/map_observation.cpp:2315 — Sub_43FEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 511 | 0x00440070 | `sub_440070` | 有正文 | `src/Yobi/map_observation.cpp:2827 — Sub_440070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 512 | 0x00440440 | `sub_440440` | 有正文 | `src/Yobi/map_observation.cpp:2547 — Sub_440440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 513 | 0x004408C0 | `sub_4408C0` | 有正文 | `src/Yobi/map_observation.cpp:2358 — Sub_4408C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 514 | 0x00440920 | `sub_440920` | 有正文 | `src/Yobi/map_observation.cpp:2917 — Sub_440920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 515 | 0x00440B50 | `sub_440B50` | 有正文 | `src/world/SceneState.cpp:7830 — Sub_440B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 516 | 0x00440B70 | `sub_440B70` | 有正文 | `src/Yobi/map_observation.cpp:6424 — Sub_440B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 517 | 0x00440C50 | `sub_440C50` | 有正文 | `src/Yobi/map_observation.cpp:6330 — Sub_440C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 518 | 0x00440F50 | `sub_440F50` | 有正文 | `src/Yobi/map_observation.cpp:6468 — Sub_440F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 519 | 0x00440FE0 | `sub_440FE0` | 有正文 | `src/Yobi/map_observation.cpp:6504 — Sub_440FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 520 | 0x00440FF0 | `sub_440FF0` | 有正文 | `src/world/SceneState.cpp:1873 — Sub_440FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 521 | 0x004410E0 | `sub_4410E0` | 有正文 | `src/world/SceneState.cpp:7837 — Sub_4410E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 522 | 0x004410F0 | `YobiGear::new` | 有正文 | `src/Yobi/map_render.cpp:6548 — NewYobiGear_4410F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 523 | 0x00441130 | `YobiGearPet::new` | 有正文 | `src/Yobi/map_render.cpp:6512 — NewYobiGearPet_441130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 524 | 0x004412D0 | `sub_4412D0` | 有正文 | `src/Yobi/map_render.cpp:5753 — Sub_4412D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 525 | 0x00441430 | `sub_441430` | 有正文 | `src/Yobi/map_render.cpp:5799 — Sub_441430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 526 | 0x00441570 | `YobiMapChara::DrawGear` | 有正文 | `src/Yobi/map_render.cpp:5840 — DrawGear_441570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 527 | 0x004417A0 | `sub_4417A0` | 有正文 | `src/Yobi/map_render.cpp:5703 — Sub_4417A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 528 | 0x004417E0 | `sub_4417E0` | 有正文 | `src/Yobi/map_actor.cpp:5011 — Sub_4417E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 529 | 0x00441DB0 | `sub_441DB0` | 有正文 | `src/Yobi/map_actor.cpp:2986 — Sub_441DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 530 | 0x00441F60 | `sub_441F60` | 有正文 | `src/Yobi/map_actor.cpp:4303 — Sub_441F60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 531 | 0x00441FC0 | `sub_441FC0` | 有正文 | `src/Yobi/map_actor.cpp:4326 — Sub_441FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 532 | 0x00442110 | `sub_442110` | 有正文 | `src/Yobi/map_actor.cpp:4391 — Sub_442110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 533 | 0x004422F0 | `sub_4422F0` | 有正文 | `src/Yobi/map_actor.cpp:4428 — Sub_4422F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 534 | 0x00442460 | `sub_442460` | 有正文 | `src/Yobi/map_actor.cpp:4461 — Sub_442460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 535 | 0x004424B0 | `sub_4424B0` | 有正文 | `src/Yobi/map_actor.cpp:5257 — Sub_4424B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 536 | 0x00442730 | `sub_442730` | 有正文 | `src/Yobi/map_actor.cpp:5343 — Sub_442730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 537 | 0x004428F0 | `sub_4428F0` | 有正文 | `src/Yobi/map_actor.cpp:5398 — Sub_4428F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 538 | 0x00442AB0 | `sub_442AB0` | 有正文 | `src/Yobi/map_actor.cpp:4279 — Sub_442AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 539 | 0x00442BD0 | `sub_442BD0` | 有正文 | `src/world/WorldActorDrawing.cpp:36010 — Sub_442BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 540 | 0x00442C00 | `sub_442C00` | 有正文 | `src/world/WorldActorDrawing.cpp:35471 — Sub_442C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 541 | 0x00442C40 | `sub_442C40` | 有正文 | `src/world/WorldActorDrawing.cpp:35985 — Sub_442C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 542 | 0x00442C70 | `sub_442C70` | 有正文 | `src/world/WorldActorDrawing.cpp:35997 — Sub_442C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 543 | 0x00442CA0 | `sub_442CA0` | 有正文 | `src/world/WorldActorDrawing.cpp:35485 — Sub_442CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 544 | 0x00442CD0 | `sub_442CD0` | 有正文 | `src/world/WorldActorDrawing.cpp:35497 — Sub_442CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 545 | 0x00442D00 | `sub_442D00` | 有正文 | `src/world/WorldActorDrawing.cpp:35509 — Sub_442D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 546 | 0x00442D30 | `sub_442D30` | 有正文 | `src/world/WorldActorDrawing.cpp:35521 — Sub_442D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 547 | 0x00442D60 | `sub_442D60` | 有正文 | `src/world/WorldActorDrawing.cpp:35533 — Sub_442D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 548 | 0x00442D90 | `sub_442D90` | 有正文 | `src/world/WorldActorDrawing.cpp:35545 — Sub_442D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 549 | 0x00442DC0 | `sub_442DC0` | 有正文 | `src/world/WorldActorDrawing.cpp:35426 — Sub_442DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 550 | 0x00442F70 | `sub_442F70` | 有正文 | `src/Yobi/map_render.cpp:5881 — Sub_442F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 551 | 0x00444000 | `sub_444000` | 有正文 | `src/Yobi/map_render.cpp:5712 — Sub_444000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 552 | 0x004441A0 | `sub_4441A0` | 有正文 | `src/Yobi/map_render.cpp:3206 — Sub_4441A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 553 | 0x00444260 | `sub_444260` | 有正文 | `src/Yobi/map_render.cpp:3241 — Sub_444260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 554 | 0x00444410 | `sub_444410` | 有正文 | `src/world/WorldActor.cpp:35680 — Sub_444410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 555 | 0x00444FA0 | `World::FindObjectIndex` | 有正文 | `src/world/WorldObjectState.cpp:79 — Sub_444FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 556 | 0x00445030 | `sub_445030` | 有正文 | `src/world/WorldActor.cpp:36384 — Sub_445030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 557 | 0x004450D0 | `sub_4450D0` | 有正文 | `src/world/WorldActor.cpp:36413 — Sub_4450D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 558 | 0x00445160 | `sub_445160` | 有正文 | `src/world/WorldActor.cpp:36440 — Sub_445160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 559 | 0x004451F0 | `sub_4451F0` | 有正文 | `src/world/WorldActor.cpp:36467 — Sub_4451F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 560 | 0x004452A0 | `sub_4452A0` | 有正文 | `src/world/WorldActor.cpp:36497 — Sub_4452A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 561 | 0x00445350 | `sub_445350` | 有正文 | `src/world/WorldActor.cpp:36119 — Sub_445350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 562 | 0x00445A60 | `sub_445A60` | 有正文 | `src/world/WorldObjectState.cpp:177 — Sub_445A60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 563 | 0x00445D20 | `sub_445D20` | 有正文 | `src/world/WorldActorDrawing.cpp:36265 — Sub_445D20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 564 | 0x00445F40 | `sub_445F40` | 有正文 | `src/world/WorldActorDrawing.cpp:36326 — Sub_445F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 565 | 0x00446130 | `World::TakeEmptyObjectIndex` | 有正文 | `src/world/WorldObjectState.cpp:110 — Sub_446130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 566 | 0x004461E0 | `sub_4461E0` | 有正文 | `src/world/WorldObjectState.cpp:142 — Sub_4461E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 567 | 0x00446210 | `sub_446210` | 有正文 | `src/world/WorldObjectState.cpp:157 — Sub_446210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 568 | 0x004462B0 | `World::FindObjectYobi` | 有正文 | `src/world/WorldObjectState.cpp:102 — FindObjectYobi_4462B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 569 | 0x004462E0 | `sub_4462E0` | 有正文 | `src/world/WorldObjectState.cpp:237 — Sub_4462E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 570 | 0x00446470 | `sub_446470` | 有正文 | `src/world/WorldObjectState.cpp:276 — Sub_446470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 571 | 0x004464D0 | `sub_4464D0` | 有正文 | `src/world/WorldObjectState.cpp:295 — Sub_4464D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 572 | 0x00446610 | `sub_446610` | 有正文 | `src/world/WorldObjectState.cpp:38 — Sub_446610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 573 | 0x00446640 | `sub_446640` | 有正文 | `src/world/WorldActor.cpp:36071 — Sub_446640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 574 | 0x004467D0 | `sub_4467D0` | 有正文 | `src/world/WorldActor.cpp:36030 — Sub_4467D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 575 | 0x004468F0 | `sub_4468F0` | 有正文 | `src/world/WorldObjectState.cpp:49 — Sub_4468F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 576 | 0x00446900 | `sub_446900` | 有正文 | `src/world/WorldObjectState.cpp:55 — Sub_446900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 577 | 0x00446970 | `sub_446970` | 有正文 | `src/Yobi/map_transition.cpp:906 — Sub_446970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 578 | 0x00446990 | `sub_446990` | 有正文 | `src/Yobi/map_transition.cpp:915 — Sub_446990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 579 | 0x004469C0 | `sub_4469C0` | 有正文 | `src/Yobi/map_transition.cpp:3280 — Sub_4469C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 580 | 0x00446B50 | `sub_446B50` | 有正文 | `src/Yobi/map_transition.cpp:3333 — Sub_446B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 581 | 0x00446C10 | `sub_446C10` | 有正文 | `src/Yobi/map_transition.cpp:3366 — Sub_446C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 582 | 0x00446D80 | `sub_446D80` | 有正文 | `src/Yobi/map_transition.cpp:3601 — Sub_446D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 583 | 0x00446FA0 | `sub_446FA0` | 有正文 | `src/Yobi/map_transition.cpp:3664 — Sub_446FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 584 | 0x00447070 | `sub_447070` | 有正文 | `src/Yobi/map_transition.cpp:3700 — Sub_447070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 585 | 0x00447230 | `sub_447230` | 有正文 | `src/Yobi/map_transition.cpp:3727 — Sub_447230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 586 | 0x004473B0 | `sub_4473B0` | 有正文 | `src/Yobi/map_transition.cpp:3098 — Sub_4473B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 587 | 0x004474E0 | `sub_4474E0` | 有正文 | `src/Yobi/map_transition.cpp:3151 — Sub_4474E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 588 | 0x00447610 | `sub_447610` | 有正文 | `src/Yobi/map_transition.cpp:3750 — Sub_447610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 589 | 0x00447820 | `sub_447820` | 有正文 | `src/Yobi/map_transition.cpp:3782 — Sub_447820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 590 | 0x00447A20 | `sub_447A20` | 有正文 | `src/Yobi/map_transition.cpp:3810 — Sub_447A20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 591 | 0x00447C40 | `sub_447C40` | 有正文 | `src/Yobi/map_transition.cpp:3816 — Sub_447C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 592 | 0x00447E60 | `sub_447E60` | 有正文 | `src/Yobi/map_transition.cpp:3822 — Sub_447E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 593 | 0x00448040 | `sub_448040` | 有正文 | `src/Yobi/map_transition.cpp:3032 — Sub_448040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 594 | 0x00448240 | `sub_448240` | 有正文 | `src/world/SceneState.cpp:470 — IsMapFloorAllowed_448240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 595 | 0x00448290 | `sub_448290` | 有正文 | `src/Yobi/environment.cpp:34309 — Sub_448290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 596 | 0x00448370 | `sub_448370` | 有正文 | `src/Yobi/environment.cpp:33689 — Sub_448370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 597 | 0x004483A0 | `sub_4483A0` | 有正文 | `src/Yobi/environment.cpp:34186 — Sub_4483A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 598 | 0x00448810 | `sub_448810` | 有正文 | `src/Yobi/environment.cpp:33897 — Sub_448810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 599 | 0x00448A50 | `sub_448A50` | 有正文 | `src/Yobi/environment.cpp:33791 — Sub_448A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 600 | 0x00448BF0 | `sub_448BF0` | 有正文 | `src/Yobi/environment.cpp:33844 — Sub_448BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 601 | 0x00448D90 | `sub_448D90` | 有正文 | `src/Yobi/environment.cpp:33958 — Sub_448D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 602 | 0x00449160 | `sub_449160` | 有正文 | `src/Yobi/environment.cpp:34410 — Sub_449160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 603 | 0x004493C0 | `sub_4493C0` | 有正文 | `src/world/SceneState.cpp:485 — Sub_4493C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 604 | 0x00449400 | `sub_449400` | 有正文 | `src/Yobi/environment.cpp:34355 — Sub_449400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 605 | 0x00449570 | `sub_449570` | 有正文 | `src/Yobi/environment.cpp:33705 — Sub_449570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 606 | 0x00449670 | `sub_449670` | 有正文 | `src/Yobi/environment.cpp:34342 — Sub_449670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 607 | 0x004496C0 | `sub_4496C0` | 有正文 | `src/Yobi/environment.cpp:33745 — Sub_4496C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 608 | 0x00449700 | `sub_449700` | 有正文 | `src/Yobi/environment.cpp:34125 — Sub_449700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 609 | 0x00449A00 | `sub_449A00` | 有正文 | `src/Yobi/environment.cpp:34039 — Sub_449A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 610 | 0x00449BC0 | `sub_449BC0` | 有正文 | `src/Yobi/environment.cpp:34106 — Sub_449BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 611 | 0x00449C10 | `sub_449C10` | 有正文 | `src/Yobi/environment.cpp:34092 — Sub_449C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 612 | 0x00449C90 | `sub_449C90` | 有正文 | `src/world/WorldActorDrawing.cpp:35956 — Sub_449C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 613 | 0x00449CC0 | `sub_449CC0` | 有正文 | `src/world/WorldActorDrawing.cpp:35972 — Sub_449CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 614 | 0x00449CF0 | `sub_449CF0` | 有正文 | `src/input/Input.cpp:943 — InitializeContext_449CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 615 | 0x00449DA0 | `sub_449DA0` | 有正文 | `src/world/ChatHistory.cpp:1073 — Sub_449DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 616 | 0x00449FA0 | `sub_449FA0` | 有正文 | `src/world/ChatHistory.cpp:1049 — Sub_449FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 617 | 0x0044A090 | `sub_44A090` | 有正文 | `src/world/ChatHistory.cpp:1024 — Sub_44A090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 618 | 0x0044A160 | `sub_44A160` | 有正文 | `src/input/Input.cpp:836 — LoadInitialText_44A160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 619 | 0x0044A1B0 | `sub_44A1B0` | 有正文 | `src/input/Input.cpp:1263 — BrowseChatHistory_44A1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 620 | 0x0044A410 | `sub_44A410` | 有正文 | `src/input/InputCaret.cpp:124 — Sub_44A410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 621 | 0x0044A540 | `sub_44A540` | 有正文 | `src/input/Input.cpp:429 — Sub_44A540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 622 | 0x0044A580 | `sub_44A580` | 有正文 | `src/input/Input.cpp:448 — Sub_44A580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 623 | 0x0044A5D0 | `sub_44A5D0` | 有正文 | `src/input/Input.cpp:423 — Sub_44A5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 624 | 0x0044A7D0 | `sub_44A7D0` | 有正文 | `src/input/Input.cpp:467 — Sub_44A7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 625 | 0x0044A990 | `sub_44A990` | 有正文 | `src/input/Input.cpp:473 — Sub_44A990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 626 | 0x0044AA90 | `sub_44AA90` | 有正文 | `src/input/Input.cpp:488 — Sub_44AA90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 627 | 0x0044AB60 | `sub_44AB60` | 有正文 | `src/input/Input.cpp:1326 — Sub_44AB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 628 | 0x0044ADB0 | `Input::AppendChar` | 有正文 | `src/input/Input.cpp:842 — AppendChar_44ADB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 629 | 0x0044B050 | `World::AppendMsgToCharWindow` | 有正文 | `src/world/ChatHistory.cpp:1178 — Sub_44B050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 630 | 0x0044B170 | `sub_44B170` | 有正文 | `src/ui/windows/WindowChat.cpp:744 — WindowChat_ClearHistory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 631 | 0x0044B190 | `sub_44B190` | 有正文 | `src/input/Input.cpp:344 — ClassifyLastAnsiUnit_44B190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 632 | 0x0044B1D0 | `sub_44B1D0` | 有正文 | `src/input/Input.cpp:409 — ClassifyTextBoundary_44B1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 633 | 0x0044B250 | `sub_44B250` | 有承接 | `src/input/Input.cpp:448 — (static_cast<unsigned char>(context->text[caret]) & 0x80u) != 0u` | 首字节高位决定 1／2 字节步长；实际输入消费者内联；完整业务／运行未验 |
| 634 | 0x0044B260 | `sub_44B260` | 有正文 | `src/render/Font.cpp:274 — MeasureDefaultText_44B260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 635 | 0x0044B2A0 | `TextMeasure` | 有正文 | `src/render/Font.cpp:278 — TextMeasure` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 636 | 0x0044B2E0 | `Font::measure` | 有正文 | `src/render/Font.cpp:227 — Font::measure` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 637 | 0x0044B330 | `Input::SetContext` | 有正文 | `src/input/Input.cpp:872 — SetContext_44B330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 638 | 0x0044B340 | `sub_44B340` | 有正文 | `src/input/Input.cpp:848 — ProcessPresetHotkeys_44B340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 639 | 0x0044B440 | `sub_44B440` | 有正文 | `src/input/Input.cpp:371 — ClassifyLegacyBoundary_44B440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 640 | 0x0044B4A0 | `sub_44B4A0` | 有正文 | `src/input/Input.cpp:383 — FindLegacyByte_44B4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 641 | 0x0044B570 | `sub_44B570` | 有正文 | `src/input/Input.cpp:364 — ClassifyChatSplit_44B570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 642 | 0x0044B5A0 | `sub_44B5A0` | 有正文 | `src/platform/MainLoopSupport.cpp:497 — ResetSystemLog_44B5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 643 | 0x0044B5C0 | `sub_44B5C0` | 有正文 | `src/LoginState.cpp:74 — Sub_44B5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 644 | 0x0044B600 | `sub_44B600` | 有承接 | `src/ui/windows/WindowStall.cpp:1243 — std::bit_cast<std::int8_t>(platform::gSupportedRenderMode)` | 原 W8 读取；当前 WindowSettings_RefreshDisplayPanel 直接读状态；完整业务／运行未验 |
| 645 | 0x0044B610 | `hasIDirect3D7` | 有正文 | `src/render/RendererResources.cpp:133 — HasLegacy3DDevice_44B610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 646 | 0x0044B630 | `Renderer::Init` | 有正文 | `src/render/RendererResources.cpp:197 — RendererInit_44B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 647 | 0x0044C3C0 | `Renderer::Destroy` | 有正文 | `src/render/SdlPresenter.cpp:541 — RendererDestroy_44C3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 648 | 0x0044C560 | `Renderer::Flip` | 有正文 | `src/render/SdlPresenter.cpp:386 — Flip_44C560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 649 | 0x0044C6B0 | `Renderer::TestCooperativeLevel` | 有正文 | `src/render/SdlPresenter.cpp:601 — TestCooperativeLevel_44C6B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 650 | 0x0044C6D0 | `Renderer::MakeSurface` | 有正文 | `src/render/GraphicDxCache.cpp:191 — CreateTextureSurface_44C6D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 651 | 0x0044C8B0 | `Renderer::CreateSurrface` | 有正文 | `src/render/GraphicDxCache.cpp:230 — CreatePlainTextureSurface_44C8B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 652 | 0x0044C9A0 | `Renderer::FillRect` | 有正文 | `src/render/LegacyTextureDrawing.cpp:273 — FillRect_44C9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 653 | 0x0044CC20 | `sub_44CC20` | 有正文 | `src/render/GraphicDxCache.cpp:164 — InitializeSurfaceTransformCache629300_44CC20` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 654 | 0x0044CC30 | `sub_44CC30` | 有承接 | `src/render/GraphicDxCache.cpp:164 — InitializeSurfaceTransformCache629300_44CC20` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 655 | 0x0044CC60 | `sub_44CC60` | 有承接 | `src/render/GraphicDxCache.cpp:164 — InitializeSurfaceTransformCache629300_44CC20` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 656 | 0x0044CD30 | `sub_44CD30` | 有正文 | `src/render/GraphicDxCache.cpp:173 — InitializeSurfaceTransformCache62A630_44CD30` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 657 | 0x0044CD40 | `sub_44CD40` | 有承接 | `src/render/GraphicDxCache.cpp:173 — InitializeSurfaceTransformCache62A630_44CD30` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 658 | 0x0044CD70 | `unknown_libname_9` | 有承接 | `src/render/GraphicDxCache.cpp:173 — InitializeSurfaceTransformCache62A630_44CD30` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 659 | 0x0044CD80 | `sub_44CD80` | 有正文 | `src/render/GraphicDxCache.cpp:767 — TransformTextureSurface_44CD80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 660 | 0x0044D3F0 | `sub_44D3F0` | 有承接 | `src/render/GraphicDxCache.cpp:138 — struct SurfaceTransformKeyLess` | 实际 operator() 比较器；关联原入口；完整业务／运行未验 |
| 661 | 0x0044D490 | `sub_44D490` | 有正文 | `src/render/GraphicDxCache.cpp:828 — ClearSurfaceTransformCache_44D490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 662 | 0x0044D520 | `Renderer::RenderTextureToSurface` | 有正文 | `src/render/LegacyTextureDrawing.cpp:307 — RenderTextureToSurface_44D520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 663 | 0x0044DA90 | `Renderer::RenderTexture` | 有正文 | `src/render/LegacyTextureDrawing.cpp:403 — RenderTexture_44DA90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 664 | 0x0044E0C0 | `sub_44E0C0` | 有正文 | `src/render/GraphicPaletteCache.cpp:55 — InitializeGraphicPaletteCache_44E0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 665 | 0x0044E110 | `sub_44E110` | 有正文 | `src/render/GraphicPaletteCache.cpp:71 — ReleaseGraphicPaletteCache_44E110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 666 | 0x0044E150 | `TakeIndexCacheFromPool` | 有正文 | `src/render/GraphicPaletteCache.cpp:78 — TakeIndexCacheFromPool_44E150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 667 | 0x0044E230 | `sub_44E230` | 有正文 | `src/render/GraphicPaletteCache.cpp:208 — AgeGraphicPaletteCache_44E230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 668 | 0x0044E290 | `Renderer::LoadDXPalette` | 有正文 | `src/render/GraphicPaletteCache.cpp:117 — LoadGraphicPalette32_44E290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 669 | 0x0044ED70 | `sub_44ED70` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 670 | 0x0044F000 | `sub_44F000` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 671 | 0x0044F330 | `sub_44F330` | 有正文 | `src/render/SurfaceState.cpp:394 — DrawIndexedAutoMap32_44F330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 672 | 0x0044F5C0 | `sub_44F5C0` | 有正文 | `src/render/SurfaceState.cpp:417 — DrawIndexedAutoMap_44F5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 673 | 0x0044F650 | `sub_44F650` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 674 | 0x0044F920 | `sub_44F920` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 675 | 0x0044FC20 | `sub_44FC20` | 有正文 | `src/render/SurfaceState.cpp:407 — DrawColorAutoMap32_44FC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 676 | 0x0044FEB0 | `sub_44FEB0` | 有正文 | `src/render/SurfaceState.cpp:425 — DrawColorAutoMap_44FEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 677 | 0x0044FF40 | `sub_44FF40` | 有承接 | `src/render/GraphicDxCache.cpp:714 — LoadTextureFromBuffer32_450400` | 32 位空 palette 分支由 CPU 载荷承接；低色深分支已排除；完整业务／运行未验 |
| 678 | 0x00450190 | `sub_450190` | 有承接 | `src/render/GraphicDxCache.cpp:714 — LoadTextureFromBuffer32_450400` | 32 位非空 palette 分支由 CPU 载荷承接；低色深分支已排除；完整业务／运行未验 |
| 679 | 0x00450400 | `Texture::LoadFromBuffer` | 有正文 | `src/render/GraphicDxCache.cpp:714 — LoadTextureFromBuffer32_450400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 680 | 0x00450470 | `Renderer::InitPalette` | 有正文 | `src/render/Palette.cpp:181 — InitPalette32` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 681 | 0x00450A80 | `sub_450A80` | 有正文 | `src/render/Palette.cpp:276 — ReadPalette` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 682 | 0x00450B70 | `sub_450B70` | 有正文 | `src/render/Palette.cpp:307 — SelectPalette` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 683 | 0x00450BB0 | `sub_450BB0` | 有正文 | `src/render/Palette.cpp:322 — UpdatePalette_450BB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 684 | 0x00451190 | `sub_451190` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 685 | 0x00451220 | `sub_451220` | 有正文 | `src/render/Palette.cpp:498 — RebuildPalette32` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 686 | 0x00451270 | `sub_451270` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 687 | 0x004512C0 | `Renderer::ClearSurface` | 有正文 | `src/render/SurfaceState.cpp:537 — ClearSurface_4512C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 688 | 0x00451310 | `Texture::FillBlack` | 有正文 | `src/render/GraphicDxCache.cpp:677 — FillTextureBlack_451310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 689 | 0x00451350 | `sub_451350` | 有正文 | `src/render/SurfaceState.cpp:548 — Sub_451350` | 32 位实际正文；低色深分支已具名排除；完整调色板／渲染运行未验 |
| 690 | 0x00451D80 | `sub_451D80` | 有正文 | `src/resource/GraphicRead.cpp:577 — GetGraphicAutoMapColor_451D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 691 | 0x00451E90 | `sub_451E90` | 有正文 | `src/render/Palette.cpp:18 — FindNearestPaletteColor_451E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 692 | 0x00451FA0 | `sub_451FA0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 693 | 0x004522F0 | `sub_4522F0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 694 | 0x00452840 | `sub_452840` | 有正文 | `src/render/SurfaceState.cpp:215 — DrawMapEffects32_452840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 695 | 0x00452CF0 | `sub_452CF0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 696 | 0x00453580 | `sub_453580` | 有正文 | `src/render/SurfaceState.cpp:388 — DrawMapEffects_453580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 697 | 0x004535B0 | `sub_4535B0` | 有正文 | `src/render/SurfaceState.cpp:689 — BlitSurfaceToBack_4535B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 698 | 0x004536D0 | `Renderer::IsLost` | 有正文 | `src/render/SdlPresenter.cpp:560 — IsLost_4536D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 699 | 0x00453760 | `takeScreenshot` | 有正文 | `src/render/SdlPresenter.cpp:444 — TakeScreenshot_453760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 700 | 0x00453DE0 | `sub_453DE0` | 有正文 | `src/render/Palette.cpp:168 — CountMaskBits_453DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 701 | 0x00453E00 | `sub_453E00` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 702 | 0x00453E10 | `unknown_libname_10` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 703 | 0x00453E30 | `unknown_libname_11` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 704 | 0x00453E40 | `sub_453E40` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 705 | 0x00453E50 | `unknown_libname_12` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 706 | 0x00453E70 | `unknown_libname_13` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 707 | 0x00453E80 | `sub_453E80` | 有正文 | `src/render/SdlPresenter.cpp:396 — SaveScreenshotBitmap_453E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 708 | 0x00453FB0 | `sub_453FB0` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 709 | 0x00454050 | `sub_454050` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 710 | 0x00454420 | `sub_454420` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 711 | 0x004544F0 | `sub_4544F0` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 712 | 0x00454730 | `sub_454730` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 713 | 0x00454780 | `sub_454780` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 714 | 0x004547C0 | `sub_4547C0` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 715 | 0x00454820 | `sub_454820` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 716 | 0x00454880 | `sub_454880` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 717 | 0x004548A0 | `sub_4548A0` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 718 | 0x00454B10 | `sub_454B10` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 719 | 0x00454B30 | `sub_454B30` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 720 | 0x00454B90 | `sub_454B90` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 721 | 0x00454BE0 | `sub_454BE0` | 有承接 | `src/render/GraphicDxCache.cpp:153 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 722 | 0x00454C00 | `sub_454C00` | 有正文 | `src/platform/RuntimeInitialization.cpp:147 — InitializeLegacyEmptyExit_454C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 723 | 0x00454C30 | `SoundSystem::Init` | 有正文 | `src/audio/LegacyAudio.cpp:1244 — InitializeMusic_454C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 724 | 0x00454EE0 | `sub_454EE0` | 有正文 | `src/audio/LegacyAudio.cpp:1231 — Sub_454EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 725 | 0x00454FE0 | `sub_454FE0` | 有承接 | `src/audio/LegacyAudio.cpp:1588 — InitializeFixedMusic；src/platform/SdlAudioBackend.cpp:637 — InitializeFixedMusic` | 原单patch GetInstrument／DownloadInstrument／临时Release；固定WAV实际资源解码／PCM流预载承接，原音色下载机制退休；保原身份，不恢复MIDI／DLS或宣称库内HRESULT等价，声音／寿命未验 |
| 726 | 0x00455030 | `sub_455030` | 有正文 | `src/audio/LegacyAudio.cpp:1142 — SetFixedMusicReverb_455030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 727 | 0x00455100 | `sub_455100` | 有承接 | `src/audio/LegacyAudio.cpp:1588 — InitializeFixedMusic；src/platform/SdlAudioBackend.cpp:637 — InitializeFixedMusic` | 原Collection装载／枚举／逐patch下载，唯一初始化父传Source=0且不消费返回码；当前固定WAV实际预载承接，客户端不读音色库；保原身份，WAV质量及资源／设备／寿命未验 |
| 728 | 0x00455260 | `sub_455260` | 有承接 | `src/audio/LegacyAudio.cpp:1567 — ShutdownFixedMusic；src/platform/SdlAudioBackend.cpp:745 — ShutdownFixedMusic` | 原Loader非零Release并清槽，唯一音乐关闭父；当前停止并释放固定SDL流／混响／PCM样本承接，旧COM loader机制退休；保原身份，不新增影子owner，正常寿命／运行未验 |
| 729 | 0x00455280 | `World::PlayFixedBGM` | 有正文 | `src/audio/LegacyAudio.cpp:1184 — PlayFixedMusic_455280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 730 | 0x004553B0 | `World::PlaySegment` | 有正文 | `src/audio/LegacyAudio.cpp:1174 — PlayFixedMusicSegment_4553B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 731 | 0x00455400 | `sub_455400` | 有正文 | `src/audio/LegacyAudio.cpp:1148 — ApplyFixedMusicVolume_455400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 732 | 0x00455450 | `World::StopBGM` | 有正文 | `src/audio/LegacyAudio.cpp:1158 — StopFixedMusic_455450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 733 | 0x00455470 | `sub_455470` | 有正文 | `src/audio/LegacyAudio.cpp:1166 — SetFixedMusicRepeats_455470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 734 | 0x00455490 | `sub_455490` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 735 | 0x004554A0 | `sub_4554A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 736 | 0x004554C0 | `sub_4554C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 737 | 0x004554D0 | `sub_4554D0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 738 | 0x00455540 | `sub_455540` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 739 | 0x00455550 | `sub_455550` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 740 | 0x004555C0 | `sub_4555C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 741 | 0x004555D0 | `sub_4555D0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 742 | 0x00455640 | `sub_455640` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 743 | 0x00455650 | `sub_455650` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 744 | 0x004556C0 | `sub_4556C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 745 | 0x004556D0 | `sub_4556D0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 746 | 0x00455740 | `sub_455740` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 747 | 0x00455750 | `sub_455750` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 748 | 0x004557C0 | `sub_4557C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 749 | 0x004557D0 | `sub_4557D0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 750 | 0x00455840 | `sub_455840` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 751 | 0x00455850 | `sub_455850` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 752 | 0x00455930 | `sub_455930` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 753 | 0x00455940 | `sub_455940` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 754 | 0x00455A90 | `canWatchBattle` | 有正文 | `src/world/SceneState.cpp:8257 — Sub_455A90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 755 | 0x00455AC0 | `___setargv` | 有正文 | `src/world/SceneState.cpp:9529 — Sub_455AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 756 | 0x00455AD0 | `___setargv_0` | 有正文 | `src/world/SceneState.cpp:9535 — Sub_455AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 757 | 0x00455AE0 | `___setargv_1` | 有正文 | `src/world/SceneState.cpp:9541 — Sub_455AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 758 | 0x00455AF0 | `___setargv_2` | 有正文 | `src/world/SceneState.cpp:9547 — Sub_455AF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 759 | 0x00455B00 | `___setargv_3` | 有正文 | `src/world/SceneState.cpp:9553 — Sub_455B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 760 | 0x00455B10 | `sub_455B10` | 有正文 | `src/world/SceneState.cpp:9559 — Sub_455B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 761 | 0x00455B20 | `sub_455B20` | 有正文 | `src/world/SceneState.cpp:9565 — Sub_455B20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 762 | 0x00455B40 | `___setargv_6` | 有正文 | `src/world/SceneState.cpp:9573 — Sub_455B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 763 | 0x00455B50 | `sub_455B50` | 有正文 | `src/world/SceneState.cpp:9579 — Sub_455B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 764 | 0x00455B70 | `sub_455B70` | 有正文 | `src/world/SceneState.cpp:9587 — Sub_455B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 765 | 0x00455B90 | `sub_455B90` | 有正文 | `src/world/SceneState.cpp:9595 — Sub_455B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 766 | 0x00455BB0 | `sub_455BB0` | 有正文 | `src/world/SceneState.cpp:9603 — Sub_455BB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 767 | 0x00455BD0 | `sub_455BD0` | 有正文 | `src/world/SceneState.cpp:9611 — Sub_455BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 768 | 0x00455C60 | `sub_455C60` | 有正文 | `src/world/SceneState.cpp:5287 — Sub_455C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 769 | 0x00455C70 | `sub_455C70` | 有正文 | `src/world/SceneState.cpp:574 — NextRotationValue_455C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 770 | 0x00455C90 | `sub_455C90` | 有正文 | `src/world/WorldUpdate.cpp:215 — Sub_455C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 771 | 0x00455FC0 | `sub_455FC0` | 有正文 | `src/world/SceneState.cpp:5293 — Sub_455FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 772 | 0x00456050 | `sub_456050` | 有正文 | `src/resource/ResourcePaths.cpp:183 — BuildResourcePaths` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 773 | 0x00456190 | `sub_456190` | 有正文 | `src/render/Font.cpp:95 — InitializeFont_456190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 774 | 0x00456530 | `sub_456530` | 有正文 | `src/render/Font.cpp:159 — ReleaseFont_456530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 775 | 0x00456560 | `Renderer::DrawText3` | 有正文 | `src/render/TextPendingQueue.cpp:81 — DrawText3_456560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 776 | 0x00456670 | `Renderer::DrawText2` | 有正文 | `src/render/TextPendingQueue.cpp:93 — DrawText2_456670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 777 | 0x00456780 | `Renderer::DrawText` | 有正文 | `src/render/TextPendingQueue.cpp:149 — DrawText_456780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 778 | 0x004568A0 | `Renderer::DrawInputText` | 有正文 | `src/render/InputText.cpp:56 — DrawInputText_4568A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 779 | 0x00456D30 | `Renderer::CacheReset` | 有正文 | `src/render/GraphicCacheState.cpp:17 — ResetGraphicCache_456D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 780 | 0x00456D50 | `sub_456D50` | 有正文 | `src/render/GraphicCacheState.cpp:10 — ClearTextQueueTail_456D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 781 | 0x00456D70 | `Renderer::RenderTextToSurface` | 有正文 | `src/render/Font.cpp:169 — RenderTextToSurface_456D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 782 | 0x00456FD0 | `sub_456FD0` | 有承接 | `src/platform/RuntimeInitialization.cpp:184 — cg::world::detail::gSceneTransitionValue` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 783 | 0x00456FE0 | `unknown_libname_15` | 有承接 | `src/platform/RuntimeInitialization.cpp:184 — cg::world::detail::gSceneTransitionValue` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 784 | 0x00457000 | `unknown_libname_16` | 有承接 | `src/platform/RuntimeInitialization.cpp:184 — cg::world::detail::gSceneTransitionValue` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 785 | 0x00457010 | `sub_457010` | 有正文 | `src/platform/MainLoopSupport.cpp:528 — RequestRender_457010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 786 | 0x00457020 | `MainLoop` | 有正文 | `src/platform/MainLoop.cpp:45 — MainLoop` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 787 | 0x004574E0 | `GraphicAlloc` | 有正文 | `src/resource/GraphicAlloc.cpp:21 — GraphicAlloc_4574E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 788 | 0x00457670 | `sub_457670` | 有正文 | `src/resource/GraphicAlloc.cpp:98 — Sub_457670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 789 | 0x004576B0 | `sub_4576B0` | 有正文 | `src/platform/ClientInitialization.cpp:179 — InitializeClient_4576B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 790 | 0x00457A40 | `sub_457A40` | 有正文 | `src/platform/ClientShutdown.cpp:42 — ShutdownClient_457A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 791 | 0x00457AB0 | `KeepAlive` | 有正文 | `src/platform/MainLoopSupport.cpp:572 — KeepAlive_457AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 792 | 0x00457B10 | `InitLoading` | 有正文 | `src/platform/LoadingScreen.cpp:28 — InitLoading_457B10` | 实际非占位正文已定位；2026-10-13 按用户要求初始阶段改为 21，自动走既有跳过清理；完整业务／接线／运行未验 |
| 793 | 0x00458390 | `sub_458390` | 有正文 | `src/platform/ClientInitialization.cpp:101 — FadeInLoading_458390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 794 | 0x004585F0 | `sub_4585F0` | 有正文 | `src/platform/ClientInitialization.cpp:147 — FadeOutLoading_4585F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 795 | 0x004587D0 | `sub_4587D0` | 有正文 | `src/platform/MainLoopWindowEnumeration.cpp:74 — EnumerateWindows_4587D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 796 | 0x00458840 | `sub_458840` | 有正文 | `src/platform/MainLoopWindowEnumeration.cpp:19 — EnumerateWindowsCallback_458840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 797 | 0x004589A0 | `sub_4589A0` | 有承接 | `src/network/NetworkRuntime.cpp:177 — platform::gWindowReportValue > 0` | 活动QueuePlaintextProtocolBytes内联承接；轮询／容量门禁通过后按首标记优先、报告值S32>0／登录状态1／截止W32按S32<0采样并清零；原命令字段LF保持，完整业务／运行未验 |
| 798 | 0x004589B0 | `sub_4589B0` | 有承接 | `src/network/NetworkRuntime.cpp:180 — platform::gWindowReportValue = 0` | 活动QueuePlaintextProtocolBytes内联承接；轮询／容量门禁通过后按首标记优先、报告值S32>0／登录状态1／截止W32按S32<0采样并清零；原命令字段LF保持，完整业务／运行未验 |
| 799 | 0x004589C0 | `sub_4589C0` | 有承接 | `src/network/NetworkRuntime.cpp:179 — std::bit_cast<std::int32_t>(platform::gWindowReportDeadline) < 0` | 活动QueuePlaintextProtocolBytes内联承接；轮询／容量门禁通过后按首标记优先、报告值S32>0／登录状态1／截止W32按S32<0采样并清零；原命令字段LF保持，完整业务／运行未验 |
| 800 | 0x004589D0 | `sub_4589D0` | 有正文 | `src/platform/MainLoopSupport.cpp:717 — ReadSystemSeconds_4589D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 801 | 0x00458A10 | `sub_458A10` | 有正文 | `src/platform/MainLoopSupport.cpp:730 — CheckSystemSeconds_458A10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 802 | 0x00458A40 | `sub_458A40` | 有正文 | `src/platform/MainLoop.cpp:22 — ShowModeChange_458A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 803 | 0x00458AE0 | `sub_458AE0` | 有正文 | `src/audio/LegacyAudio.cpp:1215 — Sub_458AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 804 | 0x00458DC0 | `sub_458DC0` | 有正文 | `src/platform/MainLoopSupport.cpp:537 — CheckLegacyDxCapability_458DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 805 | 0x00458DE0 | `World::UpdateNRTime` | 有正文 | `src/world/GameTime.cpp:16 — UpdateGameTime` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 806 | 0x00458EC0 | `Time::GetCate` | 有正文 | `src/world/GameTime.cpp:43 — GetCate_458EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 807 | 0x00458F00 | `sub_458F00` | 有正文 | `src/world/SceneState.cpp:6338 — Sub_458F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 808 | 0x00458F40 | `sub_458F40` | 有正文 | `src/world/SceneState.cpp:6311 — Sub_458F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 809 | 0x00458FD0 | `sub_458FD0` | 有正文 | `src/platform/imeImpl.cpp:45 — InitializeIme_458FD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 810 | 0x00459040 | `sub_459040` | 有正文 | `src/platform/imeImpl.cpp:73 — ShutdownIme_459040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 811 | 0x00459090 | `sub_459090` | 有正文 | `src/platform/imeImpl.cpp:89 — IsImeKeyboardLayout_459090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 812 | 0x004590A0 | `sub_4590A0` | 有正文 | `src/platform/imeImpl.cpp:98 — ResetImeOpenStatus_4590A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 813 | 0x004590D0 | `sub_4590D0` | 有承接 | `src/platform/imeImpl.cpp:119 — QueryWindowImeDescription_4734F0` | BCL02移除纯描述访问包装；原布局／IME与键表采样后直接返回同一gImeDescription视图，独立函数由原消费点承接；只静态，完整生产消费／运行未验 |
| 814 | 0x004590E0 | `World::RenderInput` | 有正文 | `src/world/RenderInput.cpp:173 — RenderInput_4590E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 815 | 0x00459580 | `sub_459580` | 有正文 | `src/platform/imeImpl.cpp:128 — Sub_459580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 816 | 0x004596D0 | `sub_4596D0` | 有正文 | `src/input/InputKeyDispatch.cpp:11 — ProcessKeyInput_4596D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 817 | 0x00459760 | `sub_459760` | 有正文 | `src/platform/WindowProcedure.cpp:43 — ClearKeyFlags_459760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 818 | 0x00459780 | `resetKeyState` | 有正文 | `src/platform/WindowProcedure.cpp:52 — ResetKeyState_459780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 819 | 0x004597A0 | `sub_4597A0` | 有正文 | `src/platform/WindowProcedure.cpp:67 — ReadKeyFlag4_4597A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 820 | 0x004597B0 | `sub_4597B0` | 有正文 | `src/platform/WindowProcedure.cpp:61 — ReadKeyPressFlag_4597B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 821 | 0x004597C0 | `sub_4597C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 822 | 0x004597E0 | `sub_4597E0` | 有正文 | `src/resource/GraphicInfo.cpp:45 — InitializeGraphicMapNumbers_4597E0` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 823 | 0x004597F0 | `sub_4597F0` | 有承接 | `src/resource/GraphicInfo.cpp:45 — InitializeGraphicMapNumbers_4597E0` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 824 | 0x00459820 | `sub_459820` | 有承接 | `src/resource/GraphicInfo.cpp:45 — InitializeGraphicMapNumbers_4597E0` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 825 | 0x00459850 | `sub_459850` | 有正文 | `src/resource/GraphicInfo.cpp:53 — InitializeGraphicFirstRecords_459850` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 826 | 0x00459860 | `sub_459860` | 有承接 | `src/resource/GraphicInfo.cpp:53 — InitializeGraphicFirstRecords_459850` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 827 | 0x00459890 | `sub_459890` | 有承接 | `src/resource/GraphicInfo.cpp:53 — InitializeGraphicFirstRecords_459850` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 828 | 0x00459960 | `GraphicInit` | 有正文 | `src/resource/GraphicInfo.cpp:61 — GraphicInit_459960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 829 | 0x0045A130 | `sub_45A130` | 有正文 | `src/resource/ResourcePaths.cpp:208 — LoadAutoMapColor_45A130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 830 | 0x0045A190 | `sub_45A190` | 有正文 | `src/resource/ResourcePaths.cpp:113 — Sub_45A190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 831 | 0x0045A290 | `sub_45A290` | 有正文 | `src/resource/ResourcePaths.cpp:90 — Sub_45A290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 832 | 0x0045A420 | `sub_45A420` | 有正文 | `src/resource/ResourcePaths.cpp:57 — Sub_45A420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 833 | 0x0045A6C0 | `sub_45A6C0` | 有正文 | `src/resource/GraphicRead.cpp:628 — CloseGraphicFiles` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 834 | 0x0045A710 | `Graphic::getXY` | 有正文 | `src/resource/GraphicInfo.cpp:210 — GetGraphicXY` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 835 | 0x0045A760 | `Graphic::getWH` | 有正文 | `src/resource/GraphicInfo.cpp:225 — GetGraphicWH` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 836 | 0x0045A7B0 | `Graphic::getSxSy` | 有正文 | `src/resource/GraphicInfo.cpp:240 — GetGraphicSxSy` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 837 | 0x0045A810 | `Graphic::getFlag1` | 有正文 | `src/resource/GraphicInfo.cpp:255 — GetGraphicFlag1` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 838 | 0x0045A860 | `Graphic::getFlag2` | 有正文 | `src/resource/GraphicInfo.cpp:268 — GetGraphicFlag2` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 839 | 0x0045A8B0 | `Graphic::getMapNoIndex` | 有正文 | `src/resource/GraphicInfo.cpp:281 — GetGraphicMapNoIndex` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 840 | 0x0045A8D0 | `Graphic::getMapNo` | 有正文 | `src/resource/GraphicInfo.cpp:323 — GetGraphicMapNo` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 841 | 0x0045A900 | `Graphic::readGraphicDataByIndex` | 有正文 | `src/resource/GraphicRead.cpp:536 — ReadGraphicDataByIndex` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 842 | 0x0045A9E0 | `sub_45A9E0` | 有承接 | `src/resource/GraphicInfo.cpp:37 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 843 | 0x0045AA80 | `sub_45AA80` | 有承接 | `src/resource/GraphicInfo.cpp:37 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 844 | 0x0045AB50 | `sub_45AB50` | 有承接 | `src/resource/GraphicInfo.cpp:37 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 845 | 0x0045AF20 | `sub_45AF20` | 有承接 | `src/resource/GraphicInfo.cpp:37 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 846 | 0x0045AF60 | `sub_45AF60` | 有承接 | `src/resource/GraphicInfo.cpp:37 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 847 | 0x0045B1D0 | `sub_45B1D0` | 有承接 | `src/resource/GraphicInfo.cpp:37 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 848 | 0x0045B1F0 | `sub_45B1F0` | 有承接 | `src/resource/GraphicInfo.cpp:37 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 849 | 0x0045B250 | `sub_45B250` | 有承接 | `src/resource/GraphicInfo.cpp:37 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 850 | 0x0045B2A0 | `sub_45B2A0` | 有承接 | `src/resource/GraphicInfo.cpp:37 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 851 | 0x0045B370 | `sub_45B370` | 有正文 | `src/resource/AnimationRuntime.cpp:72 — Sub_45B370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 852 | 0x0045B380 | `sub_45B380` | 有正文 | `src/resource/AnimationRuntime.cpp:78 — Sub_45B380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 853 | 0x0045B390 | `Animate::BinarySearch` | 有正文 | `src/resource/AnimationRuntime.cpp:203 — BinarySearch_45B390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 854 | 0x0045B400 | `Animate::getIndex` | 有正文 | `src/resource/AnimationRuntime.cpp:234 — GetIndex_45B400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 855 | 0x0045B420 | `sub_45B420` | 有正文 | `src/resource/AnimationRuntime.cpp:191 — Compare_45B420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 856 | 0x0045B440 | `sub_45B440` | 有正文 | `src/resource/AnimationCleanup.cpp:13 — Sub_45B440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 857 | 0x0045B530 | `sub_45B530` | 有正文 | `src/resource/AnimationList.cpp:44 — Sub_45B530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 858 | 0x0045B590 | `sub_45B590` | 有正文 | `src/resource/AnimationList.cpp:6 — Sub_45B590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 859 | 0x0045B5B0 | `sub_45B5B0` | 有正文 | `src/resource/AnimationList.cpp:17 — Sub_45B5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 860 | 0x0045B5D0 | `sub_45B5D0` | 有正文 | `src/resource/AnimationList.cpp:28 — Sub_45B5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 861 | 0x0045B630 | `sub_45B630` | 有正文 | `src/resource/AnimationCleanup.cpp:128 — Sub_45B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 862 | 0x0045B650 | `sub_45B650` | 有正文 | `src/resource/AnimationCleanup.cpp:70 — Sub_45B650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 863 | 0x0045B740 | `sub_45B740` | 有正文 | `src/resource/AnimationCleanup.cpp:138 — Sub_45B740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 864 | 0x0045B760 | `AnimeLoadAction` | 有正文 | `src/resource/AnimationRuntime.cpp:242 — AnimeLoadAction_45B760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 865 | 0x0045BB20 | `AnimeInit` | 有正文 | `src/resource/AnimationRuntime.cpp:86 — AnimeInit_45BB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 866 | 0x0045BEF0 | `AnimeUnload` | 有正文 | `src/resource/AnimationCleanup.cpp:148 — AnimeUnload_45BEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 867 | 0x0045BF20 | `sub_45BF20` | 有正文 | `src/resource/AnimationCleanup.cpp:161 — Sub_45BF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 868 | 0x0045BF60 | `CoordLoad` | 有正文 | `src/resource/AnimationRuntime.cpp:403 — CoordLoad_45BF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 869 | 0x0045C040 | `CoordLoadInit` | 有正文 | `src/resource/AnimationRuntime.cpp:383 — CoordLoadInit_45C040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 870 | 0x0045C150 | `CoordInit` | 有正文 | `src/resource/AnimationRuntime.cpp:345 — CoordInit_45C150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 871 | 0x0045C2D0 | `sub_45C2D0` | 有正文 | `src/LoginOuterScene.cpp:33 — ProcessOuterScene_45C2D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 872 | 0x0045C5A0 | `sub_45C5A0` | 有正文 | `src/LoginModeScene.cpp:29 — ProcessModeScene_45C5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 873 | 0x0045C7C0 | `sub_45C7C0` | 有正文 | `src/LoginInput.cpp:354 — InitializeInputContexts_45C7C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 874 | 0x0045C970 | `sub_45C970` | 有正文 | `src/resource/GraphicInfo.cpp:288 — ProbePaletteCapability_45C970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 875 | 0x0045CA20 | `sub_45CA20` | 有正文 | `src/LoginInputController.cpp:244 — ProcessLoginInput_45CA20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 876 | 0x0045CE00 | `sub_45CE00` | 有正文 | `src/LoginMessage.cpp:70 — ResetLoginMessage` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 877 | 0x0045CE10 | `sub_45CE10` | 有正文 | `src/LoginMessage.cpp:76 — UpdateLoginMessage` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 878 | 0x0045D720 | `sub_45D720` | 有正文 | `src/input/Input.cpp:314 — Sub_45D720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 879 | 0x0045D760 | `sub_45D760` | 有正文 | `src/input/Input.cpp:330 — Sub_45D760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 880 | 0x0045D7A0 | `sub_45D7A0` | 有正文 | `src/render/TextPendingQueue.cpp:27 — FindClickedTextIndex_45D7A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 881 | 0x0045D7D0 | `sub_45D7D0` | 有正文 | `src/CharacterAuthenticationFailure.cpp:22 — ResetAuthenticationFailure_45D7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 882 | 0x0045D7E0 | `sub_45D7E0` | 有正文 | `src/CharacterAuthenticationFailure.cpp:28 — UpdateAuthenticationFailure_45D7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 883 | 0x0045D980 | `sub_45D980` | 有正文 | `src/LoginState.cpp:273 — ResetValues_45D980` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 884 | 0x0045D990 | `sub_45D990` | 有正文 | `src/LoginState.cpp:281 — UpdateServerConnection` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 885 | 0x0045DBF0 | `sub_45DBF0` | 有正文 | `src/CharacterDeletionProgress.cpp:30 — ResetCharacterDeletionProgress_45DBF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 886 | 0x0045DC00 | `sub_45DC00` | 有正文 | `src/CharacterDeletionProgress.cpp:36 — UpdateCharacterDeletionProgress_45DC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 887 | 0x0045DD50 | `sub_45DD50` | 有正文 | `src/CharacterListProgress.cpp:33 — ResetCharacterListProgress_45DD50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 888 | 0x0045DD60 | `sub_45DD60` | 有正文 | `src/CharacterListProgress.cpp:39 — UpdateCharacterListProgress_45DD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 889 | 0x0045DEE0 | `sub_45DEE0` | 有正文 | `src/CharacterDeletionConfirmation.cpp:12 — ProcessConfirmationDialog_45DEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 890 | 0x0045E090 | `sub_45E090` | 有正文 | `src/CharacterDeletionConfirmation.cpp:115 — ProcessDefaultConfirmationDialog_45E090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 891 | 0x0045E0C0 | `sub_45E0C0` | 有正文 | `src/CharacterDeletionConfirmation.cpp:125 — ProcessCharacterDeletionWarning_45E0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 892 | 0x0045E3D0 | `sub_45E3D0` | 有正文 | `src/CharacterVersionUpgrade.cpp:46 — ProcessLegacyVersionScene_45E3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 893 | 0x0045E6E0 | `sub_45E6E0` | 有正文 | `src/Yobi/preview.cpp:35127 — Sub_45E6E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 894 | 0x0045E770 | `sub_45E770` | 有正文 | `src/input/Input.cpp:575 — Sub_45E770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 895 | 0x0045E7B0 | `sub_45E7B0` | 有正文 | `src/CharacterSelectScene.cpp:205 — Sub_45E7B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 896 | 0x0045E7D0 | `sub_45E7D0` | 有正文 | `src/CharacterSelectScene.cpp:214 — Sub_45E7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 897 | 0x0045E960 | `sub_45E960` | 有正文 | `src/LoginState.cpp:382 — ProcessState_45E960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 898 | 0x0045EA50 | `sub_45EA50` | 有正文 | `src/LoginState.cpp:422 — ResetLoginProgress` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 899 | 0x0045EA60 | `sub_45EA60` | 有正文 | `src/LoginState.cpp:428 — UpdateLoginProgress` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 900 | 0x0045EBB0 | `sub_45EBB0` | 有正文 | `src/world/SceneState.cpp:10983 — Sub_45EBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 901 | 0x0045ED80 | `sub_45ED80` | 有正文 | `src/world/SceneState.cpp:11062 — Sub_45ED80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 902 | 0x0045ED90 | `sub_45ED90` | 有正文 | `src/world/SceneState.cpp:11068 — Sub_45ED90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 903 | 0x0045EEE0 | `sub_45EEE0` | 有正文 | `src/world/SceneState.cpp:9677 — Sub_45EEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 904 | 0x0045EF80 | `sub_45EF80` | 有正文 | `src/LoginCapability3Scene.cpp:67 — Sub_45EF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 905 | 0x0045EFD0 | `sub_45EFD0` | 有正文 | `src/ui/LegacyUiRecord.cpp:30 — DrawUiControl_45EFD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 906 | 0x0045F090 | `sub_45F090` | 有正文 | `src/render/DrawGraphic.cpp:129 — Sub_45F090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 907 | 0x0045F100 | `sub_45F100` | 有正文 | `src/ui/LegacyUiRecord.cpp:14 — QueryUiControlInputState_45F100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 908 | 0x0045F190 | `sub_45F190` | 有正文 | `src/LoginCapability3Scene.cpp:198 — Sub_45F190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 909 | 0x0045F970 | `sub_45F970` | 有正文 | `src/LoginInput.cpp:451 — InitializeCapabilityInput_45F970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 910 | 0x0045FA70 | `sub_45FA70` | 有正文 | `src/LoginCapability3InputController.cpp:60 — ProcessCapability3Input_45FA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 911 | 0x0045FEA0 | `LoginScene::Process` | 有正文 | `src/LoginScene.cpp:349 — ProcessScene_45FEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 912 | 0x00460D00 | `sub_460D00` | 有正文 | `src/CharacterSelectPreview.cpp:479 — ProcessCharacterPreview_460D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 913 | 0x00460FF0 | `sub_460FF0` | 有正文 | `src/CharacterSelectScene.cpp:1438 — ProcessScene_460FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 914 | 0x00461B30 | `sub_461B30` | 有正文 | `src/CharacterSelectSlot.cpp:85 — ProcessCharacterSlot_461B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 915 | 0x00462090 | `sub_462090` | 有正文 | `src/CharacterSelectModal.cpp:12 — ProcessCharacterModal_462090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 916 | 0x00462330 | `sub_462330` | 有正文 | `src/CharacterVersionUpgrade.cpp:151 — ProcessScene_462330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 917 | 0x004626A0 | `sub_4626A0` | 有正文 | `src/CharacterSelectScene.cpp:269 — ProcessCreationScene_4626A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 918 | 0x00462870 | `sub_462870` | 有正文 | `src/CharacterSelectScene.cpp:389 — Sub_462870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 919 | 0x00462880 | `sub_462880` | 有正文 | `src/CharacterSelectScene.cpp:396 — Sub_462880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 920 | 0x00462930 | `sub_462930` | 有正文 | `src/CharacterSelectScene.cpp:429 — Sub_462930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 921 | 0x00463100 | `sub_463100` | 有正文 | `src/CharacterSelectScene.cpp:664 — Sub_463100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 922 | 0x004631D0 | `sub_4631D0` | 有正文 | `src/CharacterSelectScene.cpp:699 — Sub_4631D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 923 | 0x00464290 | `sub_464290` | 有正文 | `src/CharacterSelectScene.cpp:1196 — Sub_464290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 924 | 0x004642C0 | `sub_4642C0` | 有正文 | `src/CharacterSelectScene.cpp:1124 — Sub_4642C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 925 | 0x00464380 | `sub_464380` | 有正文 | `src/CharacterSelectScene.cpp:1159 — Sub_464380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 926 | 0x00464440 | `sub_464440` | 有正文 | `src/LoginCapability4Modal.cpp:76 — ResetCapability4Modal_464440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 927 | 0x00464450 | `sub_464450` | 有正文 | `src/LoginCapability4Modal.cpp:82 — UpdateCapability4Modal_464450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 928 | 0x00464840 | `sub_464840` | 有正文 | `src/LoginCapability4Scene.cpp:59 — Sub_464840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 929 | 0x00464E30 | `sub_464E30` | 有正文 | `src/LoginInputController.cpp:82 — ProcessCapability4Input_464E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 930 | 0x00465320 | `sub_465320` | 有正文 | `src/LoginInput.cpp:505 — InitializeCapability4Input_465320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 931 | 0x00465430 | `sub_465430` | 有正文 | `src/LoginInput.cpp:579 — ProcessCapability4SavedSlotSelector_465430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 932 | 0x00465740 | `sub_465740` | 有正文 | `src/LoginInput.cpp:566 — SaveAndSwitchCapability4InputSlot_465740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 933 | 0x00465790 | `_WinMain@16` | 有正文 | `src/platform/ClientEntry.cpp:263 — WinMain` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 934 | 0x004659F0 | `sub_4659F0` | 有正文 | `src/platform/StartupCommon.cpp:14 — ParseClientVersion` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 935 | 0x00465A20 | `sub_465A20` | 有正文 | `src/platform/StartupCommon.cpp:20 — CaptureDesktopColor` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 936 | 0x00465A30 | `sub_465A30` | 有正文 | `src/platform/WindowLifecycle.cpp:12 — SetWindowedDesktopColor` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 937 | 0x00465A80 | `ReadSetupIni` | 有正文 | `src/platform/SetupIni.cpp:16 — ReadSetupIni` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 938 | 0x00465F00 | `ParseCmdOpt` | 有正文 | `src/platform/CommandOptions.cpp:55 — ParseCommandOptions` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 939 | 0x004669F0 | `WndInit` | 有正文 | `src/platform/WindowLifecycle.cpp:20 — InitializeWindow` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 940 | 0x00466B80 | `Renderer::Rebuild` | 有正文 | `src/render/RendererRebuild.cpp:20 — RebuildRenderer_466B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 941 | 0x00466D50 | `sub_466D50` | 有正文 | `src/platform/WindowLifecycle.cpp:76 — PumpWindowMessages` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 942 | 0x00466DC0 | `WIN_PROC` | 有正文 | `src/platform/WindowProcedure.cpp:126 — WindowProcedure` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 943 | 0x00467B90 | `sub_467B90` | 有承接 | `src/platform/RuntimeInitialization.cpp:220 — cg::world::detail::gPlayerX／gPlayerY` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 944 | 0x00467BA0 | `unknown_libname_17` | 有承接 | `src/platform/RuntimeInitialization.cpp:220 — cg::world::detail::gPlayerX／gPlayerY` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 945 | 0x00467BD0 | `sub_467BD0` | 有承接 | `src/platform/RuntimeInitialization.cpp:220 — cg::world::detail::gPlayerX／gPlayerY` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 946 | 0x00467BF0 | `sub_467BF0` | 有承接 | `src/platform/RuntimeInitialization.cpp:224 — cg::world::detail::gPlayerSnapshotX` | LCI16同一中央位置直接定义LegacyCryptInt；MSVC生成默认构造／逆序析构，去optional、Borrow与启动包装；原RNG及业务读取保持，仅静态核对，编译／运行未验 |
| 947 | 0x00467C00 | `unknown_libname_19` | 有承接 | `src/platform/RuntimeInitialization.cpp:224 — cg::world::detail::gPlayerSnapshotX` | LCI16同一中央位置直接定义LegacyCryptInt；MSVC生成默认构造／逆序析构，去optional、Borrow与启动包装；原RNG及业务读取保持，仅静态核对，编译／运行未验 |
| 948 | 0x00467C20 | `unknown_libname_20` | 有承接 | `src/platform/RuntimeInitialization.cpp:224 — cg::world::detail::gPlayerSnapshotX` | LCI16同一中央位置直接定义LegacyCryptInt；MSVC生成默认构造／逆序析构，去optional、Borrow与启动包装；原RNG及业务读取保持，仅静态核对，编译／运行未验 |
| 949 | 0x00467C30 | `sub_467C30` | 有承接 | `src/platform/RuntimeInitialization.cpp:226 — cg::world::detail::gPlayerSnapshotY` | LCI16同一中央位置直接定义LegacyCryptInt；MSVC生成默认构造／逆序析构，去optional、Borrow与启动包装；原RNG及业务读取保持，仅静态核对，编译／运行未验 |
| 950 | 0x00467C40 | `unknown_libname_21` | 有承接 | `src/platform/RuntimeInitialization.cpp:226 — cg::world::detail::gPlayerSnapshotY` | LCI16同一中央位置直接定义LegacyCryptInt；MSVC生成默认构造／逆序析构，去optional、Borrow与启动包装；原RNG及业务读取保持，仅静态核对，编译／运行未验 |
| 951 | 0x00467C60 | `unknown_libname_22` | 有承接 | `src/platform/RuntimeInitialization.cpp:226 — cg::world::detail::gPlayerSnapshotY` | LCI16同一中央位置直接定义LegacyCryptInt；MSVC生成默认构造／逆序析构，去optional、Borrow与启动包装；原RNG及业务读取保持，仅静态核对，编译／运行未验 |
| 952 | 0x00467C70 | `sub_467C70` | 有承接 | `src/platform/RuntimeInitialization.cpp:229 — cg::world::detail::gValue9EE8D0` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 953 | 0x00467C80 | `unknown_libname_23` | 有承接 | `src/platform/RuntimeInitialization.cpp:229 — cg::world::detail::gValue9EE8D0` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 954 | 0x00467CA0 | `unknown_libname_24` | 有承接 | `src/platform/RuntimeInitialization.cpp:229 — cg::world::detail::gValue9EE8D0` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 955 | 0x00467CB0 | `sub_467CB0` | 有承接 | `src/platform/RuntimeInitialization.cpp:232 — cg::world::detail::gValue9EE8E0` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 956 | 0x00467CC0 | `unknown_libname_25` | 有承接 | `src/platform/RuntimeInitialization.cpp:232 — cg::world::detail::gValue9EE8E0` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 957 | 0x00467CE0 | `unknown_libname_26` | 有承接 | `src/platform/RuntimeInitialization.cpp:232 — cg::world::detail::gValue9EE8E0` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 958 | 0x00467CF0 | `sub_467CF0` | 有承接 | `src/platform/RuntimeInitialization.cpp:235 — cg::world::detail::gValueC27FF8／gValueC27FE8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 959 | 0x00467D00 | `unknown_libname_27` | 有承接 | `src/platform/RuntimeInitialization.cpp:235 — cg::world::detail::gValueC27FF8／gValueC27FE8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 960 | 0x00467D30 | `unknown_libname_28` | 有承接 | `src/platform/RuntimeInitialization.cpp:235 — cg::world::detail::gValueC27FF8／gValueC27FE8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 961 | 0x00467D50 | `sub_467D50` | 有承接 | `src/platform/RuntimeInitialization.cpp:239 — cg::world::detail::gMoveTargetX9A7F88／gMoveTargetY9A7F98` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 962 | 0x00467D60 | `sub_467D60` | 有承接 | `src/platform/RuntimeInitialization.cpp:239 — cg::world::detail::gMoveTargetX9A7F88／gMoveTargetY9A7F98` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 963 | 0x00467D90 | `sub_467D90` | 有承接 | `src/platform/RuntimeInitialization.cpp:239 — cg::world::detail::gMoveTargetX9A7F88／gMoveTargetY9A7F98` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 964 | 0x00467DB0 | `sub_467DB0` | 有承接 | `src/platform/RuntimeInitialization.cpp:243 — cg::world::detail::gValueC821E8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 965 | 0x00467DC0 | `unknown_libname_31` | 有承接 | `src/platform/RuntimeInitialization.cpp:243 — cg::world::detail::gValueC821E8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 966 | 0x00467DE0 | `unknown_libname_32` | 有承接 | `src/platform/RuntimeInitialization.cpp:243 — cg::world::detail::gValueC821E8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 967 | 0x00467DF0 | `sub_467DF0` | 有承接 | `src/platform/RuntimeInitialization.cpp:246 — cg::world::detail::gValue9E03C0` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 968 | 0x00467E00 | `unknown_libname_33` | 有承接 | `src/platform/RuntimeInitialization.cpp:246 — cg::world::detail::gValue9E03C0` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 969 | 0x00467E20 | `unknown_libname_34` | 有承接 | `src/platform/RuntimeInitialization.cpp:246 — cg::world::detail::gValue9E03C0` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 970 | 0x00467E30 | `sub_467E30` | 有承接 | `src/platform/RuntimeInitialization.cpp:249 — cg::world::detail::gValue9FB918` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 971 | 0x00467E40 | `unknown_libname_35` | 有承接 | `src/platform/RuntimeInitialization.cpp:249 — cg::world::detail::gValue9FB918` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 972 | 0x00467E60 | `unknown_libname_36` | 有承接 | `src/platform/RuntimeInitialization.cpp:249 — cg::world::detail::gValue9FB918` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 973 | 0x00467E70 | `sub_467E70` | 有承接 | `src/platform/RuntimeInitialization.cpp:252 — cg::world::detail::gValueAF0FE8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 974 | 0x00467E80 | `unknown_libname_37` | 有承接 | `src/platform/RuntimeInitialization.cpp:252 — cg::world::detail::gValueAF0FE8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 975 | 0x00467EA0 | `unknown_libname_38` | 有承接 | `src/platform/RuntimeInitialization.cpp:252 — cg::world::detail::gValueAF0FE8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 976 | 0x00467EB0 | `sub_467EB0` | 有承接 | `src/platform/RuntimeInitialization.cpp:255 — cg::world::detail::gValueC821D8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 977 | 0x00467EC0 | `unknown_libname_39` | 有承接 | `src/platform/RuntimeInitialization.cpp:255 — cg::world::detail::gValueC821D8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 978 | 0x00467EE0 | `unknown_libname_40` | 有承接 | `src/platform/RuntimeInitialization.cpp:255 — cg::world::detail::gValueC821D8` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 979 | 0x00467EF0 | `sub_467EF0` | 有承接 | `src/platform/RuntimeInitialization.cpp:258 — cg::world::detail::gRippleGate` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 980 | 0x00467F00 | `unknown_libname_41` | 有承接 | `src/platform/RuntimeInitialization.cpp:258 — cg::world::detail::gRippleGate` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 981 | 0x00467F20 | `unknown_libname_42` | 有承接 | `src/platform/RuntimeInitialization.cpp:258 — cg::world::detail::gRippleGate` | LCI17同一中央位置直接定义LegacyCryptInt；默认构造及逆序析构由MSVC承接，原RNG／读写序保持，optional与启动包装退休；仅静态，编译／运行未验 |
| 982 | 0x00467F30 | `sub_467F30` | 有正文 | `src/world/SceneState.cpp:5035 — Sub_467F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 983 | 0x004681B0 | `sub_4681B0` | 有正文 | `src/world/SceneState.cpp:5103 — Sub_4681B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 984 | 0x00468200 | `sub_468200` | 有正文 | `src/world/SceneState.cpp:4939 — Sub_468200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 985 | 0x00468290 | `sub_468290` | 有正文 | `src/world/SceneState.cpp:5121 — Sub_468290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 986 | 0x004682E0 | `sub_4682E0` | 有正文 | `src/world/SceneState.cpp:7044 — Sub_4682E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 987 | 0x00468330 | `sub_468330` | 有正文 | `src/world/SceneState.cpp:4984 — Sub_468330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 988 | 0x00468520 | `sub_468520` | 有正文 | `src/world/SceneState.cpp:4197 — Sub_468520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 989 | 0x00468570 | `Map::SaveToDisk` | 有正文 | `src/world/SceneState.cpp:4212 — Sub_468570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 990 | 0x004687B0 | `sub_4687B0` | 有正文 | `src/world/SceneState.cpp:4268 — Sub_4687B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 991 | 0x004688A0 | `sub_4688A0` | 有正文 | `src/world/SceneState.cpp:4301 — Sub_4688A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 992 | 0x00468C00 | `sub_468C00` | 有正文 | `src/world/SceneState.cpp:5145 — Sub_468C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 993 | 0x00468C40 | `sub_468C40` | 有正文 | `src/world/SceneState.cpp:4567 — Sub_468C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 994 | 0x00468F30 | `sub_468F30` | 有正文 | `src/world/SceneState.cpp:7368 — Sub_468F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 995 | 0x00468F70 | `sub_468F70` | 有正文 | `src/world/SceneState.cpp:7386 — Sub_468F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 996 | 0x00468FF0 | `sub_468FF0` | 有正文 | `src/world/SceneState.cpp:546 — CopyWorldProjectionState_468FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 997 | 0x00469020 | `sub_469020` | 有正文 | `src/world/MapSceneDrawing.cpp:329 — Sub_469020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 998 | 0x00469030 | `sub_469030` | 有正文 | `src/world/SceneState.cpp:6821 — Sub_469030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 999 | 0x004691D0 | `sub_4691D0` | 有正文 | `src/world/SceneState.cpp:5248 — Sub_4691D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1000 | 0x00469240 | `sub_469240` | 有正文 | `src/world/SceneState.cpp:5188 — Sub_469240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1001 | 0x00469290 | `sub_469290` | 有正文 | `src/world/SceneState.cpp:5202 — Sub_469290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1002 | 0x004693F0 | `sub_4693F0` | 有正文 | `src/world/SceneState.cpp:6895 — Sub_4693F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1003 | 0x00469810 | `sub_469810` | 有正文 | `src/world/SceneState.cpp:5168 — Sub_469810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1004 | 0x00469870 | `sub_469870` | 有正文 | `src/world/SceneState.cpp:4383 — Sub_469870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1005 | 0x00469C90 | `World::CharaMove` | 有正文 | `src/world/SceneState.cpp:8630 — Sub_469C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1006 | 0x00469FC0 | `sub_469FC0` | 有正文 | `src/world/SceneState.cpp:8713 — Sub_469FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1007 | 0x0046A190 | `sub_46A190` | 有正文 | `src/world/WorldActor.cpp:36561 — Sub_46A190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1008 | 0x0046A410 | `sub_46A410` | 有正文 | `src/world/SceneState.cpp:8810 — Sub_46A410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1009 | 0x0046A5A0 | `sub_46A5A0` | 有正文 | `src/world/SceneState.cpp:8122 — Sub_46A5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1010 | 0x0046A5F0 | `sub_46A5F0` | 有正文 | `src/world/WorldActor.cpp:36711 — Sub_46A5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1011 | 0x0046A700 | `sub_46A700` | 有正文 | `src/world/SceneState.cpp:8854 — Sub_46A700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1012 | 0x0046A940 | `sub_46A940` | 有正文 | `src/world/SceneState.cpp:8142 — Sub_46A940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1013 | 0x0046A990 | `sub_46A990` | 有正文 | `src/world/SceneState.cpp:8017 — Sub_46A990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1014 | 0x0046A9E0 | `sub_46A9E0` | 有正文 | `src/world/SceneState.cpp:7087 — Sub_46A9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1015 | 0x0046AA00 | `sub_46AA00` | 有正文 | `src/world/SceneState.cpp:7096 — Sub_46AA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1016 | 0x0046AB60 | `sub_46AB60` | 有正文 | `src/world/SceneState.cpp:7290 — Sub_46AB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1017 | 0x0046ABD0 | `sub_46ABD0` | 有正文 | `src/world/SceneState.cpp:7188 — Sub_46ABD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1018 | 0x0046AC10 | `sub_46AC10` | 有正文 | `src/world/SceneState.cpp:7205 — Sub_46AC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1019 | 0x0046AC90 | `sub_46AC90` | 有正文 | `src/world/SceneState.cpp:7325 — Sub_46AC90` | LCI10局部临时载荷改整数operator=，一次RNG、目标token／reserved与Y→X发送序保持；仅静态，完整业务／运行未验 |
| 1020 | 0x0046AD00 | `sub_46AD00` | 有正文 | `src/world/SceneState.cpp:7018 — Sub_46AD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1021 | 0x0046ADA0 | `sub_46ADA0` | 有正文 | `src/world/SceneState.cpp:7254 — Sub_46ADA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1022 | 0x0046AE60 | `sub_46AE60` | 有正文 | `src/world/SceneState.cpp:5157 — Sub_46AE60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1023 | 0x0046AEA0 | `sub_46AEA0` | 有正文 | `src/world/SceneState.cpp:7340 — Sub_46AEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1024 | 0x0046AF30 | `sub_46AF30` | 有正文 | `src/world/SceneState.cpp:4534 — Sub_46AF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1025 | 0x0046AFE0 | `sub_46AFE0` | 有正文 | `src/world/SceneState.cpp:8049 — Sub_46AFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1026 | 0x0046B1B0 | `Player::MoveOneStep` | 有正文 | `src/world/WorldActor.cpp:36662 — MovePlayerOneStep_46B1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1027 | 0x0046B370 | `sub_46B370` | 有正文 | `src/world/SceneState.cpp:8587 — Sub_46B370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1028 | 0x0046B4B0 | `sub_46B4B0` | 有正文 | `src/world/WorldActor.cpp:36526 — Sub_46B4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1029 | 0x0046B580 | `sub_46B580` | 有正文 | `src/world/SceneState.cpp:7534 — Sub_46B580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1030 | 0x0046B5C0 | `sub_46B5C0` | 有正文 | `src/world/SceneState.cpp:7548 — Sub_46B5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1031 | 0x0046B6F0 | `sub_46B6F0` | 有正文 | `src/world/SceneState.cpp:7601 — Sub_46B6F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1032 | 0x0046BC90 | `sub_46BC90` | 有正文 | `src/world/SceneState.cpp:7171 — Sub_46BC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1033 | 0x0046BCF0 | `sub_46BCF0` | 有正文 | `src/world/SceneState.cpp:7238 — Sub_46BCF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1034 | 0x0046BD20 | `sub_46BD20` | 有正文 | `src/world/SceneState.cpp:7804 — Sub_46BD20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1035 | 0x0046BD40 | `sub_46BD40` | 有正文 | `src/world/SceneState.cpp:9640 — Sub_46BD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1036 | 0x0046BD60 | `sub_46BD60` | 有正文 | `src/world/MapDrawCache.cpp:348 — Sub_46BD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1037 | 0x0046BE50 | `sub_46BE50` | 有正文 | `src/world/MapDrawCache.cpp:373 — Sub_46BE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1038 | 0x0046C060 | `YobiMapChara::DrawGearNormal` | 有正文 | `src/world/MapDrawCache.cpp:440 — DrawGearNormal_46C060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1039 | 0x0046C1C0 | `sub_46C1C0` | 有正文 | `src/world/MapDrawCache.cpp:489 — Sub_46C1C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1040 | 0x0046C2F0 | `sub_46C2F0` | 有正文 | `src/world/MapDrawCache.cpp:482 — Sub_46C2F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1041 | 0x0046C300 | `sub_46C300` | 有正文 | `src/world/MapDrawCache.cpp:100 — CompareMapObject_46C300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1042 | 0x0046C420 | `sub_46C420` | 有正文 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1043 | 0x0046C440 | `sub_46C440` | 有正文 | `src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1044 | 0x0046C4D0 | `sub_46C4D0` | 有正文 | `src/world/MapDrawCache.cpp:186 — MergeMapDrawCache_46C4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1045 | 0x0046C760 | `sub_46C760` | 有正文 | `src/world/SceneState.cpp:4844 — Sub_46C760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1046 | 0x0046C840 | `sub_46C840` | 有正文 | `src/world/SceneState.cpp:4670 — Sub_46C840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1047 | 0x0046CDF0 | `sub_46CDF0` | 有正文 | `src/world/SceneState.cpp:7522 — Sub_46CDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1048 | 0x0046CE30 | `sub_46CE30` | 有正文 | `src/world/SceneState.cpp:500 — ProjectWorldToScreen_46CE30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1049 | 0x0046CE80 | `sub_46CE80` | 有正文 | `src/world/SceneState.cpp:512 — ProjectScreenToWorld_46CE80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1050 | 0x0046CEC0 | `sub_46CEC0` | 有正文 | `src/world/SceneState.cpp:4967 — Sub_46CEC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1051 | 0x0046CEE0 | `sub_46CEE0` | 有正文 | `src/world/SceneState.cpp:4974 — Sub_46CEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1052 | 0x0046CF00 | `sub_46CF00` | 有正文 | `src/world/SceneState.cpp:7407 — Sub_46CF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1053 | 0x0046CF20 | `sub_46CF20` | 有正文 | `src/world/SceneState.cpp:7419 — Sub_46CF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1054 | 0x0046CF60 | `sub_46CF60` | 有正文 | `src/world/MapSceneDrawing.cpp:197 — Sub_46CF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1055 | 0x0046D9C0 | `sub_46D9C0` | 有正文 | `src/world/SceneState.cpp:6991 — Sub_46D9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1056 | 0x0046DA80 | `sub_46DA80` | 有正文 | `src/world/SceneState.cpp:7064 — Sub_46DA80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1057 | 0x0046DB00 | `sub_46DB00` | 有正文 | `src/world/SceneState.cpp:4879 — Sub_46DB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1058 | 0x0046DC60 | `sub_46DC60` | 有正文 | `src/world/SceneState.cpp:4870 — Sub_46DC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1059 | 0x0046DC90 | `sub_46DC90` | 有正文 | `src/world/SceneState.cpp:4917 — Sub_46DC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1060 | 0x0046DD40 | `sub_46DD40` | 有正文 | `src/Yobi/map_observation.cpp:1576 — Sub_46DD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1061 | 0x0046DD90 | `sub_46DD90` | 有正文 | `src/Yobi/map_observation.cpp:5521 — Sub_46DD90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1062 | 0x0046DE50 | `sub_46DE50` | 有正文 | `src/Yobi/map_observation.cpp:5485 — Sub_46DE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1063 | 0x0046DFF0 | `sub_46DFF0` | 有正文 | `src/world/MapDrawCache.cpp:407 — Sub_46DFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1064 | 0x0046E0D0 | `sub_46E0D0` | 有正文 | `src/world/SceneState.cpp:1581 — Sub_46E0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1065 | 0x0046E2A0 | `sub_46E2A0` | 有正文 | `src/world/SceneState.cpp:933 — Sub_46E2A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1066 | 0x0046E340 | `sub_46E340` | 有正文 | `src/world/SceneState.cpp:966 — Sub_46E340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1067 | 0x0046E390 | `sub_46E390` | 有正文 | `src/world/SceneState.cpp:990 — Sub_46E390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1068 | 0x0046E3E0 | `sub_46E3E0` | 有正文 | `src/world/SceneState.cpp:1016 — Sub_46E3E0` | 正文已有；实际五个地图效果调用由同文件1615行的 ApplyMapEffectDirection 承接，调用点1823／1887／1953／2019／2114；独立包装无当前调用不等于缺接线，本步仅定位提供者，不重验原正文或提升完整业务／运行门禁 |
| 1069 | 0x0046E480 | `sub_46E480` | 有正文 | `src/world/SceneState.cpp:1208 — Sub_46E480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1070 | 0x0046E4A0 | `sub_46E4A0` | 有正文 | `src/world/SceneState.cpp:1571 — Sub_46E4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1071 | 0x0046E4C0 | `sub_46E4C0` | 有正文 | `src/world/SceneState.cpp:1481 — Sub_46E4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1072 | 0x0046E4E0 | `sub_46E4E0` | 有正文 | `src/world/SceneState.cpp:1217 — Sub_46E4E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1073 | 0x0046E6C0 | `sub_46E6C0` | 有正文 | `src/world/SceneState.cpp:1283 — Sub_46E6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1074 | 0x0046E940 | `sub_46E940` | 有正文 | `src/world/SceneState.cpp:1347 — Sub_46E940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1075 | 0x0046EBB0 | `sub_46EBB0` | 有正文 | `src/world/SceneState.cpp:1416 — Sub_46EBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1076 | 0x0046ED50 | `sub_46ED50` | 有正文 | `src/world/SceneState.cpp:1488 — Sub_46ED50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1077 | 0x0046F000 | `sub_46F000` | 有正文 | `src/world/SceneState.cpp:1695 — Sub_46F000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1078 | 0x0046F090 | `sub_46F090` | 有正文 | `src/world/SceneState.cpp:1646 — Sub_46F090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1079 | 0x0046F160 | `sub_46F160` | 有正文 | `src/world/SceneState.cpp:1625 — Sub_46F160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1080 | 0x0046F1D0 | `sub_46F1D0` | 有正文 | `src/world/SceneState.cpp:1723 — Sub_46F1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1081 | 0x0046F680 | `World::RendererCursorGrid` | 有正文 | `src/world/SceneState.cpp:8164 — Sub_46F680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1082 | 0x0046F8F0 | `sub_46F8F0` | 有正文 | `src/world/SceneState.cpp:7431 — Sub_46F8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1083 | 0x0046F930 | `sub_46F930` | 有正文 | `src/world/SceneState.cpp:7452 — Sub_46F930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1084 | 0x0046F950 | `sub_46F950` | 有正文 | `src/world/SceneState.cpp:7461 — Sub_46F950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1085 | 0x0046F970 | `sub_46F970` | 有正文 | `src/world/SceneState.cpp:7468 — Sub_46F970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1086 | 0x0046FAF0 | `RAND` | 有正文 | `src/platform/MainLoopSupport.cpp:614 — RandomRange_46FAF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1087 | 0x0046FB20 | `sub_46FB20` | 有正文 | `src/world/SceneState.cpp:560 — InitializeRotationTable_46FB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1088 | 0x0046FB80 | `sub_46FB80` | 有正文 | `src/world/SceneState.cpp:7511 — Sub_46FB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1089 | 0x0046FBA0 | `sub_46FBA0` | 有正文 | `src/ui/windows/WindowShortcutBar.cpp:78 — WindowShortcutBar_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1090 | 0x0046FBC0 | `sub_46FBC0` | 有正文 | `src/ui/WindowRuntime.cpp:3540 — WindowRuntime_DrawDefault` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1091 | 0x0046FBD0 | `sub_46FBD0` | 有正文 | `src/ui/windows/WindowShortcutBar.cpp:87 — WindowShortcutBar_ButtonCollapse` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1092 | 0x0046FC60 | `sub_46FC60` | 有正文 | `src/ui/windows/WindowShortcutBar.cpp:113 — WindowShortcutBar_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1093 | 0x00470050 | `sub_470050` | 有正文 | `src/world/SceneState.cpp:8270 — Sub_470050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1094 | 0x00470150 | `sub_470150` | 有正文 | `src/world/SceneState.cpp:8302 — Sub_470150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1095 | 0x00470270 | `sub_470270` | 有正文 | `src/world/SceneState.cpp:8337 — Sub_470270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1096 | 0x004703C0 | `sub_4703C0` | 有正文 | `src/world/SceneState.cpp:8377 — Sub_4703C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1097 | 0x004704A0 | `sub_4704A0` | 有正文 | `src/world/SceneState.cpp:8407 — Sub_4704A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1098 | 0x004705D0 | `sub_4705D0` | 有正文 | `src/ui/WindowRuntime.cpp:3401 — WindowRuntime_Initialize` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1099 | 0x004705F0 | `sub_4705F0` | 有正文 | `src/platform/ClientShutdown.cpp:31 — Sub_4705F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1100 | 0x00470600 | `sub_470600` | 有正文 | `src/ui/WindowRuntime.cpp:3428 — WindowRuntime_InitializeLogin` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1101 | 0x00470670 | `sub_470670` | 有正文 | `src/world/SceneState.cpp:8443 — Sub_470670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1102 | 0x004708A0 | `sub_4708A0` | 有正文 | `src/ui/windows/WindowStatusBar.cpp:232 — WindowStatusBar_ButtonEmote` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1103 | 0x00470910 | `sub_470910` | 有正文 | `src/ui/windows/WindowStatusBar.cpp:252 — WindowStatusBar_ButtonMiniMap` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1104 | 0x00470980 | `sub_470980` | 有正文 | `src/ui/WindowRuntime.cpp:3381 — WindowRuntime_OpenNormalScene` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1105 | 0x00470A80 | `sub_470A80` | 有正文 | `src/ui/windows/WindowStatusBar.cpp:273 — WindowStatusBar_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1106 | 0x00470A9F | `?classic@locale@std@@SAABV12@XZ_0` | 有承接 | `src/ui/windows/WindowStatusBar.cpp:273 — WindowStatusBar_Update` | 原共享尾为调用 470AC0 后返回 1；并入实际父正文；完整业务／运行未验 |
| 1107 | 0x00470AB0 | `?classic@locale@std@@SAABV12@XZ_1` | 有正文 | `src/ui/windows/WindowStatusBar.cpp:284 — WindowCompactStatusBar_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1108 | 0x00470AC0 | `sub_470AC0` | 有正文 | `src/ui/windows/WindowStatusBar.cpp:292 — WindowStatusBar_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1109 | 0x00471270 | `UIWindowInstance::free` | 有正文 | `src/ui/WindowRuntime.cpp:4646 — WindowRuntime_Free` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1110 | 0x00471440 | `Window::open` | 有正文 | `src/ui/WindowRuntime.cpp:4377 — WindowRuntime_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1111 | 0x004718F0 | `sub_4718F0` | 有正文 | `src/ui/windows/WindowStatusBar.cpp:353 — WindowCompactStatusBar_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1112 | 0x00471DF0 | `sub_471DF0` | 有正文 | `src/ui/windows/WindowStatusBar.cpp:414 — WindowStatusBar_HandleOptionButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1113 | 0x00472160 | `Windows::close` | 有正文 | `src/ui/WindowRuntime.cpp:4716 — WindowRuntime_Close` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1114 | 0x00472190 | `Window::CloseAll` | 有正文 | `src/ui/WindowRuntime.cpp:4725 — WindowRuntime_CloseAll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1115 | 0x00472200 | `sub_472200` | 有正文 | `src/ui/WindowRuntime.cpp:2827 — WindowRuntime_ClearSavedWindowMarkers` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1116 | 0x00472220 | `sub_472220` | 有正文 | `src/ui/WindowRuntime.cpp:2835 — WindowRuntime_MarkExistingWindows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1117 | 0x00472240 | `Window::CloseByCategory` | 有正文 | `src/ui/WindowRuntime.cpp:4818 — WindowRuntime_CloseByCategory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1118 | 0x004722A0 | `MouseEvent::Reset` | 有正文 | `src/ui/WindowRuntime.cpp:2845 — WindowRuntime_ResetMouseAndItemFlags` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1119 | 0x004722E0 | `sub_4722E0` | 有正文 | `src/ui/WindowRuntime.cpp:2859 — WindowRuntime_ClearElementFocus` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1120 | 0x004722F0 | `sub_4722F0` | 有正文 | `src/ui/WindowRuntime.cpp:2865 — WindowRuntime_RestoreMarkedWindows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1121 | 0x004723D0 | `sub_4723D0` | 有正文 | `src/ui/windows/WindowStatusBar.cpp:488 — WindowStatusBar_HandleHintButton` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1122 | 0x004729B0 | `Window::DrawCurrentWindow` | 有正文 | `src/ui/WindowRuntime.cpp:4840 — WindowRuntime_DrawCurrent` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1123 | 0x00472D20 | `Window::ProsseFocus` | 有正文 | `src/ui/WindowRuntime.cpp:3874 — WindowDrag_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1124 | 0x00472F00 | `Mouse::isInRect` | 有正文 | `src/ui/WindowRuntime.cpp:3736 — WindowRuntime_HitTestRect` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1125 | 0x00472F40 | `sub_472F40` | 有正文 | `src/world/SceneState.cpp:2324 — Sub_472F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1126 | 0x00472F70 | `sub_472F70` | 有正文 | `src/world/SceneState.cpp:2335 — Sub_472F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1127 | 0x00472FA0 | `sub_472FA0` | 有正文 | `src/world/SceneState.cpp:2341 — Sub_472FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1128 | 0x00472FD0 | `sub_472FD0` | 有正文 | `src/world/SceneState.cpp:2347 — Sub_472FD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1129 | 0x00473000 | `sub_473000` | 有正文 | `src/world/SceneState.cpp:2353 — Sub_473000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1130 | 0x00473030 | `sub_473030` | 有正文 | `src/world/SceneState.cpp:2359 — Sub_473030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1131 | 0x00473080 | `sub_473080` | 有正文 | `src/ui/windows/WindowStatusBar.cpp:463 — WindowStatusBar_ReadInjuredPetIndex` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1132 | 0x004730A0 | `Window::sortLayer` | 有正文 | `src/ui/WindowRuntime.cpp:3957 — WindowRuntime_CompactAndReorder` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1133 | 0x004731B0 | `sub_4731B0` | 有正文 | `src/ui/windows/WindowStatusBar.cpp:474 — WindowStatusBar_FindSortedPetIndex` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1134 | 0x004731E0 | `sub_4731E0` | 有正文 | `src/ui/windows/WindowBottomBar.cpp:322 — WindowBottomBar_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1135 | 0x00473560 | `Window::ProcessChildElement` | 有正文 | `src/ui/WindowRuntime.cpp:4057 — WindowRuntime_DispatchElementMouse` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1136 | 0x00473840 | `sub_473840` | 有正文 | `src/ui/windows/WindowBottomBar.cpp:82 — WindowBottomBar_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1137 | 0x00473E40 | `MouseEvent::Update` | 有正文 | `src/ui/WindowRuntime.cpp:4006 — WindowRuntime_AdvanceMousePhase` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1138 | 0x00473F80 | `Windows::Update` | 有正文 | `src/ui/WindowRuntime.cpp:4265 — WindowRuntime_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1139 | 0x004740F0 | `sub_4740F0` | 有正文 | `src/ui/WindowRuntime.cpp:4236 — WindowInput_FormatSkillSlotCount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1140 | 0x00474170 | `sub_474170` | 有正文 | `src/ui/WindowRuntime.cpp:926 — WindowElement_HandleSideArea` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1141 | 0x004741A0 | `sub_4741A0` | 有正文 | `src/ui/WindowRuntime.cpp:4123 — WindowElement_HandleVerticalScroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1142 | 0x00474270 | `sub_474270` | 有正文 | `src/ui/WindowRuntime.cpp:3617 — WindowScroll_ReadPosition` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1143 | 0x004742B0 | `sub_4742B0` | 有正文 | `src/ui/WindowRuntime.cpp:3420 — WindowScroll_ResetPosition` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1144 | 0x004742C0 | `sub_4742C0` | 有正文 | `src/ui/WindowRuntime.cpp:3633 — WindowScroll_SetPosition` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1145 | 0x00474310 | `sub_474310` | 有正文 | `src/ui/windows/WindowPlayerProfile.cpp:450 — WindowPlayerProfile_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1146 | 0x00474360 | `sub_474360` | 有正文 | `src/ui/windows/WindowPlayerAttributes.cpp:368 — WindowPlayerAttributes_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1147 | 0x004743B0 | `sub_4743B0` | 有正文 | `src/ui/WindowRuntime.cpp:3656 — WindowScroll_ApplyWheel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1148 | 0x004744A0 | `sub_4744A0` | 有正文 | `src/ui/windows/WindowPlayerTitles.cpp:475 — WindowPlayerTitles_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1149 | 0x004744F0 | `sub_4744F0` | 有正文 | `src/ui/windows/WindowOwnProfile.cpp:282 — WindowOwnProfile_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1150 | 0x00474540 | `?classic@locale@std@@SAABV12@XZ_2` | 有正文 | `src/ui/windows/WindowPlayerProfile.cpp:443 — WindowPlayerProfile_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1151 | 0x00474550 | `sub_474550` | 有正文 | `src/ui/WindowRuntime.cpp:1309 — WindowInput_HandleNumber` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1152 | 0x00474FB0 | `sub_474FB0` | 有正文 | `src/ui/windows/WindowPlayerProfile.cpp:317 — WindowPlayerProfile_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1153 | 0x004758D0 | `YobiWindow::Update` | 有正文 | `src/Yobi/window.cpp:1891 — Sub_4758D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1154 | 0x00475950 | `Yobi::newWindow` | 有正文 | `src/Yobi/window.cpp:1922 — Sub_475950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1155 | 0x00475AA0 | `Yobi::newWindow2` | 有正文 | `src/ui/WindowRuntime.cpp:4543 — WindowRuntime_CreateAnimation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1156 | 0x00475C10 | `sub_475C10` | 有正文 | `src/ui/WindowRuntime.cpp:2943 — WindowRuntime_RestorePositions` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1157 | 0x00475D40 | `Window::GetFocusWinId` | 有正文 | `src/ui/WindowRuntime.cpp:3749 — WindowDrag_ReadFocusWindowId` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1158 | 0x00475D50 | `sub_475D50` | 有正文 | `src/ui/WindowRuntime.cpp:3755 — WindowDrag_ReadFocusElement` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1159 | 0x00475D60 | `sub_475D60` | 有正文 | `src/ui/WindowRuntime.cpp:3761 — WindowDrag_ReadKind` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1160 | 0x00475D70 | `sub_475D70` | 有正文 | `src/ui/WindowRuntime.cpp:3767 — WindowDrag_ReadX` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1161 | 0x00475D80 | `sub_475D80` | 有正文 | `src/ui/WindowRuntime.cpp:3773 — WindowDrag_ReadY` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1162 | 0x00475D90 | `sub_475D90` | 有正文 | `src/ui/WindowRuntime.cpp:3779 — WindowDrag_ReadSourceIndex` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1163 | 0x00475DA0 | `sub_475DA0` | 有正文 | `src/ui/WindowRuntime.cpp:3786 — WindowDrag_Start` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1164 | 0x00475DF0 | `sub_475DF0` | 有正文 | `src/ui/windows/WindowPlayerAttributes.cpp:361 — WindowPlayerAttributes_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1165 | 0x00475E00 | `sub_475E00` | 有正文 | `src/ui/windows/WindowPlayerAttributes.cpp:207 — WindowPlayerAttributes_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1166 | 0x004763B0 | `sub_4763B0` | 有正文 | `src/ui/WindowRuntime.cpp:3849 — WindowDrag_SetFocus` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1167 | 0x004763E0 | `MouseEvent::reset` | 有正文 | `src/ui/WindowRuntime.cpp:3862 — WindowDrag_ResetMouse` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1168 | 0x00476410 | `sub_476410` | 有正文 | `src/ui/WindowRuntime.cpp:4623 — WindowDrag_Cancel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1169 | 0x00476490 | `MouseEvent::setupEventCallback` | 有正文 | `src/ui/WindowRuntime.cpp:3805 — WindowDrag_StartAtPosition` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1170 | 0x00476500 | `InvDragReset` | 有正文 | `src/ui/WindowRuntime.cpp:3827 — WindowDrag_Reset` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1171 | 0x00476570 | `sub_476570` | 有正文 | `src/ui/WindowRuntime.cpp:4176 — WindowInput_ActivateElement` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1172 | 0x00476680 | `sub_476680` | 有正文 | `src/world/SceneState.cpp:1899 — Sub_476680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1173 | 0x00476740 | `sub_476740` | 有正文 | `src/input/Input.cpp:560 — Sub_476740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1174 | 0x004767E0 | `sub_4767E0` | 有正文 | `src/ui/windows/WindowPlayerAttributes.cpp:299 — WindowPlayerAttributes_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1175 | 0x00477160 | `sub_477160` | 有正文 | `src/ui/WindowRuntime.cpp:4585 — WindowNumber_InitializeLegacySpacing` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1176 | 0x00477170 | `sub_477170` | 有正文 | `src/ui/WindowRuntime.cpp:4592 — WindowNumber_InitializeSpacing` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1177 | 0x004771B0 | `sub_4771B0` | 有正文 | `src/ui/WindowRuntime.cpp:3466 — WindowNumber_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1178 | 0x00477460 | `sub_477460` | 有正文 | `src/ui/WindowRuntime.cpp:3527 — WindowNumber_MeasureWidth` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1179 | 0x00477490 | `Window::FindByWinId` | 有正文 | `src/ui/WindowRuntime.cpp:3453 — WindowRuntime_FindById` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1180 | 0x004774C0 | `sub_4774C0` | 有正文 | `src/ui/windows/WindowPlayerTitles.cpp:458 — WindowPlayerTitles_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1181 | 0x0047752C | `?classic@locale@std@@SAABV12@XZ_4` | 有承接 | `src/ui/windows/WindowPlayerTitles.cpp:458 — WindowPlayerTitles_Update` | 原共享尾为调用 478010 后返回 1；并入实际父正文；完整业务／运行未验 |
| 1182 | 0x00477540 | `sub_477540` | 有正文 | `src/ui/windows/WindowPlayerTitles.cpp:146 — WindowPlayerTitles_EditDraft` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1183 | 0x00477600 | `sub_477600` | 有正文 | `src/ui/windows/WindowPlayerTitles.cpp:170 — WindowPlayerTitles_SelectTitle` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1184 | 0x00477710 | `sub_477710` | 有正文 | `src/ui/windows/WindowPlayerTitles.cpp:204 — WindowPlayerTitles_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1185 | 0x00478010 | `sub_478010` | 有正文 | `src/ui/windows/WindowPlayerTitles.cpp:410 — WindowPlayerTitles_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1186 | 0x00478320 | `?classic@locale@std@@SAABV12@XZ_5` | 有正文 | `src/ui/windows/WindowOwnProfile.cpp:275 — WindowOwnProfile_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1187 | 0x00478330 | `sub_478330` | 有正文 | `src/ui/windows/WindowProfile.cpp:23 — WindowProfile_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1188 | 0x00478A10 | `sub_478A10` | 有正文 | `src/ui/windows/WindowOwnProfile.cpp:197 — WindowOwnProfile_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1189 | 0x004792D0 | `sub_4792D0` | 有正文 | `src/ui/windows/WindowOwnProfile.cpp:296 — WindowOwnProfile_EditField` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1190 | 0x00479610 | `sub_479610` | 有正文 | `src/ui/WindowRuntime.cpp:2926 — WindowRuntime_ResetOperationReferences` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1191 | 0x00479640 | `sub_479640` | 有正文 | `src/ui/WindowRuntime.cpp:2908 — WindowRuntime_ResetOperationCount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1192 | 0x00479650 | `___inc_tmpoff` | 有正文 | `src/ui/WindowRuntime.cpp:3575 — WindowRuntime_IncrementOperationCount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1193 | 0x00479660 | `sub_479660` | 有正文 | `src/ui/WindowRuntime.cpp:3581 — WindowRuntime_DecrementOperationCount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1194 | 0x004796C0 | `sub_4796C0` | 有正文 | `src/ui/WindowRuntime.cpp:2914 — WindowRuntime_ResetGatherOperationCount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1195 | 0x004796D0 | `sub_4796D0` | 有正文 | `src/ui/WindowRuntime.cpp:3563 — WindowRuntime_IncrementGatherOperationCount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1196 | 0x004796E0 | `sub_4796E0` | 有正文 | `src/ui/WindowRuntime.cpp:3569 — WindowRuntime_DecrementGatherOperationCount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1197 | 0x004796F0 | `sub_4796F0` | 有正文 | `src/ui/WindowRuntime.cpp:2920 — WindowRuntime_ResetSkillOperationCount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1198 | 0x00479700 | `sub_479700` | 有正文 | `src/ui/windows/WindowSkill.cpp:711 — WindowSkill_HandleDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1199 | 0x004797A0 | `sub_4797A0` | 有正文 | `src/ui/windows/WindowSkill.cpp:738 — WindowSkill_StartDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1200 | 0x004797C0 | `sub_4797C0` | 有正文 | `src/ui/windows/WindowSkill.cpp:338 — WindowSkill_AdjustSpecialHoverCount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1201 | 0x00479820 | `sub_479820` | 有正文 | `src/ui/windows/WindowSkill.cpp:355 — WindowSkill_FindSpecialSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1202 | 0x00479860 | `GetPetCrystalLv` | 有正文 | `src/ui/windows/WindowSkill.cpp:369 — WindowSkill_ReadCrystalLevel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1203 | 0x00479920 | `sub_479920` | 有正文 | `src/ui/windows/WindowSkill.cpp:386 — WindowSkill_ReadCrystalElement` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1204 | 0x004799D0 | `sub_4799D0` | 有正文 | `src/ui/windows/WindowSkill.cpp:403 — WindowSkill_ReadBattlePetImage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1205 | 0x00479A00 | `sub_479A00` | 有正文 | `src/ui/windows/WindowSkill.cpp:410 — WindowSkill_FindPetTechnique` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1206 | 0x00479A70 | `sub_479A70` | 有正文 | `src/ui/windows/WindowSkill.cpp:428 — WindowSkill_CheckPetLevel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1207 | 0x00479AB0 | `sub_479AB0` | 有正文 | `src/ui/windows/WindowSkill.cpp:439 — WindowSkill_CheckPetFp` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1208 | 0x00479B00 | `sub_479B00` | 有正文 | `src/ui/windows/WindowSkill.cpp:454 — WindowSkill_CanUseSpecialSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1209 | 0x00479B80 | `sub_479B80` | 有正文 | `src/ui/windows/WindowSkill.cpp:801 — WindowSkill_ShowSkillList` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1210 | 0x00479C80 | `sub_479C80` | 有正文 | `src/ui/windows/WindowSkill.cpp:938 — WindowSkill_ShowTechniques` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1211 | 0x00479DA0 | `sub_479DA0` | 有正文 | `src/ui/windows/WindowSkill.cpp:2129 — WindowSkill_ShowRecipes` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1212 | 0x00479EA0 | `sub_479EA0` | 有正文 | `src/ui/windows/WindowSkill.cpp:991 — WindowSkill_ShowBattleSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1213 | 0x0047A000 | `sub_47A000` | 有正文 | `src/ui/windows/WindowSkill.cpp:1049 — WindowSkill_ShowBattleTechniques` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1214 | 0x0047A140 | `sub_47A140` | 有正文 | `src/ui/windows/WindowSkill.cpp:2378 — WindowSkill_SyncStates` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1215 | 0x0047A1A0 | `sub_47A1A0` | 有正文 | `src/ui/windows/WindowSkill.cpp:1466 — WindowSkill_ShowPetSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1216 | 0x0047A320 | `sub_47A320` | 有正文 | `src/ui/windows/WindowSkill.cpp:1513 — WindowSkill_ShowRidingSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1217 | 0x0047A490 | `sub_47A490` | 有正文 | `src/ui/windows/WindowSkill.cpp:1226 — WindowSkill_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1218 | 0x0047A4C0 | `sub_47A4C0` | 有正文 | `src/ui/windows/WindowSkill.cpp:1239 — WindowSkill_OpenBattleSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1219 | 0x0047A500 | `BattleScene::SelectionCallback2` | 有正文 | `src/ui/windows/WindowSkill.cpp:1251 — WindowSkill_OpenBattleTechniques` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1220 | 0x0047A540 | `BattleScene::OpenSkillWindow` | 有正文 | `src/ui/windows/WindowSkill.cpp:1560 — WindowSkill_OpenPetSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1221 | 0x0047A580 | `BattleScene::OpenSkillWindow2` | 有正文 | `src/ui/windows/WindowSkill.cpp:1572 — WindowSkill_OpenRidingSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1222 | 0x0047A5C0 | `sub_47A5C0` | 有正文 | `src/ui/windows/WindowSkill.cpp:1630 — WindowSkill_Back` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1223 | 0x0047A620 | `sub_47A620` | 有正文 | `src/ui/windows/WindowSkill.cpp:1767 — WindowSkill_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1224 | 0x0047A7A0 | `sub_47A7A0` | 有正文 | `src/ui/windows/WindowSkill.cpp:1822 — WindowSkill_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1225 | 0x0047B110 | `sub_47B110` | 有正文 | `src/ui/windows/WindowSkill.cpp:1584 — WindowSkill_ButtonBack` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1226 | 0x0047B210 | `sub_47B210` | 有正文 | `src/ui/windows/WindowSkill.cpp:1653 — WindowSkill_ButtonUse` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1227 | 0x0047B3D0 | `sub_47B3D0` | 有正文 | `src/ui/windows/WindowSkill.cpp:2001 — WindowSkill_ButtonPreviousRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1228 | 0x0047B470 | `sub_47B470` | 有正文 | `src/ui/windows/WindowSkill.cpp:2029 — WindowSkill_ButtonNextRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1229 | 0x0047B520 | `sub_47B520` | 有正文 | `src/ui/windows/WindowSkill.cpp:2059 — WindowSkill_ButtonPreviousPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1230 | 0x0047B5D0 | `sub_47B5D0` | 有正文 | `src/ui/windows/WindowSkill.cpp:2086 — WindowSkill_ButtonNextPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1231 | 0x0047B680 | `sub_47B680` | 有正文 | `src/ui/windows/WindowSkill.cpp:2116 — WindowSkill_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1232 | 0x0047B6C0 | `sub_47B6C0` | 有正文 | `src/ui/windows/WindowSkill.cpp:678 — WindowSkill_DrawSkillRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1233 | 0x0047B820 | `sub_47B820` | 有正文 | `src/ui/windows/WindowSkill.cpp:744 — WindowSkill_HandleSkillRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1234 | 0x0047B9B0 | `sub_47B9B0` | 有正文 | `src/ui/windows/WindowSkill.cpp:849 — WindowSkill_HandleTechniqueRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1235 | 0x0047BC00 | `sub_47BC00` | 有正文 | `src/ui/windows/WindowSkill.cpp:2216 — WindowSkill_SelectSkillType` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1236 | 0x0047C240 | `sub_47C240` | 有正文 | `src/ui/windows/WindowSkill.cpp:907 — WindowSkill_HandleRecipeRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1237 | 0x0047C3A0 | `sub_47C3A0` | 有正文 | `src/ui/windows/WindowSkill.cpp:2160 — WindowSkill_SelectRecipe` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1238 | 0x0047C570 | `sub_47C570` | 有正文 | `src/ui/windows/WindowSkill.cpp:1095 — WindowSkill_HandleBattleSkillRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1239 | 0x0047C700 | `sub_47C700` | 有正文 | `src/ui/windows/WindowSkill.cpp:1157 — WindowSkill_SelectBattleTechnique` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1240 | 0x0047C950 | `sub_47C950` | 有正文 | `src/ui/windows/WindowSkill.cpp:1703 — WindowSkill_ButtonPetSkillBack` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1241 | 0x0047CB00 | `sub_47CB00` | 有正文 | `src/ui/windows/WindowSkill.cpp:1265 — WindowSkill_DrawPetSkillRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1242 | 0x0047CC80 | `sub_47CC80` | 有正文 | `src/ui/windows/WindowSkill.cpp:1341 — WindowSkill_SelectPetSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1243 | 0x0047CE70 | `sub_47CE70` | 有正文 | `src/ui/windows/WindowSkill.cpp:1304 — WindowSkill_DrawRidingSkillRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1244 | 0x0047CFE0 | `sub_47CFE0` | 有正文 | `src/ui/windows/WindowSkill.cpp:1406 — WindowSkill_SelectRidingSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1245 | 0x0047D170 | `sub_47D170` | 有正文 | `src/ui/windows/WindowCraft.cpp:3056 — WindowCraft_SetPreviewPositions` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1246 | 0x0047D220 | `sub_47D220` | 有正文 | `src/ui/windows/WindowCraft.cpp:3080 — WindowCraft_ConfigurePreviews_47D220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1247 | 0x0047D280 | `sub_47D280` | 有正文 | `src/ui/windows/WindowCraft.cpp:3562 — WindowCraft_SetPreviewImages` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1248 | 0x0047D2E0 | `sub_47D2E0` | 有正文 | `src/ui/windows/WindowCraft.cpp:3527 — WindowCraft_AnimatePreviewPositions` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1249 | 0x0047D490 | `sub_47D490` | 有正文 | `src/ui/windows/WindowCraft.cpp:442 — WindowCraft_ResetItemSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1250 | 0x0047D4A0 | `sub_47D4A0` | 有正文 | `src/ui/windows/WindowCraft.cpp:448 — WindowCraft_ReadItemSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1251 | 0x0047D4B0 | `sub_47D4B0` | 有正文 | `src/ui/windows/WindowCraft.cpp:2619 — WindowCraft_HasRecipeCrystalLevel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1252 | 0x0047D4F0 | `sub_47D4F0` | 有正文 | `src/ui/windows/WindowCraft.cpp:2628 — WindowCraft_HasSelectedCrystalLevel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1253 | 0x0047D530 | `sub_47D530` | 有正文 | `src/ui/windows/WindowCraft.cpp:2639 — WindowCraft_HasMaterialCrystalLevel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1254 | 0x0047D5B0 | `sub_47D5B0` | 有正文 | `src/ui/windows/WindowCraft.cpp:2657 — WindowCraft_HasFirstMaterialCrystalLevel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1255 | 0x0047D5F0 | `sub_47D5F0` | 有正文 | `src/ui/windows/WindowCraft.cpp:776 — WindowCraft_OpenRecipe` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1256 | 0x0047D780 | `sub_47D780` | 有正文 | `src/ui/windows/WindowCraft.cpp:800 — WindowCraft_OpenItemWork` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1257 | 0x0047D8F0 | `sub_47D8F0` | 有正文 | `src/ui/windows/WindowCraft.cpp:820 — WindowCraft_OpenDecoration` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1258 | 0x0047DA70 | `sub_47DA70` | 有正文 | `src/ui/windows/WindowCraft.cpp:844 — WindowCraft_OpenRefining` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1259 | 0x0047DB50 | `sub_47DB50` | 有正文 | `src/ui/windows/WindowCraft.cpp:865 — WindowCraft_OpenMaterials` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1260 | 0x0047DC20 | `sub_47DC20` | 有正文 | `src/ui/windows/WindowCraft.cpp:346 — WindowCraft_ReleaseMaterials` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1261 | 0x0047DCD0 | `sub_47DCD0` | 有正文 | `src/ui/windows/WindowCraft.cpp:384 — WindowCraft_ResetSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1262 | 0x0047DE00 | `sub_47DE00` | 有正文 | `src/ui/windows/WindowCraft.cpp:1488 — WindowCraft_HandleMaterialDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1263 | 0x0047DF30 | `sub_47DF30` | 有正文 | `src/ui/windows/WindowCraft.cpp:1526 — WindowCraft_StartMaterialDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1264 | 0x0047DF50 | `sub_47DF50` | 有正文 | `src/ui/windows/WindowCraft.cpp:1262 — WindowCraft_InitializeRecipe` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1265 | 0x0047E1B0 | `sub_47E1B0` | 有正文 | `src/ui/windows/WindowCraft.cpp:579 — WindowCraft_WaitCraft` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1266 | 0x0047E5E0 | `sub_47E5E0` | 有正文 | `src/ui/windows/WindowCraft.cpp:3147 — WindowCraft_ShowRecipeResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1267 | 0x0047EBE0 | `sub_47EBE0` | 有正文 | `src/ui/windows/WindowCraft.cpp:3238 — WindowCraft_RefreshRecipeResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1268 | 0x0047EC00 | `sub_47EC00` | 有正文 | `src/ui/windows/WindowCraft.cpp:2548 — WindowCraft_InitializeItemWork` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1269 | 0x0047EF00 | `sub_47EF00` | 有正文 | `src/ui/windows/WindowCraft.cpp:615 — WindowCraft_WaitItemWork` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1270 | 0x0047F3D0 | `sub_47F3D0` | 有正文 | `src/ui/windows/WindowCraft.cpp:3356 — WindowCraft_ShowItemWorkResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1271 | 0x0047FAC0 | `sub_47FAC0` | 有正文 | `src/ui/windows/WindowCraft.cpp:3465 — WindowCraft_RefreshItemWorkResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1272 | 0x0047FAE0 | `sub_47FAE0` | 有正文 | `src/ui/windows/WindowCraft.cpp:884 — WindowCraft_InitializeDecoration` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1273 | 0x0047FD00 | `sub_47FD00` | 有正文 | `src/ui/windows/WindowCraft.cpp:901 — WindowCraft_InitializePetDecoration` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1274 | 0x0047FF20 | `sub_47FF20` | 有正文 | `src/ui/windows/WindowCraft.cpp:918 — WindowCraft_InitializePacking` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1275 | 0x004801B0 | `sub_4801B0` | 有正文 | `src/ui/windows/WindowCraft.cpp:672 — WindowCraft_WaitPacking` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1276 | 0x00480600 | `sub_480600` | 有正文 | `src/ui/windows/WindowCraft.cpp:709 — WindowCraft_WaitDecoration` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1277 | 0x00480A00 | `sub_480A00` | 有正文 | `src/ui/windows/WindowCraft.cpp:935 — WindowCraft_InitializeRefining` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1278 | 0x00480C20 | `sub_480C20` | 有正文 | `src/ui/windows/WindowCraft.cpp:744 — WindowCraft_WaitRefining` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1279 | 0x00481010 | `sub_481010` | 有正文 | `src/ui/windows/WindowCraft.cpp:2984 — WindowCraft_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1280 | 0x004810A0 | `sub_4810A0` | 有正文 | `src/ui/windows/WindowCraft.cpp:2669 — WindowCraft_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1281 | 0x00482000 | `sub_482000` | 有正文 | `src/ui/windows/WindowCraft.cpp:3008 — WindowCraft_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1282 | 0x00482270 | `sub_482270` | 有正文 | `src/ui/windows/WindowCraft.cpp:1544 — WindowCraft_ButtonExecute` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1283 | 0x00482F70 | `sub_482F70` | 有正文 | `src/ui/windows/WindowCraft.cpp:1532 — WindowCraft_ClassifyWork` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1284 | 0x00482FA0 | `sub_482FA0` | 有正文 | `src/ui/windows/WindowCraft.cpp:1844 — WindowCraft_ButtonContinue` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1285 | 0x004830B0 | `sub_4830B0` | 有正文 | `src/ui/windows/WindowCraft.cpp:1891 — WindowCraft_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1286 | 0x00483150 | `sub_483150` | 有正文 | `src/ui/windows/WindowCraft.cpp:1913 — WindowCraft_HandleInventory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1287 | 0x00483E30 | `sub_483E30` | 有正文 | `src/ui/windows/WindowCraft.cpp:1335 — WindowCraft_ReleaseMaterialSlot` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1288 | 0x00483EB0 | `sub_483EB0` | 有正文 | `src/ui/windows/WindowCraft.cpp:954 — WindowCraft_HandleMaterialSlot` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1289 | 0x004847C0 | `sub_4847C0` | 有正文 | `src/ui/windows/WindowCraft.cpp:2388 — WindowCraft_HandleSingleMaterial` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1290 | 0x00484D60 | `sub_484D60` | 有正文 | `src/ui/windows/WindowCraft.cpp:3099 — WindowCraft_HandleMaterialTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1291 | 0x00484E90 | `sub_484E90` | 有正文 | `src/ui/windows/WindowCraft.cpp:3309 — WindowCraft_HandleItemTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1292 | 0x00484FB0 | `sub_484FB0` | 有正文 | `src/ui/windows/WindowCraft.cpp:2273 — WindowCraft_DrawResultImage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1293 | 0x00485110 | `sub_485110` | 有正文 | `src/ui/WindowRuntime.cpp:3718 — WindowElement_DrawHoverFrame` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1294 | 0x00485160 | `sub_485160` | 有正文 | `src/ui/windows/WindowCraft.cpp:2317 — WindowCraft_ButtonCloseIcon` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1295 | 0x00485200 | `sub_485200` | 有正文 | `src/ui/windows/WindowCraft.cpp:2372 — WindowCraft_DrawInvalidEngravingNotice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1296 | 0x00485350 | `sub_485350` | 有正文 | `src/ui/windows/WindowCraft.cpp:2380 — WindowCraft_DrawEmptyEngravingNotice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1297 | 0x004854A0 | `sub_4854A0` | 有正文 | `src/ui/windows/WindowGather.cpp:122 — WindowGather_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1298 | 0x00485700 | `sub_485700` | 有正文 | `src/ui/windows/WindowGather.cpp:218 — WindowGather_Cancel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1299 | 0x00485890 | `sub_485890` | 有正文 | `src/ui/windows/WindowGather.cpp:246 — WindowGather_InitializeAction` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1300 | 0x00485AF0 | `sub_485AF0` | 有正文 | `src/ui/windows/WindowGather.cpp:300 — WindowGather_ShowResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1301 | 0x00486020 | `sub_486020` | 有正文 | `src/ui/windows/WindowGather.cpp:375 — WindowGather_RefreshResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1302 | 0x00486040 | `sub_486040` | 有正文 | `src/ui/windows/WindowGather.cpp:439 — WindowGather_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1303 | 0x00486CF0 | `sub_486CF0` | 有正文 | `src/ui/windows/WindowGather.cpp:663 — WindowGather_ButtonContinue` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1304 | 0x00486E50 | `sub_486E50` | 有正文 | `src/ui/windows/WindowGather.cpp:702 — WindowGather_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1305 | 0x00486F20 | `sub_486F20` | 有正文 | `src/ui/windows/WindowGather.cpp:732 — WindowGather_DrawResultImage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1306 | 0x00486FE0 | `sub_486FE0` | 有正文 | `src/ui/windows/WindowSkill.cpp:475 — WindowSkill_MatchesCrystalLevel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1307 | 0x00487020 | `sub_487020` | 有正文 | `src/ui/windows/WindowSkill.cpp:2393 — WindowSkill_SelectTextTarget` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1308 | 0x004871B0 | `sub_4871B0` | 有正文 | `src/ui/windows/WindowSkill.cpp:2443 — WindowSkill_OpenTextTargets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1309 | 0x004872E0 | `sub_4872E0` | 有正文 | `src/ui/windows/WindowSkill.cpp:484 — WindowSkill_SelectAttributeTarget` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1310 | 0x004874B0 | `sub_4874B0` | 有正文 | `src/ui/windows/WindowSkill.cpp:532 — WindowSkill_OpenAttributeTargets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1311 | 0x004875E0 | `sub_4875E0` | 有正文 | `src/ui/windows/WindowSkillResult.cpp:119 — WindowSkillResult_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1312 | 0x00487660 | `sub_487660` | 有正文 | `src/ui/windows/WindowSkillResult.cpp:140 — WindowSkillResult_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1313 | 0x00487AE0 | `sub_487AE0` | 有正文 | `src/ui/windows/WindowSkillResult.cpp:223 — WindowSkillResult_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1314 | 0x00487B80 | `sub_487B80` | 有正文 | `src/ui/windows/WindowSkillResult.cpp:243 — WindowSkillResult_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1315 | 0x00487D60 | `sub_487D60` | 有正文 | `src/ui/windows/WindowSkillResult.cpp:273 — WindowSkillResult_ButtonContinue` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1316 | 0x00487E80 | `sub_487E80` | 有正文 | `src/ui/windows/WindowSkillResult.cpp:309 — WindowSkillResult_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1317 | 0x00487F30 | `sub_487F30` | 有正文 | `src/ui/windows/WindowSkillResult.cpp:95 — WindowSkillResult_ReadPreviewImage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1318 | 0x004880D0 | `sub_4880D0` | 有正文 | `src/ui/windows/WindowCraft.cpp:1351 — WindowCraft_ReadRequiredSkillLevel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1319 | 0x00488160 | `sub_488160` | 有正文 | `src/ui/windows/WindowCraft.cpp:1371 — WindowCraft_ReadMaxMaterialLevel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1320 | 0x00488180 | `sub_488180` | 有正文 | `src/ui/windows/WindowCraft.cpp:1384 — WindowCraft_ReadFirstMaterialId` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1321 | 0x004881D0 | `sub_4881D0` | 有正文 | `src/ui/windows/WindowCraft.cpp:1397 — WindowCraft_ValidatePackingMaterials` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1322 | 0x004883F0 | `sub_4883F0` | 有承接 | `src/ui/windows/WindowInventory.cpp:1123 — WindowInventory_ConstructSelector` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 1323 | 0x00488400 | `unknown_libname_44` | 有正文 | `src/ui/windows/WindowInventory.cpp:1123 — WindowInventory_ConstructSelector` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1324 | 0x00488420 | `sub_488420` | 有承接 | `src/Yobi/preview.cpp:35092 — ConstructWindowSelectorPreview` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 1325 | 0x00488510 | `sub_488510` | 有正文 | `src/ui/windows/WindowInventory.cpp:335 — WindowInventory_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1326 | 0x004885A0 | `sub_4885A0` | 有正文 | `src/ui/WindowRuntime.cpp:936 — WindowItem_RequestTransferQuantity` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1327 | 0x004886E0 | `sub_4886E0` | 有正文 | `src/ui/WindowRuntime.cpp:971 — WindowItem_RequestTransfer` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1328 | 0x00488800 | `sub_488800` | 有正文 | `src/ui/windows/WindowInventory.cpp:134 — WindowItem_TransferQuantity` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1329 | 0x004888C0 | `sub_4888C0` | 有正文 | `src/ui/windows/WindowInventory.cpp:257 — WindowInventory_OpenSpecialSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1330 | 0x00488930 | `sub_488930` | 有正文 | `src/ui/windows/WindowInventory.cpp:272 — WindowInventory_TogglePreview` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1331 | 0x00488950 | `sub_488950` | 有正文 | `src/ui/windows/WindowInventory.cpp:278 — WindowInventory_CycleEquipment` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1332 | 0x00488970 | `sub_488970` | 有正文 | `src/ui/windows/WindowInventory.cpp:284 — WindowInventory_RotatePreview` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1333 | 0x00488990 | `sub_488990` | 有正文 | `src/ui/windows/WindowInventory.cpp:359 — WindowInventory_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1334 | 0x00488B10 | `sub_488B10` | 有正文 | `src/ui/windows/WindowInventory.cpp:414 — WindowInventory_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1335 | 0x00488C50 | `sub_488C50` | 有正文 | `src/ui/windows/WindowInventory.cpp:456 — WindowInventory_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1336 | 0x00488CF0 | `sub_488CF0` | 有正文 | `src/ui/windows/WindowInventory.cpp:476 — WindowInventory_DropMoney` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1337 | 0x00488D30 | `sub_488D30` | 有正文 | `src/ui/windows/WindowInventory.cpp:488 — WindowInventory_HandleMoney` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1338 | 0x00488E60 | `sub_488E60` | 有正文 | `src/ui/windows/WindowInventory.cpp:524 — WindowInventory_RefreshMoney` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1339 | 0x00488E80 | `sub_488E80` | 有正文 | `src/ui/windows/WindowInventory.cpp:531 — WindowInventory_RefreshExtraValue` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1340 | 0x00488EA0 | `sub_488EA0` | 有正文 | `src/ui/windows/WindowInventory.cpp:291 — WindowInventory_TabEquipment` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1341 | 0x00488EC0 | `sub_488EC0` | 有正文 | `src/ui/windows/WindowInventory.cpp:298 — WindowInventory_TabPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1342 | 0x00488FB0 | `sub_488FB0` | 有正文 | `src/ui/windows/WindowInventory.cpp:538 — WindowInventory_HandleItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1343 | 0x00489760 | `sub_489760` | 有正文 | `src/ui/windows/WindowInventory.cpp:735 — WindowInventory_HandleEquipment` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1344 | 0x00489EE0 | `sub_489EE0` | 有正文 | `src/ui/windows/WindowInventory.cpp:958 — WindowInventory_TabPlayerAttributes` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1345 | 0x00489F00 | `sub_489F00` | 有正文 | `src/ui/windows/WindowInventory.cpp:965 — WindowInventory_HandleItemSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1346 | 0x0048A6C0 | `sub_48A6C0` | 有正文 | `src/ui/windows/WindowInventory.cpp:212 — WindowInventory_DrawBlockedEquipment` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1347 | 0x0048A830 | `sub_48A830` | 有正文 | `src/ui/windows/WindowQuantity.cpp:81 — WindowQuantity_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1348 | 0x0048A8C0 | `sub_48A8C0` | 有正文 | `src/ui/windows/WindowQuantity.cpp:106 — WindowQuantity_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1349 | 0x0048A900 | `sub_48A900` | 有正文 | `src/ui/windows/WindowQuantity.cpp:121 — WindowQuantity_HandleInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1350 | 0x0048A950 | `sub_48A950` | 有正文 | `src/ui/windows/WindowQuantity.cpp:135 — WindowQuantity_ButtonOk` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1351 | 0x0048AA60 | `sub_48AA60` | 有正文 | `src/ui/windows/WindowQuantity.cpp:178 — WindowQuantity_ButtonIncrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1352 | 0x0048AB10 | `sub_48AB10` | 有正文 | `src/ui/windows/WindowQuantity.cpp:207 — WindowQuantity_ButtonDecrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1353 | 0x0048ABC0 | `sub_48ABC0` | 有正文 | `src/Yobi/window.cpp:1976 — Sub_48ABC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1354 | 0x0048AC60 | `sub_48AC60` | 有正文 | `src/Yobi/window.cpp:2013 — Sub_48AC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1355 | 0x0048AEA0 | `sub_48AEA0` | 有正文 | `src/ui/windows/WindowItemUse.cpp:87 — WindowItemUse_SelectTextTarget` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1356 | 0x0048AF00 | `sub_48AF00` | 有正文 | `src/ui/windows/WindowItemUse.cpp:108 — WindowItemUse_OpenTextTargets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1357 | 0x0048B010 | `sub_48B010` | 有正文 | `src/ui/windows/WindowItemUse.cpp:193 — WindowItemUse_SelectAttributeTarget` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1358 | 0x0048B0A0 | `sub_48B0A0` | 有正文 | `src/ui/windows/WindowItemUse.cpp:220 — WindowItemUse_OpenAttributeTargets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1359 | 0x0048B1A0 | `sub_48B1A0` | 有正文 | `src/ui/WindowRuntime.cpp:2516 — WindowItem_DrawEquipmentTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1360 | 0x0048B350 | `sub_48B350` | 有正文 | `src/ui/WindowRuntime.cpp:2558 — WindowItem_DrawDescriptionTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1361 | 0x0048B450 | `sub_48B450` | 有正文 | `src/ui/windows/WindowTrade.cpp:462 — WindowItem_DrawPetTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1362 | 0x0048B560 | `sub_48B560` | 有正文 | `src/ui/windows/WindowShop.cpp:446 — WindowItem_DrawShopTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1363 | 0x0048B750 | `sub_48B750` | 有正文 | `src/ui/WindowRuntime.cpp:2587 — WindowItem_DrawWorkTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1364 | 0x0048B810 | `sub_48B810` | 有正文 | `src/ui/WindowRuntime.cpp:1561 — WindowItem_DrawTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1365 | 0x0048B930 | `sub_48B930` | 有正文 | `src/ui/windows/WindowInventory.cpp:165 — WindowItem_DrawDescriptionLines` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1366 | 0x0048BA60 | `sub_48BA60` | 有正文 | `src/ui/WindowRuntime.cpp:841 — WindowItemDrag_RemapHover` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1367 | 0x0048BB30 | `sub_48BB30` | 有正文 | `src/ui/WindowRuntime.cpp:896 — WindowItem_ClassifyItemSlot` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1368 | 0x0048BB70 | `sub_48BB70` | 有正文 | `src/ui/WindowRuntime.cpp:908 — WindowItem_ClassifyPetSlot` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1369 | 0x0048BB90 | `sub_48BB90` | 有正文 | `src/ui/WindowRuntime.cpp:1012 — WindowItem_RequestPetTransfer` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1370 | 0x0048BC60 | `sub_48BC60` | 有正文 | `src/ui/WindowRuntime.cpp:914 — WindowItem_FindSortedPetIndex` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1371 | 0x0048BC90 | `sub_48BC90` | 有正文 | `src/ui/WindowRuntime.cpp:1211 — WindowItemDrag_Handle` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1372 | 0x0048BF10 | `sub_48BF10` | 有正文 | `src/ui/WindowRuntime.cpp:1301 — WindowItemDrag_Start` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1373 | 0x0048BF30 | `sub_48BF30` | 有正文 | `src/ui/WindowRuntime.cpp:1122 — WindowPetDrag_Handle` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1374 | 0x0048C130 | `sub_48C130` | 有正文 | `src/ui/WindowRuntime.cpp:1203 — WindowPetDrag_Start` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1375 | 0x0048C150 | `sub_48C150` | 有正文 | `src/ui/windows/WindowPetList.cpp:328 — WindowPetList_ResetSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1376 | 0x0048C1B0 | `sub_48C1B0` | 有正文 | `src/ui/windows/WindowPetList.cpp:761 — WindowPetList_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1377 | 0x0048C1CF | `sub_48C1CF` | 有承接 | `src/ui/windows/WindowPetList.cpp:761 — WindowPetList_Update` | 原共享尾为调用 48C920 后返回 1；并入实际父正文；完整业务／运行未验 |
| 1378 | 0x0048C1E0 | `sub_48C1E0` | 有正文 | `src/ui/windows/WindowPetList.cpp:374 — WindowPetList_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1379 | 0x0048C920 | `sub_48C920` | 有正文 | `src/ui/windows/WindowPetList.cpp:648 — WindowPetList_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1380 | 0x0048D460 | `sub_48D460` | 有正文 | `src/ui/windows/WindowPetList.cpp:589 — WindowPetList_RefreshStates` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1381 | 0x0048D4E0 | `sub_48D4E0` | 有正文 | `src/ui/windows/WindowPetList.cpp:359 — WindowPetList_SubmitStates` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1382 | 0x0048D540 | `sub_48D540` | 有正文 | `src/ui/WindowRuntime.cpp:1044 — WindowPet_DrawTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1383 | 0x0048D9A0 | `sub_48D9A0` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:355 — WindowPetDetail_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1384 | 0x0048DAB0 | `sub_48DAB0` | 有正文 | `src/ui/windows/WindowPetAttributes.cpp:199 — WindowPetAttributes_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1385 | 0x0048DBB0 | `sub_48DBB0` | 有正文 | `src/ui/windows/WindowPetSkills.cpp:240 — WindowPetSkills_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1386 | 0x0048DCB0 | `sub_48DCB0` | 有正文 | `src/ui/windows/WindowPetSkills.cpp:273 — WindowPetSkills_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1387 | 0x0048DCC0 | `sub_48DCC0` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:391 — WindowPetDetail_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1388 | 0x0048DD00 | `sub_48DD00` | 有正文 | `src/ui/windows/WindowPetAttributes.cpp:232 — WindowPetAttributes_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1389 | 0x0048DD40 | `sub_48DD40` | 有正文 | `src/ui/windows/WindowPetSkills.cpp:281 — WindowPetSkills_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1390 | 0x0048DD80 | `sub_48DD80` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:403 — WindowPetDetail_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1391 | 0x0048E430 | `sub_48E430` | 有正文 | `src/ui/windows/WindowPetAttributes.cpp:244 — WindowPetAttributes_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1392 | 0x0048ECA0 | `sub_48ECA0` | 有正文 | `src/ui/windows/WindowPetSkills.cpp:293 — WindowPetSkills_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1393 | 0x0048F380 | `sub_48F380` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:542 — WindowPetDetail_EditName` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1394 | 0x0048F410 | `sub_48F410` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:560 — WindowPetDetail_SwitchPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1395 | 0x0048F620 | `sub_48F620` | 有正文 | `src/ui/windows/WindowPetSkills.cpp:434 — WindowPetSkills_HandleDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1396 | 0x0048F690 | `sub_48F690` | 有正文 | `src/ui/windows/WindowPetSkills.cpp:456 — WindowPetSkills_StartDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1397 | 0x0048F6C0 | `sub_48F6C0` | 有正文 | `src/ui/windows/WindowPetSkills.cpp:464 — WindowPetSkills_HandleSkillRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1398 | 0x0048F8F0 | `sub_48F8F0` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:197 — WindowPetDetail_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1399 | 0x00490780 | `sub_490780` | 有正文 | `src/ui/windows/WindowPetAttributes.cpp:79 — WindowPetAttributes_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1400 | 0x00491AA0 | `sub_491AA0` | 有正文 | `src/ui/windows/WindowPetSkills.cpp:117 — WindowPetSkills_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1401 | 0x004920F0 | `sub_4920F0` | 有正文 | `src/ui/windows/WindowPetSkills.cpp:203 — WindowPetSkills_DrawDraggedSkillTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1402 | 0x00492250 | `sub_492250` | 有正文 | `src/world/SceneState.cpp:3812 — Sub_492250` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1403 | 0x00492260 | `sub_492260` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:144 — WindowPetDetail_ResetDraftFlag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1404 | 0x00492270 | `sub_492270` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:150 — WindowPetDetail_ReadDraftFlag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1405 | 0x00492280 | `sub_492280` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:156 — WindowPetDetail_InitializeDraft` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1406 | 0x00492350 | `sub_492350` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:171 — WindowPetDetail_SavePetSequences` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1407 | 0x00492390 | `sub_492390` | 有正文 | `src/ui/windows/WindowPetDetail.cpp:183 — WindowPetDetail_HasPetSequenceChanges` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1408 | 0x004923D0 | `sub_4923D0` | 有正文 | `src/ui/WindowRuntime.cpp:3411 — WindowRuntime_ResetAlbumState` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1409 | 0x004923F0 | `sub_4923F0` | 有正文 | `src/ui/windows/WindowAlbumList.cpp:198 — WindowAlbumList_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1410 | 0x00492410 | `sub_492410` | 有正文 | `src/ui/windows/WindowAlbumDetail.cpp:314 — WindowAlbumDetail_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1411 | 0x00492430 | `sub_492430` | 有正文 | `src/ui/windows/WindowAlbumList.cpp:208 — WindowAlbumList_SelectEntry` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1412 | 0x00492520 | `sub_492520` | 有正文 | `src/ui/windows/WindowAlbumList.cpp:236 — WindowAlbumList_HandleScrollButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1413 | 0x00492840 | `sub_492840` | 有正文 | `src/ui/windows/WindowAlbumList.cpp:319 — WindowAlbumList_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1414 | 0x00492880 | `sub_492880` | 有正文 | `src/ui/windows/WindowAlbumDetail.cpp:324 — WindowAlbumDetail_HandleNavigation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1415 | 0x00492C80 | `sub_492C80` | 有正文 | `src/ui/windows/WindowAlbumList.cpp:134 — WindowAlbumList_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1416 | 0x00492F70 | `sub_492F70` | 有正文 | `src/ui/windows/WindowAlbumDetail.cpp:179 — WindowAlbumDetail_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1417 | 0x004939A0 | `sub_4939A0` | 有正文 | `src/ui/windows/WindowAlbumDetail.cpp:156 — WindowAlbumDetail_DrawRating` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1418 | 0x00493A70 | `sub_493A70` | 有正文 | `src/ui/windows/WindowSettings.cpp:218 — WindowSettings_RefreshLogoutPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1419 | 0x00493AF0 | `sub_493AF0` | 有正文 | `src/ui/windows/WindowSettings.cpp:237 — WindowSettings_RefreshInputPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1420 | 0x00493B70 | `sub_493B70` | 有正文 | `src/ui/windows/WindowSettings.cpp:257 — WindowSettings_RefreshAudioPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1421 | 0x00493CB0 | `sub_493CB0` | 有正文 | `src/ui/windows/WindowSettings.cpp:291 — WindowSettings_RefreshDisplayPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1422 | 0x00493D60 | `sub_493D60` | 有正文 | `src/ui/windows/WindowSettings.cpp:316 — WindowSettings_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1423 | 0x00493DB0 | `sub_493DB0` | 有正文 | `src/ui/windows/WindowSettings.cpp:330 — WindowSettings_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1424 | 0x00493DF9 | `sub_493DF9` | 有承接 | `src/ui/windows/WindowSettings.cpp:330 — WindowSettings_Draw` | 原绘图／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 1425 | 0x00493E10 | `sub_493E10` | 有正文 | `src/ui/windows/WindowSettings.cpp:352 — WindowSettings_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1426 | 0x00493E50 | `sub_493E50` | 有正文 | `src/ui/windows/WindowSettings.cpp:527 — WindowSettings_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1427 | 0x00493EE0 | `sub_493EE0` | 有正文 | `src/ui/windows/WindowSettings.cpp:365 — WindowSettings_ButtonLogoutPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1428 | 0x00494000 | `sub_494000` | 有正文 | `src/ui/windows/WindowSettings.cpp:398 — WindowSettings_ButtonInputPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1429 | 0x00494180 | `sub_494180` | 有正文 | `src/ui/windows/WindowSettings.cpp:443 — WindowSettings_ButtonAudioPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1430 | 0x00494370 | `sub_494370` | 有正文 | `src/ui/windows/WindowSettings.cpp:489 — WindowSettings_ButtonDisplayPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1431 | 0x004944A0 | `sub_4944A0` | 有正文 | `src/ui/windows/WindowSettings.cpp:550 — WindowSettings_ButtonHotkeys` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1432 | 0x00494550 | `sub_494550` | 有正文 | `src/ui/windows/WindowSettings.cpp:576 — WindowSettings_ButtonLogout` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1433 | 0x00494620 | `sub_494620` | 有正文 | `src/ui/windows/WindowSettings.cpp:606 — WindowSettings_ButtonServerSelect` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1434 | 0x00494720 | `sub_494720` | 有正文 | `src/ui/windows/WindowSettings.cpp:638 — WindowSettings_ToggleItemStacking` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1435 | 0x004947F0 | `sub_4947F0` | 有正文 | `src/ui/windows/WindowSettings.cpp:667 — WindowSettings_ToggleCursor` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1436 | 0x004948F0 | `sub_4948F0` | 有正文 | `src/ui/windows/WindowSettings.cpp:699 — WindowSettings_CycleHotkeyMode` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1437 | 0x004949C0 | `sub_4949C0` | 有正文 | `src/ui/windows/WindowSettings.cpp:725 — WindowSettings_ButtonMusicDecrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1438 | 0x00494AE0 | `sub_494AE0` | 有正文 | `src/ui/windows/WindowSettings.cpp:762 — WindowSettings_ButtonMusicIncrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1439 | 0x00494C10 | `sub_494C10` | 有正文 | `src/ui/windows/WindowSettings.cpp:799 — WindowSettings_ButtonSoundDecrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1440 | 0x00494D30 | `sub_494D30` | 有正文 | `src/ui/windows/WindowSettings.cpp:836 — WindowSettings_ButtonSoundIncrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1441 | 0x00494E60 | `sub_494E60` | 有正文 | `src/ui/windows/WindowSettings.cpp:873 — WindowSettings_ToggleChannels` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1442 | 0x00494F30 | `sub_494F30` | 有正文 | `src/ui/windows/WindowSettings.cpp:903 — WindowSettings_ToggleAudioFlag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1443 | 0x00495000 | `sub_495000` | 有正文 | `src/ui/windows/WindowSettings.cpp:933 — WindowSettings_ToggleScanlines` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1444 | 0x00495130 | `sub_495130` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:296 — WindowHotkeys_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1445 | 0x004951C0 | `sub_4951C0` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:316 — WindowHotkeys_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1446 | 0x004953B0 | `sub_4953B0` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:396 — WindowHotkeys_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1447 | 0x00495450 | `sub_495450` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:415 — WindowHotkeys_ButtonPreviousRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1448 | 0x00495500 | `sub_495500` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:447 — WindowHotkeys_ButtonNextRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1449 | 0x004955C0 | `sub_4955C0` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:482 — WindowHotkeys_ButtonPreviousPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1450 | 0x00495670 | `sub_495670` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:517 — WindowHotkeys_ButtonNextPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1451 | 0x004957B0 | `sub_4957B0` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:573 — WindowHotkeys_TabFirstCategory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1452 | 0x004958D0 | `sub_4958D0` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:614 — WindowHotkeys_TabSecondCategory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1453 | 0x004959F0 | `sub_4959F0` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:655 — WindowHotkeys_TabThirdCategory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1454 | 0x00495B10 | `sub_495B10` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:696 — WindowHotkeys_TabPresetText` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1455 | 0x00495C20 | `sub_495C20` | 有正文 | `src/ui/windows/WindowHotkeys.cpp:736 — WindowHotkeys_EditPresetText` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1456 | 0x00495D70 | `sub_495D70` | 有正文 | `src/ui/windows/WindowMiniMap.cpp:56 — WindowMiniMap_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1457 | 0x00495F00 | `sub_495F00` | 有正文 | `src/ui/windows/WindowMiniMap.cpp:125 — WindowMiniMap_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1458 | 0x00495F50 | `sub_495F50` | 有正文 | `src/ui/windows/WindowEmote.cpp:124 — WindowEmote_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1459 | 0x00495F70 | `sub_495F70` | 有正文 | `src/ui/windows/WindowEmote.cpp:133 — WindowEmote_Select` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1460 | 0x00495FC0 | `sub_495FC0` | 有正文 | `src/ui/windows/WindowEmote.cpp:145 — WindowEmote_ResetSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1461 | 0x00495FE0 | `sub_495FE0` | 有正文 | `src/ui/windows/WindowEmote.cpp:152 — WindowEmote_ButtonEmote` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1462 | 0x00496570 | `sub_496570` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:1694 — WindowBattleCommand_SnapshotPetVitals` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1463 | 0x004965D0 | `sub_4965D0` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:1709 — WindowBattleCommand_SyncActorVitals` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1464 | 0x004966B0 | `BattleScene::SetCommandType` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:143 — WindowBattleCommand_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1465 | 0x00496AE0 | `BattleScene::closeAllWin` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:376 — WindowBattleCommand_Close` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1466 | 0x00496B90 | `sub_496B90` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:242 — WindowBattleCommand_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1467 | 0x00496D50 | `sub_496D50` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:313 — WindowBattleCommand_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1468 | 0x00496F10 | `BattleScene::CloseAllSubWindow` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:418 — WindowBattleCommand_ResetSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1469 | 0x00496FB0 | `sub_496FB0` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:772 — WindowBattleCommand_CanSelectSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1470 | 0x00497000 | `sub_497000` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:875 — WindowBattleCommand_OpenSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1471 | 0x004970B0 | `sub_4970B0` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:789 — WindowBattleCommand_CanSelectItem` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1472 | 0x00497100 | `sub_497100` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:806 — WindowBattleCommand_OpenItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1473 | 0x004971C0 | `sub_4971C0` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:833 — WindowBattleCommand_CanSelectPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1474 | 0x00497210 | `sub_497210` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:850 — WindowBattleCommand_OpenPets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1475 | 0x004972B0 | `sub_4972B0` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:451 — WindowBattleCommand_ButtonAttack` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1476 | 0x004973C0 | `sub_4973C0` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:489 — WindowBattleCommand_ButtonDefend` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1477 | 0x004974C0 | `sub_4974C0` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:902 — WindowBattleCommand_SelectSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1478 | 0x004975A0 | `sub_4975A0` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:934 — WindowBattleCommand_ButtonSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1479 | 0x00497770 | `sub_497770` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:526 — WindowBattleCommand_ButtonItem` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1480 | 0x00497900 | `sub_497900` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:580 — WindowBattleCommand_ButtonPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1481 | 0x00497A60 | `sub_497A60` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:633 — WindowBattleCommand_ButtonFormation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1482 | 0x00497B80 | `sub_497B80` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:674 — WindowBattleCommand_ButtonEscape` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1483 | 0x00497C80 | `sub_497C80` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:712 — WindowBattleCommand_ButtonRebirth` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1484 | 0x00497E30 | `sub_497E30` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:998 — WindowBattleCommand_SelectPetSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1485 | 0x00497FF0 | `sub_497FF0` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:1050 — WindowBattleCommand_SelectRidingSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1486 | 0x00498160 | `sub_498160` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:1099 — WindowBattleCommand_ButtonPetSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1487 | 0x004982C0 | `BattleScene::DrawCountDown` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:1616 — WindowBattleCommand_DrawTimer` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1488 | 0x00498520 | `sub_498520` | 有正文 | `src/ui/windows/WindowBattleCommand.cpp:1179 — WindowBattleCommand_Dispatch` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1489 | 0x004990A0 | `sub_4990A0` | 有正文 | `src/Yobi/battle_scene.cpp:22675 — Sub_4990A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1490 | 0x004990F0 | `sub_4990F0` | 有正文 | `src/ui/windows/WindowSpectator.cpp:26 — WindowSpectator_ButtonEnd` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1491 | 0x004991C0 | `sub_4991C0` | 有正文 | `src/ui/windows/WindowBattleResult.cpp:169 — WindowBattleResult_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1492 | 0x004996C0 | `sub_4996C0` | 有正文 | `src/ui/windows/WindowBattleResult.cpp:286 — WindowBattleResult_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1493 | 0x00499750 | `sub_499750` | 有正文 | `src/ui/windows/WindowBattleResult.cpp:309 — WindowBattleResult_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1494 | 0x00499790 | `sub_499790` | 有正文 | `src/ui/windows/WindowBattleResult.cpp:321 — WindowDpResult_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1495 | 0x00499830 | `sub_499830` | 有正文 | `src/ui/WindowRuntime.cpp:3600 — WindowRuntime_DrawLargeFrame` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1496 | 0x00499880 | `sub_499880` | 有正文 | `src/ui/windows/WindowBattleResult.cpp:149 — WindowRuntime_ButtonCloseResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1497 | 0x004998F0 | `sub_4998F0` | 有承接 | `src/ui/windows/WindowBattleItems.cpp:946 — WindowBattleItems_ConstructSelector` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 1498 | 0x00499900 | `unknown_libname_45` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:946 — WindowBattleItems_ConstructSelector` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1499 | 0x00499920 | `unknown_libname_46` | 有承接 | `src/Yobi/preview.cpp:35092 — ConstructWindowSelectorPreview` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 1500 | 0x00499930 | `sub_499930` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:110 — WindowBattleItems_TogglePreview` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1501 | 0x00499950 | `sub_499950` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:116 — WindowBattleItems_CycleEquipment` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1502 | 0x00499970 | `sub_499970` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:122 — WindowBattleItems_RotatePreview` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1503 | 0x00499990 | `sub_499990` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:189 — WindowBattleItems_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1504 | 0x004999D0 | `sub_4999D0` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:202 — WindowBattleItems_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1505 | 0x00499B00 | `sub_499B00` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:129 — WindowBattleItems_RefreshMoney` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1506 | 0x00499B20 | `sub_499B20` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:136 — WindowBattleItems_RefreshExtraValue` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1507 | 0x00499B40 | `sub_499B40` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:143 — WindowBattleItems_TabEquipment` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1508 | 0x00499B60 | `sub_499B60` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:151 — WindowBattleItems_TabPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1509 | 0x00499C30 | `sub_499C30` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:244 — WindowBattleItems_HandleInventory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1510 | 0x0049AC40 | `sub_49AC40` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:683 — WindowBattleItems_HandleEquipment` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1511 | 0x0049B610 | `sub_49B610` | 有正文 | `src/ui/windows/WindowBattleItems.cpp:676 — WindowBattleItems_ShowExtraAttributes` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1512 | 0x0049B630 | `sub_49B630` | 有正文 | `src/ui/windows/WindowBattlePets.cpp:50 — WindowBattlePets_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1513 | 0x0049B670 | `sub_49B670` | 有正文 | `src/ui/windows/WindowBattlePets.cpp:89 — WindowBattlePets_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1514 | 0x0049BA10 | `sub_49BA10` | 有正文 | `src/ui/windows/WindowBattlePets.cpp:60 — WindowBattlePets_ButtonReturn` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1515 | 0x0049BB00 | `sub_49BB00` | 有正文 | `src/ui/windows/WindowBattlePets.cpp:160 — WindowBattlePets_SelectRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1516 | 0x0049BD40 | `sub_49BD40` | 有正文 | `src/ui/windows/WindowBattleStateNotice.cpp:39 — WindowBattleStateNotice_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1517 | 0x0049BDB0 | `sub_49BDB0` | 有正文 | `src/ui/WindowRuntime.cpp:3547 — WindowRuntime_DrawCompactFrame` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1518 | 0x0049BE00 | `sub_49BE00` | 有正文 | `src/Yobi/window.cpp:1558 — DrawNineGrid_49BE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1519 | 0x0049C4D0 | `sub_49C4D0` | 有正文 | `src/ui/windows/WindowDialog.cpp:211 — WindowDialog_Reset` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1520 | 0x0049C510 | `sub_49C510` | 有正文 | `src/ui/windows/WindowDialog.cpp:226 — WindowDialog_AddImage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1521 | 0x0049C5C0 | `sub_49C5C0` | 有正文 | `src/Yobi/window.cpp:34471 — Sub_49C5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1522 | 0x0049C5E0 | `sub_49C5E0` | 有正文 | `src/Yobi/window.cpp:34477 — Sub_49C5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1523 | 0x0049C600 | `Window::CreateWithAnime` | 有正文 | `src/ui/windows/WindowDialog.cpp:259 — WindowDialog_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1524 | 0x0049C710 | `sub_49C710` | 有正文 | `src/ui/windows/WindowDialog.cpp:304 — WindowDialog_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1525 | 0x0049CAD0 | `sub_49CAD0` | 有正文 | `src/ui/windows/WindowDialog.cpp:399 — WindowDialog_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1526 | 0x0049CDF0 | `sub_49CDF0` | 有正文 | `src/ui/windows/WindowDialog.cpp:525 — WindowDialog_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1527 | 0x0049CE70 | `sub_49CE70` | 有正文 | `src/ui/windows/WindowDialog.cpp:541 — WindowDialog_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1528 | 0x0049CF70 | `sub_49CF70` | 有正文 | `src/ui/windows/WindowDialog.cpp:586 — WindowDialog_HandleTextSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1529 | 0x0049D2E0 | `World::OpenDialog` | 有正文 | `src/ui/windows/WindowDialog.cpp:822 — WindowDialog_Route` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1530 | 0x0049D8F0 | `sub_49D8F0` | 有正文 | `src/ui/windows/WindowProfile.cpp:170 — WindowProfile_RouteReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1531 | 0x0049DA30 | `sub_49DA30` | 有正文 | `src/ui/windows/WindowDialog.cpp:701 — WindowDialog_Reply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1532 | 0x0049DDB0 | `sub_49DDB0` | 有正文 | `src/ui/windows/WindowDialog.cpp:759 — WindowDialog_ParseGraphicTag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1533 | 0x0049DF30 | `sub_49DF30` | 有正文 | `src/ui/windows/WindowDialog.cpp:1055 — WindowDialog_OpenText` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1534 | 0x0049E1D0 | `sub_49E1D0` | 有正文 | `src/ui/windows/WindowDialog.cpp:1134 — WindowDialog_DrawHealing` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1535 | 0x0049E400 | `sub_49E400` | 有正文 | `src/ui/windows/WindowDialog.cpp:1190 — WindowDialog_OpenHealing` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1536 | 0x0049E750 | `sub_49E750` | 有正文 | `src/ui/windows/WindowTimerNotice.cpp:26 — WindowTimerNotice_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1537 | 0x0049E800 | `sub_49E800` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:104 — WindowTradeTarget_ReceiveTargets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1538 | 0x0049E8F0 | `sub_49E8F0` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:146 — WindowTradeTarget_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1539 | 0x0049E990 | `sub_49E990` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:137 — WindowTradeTarget_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1540 | 0x0049EAA0 | `sub_49EAA0` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:164 — WindowTradeTarget_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1541 | 0x0049EB50 | `sub_49EB50` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:190 — WindowTradeTarget_ButtonPreviousRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1542 | 0x0049EC00 | `sub_49EC00` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:219 — WindowTradeTarget_ButtonNextRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1543 | 0x0049ECC0 | `sub_49ECC0` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:247 — WindowTradeTarget_ButtonPreviousPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1544 | 0x0049ED80 | `sub_49ED80` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:274 — WindowTradeTarget_ButtonNextPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1545 | 0x0049EE60 | `sub_49EE60` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:308 — WindowTradeTarget_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1546 | 0x0049EEB0 | `sub_49EEB0` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:321 — WindowTradeTarget_ButtonCancel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1547 | 0x0049EF60 | `sub_49EF60` | 有正文 | `src/ui/windows/WindowTradeTarget.cpp:345 — WindowTradeTarget_SelectRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1548 | 0x0049F000 | `sub_49F000` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:107 — WindowTextTarget_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1549 | 0x0049F040 | `sub_49F040` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:119 — WindowTextTarget_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1550 | 0x0049F060 | `sub_49F060` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:128 — WindowTextTarget_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1551 | 0x0049F0A0 | `sub_49F0A0` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:141 — WindowTextTarget_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1552 | 0x0049F1A0 | `sub_49F1A0` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:161 — WindowTextTarget_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1553 | 0x0049F260 | `sub_49F260` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:186 — WindowTextTarget_ButtonPreviousRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1554 | 0x0049F310 | `sub_49F310` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:215 — WindowTextTarget_ButtonNextRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1555 | 0x0049F3D0 | `sub_49F3D0` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:243 — WindowTextTarget_ButtonPreviousPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1556 | 0x0049F490 | `sub_49F490` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:270 — WindowTextTarget_ButtonNextPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1557 | 0x0049F570 | `sub_49F570` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:304 — WindowTextTarget_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1558 | 0x0049F5C0 | `sub_49F5C0` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:317 — WindowTextTarget_ButtonCancel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1559 | 0x0049F680 | `sub_49F680` | 有正文 | `src/ui/windows/WindowTextTarget.cpp:348 — WindowTextTarget_SelectRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1560 | 0x0049F760 | `sub_49F760` | 有正文 | `src/ui/windows/WindowAttributeTarget.cpp:110 — WindowAttributeTarget_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1561 | 0x0049F790 | `sub_49F790` | 有正文 | `src/ui/windows/WindowAttributeTarget.cpp:121 — WindowAttributeTarget_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1562 | 0x0049F7B0 | `sub_49F7B0` | 有正文 | `src/ui/windows/WindowAttributeTarget.cpp:130 — WindowAttributeTarget_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1563 | 0x0049F7E0 | `sub_49F7E0` | 有正文 | `src/ui/windows/WindowAttributeTarget.cpp:140 — WindowAttributeTarget_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1564 | 0x0049FA40 | `sub_49FA40` | 有正文 | `src/ui/windows/WindowAttributeTarget.cpp:176 — WindowAttributeTarget_ButtonCancel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1565 | 0x0049FB00 | `sub_49FB00` | 有正文 | `src/ui/windows/WindowAttributeTarget.cpp:207 — WindowAttributeTarget_ButtonBack` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1566 | 0x0049FBC0 | `sub_49FBC0` | 有正文 | `src/ui/windows/WindowAttributeTarget.cpp:238 — WindowAttributeTarget_SelectRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1567 | 0x0049FCA0 | `sub_49FCA0` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:187 — WindowShopChoice_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1568 | 0x0049FD20 | `sub_49FD20` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:214 — WindowShopChoice_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1569 | 0x0049FE40 | `sub_49FE40` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:240 — WindowShopChoice_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1570 | 0x0049FF70 | `sub_49FF70` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:274 — WindowShopChoice_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1571 | 0x0049FFA0 | `sub_49FFA0` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:282 — WindowShopChoice_SelectRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1572 | 0x004A00C0 | `sub_4A00C0` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:315 — WindowShopChoice_SendBuySellChoice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1573 | 0x004A0140 | `sub_4A0140` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:334 — WindowShopChoice_OpenBuySell` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1574 | 0x004A0280 | `sub_4A0280` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:374 — WindowShopChoice_SendFirstCommandChoice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1575 | 0x004A0300 | `sub_4A0300` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:418 — WindowShopChoice_OpenFirstCommandList` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1576 | 0x004A0420 | `sub_4A0420` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:385 — WindowShopChoice_SendSecondCommandChoice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1577 | 0x004A04A0 | `sub_4A04A0` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:444 — WindowShopChoice_OpenSecondCommandList` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1578 | 0x004A05C0 | `sub_4A05C0` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:396 — WindowShopChoice_SendExchangeChoice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1579 | 0x004A0640 | `sub_4A0640` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:470 — WindowShopChoice_OpenSkillChoice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1580 | 0x004A0700 | `sub_4A0700` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:407 — WindowShopChoice_SendGiveReceiveChoice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1581 | 0x004A0790 | `sub_4A0790` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:484 — WindowShopChoice_OpenExchangeChoice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1582 | 0x004A0850 | `sub_4A0850` | 有正文 | `src/ui/windows/WindowShopChoice.cpp:498 — WindowShopChoice_OpenGiveReceiveChoice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1583 | 0x004A0940 | `sub_4A0940` | 有正文 | `src/ui/windows/WindowMail.cpp:318 — WindowMail_ResetSelectionState` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1584 | 0x004A0970 | `sub_4A0970` | 有正文 | `src/ui/windows/WindowContacts.cpp:289 — WindowContacts_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1585 | 0x004A09B0 | `sub_4A09B0` | 有正文 | `src/ui/windows/WindowContacts.cpp:304 — WindowContacts_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1586 | 0x004A09E0 | `sub_4A09E0` | 有正文 | `src/ui/windows/WindowContacts.cpp:314 — WindowContacts_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1587 | 0x004A1690 | `sub_4A1690` | 有正文 | `src/ui/windows/WindowContacts.cpp:549 — WindowContacts_SetDisplayMode` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1588 | 0x004A16B0 | `sub_4A16B0` | 有正文 | `src/ui/windows/WindowContacts.cpp:556 — WindowContacts_ApplyDisplayLayout` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1589 | 0x004A1780 | `sub_4A1780` | 有正文 | `src/ui/windows/WindowContacts.cpp:583 — WindowContacts_ApplyCategoryLayout` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1590 | 0x004A1C30 | `sub_4A1C30` | 有正文 | `src/ui/windows/WindowContacts.cpp:648 — WindowContacts_ScrollRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1591 | 0x004A1EF0 | `sub_4A1EF0` | 有正文 | `src/ui/windows/WindowContacts.cpp:693 — WindowContacts_HandleNavigation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1592 | 0x004A2610 | `sub_4A2610` | 有正文 | `src/ui/windows/WindowContacts.cpp:785 — WindowContacts_ButtonGroupMail` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1593 | 0x004A2700 | `sub_4A2700` | 有正文 | `src/ui/windows/WindowContacts.cpp:1265 — WindowContacts_ClampScroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1594 | 0x004A2870 | `sub_4A2870` | 有正文 | `src/ui/windows/WindowContacts.cpp:815 — WindowContacts_TabCategory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1595 | 0x004A2B00 | `sub_4A2B00` | 有正文 | `src/ui/windows/WindowContacts.cpp:968 — WindowContacts_OpenMail` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1596 | 0x004A2BD0 | `sub_4A2BD0` | 有正文 | `src/ui/windows/WindowContacts.cpp:857 — WindowContacts_ButtonGuildDetails` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1597 | 0x004A2C60 | `sub_4A2C60` | 有正文 | `src/ui/windows/WindowContacts.cpp:1332 — WindowContacts_Sort` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1598 | 0x004A2CC0 | `sub_4A2CC0` | 有正文 | `src/ui/windows/WindowContacts.cpp:1354 — WindowContacts_CompareRecords` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1599 | 0x004A3010 | `sub_4A3010` | 有正文 | `src/ui/windows/WindowContacts.cpp:1427 — WindowContacts_CompareContactName` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1600 | 0x004A3070 | `sub_4A3070` | 有正文 | `src/ui/windows/WindowContacts.cpp:1441 — WindowContacts_CompareGuildName` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1601 | 0x004A30D0 | `sub_4A30D0` | 有正文 | `src/ui/windows/WindowContacts.cpp:1455 — WindowContacts_CompareGuildValue` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1602 | 0x004A3100 | `sub_4A3100` | 有正文 | `src/ui/windows/WindowContacts.cpp:1471 — WindowContacts_CompareContactTitle` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1603 | 0x004A3160 | `sub_4A3160` | 有正文 | `src/ui/windows/WindowContacts.cpp:1485 — WindowContacts_CompareContactValue` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1604 | 0x004A3190 | `sub_4A3190` | 有正文 | `src/ui/windows/WindowContacts.cpp:1501 — WindowContacts_CompareGuildStatus` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1605 | 0x004A31C0 | `sub_4A31C0` | 有正文 | `src/ui/windows/WindowContacts.cpp:1517 — WindowContacts_CompareRecordKey` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1606 | 0x004A31E0 | `sub_4A31E0` | 有正文 | `src/ui/windows/WindowContacts.cpp:1526 — WindowContacts_CompareContactNumber` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1607 | 0x004A3230 | `sub_4A3230` | 有正文 | `src/ui/windows/WindowContacts.cpp:1544 — WindowContacts_CompareMiniMailNumber` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1608 | 0x004A3260 | `sub_4A3260` | 有正文 | `src/ui/windows/WindowContacts.cpp:1560 — WindowContacts_CompareGuildNumber` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1609 | 0x004A32B0 | `sub_4A32B0` | 有正文 | `src/ui/windows/WindowMail.cpp:305 — WindowMail_AdvanceUnreadBlink` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1610 | 0x004A32E0 | `sub_4A32E0` | 有正文 | `src/ui/windows/WindowContacts.cpp:1179 — WindowContacts_ButtonDelete` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1611 | 0x004A33C0 | `sub_4A33C0` | 有正文 | `src/ui/windows/WindowContacts.cpp:1217 — WindowContacts_ConfirmDeleteContact` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1612 | 0x004A3480 | `sub_4A3480` | 有正文 | `src/ui/windows/WindowContacts.cpp:1233 — WindowContacts_ConfirmDeleteGuildMember` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1613 | 0x004A3540 | `sub_4A3540` | 有正文 | `src/ui/windows/WindowContacts.cpp:1249 — WindowContacts_ConfirmDeleteMiniMail` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1614 | 0x004A3600 | `sub_4A3600` | 有正文 | `src/ui/windows/WindowMail.cpp:220 — WindowMail_DeleteContact` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1615 | 0x004A3660 | `sub_4A3660` | 有正文 | `src/ui/windows/WindowMail.cpp:236 — WindowMail_DeleteGuildMember` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1616 | 0x004A36B0 | `sub_4A36B0` | 有正文 | `src/ui/windows/WindowMail.cpp:249 — WindowMail_DeleteMiniMailContact` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1617 | 0x004A3760 | `sub_4A3760` | 有正文 | `src/ui/windows/WindowMail.cpp:264 — WindowMail_CancelDelete` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1618 | 0x004A3780 | `sub_4A3780` | 有正文 | `src/ui/windows/WindowContacts.cpp:1003 — WindowContacts_ResolveButtonRecord` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1619 | 0x004A37E0 | `sub_4A37E0` | 有正文 | `src/ui/windows/WindowContacts.cpp:878 — WindowContacts_ResolveRecordIndex` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1620 | 0x004A3820 | `sub_4A3820` | 有正文 | `src/ui/windows/WindowContacts.cpp:1578 — WindowContacts_DragScroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1621 | 0x004A38C0 | `sub_4A38C0` | 有正文 | `src/ui/windows/WindowContacts.cpp:950 — WindowContacts_OpenRankSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1622 | 0x004A3950 | `sub_4A3950` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:783 — WindowGuildDetails_ReadRankPermissions` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1623 | 0x004A3960 | `sub_4A3960` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:789 — WindowGuildDetails_CanEdit` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1624 | 0x004A3990 | `sub_4A3990` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:800 — WindowGuildDetails_CanEditRank` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1625 | 0x004A39C0 | `sub_4A39C0` | 有正文 | `src/ui/windows/WindowContacts.cpp:1018 — WindowContacts_HandleAvatar` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1626 | 0x004A3C70 | `sub_4A3C70` | 有正文 | `src/ui/windows/WindowContacts.cpp:1127 — WindowContacts_ButtonCompose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1627 | 0x004A40C0 | `sub_4A40C0` | 有正文 | `src/ui/windows/WindowContacts.cpp:891 — WindowContacts_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1628 | 0x004A4180 | `sub_4A4180` | 有正文 | `src/ui/windows/WindowGuildRank.cpp:98 — WindowGuildRank_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1629 | 0x004A41A0 | `sub_4A41A0` | 有正文 | `src/ui/windows/WindowGuildRank.cpp:107 — WindowGuildRank_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1630 | 0x004A4210 | `sub_4A4210` | 有正文 | `src/ui/WindowRuntime.cpp:835 — WindowRuntime_ReturnOne` | 原入口的 RET／固定返回已用当前 IDA 核实；不是业务占位；完整接线未验 |
| 1631 | 0x004A4220 | `sub_4A4220` | 有正文 | `src/ui/windows/WindowGuildRank.cpp:124 — WindowGuildRank_SelectRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1632 | 0x004A42C0 | `sub_4A42C0` | 有正文 | `src/ui/windows/WindowGuildRank.cpp:143 — WindowGuildRank_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1633 | 0x004A4370 | `sub_4A4370` | 有正文 | `src/ui/windows/WindowGuildRank.cpp:167 — WindowGuildRank_ButtonOk` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1634 | 0x004A4420 | `sub_4A4420` | 有正文 | `src/ui/windows/WindowGuildRank.cpp:191 — WindowGuildRank_HandleScrollButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1635 | 0x004A4540 | `sub_4A4540` | 有正文 | `src/ui/windows/WindowGuildRank.cpp:224 — WindowGuildRank_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1636 | 0x004A45E0 | `sub_4A45E0` | 有正文 | `src/ui/windows/WindowGuildRank.cpp:241 — WindowGuildRank_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1637 | 0x004A4620 | `sub_4A4620` | 有正文 | `src/ui/windows/WindowMailRead.cpp:93 — WindowMailRead_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1638 | 0x004A4650 | `sub_4A4650` | 有正文 | `src/ui/windows/WindowMailRead.cpp:106 — WindowMailRead_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1639 | 0x004A4660 | `sub_4A4660` | 有正文 | `src/ui/windows/WindowMailRead.cpp:114 — WindowMailRead_ButtonPrevious` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1640 | 0x004A4710 | `sub_4A4710` | 有正文 | `src/ui/windows/WindowMailRead.cpp:141 — WindowMailRead_ButtonNext` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1641 | 0x004A47C0 | `sub_4A47C0` | 有正文 | `src/ui/windows/WindowMailRead.cpp:168 — WindowMailRead_ButtonReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1642 | 0x004A48A0 | `sub_4A48A0` | 有正文 | `src/ui/windows/WindowMailRead.cpp:204 — WindowMailRead_FindUnread` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1643 | 0x004A49B0 | `sub_4A49B0` | 有正文 | `src/ui/windows/WindowMailRead.cpp:255 — WindowMailRead_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1644 | 0x004A4FC0 | `sub_4A4FC0` | 有正文 | `src/ui/windows/WindowMailRead.cpp:334 — WindowMailRead_HandleAvatar` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1645 | 0x004A5390 | `sub_4A5390` | 有正文 | `src/ui/windows/WindowMail.cpp:112 — WindowMail_ResetComposeState` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1646 | 0x004A53D0 | `sub_4A53D0` | 有正文 | `src/ui/windows/WindowMailCompose.cpp:48 — WindowMailCompose_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1647 | 0x004A53F0 | `sub_4A53F0` | 有正文 | `src/ui/windows/WindowMailCompose.cpp:55 — WindowMailCompose_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1648 | 0x004A5650 | `sub_4A5650` | 有正文 | `src/ui/windows/WindowMailCompose.cpp:150 — WindowMailCompose_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1649 | 0x004A56F0 | `sub_4A56F0` | 有正文 | `src/ui/windows/WindowPetMailCompose.cpp:65 — WindowPetMailCompose_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1650 | 0x004A58E0 | `sub_4A58E0` | 有正文 | `src/ui/windows/WindowMailCompose.cpp:178 — WindowMailCompose_HandleInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1651 | 0x004A5970 | `sub_4A5970` | 有正文 | `src/ui/windows/WindowMailCompose.cpp:202 — WindowMailCompose_HandleRecipients` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1652 | 0x004A6160 | `sub_4A6160` | 有正文 | `src/ui/windows/WindowMailCompose.cpp:363 — WindowMailCompose_RefreshRecipients` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1653 | 0x004A6300 | `sub_4A6300` | 有正文 | `src/ui/windows/WindowPetMailCompose.cpp:134 — WindowPetMailCompose_HandleGiftItem` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1654 | 0x004A6450 | `sub_4A6450` | 有正文 | `src/ui/windows/WindowPetMailCompose.cpp:212 — WindowPetMailCompose_HandleSendButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1655 | 0x004A6AC0 | `sub_4A6AC0` | 有正文 | `src/ui/windows/WindowPetMailCompose.cpp:332 — WindowPetMailCompose_RefreshPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1656 | 0x004A6D00 | `sub_4A6D00` | 有正文 | `src/ui/WindowRuntime.cpp:762 — WindowRuntime_MatchesRuntimePrefix` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1657 | 0x004A6D30 | `sub_4A6D30` | 有正文 | `src/ui/windows/WindowPetMailCompose.cpp:197 — WindowPetMailCompose_HasGiftPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1658 | 0x004A6D50 | `sub_4A6D50` | 有正文 | `src/ui/windows/WindowPetMailCompose.cpp:203 — WindowPetMailCompose_HasGiftItem` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1659 | 0x004A6D80 | `sub_4A6D80` | 有正文 | `src/ui/windows/WindowChat.cpp:168 — WindowChat_ResetState` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1660 | 0x004A6DB0 | `sub_4A6DB0` | 有正文 | `src/ui/windows/WindowChat.cpp:180 — WindowChat_ResetScrollRequests` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1661 | 0x004A6DD0 | `sub_4A6DD0` | 有正文 | `src/ui/windows/WindowChat.cpp:384 — WindowChat_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1662 | 0x004A6EA0 | `sub_4A6EA0` | 有正文 | `src/ui/windows/WindowChat.cpp:622 — WindowChat_HandleInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1663 | 0x004A6FC0 | `sub_4A6FC0` | 有正文 | `src/ui/windows/WindowChat.cpp:415 — WindowChat_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1664 | 0x004A7590 | `sub_4A7590` | 有正文 | `src/ui/windows/WindowChat.cpp:793 — WindowChat_DrawInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1665 | 0x004A7640 | `sub_4A7640` | 有正文 | `src/ui/windows/WindowChat.cpp:805 — WindowChat_DrawFixedWidthInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1666 | 0x004A76E0 | `sub_4A76E0` | 有正文 | `src/world/ChatHistory.cpp:1167 — Sub_4A76E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1667 | 0x004A7710 | `___setargv_13` | 有正文 | `src/ui/windows/WindowChat.cpp:753 — WindowChat_RequestHeightIncrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1668 | 0x004A7720 | `sub_4A7720` | 有正文 | `src/ui/windows/WindowChat.cpp:759 — WindowChat_RequestHeightDecrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1669 | 0x004A7730 | `___setargv_14` | 有正文 | `src/ui/windows/WindowChat.cpp:765 — WindowChat_RequestWidthIncrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1670 | 0x004A7740 | `sub_4A7740` | 有正文 | `src/ui/windows/WindowChat.cpp:771 — WindowChat_RequestWidthDecrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1671 | 0x004A7750 | `sub_4A7750` | 有正文 | `src/ui/windows/WindowChat.cpp:814 — WindowChat_DrawHistory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1672 | 0x004A7B00 | `sub_4A7B00` | 有正文 | `src/ui/windows/WindowChat.cpp:899 — WindowChat_RestoreInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1673 | 0x004A7C40 | `sub_4A7C40` | 有正文 | `src/ui/windows/WindowChat.cpp:244 — WindowChat_RefreshGeometry` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1674 | 0x004A80E0 | `sub_4A80E0` | 有正文 | `src/ui/windows/WindowChat.cpp:956 — WindowChat_ReadInputByteWidth` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1675 | 0x004A80F0 | `sub_4A80F0` | 有正文 | `src/ui/windows/WindowChat.cpp:700 — WindowChat_DragScroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1676 | 0x004A8140 | `sub_4A8140` | 有正文 | `src/ui/windows/WindowChat.cpp:217 — WindowChat_SyncScrollThumb` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1677 | 0x004A81D0 | `sub_4A81D0` | 有正文 | `src/ui/windows/WindowChat.cpp:714 — WindowChat_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1678 | 0x004A8240 | `sub_4A8240` | 有正文 | `src/ui/windows/WindowChat.cpp:669 — WindowCompactChat_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1679 | 0x004A82C0 | `sub_4A82C0` | 有正文 | `src/ui/windows/WindowChat.cpp:690 — WindowChat_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1680 | 0x004A82E0 | `sub_4A82E0` | 有正文 | `src/ui/windows/WindowChat.cpp:777 — WindowChat_RefreshCompactHeight` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1681 | 0x004A8330 | `sub_4A8330` | 有正文 | `src/ui/windows/WindowChat.cpp:190 — WindowChat_SwitchMode` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1682 | 0x004A83A0 | `sub_4A83A0` | 有正文 | `src/ui/windows/WindowBank.cpp:329 — WindowBank_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1683 | 0x004A83D0 | `sub_4A83D0` | 有正文 | `src/ui/windows/WindowBank.cpp:341 — WindowBank_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1684 | 0x004A84A0 | `sub_4A84A0` | 有正文 | `src/ui/windows/WindowBank.cpp:1596 — WindowBank_ReceiveStorageData` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1685 | 0x004A9020 | `sub_4A9020` | 有正文 | `src/ui/windows/WindowBank.cpp:368 — WindowBank_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1686 | 0x004A9160 | `sub_4A9160` | 有正文 | `src/ui/windows/WindowBank.cpp:404 — WindowBank_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1687 | 0x004A92D0 | `sub_4A92D0` | 有正文 | `src/ui/windows/WindowBank.cpp:449 — WindowBank_FocusAmountInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1688 | 0x004A9350 | `sub_4A9350` | 有正文 | `src/ui/windows/WindowBank.cpp:1573 — WindowBank_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1689 | 0x004A93E0 | `sub_4A93E0` | 有正文 | `src/ui/windows/WindowBank.cpp:469 — WindowBank_ButtonFirstPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1690 | 0x004A94A0 | `sub_4A94A0` | 有正文 | `src/ui/windows/WindowBank.cpp:493 — WindowBank_ButtonSecondPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1691 | 0x004A9560 | `sub_4A9560` | 有正文 | `src/ui/windows/WindowBank.cpp:517 — WindowBank_DrawFirstPageSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1692 | 0x004A9580 | `sub_4A9580` | 有正文 | `src/ui/windows/WindowBank.cpp:524 — WindowBank_DrawSecondPageSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1693 | 0x004A95A0 | `sub_4A95A0` | 有正文 | `src/ui/windows/WindowBank.cpp:531 — WindowBank_TabItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1694 | 0x004A96D0 | `sub_4A96D0` | 有正文 | `src/ui/windows/WindowBank.cpp:561 — WindowBank_TabPets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1695 | 0x004A9820 | `sub_4A9820` | 有正文 | `src/ui/windows/WindowBank.cpp:594 — WindowBank_TabDeposit` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1696 | 0x004A9990 | `sub_4A9990` | 有正文 | `src/ui/windows/WindowBank.cpp:628 — WindowBank_TabWithdraw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1697 | 0x004A9B00 | `sub_4A9B00` | 有正文 | `src/ui/windows/WindowBank.cpp:1482 — WindowBank_DrawCounts` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1698 | 0x004A9D10 | `sub_4A9D10` | 有正文 | `src/ui/windows/WindowBank.cpp:662 — WindowBank_HandleOwnItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1699 | 0x004AA3D0 | `sub_4AA3D0` | 有正文 | `src/ui/windows/WindowBank.cpp:842 — WindowBank_HandleStoredItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1700 | 0x004AA9C0 | `sub_4AA9C0` | 有正文 | `src/ui/windows/WindowBank.cpp:1033 — WindowBank_ButtonPreviousPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1701 | 0x004AAA70 | `sub_4AAA70` | 有正文 | `src/ui/windows/WindowBank.cpp:1063 — WindowBank_ButtonNextPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1702 | 0x004AAB20 | `sub_4AAB20` | 有正文 | `src/ui/windows/WindowBank.cpp:1093 — WindowBank_ScrollPets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1703 | 0x004AAB70 | `sub_4AAB70` | 有正文 | `src/ui/windows/WindowBank.cpp:234 — WindowBank_DrawOwnPetTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1704 | 0x004AAEC0 | `sub_4AAEC0` | 有正文 | `src/ui/windows/WindowBank.cpp:1107 — WindowBank_HandleOwnPetRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1705 | 0x004AB400 | `sub_4AB400` | 有正文 | `src/ui/windows/WindowBank.cpp:281 — WindowBank_DrawStoredPetTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1706 | 0x004AB710 | `sub_4AB710` | 有正文 | `src/ui/windows/WindowBank.cpp:1253 — WindowBank_HandleStoredPetRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1707 | 0x004ABB20 | `sub_4ABB20` | 有正文 | `src/ui/windows/WindowBank.cpp:1388 — WindowBank_HandleAmount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1708 | 0x004ABE60 | `sub_4ABE60` | 有正文 | `src/Yobi/window.cpp:2079 — Sub_4ABE60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1709 | 0x004ABEF0 | `sub_4ABEF0` | 有正文 | `src/Yobi/window.cpp:2111 — Sub_4ABEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1710 | 0x004AC0C0 | `sub_4AC0C0` | 有正文 | `src/ui/windows/WindowItemStorage.cpp:55 — WindowItemStorage_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1711 | 0x004AC0F0 | `sub_4AC0F0` | 有正文 | `src/ui/windows/WindowItemStorage.cpp:68 — WindowItemStorage_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1712 | 0x004AC140 | `sub_4AC140` | 有正文 | `src/ui/windows/WindowItemStorage.cpp:83 — WindowItemStorage_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1713 | 0x004AC1B0 | `sub_4AC1B0` | 有正文 | `src/ui/windows/WindowItemStorage.cpp:99 — WindowItemStorage_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1714 | 0x004AC1EC | `?classic@locale@std@@SAABV12@XZ_18` | 有承接 | `src/ui/windows/WindowItemStorage.cpp:99 — WindowItemStorage_Draw` | 原绘图／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 1715 | 0x004AC200 | `sub_4AC200` | 有正文 | `src/ui/windows/WindowBank.cpp:1551 — WindowBank_ButtonCloseIcon` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1716 | 0x004AC290 | `sub_4AC290` | 有正文 | `src/ui/windows/WindowItemStorage.cpp:115 — WindowItemStorage_HandleOwnItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1717 | 0x004AC890 | `sub_4AC890` | 有正文 | `src/ui/windows/WindowItemStorage.cpp:270 — WindowItemStorage_HandleStoredItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1718 | 0x004ACDF0 | `sub_4ACDF0` | 有正文 | `src/Yobi/window.cpp:2167 — Sub_4ACDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1719 | 0x004ACE80 | `sub_4ACE80` | 有正文 | `src/Yobi/window.cpp:2200 — Sub_4ACE80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1720 | 0x004AD060 | `sub_4AD060` | 有正文 | `src/ui/windows/WindowTrade.cpp:544 — WindowTrade_RemapTextListSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1721 | 0x004AD090 | `sub_4AD090` | 有正文 | `src/ui/windows/WindowTextList.cpp:170 — WindowTextList_ReleaseLines` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1722 | 0x004AD0F0 | `sub_4AD0F0` | 有正文 | `src/ui/windows/WindowTextList.cpp:177 — WindowTextList_RefreshLines` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1723 | 0x004AD180 | `sub_4AD180` | 有正文 | `src/ui/windows/WindowTextList.cpp:201 — WindowTextList_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1724 | 0x004AD470 | `sub_4AD470` | 有正文 | `src/ui/windows/WindowTextList.cpp:220 — WindowTextList_ContinueDialog` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1725 | 0x004AD780 | `sub_4AD780` | 有正文 | `src/ui/windows/WindowTextList.cpp:242 — WindowTextList_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1726 | 0x004AD800 | `sub_4AD800` | 有正文 | `src/ui/windows/WindowTextList.cpp:267 — WindowTextList_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1727 | 0x004AD9F0 | `sub_4AD9F0` | 有正文 | `src/ui/windows/WindowTextList.cpp:323 — WindowTextList_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1728 | 0x004ADAA0 | `sub_4ADAA0` | 有正文 | `src/ui/windows/WindowTextList.cpp:345 — WindowTextList_DrawConfirmation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1729 | 0x004ADBD0 | `sub_4ADBD0` | 有正文 | `src/ui/windows/WindowTextList.cpp:371 — WindowTextList_ButtonPreviousRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1730 | 0x004ADC80 | `sub_4ADC80` | 有正文 | `src/ui/windows/WindowTextList.cpp:400 — WindowTextList_ButtonNextRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1731 | 0x004ADD50 | `sub_4ADD50` | 有正文 | `src/ui/windows/WindowTextList.cpp:429 — WindowTextList_ButtonPreviousPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1732 | 0x004ADE10 | `sub_4ADE10` | 有正文 | `src/ui/windows/WindowTextList.cpp:456 — WindowTextList_ButtonNextPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1733 | 0x004ADEF0 | `sub_4ADEF0` | 有正文 | `src/ui/windows/WindowTextList.cpp:486 — WindowTextList_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1734 | 0x004ADF40 | `sub_4ADF40` | 有正文 | `src/ui/windows/WindowTextList.cpp:499 — WindowTextList_SubmitTitle` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1735 | 0x004AE170 | `sub_4AE170` | 有正文 | `src/ui/windows/WindowTextList.cpp:544 — WindowTextList_SubmitSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1736 | 0x004AE260 | `sub_4AE260` | 有正文 | `src/ui/windows/WindowTextList.cpp:579 — WindowTextList_EditTitle` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1737 | 0x004AE350 | `sub_4AE350` | 有正文 | `src/ui/windows/WindowTextList.cpp:603 — WindowTextList_HandleItemSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1738 | 0x004AE5B0 | `sub_4AE5B0` | 有正文 | `src/ui/windows/WindowTextList.cpp:684 — WindowTextList_DrawBackgroundTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1739 | 0x004AE5F0 | `sub_4AE5F0` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:142 — WindowConfirmation_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1740 | 0x004AE620 | `sub_4AE620` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:156 — WindowConfirmation_ButtonOk` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1741 | 0x004AE690 | `sub_4AE690` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:162 — WindowConfirmation_ButtonCancel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1742 | 0x004AE700 | `sub_4AE700` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:168 — WindowConfirmation_ButtonActionTwo` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1743 | 0x004AE770 | `sub_4AE770` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:174 — WindowConfirmation_ButtonActionThree` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1744 | 0x004AE7E0 | `sub_4AE7E0` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:180 — WindowConfirmation_ButtonActionFour` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1745 | 0x004AE850 | `sub_4AE850` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:186 — WindowConfirmation_ButtonActionFive` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1746 | 0x004AE8C0 | `sub_4AE8C0` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:192 — WindowConfirmation_RequestClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1747 | 0x004AE8D0 | `sub_4AE8D0` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:200 — WindowConfirmation_RefreshText` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1748 | 0x004AE9C0 | `sub_4AE9C0` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:222 — WindowConfirmation_RefreshButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1749 | 0x004AEA10 | `sub_4AEA10` | 有正文 | `src/ui/windows/WindowConfirmation.cpp:236 — WindowConfirmation_PadText` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1750 | 0x004AEAE0 | `sub_4AEAE0` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:256 — WindowGuildDetails_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1751 | 0x004AEC10 | `sub_4AEC10` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:283 — WindowGuildDetails_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1752 | 0x004AEC30 | `sub_4AEC30` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:494 — WindowGuildDetails_ToggleRankPermission` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1753 | 0x004AED40 | `sub_4AED40` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:517 — WindowGuildDetails_HandleActionButton` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1754 | 0x004AEE00 | `sub_4AEE00` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:543 — WindowGuildDetails_HandleInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1755 | 0x004AF000 | `sub_4AF000` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:599 — WindowGuildDetails_HandleScrollButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1756 | 0x004AF090 | `sub_4AF090` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:621 — WindowGuildDetails_HandleScrollFocus` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1757 | 0x004AF0C0 | `sub_4AF0C0` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:631 — WindowGuildDetails_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1758 | 0x004AF100 | `sub_4AF100` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:646 — WindowGuildDetails_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1759 | 0x004AF180 | `sub_4AF180` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:292 — WindowGuildDetails_RefreshReadOnly` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1760 | 0x004AF250 | `sub_4AF250` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:315 — WindowGuildDetails_RefreshEditor` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1761 | 0x004AF650 | `sub_4AF650` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:397 — WindowGuildDetails_RefreshRanks` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1762 | 0x004AF960 | `sub_4AF960` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:663 — WindowGuildDetails_RefreshDraft` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1763 | 0x004AFA00 | `sub_4AFA00` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:440 — WindowGuildDetails_SaveInputDraft` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1764 | 0x004AFA60 | `sub_4AFA60` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:476 — WindowGuildDetails_ScrollRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1765 | 0x004AFAE0 | `sub_4AFAE0` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:466 — WindowGuildDetails_DragScroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1766 | 0x004AFB10 | `sub_4AFB10` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:707 — WindowGuildDetails_SubmitName` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1767 | 0x004AFC00 | `sub_4AFC00` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:730 — WindowGuildDetails_SubmitDescription` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1768 | 0x004AFCF0 | `sub_4AFCF0` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:753 — WindowGuildDetails_SubmitRanks` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1769 | 0x004AFE20 | `sub_4AFE20` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:682 — WindowGuildDetails_HasChangedRank` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1770 | 0x004AFEA0 | `sub_4AFEA0` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:695 — WindowGuildDetails_HasChangedRanks` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1771 | 0x004AFED0 | `sub_4AFED0` | 有正文 | `src/ui/windows/WindowGuildDetails.cpp:452 — WindowGuildDetails_CommitFocusedInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1772 | 0x004AFF30 | `sub_4AFF30` | 有正文 | `src/ui/windows/WindowTrade.cpp:632 — WindowTrade_AddItem` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1773 | 0x004AFF70 | `sub_4AFF70` | 有正文 | `src/ui/windows/WindowTrade.cpp:675 — WindowTrade_RemoveItem` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1774 | 0x004AFFB0 | `sub_4AFFB0` | 有正文 | `src/ui/windows/WindowTrade.cpp:646 — WindowTrade_SwapSelectedItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1775 | 0x004AFFE0 | `sub_4AFFE0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2530 — WindowTrade_ReadSelectedItem` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1776 | 0x004B0000 | `sub_4B0000` | 有正文 | `src/ui/windows/WindowTrade.cpp:524 — WindowTrade_ResetSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1777 | 0x004B0060 | `sub_4B0060` | 有正文 | `src/ui/windows/WindowTrade.cpp:655 — WindowTrade_AddPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1778 | 0x004B0090 | `sub_4B0090` | 有正文 | `src/ui/windows/WindowTrade.cpp:690 — WindowTrade_RemovePet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1779 | 0x004B00C0 | `sub_4B00C0` | 有正文 | `src/ui/windows/WindowTrade.cpp:666 — WindowTrade_SwapSelectedPets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1780 | 0x004B00F0 | `sub_4B00F0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2537 — WindowTrade_ReadSelectedPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1781 | 0x004B0110 | `sub_4B0110` | 有正文 | `src/ui/windows/WindowTrade.cpp:590 — WindowTrade_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1782 | 0x004B0130 | `sub_4B0130` | 有正文 | `src/ui/windows/WindowTrade.cpp:599 — WindowTrade_MovePanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1783 | 0x004B0180 | `sub_4B0180` | 有正文 | `src/ui/windows/WindowTrade.cpp:613 — WindowTrade_SetPanelVisible` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1784 | 0x004B01B0 | `sub_4B01B0` | 有正文 | `src/ui/windows/WindowTrade.cpp:624 — WindowTrade_SetPanelColor` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1785 | 0x004B0200 | `sub_4B0200` | 有正文 | `src/ui/windows/WindowTrade.cpp:702 — WindowTrade_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1786 | 0x004B0290 | `sub_4B0290` | 有正文 | `src/ui/windows/WindowTrade.cpp:723 — WindowTrade_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1787 | 0x004B08D0 | `sub_4B08D0` | 有正文 | `src/ui/windows/WindowTrade.cpp:899 — WindowTrade_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1788 | 0x004B09E0 | `sub_4B09E0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2159 — WindowTrade_HandleAmountInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1789 | 0x004B0A50 | `sub_4B0A50` | 有正文 | `src/ui/windows/WindowTrade.cpp:2544 — WindowTrade_ButtonPublish` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1790 | 0x004B0CF0 | `sub_4B0CF0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2607 — WindowTrade_ButtonWithdraw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1791 | 0x004B0DB0 | `sub_4B0DB0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2637 — WindowTrade_ButtonConfirm` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1792 | 0x004B0F10 | `NULL_FUNC` | 有正文 | `src/ui/WindowRuntime.cpp:828 — WindowElement_NoOp` | 原入口的 RET／固定返回已用当前 IDA 核实；不是业务占位；完整接线未验 |
| 1793 | 0x004B0F20 | `sub_4B0F20` | 有正文 | `src/ui/windows/WindowTrade.cpp:946 — WindowTrade_DrawPetRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1794 | 0x004B1260 | `sub_4B1260` | 有正文 | `src/ui/windows/WindowTrade.cpp:997 — WindowTrade_HandlePanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1795 | 0x004B1900 | `sub_4B1900` | 有正文 | `src/ui/windows/WindowTrade.cpp:2144 — WindowTrade_HitTestOwnPetRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1796 | 0x004B1940 | `sub_4B1940` | 有正文 | `src/ui/windows/WindowTrade.cpp:1144 — WindowTrade_HandleOwnInventory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1797 | 0x004B2060 | `sub_4B2060` | 有正文 | `src/ui/windows/WindowTrade.cpp:1323 — WindowTrade_HandleOwnPets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1798 | 0x004B2480 | `sub_4B2480` | 有正文 | `src/ui/windows/WindowTrade.cpp:1453 — WindowTrade_HandleOwnPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1799 | 0x004B2500 | `sub_4B2500` | 有正文 | `src/ui/windows/WindowTrade.cpp:1469 — WindowTrade_RefreshOwnPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1800 | 0x004B25D0 | `sub_4B25D0` | 有正文 | `src/ui/windows/WindowTrade.cpp:1509 — WindowTrade_HandleOwnItemDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1801 | 0x004B2710 | `sub_4B2710` | 有正文 | `src/ui/windows/WindowTrade.cpp:1591 — WindowTrade_StartOwnItemDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1802 | 0x004B2730 | `sub_4B2730` | 有正文 | `src/ui/windows/WindowTrade.cpp:1603 — WindowTrade_HandleSelectedItem` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1803 | 0x004B2E00 | `sub_4B2E00` | 有正文 | `src/ui/windows/WindowTrade.cpp:1548 — WindowTrade_HandleOwnPetDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1804 | 0x004B2F50 | `sub_4B2F50` | 有正文 | `src/ui/windows/WindowTrade.cpp:1597 — WindowTrade_StartOwnPetDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1805 | 0x004B2F70 | `sub_4B2F70` | 有正文 | `src/ui/windows/WindowTrade.cpp:1777 — WindowTrade_HandleSelectedPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1806 | 0x004B3430 | `sub_4B3430` | 有正文 | `src/ui/windows/WindowTrade.cpp:1914 — WindowTrade_HandleSelectedPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1807 | 0x004B34C0 | `sub_4B34C0` | 有正文 | `src/ui/windows/WindowTrade.cpp:1933 — WindowTrade_RefreshSelectedPanel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1808 | 0x004B3580 | `sub_4B3580` | 有正文 | `src/ui/windows/WindowTrade.cpp:1965 — WindowTrade_HandleRemoteSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1809 | 0x004B3610 | `sub_4B3610` | 有正文 | `src/ui/windows/WindowTrade.cpp:1984 — WindowTrade_HandleRemoteItem` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1810 | 0x004B3B00 | `sub_4B3B00` | 有正文 | `src/ui/windows/WindowTrade.cpp:2414 — WindowTrade_DrawOwnPetSkillRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1811 | 0x004B3BF0 | `sub_4B3BF0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2444 — WindowTrade_DrawOwnPetTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1812 | 0x004B4070 | `sub_4B4070` | 有正文 | `src/ui/windows/WindowTrade.cpp:2177 — WindowTrade_DrawRemotePetSkillRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1813 | 0x004B4140 | `sub_4B4140` | 有正文 | `src/ui/windows/WindowTrade.cpp:2204 — WindowTrade_DrawRemotePetTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1814 | 0x004B45A0 | `sub_4B45A0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2297 — WindowTrade_DrawSelectedPetSkillRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1815 | 0x004B4680 | `sub_4B4680` | 有正文 | `src/ui/windows/WindowTrade.cpp:2327 — WindowTrade_DrawSelectedPetTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1816 | 0x004B4B00 | `sub_4B4B00` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:173 — WindowGroupSelect_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1817 | 0x004B4B70 | `sub_4B4B70` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:193 — WindowGroupSelect_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1818 | 0x004B4B90 | `sub_4B4B90` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:202 — WindowGroupSelect_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1819 | 0x004B4BB0 | `sub_4B4BB0` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:211 — WindowGroupSelect_HandlePageButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1820 | 0x004B4CA0 | `sub_4B4CA0` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:238 — WindowGroupSelect_HandleScrollButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1821 | 0x004B4DB0 | `sub_4B4DB0` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:278 — WindowGroupSelect_ClampScroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1822 | 0x004B4DF0 | `sub_4B4DF0` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:294 — WindowGroupSelect_DragScroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1823 | 0x004B4E20 | `sub_4B4E20` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:305 — WindowGroupSelect_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1824 | 0x004B5210 | `sub_4B5210` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:341 — WindowGroupSelect_TabCategory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1825 | 0x004B53C0 | `sub_4B53C0` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:385 — WindowGroupSelect_ButtonSelectAll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1826 | 0x004B5550 | `sub_4B5550` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:431 — WindowGroupSelect_SelectRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1827 | 0x004B5630 | `sub_4B5630` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:475 — WindowGroupSelect_ResetRecipients` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1828 | 0x004B5660 | `sub_4B5660` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:486 — WindowGroupSelect_BuildRecipients` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1829 | 0x004B5760 | `sub_4B5760` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:527 — WindowGroupSelect_SortRecipients` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1830 | 0x004B57B0 | `sub_4B57B0` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:538 — WindowGroupSelect_CompareRecipients` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1831 | 0x004B5A40 | `sub_4B5A40` | 有正文 | `src/ui/windows/WindowGroupSelect.cpp:460 — WindowGroupSelect_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1832 | 0x004B5A90 | `sub_4B5A90` | 有正文 | `src/ui/windows/WindowMessageBoard.cpp:121 — WindowMessageBoard_ResetRecords` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1833 | 0x004B5AD0 | `sub_4B5AD0` | 有正文 | `src/ui/windows/WindowMessageBoard.cpp:136 — WindowMessageBoard_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1834 | 0x004B5B50 | `sub_4B5B50` | 有正文 | `src/ui/windows/WindowMessageBoard.cpp:155 — WindowMessageBoard_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1835 | 0x004B5B70 | `sub_4B5B70` | 有正文 | `src/ui/windows/WindowMessageBoard.cpp:163 — WindowMessageBoard_HandleInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1836 | 0x004B5C30 | `sub_4B5C30` | 有正文 | `src/ui/windows/WindowMessageBoard.cpp:183 — WindowMessageBoard_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1837 | 0x004B62A0 | `sub_4B62A0` | 有正文 | `src/ui/windows/WindowMessageBoard.cpp:303 — WindowMessageBoard_RefreshRecords` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1838 | 0x004B6970 | `sub_4B6970` | 有正文 | `src/ui/windows/WindowMessageBoard.cpp:357 — WindowMessageBoard_ReceiveRecords` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1839 | 0x004B6C20 | `sub_4B6C20` | 有正文 | `src/ui/windows/WindowMessageBoard.cpp:348 — WindowMessageBoard_Close` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1840 | 0x004B6C50 | `sub_4B6C50` | 有正文 | `src/ui/windows/WindowShop.cpp:492 — WindowShop_HandleSaleDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1841 | 0x004B6DA0 | `sub_4B6DA0` | 有正文 | `src/ui/windows/WindowShop.cpp:537 — WindowShop_StartSaleDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1842 | 0x004B6DC0 | `sub_4B6DC0` | 有正文 | `src/ui/windows/WindowShop.cpp:579 — WindowShop_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1843 | 0x004B6E00 | `sub_4B6E00` | 有正文 | `src/ui/windows/WindowShop.cpp:593 — WindowShop_OpenBuy` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1844 | 0x004B70D0 | `sub_4B70D0` | 有正文 | `src/ui/windows/WindowShop.cpp:646 — WindowShop_OpenSell` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1845 | 0x004B74E0 | `sub_4B74E0` | 有正文 | `src/ui/windows/WindowShop.cpp:738 — WindowShop_OpenExchange` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1846 | 0x004B77E0 | `sub_4B77E0` | 有正文 | `src/ui/windows/WindowShop.cpp:801 — WindowShop_OpenItemDetails` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1847 | 0x004B7AB0 | `sub_4B7AB0` | 有正文 | `src/ui/windows/WindowShop.cpp:947 — WindowShop_OpenIdentify` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1848 | 0x004B7EE0 | `sub_4B7EE0` | 有正文 | `src/ui/windows/WindowShop.cpp:954 — WindowShop_OpenRepair` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1849 | 0x004B8310 | `sub_4B8310` | 有正文 | `src/ui/windows/WindowShop.cpp:961 — WindowShop_RefreshBuyRows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1850 | 0x004B8670 | `sub_4B8670` | 有正文 | `src/ui/windows/WindowShop.cpp:1019 — WindowShop_RefreshBuyQuantities` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1851 | 0x004B86C0 | `sub_4B86C0` | 有正文 | `src/ui/windows/WindowShop.cpp:1035 — WindowShop_RefreshBuyAmount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1852 | 0x004B8730 | `sub_4B8730` | 有正文 | `src/ui/windows/WindowShop.cpp:1058 — WindowShop_CountBuySlots` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1853 | 0x004B8780 | `sub_4B8780` | 有正文 | `src/ui/windows/WindowShop.cpp:1079 — WindowShop_RefreshSellRows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1854 | 0x004B8B10 | `sub_4B8B10` | 有正文 | `src/ui/windows/WindowShop.cpp:1145 — WindowShop_RefreshSellQuantities` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1855 | 0x004B8BE0 | `sub_4B8BE0` | 有正文 | `src/ui/windows/WindowShop.cpp:1172 — WindowShop_RefreshSellAmount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1856 | 0x004B8C50 | `sub_4B8C50` | 有正文 | `src/ui/windows/WindowShop.cpp:1193 — WindowShop_CountSellItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1857 | 0x004B8C90 | `sub_4B8C90` | 有正文 | `src/ui/windows/WindowShop.cpp:1210 — WindowShop_RefreshExchangeRows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1858 | 0x004B8FF0 | `sub_4B8FF0` | 有正文 | `src/ui/windows/WindowShop.cpp:1271 — WindowShop_RefreshExchangeQuantities` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1859 | 0x004B9090 | `sub_4B9090` | 有正文 | `src/ui/windows/WindowShop.cpp:1290 — WindowShop_RefreshExchangeAmount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1860 | 0x004B90E0 | `sub_4B90E0` | 有正文 | `src/ui/windows/WindowShop.cpp:1309 — WindowShop_CountExchangeItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1861 | 0x004B9110 | `sub_4B9110` | 有正文 | `src/ui/windows/WindowShop.cpp:1325 — WindowShop_RefreshDetailName` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1862 | 0x004B9270 | `sub_4B9270` | 有正文 | `src/ui/windows/WindowShop.cpp:1335 — WindowShop_RefreshDetailAmount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1863 | 0x004B92C0 | `sub_4B92C0` | 有正文 | `src/ui/windows/WindowShop.cpp:1345 — WindowShop_CountDetailItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1864 | 0x004B9310 | `sub_4B9310` | 有正文 | `src/ui/windows/WindowShop.cpp:1365 — WindowShop_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1865 | 0x004B99D0 | `sub_4B99D0` | 有正文 | `src/ui/windows/WindowShop.cpp:3483 — WindowShop_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1866 | 0x004B9B10 | `sub_4B9B10` | 有正文 | `src/ui/windows/WindowShop.cpp:1550 — WindowShop_Submit` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1867 | 0x004B9F40 | `sub_4B9F40` | 有正文 | `src/ui/windows/WindowShop.cpp:1608 — WindowShop_ButtonOk` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1868 | 0x004BA950 | `sub_4BA950` | 有正文 | `src/ui/windows/WindowShop.cpp:1878 — WindowShop_ButtonCancel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1869 | 0x004BAA50 | `sub_4BAA50` | 有正文 | `src/ui/windows/WindowShop.cpp:1913 — WindowShop_ButtonAddAll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1870 | 0x004BAD10 | `sub_4BAD10` | 有正文 | `src/ui/windows/WindowShop.cpp:2118 — WindowShop_ButtonPreviousRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1871 | 0x004BAE10 | `sub_4BAE10` | 有正文 | `src/ui/windows/WindowShop.cpp:2164 — WindowShop_ButtonNextRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1872 | 0x004BAF10 | `sub_4BAF10` | 有正文 | `src/ui/windows/WindowShop.cpp:2210 — WindowShop_ButtonPreviousPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1873 | 0x004BB010 | `sub_4BB010` | 有正文 | `src/ui/windows/WindowShop.cpp:2254 — WindowShop_ButtonNextPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1874 | 0x004BB1D0 | `sub_4BB1D0` | 有正文 | `src/ui/windows/WindowShop.cpp:2336 — WindowShop_ButtonDecrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1875 | 0x004BB6E0 | `sub_4BB6E0` | 有正文 | `src/ui/windows/WindowShop.cpp:2569 — WindowShop_ButtonIncrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1876 | 0x004BBED0 | `sub_4BBED0` | 有正文 | `src/ui/windows/WindowShop.cpp:2920 — WindowShop_HandleItemIcon` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1877 | 0x004BC060 | `sub_4BC060` | 有正文 | `src/ui/windows/WindowShop.cpp:2976 — WindowShop_HandleDetailIcon` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1878 | 0x004BC150 | `sub_4BC150` | 有正文 | `src/ui/windows/WindowShop.cpp:543 — WindowShop_RemoveSaleSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1879 | 0x004BC230 | `sub_4BC230` | 有正文 | `src/ui/windows/WindowShop.cpp:3011 — WindowShop_HandleSaleSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1880 | 0x004BC660 | `sub_4BC660` | 有正文 | `src/ui/windows/WindowShop.cpp:3144 — WindowShop_HandleInventory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1881 | 0x004BCEE0 | `sub_4BCEE0` | 有正文 | `src/ui/windows/WindowShop.cpp:2032 — WindowShop_HandleSaleDrop` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1882 | 0x004BD100 | `sub_4BD100` | 有正文 | `src/ui/windows/WindowShop.cpp:3392 — WindowShop_HandleConfirmation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1883 | 0x004BD500 | `sub_4BD500` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:460 — WindowPetSkillChoice_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1884 | 0x004BD540 | `sub_4BD540` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:470 — WindowPetSkillChoice_OpenLearn` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1885 | 0x004BD680 | `sub_4BD680` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:504 — WindowPetSkillChoice_OpenForget` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1886 | 0x004BD810 | `sub_4BD810` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:605 — WindowPetSkillChoice_OpenPetSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1887 | 0x004BDAD0 | `sub_4BDAD0` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:611 — WindowPetSkillChoice_OpenPetSkillsByTribe` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1888 | 0x004BDD90 | `sub_4BDD90` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:617 — WindowPetSkillChoice_ReceivePetCandidates` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1889 | 0x004BDF70 | `sub_4BDF70` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:663 — WindowPetSkillChoice_ReceiveTribeCandidates` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1890 | 0x004BE200 | `sub_4BE200` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:713 — WindowPetSkillChoice_ReceiveSelectedPetSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1891 | 0x004BE3F0 | `sub_4BE3F0` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:762 — WindowPetSkillChoice_RefreshLearn` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1892 | 0x004BE600 | `sub_4BE600` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:815 — WindowPetSkillChoice_RefreshForget` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1893 | 0x004BE930 | `sub_4BE930` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:890 — WindowPetSkillChoice_RefreshPetNames` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1894 | 0x004BEC20 | `sub_4BEC20` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:944 — WindowPetSkillChoice_RefreshPetCandidates` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1895 | 0x004BEF30 | `sub_4BEF30` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1013 — WindowPetSkillChoice_RefreshSelectedPetSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1896 | 0x004BF160 | `sub_4BF160` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1061 — WindowPetSkillChoice_RefreshLearnRows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1897 | 0x004BF2E0 | `sub_4BF2E0` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1093 — WindowPetSkillChoice_RefreshForgetRows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1898 | 0x004BF460 | `sub_4BF460` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1122 — WindowPetSkillChoice_RefreshPetRows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1899 | 0x004BF620 | `sub_4BF620` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1160 — WindowPetSkillChoice_RefreshSelectedSkillRows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1900 | 0x004BF7C0 | `sub_4BF7C0` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1193 — WindowPetSkillChoice_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1901 | 0x004BF950 | `sub_4BF950` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1237 — WindowPetSkillChoice_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1902 | 0x004BFBC0 | `sub_4BFBC0` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1323 — WindowPetSkillChoice_ButtonLearn` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1903 | 0x004BFCB0 | `sub_4BFCB0` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1359 — WindowPetSkillChoice_ButtonBack` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1904 | 0x004BFF60 | `sub_4BFF60` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1473 — WindowPetSkillChoice_ButtonPreviousRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1905 | 0x004C0160 | `sub_4C0160` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1551 — WindowPetSkillChoice_ButtonNextRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1906 | 0x004C0380 | `sub_4C0380` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1633 — WindowPetSkillChoice_ButtonPreviousPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1907 | 0x004C0590 | `sub_4C0590` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1705 — WindowPetSkillChoice_ButtonNextPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1908 | 0x004C0800 | `sub_4C0800` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1801 — WindowPetSkillChoice_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1909 | 0x004C0910 | `sub_4C0910` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1845 — WindowPetSkillChoice_SubmitForget` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1910 | 0x004C0A50 | `sub_4C0A50` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1886 — WindowPetSkillChoice_SelectPetCandidate` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1911 | 0x004C0BF0 | `sub_4C0BF0` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1955 — WindowPetSkillChoice_SelectSkillSlot` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1912 | 0x004C0CB0 | `sub_4C0CB0` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:1983 — WindowPetSkillChoice_SubmitPetSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1913 | 0x004C0EA0 | `sub_4C0EA0` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:2045 — WindowPetSkillChoice_DrawSelectedSkillFrame` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1914 | 0x004C0F20 | `sub_4C0F20` | 有正文 | `src/ui/windows/WindowPetSkillChoice.cpp:2063 — WindowPetSkillChoice_HandleConfirmation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1915 | 0x004C1390 | `sub_4C1390` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:303 — WindowCrystalShop_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1916 | 0x004C13B0 | `sub_4C13B0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:312 — WindowCrystalShop_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1917 | 0x004C1700 | `sub_4C1700` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:380 — WindowCrystalShop_RefreshRemaining` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1918 | 0x004C17B0 | `sub_4C17B0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:396 — WindowCrystalShop_RefreshRows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1919 | 0x004C1AA0 | `sub_4C1AA0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:436 — WindowCrystalShop_RefreshQuantities` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1920 | 0x004C1B00 | `sub_4C1B00` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:450 — WindowCrystalShop_RefreshAmount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1921 | 0x004C1B70 | `sub_4C1B70` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:469 — WindowCrystalShop_CountSlots` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1922 | 0x004C1BC0 | `sub_4C1BC0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:487 — WindowCrystalShop_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1923 | 0x004C1C50 | `sub_4C1C50` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:514 — WindowCrystalShop_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1924 | 0x004C1EC0 | `sub_4C1EC0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:577 — WindowCrystalShop_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1925 | 0x004C1FB0 | `sub_4C1FB0` | 有正文 | `src/ui/WindowRuntime.cpp:1457 — WindowRuntime_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1926 | 0x004C2060 | `sub_4C2060` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:611 — WindowCrystalShop_Submit` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1927 | 0x004C21B0 | `sub_4C21B0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:648 — WindowCrystalShop_ButtonOk` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1928 | 0x004C2460 | `sub_4C2460` | 有正文 | `src/ui/windows/WindowCrystalTrade.cpp:9 — WindowCrystalTrade_ButtonCancel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1929 | 0x004C2530 | `sub_4C2530` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:731 — WindowCrystalShop_ButtonPreviousRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1930 | 0x004C25B0 | `sub_4C25B0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:756 — WindowCrystalShop_ButtonNextRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1931 | 0x004C2630 | `sub_4C2630` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:782 — WindowCrystalShop_ButtonPreviousPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1932 | 0x004C2634 | `sub_4C2634` | 有承接 | `src/ui/windows/WindowCrystalShop.cpp:782 — WindowCrystalShop_ButtonPreviousPage` | 原表指向含索引前缀的 4C2630；入口片段并入实际父正文；完整业务／运行未验 |
| 1933 | 0x004C26B0 | `sub_4C26B0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:806 — WindowCrystalShop_ButtonNextPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1934 | 0x004C2760 | `sub_4C2760` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:837 — WindowCrystalShop_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1935 | 0x004C27B0 | `sub_4C27B0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:851 — WindowCrystalShop_ButtonDecrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1936 | 0x004C28C0 | `sub_4C28C0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:886 — WindowCrystalShop_ButtonIncrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1937 | 0x004C29E0 | `sub_4C29E0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:930 — WindowCrystalShop_HandleItemIcon` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1938 | 0x004C2B20 | `sub_4C2B20` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:977 — WindowCrystalShop_HandleInventory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1939 | 0x004C2FA0 | `sub_4C2FA0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:1087 — WindowCrystalShop_HandleConfirmation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1940 | 0x004C33B0 | `sub_4C33B0` | 有正文 | `src/ui/windows/WindowCrystalShop.cpp:1178 — WindowCrystalShop_ShowTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1941 | 0x004C3530 | `sub_4C3530` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:299 — WindowCrystalExchangeShop_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1942 | 0x004C3550 | `sub_4C3550` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:308 — WindowCrystalExchangeShop_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1943 | 0x004C39A0 | `sub_4C39A0` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:370 — WindowCrystalExchangeShop_RefreshRemaining` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1944 | 0x004C3A50 | `sub_4C3A50` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:386 — WindowCrystalExchangeShop_RefreshRows` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1945 | 0x004C3D40 | `sub_4C3D40` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:426 — WindowCrystalExchangeShop_RefreshQuantities` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1946 | 0x004C3DA0 | `sub_4C3DA0` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:440 — WindowCrystalExchangeShop_RefreshAmount` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1947 | 0x004C3E20 | `sub_4C3E20` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:459 — WindowCrystalExchangeShop_CountSlots` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1948 | 0x004C3E70 | `sub_4C3E70` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:477 — WindowCrystalExchangeShop_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1949 | 0x004C3F00 | `sub_4C3F00` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:504 — WindowCrystalExchangeShop_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1950 | 0x004C4180 | `sub_4C4180` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:568 — WindowCrystalExchangeShop_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1951 | 0x004C4270 | `sub_4C4270` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:602 — WindowCrystalExchangeShop_Submit` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1952 | 0x004C43C0 | `sub_4C43C0` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:639 — WindowCrystalExchangeShop_ButtonOk` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1953 | 0x004C4680 | `sub_4C4680` | 有正文 | `src/ui/windows/WindowCrystalTrade.cpp:41 — WindowCrystalTrade_DrawButton` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1954 | 0x004C46C0 | `sub_4C46C0` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:722 — WindowCrystalExchangeShop_ButtonPreviousRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1955 | 0x004C4740 | `sub_4C4740` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:747 — WindowCrystalExchangeShop_ButtonNextRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1956 | 0x004C47C0 | `sub_4C47C0` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:773 — WindowCrystalExchangeShop_ButtonPreviousPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1957 | 0x004C4840 | `sub_4C4840` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:797 — WindowCrystalExchangeShop_ButtonNextPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1958 | 0x004C48EB | `sub_4C48EB` | 不适用 | `当前 IDA：0x004C48EB..0x004C48F0 仅 5 条 NOP` | 元数据登记的填充片段，无业务正文；保留原行，不扩大到其他入口 |
| 1959 | 0x004C48F0 | `sub_4C48F0` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:828 — WindowCrystalExchangeShop_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1960 | 0x004C4940 | `sub_4C4940` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:842 — WindowCrystalExchangeShop_ButtonDecrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1961 | 0x004C4A50 | `sub_4C4A50` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:877 — WindowCrystalExchangeShop_ButtonIncrease` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1962 | 0x004C4B70 | `sub_4C4B70` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:921 — WindowCrystalExchangeShop_HandleItemIcon` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1963 | 0x004C4CB0 | `sub_4C4CB0` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:968 — WindowCrystalExchangeShop_HandleInventory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1964 | 0x004C50C0 | `sub_4C50C0` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:1078 — WindowCrystalExchangeShop_HandleConfirmation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1965 | 0x004C54D0 | `sub_4C54D0` | 有正文 | `src/ui/windows/WindowCrystalExchangeShop.cpp:1169 — WindowCrystalExchangeShop_ShowTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1966 | 0x004C5650 | `sub_4C5650` | 有正文 | `src/CharacterSelectPreview.cpp:456 — Sub_4C5650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1967 | 0x004C56E0 | `sub_4C56E0` | 有正文 | `src/CharacterSelectPreview.cpp:425 — Sub_4C56E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1968 | 0x004C5790 | `sub_4C5790` | 有正文 | `src/ui/windows/WindowAppearance.cpp:126 — WindowAppearance_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1969 | 0x004C5850 | `sub_4C5850` | 有正文 | `src/ui/windows/WindowAppearance.cpp:151 — WindowAppearance_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1970 | 0x004C5B70 | `sub_4C5B70` | 有正文 | `src/ui/windows/WindowAppearance.cpp:221 — WindowAppearance_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1971 | 0x004C5C00 | `sub_4C5C00` | 有正文 | `src/ui/windows/WindowAppearance.cpp:239 — WindowAppearance_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1972 | 0x004C5C30 | `sub_4C5C30` | 有正文 | `src/ui/windows/WindowAppearance.cpp:248 — WindowAppearance_ButtonOk` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1973 | 0x004C5D00 | `sub_4C5D00` | 有正文 | `src/ui/windows/WindowAppearance.cpp:284 — WindowAppearance_CycleClass` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1974 | 0x004C5DE0 | `sub_4C5DE0` | 有正文 | `src/ui/windows/WindowAppearance.cpp:319 — WindowAppearance_CycleFifthDigit` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1975 | 0x004C5EB0 | `sub_4C5EB0` | 有正文 | `src/ui/windows/WindowAppearance.cpp:353 — WindowAppearance_CycleOnesDigit` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1976 | 0x004C5F80 | `sub_4C5F80` | 有正文 | `src/ui/windows/WindowAppearance.cpp:387 — WindowAppearance_SendDialogReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1977 | 0x004C6060 | `sub_4C6060` | 有正文 | `src/ui/windows/WindowAppearance.cpp:418 — WindowAppearance_OpenDialogPreview` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1978 | 0x004C61B0 | `sub_4C61B0` | 有正文 | `src/ui/WindowRuntime.cpp:1586 — WindowItemSelector_ConstructPreview` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1979 | 0x004C63C0 | `sub_4C63C0` | 有正文 | `src/ui/WindowRuntime.cpp:1686 — WindowItemSelector_ReadSlotX` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1980 | 0x004C6400 | `sub_4C6400` | 有正文 | `src/ui/WindowRuntime.cpp:1704 — WindowItemSelector_ReadSlotY` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1981 | 0x004C6440 | `sub_4C6440` | 有正文 | `src/ui/WindowRuntime.cpp:1722 — WindowItemSelector_ShowPlayerPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1982 | 0x004C6470 | `sub_4C6470` | 有正文 | `src/ui/WindowRuntime.cpp:1734 — WindowItemSelector_ShowPetPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1983 | 0x004C6500 | `sub_4C6500` | 有正文 | `src/ui/WindowRuntime.cpp:1756 — WindowItemSelector_HidePageElements` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1984 | 0x004C6550 | `sub_4C6550` | 有正文 | `src/ui/WindowRuntime.cpp:1774 — WindowItemSelector_SelectPage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1985 | 0x004C6790 | `sub_4C6790` | 有正文 | `src/ui/WindowRuntime.cpp:1861 — WindowItemSelector_UpdatePage` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1986 | 0x004C6990 | `sub_4C6990` | 有正文 | `src/ui/WindowRuntime.cpp:1949 — WindowItemSelector_SelectPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1987 | 0x004C69B0 | `sub_4C69B0` | 有正文 | `src/ui/WindowRuntime.cpp:1958 — WindowItemSelector_RefreshPetHeader` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1988 | 0x004C6AC0 | `sub_4C6AC0` | 有正文 | `src/ui/WindowRuntime.cpp:1991 — WindowItemSelector_MoveElements` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1989 | 0x004C6B10 | `sub_4C6B10` | 有正文 | `src/ui/WindowRuntime.cpp:2004 — WindowItemSelector_DrawTooltip` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1990 | 0x004C6D90 | `sub_4C6D90` | 有正文 | `src/ui/WindowRuntime.cpp:2082 — WindowItemSelector_DrawPreview` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1991 | 0x004C6EC0 | `sub_4C6EC0` | 有正文 | `src/ui/WindowRuntime.cpp:1626 — WindowItemSelector_BindElements` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1992 | 0x004C71B0 | `sub_4C71B0` | 有正文 | `src/ui/WindowRuntime.cpp:1619 — WindowItemSelector_ReadSelectedPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1993 | 0x004C71C0 | `sub_4C71C0` | 有正文 | `src/ui/WindowRuntime.cpp:2109 — WindowItemSelector_FindPetIndex` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1994 | 0x004C7240 | `sub_4C7240` | 有正文 | `src/ui/WindowRuntime.cpp:2320 — WindowItemSelector_ReadScreenX` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1995 | 0x004C7280 | `sub_4C7280` | 有正文 | `src/ui/WindowRuntime.cpp:2336 — WindowItemSelector_ReadScreenY` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1996 | 0x004C72C0 | `sub_4C72C0` | 有正文 | `src/ui/WindowRuntime.cpp:2352 — WindowItemSelector_GetItemList_4C72C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1997 | 0x004C7340 | `sub_4C7340` | 有正文 | `src/ui/WindowRuntime.cpp:2136 — WindowItemSelector_SetMode` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1998 | 0x004C7380 | `sub_4C7380` | 有正文 | `src/ui/WindowRuntime.cpp:2154 — WindowItemSelector_HandleTab` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1999 | 0x004C75A0 | `sub_4C75A0` | 有正文 | `src/ui/WindowRuntime.cpp:2372 — WindowItemSelector_HandlePlayerAttributes` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2000 | 0x004C78A0 | `sub_4C78A0` | 有正文 | `src/ui/WindowRuntime.cpp:2231 — WindowItemSelector_TogglePreview` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2001 | 0x004C7910 | `sub_4C7910` | 有正文 | `src/ui/WindowRuntime.cpp:2245 — WindowItemSelector_CycleEquipment` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2002 | 0x004C7A10 | `sub_4C7A10` | 有正文 | `src/ui/WindowRuntime.cpp:2284 — WindowItemSelector_RotatePreview` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2003 | 0x004C7A90 | `sub_4C7A90` | 有正文 | `src/ui/WindowRuntime.cpp:2307 — WindowItemSelector_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2004 | 0x004C7AD0 | `sub_4C7AD0` | 有正文 | `src/ui/WindowRuntime.cpp:2937 — WindowRuntime_ResetShortcutFlag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2005 | 0x004C7AE0 | `sub_4C7AE0` | 有正文 | `src/ui/WindowRuntime.cpp:3011 — WindowRuntime_ProcessShortcuts` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2006 | 0x004C9060 | `sub_4C9060` | 有正文 | `src/ui/WindowRuntime.cpp:2733 — WindowRuntime_LoadSettings` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2007 | 0x004C9130 | `sub_4C9130` | 有正文 | `src/ui/WindowRuntime.cpp:2813 — WindowRuntime_SaveSettings` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2008 | 0x004C9170 | `sub_4C9170` | 有正文 | `src/ui/WindowRuntime.cpp:2698 — WindowRuntime_ApplySettings` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2009 | 0x004C9240 | `sub_4C9240` | 有正文 | `src/ui/WindowRuntime.cpp:2637 — WindowRuntime_ResetSettings` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2010 | 0x004C92B0 | `sub_4C92B0` | 有正文 | `src/ui/WindowRuntime.cpp:2770 — WindowRuntime_ClearSavedSettings` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2011 | 0x004C92D0 | `sub_4C92D0` | 有正文 | `src/ui/WindowRuntime.cpp:2777 — WindowRuntime_CaptureSettings` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2012 | 0x004C93A0 | `sub_4C93A0` | 有正文 | `src/ui/WindowRuntime.cpp:2658 — WindowRuntime_HasChangedDefaultPositions` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2013 | 0x004C93E0 | `sub_4C93E0` | 有正文 | `src/ui/WindowRuntime.cpp:2670 — WindowRuntime_RepairSettingsTail` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2014 | 0x004C9490 | `sub_4C9490` | 有正文 | `src/world/MailState.cpp:663 — Sub_4C9490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2015 | 0x004C94B0 | `sub_4C94B0` | 有正文 | `src/ui/windows/WindowOwnProfileCategory.cpp:119 — WindowOwnProfileCategory_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2016 | 0x004C94D6 | `?classic@locale@std@@SAABV12@XZ_20` | 有承接 | `src/ui/windows/WindowOwnProfileCategory.cpp:119 — WindowOwnProfileCategory_Update` | 原调用 4C9C70／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2017 | 0x004C94F0 | `sub_4C94F0` | 有正文 | `src/ui/windows/WindowOwnProfileCategory.cpp:132 — WindowOwnProfileCategory_SelectCategory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2018 | 0x004C96F0 | `sub_4C96F0` | 有正文 | `src/ui/windows/WindowOwnProfileCategory.cpp:179 — WindowOwnProfileCategory_HandleScrollButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2019 | 0x004C9BE0 | `sub_4C9BE0` | 有正文 | `src/ui/windows/WindowOwnProfileCategory.cpp:335 — WindowOwnProfileCategory_Scroll` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2020 | 0x004C9C70 | `sub_4C9C70` | 有正文 | `src/ui/windows/WindowOwnProfileCategory.cpp:96 — WindowOwnProfileCategory_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2021 | 0x004C9E40 | `sub_4C9E40` | 有正文 | `src/world/MailState.cpp:1072 — Sub_4C9E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2022 | 0x004C9EE0 | `sub_4C9EE0` | 有正文 | `src/ui/windows/WindowProfileCategory.cpp:233 — WindowProfileCategory_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2023 | 0x004C9F08 | `?classic@locale@std@@SAABV12@XZ_22` | 有承接 | `src/ui/windows/WindowProfileCategory.cpp:233 — WindowProfileCategory_Update` | 原调用空回调 509080／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2024 | 0x004C9F20 | `sub_4C9F20` | 有正文 | `src/ui/windows/WindowProfileCategory.cpp:209 — WindowProfileCategory_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2025 | 0x004CA1A0 | `sub_4CA1A0` | 有正文 | `src/ui/windows/WindowProfileCategory.cpp:271 — WindowProfileCategory_SelectCategory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2026 | 0x004CA300 | `sub_4CA300` | 有正文 | `src/ui/windows/WindowProfileCategory.cpp:245 — WindowProfileCategory_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2027 | 0x004CA330 | `sub_4CA330` | 有正文 | `src/world/MailState.cpp:1107 — Sub_4CA330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2028 | 0x004CA410 | `sub_4CA410` | 有正文 | `src/ui/windows/WindowProfileSearch.cpp:210 — WindowProfileSearch_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2029 | 0x004CA438 | `?classic@locale@std@@SAABV12@XZ_23` | 有承接 | `src/ui/windows/WindowProfileSearch.cpp:210 — WindowProfileSearch_Update` | 原调用 4CA450／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2030 | 0x004CA450 | `sub_4CA450` | 有正文 | `src/ui/windows/WindowProfileSearch.cpp:188 — WindowProfileSearch_DrawKeyword` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2031 | 0x004CA500 | `sub_4CA500` | 有正文 | `src/ui/windows/WindowProfileSearch.cpp:157 — WindowProfileSearch_RefreshResults` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2032 | 0x004CA6F0 | `sub_4CA6F0` | 有正文 | `src/ui/windows/WindowProfileSearch.cpp:255 — WindowProfileSearch_HandleSearchButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2033 | 0x004CA9C0 | `sub_4CA9C0` | 有正文 | `src/ui/windows/WindowProfileSearch.cpp:297 — WindowProfileSearch_SelectResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2034 | 0x004CAB60 | `sub_4CAB60` | 有正文 | `src/ui/windows/WindowProfileSearch.cpp:223 — WindowProfileSearch_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2035 | 0x004CAB90 | `sub_4CAB90` | 有正文 | `src/ui/windows/WindowProfileSearch.cpp:231 — WindowProfileSearch_EditKeyword` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2036 | 0x004CAC60 | `sub_4CAC60` | 有正文 | `src/world/MailState.cpp:1135 — Sub_4CAC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2037 | 0x004CAF60 | `sub_4CAF60` | 有正文 | `src/ui/windows/WindowProfileView.cpp:145 — WindowProfileView_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2038 | 0x004CAF88 | `?classic@locale@std@@SAABV12@XZ_24` | 有承接 | `src/ui/windows/WindowProfileView.cpp:145 — WindowProfileView_Update` | 原调用 4CAFA0／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2039 | 0x004CAFA0 | `sub_4CAFA0` | 有正文 | `src/ui/windows/WindowProfileView.cpp:111 — WindowProfileView_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2040 | 0x004CB2F0 | `sub_4CB2F0` | 有正文 | `src/ui/windows/WindowProfileView.cpp:172 — WindowProfileView_ButtonMail` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2041 | 0x004CB430 | `sub_4CB430` | 有正文 | `src/ui/windows/WindowProfileView.cpp:99 — WindowProfileView_ReadSelection` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2042 | 0x004CB4A0 | `sub_4CB4A0` | 有正文 | `src/ui/windows/WindowProfileCategory.cpp:254 — WindowProfileCategory_ResolveCategory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2043 | 0x004CB530 | `sub_4CB530` | 有正文 | `src/ui/windows/WindowProfileView.cpp:165 — WindowProfileView_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2044 | 0x004CB550 | `sub_4CB550` | 有正文 | `src/ui/windows/WindowMiniMail.cpp:94 — WindowMiniMail_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2045 | 0x004CB5D0 | `sub_4CB5D0` | 有正文 | `src/ui/windows/WindowMiniMail.cpp:76 — WindowMiniMail_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2046 | 0x004CB6A0 | `sub_4CB6A0` | 有正文 | `src/ui/windows/WindowMiniMail.cpp:137 — WindowMiniMail_Send` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2047 | 0x004CB920 | `sub_4CB920` | 有正文 | `src/ui/windows/WindowMiniMail.cpp:114 — WindowMiniMail_HandleInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2048 | 0x004CB9B0 | `sub_4CB9B0` | 有正文 | `src/ui/windows/WindowProfileView.cpp:155 — WindowProfileView_RequestCategoryClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2049 | 0x004CB9E0 | `?classic@locale@std@@SAABV12@XZ_25` | 有正文 | `src/ui/windows/WindowProfileReply.cpp:121 — WindowProfileReply_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2050 | 0x004CB9F0 | `sub_4CB9F0` | 有正文 | `src/ui/windows/WindowProfileReply.cpp:88 — WindowProfileReply_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2051 | 0x004CBD30 | `sub_4CBD30` | 有正文 | `src/ui/windows/WindowAdventureLog.cpp:86 — WindowAdventureLog_InitializeDate` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2052 | 0x004CBD90 | `sub_4CBD90` | 有正文 | `src/ui/windows/WindowAdventureLog.cpp:190 — WindowAdventureLog_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2053 | 0x004CBDB0 | `sub_4CBDB0` | 有正文 | `src/ui/windows/WindowAdventureLog.cpp:200 — WindowAdventureLog_HandleNavigation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2054 | 0x004CC340 | `sub_4CC340` | 有正文 | `src/ui/windows/WindowAdventureLog.cpp:153 — WindowAdventureLog_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2055 | 0x004CC540 | `sub_4CC540` | 有正文 | `src/ui/windows/WindowAdventureLog.cpp:110 — WindowAdventureLog_Load` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2056 | 0x004CC6A0 | `sub_4CC6A0` | 有正文 | `src/ui/windows/WindowBlacklist.cpp:159 — WindowBlacklist_ResetState` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2057 | 0x004CC6C0 | `sub_4CC6C0` | 有正文 | `src/ui/windows/WindowBlacklist.cpp:218 — WindowBlacklist_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2058 | 0x004CC6E0 | `sub_4CC6E0` | 有正文 | `src/ui/windows/WindowBlacklist.cpp:228 — WindowBlacklist_EditRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2059 | 0x004CC9B0 | `sub_4CC9B0` | 有正文 | `src/ui/windows/WindowBlacklist.cpp:286 — WindowBlacklist_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2060 | 0x004CCED0 | `sub_4CCED0` | 有正文 | `src/ui/windows/WindowBlacklist.cpp:168 — WindowBlacklist_Refresh` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2061 | 0x004CD150 | `sub_4CD150` | 有正文 | `src/ui/windows/WindowBlacklist.cpp:394 — WindowBlacklist_ConfirmClear` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2062 | 0x004CD1E0 | `sub_4CD1E0` | 有正文 | `src/ui/windows/WindowBlacklist.cpp:418 — WindowBlacklist_Clear` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2063 | 0x004CD220 | `sub_4CD220` | 有正文 | `src/ui/windows/WindowBlacklist.cpp:428 — WindowBlacklist_CancelClear` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2064 | 0x004CD240 | `sub_4CD240` | 有承接 | `src/ui/windows/WindowStall.cpp:2761 — WindowStall_ConstructBrowsePetValues` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 2065 | 0x004CD250 | `sub_4CD250` | 有承接 | `src/ui/windows/WindowStall.cpp:2761 — WindowStall_ConstructBrowsePetValues` | 固定成员构造／逆序销毁由实际数组 helper 承接；未记完整展开验收；完整业务／运行未验 |
| 2066 | 0x004CD2D0 | `sub_4CD2D0` | 有承接 | `src/world/WorldUpdateDependencies.hpp:696 — PetRuntimeRecord::~PetRuntimeRecord` | 主／浏览各独立数组及六成员自动逆序析构；旧回调适配退休；运行未验 |
| 2067 | 0x004CD360 | `sub_4CD360` | 有承接 | `src/world/WorldUpdateDependencies.hpp:694 — PetRuntimeRecord::PetRuntimeRecord` | 主／浏览五项各六成员由C++／std::array正序构造；实际存储独立，RNG序保持；运行未验 |
| 2068 | 0x004CD3E0 | `sub_4CD3E0` | 有正文 | `src/ui/windows/WindowStall.cpp:2769 — WindowStallBrowse_RequestClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2069 | 0x004CD400 | `sub_4CD400` | 有正文 | `src/ui/windows/WindowStall.cpp:2779 — WindowOwnStall_RequestClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2070 | 0x004CD420 | `sub_4CD420` | 有正文 | `src/ui/windows/WindowStall.cpp:2543 — WindowStall_ButtonClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2071 | 0x004CD4F0 | `sub_4CD4F0` | 有正文 | `src/ui/windows/WindowStall.cpp:1990 — WindowStall_Update` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2072 | 0x004CDB10 | `sub_4CDB10` | 有正文 | `src/ui/windows/WindowStall.cpp:2192 — WindowStall_Draw` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2073 | 0x004CDE00 | `sub_4CDE00` | 有正文 | `src/ui/windows/WindowStall.cpp:2568 — WindowStall_Release` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2074 | 0x004CDE40 | `sub_4CDE40` | 有正文 | `src/ui/windows/WindowStall.cpp:440 — WindowStall_ButtonPreviousListing` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2075 | 0x004CDFA0 | `sub_4CDFA0` | 有正文 | `src/ui/windows/WindowStall.cpp:483 — WindowStall_ButtonNextListing` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2076 | 0x004CE1F0 | `sub_4CE1F0` | 有正文 | `src/ui/windows/WindowStall.cpp:553 — WindowStall_ButtonPreviousPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2077 | 0x004CE2B0 | `sub_4CE2B0` | 有正文 | `src/ui/windows/WindowStall.cpp:583 — WindowStall_ButtonNextPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2078 | 0x004CE3F0 | `sub_4CE3F0` | 有正文 | `src/ui/windows/WindowStall.cpp:630 — WindowStall_TabCategory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2079 | 0x004CE700 | `sub_4CE700` | 有正文 | `src/ui/windows/WindowStall.cpp:2577 — WindowStall_ReceiveOpenResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2080 | 0x004CE8A0 | `sub_4CE8A0` | 有正文 | `src/ui/windows/WindowStall.cpp:719 — WindowStall_RequestClose` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2081 | 0x004CE8B0 | `sub_4CE8B0` | 有正文 | `src/ui/windows/WindowStall.cpp:2623 — WindowStall_ReceiveCloseResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2082 | 0x004CE930 | `sub_4CE930` | 有正文 | `src/ui/windows/WindowStall.cpp:2644 — WindowStall_ReceivePurchaseResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2083 | 0x004CECB0 | `sub_4CECB0` | 有正文 | `src/ui/windows/WindowStall.cpp:2516 — WindowStall_RequestPurchase` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2084 | 0x004CED50 | `sub_4CED50` | 有正文 | `src/ui/windows/WindowStall.cpp:725 — WindowStall_ShowPurchaseConfirmation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2085 | 0x004CED60 | `sub_4CED60` | 有正文 | `src/ui/windows/WindowStall.cpp:2302 — WindowStall_OpenBrowse` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2086 | 0x004CF8A0 | `sub_4CF8A0` | 有正文 | `src/ui/windows/WindowStall.cpp:2496 — WindowStall_BuildItemDescription` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2087 | 0x004CF940 | `sub_4CF940` | 有正文 | `src/ui/windows/WindowStall.cpp:810 — WindowStall_HandleButtons` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2088 | 0x004CFC40 | `sub_4CFC40` | 有正文 | `src/ui/windows/WindowStall.cpp:733 — WindowStall_EditItemPrice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2089 | 0x004CFCA0 | `sub_4CFCA0` | 有正文 | `src/ui/windows/WindowStall.cpp:747 — WindowStall_EditPetPrice` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2090 | 0x004CFD70 | `sub_4CFD70` | 有正文 | `src/ui/windows/WindowStall.cpp:767 — WindowStall_RemoveListing` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2091 | 0x004CFE70 | `sub_4CFE70` | 有正文 | `src/ui/windows/WindowStall.cpp:951 — WindowStall_SubmitListings` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2092 | 0x004D0230 | `sub_4D0230` | 有正文 | `src/ui/windows/WindowStall.cpp:1040 — WindowStall_DrawTooltipOverlay` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2093 | 0x004D04A0 | `sub_4D04A0` | 有正文 | `src/ui/windows/WindowStall.cpp:1476 — WindowStall_HandleItemRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2094 | 0x004D0990 | `sub_4D0990` | 有正文 | `src/ui/windows/WindowStall.cpp:1081 — WindowStall_DrawPetSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2095 | 0x004D0A80 | `sub_4D0A80` | 有正文 | `src/ui/windows/WindowStall.cpp:1107 — WindowStall_DrawPetDetails` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2096 | 0x004D0EB0 | `sub_4D0EB0` | 有正文 | `src/ui/windows/WindowStall.cpp:1179 — WindowStall_HandlePetRow` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2097 | 0x004D16F0 | `sub_4D16F0` | 有正文 | `src/ui/windows/WindowStall.cpp:1362 — WindowStall_HandlePetCandidate` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2098 | 0x004D1C00 | `sub_4D1C00` | 有正文 | `src/ui/windows/WindowStall.cpp:1332 — WindowStall_DrawCandidateAttributes` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2099 | 0x004D1D50 | `sub_4D1D50` | 有正文 | `src/ui/windows/WindowStall.cpp:1626 — WindowStall_HandleInventory` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2100 | 0x004D2260 | `sub_4D2260` | 有正文 | `src/ui/windows/WindowStall.cpp:1714 — WindowStall_HandlePriceInput` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2101 | 0x004D27C0 | `sub_4D27C0` | 有正文 | `src/ui/windows/WindowStall.cpp:692 — WindowStall_AddListing` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2102 | 0x004D2840 | `sub_4D2840` | 有正文 | `src/ui/windows/WindowStall.cpp:1830 — WindowStall_HandlePurchaseConfirmation` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2103 | 0x004D3020 | `sub_4D3020` | 有正文 | `src/ui/windows/WindowProfile.cpp:129 — WindowProfile_SaveOrigin` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2104 | 0x004D3040 | `sub_4D3040` | 有正文 | `src/ui/windows/WindowProfile.cpp:137 — WindowProfile_CheckDistance` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2105 | 0x004D3110 | `sub_4D3110` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2106 | 0x004D3170 | `sub_4D3170` | 有正文 | `src/world/SceneState.cpp:7824 — Sub_4D3170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2107 | 0x004D3180 | `sub_4D3180` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2108 | 0x004D31C0 | `sub_4D31C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2109 | 0x004D3210 | `sub_4D3210` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2110 | 0x004D3410 | `sub_4D3410` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2111 | 0x004D36B0 | `sub_4D36B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2112 | 0x004D36F0 | `sub_4D36F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2113 | 0x004D4960 | `sub_4D4960` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2114 | 0x004D4A50 | `sub_4D4A50` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2115 | 0x004D4ED0 | `sub_4D4ED0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2116 | 0x004D4F00 | `sub_4D4F00` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2117 | 0x004D5220 | `sub_4D5220` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2118 | 0x004D5250 | `sub_4D5250` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2119 | 0x004D5580 | `sub_4D5580` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2120 | 0x004D55C0 | `sub_4D55C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2121 | 0x004D61E0 | `sub_4D61E0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2122 | 0x004D62A0 | `sub_4D62A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2123 | 0x004D6640 | `sub_4D6640` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2124 | 0x004D6650 | `nullsub_3` | 空方法 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 历史父Sub_4D6640已移除` | 原单RET仍按用户决定标为空方法，不再核职责、不补CPP包装；本次仅清理无消费者父实现，未改该入口状态 |
| 2125 | 0x004D6690 | `sub_4D6690` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2126 | 0x004D66E0 | `sub_4D66E0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2127 | 0x004D6710 | `sub_4D6710` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2128 | 0x004D6A30 | `sub_4D6A30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2129 | 0x004D6A60 | `sub_4D6A60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2130 | 0x004D6E70 | `sub_4D6E70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2131 | 0x004D6EA0 | `sub_4D6EA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2132 | 0x004D7440 | `sub_4D7440` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2133 | 0x004D7470 | `sub_4D7470` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2134 | 0x004D7BF0 | `sub_4D7BF0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2135 | 0x004D7C20 | `sub_4D7C20` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2136 | 0x004D8200 | `sub_4D8200` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2137 | 0x004D8230 | `sub_4D8230` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2138 | 0x004D88C0 | `sub_4D88C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2139 | 0x004D8D70 | `sub_4D8D70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2140 | 0x004D8DA0 | `sub_4D8DA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2141 | 0x004D91E0 | `sub_4D91E0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2142 | 0x004D9220 | `sub_4D9220` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2143 | 0x004DA400 | `sub_4DA400` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2144 | 0x004DA580 | `sub_4DA580` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2145 | 0x004DAA00 | `sub_4DAA00` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2146 | 0x004DAA40 | `sub_4DAA40` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2147 | 0x004DBD30 | `sub_4DBD30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2148 | 0x004DC290 | `sub_4DC290` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2149 | 0x004DC7A0 | `sub_4DC7A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2150 | 0x004DC7E0 | `sub_4DC7E0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2151 | 0x004DDA30 | `sub_4DDA30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2152 | 0x004DDB20 | `sub_4DDB20` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2153 | 0x004DDF80 | `sub_4DDF80` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2154 | 0x004DDFC0 | `sub_4DDFC0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2155 | 0x004DF210 | `sub_4DF210` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2156 | 0x004DF300 | `sub_4DF300` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2157 | 0x004DF730 | `sub_4DF730` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2158 | 0x004E0100 | `sub_4E0100` | 有正文 | `src/ui/windows/WindowTrade.cpp:2721 — WindowTrade_CopyDisplayItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2159 | 0x004E0270 | `sub_4E0270` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2160 | 0x004E02B0 | `sub_4E02B0` | 有正文 | `src/ui/windows/WindowBank.cpp:212 — WindowItem_BuildStorageDescription` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2161 | 0x004E0380 | `sub_4E0380` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2162 | 0x004E0BC0 | `sub_4E0BC0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2163 | 0x004E0C70 | `sub_4E0C70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2164 | 0x004E0D10 | `sub_4E0D10` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2165 | 0x004E0FF0 | `sub_4E0FF0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2166 | 0x004E10F0 | `sub_4E10F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2167 | 0x004E1840 | `sub_4E1840` | 有正文 | `src/ui/windows/WindowTrade.cpp:509 — WindowTrade_SwapDisplayItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2168 | 0x004E18F0 | `sub_4E18F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2169 | 0x004E1980 | `sub_4E1980` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2170 | 0x004E19E0 | `sub_4E19E0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2171 | 0x004E1B10 | `sub_4E1B10` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2172 | 0x004E1EE0 | `sub_4E1EE0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2173 | 0x004E2030 | `sub_4E2030` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2174 | 0x004E28F0 | `sub_4E28F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2175 | 0x004E2D90 | `sub_4E2D90` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2176 | 0x004E3750 | `sub_4E3750` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2177 | 0x004E4020 | `sub_4E4020` | 有正文 | `src/ui/windows/WindowItemUse.cpp:258 — WindowItemUse_ReceiveAttributeTargets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2178 | 0x004E4460 | `sub_4E4460` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2179 | 0x004E4860 | `sub_4E4860` | 有正文 | `src/ui/windows/WindowItemUse.cpp:143 — WindowItemUse_ReceiveTextTargets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2180 | 0x004E4A50 | `sub_4E4A50` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2181 | 0x004E4E60 | `sub_4E4E60` | 有正文 | `src/ui/windows/WindowTrade.cpp:2708 — WindowTrade_RequestTargetList` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2182 | 0x004E4E80 | `sub_4E4E80` | 有正文 | `src/ui/windows/WindowTrade.cpp:2715 — WindowTrade_OpenTargetList` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2183 | 0x004E4EA0 | `sub_4E4EA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2184 | 0x004E52B0 | `sub_4E52B0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2754 — WindowTrade_CopyOwnPet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2185 | 0x004E54E0 | `sub_4E54E0` | 有正文 | `src/ui/windows/WindowTrade.cpp:552 — WindowTrade_CopyOwnItem` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2186 | 0x004E5600 | `sub_4E5600` | 有正文 | `src/ui/windows/WindowTrade.cpp:582 — WindowTrade_SyncOwnItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2187 | 0x004E5620 | `sub_4E5620` | 有正文 | `src/ui/windows/WindowTrade.cpp:2682 — WindowTrade_ReceiveStart` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2188 | 0x004E5710 | `sub_4E5710` | 有正文 | `src/ui/windows/WindowTrade.cpp:2817 — WindowTrade_ResetRemoteItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2189 | 0x004E5770 | `sub_4E5770` | 有正文 | `src/ui/windows/WindowTrade.cpp:2830 — WindowTrade_BuildRemoteItemDescription` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2190 | 0x004E5830 | `sub_4E5830` | 有正文 | `src/ui/windows/WindowTrade.cpp:2852 — WindowTrade_ReceiveRemoteItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2191 | 0x004E5A40 | `sub_4E5A40` | 有正文 | `src/ui/windows/WindowTrade.cpp:2889 — WindowTrade_ReceiveRemotePet` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2192 | 0x004E5CD0 | `sub_4E5CD0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2934 — WindowTrade_ReceiveRemotePetSkills` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2193 | 0x004E5DA0 | `sub_4E5DA0` | 有正文 | `src/ui/windows/WindowTrade.cpp:494 — WindowTrade_SwapRemoteItems` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2194 | 0x004E5E40 | `sub_4E5E40` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2195 | 0x004E6CC0 | `sub_4E6CC0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2196 | 0x004E6D70 | `sub_4E6D70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2197 | 0x004E6E10 | `sub_4E6E10` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2198 | 0x004E70B0 | `sub_4E70B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2199 | 0x004E7140 | `sub_4E7140` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2200 | 0x004E71A0 | `sub_4E71A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2201 | 0x004E7290 | `sub_4E7290` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2202 | 0x004E7380 | `sub_4E7380` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2203 | 0x004E7740 | `sub_4E7740` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2204 | 0x004E7870 | `sub_4E7870` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2205 | 0x004E7FC0 | `sub_4E7FC0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2206 | 0x004E81B0 | `sub_4E81B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2207 | 0x004E82F0 | `sub_4E82F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2208 | 0x004E83D0 | `sub_4E83D0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2209 | 0x004E8920 | `sub_4E8920` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2210 | 0x004E8AD0 | `sub_4E8AD0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2211 | 0x004E9280 | `sub_4E9280` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2212 | 0x004E92A0 | `sub_4E92A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2213 | 0x004E97C0 | `sub_4E97C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2214 | 0x004E97E0 | `sub_4E97E0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2215 | 0x004E9C60 | `sub_4E9C60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2216 | 0x004E9C80 | `sub_4E9C80` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2217 | 0x004EA130 | `sub_4EA130` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2218 | 0x004EA1B0 | `sub_4EA1B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2219 | 0x004EA670 | `sub_4EA670` | 有正文 | `src/ui/WindowRuntime.cpp:1038 — WindowPet_HasSkill25600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2220 | 0x004EA690 | `sub_4EA690` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2221 | 0x004EAA50 | `sub_4EAA50` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2222 | 0x004EAAA0 | `sub_4EAAA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2223 | 0x004EB120 | `sub_4EB120` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2224 | 0x004EB1C0 | `sub_4EB1C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2225 | 0x004EB890 | `sub_4EB890` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2226 | 0x004EBAA0 | `sub_4EBAA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2227 | 0x004EBD20 | `sub_4EBD20` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2228 | 0x004EBF70 | `sub_4EBF70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2229 | 0x004EBFD0 | `sub_4EBFD0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2230 | 0x004EC030 | `sub_4EC030` | 有正文 | `src/world/MailState.cpp:159 — CompareContactIndices_4EC030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2231 | 0x004EC1A0 | `sub_4EC1A0` | 有正文 | `src/world/MailState.cpp:258 — Sub_4EC1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2232 | 0x004EC1D0 | `sub_4EC1D0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2233 | 0x004EC240 | `sub_4EC240` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2234 | 0x004ECDF0 | `sub_4ECDF0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2235 | 0x004ECE00 | `sub_4ECE00` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2236 | 0x004ED010 | `sub_4ED010` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2237 | 0x004ED020 | `sub_4ED020` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2238 | 0x004ED670 | `sub_4ED670` | 有正文 | `src/world/SceneState.cpp:5506 — Sub_4ED670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2239 | 0x004ED6B0 | `sub_4ED6B0` | 有正文 | `src/ui/windows/WindowGather.cpp:382 — WindowGather_ReceiveResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2240 | 0x004EDB20 | `sub_4EDB20` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2241 | 0x004EE130 | `sub_4EE130` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2242 | 0x004EE380 | `sub_4EE380` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2243 | 0x004EE670 | `sub_4EE670` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2244 | 0x004EE9E0 | `sub_4EE9E0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2245 | 0x004EEA40 | `sub_4EEA40` | 有正文 | `src/ui/windows/WindowSkillResult.cpp:333 — WindowSkillResult_ReceiveResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2246 | 0x004EF0F0 | `sub_4EF0F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2247 | 0x004EF5F0 | `sub_4EF5F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2248 | 0x004EF710 | `sub_4EF710` | 有正文 | `src/ui/windows/WindowSkill.cpp:666 — WindowSkill_ClassifyAttributeSkill` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2249 | 0x004EF750 | `sub_4EF750` | 有正文 | `src/ui/windows/WindowSkill.cpp:565 — WindowSkill_ReceiveAttributeTargets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2250 | 0x004EFB70 | `sub_4EFB70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2251 | 0x004F0200 | `sub_4F0200` | 有正文 | `src/ui/windows/WindowSkill.cpp:2478 — WindowSkill_ReceiveTextTargets` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2252 | 0x004F0460 | `sub_4F0460` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2253 | 0x004F0B40 | `sub_4F0B40` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2254 | 0x004F0CD0 | `sub_4F0CD0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2255 | 0x004F0E20 | `sub_4F0E20` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2256 | 0x004F0F70 | `sub_4F0F70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2257 | 0x004F1800 | `sub_4F1800` | 有正文 | `src/ui/windows/WindowCraft.cpp:3472 — WindowCraft_ReceiveItemWorkResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2258 | 0x004F1C40 | `sub_4F1C40` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2259 | 0x004F2130 | `sub_4F2130` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2260 | 0x004F24A0 | `sub_4F24A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2261 | 0x004F25C0 | `sub_4F25C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2262 | 0x004F2650 | `sub_4F2650` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2263 | 0x004F2720 | `sub_4F2720` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2264 | 0x004F2A30 | `sub_4F2A30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2265 | 0x004F39F0 | `sub_4F39F0` | 有正文 | `src/ui/windows/WindowCraft.cpp:3245 — WindowCraft_ReceiveRecipeResult` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2266 | 0x004F3ED0 | `sub_4F3ED0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2267 | 0x004F4430 | `sub_4F4430` | 有正文 | `src/ui/windows/WindowCraft.cpp:1326 — WindowCraft_ClassifySpecialMaterial` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2268 | 0x004F4470 | `sub_4F4470` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2269 | 0x004F4630 | `sub_4F4630` | 有正文 | `src/ui/windows/WindowCraft.cpp:319 — WindowCraft_CountRecipeMaterial` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2270 | 0x004F4690 | `sub_4F4690` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2271 | 0x004F4720 | `sub_4F4720` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2272 | 0x004F4740 | `sub_4F4740` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2273 | 0x004F4760 | `sub_4F4760` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2274 | 0x004F4790 | `sub_4F4790` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2275 | 0x004F50D0 | `sub_4F50D0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2276 | 0x004F5170 | `sub_4F5170` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2277 | 0x004F51A0 | `sub_4F51A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2278 | 0x004F51D0 | `sub_4F51D0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2279 | 0x004F5A60 | `sub_4F5A60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2280 | 0x004F61C0 | `sub_4F61C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2281 | 0x004F6220 | `sub_4F6220` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2282 | 0x004F6BF0 | `sub_4F6BF0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2283 | 0x004F6C60 | `sub_4F6C60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2284 | 0x004F6CA0 | `sub_4F6CA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2285 | 0x004F7140 | `sub_4F7140` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2286 | 0x004F7150 | `sub_4F7150` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2287 | 0x004F7870 | `sub_4F7870` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2288 | 0x004F8340 | `sub_4F8340` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2289 | 0x004F83B0 | `sub_4F83B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2290 | 0x004F8550 | `sub_4F8550` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2291 | 0x004F8E40 | `sub_4F8E40` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2292 | 0x004F9B10 | `sub_4F9B10` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2293 | 0x004F9FF0 | `sub_4F9FF0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2294 | 0x004FA050 | `sub_4FA050` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2295 | 0x004FA350 | `sub_4FA350` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2296 | 0x004FA8E0 | `sub_4FA8E0` | 有正文 | `src/world/SceneState.cpp:8528 — Sub_4FA8E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2297 | 0x004FACE0 | `sub_4FACE0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2298 | 0x004FAED0 | `sub_4FAED0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2299 | 0x004FAEF0 | `sub_4FAEF0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2300 | 0x004FB8F0 | `MouseEvent::resetDrag` | 有正文 | `src/ui/WindowRuntime.cpp:873 — WindowItem_ResetItemDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2301 | 0x004FB920 | `sub_4FB920` | 有正文 | `src/ui/WindowRuntime.cpp:885 — WindowItem_CancelMatchingItemDrag` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2302 | 0x004FB940 | `sub_4FB940` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2303 | 0x004FBA30 | `sub_4FBA30` | 有正文 | `src/ui/WindowRuntime.cpp:1482 — WindowItem_SplitTooltipLines` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2304 | 0x004FBB30 | `sub_4FBB30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2305 | 0x004FBB40 | `sub_4FBB40` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2306 | 0x004FC420 | `sub_4FC420` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2307 | 0x004FC430 | `sub_4FC430` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2308 | 0x004FC7F0 | `sub_4FC7F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2309 | 0x004FC810 | `sub_4FC810` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2310 | 0x004FC830 | `sub_4FC830` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2311 | 0x004FC850 | `sub_4FC850` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2312 | 0x004FC870 | `sub_4FC870` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2313 | 0x004FC890 | `sub_4FC890` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2314 | 0x004FC8B0 | `sub_4FC8B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2315 | 0x004FC8D0 | `sub_4FC8D0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2316 | 0x004FC8F0 | `sub_4FC8F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2317 | 0x004FCB90 | `sub_4FCB90` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2318 | 0x004FCBA0 | `sub_4FCBA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2319 | 0x004FD1A0 | `sub_4FD1A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2320 | 0x004FD4C0 | `sub_4FD4C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2321 | 0x004FD510 | `sub_4FD510` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2322 | 0x004FD550 | `sub_4FD550` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2323 | 0x004FD5A0 | `sub_4FD5A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2324 | 0x004FD5C0 | `sub_4FD5C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2325 | 0x004FD5F0 | `sub_4FD5F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2326 | 0x004FD640 | `sub_4FD640` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2327 | 0x004FD690 | `sub_4FD690` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2328 | 0x004FD6A0 | `sub_4FD6A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2329 | 0x004FD9F0 | `sub_4FD9F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2330 | 0x004FDA00 | `sub_4FDA00` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2331 | 0x004FDBD0 | `sub_4FDBD0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2332 | 0x004FDD70 | `sub_4FDD70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2333 | 0x004FDDC0 | `sub_4FDDC0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2334 | 0x004FDE10 | `sub_4FDE10` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2335 | 0x004FDE20 | `sub_4FDE20` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2336 | 0x004FE0E0 | `sub_4FE0E0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2337 | 0x004FE320 | `sub_4FE320` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2338 | 0x004FE370 | `sub_4FE370` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2339 | 0x004FE3B0 | `sub_4FE3B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2340 | 0x004FE3E0 | `sub_4FE3E0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2341 | 0x004FE3F0 | `sub_4FE3F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2342 | 0x004FE440 | `sub_4FE440` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2343 | 0x004FE490 | `sub_4FE490` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2344 | 0x004FE4A0 | `sub_4FE4A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2345 | 0x004FE860 | `sub_4FE860` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2346 | 0x004FEAC0 | `sub_4FEAC0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2347 | 0x004FEB00 | `sub_4FEB00` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2348 | 0x004FEB10 | `sub_4FEB10` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2349 | 0x004FEF30 | `sub_4FEF30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2350 | 0x004FEFF0 | `sub_4FEFF0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2351 | 0x004FF020 | `sub_4FF020` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2352 | 0x004FF030 | `sub_4FF030` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2353 | 0x004FF1A0 | `sub_4FF1A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2354 | 0x004FF310 | `sub_4FF310` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2355 | 0x004FF340 | `sub_4FF340` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2356 | 0x004FF350 | `sub_4FF350` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2357 | 0x004FF4C0 | `sub_4FF4C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2358 | 0x004FF620 | `sub_4FF620` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2359 | 0x004FF650 | `sub_4FF650` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2360 | 0x00500370 | `sub_500370` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2361 | 0x005006F0 | `sub_5006F0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2362 | 0x005008C0 | `sub_5008C0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2363 | 0x005014A0 | `sub_5014A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2364 | 0x00501F60 | `sub_501F60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2365 | 0x00501FD0 | `sub_501FD0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2366 | 0x00502930 | `sub_502930` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2367 | 0x00502940 | `sub_502940` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2368 | 0x005029B0 | `sub_5029B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2369 | 0x00502B80 | `sub_502B80` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2370 | 0x00502C30 | `sub_502C30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2371 | 0x00502C40 | `sub_502C40` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2372 | 0x00502C60 | `sub_502C60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2373 | 0x00502C70 | `sub_502C70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2374 | 0x00502C80 | `sub_502C80` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2375 | 0x00502C90 | `sub_502C90` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2376 | 0x00502CA0 | `sub_502CA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2377 | 0x00502CB0 | `sub_502CB0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2378 | 0x00502D20 | `sub_502D20` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2379 | 0x00502D30 | `sub_502D30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2380 | 0x00502DA0 | `sub_502DA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2381 | 0x00502DB0 | `sub_502DB0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2382 | 0x00502E20 | `sub_502E20` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2383 | 0x00502E30 | `sub_502E30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2384 | 0x00502EA0 | `sub_502EA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2385 | 0x00502EB0 | `sub_502EB0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2386 | 0x00502F20 | `sub_502F20` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2387 | 0x00502F30 | `sub_502F30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2388 | 0x00502FA0 | `sub_502FA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2389 | 0x00502FB0 | `sub_502FB0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2390 | 0x00503020 | `sub_503020` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2391 | 0x00503030 | `sub_503030` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2392 | 0x005030A0 | `sub_5030A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2393 | 0x00503120 | `sub_503120` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2394 | 0x00503160 | `sub_503160` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2395 | 0x00503440 | `sub_503440` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2396 | 0x00503590 | `sub_503590` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2397 | 0x005035E0 | `sub_5035E0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2398 | 0x005036F0 | `sub_5036F0` | 有正文 | `src/LoginInput.cpp:201 — QueryLoginControlState_5036F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2399 | 0x00503780 | `sub_503780` | 有正文 | `src/LoginInput.cpp:255 — DrawLoginControl_503780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2400 | 0x005038B0 | `sub_5038B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2401 | 0x00503990 | `sub_503990` | 有正文 | `src/CharacterModalButton.cpp:17 — Sub_503990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2402 | 0x00503A20 | `sub_503A20` | 有正文 | `src/CharacterModalButton.cpp:72 — Sub_503A20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2403 | 0x00503AB0 | `sub_503AB0` | 有正文 | `src/input/Input.cpp:748 — ExtractLegacyLine_503AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2404 | 0x00503B70 | `sub_503B70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2405 | 0x00503BC0 | `sub_503BC0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2406 | 0x00503BE0 | `sub_503BE0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2407 | 0x00503C30 | `sub_503C30` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2408 | 0x00503C40 | `sub_503C40` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2409 | 0x00503C50 | `sub_503C50` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2410 | 0x00503C60 | `sub_503C60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2411 | 0x00503C70 | `sub_503C70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2412 | 0x00503C80 | `sub_503C80` | 有正文 | `src/render/SdlPresenter.cpp:528 — TakeScreenshotThrottled_503C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2413 | 0x00503CB0 | `sub_503CB0` | 有正文 | `src/ui/WindowRuntime.cpp:2611 — WindowRuntime_CycleNameMode` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2414 | 0x00503CD0 | `sub_503CD0` | 有正文 | `src/ui/WindowRuntime.cpp:2623 — WindowRuntime_IncrementNameMode` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2415 | 0x00503E40 | `sub_503E40` | 有正文 | `src/input/Input.cpp:878 — IsCurrentContext_503E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2416 | 0x00503E60 | `sub_503E60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2417 | 0x00503E80 | `sub_503E80` | 有正文 | `src/ui/WindowRuntime.cpp:4750 — WindowRuntime_ProcessScene` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2418 | 0x00504100 | `sub_504100` | 有正文 | `src/world/SceneState.cpp:9669 — ResetSceneUiState_504100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2419 | 0x00504120 | `Window::Render` | 有正文 | `src/ui/WindowRuntime.cpp:4359 — WindowRuntime_Render` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2420 | 0x00504180 | `sub_504180` | 已排除 | `doc/cg2d_legacy_window_simplification_evidence.json` | LWR03：用户接受移除无显示链的旧选择／End残留；原身份保留，不计实现或验收 |
| 2421 | 0x005042D0 | `sub_5042D0` | 已排除 | `doc/cg2d_legacy_window_simplification_evidence.json` | LWR03：用户接受移除无显示链的旧选择／End残留；原身份保留，不计实现或验收 |
| 2422 | 0x00504380 | `sub_504380` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2423 | 0x005043D0 | `sub_5043D0` | 已排除 | `doc/cg2d_legacy_window_simplification_evidence.json` | LWR03：用户接受移除无显示链的旧选择／End残留；原身份保留，不计实现或验收 |
| 2424 | 0x00504410 | `sub_504410` | 已排除 | `doc/cg2d_legacy_window_simplification_evidence.json` | LWR03：用户接受移除无显示链的旧选择／End残留；原身份保留，不计实现或验收 |
| 2425 | 0x00504460 | `sub_504460` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2426 | 0x005044C0 | `sub_5044C0` | 已排除 | `doc/cg2d_legacy_window_simplification_evidence.json` | LWR03：用户接受移除无显示链的旧选择／End残留；原身份保留，不计实现或验收 |
| 2427 | 0x005044E0 | `sub_5044E0` | 有正文 | `src/Yobi/window.cpp:34488 — CreateModalAction_5044E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2428 | 0x005046D0 | `sub_5046D0` | 有正文 | `src/Yobi/window.cpp:1337 — UpdateModalAction_5046D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2429 | 0x005049A0 | `sub_5049A0` | 有正文 | `src/LegacyHitTest.cpp:12 — Sub_5049A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2430 | 0x00504A30 | `sub_504A30` | 有正文 | `src/world/SceneState.cpp:9650 — Sub_504A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2431 | 0x00504A50 | `sub_504A50` | 有正文 | `src/world/SceneState.cpp:5523 — Sub_504A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2432 | 0x00504AF0 | `sub_504AF0` | 已排除 | `doc/cg2d_legacy_window_simplification_evidence.json` | LWR03：用户接受移除无显示链的旧选择／End残留；原身份保留，不计实现或验收 |
| 2433 | 0x00504B10 | `sub_504B10` | 有正文 | `src/world/SceneState.cpp:7890 — Sub_504B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2434 | 0x00504B60 | `sub_504B60` | 有正文 | `src/world/SceneState.cpp:7906 — Sub_504B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2435 | 0x00504BA0 | `sub_504BA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2436 | 0x00504C50 | `sub_504C50` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2437 | 0x00504EE0 | `sub_504EE0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2438 | 0x00504F10 | `sub_504F10` | 有正文 | `src/world/SceneState.cpp:2873 — Sub_504F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2439 | 0x00504FA0 | `sub_504FA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2440 | 0x00505040 | `sub_505040` | 有正文 | `src/world/SceneState.cpp:2902 — Sub_505040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2441 | 0x005050B0 | `sub_5050B0` | 有正文 | `src/world/SceneState.cpp:2919 — Sub_5050B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2442 | 0x00505110 | `sub_505110` | 有正文 | `src/world/SceneState.cpp:2935 — Sub_505110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2443 | 0x00505170 | `sub_505170` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2444 | 0x00505BC0 | `sub_505BC0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2445 | 0x00505C20 | `sub_505C20` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2446 | 0x005062A0 | `sub_5062A0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2447 | 0x00506340 | `sub_506340` | 有承接 | `src/platform/RuntimeInitialization.cpp:290 — cg::platform::gLegacyMouseState` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 2448 | 0x00506350 | `unknown_libname_47` | 有承接 | `src/platform/RuntimeInitialization.cpp:290 — cg::platform::gLegacyMouseState` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 2449 | 0x00506530 | `sub_506530` | 有承接 | `include/cg/platform/LegacyMouse.hpp:17 — LegacyMouseState成员自动失败前缀清理／逆序析构` | 原完成前缀清理由原生聚合构造异常规则及成员析构承接；同一存储，原地址保留；原SEH／二次异常／完整owner与运行未验 |
| 2450 | 0x00506580 | `sub_506580` | 已排除 | `doc/cg2d_code_organization_review_spec.md — COR10：追踪池重建移除` | 用户要求取消重复点击追踪；不计功能实现通过 |
| 2451 | 0x005066D0 | `sub_5066D0` | 已排除 | `doc/cg2d_code_organization_review_spec.md — COR10：追踪器默认重置移除` | 用户要求取消重复点击追踪；不计功能实现通过 |
| 2452 | 0x005066F0 | `sub_5066F0` | 已排除 | `doc/cg2d_code_organization_review_spec.md — COR10：追踪节点插入移除` | 用户要求取消重复点击追踪；不计功能实现通过 |
| 2453 | 0x00506720 | `sub_506720` | 已排除 | `doc/cg2d_code_organization_review_spec.md — COR10：追踪节点摘除移除` | 用户要求取消重复点击追踪；不计功能实现通过 |
| 2454 | 0x00506760 | `sub_506760` | 已排除 | `doc/cg2d_code_organization_review_spec.md — COR10：空闲节点取得移除` | 用户要求取消重复点击追踪；不计功能实现通过 |
| 2455 | 0x00506780 | `sub_506780` | 已排除 | `doc/cg2d_code_organization_review_spec.md — COR10：活动节点淘汰移除` | 用户要求取消重复点击追踪；不计功能实现通过 |
| 2456 | 0x005067A0 | `sub_5067A0` | 已排除 | `doc/cg2d_code_organization_review_spec.md — COR10：追踪节点分配移除` | 用户要求取消重复点击追踪；不计功能实现通过 |
| 2457 | 0x005067F0 | `sub_5067F0` | 已排除 | `doc/cg2d_code_organization_review_spec.md — COR10：重复点击计数移除` | 用户要求取消重复点击追踪；不计功能实现通过 |
| 2458 | 0x00506870 | `sub_506870` | 有正文 | `src/platform/LegacyMouse.cpp:356 — ResetLegacyMouseState_506870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2459 | 0x00506890 | `MouseEvent::Reset_0` | 有正文 | `src/platform/LegacyMouse.cpp:374 — ResetLegacyMousePosition_506890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2460 | 0x00506900 | `MouseEvent::UpdateLDown` | 有正文 | `src/platform/LegacyMouse.cpp:36 — UpdateLegacyMouseLeftDown_506900` | COR10保留普通状态更新，移除追踪／上报并改void；运行未验 |
| 2461 | 0x005069E0 | `MouseEvent::UpdateLUp` | 有正文 | `src/platform/LegacyMouse.cpp:410 — UpdateLegacyMouseLeftUp_5069E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2462 | 0x00506A90 | `MouseEvent::UpdateLDbClick` | 有正文 | `src/platform/LegacyMouse.cpp:431 — UpdateLegacyMouseLeftDoubleClick_506A90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2463 | 0x00506B50 | `MouseEvent::UpdateRDown` | 有正文 | `src/platform/LegacyMouse.cpp:100 — UpdateLegacyMouseRightDown_506B50` | COR10保留普通状态更新，移除追踪／上报并改void；运行未验 |
| 2464 | 0x00506C40 | `sub_506C40` | 有正文 | `src/platform/LegacyMouse.cpp:481 — UpdateLegacyMouseRightUp_506C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2465 | 0x00506CF0 | `sub_506CF0` | 有正文 | `src/platform/LegacyMouse.cpp:502 — UpdateLegacyMouseRightDoubleClick_506CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2466 | 0x00506DB0 | `sub_506DB0` | 有正文 | `src/platform/LegacyMouse.cpp:524 — AddLegacyMouseWheelDelta_506DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2467 | 0x00506DD0 | `sub_506DD0` | 有正文 | `src/platform/LegacyCursor.cpp:185 — ResetLegacyMouseTail_506DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2468 | 0x00506DE0 | `sub_506DE0` | 有正文 | `src/platform/LegacyCursor.cpp:166 — RequestCursorState_506DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2469 | 0x00506E00 | `World::CursorUpdate` | 有正文 | `src/platform/LegacyCursor.cpp:191 — UpdateCursor_506E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2470 | 0x00507090 | `sub_507090` | 有正文 | `src/render/GraphicPendingQueue.cpp:132 — DrawMatchingGraphicHover_507090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2471 | 0x00507140 | `sub_507140` | 有正文 | `src/platform/MouseDispatch.cpp:208 — Sub_507140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2472 | 0x00507C80 | `sub_507C80` | 有正文 | `src/platform/CommandOptions.cpp:248 — ParseIpOptions_507C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2473 | 0x00507D90 | `GetServerAddressInfo` | 有正文 | `src/LoginScene.cpp:329 — GetServerAddressInfo_507D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2474 | 0x00507DF0 | `NetworkLoop` | 有正文 | `src/network/NetworkRuntime.cpp:4552 — NetworkLoop_507DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2475 | 0x00508140 | `sub_508140` | 有正文 | `src/network/NetworkRuntime.cpp:4527 — InitializeNetworkRuntime_508140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2476 | 0x005081A0 | `sub_5081A0` | 有正文 | `src/network/NetworkRuntime.cpp:4800 — ShutdownNetworkRuntime_5081A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2477 | 0x005081E0 | `sub_5081E0` | 有正文 | `src/network/NetworkRuntime.cpp:4708 — AppendReceiveQueue_5081E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2478 | 0x00508230 | `sub_508230` | 有正文 | `src/network/NetworkRuntime.cpp:4732 — ConsumeReceiveQueue_508230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2479 | 0x00508280 | `sub_508280` | 有正文 | `src/network/NetworkRuntime.cpp:4751 — ConsumeSendQueue_508280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2480 | 0x005082D0 | `sub_5082D0` | 有正文 | `src/network/NetworkRuntime.cpp:727 — Sub_5082D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2481 | 0x00508310 | `sub_508310` | 有正文 | `src/network/NetworkRuntime.cpp:737 — Sub_508310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2482 | 0x00508350 | `sub_508350` | 有正文 | `src/LoginConnection.cpp:9 — ResetServerConnectionState_508350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2483 | 0x00508360 | `sub_508360` | 有正文 | `src/LoginState.cpp:86 — Sub_508360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2484 | 0x00508800 | `sub_508800` | 有正文 | `src/network/NetworkRuntime.cpp:1058 — HandleResponse_508800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2485 | 0x00508910 | `sub_508910` | 有正文 | `src/scene/login/CharacterListRequest.cpp:145 — StartCharacterListRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2486 | 0x00508950 | `sub_508950` | 有正文 | `src/scene/login/CharacterListRequest.cpp:160 — PollCharacterListRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2487 | 0x005089B0 | `sub_5089B0` | 有正文 | `src/scene/login/CharacterListRequest.cpp:185 — HandleCharacterListResponse_5089B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2488 | 0x00508AF0 | `sub_508AF0` | 有正文 | `src/scene/login/LoginRequest.cpp:93 — StartLoginRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2489 | 0x00508B50 | `sub_508B50` | 有正文 | `src/scene/login/LoginRequest.cpp:118 — PollLoginRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2490 | 0x00508BC0 | `sub_508BC0` | 有正文 | `src/scene/login/LoginRequest.cpp:145 — HandleLoginResponse_508BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2491 | 0x00508C40 | `sub_508C40` | 有正文 | `src/scene/login/LoginRequest.cpp:182 — Sub_508C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2492 | 0x00508C90 | `sub_508C90` | 有正文 | `src/scene/login/LoginRequest.cpp:196 — Sub_508C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2493 | 0x00508CF0 | `sub_508CF0` | 有正文 | `src/scene/login/LoginRequest.cpp:219 — Sub_508CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2494 | 0x00508E00 | `sub_508E00` | 有正文 | `src/scene/login/CharacterCreationRequest.cpp:30 — StartCharacterCreationRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2495 | 0x00508E90 | `sub_508E90` | 有正文 | `src/scene/login/CharacterCreationRequest.cpp:57 — PollCharacterCreationRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2496 | 0x00508EE0 | `sub_508EE0` | 有正文 | `src/scene/login/CharacterCreationRequest.cpp:77 — HandleCharacterCreationResponse_508EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2497 | 0x00508F70 | `sub_508F70` | 有正文 | `src/scene/login/CharacterDeletionRequest.cpp:26 — StartCharacterDeletionRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2498 | 0x00508FC0 | `sub_508FC0` | 有正文 | `src/scene/login/CharacterDeletionRequest.cpp:47 — PollCharacterDeletionRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2499 | 0x00509020 | `sub_509020` | 有正文 | `src/scene/login/CharacterDeletionRequest.cpp:69 — HandleCharacterDeletionResponse_509020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2500 | 0x00509080 | `nullsub_1` | 空方法 | `include/cg/core/LegacyNoOp.hpp:37 — NullSub_509080` | 已有CPP空方法与原RET记录保留；按用户决定标为空方法，不再核对此入口 |
| 2501 | 0x00509090 | `sub_509090` | 有正文 | `src/world/SceneState.cpp:5569 — Sub_509090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2502 | 0x00509100 | `sub_509100` | 有正文 | `src/world/SceneState.cpp:5592 — Sub_509100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2503 | 0x005093C0 | `sub_5093C0` | 有正文 | `src/world/SceneState.cpp:5260 — Sub_5093C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2504 | 0x005093F0 | `Map::FlushNew` | 有正文 | `src/world/SceneState.cpp:5698 — Sub_5093F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2505 | 0x00509550 | `sub_509550` | 有正文 | `src/world/SceneState.cpp:5747 — Sub_509550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2506 | 0x00509650 | `sub_509650` | 有正文 | `src/world/SceneState.cpp:2681 — Sub_509650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2507 | 0x00509BA0 | `sub_509BA0` | 有正文 | `src/world/SceneState.cpp:2773 — Sub_509BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2508 | 0x00509E30 | `sub_509E30` | 有正文 | `src/world/SceneState.cpp:2843 — Sub_509E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2509 | 0x00509EB0 | `sub_509EB0` | 有正文 | `src/world/SceneState.cpp:2949 — Sub_509EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2510 | 0x0050A120 | `sub_50A120` | 有正文 | `src/world/SceneState.cpp:3016 — Sub_50A120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2511 | 0x0050A300 | `sub_50A300` | 有正文 | `src/world/SceneState.cpp:3822 — Sub_50A300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2512 | 0x0050A3E0 | `sub_50A3E0` | 有正文 | `src/world/SceneState.cpp:1949 — Sub_50A3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2513 | 0x0050A6B0 | `sub_50A6B0` | 有正文 | `src/world/SceneState.cpp:3540 — Sub_50A6B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2514 | 0x0050AC20 | `sub_50AC20` | 有正文 | `src/world/SceneState.cpp:3639 — Sub_50AC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2515 | 0x0050AEF0 | `sub_50AEF0` | 有正文 | `src/world/WorldActor.cpp:35358 — Sub_50AEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2516 | 0x0050B0D0 | `sub_50B0D0` | 有正文 | `src/world/SceneState.cpp:3680 — Sub_50B0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2517 | 0x0050B380 | `sub_50B380` | 有正文 | `src/world/SceneState.cpp:2053 — Sub_50B380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2518 | 0x0050BCD0 | `sub_50BCD0` | 有正文 | `src/world/SceneState.cpp:2244 — Sub_50BCD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2519 | 0x0050BF20 | `sub_50BF20` | 有正文 | `src/world/SceneState.cpp:3200 — Sub_50BF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2520 | 0x0050BFA0 | `sub_50BFA0` | 有正文 | `src/world/SceneState.cpp:2365 — Sub_50BFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2521 | 0x0050BFC0 | `sub_50BFC0` | 有正文 | `src/world/SceneState.cpp:2378 — Sub_50BFC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2522 | 0x0050BFD0 | `sub_50BFD0` | 有正文 | `src/world/SceneState.cpp:3251 — Sub_50BFD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2523 | 0x0050C260 | `sub_50C260` | 有正文 | `src/world/SceneState.cpp:3277 — Sub_50C260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2524 | 0x0050C580 | `sub_50C580` | 有正文 | `src/world/SceneState.cpp:3063 — Sub_50C580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2525 | 0x0050C7A0 | `sub_50C7A0` | 有正文 | `src/world/SceneState.cpp:2396 — Sub_50C7A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2526 | 0x0050C7F0 | `sub_50C7F0` | 有正文 | `src/ui/windows/WindowWorkResult.cpp:8 — WindowWorkResult_ReceiveReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2527 | 0x0050C870 | `sub_50C870` | 有正文 | `src/world/SceneState.cpp:6359 — Sub_50C870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2528 | 0x0050C8D0 | `sub_50C8D0` | 有正文 | `src/world/SceneState.cpp:7814 — Sub_50C8D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2529 | 0x0050C900 | `sub_50C900` | 有正文 | `src/world/SceneState.cpp:3345 — Sub_50C900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2530 | 0x0050C940 | `sub_50C940` | 有正文 | `src/world/SceneState.cpp:2497 — Sub_50C940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2531 | 0x0050CAE0 | `sub_50CAE0` | 有正文 | `src/world/SceneState.cpp:2551 — Sub_50CAE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2532 | 0x0050CE10 | `sub_50CE10` | 有正文 | `src/world/SceneState.cpp:2415 — Sub_50CE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2533 | 0x0050CF20 | `sub_50CF20` | 有正文 | `src/network/NetworkRuntime.cpp:766 — Sub_50CF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2534 | 0x0050CF60 | `sub_50CF60` | 有正文 | `src/network/NetworkRuntime.cpp:757 — Sub_50CF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2535 | 0x0050CF70 | `sub_50CF70` | 有正文 | `src/world/SceneState.cpp:2387 — Sub_50CF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2536 | 0x0050CF90 | `sub_50CF90` | 有正文 | `src/world/SceneState.cpp:5546 — Sub_50CF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2537 | 0x0050D000 | `sub_50D000` | 有正文 | `src/world/SceneState.cpp:2656 — Sub_50D000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2538 | 0x0050D080 | `sub_50D080` | 有正文 | `src/world/SceneState.cpp:5271 — Sub_50D080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2539 | 0x0050D120 | `sub_50D120` | 有正文 | `src/world/SceneState.cpp:5314 — Sub_50D120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2540 | 0x0050D3B0 | `sub_50D3B0` | 有正文 | `src/network/NetworkRuntime.cpp:781 — Sub_50D3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2541 | 0x0050D3C0 | `sub_50D3C0` | 有正文 | `src/world/SceneState.cpp:5392 — Sub_50D3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2542 | 0x0050D500 | `sub_50D500` | 有正文 | `src/world/SceneState.cpp:5451 — Sub_50D500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2543 | 0x0050D700 | `sub_50D700` | 有正文 | `src/network/NetworkRuntime.cpp:819 — Sub_50D700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2544 | 0x0050D740 | `sub_50D740` | 有正文 | `src/world/MailState.cpp:270 — Sub_50D740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2545 | 0x0050D8C0 | `sub_50D8C0` | 有正文 | `src/world/MailState.cpp:309 — Sub_50D8C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2546 | 0x0050D9E0 | `sub_50D9E0` | 有正文 | `src/world/MailState.cpp:337 — Sub_50D9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2547 | 0x0050DC70 | `sub_50DC70` | 有正文 | `src/world/MailState.cpp:613 — Sub_50DC70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2548 | 0x0050DD80 | `sub_50DD80` | 有正文 | `src/network/NetworkRuntime.cpp:839 — Sub_50DD80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2549 | 0x0050E110 | `sub_50E110` | 有正文 | `src/ui/windows/WindowDialog.cpp:1258 — WindowDialog_ReceiveReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2550 | 0x0050E1A0 | `sub_50E1A0` | 有正文 | `src/world/SceneState.cpp:2438 — Sub_50E1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2551 | 0x0050E210 | `sub_50E210` | 有正文 | `src/world/SceneState.cpp:2472 — Sub_50E210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2552 | 0x0050E310 | `sub_50E310` | 有正文 | `src/network/NetworkRuntime.cpp:791 — Sub_50E310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2553 | 0x0050E340 | `sub_50E340` | 有正文 | `src/ui/windows/WindowSkill.cpp:2552 — WindowSkill_ReceiveTextTargetReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2554 | 0x0050E360 | `sub_50E360` | 有正文 | `src/ui/windows/WindowSkill.cpp:2561 — WindowSkill_ReceiveAttributeTargetReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2555 | 0x0050E380 | `sub_50E380` | 有正文 | `src/ui/windows/WindowItemUse.cpp:353 — WindowItemUse_ReceiveTextTargetReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2556 | 0x0050E3A0 | `sub_50E3A0` | 有正文 | `src/ui/windows/WindowItemUse.cpp:362 — WindowItemUse_ReceiveAttributeTargetReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2557 | 0x0050E3C0 | `sub_50E3C0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2970 — WindowTrade_ReceiveTargetReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2558 | 0x0050E3F0 | `sub_50E3F0` | 有正文 | `src/ui/windows/WindowTrade.cpp:2982 — WindowTrade_ReceiveStartReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2559 | 0x0050E440 | `sub_50E440` | 有正文 | `src/ui/windows/WindowTrade.cpp:2999 — WindowTrade_ReceiveCancelReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2560 | 0x0050E470 | `sub_50E470` | 有正文 | `src/ui/windows/WindowTrade.cpp:3014 — WindowTrade_ReceiveStateReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2561 | 0x0050E4A0 | `sub_50E4A0` | 有正文 | `src/ui/windows/WindowTrade.cpp:3029 — WindowTrade_ReceiveItemReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2562 | 0x0050E4D0 | `sub_50E4D0` | 有正文 | `src/ui/windows/WindowTrade.cpp:3043 — WindowTrade_ReceivePetReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2563 | 0x0050E500 | `sub_50E500` | 有正文 | `src/ui/windows/WindowTrade.cpp:3058 — WindowTrade_ReceivePetSkillReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2564 | 0x0050E530 | `sub_50E530` | 有正文 | `src/ui/windows/WindowTrade.cpp:3073 — WindowTrade_ReceiveRemoteResetReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2565 | 0x0050E570 | `sub_50E570` | 有正文 | `src/ui/windows/WindowTrade.cpp:3093 — WindowTrade_ReceiveCloseReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2566 | 0x0050E5B0 | `sub_50E5B0` | 有正文 | `src/world/SceneState.cpp:832 — Sub_50E5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2567 | 0x0050E5D0 | `sub_50E5D0` | 有正文 | `src/world/SceneState.cpp:839 — Sub_50E5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2568 | 0x0050E630 | `sub_50E630` | 有正文 | `src/network/NetworkRuntime.cpp:807 — Sub_50E630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2569 | 0x0050E650 | `sub_50E650` | 有正文 | `src/world/SceneState.cpp:2463 — Sub_50E650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2570 | 0x0050E680 | `sub_50E680` | 有正文 | `src/network/NetworkRuntime.cpp:1204 — Sub_50E680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2571 | 0x0050E6C0 | `sub_50E6C0` | 有正文 | `src/network/NetworkRuntime.cpp:747 — Sub_50E6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2572 | 0x0050E6E0 | `sub_50E6E0` | 有正文 | `src/network/NetworkRuntime.cpp:937 — Sub_50E6E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2573 | 0x0050E770 | `sub_50E770` | 有正文 | `src/CharacterSelectScene.cpp:197 — Sub_50E770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2574 | 0x0050E780 | `sub_50E780` | 有正文 | `src/network/NetworkRuntime.cpp:953 — Sub_50E780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2575 | 0x0050E7B0 | `sub_50E7B0` | 已排除 | `doc/cg2d_code_organization_review_spec.md — COR10：追踪配置移除；NetworkRuntime保留tq解析` | 用户要求取消重复点击追踪；解码后忽略，不计原配置功能实现 |
| 2576 | 0x0050E7D0 | `sub_50E7D0` | 有正文 | `src/ui/windows/WindowStall.cpp:2729 — WindowStall_ReceiveOpenReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2577 | 0x0050E7E0 | `sub_50E7E0` | 有正文 | `src/ui/windows/WindowStall.cpp:2735 — WindowStall_ReceiveCloseReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2578 | 0x0050E7F0 | `sub_50E7F0` | 有正文 | `src/ui/windows/WindowStall.cpp:2741 — WindowStall_ReceiveBrowseReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2579 | 0x0050E820 | `sub_50E820` | 有正文 | `src/ui/windows/WindowStall.cpp:2750 — WindowStall_ReceivePurchaseReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2580 | 0x0050E830 | `sub_50E830` | 有正文 | `src/network/NetworkRuntime.cpp:967 — Sub_50E830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2581 | 0x0050E850 | `sub_50E850` | 有正文 | `src/world/GuildState.cpp:137 — Sub_50E850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2582 | 0x0050EB40 | `sub_50EB40` | 有正文 | `src/world/GuildState.cpp:207 — Sub_50EB40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2583 | 0x0050EC50 | `sub_50EC50` | 有正文 | `src/world/GuildState.cpp:365 — UpdateOrderedRecordBatch_50EC50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2584 | 0x0050EEB0 | `sub_50EEB0` | 有正文 | `src/world/GuildState.cpp:334 — UpdateOrderedRecords_50EEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2585 | 0x0050EF20 | `sub_50EF20` | 有正文 | `src/world/GuildState.cpp:273 — ResetOrderedRecord_50EF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2586 | 0x0050EF50 | `sub_50EF50` | 有正文 | `src/world/GuildState.cpp:261 — CountRecordsValue08EqualOne_50EF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2587 | 0x0050EF70 | `sub_50EF70` | 有正文 | `src/world/GuildState.cpp:291 — ParseOrderedRecord_50EF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2588 | 0x0050F130 | `sub_50F130` | 有正文 | `src/network/NetworkRuntime.cpp:979 — Sub_50F130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2589 | 0x0050F190 | `sub_50F190` | 有正文 | `src/network/NetworkRuntime.cpp:999 — Sub_50F190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2590 | 0x0050F280 | `sub_50F280` | 有正文 | `src/world/GuildState.cpp:241 — Sub_50F280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2591 | 0x0050F2A0 | `sub_50F2A0` | 有正文 | `src/world/MailState.cpp:994 — Sub_50F2A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2592 | 0x0050F3A0 | `sub_50F3A0` | 有正文 | `src/world/MailState.cpp:1026 — Sub_50F3A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2593 | 0x0050F430 | `sub_50F430` | 有正文 | `src/world/MailState.cpp:733 — Sub_50F430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2594 | 0x0050F450 | `sub_50F450` | 有正文 | `src/world/MailState.cpp:1192 — Sub_50F450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2595 | 0x0050F7F0 | `sub_50F7F0` | 有正文 | `src/world/MailState.cpp:741 — Sub_50F7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2596 | 0x0050F8E0 | `sub_50F8E0` | 有正文 | `src/world/MailState.cpp:1046 — Sub_50F8E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2597 | 0x0050F990 | `sub_50F990` | 有正文 | `src/ui/windows/WindowProfile.cpp:221 — WindowProfile_ReceiveReply` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2598 | 0x0050F9E0 | `sub_50F9E0` | 有正文 | `src/world/MailState.cpp:769 — Sub_50F9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2599 | 0x0050FC30 | `sub_50FC30` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2600 | 0x0050FE30 | `sub_50FE30` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2601 | 0x0050FF10 | `sub_50FF10` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2602 | 0x0050FF70 | `sub_50FF70` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2603 | 0x00510120 | `sub_510120` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2604 | 0x00510140 | `sub_510140` | 有正文 | `src/scene/login/LoginRequest.cpp:87 — ReadRequestOutcomeFlag` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2605 | 0x00510150 | `sub_510150` | 有正文 | `src/world/SceneState.cpp:3477 — Sub_510150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2606 | 0x00510460 | `sub_510460` | 有正文 | `src/network/NetworkRuntime.cpp:1021 — Sub_510460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2607 | 0x00510490 | `sub_510490` | 有正文 | `src/network/NetworkRuntime.cpp:1033 — Sub_510490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2608 | 0x00510570 | `sub_510570` | 有正文 | `src/network/NetworkRuntime.cpp:1967 — Sub_510570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2609 | 0x00510630 | `sub_510630` | 有正文 | `src/network/NetworkRuntime.cpp:2179 — Sub_510630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2610 | 0x00510710 | `sub_510710` | 有正文 | `src/network/NetworkRuntime.cpp:2236 — Sub_510710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2611 | 0x00510810 | `sub_510810` | 有正文 | `src/network/NetworkRuntime.cpp:2379 — Sub_510810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2612 | 0x00510880 | `sub_510880` | 有正文 | `src/network/NetworkRuntime.cpp:2549 — Sub_510880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2613 | 0x005108F0 | `sub_5108F0` | 有正文 | `src/network/NetworkRuntime.cpp:4216 — Sub_5108F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2614 | 0x00510940 | `sub_510940` | 有正文 | `src/network/NetworkRuntime.cpp:2579 — Sub_510940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2615 | 0x005109B0 | `SendBattlePacket` | 有正文 | `src/network/NetworkRuntime.cpp:4115 — SendBattlePacket_5109B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2616 | 0x00510A00 | `sub_510A00` | 有正文 | `src/network/NetworkRuntime.cpp:4019 — Sub_510A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2617 | 0x00510AC0 | `sub_510AC0` | 有正文 | `src/network/NetworkRuntime.cpp:2302 — Sub_510AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2618 | 0x00510B80 | `sub_510B80` | 有正文 | `src/network/NetworkRuntime.cpp:2323 — Sub_510B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2619 | 0x00510CE0 | `sub_510CE0` | 有正文 | `src/network/NetworkRuntime.cpp:3979 — Sub_510CE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2620 | 0x00510DA0 | `sub_510DA0` | 有正文 | `src/network/NetworkRuntime.cpp:4131 — Sub_510DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2621 | 0x00510E30 | `sub_510E30` | 有正文 | `src/network/NetworkRuntime.cpp:3814 — Sub_510E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2622 | 0x00510EC0 | `SendItemMove` | 有正文 | `src/network/NetworkRuntime.cpp:4164 — Sub_510EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2623 | 0x00510F50 | `sub_510F50` | 有正文 | `src/network/NetworkRuntime.cpp:4197 — Sub_510F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2624 | 0x00510FA0 | `sub_510FA0` | 有正文 | `src/network/NetworkRuntime.cpp:2690 — Sub_510FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2625 | 0x00511030 | `sub_511030` | 有正文 | `src/network/NetworkRuntime.cpp:2409 — Sub_511030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2626 | 0x005110B0 | `sub_5110B0` | 有正文 | `src/network/NetworkRuntime.cpp:2755 — Sub_5110B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2627 | 0x00511190 | `sub_511190` | 有正文 | `src/network/NetworkRuntime.cpp:2442 — Sub_511190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2628 | 0x005111E0 | `sub_5111E0` | 有正文 | `src/network/NetworkRuntime.cpp:2609 — Sub_5111E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2629 | 0x00511250 | `sub_511250` | 有正文 | `src/network/NetworkRuntime.cpp:2458 — Sub_511250` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2630 | 0x005112C0 | `sub_5112C0` | 有正文 | `src/network/NetworkRuntime.cpp:2480 — Sub_5112C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2631 | 0x00511350 | `sub_511350` | 有正文 | `src/network/NetworkRuntime.cpp:2519 — Sub_511350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2632 | 0x005113C0 | `sub_5113C0` | 有正文 | `src/network/NetworkRuntime.cpp:2639 — Sub_5113C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2633 | 0x00511430 | `sub_511430` | 有正文 | `src/network/NetworkRuntime.cpp:2669 — Sub_511430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2634 | 0x00511480 | `sub_511480` | 有正文 | `src/network/NetworkRuntime.cpp:2716 — Sub_511480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2635 | 0x00511510 | `sub_511510` | 有正文 | `src/network/NetworkRuntime.cpp:2791 — Sub_511510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2636 | 0x005115F0 | `sub_5115F0` | 有正文 | `src/network/NetworkRuntime.cpp:2848 — Sub_5115F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2637 | 0x00511640 | `sub_511640` | 有正文 | `src/network/NetworkRuntime.cpp:2869 — Sub_511640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2638 | 0x00511760 | `sub_511760` | 有正文 | `src/network/NetworkRuntime.cpp:2944 — Sub_511760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2639 | 0x005117D0 | `sub_5117D0` | 有正文 | `src/network/NetworkRuntime.cpp:2974 — Sub_5117D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2640 | 0x00511840 | `sub_511840` | 有正文 | `src/network/NetworkRuntime.cpp:3004 — Sub_511840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2641 | 0x005118B0 | `sub_5118B0` | 有正文 | `src/network/NetworkRuntime.cpp:3034 — Sub_5118B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2642 | 0x00511900 | `sub_511900` | 有正文 | `src/network/NetworkRuntime.cpp:3056 — Sub_511900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2643 | 0x00511A00 | `sub_511A00` | 有正文 | `src/network/NetworkRuntime.cpp:3122 — Sub_511A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2644 | 0x00511A50 | `sub_511A50` | 有正文 | `src/network/NetworkRuntime.cpp:3143 — Sub_511A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2645 | 0x00511AE0 | `sub_511AE0` | 有正文 | `src/network/NetworkRuntime.cpp:3182 — Sub_511AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2646 | 0x00511BC0 | `sub_511BC0` | 有正文 | `src/network/NetworkRuntime.cpp:3239 — Sub_511BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2647 | 0x00511C30 | `sub_511C30` | 有正文 | `src/network/NetworkRuntime.cpp:3269 — Sub_511C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2648 | 0x00511C80 | `sub_511C80` | 有正文 | `src/network/NetworkRuntime.cpp:3290 — Sub_511C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2649 | 0x00511CD0 | `sub_511CD0` | 有正文 | `src/network/NetworkRuntime.cpp:1816 — Sub_511CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2650 | 0x00511D10 | `sub_511D10` | 有正文 | `src/network/NetworkRuntime.cpp:3311 — Sub_511D10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2651 | 0x00511D60 | `sub_511D60` | 有正文 | `src/network/NetworkRuntime.cpp:3332 — Sub_511D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2652 | 0x00511DD0 | `sub_511DD0` | 有正文 | `src/network/NetworkRuntime.cpp:3362 — Sub_511DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2653 | 0x00511E60 | `sub_511E60` | 有正文 | `src/network/NetworkRuntime.cpp:3401 — Sub_511E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2654 | 0x00511EF0 | `sub_511EF0` | 有正文 | `src/network/NetworkRuntime.cpp:3440 — Sub_511EF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2655 | 0x00511FB0 | `sub_511FB0` | 有正文 | `src/network/NetworkRuntime.cpp:1832 — Sub_511FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2656 | 0x00511FF0 | `sub_511FF0` | 有正文 | `src/network/NetworkRuntime.cpp:3488 — Sub_511FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2657 | 0x00512040 | `sub_512040` | 有正文 | `src/network/NetworkRuntime.cpp:3504 — Sub_512040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2658 | 0x005120D0 | `sub_5120D0` | 有正文 | `src/network/NetworkRuntime.cpp:3543 — Sub_5120D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2659 | 0x00512160 | `sub_512160` | 有正文 | `src/network/NetworkRuntime.cpp:3582 — Sub_512160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2660 | 0x00512190 | `sub_512190` | 有正文 | `src/network/NetworkRuntime.cpp:3592 — Sub_512190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2661 | 0x005121E0 | `sub_5121E0` | 有正文 | `src/network/NetworkRuntime.cpp:3613 — Sub_5121E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2662 | 0x00512230 | `sub_512230` | 有正文 | `src/network/NetworkRuntime.cpp:1848 — Sub_512230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2663 | 0x00512270 | `sub_512270` | 有正文 | `src/network/NetworkRuntime.cpp:3649 — Sub_512270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2664 | 0x005122C0 | `sub_5122C0` | 有正文 | `src/network/NetworkRuntime.cpp:3670 — Sub_5122C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2665 | 0x00512310 | `sub_512310` | 有正文 | `src/network/NetworkRuntime.cpp:3691 — Sub_512310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2666 | 0x00512360 | `sub_512360` | 有正文 | `src/network/NetworkRuntime.cpp:3712 — Sub_512360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2667 | 0x005123B0 | `sub_5123B0` | 有正文 | `src/network/NetworkRuntime.cpp:3733 — Sub_5123B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2668 | 0x00512400 | `sub_512400` | 有正文 | `src/network/NetworkRuntime.cpp:3754 — Sub_512400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2669 | 0x00512470 | `sub_512470` | 有正文 | `src/network/NetworkRuntime.cpp:3784 — Sub_512470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2670 | 0x005124E0 | `sub_5124E0` | 有正文 | `src/network/NetworkRuntime.cpp:3842 — Sub_5124E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2671 | 0x00512570 | `sub_512570` | 有正文 | `src/network/NetworkRuntime.cpp:1864 — Sub_512570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2672 | 0x005125B0 | `sub_5125B0` | 有正文 | `src/network/NetworkRuntime.cpp:3881 — Sub_5125B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2673 | 0x00512620 | `sub_512620` | 有正文 | `src/network/NetworkRuntime.cpp:3912 — Sub_512620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2674 | 0x00512720 | `sub_512720` | 有正文 | `src/network/NetworkRuntime.cpp:4232 — Sub_512720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2675 | 0x005127B0 | `sub_5127B0` | 有正文 | `src/network/NetworkRuntime.cpp:3634 — Sub_5127B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2676 | 0x00512800 | `sub_512800` | 有正文 | `src/network/NetworkRuntime.cpp:2073 — Sub_512800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2677 | 0x005128C0 | `sub_5128C0` | 有正文 | `src/network/NetworkRuntime.cpp:2124 — Sub_5128C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2678 | 0x00512AB0 | `sub_512AB0` | 有正文 | `src/network/NetworkRuntime.cpp:2054 — Sub_512AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2679 | 0x00512B00 | `sub_512B00` | 有正文 | `src/network/NetworkRuntime.cpp:2015 — Sub_512B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2680 | 0x00512B90 | `sub_512B90` | 有正文 | `src/network/NetworkRuntime.cpp:1768 — Sub_512B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2681 | 0x00512BD0 | `sub_512BD0` | 有正文 | `src/network/NetworkRuntime.cpp:1784 — Sub_512BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2682 | 0x00512C10 | `sub_512C10` | 有正文 | `src/network/NetworkRuntime.cpp:1749 — Sub_512C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2683 | 0x00512C60 | `sub_512C60` | 有正文 | `src/network/NetworkRuntime.cpp:1800 — Sub_512C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2684 | 0x00512CA0 | `sub_512CA0` | 有正文 | `src/network/NetworkRuntime.cpp:1731 — Report_512CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2685 | 0x00512CF0 | `sub_512CF0` | 有正文 | `src/network/NetworkRuntime.cpp:1880 — Sub_512CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2686 | 0x00512D30 | `sub_512D30` | 有正文 | `src/network/NetworkRuntime.cpp:1916 — Sub_512D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2687 | 0x00512DA0 | `sub_512DA0` | 有正文 | `src/network/NetworkRuntime.cpp:1946 — Sub_512DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2688 | 0x00512DF0 | `sub_512DF0` | 有正文 | `src/network/NetworkRuntime.cpp:1896 — Sub_512DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2689 | 0x00512E40 | `network_dispatch` | 有正文 | `src/network/NetworkRuntime.cpp:4360 — NetworkDispatch_512E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2690 | 0x00516B60 | `sub_516B60` | 有正文 | `src/network/NetworkRuntime.cpp:4493 — AllocateNetworkWorkspace_516B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2691 | 0x00516C90 | `sub_516C90` | 有正文 | `src/network/NetworkRuntime.cpp:4059 — Sub_516C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2692 | 0x00516D70 | `sub_516D70` | 有正文 | `src/network/NetworkRuntime.cpp:4106 — SendIuEPacket_516D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2693 | 0x00516DA0 | `sub_516DA0` | 有正文 | `src/network/NetworkRuntime.cpp:1150 — AllocateProtocolBuffers_516DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2694 | 0x00517040 | `sub_517040` | 有正文 | `src/network/NetworkRuntime.cpp:1303 — Sub_517040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2695 | 0x00517080 | `sub_517080` | 有正文 | `src/network/NetworkRuntime.cpp:1284 — CopyProtocolBytes_517080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2696 | 0x005170D0 | `sub_5170D0` | 有正文 | `src/network/NetworkRuntime.cpp:1337 — AppendProtocolText_5170D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2697 | 0x00517120 | `sub_517120` | 有正文 | `src/network/NetworkRuntime.cpp:1375 — FormatProtocolIntegerToken_517120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2698 | 0x00517160 | `sub_517160` | 有正文 | `src/network/NetworkRuntime.cpp:1397 — FormatProtocolTextToken_517160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2699 | 0x00517190 | `sub_517190` | 有正文 | `src/network/NetworkRuntime.cpp:1408 — ParseProtocolInteger_517190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2700 | 0x005171B0 | `sub_5171B0` | 有正文 | `src/network/NetworkRuntime.cpp:1679 — Sub_5171B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2701 | 0x005171F0 | `sub_5171F0` | 有正文 | `src/network/NetworkRuntime.cpp:1586 — EscapeProtocolText_5171F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2702 | 0x005172D0 | `sub_5172D0` | 有正文 | `src/network/NetworkRuntime.cpp:1609 — UnescapeProtocolText_5172D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2703 | 0x005173B0 | `receivedBufferSplit` | 有正文 | `src/network/NetworkRuntime.cpp:573 — SplitReceivedBuffer_5173B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2704 | 0x00517480 | `sub_517480` | 有正文 | `src/network/NetworkRuntime.cpp:1695 — SendProtocolBuffer_517480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2705 | 0x00517510 | `sub_517510` | 有正文 | `src/network/NetworkRuntime.cpp:1101 — BuildProtocolCommand_517510` | 当前本机服务端明文头采用原命令／空格；旧命令变换／密文状态尾按获批明文合同退休，完整发送未验 |
| 2706 | 0x00517670 | `sub_517670` | 有正文 | `src/network/NetworkRuntime.cpp:1319 — FormatProtocolInteger_517670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2707 | 0x00517690 | `sub_517690` | 有正文 | `src/network/NetworkRuntime.cpp:1327 — CopyProtocolBytesAndReturn_517690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2708 | 0x005176B0 | `sub_5176B0` | 有正文 | `src/network/NetworkRuntime.cpp:616 — Sub_5176B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2709 | 0x00517730 | `sub_517730` | 有正文 | `src/network/NetworkRuntime.cpp:660 — Sub_517730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2710 | 0x00517880 | `Yobi::FrameMove` | 有正文 | `src/Yobi/animation.cpp:34628 — Sub_517880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2711 | 0x00517DA0 | `Yobi::PlayFrame` | 有正文 | `src/Yobi/animation.cpp:34744 — PlayFrame_517DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2712 | 0x005182B0 | `sub_5182B0` | 有正文 | `src/Yobi/animation.cpp:5655 — Sub_5182B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2713 | 0x00518300 | `sub_518300` | 有正文 | `src/Yobi/animation.cpp:5676 — Sub_518300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2714 | 0x005183B0 | `sub_5183B0` | 有正文 | `src/world/WorldActor.cpp:36023 — Sub_5183B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2715 | 0x005183D0 | `sub_5183D0` | 有承接 | `src/platform/RuntimeInitialization.cpp:298 — cg::world::detail::gPlayerRuntimeSlots` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 2716 | 0x005183E0 | `sub_5183E0` | 有承接 | `src/platform/RuntimeInitialization.cpp:298 — cg::world::detail::gPlayerRuntimeSlots` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 2717 | 0x00518520 | `sub_518520` | 有承接 | `src/platform/RuntimeInitialization.cpp:302 — cg::world::detail::gPetRuntimeRecords` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 2718 | 0x00518530 | `sub_518530` | 有承接 | `src/platform/RuntimeInitialization.cpp:302 — cg::world::detail::gPetRuntimeRecords` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 2719 | 0x00518560 | `sub_518560` | 有承接 | `src/platform/RuntimeInitialization.cpp:302 — cg::world::detail::gPetRuntimeRecords` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 2720 | 0x00518580 | `sub_518580` | 有正文 | `src/world/SceneState.cpp:3799 — Sub_518580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2721 | 0x005185C0 | `sub_5185C0` | 有正文 | `src/world/SceneState.cpp:3739 — Sub_5185C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2722 | 0x00518600 | `sub_518600` | 有正文 | `src/world/SceneState.cpp:3131 — Sub_518600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2723 | 0x00518660 | `sub_518660` | 有正文 | `src/world/SceneState.cpp:3116 — Sub_518660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2724 | 0x005186A0 | `sub_5186A0` | 有正文 | `src/world/SceneState.cpp:649 — Sub_5186A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2725 | 0x00518780 | `sub_518780` | 有正文 | `src/world/SceneState.cpp:681 — Sub_518780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2726 | 0x00518870 | `sub_518870` | 有正文 | `src/world/SceneState.cpp:731 — Sub_518870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2727 | 0x00518A40 | `sub_518A40` | 有正文 | `src/world/SceneState.cpp:778 — Sub_518A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2728 | 0x00518BE0 | `sub_518BE0` | 有正文 | `src/world/SceneState.cpp:719 — Sub_518BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2729 | 0x00518C20 | `sub_518C20` | 有正文 | `src/world/SceneState.cpp:589 — InitializeAlbum_518C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2730 | 0x00518C60 | `sub_518C60` | 有正文 | `src/world/SceneState.cpp:7862 — Sub_518C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2731 | 0x00518C80 | `sub_518C80` | 有正文 | `src/world/MailState.cpp:823 — Sub_518C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2732 | 0x00518CB0 | `sub_518CB0` | 有正文 | `src/world/MailState.cpp:1351 — Sub_518CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2733 | 0x00518DA0 | `sub_518DA0` | 有正文 | `src/world/MailState.cpp:409 — Sub_518DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2734 | 0x00518F00 | `sub_518F00` | 有正文 | `src/world/MailState.cpp:476 — Sub_518F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2735 | 0x00519090 | `sub_519090` | 有正文 | `src/world/MailState.cpp:546 — Sub_519090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2736 | 0x005190F0 | `sub_5190F0` | 有正文 | `src/world/MailState.cpp:567 — Sub_5190F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2737 | 0x00519170 | `sub_519170` | 有正文 | `src/world/MailState.cpp:594 — Sub_519170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2738 | 0x005191C0 | `sub_5191C0` | 有正文 | `src/world/MailState.cpp:386 — Sub_5191C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2739 | 0x00519290 | `sub_519290` | 有正文 | `src/world/MailState.cpp:1412 — Sub_519290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2740 | 0x005192C0 | `sub_5192C0` | 有正文 | `src/world/MailState.cpp:1430 — Sub_5192C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2741 | 0x00519300 | `sub_519300` | 有正文 | `src/world/MailState.cpp:1378 — Sub_519300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2742 | 0x00519370 | `sub_519370` | 有正文 | `src/world/MailState.cpp:1484 — Sub_519370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2743 | 0x005194C0 | `sub_5194C0` | 有正文 | `src/world/MailState.cpp:1420 — Sub_5194C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2744 | 0x005194F0 | `sub_5194F0` | 有正文 | `src/world/MailState.cpp:1465 — Sub_5194F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2745 | 0x00519550 | `sub_519550` | 有正文 | `src/world/MailState.cpp:1395 — Sub_519550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2746 | 0x005195C0 | `sub_5195C0` | 有正文 | `src/world/GuildState.cpp:426 — FindActiveOrderedRecordIndex_5195C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2747 | 0x005195F0 | `sub_5195F0` | 有正文 | `src/world/MailState.cpp:1442 — Sub_5195F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2748 | 0x00519640 | `sub_519640` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2749 | 0x00519680 | `sub_519680` | 有正文 | `src/ui/WindowRuntime.cpp:770 — WindowItem_HealthImageLevel` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2750 | 0x005196B0 | `sub_5196B0` | 有正文 | `src/world/SceneState.cpp:3223 — Sub_5196B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2751 | 0x00519770 | `sub_519770` | 有正文 | `src/world/SceneState.cpp:3456 — Sub_519770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2752 | 0x00519840 | `sub_519840` | 有正文 | `src/world/SceneState.cpp:3851 — Sub_519840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2753 | 0x0051985F | `?underflow@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/world/SceneState.cpp:3851 — Sub_519840` | 原共享 -1 返回尾；当前父实际失败返回承接；完整业务／运行未验 |
| 2754 | 0x00519870 | `sub_519870` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2755 | 0x005198A0 | `sub_5198A0` | 有正文 | `src/world/SceneState.cpp:3865 — Sub_5198A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2756 | 0x005198D0 | `sub_5198D0` | 有正文 | `src/world/SceneState.cpp:3883 — Sub_5198D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2757 | 0x005198F0 | `sub_5198F0` | 有正文 | `src/world/SceneState.cpp:3893 — Sub_5198F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2758 | 0x00519920 | `sub_519920` | 有正文 | `src/world/SceneState.cpp:3785 — Sub_519920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2759 | 0x00519950 | `sub_519950` | 有正文 | `src/world/SceneState.cpp:3103 — Sub_519950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2760 | 0x005199A0 | `sub_5199A0` | 有正文 | `src/world/SceneState.cpp:9485 — Sub_5199A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2761 | 0x00519A80 | `sub_519A80` | 有正文 | `src/world/SceneState.cpp:8947 — Sub_519A80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2762 | 0x00519CD0 | `sub_519CD0` | 有正文 | `src/world/SceneState.cpp:9056 — Sub_519CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2763 | 0x00519D50 | `sub_519D50` | 有正文 | `src/world/SceneState.cpp:9302 — Sub_519D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2764 | 0x00519E50 | `sub_519E50` | 有正文 | `src/world/SceneState.cpp:9081 — Sub_519E50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2765 | 0x00519EB0 | `sub_519EB0` | 有正文 | `src/world/SceneState.cpp:6352 — Sub_519EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2766 | 0x00519EC0 | `sub_519EC0` | 有正文 | `src/world/SceneState.cpp:9100 — Sub_519EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2767 | 0x00519EE0 | `sub_519EE0` | 有正文 | `src/world/WorldActor.cpp:36787 — Sub_519EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2768 | 0x0051A040 | `sub_51A040` | 有正文 | `src/world/SceneState.cpp:9106 — Sub_51A040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2769 | 0x0051A060 | `sub_51A060` | 有正文 | `src/world/SceneState.cpp:9117 — Sub_51A060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2770 | 0x0051A090 | `sub_51A090` | 有正文 | `src/world/SceneState.cpp:9000 — Sub_51A090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2771 | 0x0051A4B0 | `sub_51A4B0` | 有正文 | `src/world/SceneState.cpp:9048 — Sub_51A4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2772 | 0x0051A5A0 | `sub_51A5A0` | 有正文 | `src/world/SceneState.cpp:9318 — Sub_51A5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2773 | 0x0051A5B0 | `sub_51A5B0` | 有正文 | `src/world/SceneState.cpp:9413 — Sub_51A5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2774 | 0x0051A5E0 | `sub_51A5E0` | 有正文 | `src/world/SceneState.cpp:9326 — Sub_51A5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2775 | 0x0051A5F0 | `sub_51A5F0` | 有正文 | `src/world/SceneState.cpp:9333 — Sub_51A5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2776 | 0x0051A600 | `sub_51A600` | 有正文 | `src/world/SceneState.cpp:9339 — Sub_51A600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2777 | 0x0051A610 | `sub_51A610` | 有正文 | `src/world/SceneState.cpp:9345 — Sub_51A610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2778 | 0x0051A620 | `sub_51A620` | 有正文 | `src/world/SceneState.cpp:9351 — Sub_51A620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2779 | 0x0051A630 | `sub_51A630` | 有正文 | `src/world/SceneState.cpp:9357 — Sub_51A630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2780 | 0x0051A640 | `sub_51A640` | 有正文 | `src/world/WorldActor.cpp:36760 — Sub_51A640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2781 | 0x0051A660 | `sub_51A660` | 有正文 | `src/world/WorldActor.cpp:36770 — Sub_51A660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2782 | 0x0051A680 | `sub_51A680` | 有正文 | `src/world/WorldActor.cpp:36780 — Sub_51A680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2783 | 0x0051A6A0 | `sub_51A6A0` | 有正文 | `src/world/WorldActorDrawing.cpp:35450 — Sub_51A6A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2784 | 0x0051A840 | `sub_51A840` | 有正文 | `src/world/SceneState.cpp:9127 — Sub_51A840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2785 | 0x0051B080 | `sub_51B080` | 有正文 | `src/world/SceneState.cpp:8093 — Sub_51B080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2786 | 0x0051B0F0 | `sub_51B0F0` | 有正文 | `src/world/SceneState.cpp:8085 — Sub_51B0F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2787 | 0x0051B110 | `sub_51B110` | 有正文 | `src/scene/login/LoginRequest.cpp:56 — Sub_51B110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2788 | 0x0051B140 | `sub_51B140` | 有正文 | `src/scene/login/LoginRequest.cpp:65 — CharacterNameExists_51B140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2789 | 0x0051B1B0 | `sub_51B1B0` | 有正文 | `src/scene/login/CharacterListRequest.cpp:51 — WriteCharacterRecord_51B1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2790 | 0x0051B3B0 | `sub_51B3B0` | 有正文 | `src/scene/login/LoginRequest.cpp:46 — Sub_51B3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2791 | 0x0051B3E0 | `sub_51B3E0` | 有正文 | `src/world/SceneState.cpp:8535 — Sub_51B3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2792 | 0x0051B530 | `sub_51B530` | 有正文 | `src/world/SceneState.cpp:3309 — Sub_51B530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2793 | 0x0051B5F0 | `sub_51B5F0` | 有正文 | `src/world/SceneState.cpp:3329 — Sub_51B5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2794 | 0x0051B630 | `sub_51B630` | 有正文 | `src/world/SceneState.cpp:3359 — Sub_51B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2795 | 0x0051B740 | `sub_51B740` | 有正文 | `src/CharacterSelectPreview.cpp:293 — Sub_51B740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2796 | 0x0051B7D0 | `sub_51B7D0` | 有正文 | `src/CharacterSelectPreview.cpp:329 — Sub_51B7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2797 | 0x0051B910 | `sub_51B910` | 有正文 | `src/CharacterSelectPreview.cpp:383 — Sub_51B910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2798 | 0x0051BA10 | `sub_51BA10` | 有正文 | `src/CharacterSelectPreview.cpp:102 — Sub_51BA10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2799 | 0x0051BAC0 | `sub_51BAC0` | 有正文 | `src/CharacterSelectPreview.cpp:157 — Sub_51BAC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2800 | 0x0051BB80 | `sub_51BB80` | 有正文 | `src/CharacterSelectPreview.cpp:130 — Sub_51BB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2801 | 0x0051BC50 | `sub_51BC50` | 有正文 | `src/CharacterSelectPreview.cpp:261 — Sub_51BC50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2802 | 0x0051BD00 | `sub_51BD00` | 有正文 | `src/CharacterSelectPreview.cpp:185 — Sub_51BD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2803 | 0x0051BDB0 | `sub_51BDB0` | 有正文 | `src/world/SceneState.cpp:9363 — Sub_51BDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2804 | 0x0051BDC0 | `sub_51BDC0` | 有正文 | `src/world/SceneState.cpp:9369 — Sub_51BDC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2805 | 0x0051BDD0 | `sub_51BDD0` | 有正文 | `src/world/SceneState.cpp:9424 — Sub_51BDD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2806 | 0x0051BDF0 | `sub_51BDF0` | 有正文 | `src/world/SceneState.cpp:9432 — Sub_51BDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2807 | 0x0051BE10 | `sub_51BE10` | 有正文 | `src/world/SceneState.cpp:9379 — Sub_51BE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2808 | 0x0051BE20 | `sub_51BE20` | 有正文 | `src/world/SceneState.cpp:9385 — Sub_51BE20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2809 | 0x0051BE30 | `sub_51BE30` | 有正文 | `src/world/SceneState.cpp:9391 — Sub_51BE30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2810 | 0x0051BE40 | `sub_51BE40` | 有正文 | `src/world/SceneState.cpp:9401 — Sub_51BE40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2811 | 0x0051BE50 | `sub_51BE50` | 有正文 | `src/world/SceneState.cpp:9452 — Sub_51BE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2812 | 0x0051BEC0 | `sub_51BEC0` | 有正文 | `src/world/SceneState.cpp:9443 — Sub_51BEC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2813 | 0x0051BEE0 | `sub_51BEE0` | 有正文 | `src/world/SceneState.cpp:9407 — Sub_51BEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2814 | 0x0051BEF0 | `sub_51BEF0` | 有正文 | `src/world/SceneState.cpp:436 — Sub_51BEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2815 | 0x0051BF00 | `sub_51BF00` | 有正文 | `src/CharacterSelectPreview.cpp:218 — Sub_51BF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2816 | 0x0051BFF0 | `sub_51BFF0` | 有正文 | `src/CharacterSelectPreview.cpp:240 — Sub_51BFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2817 | 0x0051C0D0 | `mappingCharaImageNo` | 有正文 | `src/world/SceneState.cpp:9256 — Sub_51C0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2818 | 0x0051C410 | `sub_51C410` | 有正文 | `src/world/SceneState.cpp:459 — IsValidPetIndex_51C410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2819 | 0x0051C430 | `World::Update` | 有正文 | `src/world/WorldUpdate.cpp:28 — Update` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2820 | 0x0051C770 | `sub_51C770` | 有正文 | `src/world/SceneState.cpp:522 — SetCurrentSceneType` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2821 | 0x0051C790 | `sub_51C790` | 有正文 | `src/world/SceneState.cpp:530 — SetCurrentSceneTypeAndState` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2822 | 0x0051C7B0 | `sub_51C7B0` | 有正文 | `src/world/SceneState.cpp:538 — SetPendingSceneType` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2823 | 0x0051C7D0 | `World::DrawNormalScene` | 有正文 | `src/world/SceneState.cpp:5783 — DrawNormalScene_51C7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2824 | 0x0051D5B0 | `sub_51D5B0` | 有正文 | `src/world/SceneState.cpp:6789 — Sub_51D5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2825 | 0x0051D640 | `sub_51D640` | 有正文 | `src/world/SceneState.cpp:6258 — Sub_51D640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2826 | 0x0051D6F0 | `sub_51D6F0` | 有正文 | `src/world/SceneState.cpp:6296 — Sub_51D6F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2827 | 0x0051D750 | `sub_51D750` | 有正文 | `src/world/SceneState.cpp:6283 — Sub_51D750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2828 | 0x0051D790 | `sub_51D790` | 有正文 | `src/world/SceneState.cpp:9702 — Sub_51D790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2829 | 0x0051E0D0 | `sub_51E0D0` | 有正文 | `src/world/SceneState.cpp:9994 — Sub_51E0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2830 | 0x0051EC10 | `sub_51EC10` | 有正文 | `src/world/SceneState.cpp:10436 — Sub_51EC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2831 | 0x0051EF80 | `sub_51EF80` | 有正文 | `src/world/SceneState.cpp:10593 — Sub_51EF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2832 | 0x0051F610 | `sub_51F610` | 有正文 | `src/world/SceneState.cpp:10769 — Sub_51F610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2833 | 0x0051F860 | `sub_51F860` | 有正文 | `src/world/SceneState.cpp:10916 — Sub_51F860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2834 | 0x0051FA70 | `sub_51FA70` | 有正文 | `src/world/SceneState.cpp:10922 — Sub_51FA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2835 | 0x0051FC90 | `sub_51FC90` | 有正文 | `src/world/SceneState.cpp:10928 — Sub_51FC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2836 | 0x0051FDE0 | `sub_51FDE0` | 有正文 | `src/world/WorldUpdate.cpp:344 — Sub_51FDE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2837 | 0x00520160 | `sub_520160` | 有正文 | `src/scene/login/LoginSavedState.cpp:997 — Sub_520160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2838 | 0x005201B0 | `sub_5201B0` | 有正文 | `src/scene/login/LoginSavedState.cpp:791 — RestoreCurrentSavedState_5201B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2839 | 0x00520200 | `sub_520200` | 有正文 | `src/scene/login/LoginSavedState.cpp:1024 — Sub_520200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2840 | 0x005202E0 | `sub_5202E0` | 有正文 | `src/platform/LegacyModuleDigest.cpp:108 — ComputeExecutableDigest_5202E0` | COR09：保留文件读取／小写文本／模块接口正文，MD5计算交Windows CNG；本次编译／链接通过，未运行摘要链 |
| 2841 | 0x005203B0 | `sub_5203B0` | 有正文 | `src/LoginInput.cpp:163 — WriteInputValueFF494F_5203B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2842 | 0x005203E0 | `sub_5203E0` | 有正文 | `src/LoginInput.cpp:171 — ReadInputValueFF494F_5203E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2843 | 0x00520410 | `sub_520410` | 有正文 | `src/LoginInput.cpp:182 — WriteInputValueFF4982_520410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2844 | 0x00520440 | `sub_520440` | 有正文 | `src/LoginInput.cpp:190 — ReadInputValueFF4982_520440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2845 | 0x00520470 | `sub_520470` | 有正文 | `src/scene/login/LoginSavedState.cpp:895 — Sub_520470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2846 | 0x005204C0 | `sub_5204C0` | 有正文 | `src/scene/login/LoginSavedState.cpp:707 — RestoreAudioSettings_5204C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2847 | 0x00520510 | `sub_520510` | 有正文 | `src/scene/login/LoginSavedState.cpp:836 — Sub_520510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2848 | 0x00520550 | `sub_520550` | 有正文 | `src/scene/login/LoginSavedState.cpp:857 — Sub_520550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2849 | 0x00520590 | `sub_520590` | 有正文 | `src/scene/login/LoginSavedState.cpp:927 — Sub_520590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2850 | 0x005205E0 | `sub_5205E0` | 有正文 | `src/scene/login/LoginSavedState.cpp:726 — RestoreSavedScalarSettings_5205E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2851 | 0x00520640 | `sub_520640` | 有正文 | `src/scene/login/LoginSavedState.cpp:953 — Sub_520640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2852 | 0x00520690 | `sub_520690` | 有正文 | `src/scene/login/LoginSavedState.cpp:745 — RestoreSavedInputContexts_520690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2853 | 0x005206F0 | `sub_5206F0` | 有正文 | `src/scene/login/LoginSavedState.cpp:975 — Sub_5206F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2854 | 0x00520700 | `sub_520700` | 有正文 | `src/scene/login/LoginSavedState.cpp:765 — RestoreSavedByteState_520700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2855 | 0x00520720 | `sub_520720` | 有正文 | `src/scene/login/LoginSavedState.cpp:986 — Sub_520720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2856 | 0x00520730 | `sub_520730` | 有正文 | `src/scene/login/LoginSavedState.cpp:775 — RestoreCurrentSavedValue_520730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2857 | 0x00520750 | `sub_520750` | 有正文 | `src/scene/login/LoginSavedState.cpp:873 — Sub_520750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2858 | 0x00520770 | `sub_520770` | 有正文 | `src/scene/login/LoginSavedState.cpp:884 — Sub_520770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2859 | 0x00520790 | `Renderer::SortTexture` | 有正文 | `src/render/GraphicPendingQueue.cpp:395 — SortTextures_520790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2860 | 0x005208F0 | `sub_5208F0` | 有正文 | `src/render/GraphicPendingQueue.cpp:198 — BackupValues_5208F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2861 | 0x00520910 | `sub_520910` | 有正文 | `src/render/GraphicPendingQueue.cpp:208 — Sub_520910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2862 | 0x00520930 | `sub_520930` | 有正文 | `src/render/GraphicPendingQueue.cpp:214 — Sub_520930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2863 | 0x00520970 | `Renderer::DrawGraphic3` | 有正文 | `src/render/DrawGraphic.cpp:50 — DrawGraphic3_520970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2864 | 0x00520AD0 | `Renderer::FillDirtyArea` | 有正文 | `src/render/GraphicPendingQueue.cpp:286 — FillDirtyArea_520AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2865 | 0x00520B10 | `Graphic::LoadTexture` | 有正文 | `src/render/GraphicDxCache.cpp:475 — LoadGraphicTexture_520B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2866 | 0x00520DD0 | `Renderer::DrawAllQueued` | 有正文 | `src/render/DrawAllQueued.cpp:346 — DrawAllQueued_520DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2867 | 0x00521870 | `sub_521870` | 有正文 | `src/render/GraphicPendingQueue.cpp:315 — Sub_521870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2868 | 0x005219C0 | `sub_5219C0` | 有正文 | `src/render/GraphicPendingQueue.cpp:49 — CalculateRippleScreenDelta` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2869 | 0x00521A30 | `Renderer::QueueText` | 有正文 | `src/render/TextPendingQueue.cpp:50 — QueueText_521A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2870 | 0x00521AA0 | `sub_521AA0` | 有正文 | `src/render/GraphicPendingQueue.cpp:229 — QueueMap_521AA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2871 | 0x00521B10 | `Renderer::DrawGraphic` | 有正文 | `src/render/DrawGraphic.cpp:275 — DrawGraphic` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2872 | 0x00521C80 | `sub_521C80` | 有正文 | `src/render/DrawGraphic.cpp:166 — Sub_521C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2873 | 0x00521DD0 | `Renderer::DrawGraphic2` | 有正文 | `src/render/DrawGraphic.cpp:218 — DrawGraphic2_521DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2874 | 0x00521F20 | `World::RenderChara` | 有正文 | `src/world/WorldActorDrawing.cpp:36822 — RenderChara_521F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2875 | 0x00522180 | `Renderer::DrawGraphic_Hover` | 有正文 | `src/render/DrawGraphicHover.cpp:9 — DrawGraphicHover` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2876 | 0x00522210 | `sub_522210` | 有正文 | `src/render/GraphicPendingQueue.cpp:259 — Sub_522210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2877 | 0x005222A0 | `sub_5222A0` | 有正文 | `src/render/GraphicPendingQueue.cpp:30 — CompareGraphicCache3_5222A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2878 | 0x005222D0 | `Renderer::TakeGraphicDxCache` | 有正文 | `src/render/GraphicDxCache.cpp:380 — TakeGraphicDxCache_5222D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2879 | 0x00522570 | `Graphic::LoadGraphic` | 有正文 | `src/render/GraphicDxCache.cpp:314 — LoadGraphic_522570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2880 | 0x00522620 | `sub_522620` | 有正文 | `src/resource/GraphicRead.cpp:333 — InitializeIndependentPaletteQueue_522620` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 2881 | 0x00522630 | `sub_522630` | 有承接 | `src/resource/GraphicRead.cpp:333 — InitializeIndependentPaletteQueue_522620` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 2882 | 0x00522660 | `sub_522660` | 有承接 | `src/resource/GraphicRead.cpp:333 — InitializeIndependentPaletteQueue_522620` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 2883 | 0x00522690 | `sub_522690` | 有正文 | `src/resource/GraphicRead.cpp:341 — InitializePaletteDbfState_522690` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 2884 | 0x005226A0 | `unknown_libname_48` | 有承接 | `src/resource/GraphicRead.cpp:341 — InitializePaletteDbfState_522690` | BCL01原启动点创建同一私有类型静态对象，退出由MSVC原生析构承接；纯访问／手写构析及登记包装退休，原对象、顺序及RNG保持；仅静态，完整异常／编译／运行未验 |
| 2885 | 0x00522900 | `sub_522900` | 有正文 | `src/resource/GraphicRead.cpp:190 — LoadIndependentPaletteIndex_522900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2886 | 0x00522B30 | `sub_522B30` | 有正文 | `src/resource/GraphicRead.cpp:448 — InitializeIndependentPalettes_522B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2887 | 0x00522C50 | `sub_522C50` | 有正文 | `src/resource/GraphicRead.cpp:492 — ReleaseIndependentPalettes_522C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2888 | 0x00522CB0 | `sub_522CB0` | 有正文 | `src/resource/GraphicRead.cpp:256 — LoadIndependentPalette32_522CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2889 | 0x00523140 | `Graphic::LoadPatelet` | 有正文 | `src/resource/GraphicRead.cpp:504 — RefreshIndependentPalette32_523140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2890 | 0x005231A0 | `Renderer::BuildSurfaceCache` | 有正文 | `src/render/GraphicDxCache.cpp:625 — BuildSurfaceCache_5231A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2891 | 0x00523310 | `sub_523310` | 有正文 | `src/render/GraphicDxCache.cpp:270 — ResetTextureNodes_523310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2892 | 0x00523350 | `sub_523350` | 有正文 | `src/render/GraphicDxCache.cpp:286 — InvalidateGraphicRefs_523350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2893 | 0x00523390 | `sub_523390` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2894 | 0x00523450 | `sub_523450` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2895 | 0x005234B0 | `nullsub_2` | 空方法 | `src/resource/GraphicRead.cpp:173 — decodedBlocks.clear();` | 原RET 8非释放叶，指针载荷不归该叶释放，实际vector宿主构析定位保留；按用户决定标为空方法，不再核对此入口 |
| 2896 | 0x005234C0 | `sub_5234C0` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2897 | 0x005235B0 | `sub_5235B0` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | 原迭代器字段操作；当前三个实际 std::map 的迭代器由 STL 承接；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2898 | 0x005235C0 | `sub_5235C0` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2899 | 0x00523690 | `sub_523690` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2900 | 0x005236B0 | `Vector::void::insert` | 有承接 | `src/resource/GraphicRead.cpp:224 — decodedBlocks.push_back(decoded);` | 原 vector<void*> 插入；当前实际 vector push_back，另有图号／请求集合消费；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2901 | 0x005238C0 | `sub_5238C0` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2902 | 0x00523C90 | `sub_523C90` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2903 | 0x00523CD0 | `sub_523CD0` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2904 | 0x00523D20 | `sub_523D20` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2905 | 0x00523F90 | `sub_523F90` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2906 | 0x00523FB0 | `sub_523FB0` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2907 | 0x00523FD0 | `sub_523FD0` | 有承接 | `src/resource/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2908 | 0x00524030 | `sub_524030` | 有承接 | `src/resource/GraphicRead.cpp:160 — std::vector<std::uint8_t*> decodedBlocks;` | 原指针区间拷贝叶；当前实际 vector 维护元素序列；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2909 | 0x00524060 | `sub_524060` | 有正文 | `src/audio/LegacyAudio.cpp:496 — LoadBgmConfig_524060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2910 | 0x005241A0 | `sub_5241A0` | 有正文 | `src/audio/LegacyAudio.cpp:468 — InitializeAudio_5241A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2911 | 0x00524230 | `sub_524230` | 有正文 | `src/audio/LegacyAudio.cpp:368 — Sub_524230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2912 | 0x0052B630 | `sub_52B630` | 有正文 | `src/audio/LegacyAudio.cpp:347 — Sub_52B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2913 | 0x0052B710 | `sub_52B710` | 有正文 | `src/audio/LegacyAudio.cpp:169 — ReadPcmWaveHeader_52B710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2914 | 0x0052B810 | `sub_52B810` | 有承接 | `src/platform/SdlAudioBackend.cpp:113 — LoadSample` | 原音频缓冲写入由实际 WAV 装载／PrepareChannel 承接；完整业务／运行未验 |
| 2915 | 0x0052B880 | `sub_52B880` | 有承接 | `src/platform/SdlAudioBackend.cpp:234 — PrepareChannel` | 原音频缓冲准备由实际 SDL 流与 WAV 装载承接；完整业务／运行未验 |
| 2916 | 0x0052B8E0 | `sub_52B8E0` | 有正文 | `src/audio/LegacyAudio.cpp:316 — Sub_52B8E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2917 | 0x0052B940 | `sub_52B940` | 有正文 | `src/audio/LegacyAudio.cpp:334 — Sub_52B940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2918 | 0x0052B990 | `sub_52B990` | 有正文 | `src/audio/LegacyAudio.cpp:1014 — Sub_52B990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2919 | 0x0052B9E0 | `sub_52B9E0` | 有正文 | `src/audio/LegacyAudio.cpp:629 — Sub_52B9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2920 | 0x0052BBE0 | `sub_52BBE0` | 有承接 | `src/platform/SdlAudioBackend.cpp:158 — FeedChannel` | 原流喂入由实际 SDL 音频回调承接；完整业务／运行未验 |
| 2921 | 0x0052BD40 | `SoundThreadMain` | 有承接 | `src/platform/SdlAudioBackend.cpp:61 — RunBgmStopWorker` | 原音乐线程职责由实际停止线程／音频回调承接；完整业务／运行未验 |
| 2922 | 0x0052BE60 | `World::LoadAndPlayBGM` | 有正文 | `src/audio/LegacyAudio.cpp:1037 — LoadAndPlayBGM` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2923 | 0x0052C160 | `sub_52C160` | 有正文 | `src/audio/LegacyAudio.cpp:282 — LoadStreamingBgm_52C160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2924 | 0x0052C3D0 | `sub_52C3D0` | 有正文 | `src/audio/LegacyAudio.cpp:1122 — Sub_52C3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2925 | 0x0052C450 | `World::PlaySe` | 有正文 | `src/audio/LegacyAudio.cpp:706 — PlaySe_52C450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2926 | 0x0052C680 | `sub_52C680` | 有正文 | `src/audio/LegacyAudio.cpp:789 — Sub_52C680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2927 | 0x0052C910 | `sub_52C910` | 有正文 | `src/audio/LegacyAudio.cpp:558 — UpdateBgmRequest_52C910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2928 | 0x0052C9C0 | `sub_52C9C0` | 有正文 | `src/audio/LegacyAudio.cpp:547 — SelectBgmTarget_52C9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2929 | 0x0052C9E0 | `sub_52C9E0` | 有正文 | `src/audio/LegacyAudio.cpp:667 — Sub_52C9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2930 | 0x0052CB50 | `sub_52CB50` | 有正文 | `src/audio/LegacyAudio.cpp:901 — Sub_52CB50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2931 | 0x0052CCA0 | `sub_52CCA0` | 有正文 | `src/audio/LegacyAudio.cpp:954 — Sub_52CCA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2932 | 0x0052CCE0 | `sub_52CCE0` | 有正文 | `src/audio/LegacyAudio.cpp:606 — Sub_52CCE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2933 | 0x0052CD30 | `sub_52CD30` | 有正文 | `src/platform/MainLoopSupport.cpp:507 — Sub_52CD30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2934 | 0x0052CD40 | `sub_52CD40` | 有正文 | `src/audio/LegacyAudio.cpp:619 — Sub_52CD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2935 | 0x0052CD60 | `sub_52CD60` | 有正文 | `src/audio/LegacyAudio.cpp:987 — Sub_52CD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2936 | 0x0052CDE0 | `sub_52CDE0` | 有正文 | `src/audio/LegacyAudio.cpp:651 — Sub_52CDE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2937 | 0x0052CEA0 | `sub_52CEA0` | 有正文 | `src/network/NetworkRuntime.cpp:1417 — FindNextDelimitedField_52CEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2938 | 0x0052CED0 | `sub_52CED0` | 有正文 | `src/network/NetworkRuntime.cpp:1485 — ExtractDelimitedField_52CED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2939 | 0x0052CF30 | `sub_52CF30` | 有正文 | `src/network/NetworkRuntime.cpp:1444 — CopyDelimitedField_52CF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2940 | 0x0052CFA0 | `sub_52CFA0` | 有正文 | `src/network/NetworkRuntime.cpp:1505 — ParseDelimitedInteger_52CFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2941 | 0x0052CFF0 | `sub_52CFF0` | 有正文 | `src/network/NetworkRuntime.cpp:1520 — ParseDelimitedIntegerStatus_52CFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2942 | 0x0052D050 | `base62Decode` | 有正文 | `src/network/NetworkRuntime.cpp:1537 — Base62Decode_52D050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2943 | 0x0052D0D0 | `sub_52D0D0` | 有正文 | `src/network/NetworkRuntime.cpp:1571 — ParseDelimitedBase62_52D0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2944 | 0x0052D120 | `sub_52D120` | 有正文 | `src/input/Input.cpp:696 — DecodeEscapeLetter_52D120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2945 | 0x0052D150 | `Unescape` | 有正文 | `src/input/Input.cpp:711 — UnescapeLegacyText_52D150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2946 | 0x0052D1B0 | `sub_52D1B0` | 有正文 | `src/input/Input.cpp:585 — EscapeText` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2947 | 0x0052D2D0 | `sub_52D2D0` | 有正文 | `src/input/Input.cpp:685 — EscapeInputText_52D2D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2948 | 0x0052D2F0 | `Unescape2` | 有正文 | `src/input/Input.cpp:740 — UnescapeLegacyTextAndNotify_52D2F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2949 | 0x0052D310 | `sub_52D310` | 有正文 | `src/scene/login/LoginSavedState.cpp:89 — EncodeLegacyBytes_52D310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2950 | 0x0052D460 | `sub_52D460` | 有正文 | `src/scene/login/LoginSavedState.cpp:171 — DecodeLegacyBytes_52D460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2951 | 0x0052D570 | `sub_52D570` | 有正文 | `src/input/Input.cpp:814 — RemoveLegacyBytes_52D570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2952 | 0x0052D5C0 | `Graphic::readGraphicData` | 有正文 | `src/resource/GraphicDecode.cpp:6 — ReadGraphicData` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2953 | 0x0052D790 | `sub_52D790` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2954 | 0x0052D7B0 | `sub_52D7B0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2955 | 0x0052DC60 | `sub_52DC60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2956 | 0x0052DCE0 | `sub_52DCE0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2957 | 0x0052E180 | `sub_52E180` | 有正文 | `src/world/SceneState.cpp:3423 — Sub_52E180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2958 | 0x0052E1D0 | `sub_52E1D0` | 有正文 | `src/world/SceneState.cpp:3441 — Sub_52E1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2959 | 0x0052E240 | `sub_52E240` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2960 | 0x0052E600 | `sub_52E600` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2961 | 0x0052E650 | `sub_52E650` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2962 | 0x0052ECB0 | `sub_52ECB0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2963 | 0x0052ED50 | `sub_52ED50` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2964 | 0x0052F460 | `sub_52F460` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2965 | 0x0052F670 | `sub_52F670` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2966 | 0x0052F910 | `sub_52F910` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2967 | 0x0052FB60 | `sub_52FB60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2968 | 0x0052FCC0 | `sub_52FCC0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2969 | 0x0052FCF0 | `sub_52FCF0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2970 | 0x0052FD60 | `sub_52FD60` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2971 | 0x00530B70 | `sub_530B70` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2972 | 0x00530BA0 | `sub_530BA0` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2973 | 0x00532430 | `sub_532430` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md#legacy-window-cleanup-lwr02 — 当前工程废弃，源码已移除` | LWR02：无集合外消费者，或仅初始化无读者的旧数据；用户授权清理，不证明原端不可达，不计实现／验收 |
| 2974 | 0x00532450 | `sub_532450` | 已排除 | `doc/cg2d_legacy_window_retirement_spec.md — 当前工程废弃，源码已移除` | LWR01：无当前消费者；用户授权废弃，不证明原端不可达，不计实现／验收 |
| 2975 | 0x00532D80 | `sub_532D80` | 有正文 | `src/world/GuildState.cpp:253 — Sub_532D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2976 | 0x00532E70 | `sub_532E70` | 有正文 | `src/platform/StartupCommon.cpp:28 — CreateSaveDirectories` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2977 | 0x00532ED0 | `sub_532ED0` | 有正文 | `src/scene/login/LoginSavedState.cpp:233 — LoadSavedPhysicalRecord_532ED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2978 | 0x00532FC0 | `sub_532FC0` | 有正文 | `src/scene/login/LoginSavedState.cpp:278 — WriteDefaultSavedFile_532FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2979 | 0x005331A0 | `sub_5331A0` | 有正文 | `src/scene/login/LoginSavedState.cpp:392 — InitializeSavedPhysicalRecord_5331A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2980 | 0x00533300 | `sub_533300` | 有正文 | `src/scene/login/LoginSavedState.cpp:808 — InitializeSavedState_533300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2981 | 0x00533360 | `sub_533360` | 有正文 | `src/scene/login/LoginSavedState.cpp:468 — ResetSavedState_533360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2982 | 0x005333D0 | `sub_5333D0` | 有正文 | `src/scene/login/LoginSavedState.cpp:491 — ProbeSavedStateLocation_5333D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2983 | 0x00533470 | `sub_533470` | 有正文 | `src/scene/login/LoginSavedState.cpp:530 — RebuildSavedSlotMap_533470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2984 | 0x00533530 | `sub_533530` | 有正文 | `src/scene/login/LoginSavedState.cpp:580 — MigrateLegacySavedFiles_533530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2985 | 0x00533570 | `sub_533570` | 有正文 | `src/scene/login/LoginSavedState.cpp:1109 — Sub_533570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2986 | 0x005335B0 | `sub_5335B0` | 有正文 | `src/scene/login/LoginSavedState.cpp:1132 — Sub_5335B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2987 | 0x005335F0 | `sub_5335F0` | 有正文 | `src/scene/login/LoginSavedState.cpp:1178 — Sub_5335F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2988 | 0x00533640 | `sub_533640` | 有正文 | `src/scene/login/LoginSavedState.cpp:1155 — Sub_533640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2989 | 0x00533680 | `sub_533680` | 有正文 | `src/scene/login/LoginSavedState.cpp:1205 — Sub_533680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2990 | 0x005336D0 | `sub_5336D0` | 有正文 | `src/scene/login/LoginSavedState.cpp:1064 — Sub_5336D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2991 | 0x00533700 | `sub_533700` | 有正文 | `src/scene/login/LoginSavedState.cpp:610 — StoreCurrentRecordToSelectedPhysical_533700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2992 | 0x00533730 | `sub_533730` | 有正文 | `src/scene/login/LoginSavedState.cpp:1232 — Sub_533730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2993 | 0x00533860 | `sub_533860` | 有正文 | `src/scene/login/LoginSavedState.cpp:1070 — Sub_533860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2994 | 0x005338F0 | `sub_5338F0` | 有正文 | `src/scene/login/LoginSavedState.cpp:519 — FindFreeSavedPhysicalSlot_5338F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2995 | 0x00533910 | `sub_533910` | 有正文 | `src/world/MailState.cpp:832 — Sub_533910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2996 | 0x00533950 | `sub_533950` | 有正文 | `src/world/MailState.cpp:843 — Sub_533950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2997 | 0x00533B00 | `sub_533B00` | 有正文 | `src/world/MailState.cpp:869 — Sub_533B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2998 | 0x00533C80 | `sub_533C80` | 有正文 | `src/world/MailState.cpp:898 — Sub_533C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2999 | 0x00534290 | `sub_534290` | 有正文 | `src/world/MailState.cpp:984 — Sub_534290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3000 | 0x005342E0 | `sub_5342E0` | 有正文 | `src/ui/windows/WindowProfileReply.cpp:132 — WindowProfileReply_Open` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3001 | 0x005342F0 | `sub_5342F0` | 有正文 | `src/world/MailState.cpp:1090 — Sub_5342F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3002 | 0x00534370 | `sub_534370` | 有正文 | `src/world/MailState.cpp:640 — Sub_534370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3003 | 0x005343A0 | `sub_5343A0` | 有正文 | `src/world/MailState.cpp:652 — Sub_5343A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3004 | 0x005343C0 | `sub_5343C0` | 有正文 | `src/world/MailState.cpp:695 — Sub_5343C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3005 | 0x00534480 | `sub_534480` | 有正文 | `src/world/MailState.cpp:716 — Sub_534480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3006 | 0x00534520 | `sub_534520` | 有正文 | `src/world/MailState.cpp:670 — Sub_534520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3007 | 0x005345A0 | `sub_5345A0` | 有正文 | `src/world/MailState.cpp:1283 — Sub_5345A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3008 | 0x00534770 | `sub_534770` | 有正文 | `src/world/MailState.cpp:1313 — Sub_534770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3009 | 0x005347F0 | `sub_5347F0` | 有正文 | `src/world/MailState.cpp:1330 — Sub_5347F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3010 | 0x005348A0 | `sub_5348A0` | 有正文 | `src/world/MailState.cpp:1249 — Sub_5348A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3011 | 0x00534910 | `sub_534910` | 有正文 | `src/world/MailState.cpp:1266 — Sub_534910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3012 | 0x00534980 | `DirectDrawCreateEx` | 有承接 | `src/render/SdlPresenter.cpp:364 — CreateRendererPresenter` | 原 DirectDrawCreateEx import 跳板；当前渲染创建由实际 SDL presenter 承接；完整上层／运行未验 |
| 3013 | 0x00534986 | `ImmGetConversionStatus` | 有承接 | `src/platform/imeImpl.cpp:64 — ImmGetConversionStatus` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3014 | 0x0053498C | `ImmAssociateContext` | 有承接 | `src/platform/imeImpl.cpp:80 — ImmAssociateContext` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3015 | 0x00534992 | `ImmCreateContext` | 有承接 | `src/platform/imeImpl.cpp:55 — ImmCreateContext` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3016 | 0x00534998 | `ImmReleaseContext` | 有承接 | `src/platform/imeImpl.cpp:77 — ImmReleaseContext` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3017 | 0x0053499E | `ImmIsIME` | 有承接 | `src/platform/imeImpl.cpp:92 — ImmIsIME` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3018 | 0x005349A4 | `ImmSetOpenStatus` | 有承接 | `src/platform/imeImpl.cpp:106 — ImmSetOpenStatus` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3019 | 0x005349AA | `ImmGetDescriptionA` | 有承接 | `src/world/RenderInput.cpp:195 — ImmGetDescriptionW` | 原入口为对应 Windows／Winsock import 跳板；原A入口由UTL02对应W调用承接，当前真实SDK调用存在，完整调用方／运行未验 |
| 3020 | 0x005349B0 | `ImmGetCompositionStringA` | 有承接 | `src/platform/imeImpl.cpp:29 — ImmGetCompositionStringW` | 原入口为对应 Windows／Winsock import 跳板；原A入口由UTL02对应W调用承接，当前真实SDK调用存在，完整调用方／运行未验 |
| 3021 | 0x005349B6 | `ImmGetCandidateListA` | 有承接 | `src/platform/WindowProcedure.cpp:424 — ImmGetCandidateListW` | 原入口为对应 Windows／Winsock import 跳板；原A入口由UTL02对应W调用承接，当前真实SDK调用存在，完整调用方／运行未验 |
| 3022 | 0x00534A30 | `send` | 有承接 | `src/network/NetworkRuntime.cpp:107 — send` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3023 | 0x00534A36 | `closesocket` | 有承接 | `src/network/NetworkRuntime.cpp:4615 — closesocket` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3024 | 0x00534A3C | `recv` | 有承接 | `src/network/NetworkRuntime.cpp:4597 — recv` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3025 | 0x00534A42 | `__WSAFDIsSet` | 有承接 | `src/network/NetworkRuntime.cpp:4593 — FD_ISSET` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3026 | 0x00534A48 | `select` | 有承接 | `src/network/NetworkRuntime.cpp:853 — select` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3027 | 0x00534A4E | `WSAStartup` | 有承接 | `src/network/NetworkRuntime.cpp:4530 — WSAStartup` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3028 | 0x00534A54 | `WSACleanup` | 有承接 | `src/network/NetworkRuntime.cpp:4812 — WSACleanup` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3029 | 0x00534A5A | `WSAGetLastError` | 有承接 | `src/network/NetworkRuntime.cpp:729 — WSAGetLastError` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3030 | 0x00534A60 | `connect` | 有承接 | `src/LoginState.cpp:106 — connect` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3031 | 0x00534A66 | `gethostbyname` | 有承接 | `src/LoginState.cpp:135 — gethostbyname` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3032 | 0x00534A6C | `inet_addr` | 有承接 | `src/LoginState.cpp:133 — inet_addr` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3033 | 0x00534A72 | `htons` | 有承接 | `src/LoginState.cpp:132 — htons` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3034 | 0x00534A78 | `setsockopt` | 有承接 | `src/LoginState.cpp:120 — setsockopt` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3035 | 0x00534A7E | `ioctlsocket` | 有承接 | `src/LoginState.cpp:116 — ioctlsocket` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3036 | 0x00534A84 | `socket` | 有承接 | `src/LoginState.cpp:106 — socket` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3037 | 0x00534A8A | `Module32Next` | 有承接 | `src/platform/MainLoopSupport.cpp:176 — Module32Next` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3038 | 0x00534A90 | `Module32First` | 有承接 | `src/platform/MainLoopSupport.cpp:152 — Module32First` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3039 | 0x00534A96 | `CreateToolhelp32Snapshot` | 有承接 | `src/platform/MainLoopSupport.cpp:145 — CreateToolhelp32Snapshot` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3040 | 0x00534A9C | `Process32Next` | 有承接 | `src/platform/MainLoopSupport.cpp:342 — Process32Next` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3041 | 0x00534AA2 | `Process32First` | 有承接 | `src/platform/MainLoopSupport.cpp:336 — Process32First` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3042 | 0x00534AA8 | `DirectSoundCreate` | 有承接 | `src/platform/SdlAudioBackend.cpp:285 — SdlAudioBackend::Open` | 原 DirectSoundCreate 为 import 跳板；当前 Sub_524230 实际调用 SDL 后端 Open；不造旧 SDK 包装，完整上层／运行未验 |
| 3043 | 0x00534AB0 | `sub_534AB0` | 已排除 | `src/platform/LegacyExternalKeyboard.cpp:171 — Sub_534AB0` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3044 | 0x00534B80 | `DialogFunc` | 已排除 | `src/platform/LegacyExternalKeyboard.cpp:45 — DialogFunc_534B80` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3045 | 0x00534EF0 | `sub_534EF0` | 有正文 | `src/platform/platformTool.cpp:15 — GetMacAddress_534EF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3046 | 0x00535090 | `sub_535090` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3047 | 0x00535290 | `sub_535290` | 已排除 | `src/platform/LegacyExternalKeyboardObject.cpp:19 — DeleteExternalKeyboardVirtual_535290` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3048 | 0x00535310 | `sub_535310` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3049 | 0x005354C0 | `sub_5354C0` | 已排除 | `src/platform/LegacyExternalKeyboardObject.cpp:41 — GetExternalKeyboardResult_5354C0` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3050 | 0x005354D0 | `sub_5354D0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3051 | 0x00535540 | `sub_535540` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3052 | 0x00536960 | `nullsub_4` | 空方法 | `src/platform/RuntimeInitialization.cpp:131 — EmptyLegacyExit_536960` | 已有CPP空方法与原RET记录保留；按用户决定标为空方法，不再核对此入口 |
| 3053 | 0x00536970 | `sub_536970` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3054 | 0x00536C30 | `sub_536C30` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3055 | 0x00536EA0 | `sub_536EA0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3056 | 0x00536EE0 | `sub_536EE0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3057 | 0x00536F00 | `sub_536F00` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3058 | 0x00536FD0 | `sub_536FD0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3059 | 0x00537090 | `sub_537090` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3060 | 0x005371A0 | `sub_5371A0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3061 | 0x00537220 | `sub_537220` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3062 | 0x005375F0 | `sub_5375F0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3063 | 0x00537660 | `sub_537660` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3064 | 0x005378F0 | `sub_5378F0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3065 | 0x00537920 | `sub_537920` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3066 | 0x00537950 | `sub_537950` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3067 | 0x00537980 | `sub_537980` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3068 | 0x005379E0 | `sub_5379E0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3069 | 0x00537A30 | `sub_537A30` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3070 | 0x00537A40 | `sub_537A40` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3071 | 0x00537A70 | `sub_537A70` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3072 | 0x00537AE0 | `sub_537AE0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3073 | 0x00537AF0 | `sub_537AF0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3074 | 0x00537B10 | `sub_537B10` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3075 | 0x00537B20 | `sub_537B20` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3076 | 0x00537B80 | `sub_537B80` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3077 | 0x00537D30 | `sub_537D30` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3078 | 0x00537D40 | `sub_537D40` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3079 | 0x00537D50 | `sub_537D50` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3080 | 0x00537D60 | `sub_537D60` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3081 | 0x00537D70 | `sub_537D70` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3082 | 0x00537D90 | `sub_537D90` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3083 | 0x00537DA0 | `sub_537DA0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3084 | 0x00537DC0 | `sub_537DC0` | 已排除 | `src/platform/LegacyExternalKeyboardObject.cpp:34 — QueryExternalKeyboardSizeVirtual_537DC0` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3085 | 0x00537DE0 | `sub_537DE0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3086 | 0x00537E40 | `sub_537E40` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3087 | 0x00537E60 | `sub_537E60` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3088 | 0x00537F40 | `sub_537F40` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3089 | 0x00537FC0 | `sub_537FC0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3090 | 0x00537FE0 | `fn` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3091 | 0x00538170 | `sub_538170` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3092 | 0x00538200 | `sub_538200` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3093 | 0x00538270 | `sub_538270` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3094 | 0x005382E0 | `sub_5382E0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3095 | 0x00538390 | `sub_538390` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3096 | 0x005384A0 | `sub_5384A0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3097 | 0x00538870 | `sub_538870` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3098 | 0x00538940 | `sub_538940` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3099 | 0x00538960 | `sub_538960` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3100 | 0x005389D0 | `sub_5389D0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3101 | 0x00538C60 | `sub_538C60` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3102 | 0x00538C90 | `sub_538C90` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3103 | 0x00538CF0 | `sub_538CF0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3104 | 0x00538D40 | `sub_538D40` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3105 | 0x00538F60 | `sub_538F60` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3106 | 0x00539070 | `sub_539070` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3107 | 0x005390D0 | `sub_5390D0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3108 | 0x00539100 | `sub_539100` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3109 | 0x00539110 | `sub_539110` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3110 | 0x00539120 | `sub_539120` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3111 | 0x00539270 | `sub_539270` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3112 | 0x00539450 | `sub_539450` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3113 | 0x00539490 | `sub_539490` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3114 | 0x00539540 | `sub_539540` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3115 | 0x00539630 | `sub_539630` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3116 | 0x005396A0 | `sub_5396A0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3117 | 0x005397C0 | `sub_5397C0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3118 | 0x005397F0 | `sub_5397F0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3119 | 0x00539870 | `sub_539870` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3120 | 0x00539980 | `sub_539980` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3121 | 0x00539A50 | `sub_539A50` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3122 | 0x00539E20 | `sub_539E20` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3123 | 0x00539E90 | `sub_539E90` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3124 | 0x0053A120 | `sub_53A120` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3125 | 0x0053A150 | `sub_53A150` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3126 | 0x0053A1B0 | `sub_53A1B0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3127 | 0x0053A200 | `sub_53A200` | 有承接 | `src/resource/GraphicRead.cpp:237 — records.try_emplace(readWord(record), nullptr)；src/resource/GraphicInfo.cpp:199 — BorrowGraphicFirstRecords().emplace(firstMapNo, firstRecord)` | 原 9 条指令为非空目标的两 W32 pair 复制，直接原引用含 45AF60／523D20；当前对应 map 插入构造实际 pair，键宽保持、指针原生拓宽；完整 owner／运行未验 |
| 3128 | 0x0053A220 | `sub_53A220` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3129 | 0x0053A3D0 | `sub_53A3D0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3130 | 0x0053A470 | `sub_53A470` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3131 | 0x0053A490 | `sub_53A490` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3132 | 0x0053A4D0 | `sub_53A4D0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3133 | 0x0053A500 | `sub_53A500` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3134 | 0x0053A5D0 | `sub_53A5D0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3135 | 0x0053A9A0 | `sub_53A9A0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3136 | 0x0053AA20 | `sub_53AA20` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3137 | 0x0053AB30 | `sub_53AB30` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3138 | 0x0053ABB0 | `sub_53ABB0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3139 | 0x0053AC20 | `sub_53AC20` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3140 | 0x0053AEB0 | `sub_53AEB0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3141 | 0x0053AEE0 | `sub_53AEE0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3142 | 0x0053AF40 | `sub_53AF40` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3143 | 0x0053AF90 | `sub_53AF90` | 有正文 | `src/resource/GraphicRead.cpp:349 — DecompressDbf_53AF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3144 | 0x0053B1D0 | `_fclose` | 有承接 | `src/world/Blacklist.cpp:63 — std::fclose（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3145 | 0x0053B226 | `_fgets` | 有承接 | `src/world/Blacklist.cpp:90 — std::fgets（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3146 | 0x0053B27D | `_rewind` | 有承接 | `src/world/Blacklist.cpp:87 — std::rewind（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3147 | 0x0053B2D1 | `__fsopen` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：::_fsopen（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3148 | 0x0053B2F1 | `_fopen` | 有承接 | `src/world/Blacklist.cpp:53 — std::fopen（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3149 | 0x0053B304 | `_sprintf` | 有承接 | `src/scene/login/CharacterListRequest.cpp:160 — std::sprintf` | DCF01直接MSVC／标准库承接，原包装退休；现有800处调用及原合同保持；完整业务／运行未验 |
| 3150 | 0x0053B356 | `_fputs` | 有承接 | `src/world/Blacklist.cpp:59 — std::fputs（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3151 | 0x0053B399 | `__onexit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3152 | 0x0053B406 | `_atexit` | 有承接 | `src/platform/RuntimeInitialization.cpp:141 — RegisterLegacyPrimaryEmptyExit` | 原库入口沿真实std::atexit消费者／MSVC服务承接；CryptInt 24组另由LCI01原生构析承接，无手写登记；未记机器ABI或运行已验 |
| 3153 | 0x0053B418 | `___onexitinit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3154 | 0x0053B447 | `sub_53B447` | 有承接 | `CMakeLists.txt:161 — 宿主CRT启动；src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原53C8CD在C／C++初始化前从58F864间接调用；旧六回调／CPU探测／x87默认精度随MC01-C2退休，当前MSVC整体启动／格式／扫描服务；不新增旧回调槽或声明硬件状态等价 |
| 3155 | 0x0053B45E | `nullsub_6` | 空方法 | `—` | 原入口单RET，58F868／58F86C两数据槽记录保留；按用户决定标为空方法，结束职责／调用／承接核对，不补CPP包装 |
| 3156 | 0x0053B45F | `__cfltcvt_init_7` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3157 | 0x0053B498 | `__ftol` | 有承接 | `src/render/Palette.cpp:138 — TruncatedLowByte；src/render/LegacyTextureDrawing.cpp:132 — TruncateTextureCoordinate` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3158 | 0x0053B4BF | `sub_53B4BF` | 有正文 | `src/platform/MainLoopSupport.cpp:599 — SeedRandom_53B4BF` | LCG01标准线性同余引擎承接，原序列／共享状态保持；完整业务／接线／运行未验 |
| 3159 | 0x0053B4C9 | `rand` | 有正文 | `src/platform/MainLoopSupport.cpp:606 — NextRandom_53B4C9` | LCG01标准线性同余引擎承接，原序列／共享状态保持；完整业务／接线／运行未验 |
| 3160 | 0x0053B4E7 | `delete` | 有承接 | `src/platform/platformTool.cpp:34 — std::free` | 当前 IDA 的原 delete 叶调用 free；本线实际通用释放使用 std::free，与已批准 malloc 分配族成组承接；完整 owner／运行未验 |
| 3161 | 0x0053B4F2 | `?_JumpToContinuation@@YGXPAXPAUEHRegistrationNode@@@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3162 | 0x0053B526 | `sub_53B526` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3163 | 0x0053B52D | `sub_53B52D` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3164 | 0x0053B534 | `?_UnwindNestedFrames@@YGXPAUEHRegistrationNode@@PAUEHExceptionRecord@@@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3165 | 0x0053B583 | `___CxxFrameHandler` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3166 | 0x0053B5B9 | `?_CallCatchBlock2@@YAPAXPAUEHRegistrationNode@@PBU_s_FuncInfo@@PAXHK@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3167 | 0x0053B60D | `?CatchGuardHandler@@YA?AW4_EXCEPTION_DISPOSITION@@PAUEHExceptionRecord@@PAUCatchGuardRN@@PAX2@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3168 | 0x0053B632 | `?_CallSETranslator@@YAHPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAX2PBU_s_FuncInfo@@H1@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3169 | 0x0053B6E6 | `?TranslatorGuardHandler@@YA?AW4_EXCEPTION_DISPOSITION@@PAUEHExceptionRecord@@PAUTranslatorGuardRN@@PAX2@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3170 | 0x0053B75B | `?_GetRangeOfTrysToCheck@@YAPBU_s_TryBlockMapEntry@@PBU_s_FuncInfo@@HHPAI1@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3171 | 0x0053B7D8 | `__global_unwind2` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3172 | 0x0053B7F8 | `__unwind_handler` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3173 | 0x0053B81A | `__local_unwind2` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3174 | 0x0053B882 | `__abnormal_termination` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3175 | 0x0053B8A5 | `__NLG_Notify1` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3176 | 0x0053B8AE | `__NLG_Notify` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3177 | 0x0053B8C6 | `_localtime` | 有承接 | `src/world/ChatHistory.cpp:142 — ::_localtime32` | COR14直接借用CRT线程共享std::tm；旧私有副本退休，完整业务／运行未验 |
| 3178 | 0x0053BA26 | `_time` | 有承接 | `src/world/GameTime.cpp:18 — ::_time32` | COR14直接取32位CRT时间；原发布与W32业务边界保持，运行未验 |
| 3179 | 0x0053BB02 | `_fflush` | 有承接 | `src/input/Input.cpp:1163 — std::fflush（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3180 | 0x0053BB3D | `__flush` | 有承接 | `src/input/Input.cpp:1163 — std::fflush（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3181 | 0x0053BB99 | `sub_53BB99` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：::_flushall（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3182 | 0x0053BBA2 | `_flsall` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：::_flushall（标准库提供；无CPP消费者）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3183 | 0x0053BC0F | `sub_53BC0F` | 有承接 | `src/input/Input.cpp:1184 — std::fprintf` | DCF01直接MSVC／标准库承接，原包装退休；现有11处调用及原合同保持；完整业务／运行未验 |
| 3184 | 0x0053BC41 | `_fseek` | 有承接 | `src/resource/GraphicInfo.cpp:99 — std::fseek（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3185 | 0x0053BCCD | `_fread` | 有承接 | `src/resource/GraphicInfo.cpp:154 — std::fread（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3186 | 0x0053BDC0 | `__alloca_probe` | 有承接 | `doc/cg2d_crt_host_migration_spec.md — MC01：MSVC编译器栈探测支撑；doc/cg2d_network_plaintext_frame_spec.md — NKF01：旧帧源码定位退休` | MSVC宿主承接状态保持，移除已退休旧帧的源码定位；不伪造新调用或原机器ABI承接，不计当前编译产物／运行已验 |
| 3187 | 0x0053BDEF | `_isspace` | 有承接 | `src/input/Input.cpp:548 — std::isspace` | UWF01邮件分词直接使用wcstok，当前窄strtok已无消费者；原纯转发包装退休；完整业务／运行未验 |
| 3188 | 0x0053BE17 | `_iscntrl` | 有承接 | `src/input/Input.cpp:551 — std::iscntrl` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3189 | 0x0053BE40 | `_strstr` | 有承接 | `src/CharacterSelectScene.cpp:919 — std::strstr` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3190 | 0x0053BEC0 | `_strncpy` | 有承接 | `src/platform/MainLoopSupport.cpp:430 — std::strncpy` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3191 | 0x0053BFC0 | `_strncmp` | 有承接 | `src/world/WorldActorDrawing.cpp:35355 — std::strncmp` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3192 | 0x0053BFF8 | `cAlloc` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原包装已按COR08退休，服务由MSVC／标准库提供；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3193 | 0x0053C0A9 | `free` | 有承接 | `src/platform/platformTool.cpp:34 — std::free` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3194 | 0x0053C112 | `_qsort` | 有承接 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420；src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3195 | 0x0053C266 | `_shortsort` | 有承接 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420；src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3196 | 0x0053C2B4 | `_swap` | 有承接 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420；src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3197 | 0x0053C2F0 | `_strchr` | 有承接 | `src/LoginCapability4Modal.cpp:90 — std::wcschr` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3198 | 0x0053C3AC | `_malloc` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3199 | 0x0053C3BE | `__nh_malloc` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3200 | 0x0053C3EA | `sub_53C3EA` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc` | 原仅__nh_malloc调用并转旧两池／HeapAlloc；实际std::malloc整体承接分配及已批库内策略，不复刻模式，业务／owner／运行未验 |
| 3201 | 0x0053C45E | `??_L@YGXPAXIHP6EX0@Z1@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:54 — cg::world::detail::gWorldObjects` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 3202 | 0x0053C4E0 | `??_M@YGXPAXIHP6EX0@Z@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:54 — cg::world::detail::gWorldObjects` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 3203 | 0x0053C560 | `?__ArrayUnwind@@YGXPAXIHP6EX0@Z@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:54 — cg::world::detail::gWorldObjects` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 3204 | 0x0053C5BE | `_atol` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：std::atol` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3205 | 0x0053C649 | `_atoi` | 有承接 | `src/LoginScene.cpp:342 — std::atoi` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3206 | 0x0053C654 | `__mkdir` | 有承接 | `src/input/Input.cpp:1093 — ::_mkdir` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3207 | 0x0053C680 | `_fwrite` | 有承接 | `src/input/Input.cpp:1041 — std::fwrite（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3208 | 0x0053C790 | `_strncat` | 有承接 | `src/input/Input.cpp:260 — AppendText（UTF-16 STL正文插入）` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3209 | 0x0053C8B3 | `_fgetc` | 有承接 | `src/render/Palette.cpp:229 — std::fgetc（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3210 | 0x0053C8CD | `__cinit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/RuntimeInitialization.cpp:72 — gSurfaceTransformCacheStartup` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3211 | 0x0053C8FA | `_exit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3212 | 0x0053C90B | `__exit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3213 | 0x0053C91C | `_doexit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3214 | 0x0053C9B5 | `__initterm` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3215 | 0x0053C9CF | `??2@YAPAXI@Z` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc；src/platform/platformTool.cpp:29 — std::malloc` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3216 | 0x0053C9DD | `_mbstowcs` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3217 | 0x0053CADC | `_sscanf` | 有承接 | `src/platform/StartupCommon.cpp:16 — std::sscanf` | DCF01直接MSVC／标准库承接，原包装退休；现有21处调用及原合同保持；完整业务／运行未验 |
| 3218 | 0x0053CB10 | `__aulldiv` | 有承接 | `src/platform/MainLoopSupport.cpp:711 — U64整数除法（COR08直接运算）` | 原包装已按COR08退休，服务由MSVC／标准库提供；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3219 | 0x0053CB78 | `_ftell` | 有承接 | `src/resource/GraphicInfo.cpp:100 — std::ftell（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3220 | 0x0053CCD0 | `__fstat` | 有承接 | `src/resource/ResourcePaths.cpp:36 — ::_fstat32(::_fileno(file), &status)` | COR14在原调用点直接查询创建时间，成功才发布st_ctime；完整运行未验 |
| 3221 | 0x0053CF50 | `__strcmpi` | 有承接 | `src/resource/AnimationRuntime.cpp:393 — ::_stricmp` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3222 | 0x0053CFDC | `_fputc` | 有承接 | `src/world/SceneState.cpp:4279 — std::fputc（直接CRT调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3223 | 0x0053D000 | `__CIpow` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3224 | 0x0053D019 | `_pow` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3225 | 0x0053D022 | `sub_53D022` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3226 | 0x0053D1F5 | `sub_53D1F5` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3227 | 0x0053D21D | `__snprintf` | 有承接 | `include/cg/text/Encoding.hpp:20 — FormatTo` | UWF01最后7处私有显示缓冲改动态宽格式化，原有界窄调用已无消费者；GBK存档／协议容量保持；完整业务／运行未验 |
| 3228 | 0x0053D26E | `_strtok` | 有承接 | `src/ui/windows/WindowConfirmation.cpp:209 — std::wcstok` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3229 | 0x0053D30A | `__close` | 有承接 | `src/ui/WindowRuntime.cpp:2748 — ::_close（MSVC描述符服务）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3230 | 0x0053D3BD | `__filelength` | 有承接 | `src/ui/WindowRuntime.cpp:2746 — ::_filelength（MSVC描述符服务）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3231 | 0x0053D435 | `__open` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3232 | 0x0053D44C | `__sopen` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3233 | 0x0053D710 | `_memcpy` | 有承接 | `src/network/NetworkRuntime.cpp:4732 — ConsumeReceiveQueue_508230；src/network/NetworkRuntime.cpp:4751 — ConsumeSendQueue_508280` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3234 | 0x0053DA45 | `_strtol` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：std::strtol` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3235 | 0x0053DA5C | `_strtoxl` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3236 | 0x0053DC64 | `_tolower` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3237 | 0x0053DD2F | `__chdir` | 有承接 | `src/platform/StartupCommon.cpp:31 — ::_chdir` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3238 | 0x0053DDB5 | `start` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3239 | 0x0053DEAB | `__amsg_exit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3240 | 0x0053DED0 | `_fast_error_exit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3241 | 0x0053DEF3 | `nullsub_7` | 空方法 | `—` | 原入口单RET、当前无已建入引用的记录保留；按用户决定标为空方法，结束职责／调用／承接核对，不补CPP包装 |
| 3242 | 0x0053DEF4 | `__input` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | DCF01直接MSVC／标准库承接，原包装退休；无当前CPP消费者，头文件导入真实MSVC CRT提供者；完整业务／运行未验 |
| 3243 | 0x0053E919 | `__hextodec` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3244 | 0x0053E950 | `_fgetc_0` | 有承接 | `src/render/Palette.cpp:229 — std::fgetc（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3245 | 0x0053E96A | `__un_inc` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3246 | 0x0053E981 | `__whiteout` | 有承接 | `src/render/Palette.cpp:229 — std::fgetc（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3247 | 0x0053E9A5 | `___initstdio` | 有承接 | `src/world/Blacklist.cpp:53 — std::fopen（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3248 | 0x0053EA4A | `___endstdio` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3249 | 0x0053EA5E | `__stbuf` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3250 | 0x0053EAEB | `__ftbuf` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3251 | 0x0053EB28 | `sub_53EB28` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | DCF01直接MSVC／标准库承接，原包装退休；无当前CPP消费者，头文件导入真实MSVC CRT提供者；完整业务／运行未验 |
| 3252 | 0x0053F2C6 | `_write_char` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3253 | 0x0053F2FB | `_write_multi_char` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3254 | 0x0053F32C | `_write_string` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3255 | 0x0053F364 | `_get_int_arg` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3256 | 0x0053F371 | `_get_int64_arg` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3257 | 0x0053F381 | `_get_short_arg` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3258 | 0x0053F38F | `__freebuf` | 有承接 | `src/world/Blacklist.cpp:53 — std::fopen（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3259 | 0x0053F3BA | `__filbuf` | 有承接 | `src/render/Palette.cpp:229 — std::fgetc（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3260 | 0x0053F493 | `__lseek` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3261 | 0x0053F52D | `__ioinit` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3262 | 0x0053F6D8 | `__openfile` | 有承接 | `src/world/Blacklist.cpp:53 — std::fopen（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3263 | 0x0053F848 | `__getstream` | 有承接 | `src/world/Blacklist.cpp:53 — std::fopen（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3264 | 0x0053F8C0 | `__flsbuf` | 有承接 | `src/world/Blacklist.cpp:53 — std::fopen（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3265 | 0x0053F9E0 | `_strlen` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：std::strlen` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3266 | 0x0053FA5B | `sub_53FA5B` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原包装已按COR08退休，服务由MSVC／标准库提供；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3267 | 0x0053FCFB | `sub_53FCFB` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原包装已按COR08退休，服务由MSVC／标准库提供；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3268 | 0x0053FD5C | `__setdefaultprecision` | 有承接 | `src/world/WorldActorDrawing.cpp:36822 — RenderChara_521F20；src/world/SceneState.cpp:9702 — Sub_51D790` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3269 | 0x0053FD6E | `__ms_p5_test_fdiv` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3270 | 0x0053FDAC | `__ms_p5_mp_test_fdiv` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3271 | 0x0053FDD5 | `__forcdecpt` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3272 | 0x0053FE2F | `__cropzeros` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3273 | 0x0053FE7D | `__positive` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3274 | 0x0053FE95 | `__fassign` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原输入扫描的浮点赋值回调，登记58FB58并由53E324调用，实际std::vfscanf／std::vsscanf承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3275 | 0x0053FED3 | `__cftoe` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3276 | 0x0053FFD7 | `__cftof` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3277 | 0x005400B5 | `__cftog` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3278 | 0x00540150 | `__cftoe_g` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3279 | 0x00540177 | `__cftof_g` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3280 | 0x0054019A | `__cfltcvt` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3281 | 0x005401EB | `__shift` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3282 | 0x00540210 | `___InternalCxxFrameHandler` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3283 | 0x005402AB | `?FindHandler@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PAXPBU_s_FuncInfo@@EH1@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3284 | 0x00540452 | `?FindHandlerForForeignException@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PAXPBU_s_FuncInfo@@HH1@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3285 | 0x005404FA | `?TypeMatch@@YAHPBU_s_HandlerType@@PBU_s_CatchableType@@PBU_s_ThrowInfo@@@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3286 | 0x00540557 | `___FrameUnwindToState` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3287 | 0x005405F5 | `unknown_libname_49` | 有承接 | `src/platform/RuntimeInitialization.cpp:54 — cg::world::detail::gWorldObjects` | LCI18同一中央位置直接定义真实复合对象；构造／逆序析构及完成前缀清理由C++／MSVC承接，Borrow／optional与纯启动包装退休，原RNG／字段／读写序保持；仅静态，完整异常／编译／运行未验 |
| 3288 | 0x0054060B | `?CatchIt@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PAXPBU_s_FuncInfo@@PBU_s_HandlerType@@PBU_s_CatchableType@@PBU_s_TryBlockMapEntry@@H1E@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3289 | 0x00540686 | `?CallCatchBlock@@YAPAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PBU_s_FuncInfo@@PAXHK@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3290 | 0x00540792 | `?ExFilterRethrow@@YAHPAU_EXCEPTION_POINTERS@@@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3291 | 0x005407BC | `?BuildCatchObject@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PBU_s_HandlerType@@PBU_s_CatchableType@@@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3292 | 0x00540980 | `unknown_libname_50` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3293 | 0x005409E7 | `?AdjustPointer@@YAPAXPAXABUPMD@@@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3294 | 0x00540A10 | `__CallSettingFrame@12` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3295 | 0x00540A5C | `sub_540A5C` | 有承接 | `src/render/SdlPresenter.cpp:120 — std::terminate（exception导入；旧包装无调用）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3296 | 0x00540AB2 | `sub_540AB2` | 有承接 | `CMakeLists.txt:161 — 宿主VCRuntime` | 原八处引用均在C++ handler／展开链；读58FB74=540A5C、SEH保护后交终止；当前VCRuntime／实际std::terminate承接，不把标准流构造当本叶直接实现 |
| 3297 | 0x00540B08 | `___tzset` | 有承接 | `src/world/ChatHistory.cpp:142 — ::_localtime32；doc/cg2d_code_organization_review_spec.md — COR14：无消费者_mktime32映射` | COR14直接使用::_localtime32承接当前时区／夏令时链；旧反向转换无消费者，宿主_mktime32仅保留规格映射；库内策略与完整运行未验 |
| 3298 | 0x00540B1D | `__tzset` | 有承接 | `src/world/ChatHistory.cpp:142 — ::_localtime32；doc/cg2d_code_organization_review_spec.md — COR14：无消费者_mktime32映射` | COR14直接使用::_localtime32承接当前时区／夏令时链；旧反向转换无消费者，宿主_mktime32仅保留规格映射；库内策略与完整运行未验 |
| 3299 | 0x00540D7B | `__isindst` | 有承接 | `src/world/ChatHistory.cpp:142 — ::_localtime32；doc/cg2d_code_organization_review_spec.md — COR14：无消费者_mktime32映射` | COR14直接使用::_localtime32承接当前时区／夏令时链；旧反向转换无消费者，宿主_mktime32仅保留规格映射；库内策略与完整运行未验 |
| 3300 | 0x00540F27 | `_cvtdate` | 有承接 | `src/world/ChatHistory.cpp:142 — ::_localtime32；doc/cg2d_code_organization_review_spec.md — COR14：无消费者_mktime32映射` | COR14直接使用::_localtime32承接当前时区／夏令时链；旧反向转换无消费者，宿主_mktime32仅保留规格映射；库内策略与完整运行未验 |
| 3301 | 0x00541067 | `_gmtime` | 有承接 | `src/world/ChatHistory.cpp:142 — ::_localtime32` | 原UTC分解仅在旧localtime内三处调用；当前实际::_localtime32整体承接该日历转换链，不宣称直接调用::_gmtime32或新增UTC接口，std::tm借用对象及完整寿命／运行未验 |
| 3302 | 0x00541160 | `___loctotime_t` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：::_mktime32（宿主服务，无当前CPP消费者）` | COR14移除无消费者的ConvertLocalTime包装；保持已批准MSVC宿主映射，不计业务接线或运行通过 |
| 3303 | 0x00541222 | `__commit` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3304 | 0x00541279 | `__write` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3305 | 0x00541426 | `__read` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3306 | 0x00541620 | `_memcpy_0` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原包装已按COR08退休，服务由MSVC／标准库提供；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3307 | 0x00541955 | `__isctype` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3308 | 0x005419D0 | `_memset` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原包装已按COR08退休，服务由MSVC／标准库提供；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3309 | 0x00541A28 | `__callnewh` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3310 | 0x00541A43 | `sub_541A43` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc；CMakeLists.txt:161 — 宿主Windows CRT进入WinMain` | 原仅堆选择器541A70使用的模块MZ／PE两字节读取；旧选择机制随MSVC启动／堆策略承接，不新增模块头API，非原机制等价／运行通过 |
| 3311 | 0x00541A70 | `sub_541A70` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc；CMakeLists.txt:161 — 宿主Windows CRT进入WinMain` | 原仅541BB8调用的OS／环境／模块堆选择；当前MSVC启动与实际标准分配库整体承接，不复刻旧环境选择器，业务／启动／运行未验 |
| 3312 | 0x00541BB8 | `sub_541BB8` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc；CMakeLists.txt:161 — 宿主Windows CRT进入WinMain` | 原仅旧start调用的HeapCreate／池初始化／失败HeapDestroy；当前MSVC宿主启动及实际标准分配族承接，不造旧heap或失败28分支，启动／SDK／运行未验 |
| 3313 | 0x00541C15 | `___sbh_heap_init` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3314 | 0x00541C5D | `___sbh_find_block` | 有承接 | `src/platform/platformTool.cpp:34 — std::free` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3315 | 0x00541C88 | `sub_541C88` | 有承接 | `src/platform/platformTool.cpp:34 — std::free；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原只在旧free／realloc内消费的私有回收／元数据辅助；实际std::free／std::realloc整体承接，不宣原虚存策略等价，完整owner／失败／运行未验 |
| 3316 | 0x00541FB1 | `___sbh_alloc_block` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3317 | 0x005422BA | `___sbh_alloc_new_region` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3318 | 0x0054236B | `___sbh_alloc_new_group` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3319 | 0x00542466 | `___sbh_resize_block` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3320 | 0x0054275C | `sub_54275C` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原启动及542A54消费的私有区域／页初始化与失败回收；实际标准分配族／宿主启动整体承接，不复刻旧页／块布局，全部owner／运行未验 |
| 3321 | 0x005428A0 | `sub_5428A0` | 有承接 | `src/platform/platformTool.cpp:34 — std::free；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原仅5428F6转入的私有虚存／HeapFree释放链；实际标准释放／扩容库整体承接，不宣当前库保留旧划分，owner／正常寿命／运行未验 |
| 3322 | 0x005428F6 | `sub_5428F6` | 有承接 | `src/platform/platformTool.cpp:34 — std::free；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原仅542A0F转入并调用5428A0的私有decommit／释放辅助；实际std::free／std::realloc整体承接，不模拟旧元数据，owner／失败／运行未验 |
| 3323 | 0x005429B8 | `sub_5429B8` | 有承接 | `src/platform/platformTool.cpp:34 — std::free；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原仅旧free／realloc／msize消费的块定位辅助；相应实际MSVC公共服务整体承接，不推定旧物理容量／对齐或原体等价，owner／运行未验 |
| 3324 | 0x00542A0F | `sub_542A0F` | 有承接 | `src/platform/platformTool.cpp:34 — std::free；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原仅旧free／realloc消费并转5428F6的回收辅助；实际标准服务整体承接，不新建池状态或包装，完整owner／失败／运行未验 |
| 3325 | 0x00542A54 | `sub_542A54` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原旧calloc／分配／realloc消费并转542C5C／54275C的私有分配链；实际标准分配族整体承接，库内策略沿MC01-C4，完整owner／运行未验 |
| 3326 | 0x00542C5C | `sub_542C5C` | 有承接 | `src/platform/platformTool.cpp:29 — std::malloc；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原仅542A54内两处调用的私有分配辅助；实际标准分配族整体承接，不伪称同名新叶或旧算法等价，owner／SDK／运行未验 |
| 3327 | 0x00542D80 | `sub_542D80` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原仅旧realloc消费的私有扩容辅助；实际std::realloc整体承接，不复刻旧元数据／同名接口，完整块内容／失败／owner／运行未验 |
| 3328 | 0x00542E34 | `__except_handler3` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3329 | 0x00542EF1 | `__seh_longjmp_unwind@4` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3330 | 0x00542F10 | `__allmul` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3331 | 0x00542F44 | `sub_542F44` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入；src/ui/WindowRuntime.cpp:2748 — ::_close（MSVC描述符服务）` | 原九个旧文件／目录父的系统错误→DOS错误／errno转换，当前实际MSVC文件／目录服务及同线程错误访问整体承接，不手写旧表／影子槽，业务错误消费／owner／运行未验 |
| 3332 | 0x00543097 | `zerotoxdone` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3333 | 0x005430E2 | `_expbigret` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3334 | 0x0054310E | `__ffexpm1` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3335 | 0x00543151 | `_isintTOS` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3336 | 0x00543176 | `_isintTOSret` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3337 | 0x00543177 | `notanint` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3338 | 0x0054317E | `evenint` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3339 | 0x00543185 | `_usepowhlp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3340 | 0x00543390 | `__startTwoArgErrorHandling` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3341 | 0x005433A7 | `__startOneArgErrorHandling` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3342 | 0x005433F0 | `__twoToTOS` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3343 | 0x00543405 | `__load_CW` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3344 | 0x00543435 | `__fload_withFB` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3345 | 0x00543489 | `special_exp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3346 | 0x005434C5 | `__check_overflow_exit` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3347 | 0x005434D9 | `__check_range_exit` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3348 | 0x0054357C | `__powhlp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3349 | 0x005436AC | `__d_inttype` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3350 | 0x00543711 | `__alloc_osfhnd` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3351 | 0x005437A6 | `__set_osfhnd` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3352 | 0x0054381D | `__free_osfhnd` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3353 | 0x00543897 | `__get_osfhandle` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3354 | 0x005438D4 | `__chsize` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3355 | 0x00543A1A | `_toupper` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3356 | 0x00543AE8 | `___crtLCMapStringA` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3357 | 0x00543D0C | `_strncnt` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3358 | 0x00543D37 | `__mbctoupper` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3359 | 0x00543DB2 | `__XcptFilter` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3360 | 0x00543EF3 | `_xcptlookup` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3361 | 0x00543F36 | `__wincmdln` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3362 | 0x00543F8E | `__setenvp` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3363 | 0x00544047 | `__setargv` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3364 | 0x005440E0 | `_parse_cmdline` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3365 | 0x00544294 | `___crtGetEnvironmentStringsA` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3366 | 0x005443C6 | `__FF_MSGBANNER` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3367 | 0x005443FF | `sub_5443FF` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；CMakeLists.txt:161 — 宿主CRT启动／终止诊断` | 原五处引用来自旧致命错误／消息banner／abort，按错误码输出CRT弹窗或标准错误；当前MSVC库内诊断整体承接，旧R600x／钩子／缓存沿MC01-C3退休 |
| 3368 | 0x00544552 | `_mbtowc` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3369 | 0x00544620 | `__allshl` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3370 | 0x0054463F | `_ungetc` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：std::ungetc（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3371 | 0x005446AD | `__fcloseall` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：::_fcloseall（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3372 | 0x00544705 | `__isatty` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3373 | 0x0054472B | `_wctomb` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3374 | 0x005447A0 | `__aullrem` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3375 | 0x00544815 | `__getbuf` | 有承接 | `src/world/Blacklist.cpp:53 — std::fopen（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3376 | 0x00544859 | `__control87` | 有承接 | `src/world/WorldActorDrawing.cpp:36822 — RenderChara_521F20；src/world/SceneState.cpp:9702 — Sub_51D790` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3377 | 0x0054488E | `__controlfp` | 有承接 | `src/world/WorldActorDrawing.cpp:36822 — RenderChara_521F20；src/world/SceneState.cpp:9702 — Sub_51D790` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3378 | 0x005448A4 | `__abstract_cw` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3379 | 0x00544936 | `__hw_cw` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3380 | 0x005449BF | `__ZeroTail` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3381 | 0x00544A08 | `__IncMan` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3382 | 0x00544A5E | `__RoundMan` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3383 | 0x00544AEA | `__CopyMan` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3384 | 0x00544B05 | `__FillZeroMan` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3385 | 0x00544B11 | `__IsZeroMan` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3386 | 0x00544B2C | `__ShrMan` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3387 | 0x00544BB9 | `__ld12cvt` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原544D25／544D3B两输入转换叶的参数表转换支撑，实际std::vfscanf／std::vsscanf整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3388 | 0x00544D25 | `sub_544D25` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原12B中间值按5921C0的六W32表转换为双W32目标，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3389 | 0x00544D3B | `sub_544D3B` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原12B中间值按5921D8的六W32表转换为单W32目标，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3390 | 0x00544D51 | `sub_544D51` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原strgtold12→544D25链，仅53FE95双W32赋值分支使用，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3391 | 0x00544D7E | `sub_544D7E` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原strgtold12→544D3B链，仅53FE95单W32赋值分支使用，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3392 | 0x00544DB0 | `_strcpy` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：std::strcpy` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3393 | 0x00544DC0 | `_strcat` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：std::strcat` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3394 | 0x00544EA0 | `__fptostr` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3395 | 0x00544F17 | `__fltout` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3396 | 0x00544F7B | `___dtold` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3397 | 0x00545031 | `__fptrap` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3398 | 0x0054503A | `?__CxxUnhandledExceptionFilter@@YGJPAU_EXCEPTION_POINTERS@@@Z` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3399 | 0x00545080 | `sub_545080` | 有承接 | `CMakeLists.txt:161 — 宿主MSVC exe启动；src/platform/ClientEntry.cpp:263 — WinMain业务边界` | 原55317C C初始化槽安装54503A并保存旧filter；当前MSVC exe pre_cpp初始化持有默认C++异常filter，不再次安装旧入口；完整业务／运行未验 |
| 3400 | 0x00545091 | `sub_545091` | 有承接 | `CMakeLists.txt:161 — 宿主MSVC生命周期；src/platform/ClientEntry.cpp:263 — WinMain业务边界` | 原553198终止槽恢复保存filter；当前宿主拥有完整filter寿命，旧手工保存／恢复沿MC01-C3退休；既有WinMain报告临时层不冒充本叶 |
| 3401 | 0x0054509E | `?_ValidateRead@@YAHPBXI@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3402 | 0x005450BA | `?_ValidateWrite@@YAHPAXI@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3403 | 0x005450D6 | `?_ValidateExecute@@YAHP6GHXZ@Z` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3404 | 0x005450F0 | `_strcmp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：std::strcmp` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3405 | 0x00545174 | `_abort` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3406 | 0x0054518B | `_getenv` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：std::getenv` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3407 | 0x00545208 | `___crtGetStringTypeA` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3408 | 0x00545360 | `unknown_libname_51` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3409 | 0x00545477 | `unknown_libname_52` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3410 | 0x00545916 | `unknown_libname_53` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3411 | 0x00545929 | `unknown_libname_54` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3412 | 0x00545A65 | `sub_545A65` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3413 | 0x00545B3C | `unknown_libname_63` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3414 | 0x00545B51 | `unknown_libname_64` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3415 | 0x00545B66 | `unknown_libname_65` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3416 | 0x00545D6C | `unknown_libname_66` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3417 | 0x00545E1E | `unknown_libname_67` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3418 | 0x00546024 | `unknown_libname_68` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3419 | 0x005460EB | `__87except` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3420 | 0x005461BB | `__set_exp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3421 | 0x005461E4 | `__sptype` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3422 | 0x0054623E | `__decomp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3423 | 0x005462FF | `__frnd` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3424 | 0x00546311 | `__fpclass` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3425 | 0x005463A3 | `__setmode` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3426 | 0x00546419 | `___strgtold12` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3427 | 0x005468EA | `__setmbcp` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3428 | 0x00546A83 | `_getSystemCP` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3429 | 0x00546ACD | `_CPtoLCID` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3430 | 0x00546B00 | `_setSBCS` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3431 | 0x00546B29 | `_setSBUpLow` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3432 | 0x00546CAE | `___initmbctable` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3433 | 0x00546CCA | `__ismbblead` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3434 | 0x00546CDB | `_x_ismbbtype` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3435 | 0x00546D0C | `___crtMessageBoxA` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3436 | 0x00546D95 | `___addl` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3437 | 0x00546DB6 | `___add_12` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3438 | 0x00546E14 | `___shl_12` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3439 | 0x00546E42 | `___shr_12` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3440 | 0x00546E6F | `___mtold12` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3441 | 0x00546F36 | `_$I10_OUTPUT` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3442 | 0x005471C9 | `_raise` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3443 | 0x005472F6 | `_siglookup` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3444 | 0x0054733B | `__mbsnbicoll` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3445 | 0x0054737A | `___wtomb_environ` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3446 | 0x005473E8 | `__raise_exc` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3447 | 0x0054769B | `__handle_exc` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3448 | 0x005478B2 | `__set_errno` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入；doc/cg2d_code_organization_review_spec.md — COR14：LegacyErrno_1085FE0` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3449 | 0x005478D8 | `sub_5478D8` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor的std::pow；CMakeLists.txt:161 — 宿主默认数学错误服务` | 原唯一5461A4在87except传错误记录，零返回后更新errno；当前实际标准数学服务及宿主默认_matherr承接，不保旧记录／硬件状态或新增零返回包装 |
| 3450 | 0x005478DB | `__statfp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3451 | 0x005478E9 | `__clrfp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3452 | 0x005478F8 | `__ctrlfp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3453 | 0x0054791B | `__set_statfp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3454 | 0x00547971 | `___ld12mul` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3455 | 0x00547B91 | `___multtenpow12` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除；doc/cg2d_code_organization_review_spec.md — COR08：MSVC CRT格式／扫描服务，旧转引头已删除` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3456 | 0x00547C0D | `___crtCompareStringA` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3457 | 0x00547E8A | `___crtsetenv` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3458 | 0x00548011 | `_findenv` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3459 | 0x00548069 | `_copy_environ` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3460 | 0x005480D0 | `__mbschr` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3461 | 0x00548143 | `__strdup` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3462 | 0x00548170 | `sub_548170` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3463 | 0x0054817A | `sub_54817A` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3464 | 0x00548193 | `sub_548193` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3465 | 0x0054819D | `sub_54819D` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3466 | 0x005481A7 | `sub_5481A7` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3467 | 0x005481C0 | `sub_5481C0` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3468 | 0x005481CA | `sub_5481CA` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3469 | 0x005481D4 | `sub_5481D4` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3470 | 0x005481EF | `sub_5481EF` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3471 | 0x005481F9 | `sub_5481F9` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3472 | 0x0054820E | `sub_54820E` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3473 | 0x00548218 | `sub_548218` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3474 | 0x00548233 | `sub_548233` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3475 | 0x0054823D | `sub_54823D` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3476 | 0x00548252 | `sub_548252` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:88 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3477 | 0x0054825C | `sub_54825C` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:88 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3478 | 0x00548275 | `sub_548275` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:88 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3479 | 0x0054827F | `sub_54827F` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:88 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3480 | 0x00548289 | `sub_548289` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:88 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3481 | 0x005482A4 | `sub_5482A4` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:88 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3482 | 0x005482AE | `sub_5482AE` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:91 — std::clog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3483 | 0x005482B8 | `sub_5482B8` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:91 — std::clog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3484 | 0x005482D3 | `sub_5482D3` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:91 — std::clog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3485 | 0x005482DD | `??0Init@ios_base@std@@QAE@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3486 | 0x0054843B | `nullsub_5` | 空方法 | `src/platform/RuntimeInitialization.cpp:136 — EmptyLegacyExit_54843B` | 已有CPP空方法与原RET记录保留；按用户决定标为空方法，不再核对此入口 |
| 3487 | 0x00548463 | `sub_548463` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3488 | 0x005484CD | `sub_5484CD` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3489 | 0x0054852C | `sub_54852C` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3490 | 0x00548537 | `sub_548537` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3491 | 0x0054859F | `sub_54859F` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3492 | 0x005485FE | `sub_5485FE` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3493 | 0x0054865E | `??0locale@std@@QAE@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3494 | 0x0054867D | `??1locale@std@@QAE@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3495 | 0x00548695 | `?_Decref@facet@locale@std@@QAEPAV123@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3496 | 0x005486B1 | `sub_5486B1` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3497 | 0x005486BF | `sub_5486BF` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3498 | 0x005486C5 | `sub_5486C5` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3499 | 0x005486C9 | `?uflow@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3500 | 0x005486E6 | `?xsgetn@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHPADH@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3501 | 0x0054875A | `?xsputn@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHPBDH@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3502 | 0x005487D0 | `nullsub_8` | 空方法 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 已有标准流／MSVC提供者定位保留；按用户决定标为空方法，不再核对此入口 |
| 3503 | 0x005487D3 | `sub_5487D3` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3504 | 0x005487DE | `sub_5487DE` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3505 | 0x00548843 | `?overflow@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHH@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3506 | 0x00548979 | `?pbackfail@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHH@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3507 | 0x00548A26 | `?underflow@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3508 | 0x00548A53 | `?uflow@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3509 | 0x00548B68 | `unknown_libname_73` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3510 | 0x00548BEA | `unknown_libname_74` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3511 | 0x00548CAC | `?setbuf@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEPAV?$basic_streambuf@DU?$char_traits@D@std@@@2@PADH@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3512 | 0x00548CD7 | `sub_548CD7` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3513 | 0x00548CF6 | `sub_548CF6` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3514 | 0x00548D12 | `sub_548D12` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3515 | 0x00548D31 | `sub_548D31` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3516 | 0x00548D4D | `sub_548D4D` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3517 | 0x00548D69 | `?append@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ID@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3518 | 0x00548DC2 | `?erase@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@II@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3519 | 0x00548E29 | `?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEX_N@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3520 | 0x00548E65 | `?init@?$basic_ios@DU?$char_traits@D@std@@@std@@IAEXPAV?$basic_streambuf@DU?$char_traits@D@std@@@2@_N@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3521 | 0x00548EA5 | `?_Gndec@?$basic_streambuf@DU?$char_traits@D@std@@@std@@IAEPADXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3522 | 0x00548EB5 | `?_Gninc@?$basic_streambuf@DU?$char_traits@D@std@@@std@@IAEPADXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3523 | 0x00548EC5 | `unknown_libname_75` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3524 | 0x00548ED5 | `?close@?$basic_filebuf@DU?$char_traits@D@std@@@std@@QAEPAV12@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3525 | 0x00548EFC | `?_Init@?$basic_filebuf@DU?$char_traits@D@std@@@std@@IAEXPAU_iobuf@@W4_Initfl@12@@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3526 | 0x00548F74 | `?_Freeze@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3527 | 0x00548FA0 | `?_Grow@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAE_NI_N@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3528 | 0x00549025 | `?_Split@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3529 | 0x00549055 | `?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@PBDI@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3530 | 0x0054908A | `?_Copy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXI@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3531 | 0x00549147 | `?_Ungetc@std@@YA_NABDPAU_iobuf@@I@Z` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：std::ungetc（标准库提供；无CPP消费者）；src/render/Palette.cpp:229 — std::fgetc（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3532 | 0x00549194 | `unknown_libname_76` | 有承接 | `src/platform/RuntimeInitialization.cpp:156 — RegisterLegacyEmptyExitPair；src/platform/RuntimeInitialization.cpp:324 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3533 | 0x005491BE | `sub_5491BE` | 有承接 | `src/platform/RuntimeInitialization.cpp:141 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3534 | 0x005491D6 | `sub_5491D6` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3535 | 0x005491E0 | `sub_5491E0` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3536 | 0x005491F6 | `sub_5491F6` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:87 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3537 | 0x00549200 | `sub_549200` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3538 | 0x0054920A | `sub_54920A` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3539 | 0x00549220 | `sub_549220` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3540 | 0x0054922A | `sub_54922A` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3541 | 0x00549234 | `sub_549234` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3542 | 0x0054924D | `sub_54924D` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3543 | 0x00549257 | `sub_549257` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3544 | 0x00549261 | `sub_549261` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3545 | 0x0054927A | `sub_54927A` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3546 | 0x00549284 | `sub_549284` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:109 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3547 | 0x0054928E | `sub_54928E` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:109 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3548 | 0x005492A7 | `sub_5492A7` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:109 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3549 | 0x005492B1 | `sub_5492B1` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3550 | 0x005492BB | `sub_5492BB` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3551 | 0x005492D6 | `sub_5492D6` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3552 | 0x005492E0 | `sub_5492E0` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3553 | 0x005492F5 | `sub_5492F5` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3554 | 0x005492FF | `sub_5492FF` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3555 | 0x0054931A | `sub_54931A` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:108 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3556 | 0x00549324 | `sub_549324` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3557 | 0x00549339 | `sub_549339` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:109 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3558 | 0x00549343 | `sub_549343` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:109 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3559 | 0x0054935E | `sub_54935E` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:109 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3560 | 0x00549368 | `sub_549368` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:112 — std::wclog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3561 | 0x00549372 | `sub_549372` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:112 — std::wclog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3562 | 0x0054938D | `sub_54938D` | 有承接 | `src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:112 — std::wclog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3563 | 0x00549397 | `??0_Winit@std@@QAE@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3564 | 0x00549525 | `sub_549525` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3565 | 0x0054958F | `sub_54958F` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3566 | 0x005495EE | `unknown_libname_77` | 有承接 | `src/platform/RuntimeInitialization.cpp:75 — FlushLegacyStandardStream` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3567 | 0x00549638 | `sub_549638` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3568 | 0x00549643 | `sub_549643` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3569 | 0x005496AB | `sub_5496AB` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3570 | 0x0054970A | `sub_54970A` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3571 | 0x0054976A | `sub_54976A` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3572 | 0x00549778 | `sub_549778` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3573 | 0x0054977F | `sub_54977F` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3574 | 0x00549782 | `sub_549782` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3575 | 0x00549787 | `?uflow@?$basic_streambuf@_WU?$char_traits@_W@std@@@std@@MAEGXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3576 | 0x005497A8 | `?xsgetn@?$basic_streambuf@GU?$char_traits@G@std@@@std@@MAEHPAGH@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3577 | 0x0054982F | `?xsputn@?$basic_streambuf@GU?$char_traits@G@std@@@std@@MAEHPBGH@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3578 | 0x005498B7 | `unknown_libname_78` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3579 | 0x005498EE | `unknown_libname_79` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3580 | 0x00549925 | `sub_549925` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3581 | 0x0054992A | `sub_54992A` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3582 | 0x00549935 | `sub_549935` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3583 | 0x0054999A | `?overflow@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGG@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3584 | 0x00549AE2 | `?pbackfail@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGG@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3585 | 0x00549B99 | `?underflow@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3586 | 0x00549BC7 | `?uflow@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3587 | 0x00549CE5 | `?setbuf@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEPAV?$basic_streambuf@GU?$char_traits@G@std@@@2@PAGH@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3588 | 0x00549D13 | `unknown_libname_80` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3589 | 0x00549D2C | `sub_549D2C` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3590 | 0x00549D4B | `sub_549D4B` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3591 | 0x00549D67 | `sub_549D67` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3592 | 0x00549D86 | `sub_549D86` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3593 | 0x00549DA2 | `sub_549DA2` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3594 | 0x00549DBE | `?init@?$basic_ios@GU?$char_traits@G@std@@@std@@IAEXPAV?$basic_streambuf@GU?$char_traits@G@std@@@2@_N@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3595 | 0x00549E08 | `?gbump@?$basic_streambuf@GU?$char_traits@G@std@@@std@@IAEXH@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3596 | 0x00549E1C | `?_Gndec@?$basic_streambuf@GU?$char_traits@G@std@@@std@@IAEPAGXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3597 | 0x00549E2D | `?_Gninc@?$basic_streambuf@GU?$char_traits@G@std@@@std@@IAEPAGXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3598 | 0x00549E3D | `unknown_libname_81` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3599 | 0x00549E51 | `unknown_libname_82` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3600 | 0x00549E61 | `?close@?$basic_filebuf@GU?$char_traits@G@std@@@std@@QAEPAV12@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3601 | 0x00549E88 | `?_Init@?$basic_filebuf@GU?$char_traits@G@std@@@std@@IAEXPAU_iobuf@@W4_Initfl@12@@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3602 | 0x00549EDE | `unknown_libname_83` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3603 | 0x00549F20 | `unknown_libname_84` | 有承接 | `src/platform/RuntimeInitialization.cpp:156 — RegisterLegacyEmptyExitPair；src/platform/RuntimeInitialization.cpp:324 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3604 | 0x00549F4A | `sub_549F4A` | 有承接 | `src/platform/RuntimeInitialization.cpp:141 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3605 | 0x00549F62 | `?clear@ios_base@std@@QAEXH_N@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3606 | 0x00549FFE | `sub_549FFE` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3607 | 0x0054A062 | `sub_54A062` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3608 | 0x0054A06F | `sub_54A06F` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3609 | 0x0054A08C | `unknown_libname_85` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3610 | 0x0054A0C9 | `sub_54A0C9` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3611 | 0x0054A0E5 | `sub_54A0E5` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3612 | 0x0054A142 | `unknown_libname_86` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3613 | 0x0054A17F | `sub_54A17F` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3614 | 0x0054A19C | `sub_54A19C` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3615 | 0x0054A1B8 | `sub_54A1B8` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3616 | 0x0054A1D0 | `??1ios_base@std@@UAE@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3617 | 0x0054A220 | `??_Gios_base@std@@UAEPAXI@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3618 | 0x0054A23C | `?_Callfns@ios_base@std@@AAEXW4event@12@@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3619 | 0x0054A25E | `?_Addstd@ios_base@std@@IAEXXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3620 | 0x0054A293 | `?_Init@ios_base@std@@IAEXXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3621 | 0x0054A2CB | `?_Tidy@ios_base@std@@AAEXXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3622 | 0x0054A315 | `?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ABV12@II@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3623 | 0x0054A3E3 | `sub_54A3E3` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3624 | 0x0054A3ED | `nullsub_9` | 空方法 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 已有标准流／MSVC提供者定位保留；按用户决定标为空方法，不再核对此入口 |
| 3625 | 0x0054A3FA | `sub_54A3FA` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3626 | 0x0054A404 | `?_Init@locale@std@@CAPAV_Locimp@12@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3627 | 0x0054A4BD | `sub_54A4BD` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3628 | 0x0054A505 | `??0_Locimp@locale@std@@AAE@_N@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3629 | 0x0054A57E | `unknown_libname_87` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3630 | 0x0054A59B | `??_G_Locimp@locale@std@@UAEPAXI@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3631 | 0x0054A5B7 | `??1_Locimp@locale@std@@UAE@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3632 | 0x0054A638 | `unknown_libname_88` | 有承接 | `src/platform/RuntimeInitialization.cpp:156 — RegisterLegacyEmptyExitPair；src/platform/RuntimeInitialization.cpp:324 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3633 | 0x0054A662 | `sub_54A662` | 有承接 | `src/platform/RuntimeInitialization.cpp:141 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3634 | 0x0054A67A | `?_Xlen@std@@YAXXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3635 | 0x0054A6D4 | `sub_54A6D4` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3636 | 0x0054A738 | `sub_54A738` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3637 | 0x0054A755 | `unknown_libname_89` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3638 | 0x0054A792 | `sub_54A792` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3639 | 0x0054A7AE | `sub_54A7AE` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3640 | 0x0054A80B | `unknown_libname_90` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3641 | 0x0054A848 | `sub_54A848` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3642 | 0x0054A865 | `sub_54A865` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3643 | 0x0054A881 | `sub_54A881` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3644 | 0x0054A899 | `?_Xran@std@@YAXXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3645 | 0x0054A8F3 | `unknown_libname_91` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3646 | 0x0054A930 | `sub_54A930` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3647 | 0x0054A94D | `sub_54A94D` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3648 | 0x0054A969 | `sub_54A969` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3649 | 0x0054A981 | `unknown_libname_92` | 有承接 | `src/platform/RuntimeInitialization.cpp:156 — RegisterLegacyEmptyExitPair；src/platform/RuntimeInitialization.cpp:324 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3650 | 0x0054A9AB | `sub_54A9AB` | 有承接 | `src/platform/RuntimeInitialization.cpp:141 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3651 | 0x0054A9C3 | `__Mbrtowc` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3652 | 0x0054AAD5 | `_btowc` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3653 | 0x0054AB16 | `GetAdaptersAddresses` | 有承接 | `src/platform/platformTool.cpp:20 — GetAdaptersAddresses；src/platform/platformTool.cpp:15 — GetMacAddress_534EF0` | 原 GetAdaptersAddresses import 跳板；当前实际 Windows IP Helper API 调用存在，保留原适配器消费边界；完整网络／运行未验 |
| 3654 | 0x0054AB1C | `RtlUnwind` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3655 | 0x0054AB30 | `__strnicmp` | 有承接 | `src/platform/MainLoopSupport.cpp:205 — ::_strnicmp` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3656 | 0x0054ABDD | `__itoa` | 有承接 | `src/world/RenderInput.cpp:132 — std::to_wstring` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3657 | 0x0054AC0A | `_xtoa` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR14：MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3658 | 0x0054AC66 | `??1type_info@@UAE@XZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3659 | 0x0054AC7B | `??_Gtype_info@@UAEPAXI@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3660 | 0x0054AC98 | `__EH_prolog` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3661 | 0x0054ACB7 | `_fgetpos` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：std::fgetpos（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3662 | 0x0054ACD9 | `_fsetpos` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：std::fsetpos（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3663 | 0x0054ACF1 | `_setvbuf` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：std::setvbuf（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3664 | 0x0054AD92 | `_fputwc` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：std::fputwc（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3665 | 0x0054AE87 | `_ungetwc` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：std::ungetwc（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3666 | 0x0054AF70 | `_fgetwc` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：std::fgetwc（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3667 | 0x0054B05D | `sub_54B05D` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3668 | 0x0054B079 | `??0exception@@QAE@ABQBD@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3669 | 0x0054B0B6 | `??0exception@@QAE@ABV0@@Z` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3670 | 0x0054B100 | `sub_54B100` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3671 | 0x0054B116 | `?what@exception@@UBEPBDXZ` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3672 | 0x0054B123 | `__CxxThrowException@8` | 有承接 | `src/platform/RuntimeInitialization.cpp:85 — NarrowStandardStreamsStaticInitializer；src/platform/RuntimeInitialization.cpp:106 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3673 | 0x0054B160 | `_memcmp` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原包装已按COR08退休，服务由MSVC／标准库提供；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3674 | 0x0054B1AC | `retnull_0` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供；doc/cg2d_code_organization_review_spec.md — COR08：无消费者内存转发已退休，由MSVC CRT提供` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3675 | 0x0054B20C | `___termcon` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3676 | 0x0054B239 | `__ftelli64` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：::_ftelli64（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3677 | 0x0054B3C0 | `__fseeki64` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：::_fseeki64（标准库提供；无CPP消费者）` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3678 | 0x0054B450 | `__flswbuf` | 有承接 | `doc/cg2d_code_organization_review_spec.md — COR12：std::fputwc（标准库提供；无CPP消费者）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3679 | 0x0054B574 | `__filwbuf` | 有承接 | `src/render/Palette.cpp:229 — std::fgetc（直接CRT调用）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3680 | 0x0054B658 | `__lseeki64` | 有承接 | `src/ui/WindowRuntime.cpp:2742 — ::_open（MSVC描述符服务）` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
