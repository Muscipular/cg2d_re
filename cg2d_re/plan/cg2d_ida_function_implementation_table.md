# IDA 函数全量实现表

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
| 有正文 | 2864 |
| 有承接 | 688 |
| 空方法 | 9 |
| 部分实现 | 5 |
| 未实现 | 0 |
| 待确认 | 0 |
| 已排除 | 113 |
| 不适用 | 1 |

2026-10-12 进度核对：3680行入口与当前IDA地址集合、名称此前经15页逐项核对零差异；3964处源码引用及9处CMake引用可读。当时2911个正文提供者可定位到定义；本次九个nullsub仅归类为空方法，其中3个已有CPP定义和4个已有承接定位保留，其余2908个有正文及644个有承接不变。序号、身份、名称、提供者均不变，未重新查询IDA；UTF-8无BOM/LF。这不提升全部逻辑、完整接线或运行验收，未实现0不能推为功能缺口归零。

所有3680行均已标记。九个空方法按用户决定结束核对，不计为业务正文、承接或范围排除；当前待确认0。未实现0只表示尚未确认独立缺正文入口，不表示不存在开发缺口；五项部分实现仍保留当前源码的数据定义／生产缺项。

提供者采用本线相对路径、实际符号／操作与定位行号。库内叶的“有承接”仅指当前真实 MSVC／STL／SDL／SDK 服务或已证内联操作，不声称复制旧私有算法或机器 ABI。现有正文只确认存在与非占位，未例行重做完整差异核对；所有完整业务、owner、接线及运行门禁仍按各自合同验收。未见调用仅为当前 CPP/HPP 定位事实，不声称原入口不可达。

| 序号 | IDA 地址 | IDA 函数名 | 是否有实现 | 当前提供者 | 接线／缺项 |
|---:|---|---|---|---|---|
| 1 | 0x00401000 | `sub_401000` | 有正文 | `src/Blacklist.cpp:146 — PersistAndReset_401000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2 | 0x00401030 | `sub_401030` | 有正文 | `src/Blacklist.cpp:86 — LoadBlacklistFile` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 3 | 0x00401170 | `sub_401170` | 有正文 | `src/Blacklist.cpp:79 — BlacklistRecordAt_401170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 4 | 0x00401190 | `sub_401190` | 有正文 | `src/Blacklist.cpp:158 — GetBlacklistCount_401190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 5 | 0x004011A0 | `sub_4011A0` | 有正文 | `src/Blacklist.cpp:44 — PersistBlacklistFile_4011A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 6 | 0x00401270 | `sub_401270` | 有正文 | `src/Blacklist.cpp:188 — AddBlacklistRecord_401270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 7 | 0x004012F0 | `sub_4012F0` | 有正文 | `src/Blacklist.cpp:234 — RemoveBlacklistRecord_4012F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 8 | 0x004013D0 | `sub_4013D0` | 有正文 | `src/Blacklist.cpp:164 — ContainsBlacklistRecord_4013D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 9 | 0x00401470 | `sub_401470` | 有正文 | `src/Blacklist.cpp:288 — ClearBlacklistRecords_401470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 10 | 0x004014B0 | `sub_4014B0` | 有正文 | `src/LegacyUiRecord.cpp:15429 — Sub_4014B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 11 | 0x00401960 | `sub_401960` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 12 | 0x00401AB0 | `sub_401AB0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 13 | 0x00401C10 | `sub_401C10` | 有承接 | `src/platform/LegacyMouse.cpp:39 — gStartupValue5939C8 / gStartupValue5939D8` | LCI07在原中央位置直接定义两值，默认构造各一次RNG并自动逆序析构；原地址保持；仅静态核对，编译／运行未验 |
| 14 | 0x00401C20 | `unknown_libname_1` | 有承接 | `src/platform/LegacyMouse.cpp:39 — gStartupValue5939C8 / gStartupValue5939D8` | 原401C20两值正序构造由全局定义处默认构造承接；原中央位置与两次RNG序保持；SDK退出／完整运行未验 |
| 15 | 0x00401C50 | `unknown_libname_2` | 有承接 | `src/platform/LegacyMouse.cpp:40 — gStartupValue5939D8 / gStartupValue5939C8逆序析构` | 原401C50逆序退出由同一中央两值的原生全局析构承接；MSVC自动登记；完整运行未验 |
| 16 | 0x00401C70 | `sub_401C70` | 有正文 | `src/platform/LegacyCryptInt.cpp:25 — LegacyCryptInt::LegacyCryptInt()` | 零值默认构造委托整数0构造，仍一次共享RNG；原16B／偏移保持；布局／运行未验 |
| 17 | 0x00401CA0 | `sub_401CA0` | 有承接 | `src/platform/LegacyCryptInt.cpp:41 — LegacyCryptInt::~LegacyCryptInt；释放由默认MSVC delete承接` | LCI08无动态消费者，类专用分配重载退休；原401CA0销毁注释归入析构，未来动态释放沿默认MSVC；原flags／返回ABI保历史，运行未验 |
| 18 | 0x00401CC0 | `CryptInt_Init` | 有正文 | `src/platform/LegacyCryptInt.cpp:31 — LegacyCryptInt::LegacyCryptInt(int32_t)` | 整数构造接原发布顺序和一次RNG；不显式写尾部padding；完整运行未验 |
| 19 | 0x00401CF0 | `CryptInt_delete` | 有正文 | `src/platform/LegacyCryptInt.cpp:41 — LegacyCryptInt::~LegacyCryptInt` | 原生析构只恢复token；栈／成员／动态存储实际寿命已迁移；运行未验 |
| 20 | 0x00401D00 | `CryptInt::eq` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建==` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 21 | 0x00401D10 | `sub_401D10` | 有承接 | `src/YobiRuntime.cpp:33826 — action->mapY1F0.value() != floor` | != 比较内联；共享 value，不重建包装；完整业务／运行未验 |
| 22 | 0x00401D20 | `CryptInt::neq` | 有正文 | `src/platform/LegacyCryptInt.cpp:194 — cg::platform::operator!=` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 23 | 0x00401D40 | `sub_401D40` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建>` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 24 | 0x00401D50 | `sub_401D50` | 有正文 | `src/platform/LegacyCryptInt.cpp:201 — cg::platform::operator>` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 25 | 0x00401D70 | `CryptInt::gte` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建>=` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 26 | 0x00401D80 | `sub_401D80` | 有正文 | `src/platform/LegacyCryptInt.cpp:208 — cg::platform::operator>=` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 27 | 0x00401DA0 | `CryptInt::Lte` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建<=` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 28 | 0x00401DB0 | `CryptInt::lt` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建<` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 29 | 0x00401DC0 | `sub_401DC0` | 有正文 | `src/platform/LegacyCryptInt.cpp:215 — cg::platform::operator<` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 30 | 0x00401DE0 | `sub_401DE0` | 有正文 | `src/platform/LegacyCryptInt.cpp:155 — LegacyCryptInt::operator++` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 31 | 0x00401E00 | `sub_401E00` | 有承接 | `src/YobiRuntime.cpp:3354 — Sub_446B50` | S16 加值在实际坐标计算中内联；共享 value；完整业务／运行未验 |
| 32 | 0x00401E10 | `CryptInt::add` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建+` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 33 | 0x00401E20 | `sub_401E20` | 有正文 | `src/platform/LegacyCryptInt.cpp:125 — LegacyCryptInt::operator+=` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 34 | 0x00401E40 | `sub_401E40` | 有正文 | `src/platform/LegacyCryptInt.cpp:140 — LegacyCryptInt::operator-=` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 35 | 0x00401E60 | `CryptInt::dec` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建-` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 36 | 0x00401E70 | `CryptInt::dec_0` | 有正文 | `src/platform/LegacyCryptInt.cpp:176 — cg::platform::operator-` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 37 | 0x00401E90 | `CryptInt::mul` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建*` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 38 | 0x00401EA0 | `CryptInt::GetVal` | 有承接 | `src/platform/LegacyCryptInt.cpp:76 — LegacyCryptInt::value` | 原入口为跳转到 401EC0；共用同一读取正文；完整业务／运行未验 |
| 39 | 0x00401EB0 | `sub_401EB0` | 有承接 | `src/YobiRuntime.cpp:5278 — Sub_4424B0` | 原读取后 fild 转换；当前实际读取后转 float／double；完整业务／运行未验 |
| 40 | 0x00401EC0 | `sub_401EC0` | 有正文 | `src/platform/LegacyCryptInt.cpp:76 — LegacyCryptInt::value` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 41 | 0x00401F50 | `CryptInt::SetNorVal` | 有正文 | `src/platform/LegacyCryptInt.cpp:60 — LegacyCryptInt::SetNorVal_401F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 42 | 0x00401F60 | `sub_401F60` | 有承接 | `src/YobiRuntime.cpp:33826 — action->mapY1F0.value() != floor` | 标量与读取结果 != 在消费者中内联；完整业务／运行未验 |
| 43 | 0x00401F80 | `CryptInt::gt` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建>` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 44 | 0x00401FA0 | `sub_401FA0` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32左整数内建<` | LCI-FA0八处已知调用只消费AL，改为单次读取与布尔比较；高24位保历史，完整机器ABI／运行未验 |
| 45 | 0x00401FC0 | `CrypyInt::lte` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建<=` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 46 | 0x00401FE0 | `CryptInt::Dec2` | 有承接 | `src/platform/LegacyCryptInt.cpp:114 — LegacyCryptInt::operator std::int32_t + S32内建-` | LCI03／LCI04经单次转换与内建operator承接原S32运算；完整业务／接线／运行未验 |
| 47 | 0x00402000 | `sub_402000` | 有承接 | `src/YobiRuntime.cpp:36275 — Equal_401D00(&action->mapX200, x)` | 原标量 == 读取值；当前共享 Equal_401D00；完整业务／运行未验 |
| 48 | 0x00402020 | `sub_402020` | 有正文 | `src/LegacyUiRecord.cpp:1938 — ReloadLegacyLog_402020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 49 | 0x004021A0 | `sub_4021A0` | 有正文 | `src/LegacyUiRecord.cpp:1990 — Sub_4021A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 50 | 0x00402B30 | `sub_402B30` | 有正文 | `src/LegacyUiRecord.cpp:15358 — Sub_402B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 51 | 0x00402B70 | `sub_402B70` | 有正文 | `src/platform/LegacyModuleDigest.cpp:69 — Initialize` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 52 | 0x00402BA0 | `sub_402BA0` | 有承接 | `src/platform/LegacyModuleDigest.cpp:191 — StoreLittleEndian32(context.state[index], digest.data() + index * 4)` | 原四 W32 的小端输出在当前 Finalize 的四次循环中承接，共用实际 StoreLittleEndian32；不以单次四字节 helper 冒充原整根正文；完整摘要链／运行未验 |
| 53 | 0x00402BC0 | `sub_402BC0` | 有正文 | `src/platform/LegacyModuleDigest.cpp:173 — Finalize` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 54 | 0x00402C50 | `sub_402C50` | 有正文 | `src/platform/LegacyModuleDigest.cpp:129 — Update` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 55 | 0x00402D30 | `sub_402D30` | 有正文 | `src/platform/LegacyModuleDigest.cpp:84 — Transform` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 56 | 0x00403660 | `sub_403660` | 有正文 | `src/platform/LegacyModuleDigest.cpp:196 — ComputeFileDigest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 57 | 0x00403750 | `sub_403750` | 有正文 | `src/platform/LegacyModuleDigest.cpp:237 — EncodeLowerHex` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 58 | 0x004037D0 | `sub_4037D0` | 有正文 | `src/network/NetworkRuntime.cpp:1137 — ResetProtocolState_4037D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 59 | 0x004038A0 | `sub_4038A0` | 有正文 | `src/network/NetworkRuntime.cpp:426 — Sub_4038A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 60 | 0x00403A70 | `sub_403A70` | 有正文 | `src/network/NetworkRuntime.cpp:509 — Sub_403A70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 61 | 0x00403C50 | `sub_403C50` | 有正文 | `src/network/NetworkRuntime.cpp:221 — Sub_403C50` | 保留旧W16帧正文，当前CPP无调用；活动LF接收由ReceivePlaintextProtocolLine另接，旧帧状态不作为当前接线；当前窗口报告采样／清零另在QueuePlaintextProtocolBytes承接 |
| 62 | 0x00403E50 | `sub_403E50` | 有正文 | `src/network/NetworkRuntime.cpp:306 — Sub_403E50` | 保留旧W16帧正文，当前CPP无调用；活动明文收发由ReceivePlaintextProtocolLine／QueuePlaintextProtocolBytes另接，窗口报告状态采样／清零已接回活动发送路径；不等于原帧整根或网络验收通过 |
| 63 | 0x00404060 | `sub_404060` | 有正文 | `src/Input.cpp:551 — IsRejectedInputText_404060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 64 | 0x00404120 | `sub_404120` | 有承接 | `src/platform/MainLoopSupport.cpp:271 — ProtectionChecksum1Startup::ProtectionChecksum1Startup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 65 | 0x00404130 | `unknown_libname_3` | 有正文 | `src/platform/MainLoopSupport.cpp:283 — ConstructProtectionChecksum1_404130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 66 | 0x00404150 | `unknown_libname_4` | 有正文 | `src/platform/MainLoopSupport.cpp:289 — DestroyProtectionChecksum1_404150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 67 | 0x00404160 | `sub_404160` | 有承接 | `src/platform/MainLoopSupport.cpp:295 — ProtectionChecksum2Startup::ProtectionChecksum2Startup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 68 | 0x00404170 | `unknown_libname_5` | 有正文 | `src/platform/MainLoopSupport.cpp:307 — ConstructProtectionChecksum2_404170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 69 | 0x00404190 | `unknown_libname_6` | 有正文 | `src/platform/MainLoopSupport.cpp:313 — DestroyProtectionChecksum2_404190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 70 | 0x004041A0 | `sub_4041A0` | 有承接 | `src/platform/MainLoopSupport.cpp:319 — ProtectionProcessStateStartup::ProtectionProcessStateStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 71 | 0x004041B0 | `unknown_libname_7` | 有正文 | `src/platform/MainLoopSupport.cpp:331 — ConstructProtectionProcessState_4041B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 72 | 0x004041D0 | `unknown_libname_8` | 有正文 | `src/platform/MainLoopSupport.cpp:337 — DestroyProtectionProcessState_4041D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 73 | 0x004041E0 | `sub_4041E0` | 有正文 | `src/platform/MainLoopSupport.cpp:343 — InitializeProtection_4041E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 74 | 0x004042B0 | `sub_4042B0` | 有正文 | `src/platform/MainLoopSupport.cpp:578 — ReleaseProtection_4042B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 75 | 0x004042D0 | `sub_4042D0` | 有正文 | `src/platform/MainLoopSupport.cpp:159 — CheckProcessModules_4042D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 76 | 0x004043B0 | `sub_4043B0` | 有正文 | `src/platform/MainLoopSupport.cpp:376 — CheckProtection_4043B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 77 | 0x00404600 | `sub_404600` | 有正文 | `src/platform/MainLoopSupport.cpp:493 — ParseProtectionTables_404600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 78 | 0x00404880 | `sub_404880` | 有正文 | `src/platform/MainLoopSupport.cpp:202 — CheckDebugger_404880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 79 | 0x004048D0 | `sub_4048D0` | 有正文 | `src/platform/MainLoopSupport.cpp:606 — ResetProtection_4048D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 80 | 0x004048E0 | `sub_4048E0` | 有正文 | `src/platform/MainLoopSupport.cpp:636 — CheckWindows_4048E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 81 | 0x00404940 | `EnumFunc` | 有正文 | `src/platform/MainLoopSupport.cpp:223 — CheckWindowsCallback_404940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 82 | 0x00404A40 | `sub_404A40` | 有正文 | `src/YobiRuntime.cpp:1635 — Sub_404A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 83 | 0x00404AE0 | `YobiAction::new` | 有正文 | `src/YobiRuntime.cpp:1666 — NewYobiAction_404AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 84 | 0x00404C50 | `Yobi::UpdateAll` | 有正文 | `src/YobiRuntime.cpp:6585 — YobiUpdateAll_404C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 85 | 0x00404D00 | `Yobi::MarkDeath` | 有正文 | `src/YobiRuntime.cpp:6624 — MarkDeath` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 86 | 0x00404D10 | `sub_404D10` | 有正文 | `src/YobiRuntime.cpp:1457 — DeleteYobiAction_404D10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 87 | 0x00404D30 | `YobiClearUp` | 有正文 | `src/YobiRuntime.cpp:33690 — YobiClearUp_404D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 88 | 0x00404D60 | `sub_404D60` | 有正文 | `src/YobiRuntime.cpp:33701 — Sub_404D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 89 | 0x00404D90 | `SelectIntValue` | 有正文 | `src/YobiRuntime.cpp:11878 — SelectIntValue_404D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 90 | 0x00404DB0 | `BattleEntry::EffectUpdate` | 有正文 | `src/YobiRuntime.cpp:23130 — UpdateBattleActor_404DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 91 | 0x00405B40 | `GetPetPosition` | 有正文 | `src/world/SceneState.cpp:8555 — GetBattlePetPosition_405B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 92 | 0x00405B60 | `sub_405B60` | 有正文 | `src/YobiRuntime.cpp:26507 — ConsumeBattleFpCost_405B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 93 | 0x00405BD0 | `CompareFunction` | 有正文 | `src/YobiRuntime.cpp:22823 — CompareBattleDrawActions_405BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 94 | 0x00405C30 | `Yobi::sort` | 有正文 | `src/YobiRuntime.cpp:22851 — SortBattleActions_405C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 95 | 0x00405CB0 | `sub_405CB0` | 有正文 | `src/YobiRuntime.cpp:22883 — Sub_405CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 96 | 0x00405E60 | `sub_405E60` | 有正文 | `src/YobiRuntime.cpp:22919 — NormalizeBattleAngle_405E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 97 | 0x00405EA0 | `YobiAction::UpdateXYByRad` | 有正文 | `src/YobiRuntime.cpp:22931 — MoveBattleByAngle_405EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 98 | 0x00405ED0 | `sub_405ED0` | 有正文 | `src/YobiRuntime.cpp:22942 — GetBattleRelativeDirection_405ED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 99 | 0x00406060 | `sub_406060` | 有正文 | `src/YobiRuntime.cpp:22985 — GetBattleAngleDifference_406060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 100 | 0x004060D0 | `sub_4060D0` | 有正文 | `src/YobiRuntime.cpp:23004 — TurnBattleTowardPosition_4060D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 101 | 0x00406190 | `YobiAction::UpdateRad` | 有正文 | `src/YobiRuntime.cpp:23036 — TurnBattleToward_406190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 102 | 0x00406260 | `sub_406260` | 有正文 | `src/YobiRuntime.cpp:29265 — Sub_406260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 103 | 0x004062A0 | `sub_4062A0` | 有正文 | `src/YobiRuntime.cpp:23072 — GetBattleDistanceToPosition_4062A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 104 | 0x00406300 | `sub_406300` | 有正文 | `src/YobiRuntime.cpp:23087 — GetBattleDistance_406300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 105 | 0x00406360 | `sub_406360` | 有正文 | `src/YobiRuntime.cpp:23103 — CountBattleActiveEntries_406360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 106 | 0x00406390 | `sub_406390` | 有正文 | `src/YobiRuntime.cpp:31660 — ApplyBattleEffectFlags_406390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 107 | 0x004065A0 | `sub_4065A0` | 有正文 | `src/YobiRuntime.cpp:23115 — SeedBattleRandom_4065A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 108 | 0x004065E0 | `sub_4065E0` | 有正文 | `src/YobiRuntime.cpp:24796 — ProbeBattleAttackEvent_4065E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 109 | 0x00406630 | `sub_406630` | 有正文 | `src/YobiRuntime.cpp:29256 — UpdateBattleAction236_406630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 110 | 0x00406650 | `sub_406650` | 有正文 | `src/YobiRuntime.cpp:23852 — UpdateBattleAction0_406650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 111 | 0x00406720 | `sub_406720` | 有正文 | `src/YobiRuntime.cpp:23891 — UpdateBattleAction1_406720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 112 | 0x00406760 | `sub_406760` | 有正文 | `src/YobiRuntime.cpp:23907 — UpdateBattleAction2_406760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 113 | 0x004067F0 | `sub_4067F0` | 有正文 | `src/YobiRuntime.cpp:24102 — UpdateBattleAction11_4067F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 114 | 0x00406930 | `sub_406930` | 有正文 | `src/YobiRuntime.cpp:24159 — UpdateBattleAction12_406930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 115 | 0x00406A80 | `sub_406A80` | 有正文 | `src/YobiRuntime.cpp:24203 — UpdateBattleAction13_406A80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 116 | 0x00406BA0 | `sub_406BA0` | 有正文 | `src/YobiRuntime.cpp:25936 — UpdateBattleAction25_406BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 117 | 0x00406C30 | `sub_406C30` | 有正文 | `src/YobiRuntime.cpp:23931 — UpdateBattleAction3_406C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 118 | 0x00406CD0 | `sub_406CD0` | 有正文 | `src/YobiRuntime.cpp:23990 — UpdateBattleAction6_406CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 119 | 0x00406D70 | `sub_406D70` | 有正文 | `src/YobiRuntime.cpp:23951 — UpdateBattleAction4_406D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 120 | 0x00406E10 | `Battle::Boomerange::runEffect` | 有正文 | `src/YobiRuntime.cpp:24817 — UpdateBattleAction15_406E10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 121 | 0x004070E0 | `Battle::SpiracleShot::runEffect` | 有正文 | `src/YobiRuntime.cpp:24982 — UpdateBattleAction70_4070E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 122 | 0x004071C0 | `Battle::BlastWave::runEffect` | 有正文 | `src/YobiRuntime.cpp:25006 — UpdateBattleAction249_4071C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 123 | 0x004072E0 | `sub_4072E0` | 有正文 | `src/YobiRuntime.cpp:25043 — UpdateBattleAction16_4072E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 124 | 0x00408170 | `Battle::BowShot::runEffect` | 有正文 | `src/YobiRuntime.cpp:24901 — UpdateBattleAction18_408170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 125 | 0x004082F0 | `Battle::RandomShot::runEffect` | 有正文 | `src/YobiRuntime.cpp:24943 — UpdateBattleAction19_4082F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 126 | 0x00408410 | `sub_408410` | 有正文 | `src/YobiRuntime.cpp:25468 — UpdateBattleAction20_408410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 127 | 0x00408750 | `sub_408750` | 有正文 | `src/YobiRuntime.cpp:25570 — UpdateBattleAction21_408750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 128 | 0x004087C0 | `sub_4087C0` | 有正文 | `src/YobiRuntime.cpp:25588 — UpdateBattleAction22_4087C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 129 | 0x004090F0 | `sub_4090F0` | 有正文 | `src/YobiRuntime.cpp:25841 — UpdateBattleAction23_4090F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 130 | 0x00409380 | `sub_409380` | 有正文 | `src/YobiRuntime.cpp:25923 — UpdateBattleAction24_409380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 131 | 0x004093B0 | `sub_4093B0` | 有正文 | `src/YobiRuntime.cpp:25457 — UpdateBattleAction17_4093B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 132 | 0x00409410 | `sub_409410` | 有正文 | `src/YobiRuntime.cpp:24012 — UpdateBattleAction7_409410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 133 | 0x004094A0 | `sub_4094A0` | 有正文 | `src/YobiRuntime.cpp:24035 — UpdateBattleAction8_4094A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 134 | 0x00409510 | `sub_409510` | 有正文 | `src/YobiRuntime.cpp:24053 — UpdateBattleAction9_409510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 135 | 0x00409610 | `sub_409610` | 有正文 | `src/YobiRuntime.cpp:24086 — UpdateBattleAction10_409610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 136 | 0x00409680 | `sub_409680` | 有正文 | `src/YobiRuntime.cpp:24246 — UpdateBattleAction14_409680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 137 | 0x0040A9E0 | `sub_40A9E0` | 有正文 | `src/YobiRuntime.cpp:25968 — UpdateBattleAction26_40A9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 138 | 0x0040AA70 | `sub_40AA70` | 有正文 | `src/YobiRuntime.cpp:25988 — UpdateBattleAction27_40AA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 139 | 0x0040ACF0 | `sub_40ACF0` | 有正文 | `src/YobiRuntime.cpp:26074 — UpdateBattleAction28_40ACF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 140 | 0x0040AE50 | `sub_40AE50` | 有正文 | `src/YobiRuntime.cpp:26126 — UpdateBattleAction29_40AE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 141 | 0x0040AF30 | `sub_40AF30` | 有正文 | `src/YobiRuntime.cpp:26157 — UpdateBattleAction30_40AF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 142 | 0x0040B020 | `sub_40B020` | 有正文 | `src/YobiRuntime.cpp:26188 — UpdateBattleAction31_40B020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 143 | 0x0040B130 | `sub_40B130` | 有正文 | `src/YobiRuntime.cpp:26574 — UpdateBattleAction45_40B130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 144 | 0x0040B180 | `sub_40B180` | 有正文 | `src/YobiRuntime.cpp:33183 — UpdateBattleAction251_40B180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 145 | 0x0040B200 | `sub_40B200` | 有正文 | `src/YobiRuntime.cpp:26584 — UpdateBattleAction46_40B200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 146 | 0x0040B3C0 | `sub_40B3C0` | 有正文 | `src/YobiRuntime.cpp:26724 — UpdateBattleAction50_40B3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 147 | 0x0040B3F0 | `sub_40B3F0` | 有正文 | `src/YobiRuntime.cpp:26662 — UpdateBattleAction47_40B3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 148 | 0x0040B410 | `sub_40B410` | 有正文 | `src/YobiRuntime.cpp:26227 — UpdateBattleAction32_40B410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 149 | 0x0040B450 | `sub_40B450` | 有正文 | `src/YobiRuntime.cpp:26238 — UpdateBattleAction33_40B450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 150 | 0x0040B490 | `sub_40B490` | 有正文 | `src/YobiRuntime.cpp:26671 — UpdateBattleAction48_40B490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 151 | 0x0040B5B0 | `sub_40B5B0` | 有正文 | `src/YobiRuntime.cpp:26713 — UpdateBattleAction49_40B5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 152 | 0x0040B5E0 | `sub_40B5E0` | 有正文 | `src/YobiRuntime.cpp:26249 — UpdateBattleAction34_40B5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 153 | 0x0040B930 | `sub_40B930` | 有正文 | `src/YobiRuntime.cpp:26341 — UpdateBattleAction35_40B930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 154 | 0x0040BAD0 | `sub_40BAD0` | 有正文 | `src/YobiRuntime.cpp:26399 — UpdateBattleAction36_40BAD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 155 | 0x0040BB20 | `sub_40BB20` | 有正文 | `src/YobiRuntime.cpp:26415 — UpdateBattleAction37_40BB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 156 | 0x0040BB70 | `sub_40BB70` | 有正文 | `src/YobiRuntime.cpp:26431 — UpdateBattleAction38_40BB70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 157 | 0x0040BBC0 | `sub_40BBC0` | 有正文 | `src/YobiRuntime.cpp:26447 — UpdateBattleAction39_40BBC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 158 | 0x0040BD40 | `sub_40BD40` | 有正文 | `src/YobiRuntime.cpp:26499 — UpdateBattleAction40_40BD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 159 | 0x0040BD60 | `sub_40BD60` | 有正文 | `src/YobiRuntime.cpp:26524 — UpdateBattleAction41_40BD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 160 | 0x0040BDB0 | `sub_40BDB0` | 有正文 | `src/YobiRuntime.cpp:26538 — UpdateBattleAction42_40BDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 161 | 0x0040BDF0 | `sub_40BDF0` | 有正文 | `src/YobiRuntime.cpp:26563 — UpdateBattleAction44_40BDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 162 | 0x0040BE30 | `sub_40BE30` | 有正文 | `src/YobiRuntime.cpp:26550 — UpdateBattleAction43_40BE30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 163 | 0x0040BE70 | `sub_40BE70` | 有正文 | `src/YobiRuntime.cpp:26734 — UpdateBattleAction51_40BE70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 164 | 0x0040BE90 | `sub_40BE90` | 有正文 | `src/YobiRuntime.cpp:26743 — UpdateBattleAction52_40BE90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 165 | 0x0040BF20 | `sub_40BF20` | 有正文 | `src/YobiRuntime.cpp:26766 — UpdateBattleAction53_40BF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 166 | 0x0040BFE0 | `sub_40BFE0` | 有正文 | `src/YobiRuntime.cpp:26797 — UpdateBattleAction54_40BFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 167 | 0x0040C0C0 | `sub_40C0C0` | 有正文 | `src/YobiRuntime.cpp:26830 — UpdateBattleAction55_40C0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 168 | 0x0040C0E0 | `sub_40C0E0` | 有正文 | `src/YobiRuntime.cpp:27044 — UpdateBattleAction68_40C0E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 169 | 0x0040C120 | `sub_40C120` | 有正文 | `src/YobiRuntime.cpp:27056 — UpdateBattleAction69_40C120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 170 | 0x0040C160 | `sub_40C160` | 有正文 | `src/YobiRuntime.cpp:26839 — UpdateBattleAction56_40C160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 171 | 0x0040C1A0 | `sub_40C1A0` | 有正文 | `src/YobiRuntime.cpp:26852 — UpdateBattleAction57_40C1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 172 | 0x0040C1E0 | `sub_40C1E0` | 有正文 | `src/YobiRuntime.cpp:26864 — UpdateBattleAction58_40C1E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 173 | 0x0040C220 | `sub_40C220` | 有正文 | `src/YobiRuntime.cpp:26888 — UpdateBattleAction60_40C220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 174 | 0x0040C240 | `sub_40C240` | 有正文 | `src/YobiRuntime.cpp:26876 — UpdateBattleAction59_40C240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 175 | 0x0040C280 | `sub_40C280` | 有正文 | `src/YobiRuntime.cpp:26896 — UpdateBattleAction61_40C280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 176 | 0x0040C2B0 | `sub_40C2B0` | 有正文 | `src/YobiRuntime.cpp:26906 — UpdateBattleAction62_40C2B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 177 | 0x0040C2E0 | `sub_40C2E0` | 有正文 | `src/YobiRuntime.cpp:26916 — UpdateBattleAction63_40C2E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 178 | 0x0040C380 | `sub_40C380` | 有正文 | `src/YobiRuntime.cpp:26956 — UpdateBattleAction65_40C380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 179 | 0x0040C480 | `sub_40C480` | 有正文 | `src/YobiRuntime.cpp:27006 — UpdateBattleAction66_40C480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 180 | 0x0040C4D0 | `sub_40C4D0` | 有正文 | `src/YobiRuntime.cpp:27068 — UpdateBattleAction71_40C4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 181 | 0x0040C520 | `sub_40C520` | 有正文 | `src/YobiRuntime.cpp:27275 — UpdateBattleAction86_40C520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 182 | 0x0040C780 | `sub_40C780` | 有正文 | `src/YobiRuntime.cpp:27348 — UpdateBattleAction87_40C780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 183 | 0x0040C8D0 | `sub_40C8D0` | 有正文 | `src/YobiRuntime.cpp:27389 — UpdateBattleAction88_40C8D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 184 | 0x0040C8F0 | `sub_40C8F0` | 有正文 | `src/YobiRuntime.cpp:27398 — UpdateBattleAction89_40C8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 185 | 0x0040C910 | `sub_40C910` | 有正文 | `src/YobiRuntime.cpp:27406 — UpdateBattleAction90_40C910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 186 | 0x0040CAA0 | `sub_40CAA0` | 有正文 | `src/YobiRuntime.cpp:27086 — UpdateBattleAction72_40CAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 187 | 0x0040CAC0 | `sub_40CAC0` | 有正文 | `src/YobiRuntime.cpp:33382 — AdvanceBattleLinkedCast_40CAC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 188 | 0x0040CAF0 | `sub_40CAF0` | 有正文 | `src/YobiRuntime.cpp:33392 — AdvanceBattleEffectAndCast_40CAF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 189 | 0x0040CB10 | `sub_40CB10` | 有正文 | `src/YobiRuntime.cpp:27096 — UpdateBattleAction75_40CB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 190 | 0x0040CB30 | `sub_40CB30` | 有正文 | `src/YobiRuntime.cpp:27105 — UpdateBattleAction76_40CB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 191 | 0x0040CB50 | `sub_40CB50` | 有正文 | `src/YobiRuntime.cpp:27114 — UpdateBattleAction77_40CB50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 192 | 0x0040CB80 | `sub_40CB80` | 有正文 | `src/YobiRuntime.cpp:27123 — UpdateBattleAction78_40CB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 193 | 0x0040CBB0 | `sub_40CBB0` | 有正文 | `src/YobiRuntime.cpp:26948 — UpdateBattleAction64_40CBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 194 | 0x0040CBD0 | `sub_40CBD0` | 有正文 | `src/YobiRuntime.cpp:27132 — UpdateBattleAction79_40CBD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 195 | 0x0040CC80 | `sub_40CC80` | 有正文 | `src/YobiRuntime.cpp:27162 — UpdateBattleAction80_40CC80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 196 | 0x0040CCB0 | `sub_40CCB0` | 有正文 | `src/YobiRuntime.cpp:27173 — UpdateBattleAction81_40CCB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 197 | 0x0040CCE0 | `sub_40CCE0` | 有正文 | `src/YobiRuntime.cpp:27184 — UpdateBattleAction82_40CCE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 198 | 0x0040CD10 | `sub_40CD10` | 有正文 | `src/YobiRuntime.cpp:27195 — UpdateBattleAction83_40CD10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 199 | 0x0040CD70 | `sub_40CD70` | 有正文 | `src/YobiRuntime.cpp:27212 — UpdateBattleAction84_40CD70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 200 | 0x0040CDE0 | `sub_40CDE0` | 有正文 | `src/YobiRuntime.cpp:27233 — UpdateBattleAction85_40CDE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 201 | 0x0040CF30 | `sub_40CF30` | 有正文 | `src/YobiRuntime.cpp:27456 — UpdateBattleAction91_40CF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 202 | 0x0040CF50 | `sub_40CF50` | 有正文 | `src/YobiRuntime.cpp:27465 — UpdateBattleAction92_40CF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 203 | 0x0040CF70 | `sub_40CF70` | 有正文 | `src/YobiRuntime.cpp:27474 — UpdateBattleAction93_40CF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 204 | 0x0040CFA0 | `sub_40CFA0` | 有正文 | `src/YobiRuntime.cpp:23971 — UpdateBattleAction5_40CFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 205 | 0x0040D010 | `sub_40D010` | 有正文 | `src/YobiRuntime.cpp:27486 — UpdateBattleAction94_40D010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 206 | 0x0040D0B0 | `sub_40D0B0` | 有正文 | `src/YobiRuntime.cpp:27507 — UpdateBattleAction95_40D0B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 207 | 0x0040D170 | `sub_40D170` | 有正文 | `src/YobiRuntime.cpp:27537 — UpdateBattleAction96_40D170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 208 | 0x0040D1A0 | `sub_40D1A0` | 有正文 | `src/YobiRuntime.cpp:27547 — UpdateBattleAction97_40D1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 209 | 0x0040D1D0 | `sub_40D1D0` | 有正文 | `src/YobiRuntime.cpp:27557 — UpdateBattleAction98_40D1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 210 | 0x0040D390 | `sub_40D390` | 有正文 | `src/YobiRuntime.cpp:27609 — UpdateBattleAction99_40D390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 211 | 0x0040D3C0 | `sub_40D3C0` | 有正文 | `src/YobiRuntime.cpp:27620 — UpdateBattleAction100_40D3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 212 | 0x0040D580 | `sub_40D580` | 有正文 | `src/YobiRuntime.cpp:27674 — UpdateBattleAction101_40D580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 213 | 0x0040D6C0 | `sub_40D6C0` | 有正文 | `src/YobiRuntime.cpp:27714 — UpdateBattleAction102_40D6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 214 | 0x0040DBE0 | `sub_40DBE0` | 有正文 | `src/YobiRuntime.cpp:30155 — UpdateBattleAction103_40DBE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 215 | 0x0040DC90 | `sub_40DC90` | 有正文 | `src/YobiRuntime.cpp:27861 — UpdateBattleAction104_40DC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 216 | 0x0040DE40 | `sub_40DE40` | 有正文 | `src/YobiRuntime.cpp:27910 — UpdateBattleAction105_40DE40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 217 | 0x0040DF50 | `sub_40DF50` | 有正文 | `src/YobiRuntime.cpp:27953 — UpdateBattleAction106_40DF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 218 | 0x0040DF70 | `sub_40DF70` | 有正文 | `src/YobiRuntime.cpp:27961 — UpdateBattleAction107_40DF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 219 | 0x0040DF80 | `sub_40DF80` | 有正文 | `src/YobiRuntime.cpp:27968 — UpdateBattleAction108_40DF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 220 | 0x0040E4B0 | `sub_40E4B0` | 有正文 | `src/YobiRuntime.cpp:30181 — UpdateBattleAction109_40E4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 221 | 0x0040E560 | `sub_40E560` | 有正文 | `src/YobiRuntime.cpp:28117 — UpdateBattleAction110_40E560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 222 | 0x0040E970 | `sub_40E970` | 有正文 | `src/YobiRuntime.cpp:28248 — UpdateBattleAction111_40E970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 223 | 0x0040EB00 | `sub_40EB00` | 有正文 | `src/YobiRuntime.cpp:30208 — UpdateBattleAction112_40EB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 224 | 0x0040EFE0 | `sub_40EFE0` | 有正文 | `src/YobiRuntime.cpp:28300 — UpdateBattleAction113_40EFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 225 | 0x0040F0A0 | `sub_40F0A0` | 有正文 | `src/YobiRuntime.cpp:28334 — UpdateBattleAction114_40F0A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 226 | 0x0040F790 | `sub_40F790` | 有正文 | `src/YobiRuntime.cpp:29017 — UpdateBattleAction154_40F790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 227 | 0x0040F800 | `sub_40F800` | 有正文 | `src/YobiRuntime.cpp:29033 — UpdateBattleAction155_40F800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 228 | 0x0040FB30 | `sub_40FB30` | 有正文 | `src/YobiRuntime.cpp:29142 — UpdateBattleAction156_40FB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 229 | 0x0040FB80 | `sub_40FB80` | 有正文 | `src/YobiRuntime.cpp:28518 — UpdateBattleAction115_40FB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 230 | 0x0040FC10 | `sub_40FC10` | 有正文 | `src/YobiRuntime.cpp:28534 — UpdateBattleFactoryAction_40FC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 231 | 0x0040FE50 | `sub_40FE50` | 有正文 | `src/YobiRuntime.cpp:33191 — UpdateBattleAction253_40FE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 232 | 0x0040FEB0 | `sub_40FEB0` | 有正文 | `src/YobiRuntime.cpp:33204 — UpdateBattleAction254_40FEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 233 | 0x0040FEF0 | `sub_40FEF0` | 有正文 | `src/YobiRuntime.cpp:29004 — UpdateBattleAction148_40FEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 234 | 0x0040FF40 | `sub_40FF40` | 有正文 | `src/YobiRuntime.cpp:28603 — UpdateBattleAction138_40FF40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 235 | 0x0040FF90 | `sub_40FF90` | 有正文 | `src/YobiRuntime.cpp:28585 — UpdateBattleActions127To129_40FF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 236 | 0x00410000 | `sub_410000` | 有正文 | `src/YobiRuntime.cpp:28617 — UpdateBattleAction139_410000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 237 | 0x004106B0 | `sub_4106B0` | 有正文 | `src/YobiRuntime.cpp:27019 — UpdateBattleAction67_4106B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 238 | 0x00410730 | `sub_410730` | 有正文 | `src/YobiRuntime.cpp:28829 — UpdateBattleAction140_410730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 239 | 0x004107C0 | `sub_4107C0` | 有正文 | `src/YobiRuntime.cpp:29227 — UpdateBattleAction159_4107C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 240 | 0x00410B00 | `sub_410B00` | 有正文 | `src/YobiRuntime.cpp:29234 — UpdateBattleAction160_410B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 241 | 0x00410E30 | `sub_410E30` | 有正文 | `src/YobiRuntime.cpp:29241 — UpdateBattleAction162_410E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 242 | 0x00410E90 | `sub_410E90` | 有正文 | `src/YobiRuntime.cpp:28844 — UpdateBattleAction141_410E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 243 | 0x00410F70 | `sub_410F70` | 有正文 | `src/YobiRuntime.cpp:28884 — UpdateBattleAction142_410F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 244 | 0x00410FC0 | `sub_410FC0` | 有正文 | `src/YobiRuntime.cpp:28896 — UpdateBattleAction143_410FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 245 | 0x00411240 | `sub_411240` | 有正文 | `src/YobiRuntime.cpp:28955 — UpdateBattleAction144_411240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 246 | 0x00411290 | `sub_411290` | 有正文 | `src/YobiRuntime.cpp:28968 — UpdateBattleAction145_411290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 247 | 0x00411320 | `sub_411320` | 有正文 | `src/YobiRuntime.cpp:28987 — UpdateBattleAction147_411320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 248 | 0x004113A0 | `sub_4113A0` | 有正文 | `src/YobiRuntime.cpp:29434 — UpdateBattleAction163_4113A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 249 | 0x00411A50 | `sub_411A50` | 有正文 | `src/YobiRuntime.cpp:29642 — UpdateBattleAction164_411A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 250 | 0x00411CB0 | `sub_411CB0` | 有正文 | `src/YobiRuntime.cpp:29802 — UpdateBattleAction166_411CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 251 | 0x00411D30 | `sub_411D30` | 有正文 | `src/YobiRuntime.cpp:29715 — UpdateBattleAction165_411D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 252 | 0x00411FA0 | `sub_411FA0` | 有正文 | `src/YobiRuntime.cpp:29819 — UpdateBattleAction167_411FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 253 | 0x00411FC0 | `sub_411FC0` | 有正文 | `src/YobiRuntime.cpp:29826 — UpdateBattleAction168_411FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 254 | 0x00411FE0 | `sub_411FE0` | 有正文 | `src/YobiRuntime.cpp:29833 — UpdateBattleAction169_411FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 255 | 0x00412000 | `sub_412000` | 有正文 | `src/YobiRuntime.cpp:29878 — UpdateBattleAction170_412000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 256 | 0x00412020 | `sub_412020` | 有正文 | `src/YobiRuntime.cpp:29885 — UpdateBattleAction171_412020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 257 | 0x004120F0 | `sub_4120F0` | 有正文 | `src/YobiRuntime.cpp:29909 — UpdateBattleActions172_173_4120F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 258 | 0x004121D0 | `sub_4121D0` | 有正文 | `src/YobiRuntime.cpp:29934 — UpdateBattleAction174_4121D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 259 | 0x00412210 | `sub_412210` | 有正文 | `src/YobiRuntime.cpp:29946 — UpdateBattleAction175_412210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 260 | 0x00412260 | `sub_412260` | 有正文 | `src/YobiRuntime.cpp:29958 — UpdateBattleAction176_412260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 261 | 0x00412290 | `sub_412290` | 有正文 | `src/YobiRuntime.cpp:29970 — UpdateBattleAction177_412290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 262 | 0x004122C0 | `sub_4122C0` | 有正文 | `src/YobiRuntime.cpp:29979 — UpdateBattleAction178_4122C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 263 | 0x004122E0 | `sub_4122E0` | 有正文 | `src/YobiRuntime.cpp:29986 — UpdateBattleAction179_4122E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 264 | 0x00412310 | `sub_412310` | 有正文 | `src/YobiRuntime.cpp:29995 — UpdateBattleActions180_203_412310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 265 | 0x004124E0 | `sub_4124E0` | 有正文 | `src/YobiRuntime.cpp:30037 — UpdateBattleAction204_4124E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 266 | 0x004127B0 | `sub_4127B0` | 有正文 | `src/YobiRuntime.cpp:30122 — UpdateBattleAction205_4127B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 267 | 0x004127F0 | `sub_4127F0` | 有正文 | `src/YobiRuntime.cpp:30133 — UpdateBattleAction206_4127F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 268 | 0x00412830 | `sub_412830` | 有正文 | `src/YobiRuntime.cpp:30143 — UpdateBattleAction207_412830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 269 | 0x00412870 | `sub_412870` | 有正文 | `src/YobiRuntime.cpp:31712 — SetBattleAction208Target_412870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 270 | 0x00412900 | `sub_412900` | 有正文 | `src/YobiRuntime.cpp:31731 — UpdateBattleAction209_412900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 271 | 0x00412F70 | `sub_412F70` | 有正文 | `src/YobiRuntime.cpp:31948 — UpdateBattleAction210_412F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 272 | 0x00413070 | `sub_413070` | 有正文 | `src/YobiRuntime.cpp:31971 — ApplyBattleSceneEffects_413070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 273 | 0x00413110 | `sub_413110` | 有正文 | `src/YobiRuntime.cpp:32013 — UpdateBattleAction212_413110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 274 | 0x004131B0 | `sub_4131B0` | 有正文 | `src/YobiRuntime.cpp:32039 — UpdateBattleAction213_4131B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 275 | 0x004132B0 | `sub_4132B0` | 有正文 | `src/YobiRuntime.cpp:32085 — UpdateBattleAction214_4132B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 276 | 0x004133D0 | `sub_4133D0` | 有正文 | `src/YobiRuntime.cpp:32126 — UpdateBattleAction215_4133D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 277 | 0x00413400 | `sub_413400` | 有正文 | `src/YobiRuntime.cpp:32136 — UpdateBattleAction216_413400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 278 | 0x00413550 | `sub_413550` | 有正文 | `src/YobiRuntime.cpp:32192 — UpdateBattleAction218_413550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 279 | 0x00413750 | `sub_413750` | 有正文 | `src/YobiRuntime.cpp:32324 — UpdateBattleAction219_413750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 280 | 0x00413980 | `sub_413980` | 有正文 | `src/YobiRuntime.cpp:32349 — UpdateBattleAction220_413980` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 281 | 0x00413B40 | `sub_413B40` | 有正文 | `src/YobiRuntime.cpp:32358 — UpdateBattleAction221_413B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 282 | 0x00413E90 | `sub_413E90` | 有正文 | `src/YobiRuntime.cpp:32457 — UpdateBattleAction222_413E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 283 | 0x00413FE0 | `sub_413FE0` | 有正文 | `src/YobiRuntime.cpp:32495 — UpdateBattleAction223_413FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 284 | 0x00414050 | `sub_414050` | 有正文 | `src/YobiRuntime.cpp:30478 — UpdateBattleAction224_414050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 285 | 0x004150F0 | `sub_4150F0` | 有正文 | `src/YobiRuntime.cpp:31093 — UpdateBattleAction225_4150F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 286 | 0x004154C0 | `sub_4154C0` | 有正文 | `src/YobiRuntime.cpp:30850 — UpdateBattleAction226_4154C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 287 | 0x00415940 | `sub_415940` | 有正文 | `src/YobiRuntime.cpp:30943 — UpdateBattleAction227_415940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 288 | 0x00416000 | `sub_416000` | 有正文 | `src/YobiRuntime.cpp:31211 — UpdateBattleAction228_416000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 289 | 0x00416020 | `sub_416020` | 有正文 | `src/YobiRuntime.cpp:31220 — UpdateBattleAction229_416020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 290 | 0x004160A0 | `sub_4160A0` | 有正文 | `src/YobiRuntime.cpp:31236 — UpdateBattleAction230_4160A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 291 | 0x00416150 | `sub_416150` | 有正文 | `src/YobiRuntime.cpp:31258 — UpdateBattleAction231_416150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 292 | 0x00416220 | `sub_416220` | 有正文 | `src/YobiRuntime.cpp:31289 — UpdateBattleAction232_416220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 293 | 0x00416860 | `sub_416860` | 有正文 | `src/YobiRuntime.cpp:31457 — UpdateBattleAction233_416860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 294 | 0x00416AB0 | `sub_416AB0` | 有正文 | `src/YobiRuntime.cpp:31536 — UpdateBattleAction234_416AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 295 | 0x00416DB0 | `sub_416DB0` | 有正文 | `src/YobiRuntime.cpp:31704 — ApplyBattleCastEffects_416DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 296 | 0x00416DD0 | `sub_416DD0` | 有正文 | `src/YobiRuntime.cpp:32517 — UpdateBattleAction237_416DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 297 | 0x00417410 | `sub_417410` | 有正文 | `src/YobiRuntime.cpp:32695 — UpdateBattleActions238_239_417410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 298 | 0x00417AC0 | `sub_417AC0` | 有正文 | `src/YobiRuntime.cpp:32893 — UpdateBattleAction240_417AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 299 | 0x00417B80 | `sub_417B80` | 有正文 | `src/YobiRuntime.cpp:30341 — UpdateBattleAction241_417B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 300 | 0x00417CC0 | `sub_417CC0` | 有正文 | `src/YobiRuntime.cpp:30382 — UpdateBattleAction242_417CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 301 | 0x00417D80 | `sub_417D80` | 有正文 | `src/YobiRuntime.cpp:32921 — UpdateBattleAction243_417D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 302 | 0x00418550 | `sub_418550` | 有正文 | `src/YobiRuntime.cpp:33120 — UpdateBattleAction244_418550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 303 | 0x00418580 | `sub_418580` | 有正文 | `src/YobiRuntime.cpp:33130 — UpdateBattleAction245_418580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 304 | 0x004185C0 | `sub_4185C0` | 有正文 | `src/YobiRuntime.cpp:33142 — UpdateBattleAction246_4185C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 305 | 0x004185F0 | `sub_4185F0` | 有正文 | `src/YobiRuntime.cpp:33152 — UpdateBattleAction247_4185F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 306 | 0x00418620 | `sub_418620` | 有正文 | `src/YobiRuntime.cpp:33162 — UpdateBattleAction248_418620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 307 | 0x004186B0 | `sub_4186B0` | 有正文 | `src/YobiRuntime.cpp:33217 — UpdateBattleAction255_4186B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 308 | 0x00418B50 | `sub_418B50` | 有正文 | `src/YobiRuntime.cpp:33375 — UpdateBattleAction256_418B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 309 | 0x00418B70 | `Battle::SpiracleShot::newEffect` | 有正文 | `src/YobiRuntime.cpp:9436 — NewBattleSpiracleShot_418B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 310 | 0x00418D80 | `YobiBattleEntry::ProjectionUpdate` | 有正文 | `src/YobiRuntime.cpp:9368 — UpdateBattleProjection_418D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 311 | 0x00418DF0 | `YobiBattleEntry::newProjection` | 有正文 | `src/YobiRuntime.cpp:9389 — NewBattleProjection_418DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 312 | 0x00418F10 | `sub_418F10` | 有正文 | `src/YobiRuntime.cpp:9277 — UpdateBattleArcProjection_418F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 313 | 0x00418FF0 | `sub_418FF0` | 有正文 | `src/YobiRuntime.cpp:9317 — NewBattleArcProjection_418FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 314 | 0x00419180 | `Battle::BlastWave::newEffect` | 有正文 | `src/YobiRuntime.cpp:9507 — NewBattleBlastWave_419180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 315 | 0x00419390 | `Battle::RangeHit::newEffect` | 有正文 | `src/YobiRuntime.cpp:9578 — NewBattleRangeHit_419390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 316 | 0x004195F0 | `Battle::Boomerange::newEffect` | 有正文 | `src/YobiRuntime.cpp:9745 — NewBattleBoomerang_4195F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 317 | 0x00419BE0 | `sub_419BE0` | 有正文 | `src/YobiRuntime.cpp:10630 — UpdateBattleTransitionEffect_419BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 318 | 0x00419C10 | `sub_419C10` | 有正文 | `src/YobiRuntime.cpp:10640 — NewBattleTransitionEffect_419C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 319 | 0x00419CB0 | `sub_419CB0` | 有正文 | `src/YobiRuntime.cpp:10669 — UpdateBattleOffsetFollowEffect_419CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 320 | 0x00419CF0 | `sub_419CF0` | 有正文 | `src/YobiRuntime.cpp:10686 — NewBattleOffsetFollowEffect_419CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 321 | 0x00419DA0 | `sub_419DA0` | 有正文 | `src/YobiRuntime.cpp:9655 — NewBattleArcShot_419DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 322 | 0x00419FA0 | `sub_419FA0` | 有正文 | `src/YobiRuntime.cpp:10068 — UpdateBattleOneShotEffect_419FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 323 | 0x00419FC0 | `sub_419FC0` | 有正文 | `src/YobiRuntime.cpp:10076 — NewBattleRandomEffect_419FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 324 | 0x0041A0D0 | `sub_41A0D0` | 有正文 | `src/YobiRuntime.cpp:10109 — NewBattleScreenEffect_41A0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 325 | 0x0041A160 | `sub_41A160` | 有正文 | `src/YobiRuntime.cpp:6890 — UpdateBattleCompletingEffect_41A160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 326 | 0x0041A1C0 | `sub_41A1C0` | 有正文 | `src/YobiRuntime.cpp:6909 — UpdateBattleParentEffect_41A1C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 327 | 0x0041A200 | `sub_41A200` | 有正文 | `src/YobiRuntime.cpp:6923 — UpdateBattleRisingEffect_41A200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 328 | 0x0041A270 | `sub_41A270` | 有正文 | `src/YobiRuntime.cpp:6947 — UpdateBattleScalingEffect_41A270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 329 | 0x0041A310 | `sub_41A310` | 有正文 | `src/YobiRuntime.cpp:6977 — UpdateBattleRandomEmitter_41A310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 330 | 0x0041A530 | `sub_41A530` | 有正文 | `src/YobiRuntime.cpp:7033 — UpdateBattleFallingEffect_41A530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 331 | 0x0041A590 | `sub_41A590` | 有正文 | `src/YobiRuntime.cpp:7048 — UpdateBattleReverseEmitter_41A590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 332 | 0x0041A7D0 | `sub_41A7D0` | 有正文 | `src/YobiRuntime.cpp:7101 — UpdateBattleIndexedEmitter_41A7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 333 | 0x0041A9E0 | `sub_41A9E0` | 有正文 | `src/YobiRuntime.cpp:7177 — UpdateBattleImpactEffect_41A9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 334 | 0x0041AF00 | `sub_41AF00` | 有正文 | `src/YobiRuntime.cpp:7343 — UpdateBattleImpactSoundEffect_41AF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 335 | 0x0041B440 | `sub_41B440` | 有正文 | `src/YobiRuntime.cpp:7510 — UpdateBattleFrameEventEffect_41B440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 336 | 0x0041BB00 | `sub_41BB00` | 有正文 | `src/YobiRuntime.cpp:7715 — UpdateBattleMovingImpactEffect_41BB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 337 | 0x0041C3B0 | `sub_41C3B0` | 有正文 | `src/YobiRuntime.cpp:7965 — UpdateBattleHomingImpactEffect_41C3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 338 | 0x0041CD00 | `sub_41CD00` | 有正文 | `src/YobiRuntime.cpp:8396 — NewBattleEffect_41CD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 339 | 0x0041E4D0 | `sub_41E4D0` | 有正文 | `src/YobiRuntime.cpp:9932 — UpdateBattleScatterEffect_41E4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 340 | 0x0041E510 | `sub_41E510` | 有正文 | `src/YobiRuntime.cpp:9946 — NewBattleScatterEffect_41E510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 341 | 0x0041E5C0 | `sub_41E5C0` | 有正文 | `src/YobiRuntime.cpp:1874 — UpdateBattleAfterimage_41E5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 342 | 0x0041E5F0 | `sub_41E5F0` | 有正文 | `src/YobiRuntime.cpp:1886 — NewBattleAfterimage_41E5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 343 | 0x0041E640 | `sub_41E640` | 有正文 | `src/YobiRuntime.cpp:9971 — UpdateBattlePositionEffect_41E640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 344 | 0x0041E680 | `sub_41E680` | 有正文 | `src/YobiRuntime.cpp:9983 — NewBattlePositionEffect_41E680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 345 | 0x0041E720 | `sub_41E720` | 有正文 | `src/YobiRuntime.cpp:10010 — NewBattleCoordinateEffect_41E720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 346 | 0x0041E7C0 | `sub_41E7C0` | 有正文 | `src/YobiRuntime.cpp:10036 — NewBattleTrailEffect_41E7C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 347 | 0x0041E890 | `sub_41E890` | 有正文 | `src/YobiRuntime.cpp:8929 — DrawBattleTiledEffect_41E890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 348 | 0x0041E920 | `sub_41E920` | 有正文 | `src/YobiRuntime.cpp:8956 — UpdateBattleTiledEffect_41E920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 349 | 0x0041E9D0 | `sub_41E9D0` | 有正文 | `src/YobiRuntime.cpp:8989 — NewBattleTiledEffect_41E9D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 350 | 0x0041EAB0 | `sub_41EAB0` | 有正文 | `src/YobiRuntime.cpp:1854 — CloneBattleAction_41EAB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 351 | 0x0041EB10 | `sub_41EB10` | 有正文 | `src/YobiRuntime.cpp:1741 — CopyYobiAction_41EB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 352 | 0x0041F110 | `sub_41F110` | 有正文 | `src/YobiRuntime.cpp:10134 — UpdateBattleNumber_41F110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 353 | 0x0041F1A0 | `sub_41F1A0` | 有正文 | `src/YobiRuntime.cpp:10167 — NewBattleNumber_41F1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 354 | 0x0041F450 | `sub_41F450` | 有正文 | `src/YobiRuntime.cpp:10302 — UpdateBattleFloatingIcon_41F450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 355 | 0x0041F4C0 | `CreateSubYobi` | 有正文 | `src/YobiRuntime.cpp:10329 — NewBattleFloatingIcon_41F4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 356 | 0x0041F570 | `sub_41F570` | 有正文 | `src/YobiRuntime.cpp:10468 — UpdateBattleHitEffect_41F570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 357 | 0x0041F5B0 | `Battle::Boomerange::newHitEffect` | 有正文 | `src/YobiRuntime.cpp:10485 — NewBattleHitEffect_41F5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 358 | 0x0041F660 | `BattleYobi::Debuff::newEffect` | 有正文 | `src/YobiRuntime.cpp:10518 — NewBattleDebuffEffect_41F660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 359 | 0x0041F800 | `BattleYobi::Debuff::UpdateEffect` | 有正文 | `src/YobiRuntime.cpp:10571 — UpdateBattleDebuffEffect_41F800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 360 | 0x0041F860 | `sub_41F860` | 有正文 | `src/YobiRuntime.cpp:10592 — NewBattleReverseEffect_41F860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 361 | 0x0041F950 | `sub_41F950` | 有正文 | `src/YobiRuntime.cpp:10719 — UpdateBattleCenteredEffect_41F950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 362 | 0x0041FA00 | `sub_41FA00` | 有正文 | `src/YobiRuntime.cpp:10743 — NewBattleCenteredEffect_41FA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 363 | 0x0041FB20 | `sub_41FB20` | 有正文 | `src/YobiRuntime.cpp:10783 — UpdateBattleAttachedEffect_41FB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 364 | 0x0041FBE0 | `sub_41FBE0` | 有正文 | `src/YobiRuntime.cpp:10822 — NewBattleAttachedEffect_41FBE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 365 | 0x0041FCD0 | `sub_41FCD0` | 有正文 | `src/YobiRuntime.cpp:10854 — UpdateBattleStatusEffect_41FCD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 366 | 0x0041FE00 | `sub_41FE00` | 有正文 | `src/YobiRuntime.cpp:10915 — NewBattleStatusEffect_41FE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 367 | 0x0041FF90 | `sub_41FF90` | 有正文 | `src/YobiRuntime.cpp:10970 — UpdateBattleFieldEffect_41FF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 368 | 0x0041FFE0 | `sub_41FFE0` | 有正文 | `src/YobiRuntime.cpp:10988 — NewBattleFieldEffect_41FFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 369 | 0x00420100 | `sub_420100` | 有正文 | `src/YobiRuntime.cpp:6732 — NewBattleSequence_420100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 370 | 0x00420300 | `sub_420300` | 有正文 | `src/YobiRuntime.cpp:6806 — UpdateBattleSequence_420300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 371 | 0x00420590 | `sub_420590` | 有正文 | `src/YobiRuntime.cpp:6789 — MoveBattleSequence_420590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 372 | 0x004205F0 | `sub_4205F0` | 有正文 | `src/YobiRuntime.cpp:9712 — NewBattleCastCopy_4205F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 373 | 0x004206C0 | `sub_4206C0` | 有正文 | `src/YobiRuntime.cpp:10235 — NewBattleTimeNumber_4206C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 374 | 0x004208F0 | `sub_4208F0` | 有正文 | `src/YobiRuntime.cpp:10361 — NewBattleRebirthEffects_4208F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 375 | 0x004209C0 | `sub_4209C0` | 有正文 | `src/YobiRuntime.cpp:10388 — NewBattleAttributeEffects_4209C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 376 | 0x00420D50 | `sub_420D50` | 有正文 | `src/YobiRuntime.cpp:7143 — NewBattleRisingEffect_420D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 377 | 0x00420F10 | `sub_420F10` | 有正文 | `src/YobiRuntime.cpp:29840 — SyncBattleAttachmentAlpha_420F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 378 | 0x00421480 | `sub_421480` | 有正文 | `src/YobiRuntime.cpp:11035 — UpdateBattleBackgroundFade_421480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 379 | 0x00421570 | `sub_421570` | 有正文 | `src/YobiRuntime.cpp:11093 — NewBattleBackgroundFade_421570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 380 | 0x00421610 | `sub_421610` | 有正文 | `src/YobiRuntime.cpp:11217 — NewBattleScreenEffect_421610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 381 | 0x004216B0 | `sub_4216B0` | 有正文 | `src/YobiRuntime.cpp:11245 — UpdateBattleScreenEmitter_4216B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 382 | 0x00421700 | `sub_421700` | 有正文 | `src/YobiRuntime.cpp:11260 — NewBattleScreenEmitter_421700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 383 | 0x00421750 | `sub_421750` | 有正文 | `src/YobiRuntime.cpp:6638 — UpdateBattleText_421750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 384 | 0x004217F0 | `sub_4217F0` | 有正文 | `src/YobiRuntime.cpp:6667 — NewBattleText_4217F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 385 | 0x004219D0 | `sub_4219D0` | 有正文 | `src/YobiRuntime.cpp:8225 — UpdateBattleCarryingEffect_4219D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 386 | 0x00421F90 | `sub_421F90` | 有正文 | `src/YobiRuntime.cpp:11278 — UpdateBattleAttachment_421F90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 387 | 0x00421FB0 | `sub_421FB0` | 有正文 | `src/YobiRuntime.cpp:11289 — SyncBattleRider_421FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 388 | 0x004221F0 | `sub_4221F0` | 有正文 | `src/YobiRuntime.cpp:11563 — NewBattleRider_4221F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 389 | 0x004222B0 | `YobiChara::SetRider` | 有正文 | `src/YobiRuntime.cpp:11662 — SetBattleRider_4222B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 390 | 0x00422370 | `sub_422370` | 有正文 | `src/YobiRuntime.cpp:11126 — RestoreBattleStateEffects_422370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 391 | 0x004225D0 | `sub_4225D0` | 有正文 | `src/YobiRuntime.cpp:11374 — SyncBattleGear_4225D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 392 | 0x00422780 | `sub_422780` | 有正文 | `src/YobiRuntime.cpp:11455 — SyncBattleAuxGear_422780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 393 | 0x00422A30 | `sub_422A30` | 有正文 | `src/YobiRuntime.cpp:11594 — NewBattleGear_422A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 394 | 0x00422B00 | `sub_422B00` | 有正文 | `src/YobiRuntime.cpp:11626 — NewBattleAuxGear_422B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 395 | 0x00422BC0 | `sub_422BC0` | 有正文 | `src/YobiRuntime.cpp:11694 — SetBattleGear_422BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 396 | 0x00422C20 | `sub_422C20` | 有正文 | `src/YobiRuntime.cpp:11715 — SetBattleGear_422C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 397 | 0x00422C90 | `sub_422C90` | 有正文 | `src/YobiRuntime.cpp:11737 — SetBattleAuxGear_422C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 398 | 0x00422E50 | `sub_422E50` | 有正文 | `src/YobiRuntime.cpp:9013 — InitBattleTargetEffect_422E50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 399 | 0x00423040 | `sub_423040` | 有正文 | `src/YobiRuntime.cpp:9075 — InitBattleOffsetEffect_423040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 400 | 0x004231D0 | `sub_4231D0` | 有正文 | `src/YobiRuntime.cpp:9125 — InitBattleCenterEffect_4231D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 401 | 0x00423350 | `sub_423350` | 有正文 | `src/YobiRuntime.cpp:9175 — InitBattleLowerEffect_423350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 402 | 0x004234B0 | `sub_4234B0` | 有正文 | `src/YobiRuntime.cpp:9222 — InitBattleScreenYEffect_4234B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 403 | 0x00423640 | `sub_423640` | 有正文 | `src/platform/MainLoopSupport.cpp:717 — Sub_423640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 404 | 0x004237E0 | `GetBattlePosition` | 有正文 | `src/YobiRuntime.cpp:22554 — GetBattlePosition_4237E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 405 | 0x00423800 | `sub_423800` | 有正文 | `src/YobiRuntime.cpp:11823 — MatchBattleAlbum_423800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 406 | 0x004238F0 | `sub_4238F0` | 有正文 | `src/YobiRuntime.cpp:11849 — UpdateBattleAlbumFlag_4238F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 407 | 0x00423960 | `Battle::ParsePacket_M::GetHeader2` | 有正文 | `src/YobiRuntime.cpp:14331 — GetBattleMObjectHeader_423960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 408 | 0x004239D0 | `Battle::ParsePacket_M_RID` | 有正文 | `src/YobiRuntime.cpp:21858 — ParseBattleMRide_4239D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 409 | 0x004242B0 | `sub_4242B0` | 有正文 | `src/YobiRuntime.cpp:11888 — AppendBattleRedirect_4242B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 410 | 0x00424500 | `sub_424500` | 有正文 | `src/YobiRuntime.cpp:11928 — AppendBattleSkillTail_424500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 411 | 0x00424620 | `sub_424620` | 有正文 | `src/YobiRuntime.cpp:11964 — TakeBattleMSkillAttackTargets_424620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 412 | 0x00426FC0 | `sub_426FC0` | 有正文 | `src/YobiRuntime.cpp:13081 — TakeBattleMSkillTargets_426FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 413 | 0x00427470 | `Yobi::BattleSceneUpdate` | 有正文 | `src/YobiRuntime.cpp:21664 — UpdateBattleEvent_427470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 414 | 0x00438230 | `sub_438230` | 有承接 | `src/YobiRuntime.cpp:19753 — targetPayload->value13D8 != 1 \|\| targetPayload->value13DC != statusType` | 原 type=1 且 value 匹配；当前战斗 M 消费内联；完整业务／运行未验 |
| 415 | 0x00438250 | `sub_438250` | 有承接 | `src/YobiRuntime.cpp:20088 — targetPayload->value13D8 != 3 \|\| targetPayload->value13DC != type` | 原 type=3 且 value 匹配；当前战斗 M 消费内联；完整业务／运行未验 |
| 416 | 0x00438270 | `sub_438270` | 有承接 | `src/platform/LegacyCryptInt.cpp:66 — LegacyCryptInt::operator=(const LegacyCryptInt&)` | 原三字段载荷复制由同类赋值承接，保留目标token、无读取／RNG，返回目标引用；完整业务／运行未验 |
| 417 | 0x00438290 | `YobiBattleChara::new` | 有正文 | `src/YobiRuntime.cpp:21819 — NewBattleActor_438290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 418 | 0x004383B0 | `sub_4383B0` | 有正文 | `src/YobiRuntime.cpp:21787 — NewBattleScene_4383B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 419 | 0x004383F0 | `sub_4383F0` | 有正文 | `src/YobiRuntime.cpp:21803 — InitializeBattleScene_4383F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 420 | 0x00438450 | `getIntFromBattleBuffC1` | 有正文 | `src/YobiRuntime.cpp:22075 — TakeBattleCInteger_438450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 421 | 0x004384C0 | `getCharFromBattleBuffC1` | 有正文 | `src/YobiRuntime.cpp:22107 — TakeBattleCString_4384C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 422 | 0x00438550 | `Battle::ParsePacket_C` | 有正文 | `src/YobiRuntime.cpp:22149 — ParseBattleC_438550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 423 | 0x00438F70 | `Battle::ParsePacket_M::TakeInt` | 有正文 | `src/YobiRuntime.cpp:13231 — TakeBattleMInteger_438F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 424 | 0x00438FE0 | `Battle::ParsePacket_M::TakeStr2` | 有正文 | `src/YobiRuntime.cpp:13263 — TakeBattleMString_438FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 425 | 0x00439070 | `Battle::ParsePacket_M::GetHeader` | 有正文 | `src/YobiRuntime.cpp:13205 — GetBattleMHeader_439070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 426 | 0x004390E0 | `InitBattleEntry` | 有正文 | `src/YobiRuntime.cpp:29278 — InitBattleCastList_4390E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 427 | 0x004391E0 | `sub_4391E0` | 有正文 | `src/YobiRuntime.cpp:29310 — ResetBattleCastList_4391E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 428 | 0x004392A0 | `BattleScene::AppendCastList` | 有正文 | `src/YobiRuntime.cpp:29332 — AppendBattleCast_4392A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 429 | 0x004392F0 | `BattleScene::AppendCastList2` | 有正文 | `src/YobiRuntime.cpp:29348 — AppendBattleCastAt_4392F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 430 | 0x00439340 | `sub_439340` | 有正文 | `src/YobiRuntime.cpp:29365 — AdvanceBattleCastState_439340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 431 | 0x00439380 | `BattleEntry::MoveState` | 有正文 | `src/YobiRuntime.cpp:33399 — MoveBattleCastState_439380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 432 | 0x004393A0 | `BattleEntry::AppendEffectList` | 有正文 | `src/YobiRuntime.cpp:29383 — AppendBattleEffectList_4393A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 433 | 0x00439400 | `BattleEntry::AppendEffect` | 有正文 | `src/YobiRuntime.cpp:29399 — AppendBattleEffect_439400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 434 | 0x00439470 | `BattleEntry::MoveEffect` | 有正文 | `src/YobiRuntime.cpp:31632 — AdvanceBattleEffect_439470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 435 | 0x004394A0 | `sub_4394A0` | 有正文 | `src/YobiRuntime.cpp:33409 — CheckBattleTurnReady_4394A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 436 | 0x00439600 | `Battle::ParsePacket_M::TakeInt2` | 有正文 | `src/YobiRuntime.cpp:13309 — TakeBattleMPreviewInteger_439600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 437 | 0x00439670 | `Battle::ParsePacket_M::TakeStr3` | 有正文 | `src/YobiRuntime.cpp:13341 — TakeBattleMPreviewString_439670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 438 | 0x00439700 | `Battle::ParsePacket_M::GetHeader3` | 有正文 | `src/YobiRuntime.cpp:13386 — GetBattleMPreviewHeader_439700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 439 | 0x00439770 | `Battle::ParsePacket_M::handleRID` | 有正文 | `src/YobiRuntime.cpp:13524 — ScanBattleMRideParticipants_439770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 440 | 0x00439920 | `sub_439920` | 有正文 | `src/YobiRuntime.cpp:13599 — ScanBattleMAttackParticipants_439920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 441 | 0x00439B30 | `Battle::ParsePacket_M::Most` | 有正文 | `src/YobiRuntime.cpp:13671 — ScanBattleMParticipants_439B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 442 | 0x0043B290 | `sub_43B290` | 有正文 | `src/YobiRuntime.cpp:14130 — CheckBattleMParticipants_43B290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 443 | 0x0043B360 | `sub_43B360` | 有正文 | `src/YobiRuntime.cpp:14172 — DispatchBattleEvents_43B360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 444 | 0x0043B610 | `sub_43B610` | 有正文 | `src/YobiRuntime.cpp:21755 — NewBattleEvent_43B610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 445 | 0x0043B6C0 | `sub_43B6C0` | 有正文 | `src/YobiRuntime.cpp:22466 — FinishBattleEvent_43B6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 446 | 0x0043B7F0 | `sub_43B7F0` | 有正文 | `src/YobiRuntime.cpp:22525 — AppendBattleQueuedTarget_43B7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 447 | 0x0043B830 | `sub_43B830` | 有正文 | `src/YobiRuntime.cpp:31620 — AdvanceBattleQueuedTarget_43B830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 448 | 0x0043B860 | `Battle::ParsePacket_M::TakeInt4` | 有正文 | `src/YobiRuntime.cpp:14302 — PeekBattleMInteger_43B860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 449 | 0x0043B8C0 | `Battle::ParsePacket_M::TakeStr` | 有正文 | `src/YobiRuntime.cpp:13479 — TakeBattleMStringAt_43B8C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 450 | 0x0043B940 | `Battle::ParsePacket_M::TakeInt3` | 有正文 | `src/YobiRuntime.cpp:13412 — TakeBattleMIntegerAt_43B940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 451 | 0x0043B9B0 | `sub_43B9B0` | 有正文 | `src/YobiRuntime.cpp:13443 — FindBattleMTarget_43B9B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 452 | 0x0043BAA0 | `sub_43BAA0` | 有正文 | `src/YobiRuntime.cpp:22567 — Sub_43BAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 453 | 0x0043BC10 | `sub_43BC10` | 有正文 | `src/YobiRuntime.cpp:29414 — AppendBattleAuxEffects_43BC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 454 | 0x0043BC90 | `sub_43BC90` | 有正文 | `src/YobiRuntime.cpp:31644 — TakeBattleAuxEffects_43BC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 455 | 0x0043BD00 | `BattleScene::HasAppendEffect` | 有正文 | `src/YobiRuntime.cpp:22608 — IsBattleAppendEffectSkill_43BD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 456 | 0x0043BE00 | `BattleScene::AddAppendEffect` | 有正文 | `src/YobiRuntime.cpp:22631 — AddBattleAppendEffect_43BE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 457 | 0x0043BEA0 | `sub_43BEA0` | 有正文 | `src/YobiRuntime.cpp:22669 — FindBattleAppendEffectTechnique_43BEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 458 | 0x0043BED0 | `sub_43BED0` | 有正文 | `src/YobiRuntime.cpp:22683 — ClearBattleAppendEffects_43BED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 459 | 0x0043BF00 | `sub_43BF00` | 有正文 | `src/YobiRuntime.cpp:22538 — GetBattleValueBand_43BF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 460 | 0x0043BF30 | `sub_43BF30` | 有正文 | `src/YobiRuntime.cpp:13574 — TakeBattleMAuxGraphics_43BF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 461 | 0x0043BFF0 | `sub_43BFF0` | 有正文 | `src/YobiRuntime.cpp:22709 — Sub_43BFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 462 | 0x0043C040 | `sub_43C040` | 有正文 | `src/YobiRuntime.cpp:22725 — Sub_43C040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 463 | 0x0043C060 | `BattleScene::FlushEntry` | 有正文 | `src/YobiRuntime.cpp:33664 — FlushBattleEntries_43C060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 464 | 0x0043C0A0 | `sub_43C0A0` | 有正文 | `src/YobiRuntime.cpp:22735 — DrawBattleEntryHover_43C0A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 465 | 0x0043C120 | `BattleScene::GetSelectedEntry` | 有正文 | `src/YobiRuntime.cpp:22759 — GetSelectedBattleEntry_43C120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 466 | 0x0043C3E0 | `sub_43C3E0` | 有正文 | `src/ui/WindowRuntime.cpp:37097 — Sub_43C3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 467 | 0x0043C490 | `sub_43C490` | 有正文 | `src/ui/WindowRuntime.cpp:37121 — Sub_43C490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 468 | 0x0043C4B0 | `Battle::GetBattlePetIndex` | 有正文 | `src/world/SceneState.cpp:4296 — Sub_43C4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 469 | 0x0043C4E0 | `BattleScene::InitCommand` | 有正文 | `src/world/SceneState.cpp:4308 — Sub_43C4E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 470 | 0x0043C560 | `BattleScene::CheckCommand` | 有正文 | `src/YobiRuntime.cpp:33556 — CheckBattleCommand_43C560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 471 | 0x0043C8F0 | `sub_43C8F0` | 有正文 | `src/world/SceneState.cpp:10513 — Sub_43C8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 472 | 0x0043C910 | `sub_43C910` | 有正文 | `src/YobiRuntime.cpp:33465 — Sub_43C910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 473 | 0x0043C950 | `sub_43C950` | 有正文 | `src/ui/WindowRuntime.cpp:30941 — Sub_43C950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 474 | 0x0043C9A0 | `sub_43C9A0` | 有正文 | `src/YobiRuntime.cpp:33480 — Sub_43C9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 475 | 0x0043CA50 | `sub_43CA50` | 有正文 | `src/YobiRuntime.cpp:33514 — Sub_43CA50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 476 | 0x0043CA80 | `sub_43CA80` | 有正文 | `src/YobiRuntime.cpp:33528 — Sub_43CA80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 477 | 0x0043CB10 | `sub_43CB10` | 有正文 | `src/world/MapSceneDrawing.cpp:336 — Sub_43CB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 478 | 0x0043CBB0 | `World::DrawBattleScene` | 有正文 | `src/world/SceneState.cpp:7060 — DrawBattleScene_43CBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 479 | 0x0043D760 | `sub_43D760` | 有承接 | `src/platform/LegacyMouse.cpp:52 — WorldObjectsStaticInitializer构造／析构` | 世界表原位置构造，退出由拥有者析构承接；手写atexit退休，登记结果／机器ABI不承诺等价；运行未验 |
| 480 | 0x0043D770 | `sub_43D770` | 有正文 | `src/world/WorldObjectState.cpp:332 — ConstructWorldObjects` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 481 | 0x0043D7A0 | `sub_43D7A0` | 有正文 | `src/world/WorldObjectState.cpp:340 — DestroyWorldObjects` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 482 | 0x0043D7C0 | `sub_43D7C0` | 有承接 | `src/world/WorldObjectState.hpp:19 — WorldObjectRecord::WorldObjectRecord` | 两CryptInt成员按声明序自动构造；千项同一optional数组；C++完成前缀清理承接；运行未验 |
| 483 | 0x0043D810 | `sub_43D810` | 有承接 | `src/world/WorldObjectState.hpp:21 — WorldObjectRecord::~WorldObjectRecord` | 成员1C→0C及数组元素自动逆序析构，退休手工适配；原SEH／完整运行未验 |
| 484 | 0x0043D860 | `sub_43D860` | 有正文 | `src/YobiRuntime.cpp:35963 — Sub_43D860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 485 | 0x0043D970 | `sub_43D970` | 有正文 | `src/YobiRuntime.cpp:36128 — Sub_43D970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 486 | 0x0043D9A0 | `sub_43D9A0` | 有正文 | `src/YobiRuntime.cpp:36140 — Sub_43D9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 487 | 0x0043D9D0 | `sub_43D9D0` | 有正文 | `src/YobiRuntime.cpp:36152 — Sub_43D9D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 488 | 0x0043DA10 | `sub_43DA10` | 有正文 | `src/YobiRuntime.cpp:36167 — Sub_43DA10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 489 | 0x0043DA40 | `sub_43DA40` | 有正文 | `src/YobiRuntime.cpp:36179 — Sub_43DA40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 490 | 0x0043DA70 | `sub_43DA70` | 有正文 | `src/YobiRuntime.cpp:36191 — Sub_43DA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 491 | 0x0043DAA0 | `sub_43DAA0` | 有正文 | `src/YobiRuntime.cpp:36203 — Sub_43DAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 492 | 0x0043DAD0 | `sub_43DAD0` | 有正文 | `src/YobiRuntime.cpp:36215 — Sub_43DAD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 493 | 0x0043DB00 | `sub_43DB00` | 有正文 | `src/YobiRuntime.cpp:36227 — Sub_43DB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 494 | 0x0043DB30 | `sub_43DB30` | 有正文 | `src/YobiRuntime.cpp:36239 — Sub_43DB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 495 | 0x0043DB60 | `BattleEntry::showHpBar` | 有正文 | `src/YobiRuntime.cpp:6302 — ShowHpBar_43DB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 496 | 0x0043DD00 | `sub_43DD00` | 有正文 | `src/YobiRuntime.cpp:4824 — Sub_43DD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 497 | 0x0043E490 | `sub_43E490` | 有正文 | `src/YobiRuntime.cpp:4996 — Sub_43E490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 498 | 0x0043EFC0 | `sub_43EFC0` | 有正文 | `src/YobiRuntime.cpp:5218 — Sub_43EFC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 499 | 0x0043F180 | `sub_43F180` | 有正文 | `src/YobiRuntime.cpp:5569 — Sub_43F180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 500 | 0x0043F1B0 | `sub_43F1B0` | 有正文 | `src/YobiRuntime.cpp:5588 — Sub_43F1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 501 | 0x0043F1F0 | `sub_43F1F0` | 有正文 | `src/YobiRuntime.cpp:5611 — Sub_43F1F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 502 | 0x0043F220 | `sub_43F220` | 有正文 | `src/YobiRuntime.cpp:5629 — Sub_43F220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 503 | 0x0043F230 | `sub_43F230` | 有正文 | `src/YobiRuntime.cpp:5635 — Sub_43F230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 504 | 0x0043F290 | `sub_43F290` | 有正文 | `src/YobiRuntime.cpp:2393 — Sub_43F290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 505 | 0x0043F330 | `YobiMapChara::new` | 有正文 | `src/YobiRuntime.cpp:2289 — Sub_43F330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 506 | 0x0043F520 | `YobiMapChara::Update` | 有正文 | `src/YobiRuntime.cpp:2739 — Sub_43F520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 507 | 0x0043F910 | `sub_43F910` | 有正文 | `src/YobiRuntime.cpp:2484 — Sub_43F910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 508 | 0x0043FD90 | `sub_43FD90` | 有正文 | `src/YobiRuntime.cpp:5657 — Sub_43FD90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 509 | 0x0043FE00 | `sub_43FE00` | 有正文 | `src/YobiRuntime.cpp:2425 — Sub_43FE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 510 | 0x0043FEB0 | `sub_43FEB0` | 有正文 | `src/YobiRuntime.cpp:2336 — Sub_43FEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 511 | 0x00440070 | `sub_440070` | 有正文 | `src/YobiRuntime.cpp:2848 — Sub_440070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 512 | 0x00440440 | `sub_440440` | 有正文 | `src/YobiRuntime.cpp:2568 — Sub_440440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 513 | 0x004408C0 | `sub_4408C0` | 有正文 | `src/YobiRuntime.cpp:2379 — Sub_4408C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 514 | 0x00440920 | `sub_440920` | 有正文 | `src/YobiRuntime.cpp:2938 — Sub_440920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 515 | 0x00440B50 | `sub_440B50` | 有正文 | `src/world/SceneState.cpp:8518 — Sub_440B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 516 | 0x00440B70 | `sub_440B70` | 有正文 | `src/YobiRuntime.cpp:6445 — Sub_440B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 517 | 0x00440C50 | `sub_440C50` | 有正文 | `src/YobiRuntime.cpp:6351 — Sub_440C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 518 | 0x00440F50 | `sub_440F50` | 有正文 | `src/YobiRuntime.cpp:6489 — Sub_440F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 519 | 0x00440FE0 | `sub_440FE0` | 有正文 | `src/YobiRuntime.cpp:6525 — Sub_440FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 520 | 0x00440FF0 | `sub_440FF0` | 有正文 | `src/world/SceneState.cpp:2411 — Sub_440FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 521 | 0x004410E0 | `sub_4410E0` | 有正文 | `src/world/SceneState.cpp:8525 — Sub_4410E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 522 | 0x004410F0 | `YobiGear::new` | 有正文 | `src/YobiRuntime.cpp:6569 — NewYobiGear_4410F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 523 | 0x00441130 | `YobiGearPet::new` | 有正文 | `src/YobiRuntime.cpp:6533 — NewYobiGearPet_441130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 524 | 0x004412D0 | `sub_4412D0` | 有正文 | `src/YobiRuntime.cpp:5774 — Sub_4412D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 525 | 0x00441430 | `sub_441430` | 有正文 | `src/YobiRuntime.cpp:5820 — Sub_441430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 526 | 0x00441570 | `YobiMapChara::DrawGear` | 有正文 | `src/YobiRuntime.cpp:5861 — DrawGear_441570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 527 | 0x004417A0 | `sub_4417A0` | 有正文 | `src/YobiRuntime.cpp:5724 — Sub_4417A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 528 | 0x004417E0 | `sub_4417E0` | 有正文 | `src/YobiRuntime.cpp:5032 — Sub_4417E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 529 | 0x00441DB0 | `sub_441DB0` | 有正文 | `src/YobiRuntime.cpp:3007 — Sub_441DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 530 | 0x00441F60 | `sub_441F60` | 有正文 | `src/YobiRuntime.cpp:4324 — Sub_441F60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 531 | 0x00441FC0 | `sub_441FC0` | 有正文 | `src/YobiRuntime.cpp:4347 — Sub_441FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 532 | 0x00442110 | `sub_442110` | 有正文 | `src/YobiRuntime.cpp:4412 — Sub_442110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 533 | 0x004422F0 | `sub_4422F0` | 有正文 | `src/YobiRuntime.cpp:4449 — Sub_4422F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 534 | 0x00442460 | `sub_442460` | 有正文 | `src/YobiRuntime.cpp:4482 — Sub_442460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 535 | 0x004424B0 | `sub_4424B0` | 有正文 | `src/YobiRuntime.cpp:5278 — Sub_4424B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 536 | 0x00442730 | `sub_442730` | 有正文 | `src/YobiRuntime.cpp:5364 — Sub_442730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 537 | 0x004428F0 | `sub_4428F0` | 有正文 | `src/YobiRuntime.cpp:5419 — Sub_4428F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 538 | 0x00442AB0 | `sub_442AB0` | 有正文 | `src/YobiRuntime.cpp:4300 — Sub_442AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 539 | 0x00442BD0 | `sub_442BD0` | 有正文 | `src/YobiRuntime.cpp:36581 — Sub_442BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 540 | 0x00442C00 | `sub_442C00` | 有正文 | `src/YobiRuntime.cpp:36042 — Sub_442C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 541 | 0x00442C40 | `sub_442C40` | 有正文 | `src/YobiRuntime.cpp:36556 — Sub_442C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 542 | 0x00442C70 | `sub_442C70` | 有正文 | `src/YobiRuntime.cpp:36568 — Sub_442C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 543 | 0x00442CA0 | `sub_442CA0` | 有正文 | `src/YobiRuntime.cpp:36056 — Sub_442CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 544 | 0x00442CD0 | `sub_442CD0` | 有正文 | `src/YobiRuntime.cpp:36068 — Sub_442CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 545 | 0x00442D00 | `sub_442D00` | 有正文 | `src/YobiRuntime.cpp:36080 — Sub_442D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 546 | 0x00442D30 | `sub_442D30` | 有正文 | `src/YobiRuntime.cpp:36092 — Sub_442D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 547 | 0x00442D60 | `sub_442D60` | 有正文 | `src/YobiRuntime.cpp:36104 — Sub_442D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 548 | 0x00442D90 | `sub_442D90` | 有正文 | `src/YobiRuntime.cpp:36116 — Sub_442D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 549 | 0x00442DC0 | `sub_442DC0` | 有正文 | `src/YobiRuntime.cpp:35997 — Sub_442DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 550 | 0x00442F70 | `sub_442F70` | 有正文 | `src/YobiRuntime.cpp:5902 — Sub_442F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 551 | 0x00444000 | `sub_444000` | 有正文 | `src/YobiRuntime.cpp:5733 — Sub_444000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 552 | 0x004441A0 | `sub_4441A0` | 有正文 | `src/YobiRuntime.cpp:3227 — Sub_4441A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 553 | 0x00444260 | `sub_444260` | 有正文 | `src/YobiRuntime.cpp:3262 — Sub_444260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 554 | 0x00444410 | `sub_444410` | 有正文 | `src/YobiRuntime.cpp:36251 — Sub_444410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 555 | 0x00444FA0 | `World::FindObjectIndex` | 有正文 | `src/world/WorldObjectState.cpp:87 — Sub_444FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 556 | 0x00445030 | `sub_445030` | 有正文 | `src/YobiRuntime.cpp:36954 — Sub_445030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 557 | 0x004450D0 | `sub_4450D0` | 有正文 | `src/YobiRuntime.cpp:36983 — Sub_4450D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 558 | 0x00445160 | `sub_445160` | 有正文 | `src/YobiRuntime.cpp:37010 — Sub_445160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 559 | 0x004451F0 | `sub_4451F0` | 有正文 | `src/YobiRuntime.cpp:37037 — Sub_4451F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 560 | 0x004452A0 | `sub_4452A0` | 有正文 | `src/YobiRuntime.cpp:37067 — Sub_4452A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 561 | 0x00445350 | `sub_445350` | 有正文 | `src/YobiRuntime.cpp:36690 — Sub_445350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 562 | 0x00445A60 | `sub_445A60` | 有正文 | `src/world/WorldObjectState.cpp:185 — Sub_445A60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 563 | 0x00445D20 | `sub_445D20` | 有正文 | `src/YobiRuntime.cpp:36836 — Sub_445D20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 564 | 0x00445F40 | `sub_445F40` | 有正文 | `src/YobiRuntime.cpp:36897 — Sub_445F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 565 | 0x00446130 | `World::TakeEmptyObjectIndex` | 有正文 | `src/world/WorldObjectState.cpp:118 — Sub_446130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 566 | 0x004461E0 | `sub_4461E0` | 有正文 | `src/world/WorldObjectState.cpp:150 — Sub_4461E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 567 | 0x00446210 | `sub_446210` | 有正文 | `src/world/WorldObjectState.cpp:165 — Sub_446210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 568 | 0x004462B0 | `World::FindObjectYobi` | 有正文 | `src/world/WorldObjectState.cpp:110 — FindObjectYobi_4462B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 569 | 0x004462E0 | `sub_4462E0` | 有正文 | `src/world/WorldObjectState.cpp:245 — Sub_4462E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 570 | 0x00446470 | `sub_446470` | 有正文 | `src/world/WorldObjectState.cpp:284 — Sub_446470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 571 | 0x004464D0 | `sub_4464D0` | 有正文 | `src/world/WorldObjectState.cpp:303 — Sub_4464D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 572 | 0x00446610 | `sub_446610` | 有正文 | `src/world/WorldObjectState.cpp:46 — Sub_446610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 573 | 0x00446640 | `sub_446640` | 有正文 | `src/YobiRuntime.cpp:36642 — Sub_446640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 574 | 0x004467D0 | `sub_4467D0` | 有正文 | `src/YobiRuntime.cpp:36601 — Sub_4467D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 575 | 0x004468F0 | `sub_4468F0` | 有正文 | `src/world/WorldObjectState.cpp:57 — Sub_4468F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 576 | 0x00446900 | `sub_446900` | 有正文 | `src/world/WorldObjectState.cpp:63 — Sub_446900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 577 | 0x00446970 | `sub_446970` | 有正文 | `src/YobiRuntime.cpp:927 — Sub_446970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 578 | 0x00446990 | `sub_446990` | 有正文 | `src/YobiRuntime.cpp:936 — Sub_446990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 579 | 0x004469C0 | `sub_4469C0` | 有正文 | `src/YobiRuntime.cpp:3301 — Sub_4469C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 580 | 0x00446B50 | `sub_446B50` | 有正文 | `src/YobiRuntime.cpp:3354 — Sub_446B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 581 | 0x00446C10 | `sub_446C10` | 有正文 | `src/YobiRuntime.cpp:3387 — Sub_446C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 582 | 0x00446D80 | `sub_446D80` | 有正文 | `src/YobiRuntime.cpp:3622 — Sub_446D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 583 | 0x00446FA0 | `sub_446FA0` | 有正文 | `src/YobiRuntime.cpp:3685 — Sub_446FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 584 | 0x00447070 | `sub_447070` | 有正文 | `src/YobiRuntime.cpp:3721 — Sub_447070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 585 | 0x00447230 | `sub_447230` | 有正文 | `src/YobiRuntime.cpp:3748 — Sub_447230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 586 | 0x004473B0 | `sub_4473B0` | 有正文 | `src/YobiRuntime.cpp:3119 — Sub_4473B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 587 | 0x004474E0 | `sub_4474E0` | 有正文 | `src/YobiRuntime.cpp:3172 — Sub_4474E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 588 | 0x00447610 | `sub_447610` | 有正文 | `src/YobiRuntime.cpp:3771 — Sub_447610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 589 | 0x00447820 | `sub_447820` | 有正文 | `src/YobiRuntime.cpp:3803 — Sub_447820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 590 | 0x00447A20 | `sub_447A20` | 有正文 | `src/YobiRuntime.cpp:3831 — Sub_447A20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 591 | 0x00447C40 | `sub_447C40` | 有正文 | `src/YobiRuntime.cpp:3837 — Sub_447C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 592 | 0x00447E60 | `sub_447E60` | 有正文 | `src/YobiRuntime.cpp:3843 — Sub_447E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 593 | 0x00448040 | `sub_448040` | 有正文 | `src/YobiRuntime.cpp:3053 — Sub_448040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 594 | 0x00448240 | `sub_448240` | 有正文 | `src/world/SceneState.cpp:1028 — IsMapFloorAllowed_448240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 595 | 0x00448290 | `sub_448290` | 有正文 | `src/YobiRuntime.cpp:34332 — Sub_448290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 596 | 0x00448370 | `sub_448370` | 有正文 | `src/YobiRuntime.cpp:33712 — Sub_448370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 597 | 0x004483A0 | `sub_4483A0` | 有正文 | `src/YobiRuntime.cpp:34209 — Sub_4483A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 598 | 0x00448810 | `sub_448810` | 有正文 | `src/YobiRuntime.cpp:33920 — Sub_448810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 599 | 0x00448A50 | `sub_448A50` | 有正文 | `src/YobiRuntime.cpp:33814 — Sub_448A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 600 | 0x00448BF0 | `sub_448BF0` | 有正文 | `src/YobiRuntime.cpp:33867 — Sub_448BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 601 | 0x00448D90 | `sub_448D90` | 有正文 | `src/YobiRuntime.cpp:33981 — Sub_448D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 602 | 0x00449160 | `sub_449160` | 有正文 | `src/YobiRuntime.cpp:34433 — Sub_449160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 603 | 0x004493C0 | `sub_4493C0` | 有正文 | `src/world/SceneState.cpp:1043 — Sub_4493C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 604 | 0x00449400 | `sub_449400` | 有正文 | `src/YobiRuntime.cpp:34378 — Sub_449400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 605 | 0x00449570 | `sub_449570` | 有正文 | `src/YobiRuntime.cpp:33728 — Sub_449570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 606 | 0x00449670 | `sub_449670` | 有正文 | `src/YobiRuntime.cpp:34365 — Sub_449670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 607 | 0x004496C0 | `sub_4496C0` | 有正文 | `src/YobiRuntime.cpp:33768 — Sub_4496C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 608 | 0x00449700 | `sub_449700` | 有正文 | `src/YobiRuntime.cpp:34148 — Sub_449700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 609 | 0x00449A00 | `sub_449A00` | 有正文 | `src/YobiRuntime.cpp:34062 — Sub_449A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 610 | 0x00449BC0 | `sub_449BC0` | 有正文 | `src/YobiRuntime.cpp:34129 — Sub_449BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 611 | 0x00449C10 | `sub_449C10` | 有正文 | `src/YobiRuntime.cpp:34115 — Sub_449C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 612 | 0x00449C90 | `sub_449C90` | 有正文 | `src/YobiRuntime.cpp:36527 — Sub_449C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 613 | 0x00449CC0 | `sub_449CC0` | 有正文 | `src/YobiRuntime.cpp:36543 — Sub_449CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 614 | 0x00449CF0 | `sub_449CF0` | 有正文 | `src/Input.cpp:1111 — InitializeContext_449CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 615 | 0x00449DA0 | `sub_449DA0` | 有正文 | `src/Input.cpp:1327 — Sub_449DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 616 | 0x00449FA0 | `sub_449FA0` | 有正文 | `src/Input.cpp:1303 — Sub_449FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 617 | 0x0044A090 | `sub_44A090` | 有正文 | `src/Input.cpp:1278 — Sub_44A090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 618 | 0x0044A160 | `sub_44A160` | 有正文 | `src/Input.cpp:462 — LoadInitialText_44A160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 619 | 0x0044A1B0 | `sub_44A1B0` | 有正文 | `src/Input.cpp:1515 — BrowseChatHistory_44A1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 620 | 0x0044A410 | `sub_44A410` | 有正文 | `src/InputCaret.cpp:124 — Sub_44A410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 621 | 0x0044A540 | `sub_44A540` | 有正文 | `src/Input.cpp:476 — Sub_44A540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 622 | 0x0044A580 | `sub_44A580` | 有正文 | `src/Input.cpp:495 — Sub_44A580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 623 | 0x0044A5D0 | `sub_44A5D0` | 有正文 | `src/Input.cpp:470 — Sub_44A5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 624 | 0x0044A7D0 | `sub_44A7D0` | 有正文 | `src/Input.cpp:514 — Sub_44A7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 625 | 0x0044A990 | `sub_44A990` | 有正文 | `src/Input.cpp:520 — Sub_44A990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 626 | 0x0044AA90 | `sub_44AA90` | 有正文 | `src/Input.cpp:535 — Sub_44AA90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 627 | 0x0044AB60 | `sub_44AB60` | 有正文 | `src/Input.cpp:1578 — Sub_44AB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 628 | 0x0044ADB0 | `Input::AppendChar` | 有正文 | `src/Input.cpp:917 — AppendChar_44ADB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 629 | 0x0044B050 | `World::AppendMsgToCharWindow` | 有正文 | `src/Input.cpp:1430 — Sub_44B050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 630 | 0x0044B170 | `sub_44B170` | 有正文 | `src/ui/WindowRuntime.cpp:30677 — Sub_44B170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 631 | 0x0044B190 | `sub_44B190` | 有正文 | `src/Input.cpp:383 — ClassifyLastAnsiUnit_44B190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 632 | 0x0044B1D0 | `sub_44B1D0` | 有正文 | `src/Input.cpp:450 — ClassifyAnsiBoundary_44B1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 633 | 0x0044B250 | `sub_44B250` | 有承接 | `src/Input.cpp:495 — (static_cast<unsigned char>(context->text[caret]) & 0x80u) != 0u` | 首字节高位决定 1／2 字节步长；实际输入消费者内联；完整业务／运行未验 |
| 634 | 0x0044B260 | `sub_44B260` | 有正文 | `src/render/Font.cpp:306 — MeasureDefaultText_44B260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 635 | 0x0044B2A0 | `TextMeasure` | 有正文 | `src/render/Font.cpp:310 — TextMeasure` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 636 | 0x0044B2E0 | `Font::measure` | 有正文 | `src/render/Font.cpp:227 — Font::measure` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 637 | 0x0044B330 | `Input::SetContext` | 有正文 | `src/Input.cpp:947 — SetContext_44B330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 638 | 0x0044B340 | `sub_44B340` | 有正文 | `src/Input.cpp:923 — ProcessPresetHotkeys_44B340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 639 | 0x0044B440 | `sub_44B440` | 有正文 | `src/Input.cpp:411 — ClassifyLegacyBoundary_44B440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 640 | 0x0044B4A0 | `sub_44B4A0` | 有正文 | `src/Input.cpp:424 — FindLegacyByte_44B4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 641 | 0x0044B570 | `sub_44B570` | 有正文 | `src/Input.cpp:403 — ClassifyChatSplit_44B570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 642 | 0x0044B5A0 | `sub_44B5A0` | 有正文 | `src/platform/MainLoopSupport.cpp:588 — ResetSystemLog_44B5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 643 | 0x0044B5C0 | `sub_44B5C0` | 有正文 | `src/LoginState.cpp:99 — Sub_44B5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 644 | 0x0044B600 | `sub_44B600` | 有承接 | `src/ui/WindowRuntime.cpp:2994 — std::bit_cast<std::int8_t>(platform::gSupportedRenderMode)` | 原 W8 读取；当前 Sub_493CB0 直接读状态；完整业务／运行未验 |
| 645 | 0x0044B610 | `hasIDirect3D7` | 有正文 | `src/render/RendererResources.cpp:146 — HasLegacy3DDevice_44B610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 646 | 0x0044B630 | `Renderer::Init` | 有正文 | `src/render/RendererResources.cpp:210 — RendererInit_44B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 647 | 0x0044C3C0 | `Renderer::Destroy` | 有正文 | `src/render/SdlPresenter.cpp:556 — RendererDestroy_44C3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 648 | 0x0044C560 | `Renderer::Flip` | 有正文 | `src/render/SdlPresenter.cpp:399 — Flip_44C560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 649 | 0x0044C6B0 | `Renderer::TestCooperativeLevel` | 有正文 | `src/render/SdlPresenter.cpp:616 — TestCooperativeLevel_44C6B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 650 | 0x0044C6D0 | `Renderer::MakeSurface` | 有正文 | `src/render/GraphicDxCache.cpp:239 — CreateTextureSurface_44C6D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 651 | 0x0044C8B0 | `Renderer::CreateSurrface` | 有正文 | `src/render/GraphicDxCache.cpp:278 — CreatePlainTextureSurface_44C8B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 652 | 0x0044C9A0 | `Renderer::FillRect` | 有正文 | `src/render/LegacyTextureDrawing.cpp:273 — FillRect_44C9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 653 | 0x0044CC20 | `sub_44CC20` | 有正文 | `src/render/GraphicDxCache.cpp:216 — InitializeSurfaceTransformCache629300_44CC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 654 | 0x0044CC30 | `sub_44CC30` | 有正文 | `src/render/GraphicDxCache.cpp:188 — ConstructSurfaceTransformCache629300_44CC30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 655 | 0x0044CC60 | `sub_44CC60` | 有正文 | `src/render/GraphicDxCache.cpp:202 — DestroySurfaceTransformCache629300_44CC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 656 | 0x0044CD30 | `sub_44CD30` | 有正文 | `src/render/GraphicDxCache.cpp:223 — InitializeSurfaceTransformCache62A630_44CD30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 657 | 0x0044CD40 | `sub_44CD40` | 有正文 | `src/render/GraphicDxCache.cpp:195 — ConstructSurfaceTransformCache62A630_44CD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 658 | 0x0044CD70 | `unknown_libname_9` | 有正文 | `src/render/GraphicDxCache.cpp:208 — DestroySurfaceTransformCache62A630_44CD70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 659 | 0x0044CD80 | `sub_44CD80` | 有正文 | `src/render/GraphicDxCache.cpp:820 — TransformTextureSurface_44CD80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 660 | 0x0044D3F0 | `sub_44D3F0` | 有承接 | `src/render/GraphicDxCache.cpp:150 — struct SurfaceTransformKeyLess` | 实际 operator() 比较器；关联原入口；完整业务／运行未验 |
| 661 | 0x0044D490 | `sub_44D490` | 有正文 | `src/render/GraphicDxCache.cpp:881 — ClearSurfaceTransformCache_44D490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 662 | 0x0044D520 | `Renderer::RenderTextureToSurface` | 有正文 | `src/render/LegacyTextureDrawing.cpp:307 — RenderTextureToSurface_44D520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 663 | 0x0044DA90 | `Renderer::RenderTexture` | 有正文 | `src/render/LegacyTextureDrawing.cpp:403 — RenderTexture_44DA90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 664 | 0x0044E0C0 | `sub_44E0C0` | 有正文 | `src/render/GraphicPaletteCache.cpp:55 — InitializeGraphicPaletteCache_44E0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 665 | 0x0044E110 | `sub_44E110` | 有正文 | `src/render/GraphicPaletteCache.cpp:71 — ReleaseGraphicPaletteCache_44E110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 666 | 0x0044E150 | `TakeIndexCacheFromPool` | 有正文 | `src/render/GraphicPaletteCache.cpp:78 — TakeIndexCacheFromPool_44E150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 667 | 0x0044E230 | `sub_44E230` | 有正文 | `src/render/GraphicPaletteCache.cpp:208 — AgeGraphicPaletteCache_44E230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 668 | 0x0044E290 | `Renderer::LoadDXPalette` | 有正文 | `src/render/GraphicPaletteCache.cpp:117 — LoadGraphicPalette32_44E290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 669 | 0x0044ED70 | `sub_44ED70` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 670 | 0x0044F000 | `sub_44F000` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 671 | 0x0044F330 | `sub_44F330` | 有正文 | `src/render/SurfaceState.cpp:393 — DrawIndexedAutoMap32_44F330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 672 | 0x0044F5C0 | `sub_44F5C0` | 有正文 | `src/render/SurfaceState.cpp:416 — DrawIndexedAutoMap_44F5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 673 | 0x0044F650 | `sub_44F650` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 674 | 0x0044F920 | `sub_44F920` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 675 | 0x0044FC20 | `sub_44FC20` | 有正文 | `src/render/SurfaceState.cpp:406 — DrawColorAutoMap32_44FC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 676 | 0x0044FEB0 | `sub_44FEB0` | 有正文 | `src/render/SurfaceState.cpp:424 — DrawColorAutoMap_44FEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 677 | 0x0044FF40 | `sub_44FF40` | 有承接 | `src/render/GraphicDxCache.cpp:767 — LoadTextureFromBuffer32_450400` | 32 位空 palette 分支由 CPU 载荷承接；低色深分支已排除；完整业务／运行未验 |
| 678 | 0x00450190 | `sub_450190` | 有承接 | `src/render/GraphicDxCache.cpp:767 — LoadTextureFromBuffer32_450400` | 32 位非空 palette 分支由 CPU 载荷承接；低色深分支已排除；完整业务／运行未验 |
| 679 | 0x00450400 | `Texture::LoadFromBuffer` | 有正文 | `src/render/GraphicDxCache.cpp:767 — LoadTextureFromBuffer32_450400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 680 | 0x00450470 | `Renderer::InitPalette` | 有正文 | `src/Palette.cpp:181 — InitPalette32` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 681 | 0x00450A80 | `sub_450A80` | 有正文 | `src/Palette.cpp:276 — ReadPalette` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 682 | 0x00450B70 | `sub_450B70` | 有正文 | `src/Palette.cpp:307 — SelectPalette` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 683 | 0x00450BB0 | `sub_450BB0` | 有正文 | `src/Palette.cpp:322 — UpdatePalette_450BB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 684 | 0x00451190 | `sub_451190` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 685 | 0x00451220 | `sub_451220` | 有正文 | `src/Palette.cpp:498 — RebuildPalette32` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 686 | 0x00451270 | `sub_451270` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 687 | 0x004512C0 | `Renderer::ClearSurface` | 有正文 | `src/render/SurfaceState.cpp:536 — ClearSurface_4512C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 688 | 0x00451310 | `Texture::FillBlack` | 有正文 | `src/render/GraphicDxCache.cpp:730 — FillTextureBlack_451310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 689 | 0x00451350 | `sub_451350` | 有正文 | `src/render/SurfaceState.cpp:547 — Sub_451350` | 32 位实际正文；低色深分支已具名排除；完整调色板／渲染运行未验 |
| 690 | 0x00451D80 | `sub_451D80` | 有正文 | `src/GraphicRead.cpp:613 — GetGraphicAutoMapColor_451D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 691 | 0x00451E90 | `sub_451E90` | 有正文 | `src/Palette.cpp:18 — FindNearestPaletteColor_451E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 692 | 0x00451FA0 | `sub_451FA0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 693 | 0x004522F0 | `sub_4522F0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 694 | 0x00452840 | `sub_452840` | 有正文 | `src/render/SurfaceState.cpp:214 — DrawMapEffects32_452840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 695 | 0x00452CF0 | `sub_452CF0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 696 | 0x00453580 | `sub_453580` | 有正文 | `src/render/SurfaceState.cpp:387 — DrawMapEffects_453580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 697 | 0x004535B0 | `sub_4535B0` | 有正文 | `src/render/SurfaceState.cpp:693 — BlitSurfaceToBack_4535B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 698 | 0x004536D0 | `Renderer::IsLost` | 有正文 | `src/render/SdlPresenter.cpp:575 — IsLost_4536D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 699 | 0x00453760 | `takeScreenshot` | 有正文 | `src/render/SdlPresenter.cpp:457 — TakeScreenshot_453760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 700 | 0x00453DE0 | `sub_453DE0` | 有正文 | `src/Palette.cpp:168 — CountMaskBits_453DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 701 | 0x00453E00 | `sub_453E00` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 702 | 0x00453E10 | `unknown_libname_10` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 703 | 0x00453E30 | `unknown_libname_11` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 704 | 0x00453E40 | `sub_453E40` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 705 | 0x00453E50 | `unknown_libname_12` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 706 | 0x00453E70 | `unknown_libname_13` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 707 | 0x00453E80 | `sub_453E80` | 有正文 | `src/render/SdlPresenter.cpp:409 — SaveScreenshotBitmap_453E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 708 | 0x00453FB0 | `sub_453FB0` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 709 | 0x00454050 | `sub_454050` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 710 | 0x00454420 | `sub_454420` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 711 | 0x004544F0 | `sub_4544F0` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 712 | 0x00454730 | `sub_454730` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 713 | 0x00454780 | `sub_454780` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 714 | 0x004547C0 | `sub_4547C0` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 715 | 0x00454820 | `sub_454820` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 716 | 0x00454880 | `sub_454880` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 717 | 0x004548A0 | `sub_4548A0` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 718 | 0x00454B10 | `sub_454B10` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 719 | 0x00454B30 | `sub_454B30` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 720 | 0x00454B90 | `sub_454B90` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 721 | 0x00454BE0 | `sub_454BE0` | 有承接 | `src/render/GraphicDxCache.cpp:165 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 722 | 0x00454C00 | `sub_454C00` | 有正文 | `src/platform/LegacyMouse.cpp:152 — InitializeLegacyEmptyExit_454C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 723 | 0x00454C30 | `SoundSystem::Init` | 有正文 | `src/LegacyAudio.cpp:1588 — InitializeMusic_454C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 724 | 0x00454EE0 | `sub_454EE0` | 有正文 | `src/LegacyAudio.cpp:1575 — Sub_454EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 725 | 0x00454FE0 | `sub_454FE0` | 有承接 | `src/LegacyAudio.cpp:1599 — InitializeFixedMusic；src/platform/SdlAudioBackend.cpp:630 — InitializeFixedMusic` | 原单patch GetInstrument／DownloadInstrument／临时Release；固定WAV实际资源解码／PCM流预载承接，原音色下载机制退休；保原身份，不恢复MIDI／DLS或宣称库内HRESULT等价，声音／寿命未验 |
| 726 | 0x00455030 | `sub_455030` | 有正文 | `src/LegacyAudio.cpp:1486 — SetFixedMusicReverb_455030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 727 | 0x00455100 | `sub_455100` | 有承接 | `src/LegacyAudio.cpp:1599 — InitializeFixedMusic；src/platform/SdlAudioBackend.cpp:630 — InitializeFixedMusic` | 原Collection装载／枚举／逐patch下载，唯一初始化父传Source=0且不消费返回码；当前固定WAV实际预载承接，客户端不读音色库；保原身份，WAV质量及资源／设备／寿命未验 |
| 728 | 0x00455260 | `sub_455260` | 有承接 | `src/LegacyAudio.cpp:1578 — ShutdownFixedMusic；src/platform/SdlAudioBackend.cpp:738 — ShutdownFixedMusic` | 原Loader非零Release并清槽，唯一音乐关闭父；当前停止并释放固定SDL流／混响／PCM样本承接，旧COM loader机制退休；保原身份，不新增影子owner，正常寿命／运行未验 |
| 729 | 0x00455280 | `World::PlayFixedBGM` | 有正文 | `src/LegacyAudio.cpp:1528 — PlayFixedMusic_455280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 730 | 0x004553B0 | `World::PlaySegment` | 有正文 | `src/LegacyAudio.cpp:1518 — PlayFixedMusicSegment_4553B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 731 | 0x00455400 | `sub_455400` | 有正文 | `src/LegacyAudio.cpp:1492 — ApplyFixedMusicVolume_455400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 732 | 0x00455450 | `World::StopBGM` | 有正文 | `src/LegacyAudio.cpp:1502 — StopFixedMusic_455450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 733 | 0x00455470 | `sub_455470` | 有正文 | `src/LegacyAudio.cpp:1510 — SetFixedMusicRepeats_455470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 734 | 0x00455490 | `sub_455490` | 有正文 | `src/LegacyUiRecord.cpp:2247 — Sub_455490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 735 | 0x004554A0 | `sub_4554A0` | 有正文 | `src/LegacyUiRecord.cpp:2253 — Sub_4554A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 736 | 0x004554C0 | `sub_4554C0` | 有正文 | `src/LegacyUiRecord.cpp:2262 — Sub_4554C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 737 | 0x004554D0 | `sub_4554D0` | 有正文 | `src/LegacyUiRecord.cpp:2268 — Sub_4554D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 738 | 0x00455540 | `sub_455540` | 有正文 | `src/LegacyUiRecord.cpp:2274 — Sub_455540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 739 | 0x00455550 | `sub_455550` | 有正文 | `src/LegacyUiRecord.cpp:2280 — Sub_455550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 740 | 0x004555C0 | `sub_4555C0` | 有正文 | `src/LegacyUiRecord.cpp:2286 — Sub_4555C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 741 | 0x004555D0 | `sub_4555D0` | 有正文 | `src/LegacyUiRecord.cpp:2292 — Sub_4555D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 742 | 0x00455640 | `sub_455640` | 有正文 | `src/LegacyUiRecord.cpp:2298 — Sub_455640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 743 | 0x00455650 | `sub_455650` | 有正文 | `src/LegacyUiRecord.cpp:2304 — Sub_455650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 744 | 0x004556C0 | `sub_4556C0` | 有正文 | `src/LegacyUiRecord.cpp:2310 — Sub_4556C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 745 | 0x004556D0 | `sub_4556D0` | 有正文 | `src/LegacyUiRecord.cpp:2316 — Sub_4556D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 746 | 0x00455740 | `sub_455740` | 有正文 | `src/LegacyUiRecord.cpp:2322 — Sub_455740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 747 | 0x00455750 | `sub_455750` | 有正文 | `src/LegacyUiRecord.cpp:2328 — Sub_455750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 748 | 0x004557C0 | `sub_4557C0` | 有正文 | `src/LegacyUiRecord.cpp:2334 — Sub_4557C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 749 | 0x004557D0 | `sub_4557D0` | 有正文 | `src/LegacyUiRecord.cpp:2340 — Sub_4557D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 750 | 0x00455840 | `sub_455840` | 有正文 | `src/LegacyUiRecord.cpp:2360 — Sub_455840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 751 | 0x00455850 | `sub_455850` | 有正文 | `src/LegacyUiRecord.cpp:2366 — Sub_455850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 752 | 0x00455930 | `sub_455930` | 有正文 | `src/LegacyUiRecord.cpp:2405 — Sub_455930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 753 | 0x00455940 | `sub_455940` | 有正文 | `src/LegacyUiRecord.cpp:2411 — Sub_455940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 754 | 0x00455A90 | `canWatchBattle` | 有正文 | `src/world/SceneState.cpp:9105 — Sub_455A90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 755 | 0x00455AC0 | `___setargv` | 有正文 | `src/world/SceneState.cpp:10383 — Sub_455AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 756 | 0x00455AD0 | `___setargv_0` | 有正文 | `src/world/SceneState.cpp:10389 — Sub_455AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 757 | 0x00455AE0 | `___setargv_1` | 有正文 | `src/world/SceneState.cpp:10395 — Sub_455AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 758 | 0x00455AF0 | `___setargv_2` | 有正文 | `src/world/SceneState.cpp:10401 — Sub_455AF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 759 | 0x00455B00 | `___setargv_3` | 有正文 | `src/world/SceneState.cpp:10407 — Sub_455B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 760 | 0x00455B10 | `sub_455B10` | 有正文 | `src/world/SceneState.cpp:10413 — Sub_455B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 761 | 0x00455B20 | `sub_455B20` | 有正文 | `src/world/SceneState.cpp:10419 — Sub_455B20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 762 | 0x00455B40 | `___setargv_6` | 有正文 | `src/world/SceneState.cpp:10427 — Sub_455B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 763 | 0x00455B50 | `sub_455B50` | 有正文 | `src/world/SceneState.cpp:10433 — Sub_455B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 764 | 0x00455B70 | `sub_455B70` | 有正文 | `src/world/SceneState.cpp:10441 — Sub_455B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 765 | 0x00455B90 | `sub_455B90` | 有正文 | `src/world/SceneState.cpp:10449 — Sub_455B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 766 | 0x00455BB0 | `sub_455BB0` | 有正文 | `src/world/SceneState.cpp:10457 — Sub_455BB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 767 | 0x00455BD0 | `sub_455BD0` | 有正文 | `src/world/SceneState.cpp:10465 — Sub_455BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 768 | 0x00455C60 | `sub_455C60` | 有正文 | `src/world/SceneState.cpp:5924 — Sub_455C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 769 | 0x00455C70 | `sub_455C70` | 有正文 | `src/world/SceneState.cpp:1132 — NextRotationValue_455C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 770 | 0x00455C90 | `sub_455C90` | 有正文 | `src/world/WorldUpdate.cpp:228 — Sub_455C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 771 | 0x00455FC0 | `sub_455FC0` | 有正文 | `src/world/SceneState.cpp:5930 — Sub_455FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 772 | 0x00456050 | `sub_456050` | 有正文 | `src/ResourcePaths.cpp:187 — BuildResourcePaths` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 773 | 0x00456190 | `sub_456190` | 有正文 | `src/render/Font.cpp:95 — InitializeFont_456190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 774 | 0x00456530 | `sub_456530` | 有正文 | `src/render/Font.cpp:159 — ReleaseFont_456530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 775 | 0x00456560 | `Renderer::DrawText3` | 有正文 | `src/render/TextPendingQueue.cpp:81 — DrawText3_456560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 776 | 0x00456670 | `Renderer::DrawText2` | 有正文 | `src/render/TextPendingQueue.cpp:104 — DrawText2_456670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 777 | 0x00456780 | `Renderer::DrawText` | 有正文 | `src/render/TextPendingQueue.cpp:172 — DrawText_456780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 778 | 0x004568A0 | `Renderer::DrawInputText` | 有正文 | `src/render/InputText.cpp:56 — DrawInputText_4568A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 779 | 0x00456D30 | `Renderer::CacheReset` | 有正文 | `src/render/GraphicCacheState.cpp:17 — ResetGraphicCache_456D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 780 | 0x00456D50 | `sub_456D50` | 有正文 | `src/render/GraphicCacheState.cpp:10 — ClearTextQueueTail_456D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 781 | 0x00456D70 | `Renderer::RenderTextToSurface` | 有正文 | `src/render/Font.cpp:169 — RenderTextToSurface_456D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 782 | 0x00456FD0 | `sub_456FD0` | 有承接 | `src/world/SceneState.cpp:677 — SceneTransitionValueStartup::SceneTransitionValueStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 783 | 0x00456FE0 | `unknown_libname_15` | 有正文 | `src/world/SceneState.cpp:689 — ConstructSceneTransitionValue` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 784 | 0x00457000 | `unknown_libname_16` | 有正文 | `src/world/SceneState.cpp:695 — DestroySceneTransitionValue` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 785 | 0x00457010 | `sub_457010` | 有正文 | `src/platform/MainLoopSupport.cpp:619 — RequestRender_457010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 786 | 0x00457020 | `MainLoop` | 有正文 | `src/platform/MainLoop.cpp:47 — MainLoop` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 787 | 0x004574E0 | `GraphicAlloc` | 有正文 | `src/GraphicAlloc.cpp:20 — GraphicAlloc_4574E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 788 | 0x00457670 | `sub_457670` | 有正文 | `src/GraphicAlloc.cpp:97 — Sub_457670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 789 | 0x004576B0 | `sub_4576B0` | 有正文 | `src/platform/ClientInitialization.cpp:186 — InitializeClient_4576B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 790 | 0x00457A40 | `sub_457A40` | 有正文 | `src/platform/ClientShutdown.cpp:42 — ShutdownClient_457A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 791 | 0x00457AB0 | `KeepAlive` | 有正文 | `src/platform/MainLoopSupport.cpp:663 — KeepAlive_457AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 792 | 0x00457B10 | `InitLoading` | 有正文 | `src/platform/LoadingScreen.cpp:31 — InitLoading_457B10` | 实际非占位正文已定位；2026-10-13 按用户要求初始阶段改为 21，自动走既有跳过清理；完整业务／接线／运行未验 |
| 793 | 0x00458390 | `sub_458390` | 有正文 | `src/platform/ClientInitialization.cpp:108 — FadeInLoading_458390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 794 | 0x004585F0 | `sub_4585F0` | 有正文 | `src/platform/ClientInitialization.cpp:154 — FadeOutLoading_4585F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 795 | 0x004587D0 | `sub_4587D0` | 有正文 | `src/platform/MainLoopWindowEnumeration.cpp:84 — EnumerateWindows_4587D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 796 | 0x00458840 | `sub_458840` | 有正文 | `src/platform/MainLoopWindowEnumeration.cpp:27 — EnumerateWindowsCallback_458840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 797 | 0x004589A0 | `sub_4589A0` | 有承接 | `src/network/NetworkRuntime.cpp:177 — platform::gWindowReportValue > 0` | 活动QueuePlaintextProtocolBytes内联承接；轮询／容量门禁通过后按首标记优先、报告值S32>0／登录状态1／截止W32按S32<0采样并清零；原命令字段LF保持，完整业务／运行未验 |
| 798 | 0x004589B0 | `sub_4589B0` | 有承接 | `src/network/NetworkRuntime.cpp:180 — platform::gWindowReportValue = 0` | 活动QueuePlaintextProtocolBytes内联承接；轮询／容量门禁通过后按首标记优先、报告值S32>0／登录状态1／截止W32按S32<0采样并清零；原命令字段LF保持，完整业务／运行未验 |
| 799 | 0x004589C0 | `sub_4589C0` | 有承接 | `src/network/NetworkRuntime.cpp:179 — std::bit_cast<std::int32_t>(platform::gWindowReportDeadline) < 0` | 活动QueuePlaintextProtocolBytes内联承接；轮询／容量门禁通过后按首标记优先、报告值S32>0／登录状态1／截止W32按S32<0采样并清零；原命令字段LF保持，完整业务／运行未验 |
| 800 | 0x004589D0 | `sub_4589D0` | 有正文 | `src/platform/MainLoopSupport.cpp:808 — ReadSystemSeconds_4589D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 801 | 0x00458A10 | `sub_458A10` | 有正文 | `src/platform/MainLoopSupport.cpp:821 — CheckSystemSeconds_458A10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 802 | 0x00458A40 | `sub_458A40` | 有正文 | `src/platform/MainLoop.cpp:22 — ShowModeChange_458A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 803 | 0x00458AE0 | `sub_458AE0` | 有正文 | `src/LegacyAudio.cpp:1559 — Sub_458AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 804 | 0x00458DC0 | `sub_458DC0` | 有正文 | `src/platform/MainLoopSupport.cpp:628 — CheckLegacyDxCapability_458DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 805 | 0x00458DE0 | `World::UpdateNRTime` | 有正文 | `src/world/GameTime.cpp:16 — UpdateGameTime` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 806 | 0x00458EC0 | `Time::GetCate` | 有正文 | `src/world/GameTime.cpp:43 — GetCate_458EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 807 | 0x00458F00 | `sub_458F00` | 有正文 | `src/world/SceneState.cpp:7020 — Sub_458F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 808 | 0x00458F40 | `sub_458F40` | 有正文 | `src/world/SceneState.cpp:6993 — Sub_458F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 809 | 0x00458FD0 | `sub_458FD0` | 有正文 | `src/platform/ImeLifecycle.cpp:18 — InitializeIme_458FD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 810 | 0x00459040 | `sub_459040` | 有正文 | `src/platform/ImeLifecycle.cpp:46 — ShutdownIme_459040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 811 | 0x00459090 | `sub_459090` | 有正文 | `src/platform/ImeLifecycle.cpp:62 — IsImeKeyboardLayout_459090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 812 | 0x004590A0 | `sub_4590A0` | 有正文 | `src/platform/ImeLifecycle.cpp:71 — ResetImeOpenStatus_4590A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 813 | 0x004590D0 | `sub_4590D0` | 有正文 | `src/platform/ImeLifecycle.cpp:92 — BorrowImeDescription_4590D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 814 | 0x004590E0 | `World::RenderInput` | 有正文 | `src/world/RenderInput.cpp:173 — RenderInput_4590E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 815 | 0x00459580 | `sub_459580` | 有正文 | `src/platform/ImeComposition.cpp:42 — Sub_459580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 816 | 0x004596D0 | `sub_4596D0` | 有正文 | `src/InputKeyDispatch.cpp:14 — ProcessKeyInput_4596D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 817 | 0x00459760 | `sub_459760` | 有正文 | `src/platform/WindowProcedure.cpp:43 — ClearKeyFlags_459760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 818 | 0x00459780 | `resetKeyState` | 有正文 | `src/platform/WindowProcedure.cpp:52 — ResetKeyState_459780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 819 | 0x004597A0 | `sub_4597A0` | 有正文 | `src/platform/WindowProcedure.cpp:67 — ReadKeyFlag4_4597A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 820 | 0x004597B0 | `sub_4597B0` | 有正文 | `src/platform/WindowProcedure.cpp:61 — ReadKeyPressFlag_4597B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 821 | 0x004597C0 | `sub_4597C0` | 有正文 | `src/platform/WindowProcedure.cpp:73 — ReadControlKeyPressFlag_4597C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 822 | 0x004597E0 | `sub_4597E0` | 有正文 | `src/GraphicInfo.cpp:83 — InitializeGraphicMapNumbers_4597E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 823 | 0x004597F0 | `sub_4597F0` | 有正文 | `src/GraphicInfo.cpp:55 — ConstructGraphicMapNumbers_4597F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 824 | 0x00459820 | `sub_459820` | 有正文 | `src/GraphicInfo.cpp:62 — DestroyGraphicMapNumbers_459820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 825 | 0x00459850 | `sub_459850` | 有正文 | `src/GraphicInfo.cpp:90 — InitializeGraphicFirstRecords_459850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 826 | 0x00459860 | `sub_459860` | 有正文 | `src/GraphicInfo.cpp:68 — ConstructGraphicFirstRecords_459860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 827 | 0x00459890 | `sub_459890` | 有正文 | `src/GraphicInfo.cpp:75 — DestroyGraphicFirstRecords_459890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 828 | 0x00459960 | `GraphicInit` | 有正文 | `src/GraphicInfo.cpp:97 — GraphicInit_459960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 829 | 0x0045A130 | `sub_45A130` | 有正文 | `src/ResourcePaths.cpp:212 — LoadAutoMapColor_45A130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 830 | 0x0045A190 | `sub_45A190` | 有正文 | `src/ResourcePaths.cpp:112 — Sub_45A190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 831 | 0x0045A290 | `sub_45A290` | 有正文 | `src/ResourcePaths.cpp:89 — Sub_45A290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 832 | 0x0045A420 | `sub_45A420` | 有正文 | `src/ResourcePaths.cpp:56 — Sub_45A420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 833 | 0x0045A6C0 | `sub_45A6C0` | 有正文 | `src/GraphicRead.cpp:664 — CloseGraphicFiles` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 834 | 0x0045A710 | `Graphic::getXY` | 有正文 | `src/GraphicInfo.cpp:247 — GetGraphicXY` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 835 | 0x0045A760 | `Graphic::getWH` | 有正文 | `src/GraphicInfo.cpp:262 — GetGraphicWH` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 836 | 0x0045A7B0 | `Graphic::getSxSy` | 有正文 | `src/GraphicInfo.cpp:277 — GetGraphicSxSy` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 837 | 0x0045A810 | `Graphic::getFlag1` | 有正文 | `src/GraphicInfo.cpp:292 — GetGraphicFlag1` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 838 | 0x0045A860 | `Graphic::getFlag2` | 有正文 | `src/GraphicInfo.cpp:305 — GetGraphicFlag2` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 839 | 0x0045A8B0 | `Graphic::getMapNoIndex` | 有正文 | `src/GraphicInfo.cpp:318 — GetGraphicMapNoIndex` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 840 | 0x0045A8D0 | `Graphic::getMapNo` | 有正文 | `src/GraphicInfo.cpp:360 — GetGraphicMapNo` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 841 | 0x0045A900 | `Graphic::readGraphicDataByIndex` | 有正文 | `src/GraphicRead.cpp:572 — ReadGraphicDataByIndex` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 842 | 0x0045A9E0 | `sub_45A9E0` | 有承接 | `src/GraphicInfo.cpp:35 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 843 | 0x0045AA80 | `sub_45AA80` | 有承接 | `src/GraphicInfo.cpp:35 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 844 | 0x0045AB50 | `sub_45AB50` | 有承接 | `src/GraphicInfo.cpp:35 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 845 | 0x0045AF20 | `sub_45AF20` | 有承接 | `src/GraphicInfo.cpp:35 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 846 | 0x0045AF60 | `sub_45AF60` | 有承接 | `src/GraphicInfo.cpp:35 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 847 | 0x0045B1D0 | `sub_45B1D0` | 有承接 | `src/GraphicInfo.cpp:35 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 848 | 0x0045B1F0 | `sub_45B1F0` | 有承接 | `src/GraphicInfo.cpp:35 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 849 | 0x0045B250 | `sub_45B250` | 有承接 | `src/GraphicInfo.cpp:35 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 850 | 0x0045B2A0 | `sub_45B2A0` | 有承接 | `src/GraphicInfo.cpp:35 — using GraphicFirstRecords` | GraphicFirstRecords 的实际 std::map 构析、clear、emplace 与键／迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 851 | 0x0045B370 | `sub_45B370` | 有正文 | `src/AnimationRuntime.cpp:74 — Sub_45B370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 852 | 0x0045B380 | `sub_45B380` | 有正文 | `src/AnimationRuntime.cpp:80 — Sub_45B380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 853 | 0x0045B390 | `Animate::BinarySearch` | 有正文 | `src/AnimationRuntime.cpp:205 — BinarySearch_45B390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 854 | 0x0045B400 | `Animate::getIndex` | 有正文 | `src/AnimationRuntime.cpp:236 — GetIndex_45B400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 855 | 0x0045B420 | `sub_45B420` | 有正文 | `src/AnimationRuntime.cpp:193 — Compare_45B420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 856 | 0x0045B440 | `sub_45B440` | 有正文 | `src/AnimationCleanup.cpp:12 — Sub_45B440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 857 | 0x0045B530 | `sub_45B530` | 有正文 | `src/AnimationList.cpp:44 — Sub_45B530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 858 | 0x0045B590 | `sub_45B590` | 有正文 | `src/AnimationList.cpp:6 — Sub_45B590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 859 | 0x0045B5B0 | `sub_45B5B0` | 有正文 | `src/AnimationList.cpp:17 — Sub_45B5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 860 | 0x0045B5D0 | `sub_45B5D0` | 有正文 | `src/AnimationList.cpp:28 — Sub_45B5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 861 | 0x0045B630 | `sub_45B630` | 有正文 | `src/AnimationCleanup.cpp:127 — Sub_45B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 862 | 0x0045B650 | `sub_45B650` | 有正文 | `src/AnimationCleanup.cpp:69 — Sub_45B650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 863 | 0x0045B740 | `sub_45B740` | 有正文 | `src/AnimationCleanup.cpp:137 — Sub_45B740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 864 | 0x0045B760 | `AnimeLoadAction` | 有正文 | `src/AnimationRuntime.cpp:244 — AnimeLoadAction_45B760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 865 | 0x0045BB20 | `AnimeInit` | 有正文 | `src/AnimationRuntime.cpp:88 — AnimeInit_45BB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 866 | 0x0045BEF0 | `AnimeUnload` | 有正文 | `src/AnimationCleanup.cpp:147 — AnimeUnload_45BEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 867 | 0x0045BF20 | `sub_45BF20` | 有正文 | `src/AnimationCleanup.cpp:160 — Sub_45BF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 868 | 0x0045BF60 | `CoordLoad` | 有正文 | `src/AnimationRuntime.cpp:405 — CoordLoad_45BF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 869 | 0x0045C040 | `CoordLoadInit` | 有正文 | `src/AnimationRuntime.cpp:385 — CoordLoadInit_45C040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 870 | 0x0045C150 | `CoordInit` | 有正文 | `src/AnimationRuntime.cpp:347 — CoordInit_45C150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 871 | 0x0045C2D0 | `sub_45C2D0` | 有正文 | `src/LoginOuterScene.cpp:36 — ProcessOuterScene_45C2D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 872 | 0x0045C5A0 | `sub_45C5A0` | 有正文 | `src/LoginModeScene.cpp:32 — ProcessModeScene_45C5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 873 | 0x0045C7C0 | `sub_45C7C0` | 有正文 | `src/LoginInput.cpp:354 — InitializeInputContexts_45C7C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 874 | 0x0045C970 | `sub_45C970` | 有正文 | `src/GraphicInfo.cpp:325 — ProbePaletteCapability_45C970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 875 | 0x0045CA20 | `sub_45CA20` | 有正文 | `src/LoginInputController.cpp:261 — ProcessLoginInput_45CA20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 876 | 0x0045CE00 | `sub_45CE00` | 有正文 | `src/LoginMessage.cpp:72 — ResetLoginMessage` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 877 | 0x0045CE10 | `sub_45CE10` | 有正文 | `src/LoginMessage.cpp:78 — UpdateLoginMessage` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 878 | 0x0045D720 | `sub_45D720` | 有正文 | `src/Input.cpp:353 — Sub_45D720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 879 | 0x0045D760 | `sub_45D760` | 有正文 | `src/Input.cpp:369 — Sub_45D760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 880 | 0x0045D7A0 | `sub_45D7A0` | 有正文 | `src/render/TextPendingQueue.cpp:27 — FindClickedTextIndex_45D7A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 881 | 0x0045D7D0 | `sub_45D7D0` | 有正文 | `src/CharacterAuthenticationFailure.cpp:26 — ResetAuthenticationFailure_45D7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 882 | 0x0045D7E0 | `sub_45D7E0` | 有正文 | `src/CharacterAuthenticationFailure.cpp:32 — UpdateAuthenticationFailure_45D7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 883 | 0x0045D980 | `sub_45D980` | 有正文 | `src/LoginState.cpp:298 — ResetValues_45D980` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 884 | 0x0045D990 | `sub_45D990` | 有正文 | `src/LoginState.cpp:306 — UpdateServerConnection` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 885 | 0x0045DBF0 | `sub_45DBF0` | 有正文 | `src/CharacterDeletionProgress.cpp:30 — ResetCharacterDeletionProgress_45DBF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 886 | 0x0045DC00 | `sub_45DC00` | 有正文 | `src/CharacterDeletionProgress.cpp:36 — UpdateCharacterDeletionProgress_45DC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 887 | 0x0045DD50 | `sub_45DD50` | 有正文 | `src/CharacterListProgress.cpp:36 — ResetCharacterListProgress_45DD50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 888 | 0x0045DD60 | `sub_45DD60` | 有正文 | `src/CharacterListProgress.cpp:42 — UpdateCharacterListProgress_45DD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 889 | 0x0045DEE0 | `sub_45DEE0` | 有正文 | `src/CharacterDeletionConfirmation.cpp:29 — ProcessConfirmationDialog_45DEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 890 | 0x0045E090 | `sub_45E090` | 有正文 | `src/CharacterDeletionConfirmation.cpp:132 — ProcessDefaultConfirmationDialog_45E090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 891 | 0x0045E0C0 | `sub_45E0C0` | 有正文 | `src/CharacterDeletionConfirmation.cpp:142 — ProcessCharacterDeletionWarning_45E0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 892 | 0x0045E3D0 | `sub_45E3D0` | 有正文 | `src/CharacterVersionUpgrade.cpp:49 — ProcessLegacyVersionScene_45E3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 893 | 0x0045E6E0 | `sub_45E6E0` | 有正文 | `src/YobiRuntime.cpp:35176 — Sub_45E6E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 894 | 0x0045E770 | `sub_45E770` | 有正文 | `src/Input.cpp:622 — Sub_45E770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 895 | 0x0045E7B0 | `sub_45E7B0` | 有正文 | `src/CharacterSelectScene.cpp:211 — Sub_45E7B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 896 | 0x0045E7D0 | `sub_45E7D0` | 有正文 | `src/CharacterSelectScene.cpp:220 — Sub_45E7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 897 | 0x0045E960 | `sub_45E960` | 有正文 | `src/LoginState.cpp:407 — ProcessState_45E960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 898 | 0x0045EA50 | `sub_45EA50` | 有正文 | `src/LoginState.cpp:447 — ResetLoginProgress` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 899 | 0x0045EA60 | `sub_45EA60` | 有正文 | `src/LoginState.cpp:453 — UpdateLoginProgress` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 900 | 0x0045EBB0 | `sub_45EBB0` | 有正文 | `src/world/SceneState.cpp:11838 — Sub_45EBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 901 | 0x0045ED80 | `sub_45ED80` | 有正文 | `src/world/SceneState.cpp:11916 — Sub_45ED80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 902 | 0x0045ED90 | `sub_45ED90` | 有正文 | `src/world/SceneState.cpp:11922 — Sub_45ED90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 903 | 0x0045EEE0 | `sub_45EEE0` | 有正文 | `src/world/SceneState.cpp:10532 — Sub_45EEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 904 | 0x0045EF80 | `sub_45EF80` | 有正文 | `src/LoginCapability3Scene.cpp:70 — Sub_45EF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 905 | 0x0045EFD0 | `sub_45EFD0` | 有正文 | `src/LegacyUiRecord.cpp:19021 — Sub_45EFD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 906 | 0x0045F090 | `sub_45F090` | 有正文 | `src/DrawGraphic.cpp:129 — Sub_45F090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 907 | 0x0045F100 | `sub_45F100` | 有正文 | `src/LegacyUiRecord.cpp:19015 — Sub_45F100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 908 | 0x0045F190 | `sub_45F190` | 有正文 | `src/LoginCapability3Scene.cpp:201 — Sub_45F190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 909 | 0x0045F970 | `sub_45F970` | 有正文 | `src/LoginInput.cpp:451 — InitializeCapabilityInput_45F970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 910 | 0x0045FA70 | `sub_45FA70` | 有正文 | `src/LoginCapability3InputController.cpp:79 — ProcessCapability3Input_45FA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 911 | 0x0045FEA0 | `LoginScene::Process` | 有正文 | `src/LoginScene.cpp:451 — ProcessScene_45FEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 912 | 0x00460D00 | `sub_460D00` | 有正文 | `src/CharacterSelectPreview.cpp:479 — ProcessCharacterPreview_460D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 913 | 0x00460FF0 | `sub_460FF0` | 有正文 | `src/CharacterSelectScene.cpp:1470 — ProcessScene_460FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 914 | 0x00461B30 | `sub_461B30` | 有正文 | `src/CharacterSelectSlot.cpp:87 — ProcessCharacterSlot_461B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 915 | 0x00462090 | `sub_462090` | 有正文 | `src/CharacterSelectModal.cpp:22 — ProcessCharacterModal_462090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 916 | 0x00462330 | `sub_462330` | 有正文 | `src/CharacterVersionUpgrade.cpp:154 — ProcessScene_462330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 917 | 0x004626A0 | `sub_4626A0` | 有正文 | `src/CharacterSelectScene.cpp:275 — ProcessCreationScene_4626A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 918 | 0x00462870 | `sub_462870` | 有正文 | `src/CharacterSelectScene.cpp:395 — Sub_462870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 919 | 0x00462880 | `sub_462880` | 有正文 | `src/CharacterSelectScene.cpp:402 — Sub_462880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 920 | 0x00462930 | `sub_462930` | 有正文 | `src/CharacterSelectScene.cpp:435 — Sub_462930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 921 | 0x00463100 | `sub_463100` | 有正文 | `src/CharacterSelectScene.cpp:670 — Sub_463100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 922 | 0x004631D0 | `sub_4631D0` | 有正文 | `src/CharacterSelectScene.cpp:705 — Sub_4631D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 923 | 0x00464290 | `sub_464290` | 有正文 | `src/CharacterSelectScene.cpp:1202 — Sub_464290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 924 | 0x004642C0 | `sub_4642C0` | 有正文 | `src/CharacterSelectScene.cpp:1130 — Sub_4642C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 925 | 0x00464380 | `sub_464380` | 有正文 | `src/CharacterSelectScene.cpp:1165 — Sub_464380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 926 | 0x00464440 | `sub_464440` | 有正文 | `src/LoginCapability4Modal.cpp:76 — ResetCapability4Modal_464440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 927 | 0x00464450 | `sub_464450` | 有正文 | `src/LoginCapability4Modal.cpp:82 — UpdateCapability4Modal_464450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 928 | 0x00464840 | `sub_464840` | 有正文 | `src/LoginCapability4Scene.cpp:65 — Sub_464840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 929 | 0x00464E30 | `sub_464E30` | 有正文 | `src/LoginInputController.cpp:99 — ProcessCapability4Input_464E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 930 | 0x00465320 | `sub_465320` | 有正文 | `src/LoginInput.cpp:505 — InitializeCapability4Input_465320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 931 | 0x00465430 | `sub_465430` | 有正文 | `src/LoginInput.cpp:579 — ProcessCapability4SavedSlotSelector_465430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 932 | 0x00465740 | `sub_465740` | 有正文 | `src/LoginInput.cpp:566 — SaveAndSwitchCapability4InputSlot_465740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 933 | 0x00465790 | `_WinMain@16` | 有正文 | `src/platform/ClientEntry.cpp:263 — WinMain` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 934 | 0x004659F0 | `sub_4659F0` | 有正文 | `src/platform/StartupCommon.cpp:14 — ParseClientVersion` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 935 | 0x00465A20 | `sub_465A20` | 有正文 | `src/platform/StartupCommon.cpp:20 — CaptureDesktopColor` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 936 | 0x00465A30 | `sub_465A30` | 有正文 | `src/platform/WindowLifecycle.cpp:12 — SetWindowedDesktopColor` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 937 | 0x00465A80 | `ReadSetupIni` | 有正文 | `src/platform/SetupIni.cpp:16 — ReadSetupIni` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 938 | 0x00465F00 | `ParseCmdOpt` | 有正文 | `src/platform/CommandOptions.cpp:60 — ParseCommandOptions` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 939 | 0x004669F0 | `WndInit` | 有正文 | `src/platform/WindowLifecycle.cpp:20 — InitializeWindow` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 940 | 0x00466B80 | `Renderer::Rebuild` | 有正文 | `src/platform/RendererRebuild.cpp:20 — RebuildRenderer_466B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 941 | 0x00466D50 | `sub_466D50` | 有正文 | `src/platform/WindowLifecycle.cpp:77 — PumpWindowMessages` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 942 | 0x00466DC0 | `WIN_PROC` | 有正文 | `src/platform/WindowProcedure.cpp:139 — WindowProcedure` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 943 | 0x00467B90 | `sub_467B90` | 有承接 | `src/world/SceneState.cpp:867 — PlayerCoordinatesStartup::PlayerCoordinatesStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 944 | 0x00467BA0 | `unknown_libname_17` | 有正文 | `src/world/SceneState.cpp:703 — ConstructLegacyPlayerCoordinates_467BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 945 | 0x00467BD0 | `sub_467BD0` | 有正文 | `src/world/SceneState.cpp:710 — DestroyLegacyPlayerCoordinates_467BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 946 | 0x00467BF0 | `sub_467BF0` | 有承接 | `src/world/SceneState.cpp:879 — PlayerSnapshotXStartup::PlayerSnapshotXStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 947 | 0x00467C00 | `unknown_libname_19` | 有正文 | `src/world/SceneState.cpp:717 — ConstructLegacyPlayerSnapshotX_467C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 948 | 0x00467C20 | `unknown_libname_20` | 有正文 | `src/world/SceneState.cpp:723 — DestroyLegacyPlayerSnapshotX_467C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 949 | 0x00467C30 | `sub_467C30` | 有承接 | `src/world/SceneState.cpp:891 — PlayerSnapshotYStartup::PlayerSnapshotYStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 950 | 0x00467C40 | `unknown_libname_21` | 有正文 | `src/world/SceneState.cpp:729 — ConstructLegacyPlayerSnapshotY_467C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 951 | 0x00467C60 | `unknown_libname_22` | 有正文 | `src/world/SceneState.cpp:735 — DestroyLegacyPlayerSnapshotY_467C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 952 | 0x00467C70 | `sub_467C70` | 有承接 | `src/world/SceneState.cpp:903 — WorldCoordinateState9EE8D0Startup::WorldCoordinateState9EE8D0Startup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 953 | 0x00467C80 | `unknown_libname_23` | 有正文 | `src/world/SceneState.cpp:741 — ConstructLegacyWorldCoordinateState_467C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 954 | 0x00467CA0 | `unknown_libname_24` | 有正文 | `src/world/SceneState.cpp:747 — DestroyLegacyWorldCoordinateState_467CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 955 | 0x00467CB0 | `sub_467CB0` | 有承接 | `src/world/SceneState.cpp:915 — WorldCoordinateState9EE8E0Startup::WorldCoordinateState9EE8E0Startup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 956 | 0x00467CC0 | `unknown_libname_25` | 有正文 | `src/world/SceneState.cpp:753 — ConstructLegacyWorldCoordinateState_467CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 957 | 0x00467CE0 | `unknown_libname_26` | 有正文 | `src/world/SceneState.cpp:759 — DestroyLegacyWorldCoordinateState_467CE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 958 | 0x00467CF0 | `sub_467CF0` | 有承接 | `src/world/SceneState.cpp:927 — WorldCoordinatePairStartup::WorldCoordinatePairStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 959 | 0x00467D00 | `unknown_libname_27` | 有正文 | `src/world/SceneState.cpp:765 — ConstructLegacyWorldCoordinatePair_467D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 960 | 0x00467D30 | `unknown_libname_28` | 有正文 | `src/world/SceneState.cpp:772 — DestroyLegacyWorldCoordinatePair_467D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 961 | 0x00467D50 | `sub_467D50` | 有承接 | `src/world/SceneState.cpp:939 — WorldMoveTargetsStartup::WorldMoveTargetsStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 962 | 0x00467D60 | `sub_467D60` | 有正文 | `src/world/SceneState.cpp:779 — ConstructLegacyWorldMoveTargets_467D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 963 | 0x00467D90 | `sub_467D90` | 有正文 | `src/world/SceneState.cpp:786 — DestroyLegacyWorldMoveTargets_467D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 964 | 0x00467DB0 | `sub_467DB0` | 有承接 | `src/world/SceneState.cpp:951 — WorldMoveStateC821E8Startup::WorldMoveStateC821E8Startup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 965 | 0x00467DC0 | `unknown_libname_31` | 有正文 | `src/world/SceneState.cpp:793 — ConstructLegacyWorldMoveState_467DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 966 | 0x00467DE0 | `unknown_libname_32` | 有正文 | `src/world/SceneState.cpp:799 — DestroyLegacyWorldMoveState_467DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 967 | 0x00467DF0 | `sub_467DF0` | 有承接 | `src/world/SceneState.cpp:963 — WorldMoveState9E03C0Startup::WorldMoveState9E03C0Startup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 968 | 0x00467E00 | `unknown_libname_33` | 有正文 | `src/world/SceneState.cpp:805 — ConstructLegacyWorldMoveState_467E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 969 | 0x00467E20 | `unknown_libname_34` | 有正文 | `src/world/SceneState.cpp:811 — DestroyLegacyWorldMoveState_467E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 970 | 0x00467E30 | `sub_467E30` | 有承接 | `src/world/SceneState.cpp:975 — WorldMoveState9FB918Startup::WorldMoveState9FB918Startup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 971 | 0x00467E40 | `unknown_libname_35` | 有正文 | `src/world/SceneState.cpp:817 — ConstructLegacyWorldMoveState_467E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 972 | 0x00467E60 | `unknown_libname_36` | 有正文 | `src/world/SceneState.cpp:823 — DestroyLegacyWorldMoveState_467E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 973 | 0x00467E70 | `sub_467E70` | 有承接 | `src/world/SceneState.cpp:987 — WorldMoveStateAF0FE8Startup::WorldMoveStateAF0FE8Startup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 974 | 0x00467E80 | `unknown_libname_37` | 有正文 | `src/world/SceneState.cpp:829 — ConstructLegacyWorldMoveState_467E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 975 | 0x00467EA0 | `unknown_libname_38` | 有正文 | `src/world/SceneState.cpp:835 — DestroyLegacyWorldMoveState_467EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 976 | 0x00467EB0 | `sub_467EB0` | 有承接 | `src/world/SceneState.cpp:999 — WorldMoveStateC821D8Startup::WorldMoveStateC821D8Startup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 977 | 0x00467EC0 | `unknown_libname_39` | 有正文 | `src/world/SceneState.cpp:841 — ConstructLegacyWorldMoveState_467EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 978 | 0x00467EE0 | `unknown_libname_40` | 有正文 | `src/world/SceneState.cpp:847 — DestroyLegacyWorldMoveState_467EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 979 | 0x00467EF0 | `sub_467EF0` | 有承接 | `src/world/SceneState.cpp:1011 — RippleGateStartup::RippleGateStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 980 | 0x00467F00 | `unknown_libname_41` | 有正文 | `src/world/SceneState.cpp:853 — ConstructLegacyRippleGate_467F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 981 | 0x00467F20 | `unknown_libname_42` | 有正文 | `src/world/SceneState.cpp:859 — DestroyLegacyRippleGate_467F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 982 | 0x00467F30 | `sub_467F30` | 有正文 | `src/world/SceneState.cpp:5672 — Sub_467F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 983 | 0x004681B0 | `sub_4681B0` | 有正文 | `src/world/SceneState.cpp:5740 — Sub_4681B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 984 | 0x00468200 | `sub_468200` | 有正文 | `src/world/SceneState.cpp:5576 — Sub_468200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 985 | 0x00468290 | `sub_468290` | 有正文 | `src/world/SceneState.cpp:5758 — Sub_468290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 986 | 0x004682E0 | `sub_4682E0` | 有正文 | `src/world/SceneState.cpp:7726 — Sub_4682E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 987 | 0x00468330 | `sub_468330` | 有正文 | `src/world/SceneState.cpp:5621 — Sub_468330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 988 | 0x00468520 | `sub_468520` | 有正文 | `src/world/SceneState.cpp:4834 — Sub_468520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 989 | 0x00468570 | `Map::SaveToDisk` | 有正文 | `src/world/SceneState.cpp:4849 — Sub_468570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 990 | 0x004687B0 | `sub_4687B0` | 有正文 | `src/world/SceneState.cpp:4905 — Sub_4687B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 991 | 0x004688A0 | `sub_4688A0` | 有正文 | `src/world/SceneState.cpp:4938 — Sub_4688A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 992 | 0x00468C00 | `sub_468C00` | 有正文 | `src/world/SceneState.cpp:5782 — Sub_468C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 993 | 0x00468C40 | `sub_468C40` | 有正文 | `src/world/SceneState.cpp:5204 — Sub_468C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 994 | 0x00468F30 | `sub_468F30` | 有正文 | `src/world/SceneState.cpp:8056 — Sub_468F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 995 | 0x00468F70 | `sub_468F70` | 有正文 | `src/world/SceneState.cpp:8074 — Sub_468F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 996 | 0x00468FF0 | `sub_468FF0` | 有正文 | `src/world/SceneState.cpp:1104 — CopyWorldProjectionState_468FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 997 | 0x00469020 | `sub_469020` | 有正文 | `src/world/MapSceneDrawing.cpp:329 — Sub_469020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 998 | 0x00469030 | `sub_469030` | 有正文 | `src/world/SceneState.cpp:7503 — Sub_469030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 999 | 0x004691D0 | `sub_4691D0` | 有正文 | `src/world/SceneState.cpp:5885 — Sub_4691D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1000 | 0x00469240 | `sub_469240` | 有正文 | `src/world/SceneState.cpp:5825 — Sub_469240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1001 | 0x00469290 | `sub_469290` | 有正文 | `src/world/SceneState.cpp:5839 — Sub_469290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1002 | 0x004693F0 | `sub_4693F0` | 有正文 | `src/world/SceneState.cpp:7577 — Sub_4693F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1003 | 0x00469810 | `sub_469810` | 有正文 | `src/world/SceneState.cpp:5805 — Sub_469810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1004 | 0x00469870 | `sub_469870` | 有正文 | `src/world/SceneState.cpp:5020 — Sub_469870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1005 | 0x00469C90 | `World::CharaMove` | 有正文 | `src/world/SceneState.cpp:9478 — Sub_469C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1006 | 0x00469FC0 | `sub_469FC0` | 有正文 | `src/world/SceneState.cpp:9561 — Sub_469FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1007 | 0x0046A190 | `sub_46A190` | 有正文 | `src/YobiRuntime.cpp:37131 — Sub_46A190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1008 | 0x0046A410 | `sub_46A410` | 有正文 | `src/world/SceneState.cpp:9658 — Sub_46A410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1009 | 0x0046A5A0 | `sub_46A5A0` | 有正文 | `src/world/SceneState.cpp:8970 — Sub_46A5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1010 | 0x0046A5F0 | `sub_46A5F0` | 有正文 | `src/YobiRuntime.cpp:37281 — Sub_46A5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1011 | 0x0046A700 | `sub_46A700` | 有正文 | `src/world/SceneState.cpp:9702 — Sub_46A700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1012 | 0x0046A940 | `sub_46A940` | 有正文 | `src/world/SceneState.cpp:8990 — Sub_46A940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1013 | 0x0046A990 | `sub_46A990` | 有正文 | `src/world/SceneState.cpp:8865 — Sub_46A990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1014 | 0x0046A9E0 | `sub_46A9E0` | 有正文 | `src/world/SceneState.cpp:7769 — Sub_46A9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1015 | 0x0046AA00 | `sub_46AA00` | 有正文 | `src/world/SceneState.cpp:7778 — Sub_46AA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1016 | 0x0046AB60 | `sub_46AB60` | 有正文 | `src/world/SceneState.cpp:7975 — Sub_46AB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1017 | 0x0046ABD0 | `sub_46ABD0` | 有正文 | `src/world/SceneState.cpp:7870 — Sub_46ABD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1018 | 0x0046AC10 | `sub_46AC10` | 有正文 | `src/world/SceneState.cpp:7887 — Sub_46AC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1019 | 0x0046AC90 | `sub_46AC90` | 有正文 | `src/world/SceneState.cpp:8010 — Sub_46AC90` | LCI10局部临时载荷改整数operator=，一次RNG、目标token／reserved与Y→X发送序保持；仅静态，完整业务／运行未验 |
| 1020 | 0x0046AD00 | `sub_46AD00` | 有正文 | `src/world/SceneState.cpp:7700 — Sub_46AD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1021 | 0x0046ADA0 | `sub_46ADA0` | 有正文 | `src/world/SceneState.cpp:7936 — Sub_46ADA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1022 | 0x0046AE60 | `sub_46AE60` | 有正文 | `src/world/SceneState.cpp:5794 — Sub_46AE60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1023 | 0x0046AEA0 | `sub_46AEA0` | 有正文 | `src/world/SceneState.cpp:8025 — Sub_46AEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1024 | 0x0046AF30 | `sub_46AF30` | 有正文 | `src/world/SceneState.cpp:5171 — Sub_46AF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1025 | 0x0046AFE0 | `sub_46AFE0` | 有正文 | `src/world/SceneState.cpp:8897 — Sub_46AFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1026 | 0x0046B1B0 | `Player::MoveOneStep` | 有正文 | `src/YobiRuntime.cpp:37232 — MovePlayerOneStep_46B1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1027 | 0x0046B370 | `sub_46B370` | 有正文 | `src/world/SceneState.cpp:9435 — Sub_46B370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1028 | 0x0046B4B0 | `sub_46B4B0` | 有正文 | `src/YobiRuntime.cpp:37096 — Sub_46B4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1029 | 0x0046B580 | `sub_46B580` | 有正文 | `src/world/SceneState.cpp:8222 — Sub_46B580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1030 | 0x0046B5C0 | `sub_46B5C0` | 有正文 | `src/world/SceneState.cpp:8236 — Sub_46B5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1031 | 0x0046B6F0 | `sub_46B6F0` | 有正文 | `src/world/SceneState.cpp:8289 — Sub_46B6F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1032 | 0x0046BC90 | `sub_46BC90` | 有正文 | `src/world/SceneState.cpp:7853 — Sub_46BC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1033 | 0x0046BCF0 | `sub_46BCF0` | 有正文 | `src/world/SceneState.cpp:7920 — Sub_46BCF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1034 | 0x0046BD20 | `sub_46BD20` | 有正文 | `src/world/SceneState.cpp:8492 — Sub_46BD20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1035 | 0x0046BD40 | `sub_46BD40` | 有正文 | `src/world/SceneState.cpp:10494 — Sub_46BD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1036 | 0x0046BD60 | `sub_46BD60` | 有正文 | `src/world/MapDrawCache.cpp:348 — Sub_46BD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1037 | 0x0046BE50 | `sub_46BE50` | 有正文 | `src/world/MapDrawCache.cpp:373 — Sub_46BE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1038 | 0x0046C060 | `YobiMapChara::DrawGearNormal` | 有正文 | `src/world/MapDrawCache.cpp:440 — DrawGearNormal_46C060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1039 | 0x0046C1C0 | `sub_46C1C0` | 有正文 | `src/world/MapDrawCache.cpp:489 — Sub_46C1C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1040 | 0x0046C2F0 | `sub_46C2F0` | 有正文 | `src/world/MapDrawCache.cpp:482 — Sub_46C2F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1041 | 0x0046C300 | `sub_46C300` | 有正文 | `src/world/MapDrawCache.cpp:100 — CompareMapObject_46C300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1042 | 0x0046C420 | `sub_46C420` | 有正文 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1043 | 0x0046C440 | `sub_46C440` | 有正文 | `src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1044 | 0x0046C4D0 | `sub_46C4D0` | 有正文 | `src/world/MapDrawCache.cpp:186 — MergeMapDrawCache_46C4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1045 | 0x0046C760 | `sub_46C760` | 有正文 | `src/world/SceneState.cpp:5481 — Sub_46C760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1046 | 0x0046C840 | `sub_46C840` | 有正文 | `src/world/SceneState.cpp:5307 — Sub_46C840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1047 | 0x0046CDF0 | `sub_46CDF0` | 有正文 | `src/world/SceneState.cpp:8210 — Sub_46CDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1048 | 0x0046CE30 | `sub_46CE30` | 有正文 | `src/world/SceneState.cpp:1058 — ProjectWorldToScreen_46CE30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1049 | 0x0046CE80 | `sub_46CE80` | 有正文 | `src/world/SceneState.cpp:1070 — ProjectScreenToWorld_46CE80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1050 | 0x0046CEC0 | `sub_46CEC0` | 有正文 | `src/world/SceneState.cpp:5604 — Sub_46CEC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1051 | 0x0046CEE0 | `sub_46CEE0` | 有正文 | `src/world/SceneState.cpp:5611 — Sub_46CEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1052 | 0x0046CF00 | `sub_46CF00` | 有正文 | `src/world/SceneState.cpp:8095 — Sub_46CF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1053 | 0x0046CF20 | `sub_46CF20` | 有正文 | `src/world/SceneState.cpp:8107 — Sub_46CF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1054 | 0x0046CF60 | `sub_46CF60` | 有正文 | `src/world/MapSceneDrawing.cpp:197 — Sub_46CF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1055 | 0x0046D9C0 | `sub_46D9C0` | 有正文 | `src/world/SceneState.cpp:7673 — Sub_46D9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1056 | 0x0046DA80 | `sub_46DA80` | 有正文 | `src/world/SceneState.cpp:7746 — Sub_46DA80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1057 | 0x0046DB00 | `sub_46DB00` | 有正文 | `src/world/SceneState.cpp:5516 — Sub_46DB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1058 | 0x0046DC60 | `sub_46DC60` | 有正文 | `src/world/SceneState.cpp:5507 — Sub_46DC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1059 | 0x0046DC90 | `sub_46DC90` | 有正文 | `src/world/SceneState.cpp:5554 — Sub_46DC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1060 | 0x0046DD40 | `sub_46DD40` | 有正文 | `src/YobiRuntime.cpp:1597 — Sub_46DD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1061 | 0x0046DD90 | `sub_46DD90` | 有正文 | `src/YobiRuntime.cpp:5542 — Sub_46DD90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1062 | 0x0046DE50 | `sub_46DE50` | 有正文 | `src/YobiRuntime.cpp:5506 — Sub_46DE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1063 | 0x0046DFF0 | `sub_46DFF0` | 有正文 | `src/world/MapDrawCache.cpp:407 — Sub_46DFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1064 | 0x0046E0D0 | `sub_46E0D0` | 有正文 | `src/world/SceneState.cpp:2119 — Sub_46E0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1065 | 0x0046E2A0 | `sub_46E2A0` | 有正文 | `src/world/SceneState.cpp:1471 — Sub_46E2A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1066 | 0x0046E340 | `sub_46E340` | 有正文 | `src/world/SceneState.cpp:1504 — Sub_46E340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1067 | 0x0046E390 | `sub_46E390` | 有正文 | `src/world/SceneState.cpp:1528 — Sub_46E390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1068 | 0x0046E3E0 | `sub_46E3E0` | 有正文 | `src/world/SceneState.cpp:1554 — Sub_46E3E0` | 正文已有；实际五个地图效果调用由同文件1615行的 ApplyMapEffectDirection 承接，调用点1823／1887／1953／2019／2114；独立包装无当前调用不等于缺接线，本步仅定位提供者，不重验原正文或提升完整业务／运行门禁 |
| 1069 | 0x0046E480 | `sub_46E480` | 有正文 | `src/world/SceneState.cpp:1746 — Sub_46E480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1070 | 0x0046E4A0 | `sub_46E4A0` | 有正文 | `src/world/SceneState.cpp:2109 — Sub_46E4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1071 | 0x0046E4C0 | `sub_46E4C0` | 有正文 | `src/world/SceneState.cpp:2019 — Sub_46E4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1072 | 0x0046E4E0 | `sub_46E4E0` | 有正文 | `src/world/SceneState.cpp:1755 — Sub_46E4E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1073 | 0x0046E6C0 | `sub_46E6C0` | 有正文 | `src/world/SceneState.cpp:1821 — Sub_46E6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1074 | 0x0046E940 | `sub_46E940` | 有正文 | `src/world/SceneState.cpp:1885 — Sub_46E940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1075 | 0x0046EBB0 | `sub_46EBB0` | 有正文 | `src/world/SceneState.cpp:1954 — Sub_46EBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1076 | 0x0046ED50 | `sub_46ED50` | 有正文 | `src/world/SceneState.cpp:2026 — Sub_46ED50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1077 | 0x0046F000 | `sub_46F000` | 有正文 | `src/world/SceneState.cpp:2233 — Sub_46F000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1078 | 0x0046F090 | `sub_46F090` | 有正文 | `src/world/SceneState.cpp:2184 — Sub_46F090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1079 | 0x0046F160 | `sub_46F160` | 有正文 | `src/world/SceneState.cpp:2163 — Sub_46F160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1080 | 0x0046F1D0 | `sub_46F1D0` | 有正文 | `src/world/SceneState.cpp:2261 — Sub_46F1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1081 | 0x0046F680 | `World::RendererCursorGrid` | 有正文 | `src/world/SceneState.cpp:9012 — Sub_46F680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1082 | 0x0046F8F0 | `sub_46F8F0` | 有正文 | `src/world/SceneState.cpp:8119 — Sub_46F8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1083 | 0x0046F930 | `sub_46F930` | 有正文 | `src/world/SceneState.cpp:8140 — Sub_46F930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1084 | 0x0046F950 | `sub_46F950` | 有正文 | `src/world/SceneState.cpp:8149 — Sub_46F950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1085 | 0x0046F970 | `sub_46F970` | 有正文 | `src/world/SceneState.cpp:8156 — Sub_46F970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1086 | 0x0046FAF0 | `RAND` | 有正文 | `src/platform/MainLoopSupport.cpp:705 — RandomRange_46FAF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1087 | 0x0046FB20 | `sub_46FB20` | 有正文 | `src/world/SceneState.cpp:1118 — InitializeRotationTable_46FB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1088 | 0x0046FB80 | `sub_46FB80` | 有正文 | `src/world/SceneState.cpp:8199 — Sub_46FB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1089 | 0x0046FBA0 | `sub_46FBA0` | 有正文 | `src/ui/WindowRuntime.cpp:41153 — Sub_46FBA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1090 | 0x0046FBC0 | `sub_46FBC0` | 有正文 | `src/ui/WindowRuntime.cpp:30913 — Sub_46FBC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1091 | 0x0046FBD0 | `sub_46FBD0` | 有正文 | `src/ui/WindowRuntime.cpp:41162 — Sub_46FBD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1092 | 0x0046FC60 | `sub_46FC60` | 有正文 | `src/ui/WindowRuntime.cpp:41369 — Sub_46FC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1093 | 0x00470050 | `sub_470050` | 有正文 | `src/world/SceneState.cpp:9118 — Sub_470050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1094 | 0x00470150 | `sub_470150` | 有正文 | `src/world/SceneState.cpp:9150 — Sub_470150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1095 | 0x00470270 | `sub_470270` | 有正文 | `src/world/SceneState.cpp:9185 — Sub_470270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1096 | 0x004703C0 | `sub_4703C0` | 有正文 | `src/world/SceneState.cpp:9225 — Sub_4703C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1097 | 0x004704A0 | `sub_4704A0` | 有正文 | `src/world/SceneState.cpp:9255 — Sub_4704A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1098 | 0x004705D0 | `sub_4705D0` | 有正文 | `src/ui/WindowRuntime.cpp:16321 — InitializeWindowSystem_4705D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1099 | 0x004705F0 | `sub_4705F0` | 有正文 | `src/platform/ClientShutdown.cpp:31 — Sub_4705F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1100 | 0x00470600 | `sub_470600` | 有正文 | `src/ui/WindowRuntime.cpp:30065 — InitializeLoginWindows_470600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1101 | 0x00470670 | `sub_470670` | 有正文 | `src/world/SceneState.cpp:9291 — Sub_470670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1102 | 0x004708A0 | `sub_4708A0` | 有正文 | `src/ui/WindowRuntime.cpp:41329 — Sub_4708A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1103 | 0x00470910 | `sub_470910` | 有正文 | `src/ui/WindowRuntime.cpp:41349 — Sub_470910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1104 | 0x00470980 | `sub_470980` | 有正文 | `src/ui/WindowRuntime.cpp:16301 — Sub_470980` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1105 | 0x00470A80 | `sub_470A80` | 有正文 | `src/ui/WindowRuntime.cpp:41465 — Sub_470A80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1106 | 0x00470A9F | `?classic@locale@std@@SAABV12@XZ_0` | 有承接 | `src/ui/WindowRuntime.cpp:41465 — Sub_470A80` | 原共享尾为调用 470AC0 后返回 1；并入实际父正文；完整业务／运行未验 |
| 1107 | 0x00470AB0 | `?classic@locale@std@@SAABV12@XZ_1` | 有正文 | `src/ui/WindowRuntime.cpp:41476 — Sub_470AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1108 | 0x00470AC0 | `sub_470AC0` | 有正文 | `src/ui/WindowRuntime.cpp:41484 — Sub_470AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1109 | 0x00471270 | `UIWindowInstance::free` | 有正文 | `src/ui/WindowRuntime.cpp:45692 — FreeWindow_471270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1110 | 0x00471440 | `Window::open` | 有正文 | `src/ui/WindowRuntime.cpp:45051 — OpenWindow_471440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1111 | 0x004718F0 | `sub_4718F0` | 有正文 | `src/ui/WindowRuntime.cpp:41546 — Sub_4718F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1112 | 0x00471DF0 | `sub_471DF0` | 有正文 | `src/ui/WindowRuntime.cpp:41603 — Sub_471DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1113 | 0x00472160 | `Windows::close` | 有正文 | `src/ui/WindowRuntime.cpp:45762 — CloseWindow_472160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1114 | 0x00472190 | `Window::CloseAll` | 有正文 | `src/ui/WindowRuntime.cpp:45771 — CloseAllWindows_472190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1115 | 0x00472200 | `sub_472200` | 有正文 | `src/ui/WindowRuntime.cpp:15706 — Sub_472200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1116 | 0x00472220 | `sub_472220` | 有正文 | `src/ui/WindowRuntime.cpp:15714 — Sub_472220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1117 | 0x00472240 | `Window::CloseByCategory` | 有正文 | `src/ui/WindowRuntime.cpp:45864 — CloseWindowsByCategory_472240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1118 | 0x004722A0 | `MouseEvent::Reset` | 有正文 | `src/ui/WindowRuntime.cpp:15724 — Sub_4722A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1119 | 0x004722E0 | `sub_4722E0` | 有正文 | `src/ui/WindowRuntime.cpp:15738 — Sub_4722E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1120 | 0x004722F0 | `sub_4722F0` | 有正文 | `src/ui/WindowRuntime.cpp:15751 — Sub_4722F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1121 | 0x004723D0 | `sub_4723D0` | 有正文 | `src/ui/WindowRuntime.cpp:41677 — Sub_4723D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1122 | 0x004729B0 | `Window::DrawCurrentWindow` | 有正文 | `src/ui/WindowRuntime.cpp:45886 — DrawCurrentWindow_4729B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1123 | 0x00472D20 | `Window::ProsseFocus` | 有正文 | `src/ui/WindowRuntime.cpp:40639 — Sub_472D20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1124 | 0x00472F00 | `Mouse::isInRect` | 有正文 | `src/ui/WindowRuntime.cpp:40501 — Sub_472F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1125 | 0x00472F40 | `sub_472F40` | 有正文 | `src/world/SceneState.cpp:2862 — Sub_472F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1126 | 0x00472F70 | `sub_472F70` | 有正文 | `src/world/SceneState.cpp:2873 — Sub_472F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1127 | 0x00472FA0 | `sub_472FA0` | 有正文 | `src/world/SceneState.cpp:2879 — Sub_472FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1128 | 0x00472FD0 | `sub_472FD0` | 有正文 | `src/world/SceneState.cpp:2885 — Sub_472FD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1129 | 0x00473000 | `sub_473000` | 有正文 | `src/world/SceneState.cpp:2891 — Sub_473000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1130 | 0x00473030 | `sub_473030` | 有正文 | `src/world/SceneState.cpp:2897 — Sub_473030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1131 | 0x00473080 | `sub_473080` | 有正文 | `src/ui/WindowRuntime.cpp:41652 — Sub_473080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1132 | 0x004730A0 | `Window::sortLayer` | 有正文 | `src/ui/WindowRuntime.cpp:44574 — Sub_4730A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1133 | 0x004731B0 | `sub_4731B0` | 有正文 | `src/ui/WindowRuntime.cpp:41663 — Sub_4731B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1134 | 0x004731E0 | `sub_4731E0` | 有正文 | `src/ui/WindowRuntime.cpp:41867 — Sub_4731E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1135 | 0x00473560 | `Window::ProcessChildElement` | 有正文 | `src/ui/WindowRuntime.cpp:44674 — Sub_473560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1136 | 0x00473840 | `sub_473840` | 有正文 | `src/ui/WindowRuntime.cpp:40721 — Sub_473840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1137 | 0x00473E40 | `MouseEvent::Update` | 有正文 | `src/ui/WindowRuntime.cpp:44623 — Sub_473E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1138 | 0x00473F80 | `Windows::Update` | 有正文 | `src/ui/WindowRuntime.cpp:44926 — UpdateWindows_473F80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1139 | 0x004740F0 | `sub_4740F0` | 有正文 | `src/ui/WindowRuntime.cpp:44853 — Sub_4740F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1140 | 0x00474170 | `sub_474170` | 有正文 | `src/ui/WindowRuntime.cpp:5696 — Sub_474170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1141 | 0x004741A0 | `sub_4741A0` | 有正文 | `src/ui/WindowRuntime.cpp:44740 — Sub_4741A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1142 | 0x00474270 | `sub_474270` | 有正文 | `src/ui/WindowRuntime.cpp:40204 — Sub_474270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1143 | 0x004742B0 | `sub_4742B0` | 有正文 | `src/ui/WindowRuntime.cpp:25536 — Sub_4742B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1144 | 0x004742C0 | `sub_4742C0` | 有正文 | `src/ui/WindowRuntime.cpp:40220 — Sub_4742C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1145 | 0x00474310 | `sub_474310` | 有正文 | `src/ui/WindowRuntime.cpp:42190 — Sub_474310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1146 | 0x00474360 | `sub_474360` | 有正文 | `src/ui/WindowRuntime.cpp:42366 — Sub_474360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1147 | 0x004743B0 | `sub_4743B0` | 有正文 | `src/ui/WindowRuntime.cpp:40243 — Sub_4743B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1148 | 0x004744A0 | `sub_4744A0` | 有正文 | `src/ui/WindowRuntime.cpp:44559 — Sub_4744A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1149 | 0x004744F0 | `sub_4744F0` | 有正文 | `src/ui/WindowRuntime.cpp:44426 — Sub_4744F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1150 | 0x00474540 | `?classic@locale@std@@SAABV12@XZ_2` | 有正文 | `src/ui/WindowRuntime.cpp:42183 — Sub_474540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1151 | 0x00474550 | `sub_474550` | 有正文 | `src/ui/WindowRuntime.cpp:7696 — Sub_474550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1152 | 0x00474FB0 | `sub_474FB0` | 有正文 | `src/ui/WindowRuntime.cpp:42041 — Sub_474FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1153 | 0x004758D0 | `YobiWindow::Update` | 有正文 | `src/YobiRuntime.cpp:1912 — Sub_4758D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1154 | 0x00475950 | `Yobi::newWindow` | 有正文 | `src/YobiRuntime.cpp:1943 — Sub_475950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1155 | 0x00475AA0 | `Yobi::newWindow2` | 有正文 | `src/ui/WindowRuntime.cpp:45217 — CreateWindowAnimation_475AA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1156 | 0x00475C10 | `sub_475C10` | 有正文 | `src/ui/WindowRuntime.cpp:15829 — RestoreWindowPositions_475C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1157 | 0x00475D40 | `Window::GetFocusWinId` | 有正文 | `src/ui/WindowRuntime.cpp:40514 — Sub_475D40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1158 | 0x00475D50 | `sub_475D50` | 有正文 | `src/ui/WindowRuntime.cpp:40520 — Sub_475D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1159 | 0x00475D60 | `sub_475D60` | 有正文 | `src/ui/WindowRuntime.cpp:40526 — Sub_475D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1160 | 0x00475D70 | `sub_475D70` | 有正文 | `src/ui/WindowRuntime.cpp:40532 — Sub_475D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1161 | 0x00475D80 | `sub_475D80` | 有正文 | `src/ui/WindowRuntime.cpp:40538 — Sub_475D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1162 | 0x00475D90 | `sub_475D90` | 有正文 | `src/ui/WindowRuntime.cpp:40544 — Sub_475D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1163 | 0x00475DA0 | `sub_475DA0` | 有正文 | `src/ui/WindowRuntime.cpp:40551 — Sub_475DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1164 | 0x00475DF0 | `sub_475DF0` | 有正文 | `src/ui/WindowRuntime.cpp:42359 — Sub_475DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1165 | 0x00475E00 | `sub_475E00` | 有正文 | `src/ui/WindowRuntime.cpp:42204 — Sub_475E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1166 | 0x004763B0 | `sub_4763B0` | 有正文 | `src/ui/WindowRuntime.cpp:40614 — Sub_4763B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1167 | 0x004763E0 | `MouseEvent::reset` | 有正文 | `src/ui/WindowRuntime.cpp:40627 — Sub_4763E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1168 | 0x00476410 | `sub_476410` | 有正文 | `src/ui/WindowRuntime.cpp:45669 — CancelWindowDrag_476410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1169 | 0x00476490 | `MouseEvent::setupEventCallback` | 有正文 | `src/ui/WindowRuntime.cpp:40570 — Sub_476490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1170 | 0x00476500 | `InvDragReset` | 有正文 | `src/ui/WindowRuntime.cpp:40592 — Sub_476500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1171 | 0x00476570 | `sub_476570` | 有正文 | `src/ui/WindowRuntime.cpp:44793 — Sub_476570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1172 | 0x00476680 | `sub_476680` | 有正文 | `src/world/SceneState.cpp:2437 — Sub_476680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1173 | 0x00476740 | `sub_476740` | 有正文 | `src/Input.cpp:607 — Sub_476740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1174 | 0x004767E0 | `sub_4767E0` | 有正文 | `src/ui/WindowRuntime.cpp:42295 — Sub_4767E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1175 | 0x00477160 | `sub_477160` | 有正文 | `src/ui/WindowRuntime.cpp:45602 — InitializeLegacyNumberSpacing_477160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1176 | 0x00477170 | `sub_477170` | 有正文 | `src/ui/WindowRuntime.cpp:45609 — InitializeNumberSpacing_477170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1177 | 0x004771B0 | `sub_4771B0` | 有正文 | `src/ui/WindowRuntime.cpp:30833 — Sub_4771B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1178 | 0x00477460 | `sub_477460` | 有正文 | `src/ui/WindowRuntime.cpp:30900 — Sub_477460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1179 | 0x00477490 | `Window::FindByWinId` | 有正文 | `src/ui/WindowRuntime.cpp:30138 — FindWindowById_477490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1180 | 0x004774C0 | `sub_4774C0` | 有正文 | `src/ui/WindowRuntime.cpp:44540 — Sub_4774C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1181 | 0x0047752C | `?classic@locale@std@@SAABV12@XZ_4` | 有承接 | `src/ui/WindowRuntime.cpp:44540 — Sub_4774C0` | 原共享尾为调用 478010 后返回 1；并入实际父正文；完整业务／运行未验 |
| 1182 | 0x00477540 | `sub_477540` | 有正文 | `src/ui/WindowRuntime.cpp:44028 — Sub_477540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1183 | 0x00477600 | `sub_477600` | 有正文 | `src/ui/WindowRuntime.cpp:44053 — Sub_477600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1184 | 0x00477710 | `sub_477710` | 有正文 | `src/ui/WindowRuntime.cpp:44087 — Sub_477710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1185 | 0x00478010 | `sub_478010` | 有正文 | `src/ui/WindowRuntime.cpp:44489 — Sub_478010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1186 | 0x00478320 | `?classic@locale@std@@SAABV12@XZ_5` | 有正文 | `src/ui/WindowRuntime.cpp:44419 — Sub_478320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1187 | 0x00478330 | `sub_478330` | 有正文 | `src/ui/WindowRuntime.cpp:5279 — Sub_478330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1188 | 0x00478A10 | `sub_478A10` | 有正文 | `src/ui/WindowRuntime.cpp:44336 — Sub_478A10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1189 | 0x004792D0 | `sub_4792D0` | 有正文 | `src/ui/WindowRuntime.cpp:44440 — Sub_4792D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1190 | 0x00479610 | `sub_479610` | 有正文 | `src/ui/WindowRuntime.cpp:15812 — Sub_479610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1191 | 0x00479640 | `sub_479640` | 有正文 | `src/ui/WindowRuntime.cpp:15794 — Sub_479640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1192 | 0x00479650 | `___inc_tmpoff` | 有正文 | `src/ui/WindowRuntime.cpp:39036 — Sub_479650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1193 | 0x00479660 | `sub_479660` | 有正文 | `src/ui/WindowRuntime.cpp:39042 — Sub_479660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1194 | 0x004796C0 | `sub_4796C0` | 有正文 | `src/ui/WindowRuntime.cpp:15800 — Sub_4796C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1195 | 0x004796D0 | `sub_4796D0` | 有正文 | `src/ui/WindowRuntime.cpp:34140 — Sub_4796D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1196 | 0x004796E0 | `sub_4796E0` | 有正文 | `src/ui/WindowRuntime.cpp:34358 — Sub_4796E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1197 | 0x004796F0 | `sub_4796F0` | 有正文 | `src/ui/WindowRuntime.cpp:15806 — Sub_4796F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1198 | 0x00479700 | `sub_479700` | 有正文 | `src/ui/WindowRuntime.cpp:35580 — Sub_479700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1199 | 0x004797A0 | `sub_4797A0` | 有正文 | `src/ui/WindowRuntime.cpp:35607 — Sub_4797A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1200 | 0x004797C0 | `sub_4797C0` | 有正文 | `src/ui/WindowRuntime.cpp:2078 — Sub_4797C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1201 | 0x00479820 | `sub_479820` | 有正文 | `src/ui/WindowRuntime.cpp:35177 — Sub_479820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1202 | 0x00479860 | `GetPetCrystalLv` | 有正文 | `src/ui/WindowRuntime.cpp:35191 — Sub_479860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1203 | 0x00479920 | `sub_479920` | 有正文 | `src/ui/WindowRuntime.cpp:35208 — Sub_479920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1204 | 0x004799D0 | `sub_4799D0` | 有正文 | `src/ui/WindowRuntime.cpp:35225 — Sub_4799D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1205 | 0x00479A00 | `sub_479A00` | 有正文 | `src/ui/WindowRuntime.cpp:35232 — Sub_479A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1206 | 0x00479A70 | `sub_479A70` | 有正文 | `src/ui/WindowRuntime.cpp:35250 — Sub_479A70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1207 | 0x00479AB0 | `sub_479AB0` | 有正文 | `src/ui/WindowRuntime.cpp:35261 — Sub_479AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1208 | 0x00479B00 | `sub_479B00` | 有正文 | `src/ui/WindowRuntime.cpp:35276 — Sub_479B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1209 | 0x00479B80 | `sub_479B80` | 有正文 | `src/ui/WindowRuntime.cpp:35670 — Sub_479B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1210 | 0x00479C80 | `sub_479C80` | 有正文 | `src/ui/WindowRuntime.cpp:35807 — Sub_479C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1211 | 0x00479DA0 | `sub_479DA0` | 有正文 | `src/ui/WindowRuntime.cpp:38772 — Sub_479DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1212 | 0x00479EA0 | `sub_479EA0` | 有正文 | `src/ui/WindowRuntime.cpp:35859 — Sub_479EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1213 | 0x0047A000 | `sub_47A000` | 有正文 | `src/ui/WindowRuntime.cpp:35917 — Sub_47A000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1214 | 0x0047A140 | `sub_47A140` | 有正文 | `src/ui/WindowRuntime.cpp:39021 — Sub_47A140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1215 | 0x0047A1A0 | `sub_47A1A0` | 有正文 | `src/ui/WindowRuntime.cpp:38109 — Sub_47A1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1216 | 0x0047A320 | `sub_47A320` | 有正文 | `src/ui/WindowRuntime.cpp:38156 — Sub_47A320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1217 | 0x0047A490 | `sub_47A490` | 有正文 | `src/ui/WindowRuntime.cpp:37869 — Sub_47A490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1218 | 0x0047A4C0 | `sub_47A4C0` | 有正文 | `src/ui/WindowRuntime.cpp:37882 — Sub_47A4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1219 | 0x0047A500 | `BattleScene::SelectionCallback2` | 有正文 | `src/ui/WindowRuntime.cpp:37894 — Sub_47A500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1220 | 0x0047A540 | `BattleScene::OpenSkillWindow` | 有正文 | `src/ui/WindowRuntime.cpp:38203 — Sub_47A540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1221 | 0x0047A580 | `BattleScene::OpenSkillWindow2` | 有正文 | `src/ui/WindowRuntime.cpp:38215 — Sub_47A580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1222 | 0x0047A5C0 | `sub_47A5C0` | 有正文 | `src/ui/WindowRuntime.cpp:38273 — Sub_47A5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1223 | 0x0047A620 | `sub_47A620` | 有正文 | `src/ui/WindowRuntime.cpp:38410 — Sub_47A620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1224 | 0x0047A7A0 | `sub_47A7A0` | 有正文 | `src/ui/WindowRuntime.cpp:38465 — Sub_47A7A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1225 | 0x0047B110 | `sub_47B110` | 有正文 | `src/ui/WindowRuntime.cpp:38227 — Sub_47B110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1226 | 0x0047B210 | `sub_47B210` | 有正文 | `src/ui/WindowRuntime.cpp:38296 — Sub_47B210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1227 | 0x0047B3D0 | `sub_47B3D0` | 有正文 | `src/ui/WindowRuntime.cpp:38644 — Sub_47B3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1228 | 0x0047B470 | `sub_47B470` | 有正文 | `src/ui/WindowRuntime.cpp:38672 — Sub_47B470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1229 | 0x0047B520 | `sub_47B520` | 有正文 | `src/ui/WindowRuntime.cpp:38702 — Sub_47B520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1230 | 0x0047B5D0 | `sub_47B5D0` | 有正文 | `src/ui/WindowRuntime.cpp:38729 — Sub_47B5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1231 | 0x0047B680 | `sub_47B680` | 有正文 | `src/ui/WindowRuntime.cpp:38759 — Sub_47B680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1232 | 0x0047B6C0 | `sub_47B6C0` | 有正文 | `src/ui/WindowRuntime.cpp:35547 — Sub_47B6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1233 | 0x0047B820 | `sub_47B820` | 有正文 | `src/ui/WindowRuntime.cpp:35613 — Sub_47B820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1234 | 0x0047B9B0 | `sub_47B9B0` | 有正文 | `src/ui/WindowRuntime.cpp:35718 — Sub_47B9B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1235 | 0x0047BC00 | `sub_47BC00` | 有正文 | `src/ui/WindowRuntime.cpp:38859 — Sub_47BC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1236 | 0x0047C240 | `sub_47C240` | 有正文 | `src/ui/WindowRuntime.cpp:35776 — Sub_47C240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1237 | 0x0047C3A0 | `sub_47C3A0` | 有正文 | `src/ui/WindowRuntime.cpp:38803 — Sub_47C3A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1238 | 0x0047C570 | `sub_47C570` | 有正文 | `src/ui/WindowRuntime.cpp:35963 — Sub_47C570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1239 | 0x0047C700 | `sub_47C700` | 有正文 | `src/ui/WindowRuntime.cpp:36025 — Sub_47C700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1240 | 0x0047C950 | `sub_47C950` | 有正文 | `src/ui/WindowRuntime.cpp:38346 — Sub_47C950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1241 | 0x0047CB00 | `sub_47CB00` | 有正文 | `src/ui/WindowRuntime.cpp:37908 — Sub_47CB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1242 | 0x0047CC80 | `sub_47CC80` | 有正文 | `src/ui/WindowRuntime.cpp:37984 — Sub_47CC80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1243 | 0x0047CE70 | `sub_47CE70` | 有正文 | `src/ui/WindowRuntime.cpp:37947 — Sub_47CE70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1244 | 0x0047CFE0 | `sub_47CFE0` | 有正文 | `src/ui/WindowRuntime.cpp:38049 — Sub_47CFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1245 | 0x0047D170 | `sub_47D170` | 有正文 | `src/ui/WindowRuntime.cpp:33654 — Sub_47D170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1246 | 0x0047D220 | `sub_47D220` | 有正文 | `src/ui/WindowRuntime.cpp:33678 — Sub_47D220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1247 | 0x0047D280 | `sub_47D280` | 有正文 | `src/ui/WindowRuntime.cpp:34828 — Sub_47D280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1248 | 0x0047D2E0 | `sub_47D2E0` | 有正文 | `src/ui/WindowRuntime.cpp:34323 — Sub_47D2E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1249 | 0x0047D490 | `sub_47D490` | 有正文 | `src/ui/WindowRuntime.cpp:5541 — Sub_47D490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1250 | 0x0047D4A0 | `sub_47D4A0` | 有正文 | `src/ui/WindowRuntime.cpp:27955 — Sub_47D4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1251 | 0x0047D4B0 | `sub_47D4B0` | 有正文 | `src/ui/WindowRuntime.cpp:33211 — Sub_47D4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1252 | 0x0047D4F0 | `sub_47D4F0` | 有正文 | `src/ui/WindowRuntime.cpp:33220 — Sub_47D4F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1253 | 0x0047D530 | `sub_47D530` | 有正文 | `src/ui/WindowRuntime.cpp:33231 — Sub_47D530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1254 | 0x0047D5B0 | `sub_47D5B0` | 有正文 | `src/ui/WindowRuntime.cpp:33249 — Sub_47D5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1255 | 0x0047D5F0 | `sub_47D5F0` | 有正文 | `src/ui/WindowRuntime.cpp:31343 — Sub_47D5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1256 | 0x0047D780 | `sub_47D780` | 有正文 | `src/ui/WindowRuntime.cpp:31367 — Sub_47D780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1257 | 0x0047D8F0 | `sub_47D8F0` | 有正文 | `src/ui/WindowRuntime.cpp:31387 — Sub_47D8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1258 | 0x0047DA70 | `sub_47DA70` | 有正文 | `src/ui/WindowRuntime.cpp:31411 — Sub_47DA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1259 | 0x0047DB50 | `sub_47DB50` | 有正文 | `src/ui/WindowRuntime.cpp:31432 — Sub_47DB50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1260 | 0x0047DC20 | `sub_47DC20` | 有正文 | `src/ui/WindowRuntime.cpp:1974 — Sub_47DC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1261 | 0x0047DCD0 | `sub_47DCD0` | 有正文 | `src/ui/WindowRuntime.cpp:2012 — Sub_47DCD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1262 | 0x0047DE00 | `sub_47DE00` | 有正文 | `src/ui/WindowRuntime.cpp:32063 — Sub_47DE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1263 | 0x0047DF30 | `sub_47DF30` | 有正文 | `src/ui/WindowRuntime.cpp:32101 — Sub_47DF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1264 | 0x0047DF50 | `sub_47DF50` | 有正文 | `src/ui/WindowRuntime.cpp:31834 — Sub_47DF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1265 | 0x0047E1B0 | `sub_47E1B0` | 有正文 | `src/ui/WindowRuntime.cpp:31146 — Sub_47E1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1266 | 0x0047E5E0 | `sub_47E5E0` | 有正文 | `src/ui/WindowRuntime.cpp:33745 — Sub_47E5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1267 | 0x0047EBE0 | `sub_47EBE0` | 有正文 | `src/ui/WindowRuntime.cpp:33840 — Sub_47EBE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1268 | 0x0047EC00 | `sub_47EC00` | 有正文 | `src/ui/WindowRuntime.cpp:33140 — Sub_47EC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1269 | 0x0047EF00 | `sub_47EF00` | 有正文 | `src/ui/WindowRuntime.cpp:31182 — Sub_47EF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1270 | 0x0047F3D0 | `sub_47F3D0` | 有正文 | `src/ui/WindowRuntime.cpp:33961 — Sub_47F3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1271 | 0x0047FAC0 | `sub_47FAC0` | 有正文 | `src/ui/WindowRuntime.cpp:34074 — Sub_47FAC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1272 | 0x0047FAE0 | `sub_47FAE0` | 有正文 | `src/ui/WindowRuntime.cpp:31451 — Sub_47FAE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1273 | 0x0047FD00 | `sub_47FD00` | 有正文 | `src/ui/WindowRuntime.cpp:31468 — Sub_47FD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1274 | 0x0047FF20 | `sub_47FF20` | 有正文 | `src/ui/WindowRuntime.cpp:31485 — Sub_47FF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1275 | 0x004801B0 | `sub_4801B0` | 有正文 | `src/ui/WindowRuntime.cpp:31239 — Sub_4801B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1276 | 0x00480600 | `sub_480600` | 有正文 | `src/ui/WindowRuntime.cpp:31276 — Sub_480600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1277 | 0x00480A00 | `sub_480A00` | 有正文 | `src/ui/WindowRuntime.cpp:31502 — Sub_480A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1278 | 0x00480C20 | `sub_480C20` | 有正文 | `src/ui/WindowRuntime.cpp:31311 — Sub_480C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1279 | 0x00481010 | `sub_481010` | 有正文 | `src/ui/WindowRuntime.cpp:33582 — Sub_481010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1280 | 0x004810A0 | `sub_4810A0` | 有正文 | `src/ui/WindowRuntime.cpp:33261 — Sub_4810A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1281 | 0x00482000 | `sub_482000` | 有正文 | `src/ui/WindowRuntime.cpp:33606 — Sub_482000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1282 | 0x00482270 | `sub_482270` | 有正文 | `src/ui/WindowRuntime.cpp:32119 — Sub_482270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1283 | 0x00482F70 | `sub_482F70` | 有正文 | `src/ui/WindowRuntime.cpp:32107 — Sub_482F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1284 | 0x00482FA0 | `sub_482FA0` | 有正文 | `src/ui/WindowRuntime.cpp:32424 — Sub_482FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1285 | 0x004830B0 | `sub_4830B0` | 有正文 | `src/ui/WindowRuntime.cpp:32471 — Sub_4830B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1286 | 0x00483150 | `sub_483150` | 有正文 | `src/ui/WindowRuntime.cpp:32493 — Sub_483150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1287 | 0x00483E30 | `sub_483E30` | 有正文 | `src/ui/WindowRuntime.cpp:31907 — Sub_483E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1288 | 0x00483EB0 | `sub_483EB0` | 有正文 | `src/ui/WindowRuntime.cpp:31521 — Sub_483EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1289 | 0x004847C0 | `sub_4847C0` | 有正文 | `src/ui/WindowRuntime.cpp:32978 — Sub_4847C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1290 | 0x00484D60 | `sub_484D60` | 有正文 | `src/ui/WindowRuntime.cpp:33697 — Sub_484D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1291 | 0x00484E90 | `sub_484E90` | 有正文 | `src/ui/WindowRuntime.cpp:33914 — Sub_484E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1292 | 0x00484FB0 | `sub_484FB0` | 有正文 | `src/ui/WindowRuntime.cpp:32860 — Sub_484FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1293 | 0x00485110 | `sub_485110` | 有正文 | `src/ui/WindowRuntime.cpp:40460 — Sub_485110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1294 | 0x00485160 | `sub_485160` | 有正文 | `src/ui/WindowRuntime.cpp:32903 — Sub_485160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1295 | 0x00485200 | `sub_485200` | 有正文 | `src/ui/WindowRuntime.cpp:32962 — Sub_485200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1296 | 0x00485350 | `sub_485350` | 有正文 | `src/ui/WindowRuntime.cpp:32970 — Sub_485350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1297 | 0x004854A0 | `sub_4854A0` | 有正文 | `src/ui/WindowRuntime.cpp:34146 — Sub_4854A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1298 | 0x00485700 | `sub_485700` | 有正文 | `src/ui/WindowRuntime.cpp:34241 — Sub_485700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1299 | 0x00485890 | `sub_485890` | 有正文 | `src/ui/WindowRuntime.cpp:34269 — Sub_485890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1300 | 0x00485AF0 | `sub_485AF0` | 有正文 | `src/ui/WindowRuntime.cpp:34364 — Sub_485AF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1301 | 0x00486020 | `sub_486020` | 有正文 | `src/ui/WindowRuntime.cpp:34443 — Sub_486020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1302 | 0x00486040 | `sub_486040` | 有正文 | `src/ui/WindowRuntime.cpp:34512 — Sub_486040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1303 | 0x00486CF0 | `sub_486CF0` | 有正文 | `src/ui/WindowRuntime.cpp:34736 — Sub_486CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1304 | 0x00486E50 | `sub_486E50` | 有正文 | `src/ui/WindowRuntime.cpp:34775 — Sub_486E50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1305 | 0x00486F20 | `sub_486F20` | 有正文 | `src/ui/WindowRuntime.cpp:34805 — Sub_486F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1306 | 0x00486FE0 | `sub_486FE0` | 有正文 | `src/ui/WindowRuntime.cpp:35297 — Sub_486FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1307 | 0x00487020 | `sub_487020` | 有正文 | `src/ui/WindowRuntime.cpp:39061 — Sub_487020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1308 | 0x004871B0 | `sub_4871B0` | 有正文 | `src/ui/WindowRuntime.cpp:39111 — Sub_4871B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1309 | 0x004872E0 | `sub_4872E0` | 有正文 | `src/ui/WindowRuntime.cpp:35306 — Sub_4872E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1310 | 0x004874B0 | `sub_4874B0` | 有正文 | `src/ui/WindowRuntime.cpp:35354 — Sub_4874B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1311 | 0x004875E0 | `sub_4875E0` | 有正文 | `src/ui/WindowRuntime.cpp:34868 — Sub_4875E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1312 | 0x00487660 | `sub_487660` | 有正文 | `src/ui/WindowRuntime.cpp:34889 — Sub_487660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1313 | 0x00487AE0 | `sub_487AE0` | 有正文 | `src/ui/WindowRuntime.cpp:34975 — Sub_487AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1314 | 0x00487B80 | `sub_487B80` | 有正文 | `src/ui/WindowRuntime.cpp:34995 — Sub_487B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1315 | 0x00487D60 | `sub_487D60` | 有正文 | `src/ui/WindowRuntime.cpp:35025 — Sub_487D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1316 | 0x00487E80 | `sub_487E80` | 有正文 | `src/ui/WindowRuntime.cpp:35061 — Sub_487E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1317 | 0x00487F30 | `sub_487F30` | 有正文 | `src/ui/WindowRuntime.cpp:34844 — Sub_487F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1318 | 0x004880D0 | `sub_4880D0` | 有正文 | `src/ui/WindowRuntime.cpp:31923 — Sub_4880D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1319 | 0x00488160 | `sub_488160` | 有正文 | `src/ui/WindowRuntime.cpp:31943 — Sub_488160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1320 | 0x00488180 | `sub_488180` | 有正文 | `src/ui/WindowRuntime.cpp:31957 — Sub_488180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1321 | 0x004881D0 | `sub_4881D0` | 有正文 | `src/ui/WindowRuntime.cpp:31970 — Sub_4881D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1322 | 0x004883F0 | `sub_4883F0` | 有承接 | `src/ui/WindowRuntime.cpp:45545 — FirstItemSelectorStartup::FirstItemSelectorStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 1323 | 0x00488400 | `unknown_libname_44` | 有正文 | `src/ui/WindowRuntime.cpp:45528 — ConstructLegacyFirstItemSelector_488400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1324 | 0x00488420 | `sub_488420` | 有正文 | `src/ui/WindowRuntime.cpp:45534 — ReleaseLegacyFirstItemSelectorAtExit_488420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1325 | 0x00488510 | `sub_488510` | 有正文 | `src/ui/WindowRuntime.cpp:8996 — Sub_488510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1326 | 0x004885A0 | `sub_4885A0` | 有正文 | `src/ui/WindowRuntime.cpp:5706 — Sub_4885A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1327 | 0x004886E0 | `sub_4886E0` | 有正文 | `src/ui/WindowRuntime.cpp:5741 — Sub_4886E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1328 | 0x00488800 | `sub_488800` | 有正文 | `src/ui/WindowRuntime.cpp:5783 — Sub_488800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1329 | 0x004888C0 | `sub_4888C0` | 有正文 | `src/ui/WindowRuntime.cpp:8918 — Sub_4888C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1330 | 0x00488930 | `sub_488930` | 有正文 | `src/ui/WindowRuntime.cpp:8933 — Sub_488930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1331 | 0x00488950 | `sub_488950` | 有正文 | `src/ui/WindowRuntime.cpp:8939 — Sub_488950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1332 | 0x00488970 | `sub_488970` | 有正文 | `src/ui/WindowRuntime.cpp:8945 — Sub_488970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1333 | 0x00488990 | `sub_488990` | 有正文 | `src/ui/WindowRuntime.cpp:9020 — Sub_488990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1334 | 0x00488B10 | `sub_488B10` | 有正文 | `src/ui/WindowRuntime.cpp:9075 — Sub_488B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1335 | 0x00488C50 | `sub_488C50` | 有正文 | `src/ui/WindowRuntime.cpp:9113 — Sub_488C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1336 | 0x00488CF0 | `sub_488CF0` | 有正文 | `src/ui/WindowRuntime.cpp:9132 — Sub_488CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1337 | 0x00488D30 | `sub_488D30` | 有正文 | `src/ui/WindowRuntime.cpp:9143 — Sub_488D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1338 | 0x00488E60 | `sub_488E60` | 有正文 | `src/ui/WindowRuntime.cpp:9179 — Sub_488E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1339 | 0x00488E80 | `sub_488E80` | 有正文 | `src/ui/WindowRuntime.cpp:9186 — Sub_488E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1340 | 0x00488EA0 | `sub_488EA0` | 有正文 | `src/ui/WindowRuntime.cpp:8952 — Sub_488EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1341 | 0x00488EC0 | `sub_488EC0` | 有正文 | `src/ui/WindowRuntime.cpp:8959 — Sub_488EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1342 | 0x00488FB0 | `sub_488FB0` | 有正文 | `src/ui/WindowRuntime.cpp:11469 — Sub_488FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1343 | 0x00489760 | `sub_489760` | 有正文 | `src/ui/WindowRuntime.cpp:11668 — Sub_489760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1344 | 0x00489EE0 | `sub_489EE0` | 有正文 | `src/ui/WindowRuntime.cpp:12041 — Sub_489EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1345 | 0x00489F00 | `sub_489F00` | 有正文 | `src/ui/WindowRuntime.cpp:12048 — Sub_489F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1346 | 0x0048A6C0 | `sub_48A6C0` | 有正文 | `src/ui/WindowRuntime.cpp:8873 — Sub_48A6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1347 | 0x0048A830 | `sub_48A830` | 有正文 | `src/ui/WindowRuntime.cpp:5547 — Sub_48A830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1348 | 0x0048A8C0 | `sub_48A8C0` | 有正文 | `src/ui/WindowRuntime.cpp:5571 — Sub_48A8C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1349 | 0x0048A900 | `sub_48A900` | 有正文 | `src/ui/WindowRuntime.cpp:5585 — Sub_48A900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1350 | 0x0048A950 | `sub_48A950` | 有正文 | `src/ui/WindowRuntime.cpp:5598 — Sub_48A950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1351 | 0x0048AA60 | `sub_48AA60` | 有正文 | `src/ui/WindowRuntime.cpp:5641 — Sub_48AA60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1352 | 0x0048AB10 | `sub_48AB10` | 有正文 | `src/ui/WindowRuntime.cpp:5669 — Sub_48AB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1353 | 0x0048ABC0 | `sub_48ABC0` | 有正文 | `src/YobiRuntime.cpp:1997 — Sub_48ABC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1354 | 0x0048AC60 | `sub_48AC60` | 有正文 | `src/YobiRuntime.cpp:2034 — Sub_48AC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1355 | 0x0048AEA0 | `sub_48AEA0` | 有正文 | `src/ui/WindowRuntime.cpp:39220 — Sub_48AEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1356 | 0x0048AF00 | `sub_48AF00` | 有正文 | `src/ui/WindowRuntime.cpp:39241 — Sub_48AF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1357 | 0x0048B010 | `sub_48B010` | 有正文 | `src/ui/WindowRuntime.cpp:39326 — Sub_48B010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1358 | 0x0048B0A0 | `sub_48B0A0` | 有正文 | `src/ui/WindowRuntime.cpp:39353 — Sub_48B0A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1359 | 0x0048B1A0 | `sub_48B1A0` | 有正文 | `src/ui/WindowRuntime.cpp:12201 — Sub_48B1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1360 | 0x0048B350 | `sub_48B350` | 有正文 | `src/ui/WindowRuntime.cpp:12244 — Sub_48B350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1361 | 0x0048B450 | `sub_48B450` | 有正文 | `src/ui/WindowRuntime.cpp:8054 — Sub_48B450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1362 | 0x0048B560 | `sub_48B560` | 有正文 | `src/ui/WindowRuntime.cpp:12273 — Sub_48B560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1363 | 0x0048B750 | `sub_48B750` | 有正文 | `src/ui/WindowRuntime.cpp:12320 — Sub_48B750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1364 | 0x0048B810 | `sub_48B810` | 有正文 | `src/ui/WindowRuntime.cpp:7962 — Sub_48B810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1365 | 0x0048B930 | `sub_48B930` | 有正文 | `src/ui/WindowRuntime.cpp:8008 — Sub_48B930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1366 | 0x0048BA60 | `sub_48BA60` | 有正文 | `src/ui/WindowRuntime.cpp:2095 — Sub_48BA60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1367 | 0x0048BB30 | `sub_48BB30` | 有正文 | `src/ui/WindowRuntime.cpp:5512 — Sub_48BB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1368 | 0x0048BB70 | `sub_48BB70` | 有正文 | `src/ui/WindowRuntime.cpp:5524 — Sub_48BB70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1369 | 0x0048BB90 | `sub_48BB90` | 有正文 | `src/ui/WindowRuntime.cpp:5814 — Sub_48BB90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1370 | 0x0048BC60 | `sub_48BC60` | 有正文 | `src/ui/WindowRuntime.cpp:5530 — Sub_48BC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1371 | 0x0048BC90 | `sub_48BC90` | 有正文 | `src/ui/WindowRuntime.cpp:6580 — Sub_48BC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1372 | 0x0048BF10 | `sub_48BF10` | 有正文 | `src/ui/WindowRuntime.cpp:6670 — Sub_48BF10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1373 | 0x0048BF30 | `sub_48BF30` | 有正文 | `src/ui/WindowRuntime.cpp:6491 — Sub_48BF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1374 | 0x0048C130 | `sub_48C130` | 有正文 | `src/ui/WindowRuntime.cpp:6572 — Sub_48C130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1375 | 0x0048C150 | `sub_48C150` | 有正文 | `src/ui/WindowRuntime.cpp:25517 — Sub_48C150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1376 | 0x0048C1B0 | `sub_48C1B0` | 有正文 | `src/ui/WindowRuntime.cpp:42820 — Sub_48C1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1377 | 0x0048C1CF | `sub_48C1CF` | 有承接 | `src/ui/WindowRuntime.cpp:42820 — Sub_48C1B0` | 原共享尾为调用 48C920 后返回 1；并入实际父正文；完整业务／运行未验 |
| 1378 | 0x0048C1E0 | `sub_48C1E0` | 有正文 | `src/ui/WindowRuntime.cpp:42424 — Sub_48C1E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1379 | 0x0048C920 | `sub_48C920` | 有正文 | `src/ui/WindowRuntime.cpp:42704 — Sub_48C920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1380 | 0x0048D460 | `sub_48D460` | 有正文 | `src/ui/WindowRuntime.cpp:42641 — Sub_48D460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1381 | 0x0048D4E0 | `sub_48D4E0` | 有正文 | `src/ui/WindowRuntime.cpp:42409 — Sub_48D4E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1382 | 0x0048D540 | `sub_48D540` | 有正文 | `src/ui/WindowRuntime.cpp:6214 — Sub_48D540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1383 | 0x0048D9A0 | `sub_48D9A0` | 有正文 | `src/ui/WindowRuntime.cpp:43051 — Sub_48D9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1384 | 0x0048DAB0 | `sub_48DAB0` | 有正文 | `src/ui/WindowRuntime.cpp:43208 — Sub_48DAB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1385 | 0x0048DBB0 | `sub_48DBB0` | 有正文 | `src/ui/WindowRuntime.cpp:43365 — Sub_48DBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1386 | 0x0048DCB0 | `sub_48DCB0` | 有正文 | `src/ui/WindowRuntime.cpp:43399 — Sub_48DCB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1387 | 0x0048DCC0 | `sub_48DCC0` | 有正文 | `src/ui/WindowRuntime.cpp:43407 — Sub_48DCC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1388 | 0x0048DD00 | `sub_48DD00` | 有正文 | `src/ui/WindowRuntime.cpp:43419 — Sub_48DD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1389 | 0x0048DD40 | `sub_48DD40` | 有正文 | `src/ui/WindowRuntime.cpp:43431 — Sub_48DD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1390 | 0x0048DD80 | `sub_48DD80` | 有正文 | `src/ui/WindowRuntime.cpp:43598 — Sub_48DD80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1391 | 0x0048E430 | `sub_48E430` | 有正文 | `src/ui/WindowRuntime.cpp:43740 — Sub_48E430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1392 | 0x0048ECA0 | `sub_48ECA0` | 有正文 | `src/ui/WindowRuntime.cpp:43454 — Sub_48ECA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1393 | 0x0048F380 | `sub_48F380` | 有正文 | `src/ui/WindowRuntime.cpp:43884 — Sub_48F380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1394 | 0x0048F410 | `sub_48F410` | 有正文 | `src/ui/WindowRuntime.cpp:43902 — Sub_48F410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1395 | 0x0048F620 | `sub_48F620` | 有正文 | `src/ui/WindowRuntime.cpp:43946 — Sub_48F620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1396 | 0x0048F690 | `sub_48F690` | 有正文 | `src/ui/WindowRuntime.cpp:43968 — Sub_48F690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1397 | 0x0048F6C0 | `sub_48F6C0` | 有正文 | `src/ui/WindowRuntime.cpp:43976 — Sub_48F6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1398 | 0x0048F8F0 | `sub_48F8F0` | 有正文 | `src/ui/WindowRuntime.cpp:42893 — Sub_48F8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1399 | 0x00490780 | `sub_490780` | 有正文 | `src/ui/WindowRuntime.cpp:43088 — Sub_490780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1400 | 0x00491AA0 | `sub_491AA0` | 有正文 | `src/ui/WindowRuntime.cpp:43242 — Sub_491AA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1401 | 0x004920F0 | `sub_4920F0` | 有正文 | `src/ui/WindowRuntime.cpp:43328 — Sub_4920F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1402 | 0x00492250 | `sub_492250` | 有正文 | `src/world/SceneState.cpp:4357 — Sub_492250` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1403 | 0x00492260 | `sub_492260` | 有正文 | `src/ui/WindowRuntime.cpp:42840 — Sub_492260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1404 | 0x00492270 | `sub_492270` | 有正文 | `src/ui/WindowRuntime.cpp:42846 — Sub_492270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1405 | 0x00492280 | `sub_492280` | 有正文 | `src/ui/WindowRuntime.cpp:42852 — Sub_492280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1406 | 0x00492350 | `sub_492350` | 有正文 | `src/ui/WindowRuntime.cpp:42867 — Sub_492350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1407 | 0x00492390 | `sub_492390` | 有正文 | `src/ui/WindowRuntime.cpp:42879 — Sub_492390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1408 | 0x004923D0 | `sub_4923D0` | 有正文 | `src/ui/WindowRuntime.cpp:16331 — Sub_4923D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1409 | 0x004923F0 | `sub_4923F0` | 有正文 | `src/ui/WindowRuntime.cpp:16407 — Sub_4923F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1410 | 0x00492410 | `sub_492410` | 有正文 | `src/ui/WindowRuntime.cpp:16712 — Sub_492410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1411 | 0x00492430 | `sub_492430` | 有正文 | `src/ui/WindowRuntime.cpp:16417 — Sub_492430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1412 | 0x00492520 | `sub_492520` | 有正文 | `src/ui/WindowRuntime.cpp:16445 — Sub_492520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1413 | 0x00492840 | `sub_492840` | 有正文 | `src/ui/WindowRuntime.cpp:16527 — Sub_492840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1414 | 0x00492880 | `sub_492880` | 有正文 | `src/ui/WindowRuntime.cpp:16722 — Sub_492880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1415 | 0x00492C80 | `sub_492C80` | 有正文 | `src/ui/WindowRuntime.cpp:16341 — Sub_492C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1416 | 0x00492F70 | `sub_492F70` | 有正文 | `src/ui/WindowRuntime.cpp:16574 — Sub_492F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1417 | 0x004939A0 | `sub_4939A0` | 有正文 | `src/ui/WindowRuntime.cpp:16551 — Sub_4939A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1418 | 0x00493A70 | `sub_493A70` | 有正文 | `src/ui/WindowRuntime.cpp:16829 — Sub_493A70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1419 | 0x00493AF0 | `sub_493AF0` | 有正文 | `src/ui/WindowRuntime.cpp:16848 — Sub_493AF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1420 | 0x00493B70 | `sub_493B70` | 有正文 | `src/ui/WindowRuntime.cpp:16868 — Sub_493B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1421 | 0x00493CB0 | `sub_493CB0` | 有正文 | `src/ui/WindowRuntime.cpp:16902 — Sub_493CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1422 | 0x00493D60 | `sub_493D60` | 有正文 | `src/ui/WindowRuntime.cpp:16927 — Sub_493D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1423 | 0x00493DB0 | `sub_493DB0` | 有正文 | `src/ui/WindowRuntime.cpp:16941 — Sub_493DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1424 | 0x00493DF9 | `sub_493DF9` | 有承接 | `src/ui/WindowRuntime.cpp:16941 — Sub_493DB0` | 原绘图／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 1425 | 0x00493E10 | `sub_493E10` | 有正文 | `src/ui/WindowRuntime.cpp:16963 — Sub_493E10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1426 | 0x00493E50 | `sub_493E50` | 有正文 | `src/ui/WindowRuntime.cpp:17138 — Sub_493E50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1427 | 0x00493EE0 | `sub_493EE0` | 有正文 | `src/ui/WindowRuntime.cpp:16976 — Sub_493EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1428 | 0x00494000 | `sub_494000` | 有正文 | `src/ui/WindowRuntime.cpp:17009 — Sub_494000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1429 | 0x00494180 | `sub_494180` | 有正文 | `src/ui/WindowRuntime.cpp:17054 — Sub_494180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1430 | 0x00494370 | `sub_494370` | 有正文 | `src/ui/WindowRuntime.cpp:17100 — Sub_494370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1431 | 0x004944A0 | `sub_4944A0` | 有正文 | `src/ui/WindowRuntime.cpp:17161 — Sub_4944A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1432 | 0x00494550 | `sub_494550` | 有正文 | `src/ui/WindowRuntime.cpp:17187 — Sub_494550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1433 | 0x00494620 | `sub_494620` | 有正文 | `src/ui/WindowRuntime.cpp:17217 — Sub_494620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1434 | 0x00494720 | `sub_494720` | 有正文 | `src/ui/WindowRuntime.cpp:17249 — Sub_494720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1435 | 0x004947F0 | `sub_4947F0` | 有正文 | `src/ui/WindowRuntime.cpp:17278 — Sub_4947F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1436 | 0x004948F0 | `sub_4948F0` | 有正文 | `src/ui/WindowRuntime.cpp:17310 — Sub_4948F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1437 | 0x004949C0 | `sub_4949C0` | 有正文 | `src/ui/WindowRuntime.cpp:17336 — Sub_4949C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1438 | 0x00494AE0 | `sub_494AE0` | 有正文 | `src/ui/WindowRuntime.cpp:17373 — Sub_494AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1439 | 0x00494C10 | `sub_494C10` | 有正文 | `src/ui/WindowRuntime.cpp:17410 — Sub_494C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1440 | 0x00494D30 | `sub_494D30` | 有正文 | `src/ui/WindowRuntime.cpp:17447 — Sub_494D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1441 | 0x00494E60 | `sub_494E60` | 有正文 | `src/ui/WindowRuntime.cpp:17484 — Sub_494E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1442 | 0x00494F30 | `sub_494F30` | 有正文 | `src/ui/WindowRuntime.cpp:17514 — Sub_494F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1443 | 0x00495000 | `sub_495000` | 有正文 | `src/ui/WindowRuntime.cpp:17544 — Sub_495000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1444 | 0x00495130 | `sub_495130` | 有正文 | `src/ui/WindowRuntime.cpp:17750 — Sub_495130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1445 | 0x004951C0 | `sub_4951C0` | 有正文 | `src/ui/WindowRuntime.cpp:17770 — Sub_4951C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1446 | 0x004953B0 | `sub_4953B0` | 有正文 | `src/ui/WindowRuntime.cpp:17850 — Sub_4953B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1447 | 0x00495450 | `sub_495450` | 有正文 | `src/ui/WindowRuntime.cpp:17869 — Sub_495450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1448 | 0x00495500 | `sub_495500` | 有正文 | `src/ui/WindowRuntime.cpp:17901 — Sub_495500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1449 | 0x004955C0 | `sub_4955C0` | 有正文 | `src/ui/WindowRuntime.cpp:17936 — Sub_4955C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1450 | 0x00495670 | `sub_495670` | 有正文 | `src/ui/WindowRuntime.cpp:17971 — Sub_495670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1451 | 0x004957B0 | `sub_4957B0` | 有正文 | `src/ui/WindowRuntime.cpp:18026 — Sub_4957B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1452 | 0x004958D0 | `sub_4958D0` | 有正文 | `src/ui/WindowRuntime.cpp:18067 — Sub_4958D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1453 | 0x004959F0 | `sub_4959F0` | 有正文 | `src/ui/WindowRuntime.cpp:18108 — Sub_4959F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1454 | 0x00495B10 | `sub_495B10` | 有正文 | `src/ui/WindowRuntime.cpp:18149 — Sub_495B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1455 | 0x00495C20 | `sub_495C20` | 有正文 | `src/ui/WindowRuntime.cpp:18189 — Sub_495C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1456 | 0x00495D70 | `sub_495D70` | 有正文 | `src/ui/WindowRuntime.cpp:41187 — Sub_495D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1457 | 0x00495F00 | `sub_495F00` | 有正文 | `src/ui/WindowRuntime.cpp:41253 — Sub_495F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1458 | 0x00495F50 | `sub_495F50` | 有正文 | `src/ui/WindowRuntime.cpp:41273 — Sub_495F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1459 | 0x00495F70 | `sub_495F70` | 有正文 | `src/ui/WindowRuntime.cpp:41282 — Sub_495F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1460 | 0x00495FC0 | `sub_495FC0` | 有正文 | `src/ui/WindowRuntime.cpp:41294 — Sub_495FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1461 | 0x00495FE0 | `sub_495FE0` | 有正文 | `src/ui/WindowRuntime.cpp:41301 — Sub_495FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1462 | 0x00496570 | `sub_496570` | 有正文 | `src/ui/WindowRuntime.cpp:37649 — Sub_496570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1463 | 0x004965D0 | `sub_4965D0` | 有正文 | `src/ui/WindowRuntime.cpp:37664 — Sub_4965D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1464 | 0x004966B0 | `BattleScene::SetCommandType` | 有正文 | `src/ui/WindowRuntime.cpp:36094 — Sub_4966B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1465 | 0x00496AE0 | `BattleScene::closeAllWin` | 有正文 | `src/ui/WindowRuntime.cpp:36326 — Sub_496AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1466 | 0x00496B90 | `sub_496B90` | 有正文 | `src/ui/WindowRuntime.cpp:36193 — Sub_496B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1467 | 0x00496D50 | `sub_496D50` | 有正文 | `src/ui/WindowRuntime.cpp:36264 — Sub_496D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1468 | 0x00496F10 | `BattleScene::CloseAllSubWindow` | 有正文 | `src/ui/WindowRuntime.cpp:36368 — Sub_496F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1469 | 0x00496FB0 | `sub_496FB0` | 有正文 | `src/ui/WindowRuntime.cpp:36722 — Sub_496FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1470 | 0x00497000 | `sub_497000` | 有正文 | `src/ui/WindowRuntime.cpp:36825 — Sub_497000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1471 | 0x004970B0 | `sub_4970B0` | 有正文 | `src/ui/WindowRuntime.cpp:36739 — Sub_4970B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1472 | 0x00497100 | `sub_497100` | 有正文 | `src/ui/WindowRuntime.cpp:36756 — Sub_497100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1473 | 0x004971C0 | `sub_4971C0` | 有正文 | `src/ui/WindowRuntime.cpp:36783 — Sub_4971C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1474 | 0x00497210 | `sub_497210` | 有正文 | `src/ui/WindowRuntime.cpp:36800 — Sub_497210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1475 | 0x004972B0 | `sub_4972B0` | 有正文 | `src/ui/WindowRuntime.cpp:36401 — Sub_4972B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1476 | 0x004973C0 | `sub_4973C0` | 有正文 | `src/ui/WindowRuntime.cpp:36439 — Sub_4973C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1477 | 0x004974C0 | `sub_4974C0` | 有正文 | `src/ui/WindowRuntime.cpp:36852 — Sub_4974C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1478 | 0x004975A0 | `sub_4975A0` | 有正文 | `src/ui/WindowRuntime.cpp:36884 — Sub_4975A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1479 | 0x00497770 | `sub_497770` | 有正文 | `src/ui/WindowRuntime.cpp:36476 — Sub_497770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1480 | 0x00497900 | `sub_497900` | 有正文 | `src/ui/WindowRuntime.cpp:36530 — Sub_497900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1481 | 0x00497A60 | `sub_497A60` | 有正文 | `src/ui/WindowRuntime.cpp:36583 — Sub_497A60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1482 | 0x00497B80 | `sub_497B80` | 有正文 | `src/ui/WindowRuntime.cpp:36624 — Sub_497B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1483 | 0x00497C80 | `sub_497C80` | 有正文 | `src/ui/WindowRuntime.cpp:36662 — Sub_497C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1484 | 0x00497E30 | `sub_497E30` | 有正文 | `src/ui/WindowRuntime.cpp:36948 — Sub_497E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1485 | 0x00497FF0 | `sub_497FF0` | 有正文 | `src/ui/WindowRuntime.cpp:37000 — Sub_497FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1486 | 0x00498160 | `sub_498160` | 有正文 | `src/ui/WindowRuntime.cpp:37049 — Sub_498160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1487 | 0x004982C0 | `BattleScene::DrawCountDown` | 有正文 | `src/ui/WindowRuntime.cpp:37571 — Sub_4982C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1488 | 0x00498520 | `sub_498520` | 有正文 | `src/ui/WindowRuntime.cpp:37129 — Sub_498520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1489 | 0x004990A0 | `sub_4990A0` | 有正文 | `src/YobiRuntime.cpp:22696 — Sub_4990A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1490 | 0x004990F0 | `sub_4990F0` | 有正文 | `src/ui/WindowRuntime.cpp:39823 — Sub_4990F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1491 | 0x004991C0 | `sub_4991C0` | 有正文 | `src/ui/WindowRuntime.cpp:39852 — Sub_4991C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1492 | 0x004996C0 | `sub_4996C0` | 有正文 | `src/ui/WindowRuntime.cpp:39969 — Sub_4996C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1493 | 0x00499750 | `sub_499750` | 有正文 | `src/ui/WindowRuntime.cpp:39992 — Sub_499750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1494 | 0x00499790 | `sub_499790` | 有正文 | `src/ui/WindowRuntime.cpp:40004 — Sub_499790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1495 | 0x00499830 | `sub_499830` | 有正文 | `src/ui/WindowRuntime.cpp:40045 — Sub_499830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1496 | 0x00499880 | `sub_499880` | 有正文 | `src/ui/WindowRuntime.cpp:40025 — Sub_499880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1497 | 0x004998F0 | `sub_4998F0` | 有承接 | `src/ui/WindowRuntime.cpp:45557 — SecondItemSelectorStartup::SecondItemSelectorStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 1498 | 0x00499900 | `unknown_libname_45` | 有正文 | `src/ui/WindowRuntime.cpp:45540 — ConstructLegacySecondItemSelector_499900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1499 | 0x00499920 | `unknown_libname_46` | 有正文 | `src/ui/WindowRuntime.cpp:45546 — ReleaseLegacySecondItemSelectorAtExit_499920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1500 | 0x00499930 | `sub_499930` | 有正文 | `src/ui/WindowRuntime.cpp:10625 — Sub_499930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1501 | 0x00499950 | `sub_499950` | 有正文 | `src/ui/WindowRuntime.cpp:10631 — Sub_499950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1502 | 0x00499970 | `sub_499970` | 有正文 | `src/ui/WindowRuntime.cpp:10637 — Sub_499970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1503 | 0x00499990 | `sub_499990` | 有正文 | `src/ui/WindowRuntime.cpp:10704 — Sub_499990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1504 | 0x004999D0 | `sub_4999D0` | 有正文 | `src/ui/WindowRuntime.cpp:10717 — Sub_4999D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1505 | 0x00499B00 | `sub_499B00` | 有正文 | `src/ui/WindowRuntime.cpp:10644 — Sub_499B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1506 | 0x00499B20 | `sub_499B20` | 有正文 | `src/ui/WindowRuntime.cpp:10651 — Sub_499B20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1507 | 0x00499B40 | `sub_499B40` | 有正文 | `src/ui/WindowRuntime.cpp:10658 — Sub_499B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1508 | 0x00499B60 | `sub_499B60` | 有正文 | `src/ui/WindowRuntime.cpp:10666 — Sub_499B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1509 | 0x00499C30 | `sub_499C30` | 有正文 | `src/ui/WindowRuntime.cpp:10759 — Sub_499C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1510 | 0x0049AC40 | `sub_49AC40` | 有正文 | `src/ui/WindowRuntime.cpp:11205 — Sub_49AC40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1511 | 0x0049B610 | `sub_49B610` | 有正文 | `src/ui/WindowRuntime.cpp:11198 — Sub_49B610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1512 | 0x0049B630 | `sub_49B630` | 有正文 | `src/ui/WindowRuntime.cpp:37693 — Sub_49B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1513 | 0x0049B670 | `sub_49B670` | 有正文 | `src/ui/WindowRuntime.cpp:37732 — Sub_49B670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1514 | 0x0049BA10 | `sub_49BA10` | 有正文 | `src/ui/WindowRuntime.cpp:37703 — Sub_49BA10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1515 | 0x0049BB00 | `sub_49BB00` | 有正文 | `src/ui/WindowRuntime.cpp:37803 — Sub_49BB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1516 | 0x0049BD40 | `sub_49BD40` | 有正文 | `src/ui/WindowRuntime.cpp:30952 — Sub_49BD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1517 | 0x0049BDB0 | `sub_49BDB0` | 有正文 | `src/ui/WindowRuntime.cpp:30973 — Sub_49BDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1518 | 0x0049BE00 | `sub_49BE00` | 有正文 | `src/YobiRuntime.cpp:1579 — DrawNineGrid_49BE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1519 | 0x0049C4D0 | `sub_49C4D0` | 有正文 | `src/ui/WindowRuntime.cpp:9193 — Sub_49C4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1520 | 0x0049C510 | `sub_49C510` | 有正文 | `src/ui/WindowRuntime.cpp:9208 — Sub_49C510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1521 | 0x0049C5C0 | `sub_49C5C0` | 有正文 | `src/YobiRuntime.cpp:34494 — Sub_49C5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1522 | 0x0049C5E0 | `sub_49C5E0` | 有正文 | `src/YobiRuntime.cpp:34500 — Sub_49C5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1523 | 0x0049C600 | `Window::CreateWithAnime` | 有正文 | `src/ui/WindowRuntime.cpp:9241 — Sub_49C600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1524 | 0x0049C710 | `sub_49C710` | 有正文 | `src/ui/WindowRuntime.cpp:9286 — Sub_49C710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1525 | 0x0049CAD0 | `sub_49CAD0` | 有正文 | `src/ui/WindowRuntime.cpp:9381 — Sub_49CAD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1526 | 0x0049CDF0 | `sub_49CDF0` | 有正文 | `src/ui/WindowRuntime.cpp:9507 — Sub_49CDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1527 | 0x0049CE70 | `sub_49CE70` | 有正文 | `src/ui/WindowRuntime.cpp:9523 — Sub_49CE70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1528 | 0x0049CF70 | `sub_49CF70` | 有正文 | `src/ui/WindowRuntime.cpp:9568 — Sub_49CF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1529 | 0x0049D2E0 | `World::OpenDialog` | 有正文 | `src/ui/WindowRuntime.cpp:9804 — Sub_49D2E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1530 | 0x0049D8F0 | `sub_49D8F0` | 有正文 | `src/ui/WindowRuntime.cpp:10015 — Sub_49D8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1531 | 0x0049DA30 | `sub_49DA30` | 有正文 | `src/ui/WindowRuntime.cpp:9683 — Sub_49DA30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1532 | 0x0049DDB0 | `sub_49DDB0` | 有正文 | `src/ui/WindowRuntime.cpp:9742 — Sub_49DDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1533 | 0x0049DF30 | `sub_49DF30` | 有正文 | `src/ui/WindowRuntime.cpp:10083 — Sub_49DF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1534 | 0x0049E1D0 | `sub_49E1D0` | 有正文 | `src/ui/WindowRuntime.cpp:10162 — Sub_49E1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1535 | 0x0049E400 | `sub_49E400` | 有正文 | `src/ui/WindowRuntime.cpp:10220 — Sub_49E400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1536 | 0x0049E750 | `sub_49E750` | 有正文 | `src/ui/WindowRuntime.cpp:30920 — Sub_49E750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1537 | 0x0049E800 | `sub_49E800` | 有正文 | `src/ui/WindowRuntime.cpp:12881 — Sub_49E800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1538 | 0x0049E8F0 | `sub_49E8F0` | 有正文 | `src/ui/WindowRuntime.cpp:40159 — Sub_49E8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1539 | 0x0049E990 | `sub_49E990` | 有正文 | `src/ui/WindowRuntime.cpp:40150 — Sub_49E990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1540 | 0x0049EAA0 | `sub_49EAA0` | 有正文 | `src/ui/WindowRuntime.cpp:40177 — Sub_49EAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1541 | 0x0049EB50 | `sub_49EB50` | 有正文 | `src/ui/WindowRuntime.cpp:40305 — Sub_49EB50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1542 | 0x0049EC00 | `sub_49EC00` | 有正文 | `src/ui/WindowRuntime.cpp:40334 — Sub_49EC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1543 | 0x0049ECC0 | `sub_49ECC0` | 有正文 | `src/ui/WindowRuntime.cpp:40362 — Sub_49ECC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1544 | 0x0049ED80 | `sub_49ED80` | 有正文 | `src/ui/WindowRuntime.cpp:40389 — Sub_49ED80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1545 | 0x0049EE60 | `sub_49EE60` | 有正文 | `src/ui/WindowRuntime.cpp:40423 — Sub_49EE60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1546 | 0x0049EEB0 | `sub_49EEB0` | 有正文 | `src/ui/WindowRuntime.cpp:40436 — Sub_49EEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1547 | 0x0049EF60 | `sub_49EF60` | 有正文 | `src/ui/WindowRuntime.cpp:40478 — Sub_49EF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1548 | 0x0049F000 | `sub_49F000` | 有正文 | `src/ui/WindowRuntime.cpp:39482 — Sub_49F000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1549 | 0x0049F040 | `sub_49F040` | 有正文 | `src/ui/WindowRuntime.cpp:39494 — Sub_49F040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1550 | 0x0049F060 | `sub_49F060` | 有正文 | `src/ui/WindowRuntime.cpp:39503 — Sub_49F060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1551 | 0x0049F0A0 | `sub_49F0A0` | 有正文 | `src/ui/WindowRuntime.cpp:39516 — Sub_49F0A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1552 | 0x0049F1A0 | `sub_49F1A0` | 有正文 | `src/ui/WindowRuntime.cpp:39536 — Sub_49F1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1553 | 0x0049F260 | `sub_49F260` | 有正文 | `src/ui/WindowRuntime.cpp:39561 — Sub_49F260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1554 | 0x0049F310 | `sub_49F310` | 有正文 | `src/ui/WindowRuntime.cpp:39590 — Sub_49F310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1555 | 0x0049F3D0 | `sub_49F3D0` | 有正文 | `src/ui/WindowRuntime.cpp:39618 — Sub_49F3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1556 | 0x0049F490 | `sub_49F490` | 有正文 | `src/ui/WindowRuntime.cpp:39645 — Sub_49F490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1557 | 0x0049F570 | `sub_49F570` | 有正文 | `src/ui/WindowRuntime.cpp:39679 — Sub_49F570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1558 | 0x0049F5C0 | `sub_49F5C0` | 有正文 | `src/ui/WindowRuntime.cpp:39692 — Sub_49F5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1559 | 0x0049F680 | `sub_49F680` | 有正文 | `src/ui/WindowRuntime.cpp:39723 — Sub_49F680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1560 | 0x0049F760 | `sub_49F760` | 有正文 | `src/ui/WindowRuntime.cpp:39751 — Sub_49F760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1561 | 0x0049F790 | `sub_49F790` | 有正文 | `src/ui/WindowRuntime.cpp:39762 — Sub_49F790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1562 | 0x0049F7B0 | `sub_49F7B0` | 有正文 | `src/ui/WindowRuntime.cpp:39771 — Sub_49F7B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1563 | 0x0049F7E0 | `sub_49F7E0` | 有正文 | `src/ui/WindowRuntime.cpp:39781 — Sub_49F7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1564 | 0x0049FA40 | `sub_49FA40` | 有正文 | `src/ui/WindowRuntime.cpp:40061 — Sub_49FA40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1565 | 0x0049FB00 | `sub_49FB00` | 有正文 | `src/ui/WindowRuntime.cpp:40092 — Sub_49FB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1566 | 0x0049FBC0 | `sub_49FBC0` | 有正文 | `src/ui/WindowRuntime.cpp:40123 — Sub_49FBC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1567 | 0x0049FCA0 | `sub_49FCA0` | 有正文 | `src/ui/WindowRuntime.cpp:10284 — Sub_49FCA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1568 | 0x0049FD20 | `sub_49FD20` | 有正文 | `src/ui/WindowRuntime.cpp:10311 — Sub_49FD20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1569 | 0x0049FE40 | `sub_49FE40` | 有正文 | `src/ui/WindowRuntime.cpp:10337 — Sub_49FE40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1570 | 0x0049FF70 | `sub_49FF70` | 有正文 | `src/ui/WindowRuntime.cpp:10371 — Sub_49FF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1571 | 0x0049FFA0 | `sub_49FFA0` | 有正文 | `src/ui/WindowRuntime.cpp:10379 — Sub_49FFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1572 | 0x004A00C0 | `sub_4A00C0` | 有正文 | `src/ui/WindowRuntime.cpp:10412 — Sub_4A00C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1573 | 0x004A0140 | `sub_4A0140` | 有正文 | `src/ui/WindowRuntime.cpp:10432 — Sub_4A0140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1574 | 0x004A0280 | `sub_4A0280` | 有正文 | `src/ui/WindowRuntime.cpp:10473 — Sub_4A0280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1575 | 0x004A0300 | `sub_4A0300` | 有正文 | `src/ui/WindowRuntime.cpp:10521 — Sub_4A0300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1576 | 0x004A0420 | `sub_4A0420` | 有正文 | `src/ui/WindowRuntime.cpp:10484 — Sub_4A0420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1577 | 0x004A04A0 | `sub_4A04A0` | 有正文 | `src/ui/WindowRuntime.cpp:10548 — Sub_4A04A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1578 | 0x004A05C0 | `sub_4A05C0` | 有正文 | `src/ui/WindowRuntime.cpp:10495 — Sub_4A05C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1579 | 0x004A0640 | `sub_4A0640` | 有正文 | `src/ui/WindowRuntime.cpp:10575 — Sub_4A0640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1580 | 0x004A0700 | `sub_4A0700` | 有正文 | `src/ui/WindowRuntime.cpp:10506 — Sub_4A0700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1581 | 0x004A0790 | `sub_4A0790` | 有正文 | `src/ui/WindowRuntime.cpp:10590 — Sub_4A0790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1582 | 0x004A0850 | `sub_4A0850` | 有正文 | `src/ui/WindowRuntime.cpp:10605 — Sub_4A0850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1583 | 0x004A0940 | `sub_4A0940` | 有正文 | `src/ui/WindowRuntime.cpp:29552 — Sub_4A0940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1584 | 0x004A0970 | `sub_4A0970` | 有正文 | `src/ui/WindowRuntime.cpp:25826 — Sub_4A0970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1585 | 0x004A09B0 | `sub_4A09B0` | 有正文 | `src/ui/WindowRuntime.cpp:25841 — Sub_4A09B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1586 | 0x004A09E0 | `sub_4A09E0` | 有正文 | `src/ui/WindowRuntime.cpp:25851 — Sub_4A09E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1587 | 0x004A1690 | `sub_4A1690` | 有正文 | `src/ui/WindowRuntime.cpp:26086 — Sub_4A1690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1588 | 0x004A16B0 | `sub_4A16B0` | 有正文 | `src/ui/WindowRuntime.cpp:26093 — Sub_4A16B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1589 | 0x004A1780 | `sub_4A1780` | 有正文 | `src/ui/WindowRuntime.cpp:26120 — Sub_4A1780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1590 | 0x004A1C30 | `sub_4A1C30` | 有正文 | `src/ui/WindowRuntime.cpp:26185 — Sub_4A1C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1591 | 0x004A1EF0 | `sub_4A1EF0` | 有正文 | `src/ui/WindowRuntime.cpp:26230 — Sub_4A1EF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1592 | 0x004A2610 | `sub_4A2610` | 有正文 | `src/ui/WindowRuntime.cpp:26322 — Sub_4A2610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1593 | 0x004A2700 | `sub_4A2700` | 有正文 | `src/ui/WindowRuntime.cpp:28560 — Sub_4A2700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1594 | 0x004A2870 | `sub_4A2870` | 有正文 | `src/ui/WindowRuntime.cpp:26352 — Sub_4A2870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1595 | 0x004A2B00 | `sub_4A2B00` | 有正文 | `src/ui/WindowRuntime.cpp:26504 — Sub_4A2B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1596 | 0x004A2BD0 | `sub_4A2BD0` | 有正文 | `src/ui/WindowRuntime.cpp:26394 — Sub_4A2BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1597 | 0x004A2C60 | `sub_4A2C60` | 有正文 | `src/ui/WindowRuntime.cpp:28627 — Sub_4A2C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1598 | 0x004A2CC0 | `sub_4A2CC0` | 有正文 | `src/ui/WindowRuntime.cpp:28649 — Sub_4A2CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1599 | 0x004A3010 | `sub_4A3010` | 有正文 | `src/ui/WindowRuntime.cpp:28722 — Sub_4A3010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1600 | 0x004A3070 | `sub_4A3070` | 有正文 | `src/ui/WindowRuntime.cpp:28736 — Sub_4A3070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1601 | 0x004A30D0 | `sub_4A30D0` | 有正文 | `src/ui/WindowRuntime.cpp:28750 — Sub_4A30D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1602 | 0x004A3100 | `sub_4A3100` | 有正文 | `src/ui/WindowRuntime.cpp:28766 — Sub_4A3100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1603 | 0x004A3160 | `sub_4A3160` | 有正文 | `src/ui/WindowRuntime.cpp:28780 — Sub_4A3160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1604 | 0x004A3190 | `sub_4A3190` | 有正文 | `src/ui/WindowRuntime.cpp:28796 — Sub_4A3190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1605 | 0x004A31C0 | `sub_4A31C0` | 有正文 | `src/ui/WindowRuntime.cpp:28812 — Sub_4A31C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1606 | 0x004A31E0 | `sub_4A31E0` | 有正文 | `src/ui/WindowRuntime.cpp:28821 — Sub_4A31E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1607 | 0x004A3230 | `sub_4A3230` | 有正文 | `src/ui/WindowRuntime.cpp:28839 — Sub_4A3230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1608 | 0x004A3260 | `sub_4A3260` | 有正文 | `src/ui/WindowRuntime.cpp:28855 — Sub_4A3260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1609 | 0x004A32B0 | `sub_4A32B0` | 有正文 | `src/ui/WindowRuntime.cpp:29539 — Sub_4A32B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1610 | 0x004A32E0 | `sub_4A32E0` | 有正文 | `src/ui/WindowRuntime.cpp:26715 — Sub_4A32E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1611 | 0x004A33C0 | `sub_4A33C0` | 有正文 | `src/ui/WindowRuntime.cpp:26753 — Sub_4A33C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1612 | 0x004A3480 | `sub_4A3480` | 有正文 | `src/ui/WindowRuntime.cpp:26763 — Sub_4A3480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1613 | 0x004A3540 | `sub_4A3540` | 有正文 | `src/ui/WindowRuntime.cpp:26773 — Sub_4A3540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1614 | 0x004A3600 | `sub_4A3600` | 有正文 | `src/ui/WindowRuntime.cpp:26788 — Sub_4A3600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1615 | 0x004A3660 | `sub_4A3660` | 有正文 | `src/ui/WindowRuntime.cpp:26804 — Sub_4A3660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1616 | 0x004A36B0 | `sub_4A36B0` | 有正文 | `src/ui/WindowRuntime.cpp:26817 — Sub_4A36B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1617 | 0x004A3760 | `sub_4A3760` | 有正文 | `src/ui/WindowRuntime.cpp:26832 — Sub_4A3760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1618 | 0x004A3780 | `sub_4A3780` | 有正文 | `src/ui/WindowRuntime.cpp:26539 — Sub_4A3780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1619 | 0x004A37E0 | `sub_4A37E0` | 有正文 | `src/ui/WindowRuntime.cpp:26415 — Sub_4A37E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1620 | 0x004A3820 | `sub_4A3820` | 有正文 | `src/ui/WindowRuntime.cpp:29504 — Sub_4A3820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1621 | 0x004A38C0 | `sub_4A38C0` | 有正文 | `src/ui/WindowRuntime.cpp:26486 — Sub_4A38C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1622 | 0x004A3950 | `sub_4A3950` | 有正文 | `src/ui/WindowRuntime.cpp:29477 — Sub_4A3950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1623 | 0x004A3960 | `sub_4A3960` | 有正文 | `src/ui/WindowRuntime.cpp:29483 — Sub_4A3960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1624 | 0x004A3990 | `sub_4A3990` | 有正文 | `src/ui/WindowRuntime.cpp:29494 — Sub_4A3990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1625 | 0x004A39C0 | `sub_4A39C0` | 有正文 | `src/ui/WindowRuntime.cpp:26554 — Sub_4A39C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1626 | 0x004A3C70 | `sub_4A3C70` | 有正文 | `src/ui/WindowRuntime.cpp:26663 — Sub_4A3C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1627 | 0x004A40C0 | `sub_4A40C0` | 有正文 | `src/ui/WindowRuntime.cpp:26428 — Sub_4A40C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1628 | 0x004A4180 | `sub_4A4180` | 有正文 | `src/ui/WindowRuntime.cpp:26950 — Sub_4A4180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1629 | 0x004A41A0 | `sub_4A41A0` | 有正文 | `src/ui/WindowRuntime.cpp:26959 — Sub_4A41A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1630 | 0x004A4210 | `sub_4A4210` | 有正文 | `src/ui/WindowRuntime.cpp:1891 — Sub_4A4210` | 原入口的 RET／固定返回已用当前 IDA 核实；不是业务占位；完整接线未验 |
| 1631 | 0x004A4220 | `sub_4A4220` | 有正文 | `src/ui/WindowRuntime.cpp:26976 — Sub_4A4220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1632 | 0x004A42C0 | `sub_4A42C0` | 有正文 | `src/ui/WindowRuntime.cpp:26995 — Sub_4A42C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1633 | 0x004A4370 | `sub_4A4370` | 有正文 | `src/ui/WindowRuntime.cpp:27019 — Sub_4A4370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1634 | 0x004A4420 | `sub_4A4420` | 有正文 | `src/ui/WindowRuntime.cpp:27043 — Sub_4A4420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1635 | 0x004A4540 | `sub_4A4540` | 有正文 | `src/ui/WindowRuntime.cpp:27076 — Sub_4A4540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1636 | 0x004A45E0 | `sub_4A45E0` | 有正文 | `src/ui/WindowRuntime.cpp:27093 — Sub_4A45E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1637 | 0x004A4620 | `sub_4A4620` | 有正文 | `src/ui/WindowRuntime.cpp:27153 — Sub_4A4620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1638 | 0x004A4650 | `sub_4A4650` | 有正文 | `src/ui/WindowRuntime.cpp:27166 — Sub_4A4650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1639 | 0x004A4660 | `sub_4A4660` | 有正文 | `src/ui/WindowRuntime.cpp:27174 — Sub_4A4660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1640 | 0x004A4710 | `sub_4A4710` | 有正文 | `src/ui/WindowRuntime.cpp:27201 — Sub_4A4710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1641 | 0x004A47C0 | `sub_4A47C0` | 有正文 | `src/ui/WindowRuntime.cpp:27228 — Sub_4A47C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1642 | 0x004A48A0 | `sub_4A48A0` | 有正文 | `src/ui/WindowRuntime.cpp:27264 — Sub_4A48A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1643 | 0x004A49B0 | `sub_4A49B0` | 有正文 | `src/ui/WindowRuntime.cpp:27315 — Sub_4A49B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1644 | 0x004A4FC0 | `sub_4A4FC0` | 有正文 | `src/ui/WindowRuntime.cpp:27394 — Sub_4A4FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1645 | 0x004A5390 | `sub_4A5390` | 有正文 | `src/ui/WindowRuntime.cpp:25501 — Sub_4A5390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1646 | 0x004A53D0 | `sub_4A53D0` | 有正文 | `src/ui/WindowRuntime.cpp:15744 — Sub_4A53D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1647 | 0x004A53F0 | `sub_4A53F0` | 有正文 | `src/ui/WindowRuntime.cpp:27471 — Sub_4A53F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1648 | 0x004A5650 | `sub_4A5650` | 有正文 | `src/ui/WindowRuntime.cpp:27566 — Sub_4A5650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1649 | 0x004A56F0 | `sub_4A56F0` | 有正文 | `src/ui/WindowRuntime.cpp:27823 — Sub_4A56F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1650 | 0x004A58E0 | `sub_4A58E0` | 有正文 | `src/ui/WindowRuntime.cpp:27595 — Sub_4A58E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1651 | 0x004A5970 | `sub_4A5970` | 有正文 | `src/ui/WindowRuntime.cpp:27619 — Sub_4A5970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1652 | 0x004A6160 | `sub_4A6160` | 有正文 | `src/ui/WindowRuntime.cpp:27780 — Sub_4A6160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1653 | 0x004A6300 | `sub_4A6300` | 有正文 | `src/ui/WindowRuntime.cpp:27892 — Sub_4A6300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1654 | 0x004A6450 | `sub_4A6450` | 有正文 | `src/ui/WindowRuntime.cpp:27976 — Sub_4A6450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1655 | 0x004A6AC0 | `sub_4A6AC0` | 有正文 | `src/ui/WindowRuntime.cpp:28096 — Sub_4A6AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1656 | 0x004A6D00 | `sub_4A6D00` | 有正文 | `src/ui/WindowRuntime.cpp:1024 — Sub_4A6D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1657 | 0x004A6D30 | `sub_4A6D30` | 有正文 | `src/ui/WindowRuntime.cpp:27961 — Sub_4A6D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1658 | 0x004A6D50 | `sub_4A6D50` | 有正文 | `src/ui/WindowRuntime.cpp:27967 — Sub_4A6D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1659 | 0x004A6D80 | `sub_4A6D80` | 有正文 | `src/ui/WindowRuntime.cpp:30090 — Sub_4A6D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1660 | 0x004A6DB0 | `sub_4A6DB0` | 有正文 | `src/ui/WindowRuntime.cpp:30102 — Sub_4A6DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1661 | 0x004A6DD0 | `sub_4A6DD0` | 有正文 | `src/ui/WindowRuntime.cpp:30318 — Sub_4A6DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1662 | 0x004A6EA0 | `sub_4A6EA0` | 有正文 | `src/ui/WindowRuntime.cpp:30556 — Sub_4A6EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1663 | 0x004A6FC0 | `sub_4A6FC0` | 有正文 | `src/ui/WindowRuntime.cpp:30349 — Sub_4A6FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1664 | 0x004A7590 | `sub_4A7590` | 有正文 | `src/ui/WindowRuntime.cpp:30726 — Sub_4A7590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1665 | 0x004A7640 | `sub_4A7640` | 有正文 | `src/ui/WindowRuntime.cpp:30738 — Sub_4A7640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1666 | 0x004A76E0 | `sub_4A76E0` | 有正文 | `src/Input.cpp:1419 — Sub_4A76E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1667 | 0x004A7710 | `___setargv_13` | 有正文 | `src/ui/WindowRuntime.cpp:30686 — Sub_4A7710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1668 | 0x004A7720 | `sub_4A7720` | 有正文 | `src/ui/WindowRuntime.cpp:30692 — Sub_4A7720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1669 | 0x004A7730 | `___setargv_14` | 有正文 | `src/ui/WindowRuntime.cpp:30698 — Sub_4A7730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1670 | 0x004A7740 | `sub_4A7740` | 有正文 | `src/ui/WindowRuntime.cpp:30704 — Sub_4A7740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1671 | 0x004A7750 | `sub_4A7750` | 有正文 | `src/ui/WindowRuntime.cpp:30747 — Sub_4A7750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1672 | 0x004A7B00 | `sub_4A7B00` | 有正文 | `src/ui/WindowRuntime.cpp:44872 — Sub_4A7B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1673 | 0x004A7C40 | `sub_4A7C40` | 有正文 | `src/ui/WindowRuntime.cpp:30178 — Sub_4A7C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1674 | 0x004A80E0 | `sub_4A80E0` | 有正文 | `src/ui/WindowRuntime.cpp:45021 — ReadChatInputByteWidth_4A80E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1675 | 0x004A80F0 | `sub_4A80F0` | 有正文 | `src/ui/WindowRuntime.cpp:30634 — Sub_4A80F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1676 | 0x004A8140 | `sub_4A8140` | 有正文 | `src/ui/WindowRuntime.cpp:30151 — Sub_4A8140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1677 | 0x004A81D0 | `sub_4A81D0` | 有正文 | `src/ui/WindowRuntime.cpp:30648 — Sub_4A81D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1678 | 0x004A8240 | `sub_4A8240` | 有正文 | `src/ui/WindowRuntime.cpp:30603 — Sub_4A8240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1679 | 0x004A82C0 | `sub_4A82C0` | 有正文 | `src/ui/WindowRuntime.cpp:30624 — Sub_4A82C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1680 | 0x004A82E0 | `sub_4A82E0` | 有正文 | `src/ui/WindowRuntime.cpp:30710 — Sub_4A82E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1681 | 0x004A8330 | `sub_4A8330` | 有正文 | `src/ui/WindowRuntime.cpp:30112 — Sub_4A8330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1682 | 0x004A83A0 | `sub_4A83A0` | 有正文 | `src/ui/WindowRuntime.cpp:13017 — Sub_4A83A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1683 | 0x004A83D0 | `sub_4A83D0` | 有正文 | `src/ui/WindowRuntime.cpp:13569 — Sub_4A83D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1684 | 0x004A84A0 | `sub_4A84A0` | 有正文 | `src/ui/WindowRuntime.cpp:15218 — Sub_4A84A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1685 | 0x004A9020 | `sub_4A9020` | 有正文 | `src/ui/WindowRuntime.cpp:13596 — Sub_4A9020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1686 | 0x004A9160 | `sub_4A9160` | 有正文 | `src/ui/WindowRuntime.cpp:13631 — Sub_4A9160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1687 | 0x004A92D0 | `sub_4A92D0` | 有正文 | `src/ui/WindowRuntime.cpp:13722 — Sub_4A92D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1688 | 0x004A9350 | `sub_4A9350` | 有正文 | `src/ui/WindowRuntime.cpp:14850 — Sub_4A9350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1689 | 0x004A93E0 | `sub_4A93E0` | 有正文 | `src/ui/WindowRuntime.cpp:13742 — Sub_4A93E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1690 | 0x004A94A0 | `sub_4A94A0` | 有正文 | `src/ui/WindowRuntime.cpp:13766 — Sub_4A94A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1691 | 0x004A9560 | `sub_4A9560` | 有正文 | `src/ui/WindowRuntime.cpp:13790 — Sub_4A9560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1692 | 0x004A9580 | `sub_4A9580` | 有正文 | `src/ui/WindowRuntime.cpp:13797 — Sub_4A9580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1693 | 0x004A95A0 | `sub_4A95A0` | 有正文 | `src/ui/WindowRuntime.cpp:13804 — Sub_4A95A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1694 | 0x004A96D0 | `sub_4A96D0` | 有正文 | `src/ui/WindowRuntime.cpp:13834 — Sub_4A96D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1695 | 0x004A9820 | `sub_4A9820` | 有正文 | `src/ui/WindowRuntime.cpp:13867 — Sub_4A9820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1696 | 0x004A9990 | `sub_4A9990` | 有正文 | `src/ui/WindowRuntime.cpp:13901 — Sub_4A9990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1697 | 0x004A9B00 | `sub_4A9B00` | 有正文 | `src/ui/WindowRuntime.cpp:14759 — Sub_4A9B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1698 | 0x004A9D10 | `sub_4A9D10` | 有正文 | `src/ui/WindowRuntime.cpp:13935 — Sub_4A9D10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1699 | 0x004AA3D0 | `sub_4AA3D0` | 有正文 | `src/ui/WindowRuntime.cpp:14114 — Sub_4AA3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1700 | 0x004AA9C0 | `sub_4AA9C0` | 有正文 | `src/ui/WindowRuntime.cpp:14305 — Sub_4AA9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1701 | 0x004AAA70 | `sub_4AAA70` | 有正文 | `src/ui/WindowRuntime.cpp:14335 — Sub_4AAA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1702 | 0x004AAB20 | `sub_4AAB20` | 有正文 | `src/ui/WindowRuntime.cpp:14365 — Sub_4AAB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1703 | 0x004AAB70 | `sub_4AAB70` | 有正文 | `src/ui/WindowRuntime.cpp:6111 — Sub_4AAB70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1704 | 0x004AAEC0 | `sub_4AAEC0` | 有正文 | `src/ui/WindowRuntime.cpp:14379 — Sub_4AAEC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1705 | 0x004AB400 | `sub_4AB400` | 有正文 | `src/ui/WindowRuntime.cpp:6158 — Sub_4AB400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1706 | 0x004AB710 | `sub_4AB710` | 有正文 | `src/ui/WindowRuntime.cpp:14528 — Sub_4AB710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1707 | 0x004ABB20 | `sub_4ABB20` | 有正文 | `src/ui/WindowRuntime.cpp:14663 — Sub_4ABB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1708 | 0x004ABE60 | `sub_4ABE60` | 有正文 | `src/YobiRuntime.cpp:2100 — Sub_4ABE60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1709 | 0x004ABEF0 | `sub_4ABEF0` | 有正文 | `src/YobiRuntime.cpp:2132 — Sub_4ABEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1710 | 0x004AC0C0 | `sub_4AC0C0` | 有正文 | `src/ui/WindowRuntime.cpp:13029 — Sub_4AC0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1711 | 0x004AC0F0 | `sub_4AC0F0` | 有正文 | `src/ui/WindowRuntime.cpp:13676 — Sub_4AC0F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1712 | 0x004AC140 | `sub_4AC140` | 有正文 | `src/ui/WindowRuntime.cpp:13691 — Sub_4AC140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1713 | 0x004AC1B0 | `sub_4AC1B0` | 有正文 | `src/ui/WindowRuntime.cpp:13706 — Sub_4AC1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1714 | 0x004AC1EC | `?classic@locale@std@@SAABV12@XZ_18` | 有承接 | `src/ui/WindowRuntime.cpp:13706 — Sub_4AC1B0` | 原绘图／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 1715 | 0x004AC200 | `sub_4AC200` | 有正文 | `src/ui/WindowRuntime.cpp:14828 — Sub_4AC200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1716 | 0x004AC290 | `sub_4AC290` | 有正文 | `src/ui/WindowRuntime.cpp:14872 — Sub_4AC290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1717 | 0x004AC890 | `sub_4AC890` | 有正文 | `src/ui/WindowRuntime.cpp:15026 — Sub_4AC890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1718 | 0x004ACDF0 | `sub_4ACDF0` | 有正文 | `src/YobiRuntime.cpp:2188 — Sub_4ACDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1719 | 0x004ACE80 | `sub_4ACE80` | 有正文 | `src/YobiRuntime.cpp:2221 — Sub_4ACE80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1720 | 0x004AD060 | `sub_4AD060` | 有正文 | `src/ui/WindowRuntime.cpp:2070 — Sub_4AD060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1721 | 0x004AD090 | `sub_4AD090` | 有正文 | `src/ui/WindowRuntime.cpp:13042 — Sub_4AD090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1722 | 0x004AD0F0 | `sub_4AD0F0` | 有正文 | `src/ui/WindowRuntime.cpp:13049 — Sub_4AD0F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1723 | 0x004AD180 | `sub_4AD180` | 有正文 | `src/ui/WindowRuntime.cpp:13077 — Sub_4AD180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1724 | 0x004AD470 | `sub_4AD470` | 有正文 | `src/ui/WindowRuntime.cpp:13096 — Sub_4AD470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1725 | 0x004AD780 | `sub_4AD780` | 有正文 | `src/ui/WindowRuntime.cpp:13118 — Sub_4AD780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1726 | 0x004AD800 | `sub_4AD800` | 有正文 | `src/ui/WindowRuntime.cpp:13143 — Sub_4AD800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1727 | 0x004AD9F0 | `sub_4AD9F0` | 有正文 | `src/ui/WindowRuntime.cpp:13199 — Sub_4AD9F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1728 | 0x004ADAA0 | `sub_4ADAA0` | 有正文 | `src/ui/WindowRuntime.cpp:13221 — Sub_4ADAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1729 | 0x004ADBD0 | `sub_4ADBD0` | 有正文 | `src/ui/WindowRuntime.cpp:13247 — Sub_4ADBD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1730 | 0x004ADC80 | `sub_4ADC80` | 有正文 | `src/ui/WindowRuntime.cpp:13276 — Sub_4ADC80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1731 | 0x004ADD50 | `sub_4ADD50` | 有正文 | `src/ui/WindowRuntime.cpp:13305 — Sub_4ADD50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1732 | 0x004ADE10 | `sub_4ADE10` | 有正文 | `src/ui/WindowRuntime.cpp:13332 — Sub_4ADE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1733 | 0x004ADEF0 | `sub_4ADEF0` | 有正文 | `src/ui/WindowRuntime.cpp:13362 — Sub_4ADEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1734 | 0x004ADF40 | `sub_4ADF40` | 有正文 | `src/ui/WindowRuntime.cpp:13375 — Sub_4ADF40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1735 | 0x004AE170 | `sub_4AE170` | 有正文 | `src/ui/WindowRuntime.cpp:13420 — Sub_4AE170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1736 | 0x004AE260 | `sub_4AE260` | 有正文 | `src/ui/WindowRuntime.cpp:13455 — Sub_4AE260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1737 | 0x004AE350 | `sub_4AE350` | 有正文 | `src/ui/WindowRuntime.cpp:13479 — Sub_4AE350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1738 | 0x004AE5B0 | `sub_4AE5B0` | 有正文 | `src/ui/WindowRuntime.cpp:13560 — Sub_4AE5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1739 | 0x004AE5F0 | `sub_4AE5F0` | 有正文 | `src/ui/WindowRuntime.cpp:26841 — Sub_4AE5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1740 | 0x004AE620 | `sub_4AE620` | 有正文 | `src/ui/WindowRuntime.cpp:26855 — Sub_4AE620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1741 | 0x004AE690 | `sub_4AE690` | 有正文 | `src/ui/WindowRuntime.cpp:26861 — Sub_4AE690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1742 | 0x004AE700 | `sub_4AE700` | 有正文 | `src/ui/WindowRuntime.cpp:26867 — Sub_4AE700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1743 | 0x004AE770 | `sub_4AE770` | 有正文 | `src/ui/WindowRuntime.cpp:26873 — Sub_4AE770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1744 | 0x004AE7E0 | `sub_4AE7E0` | 有正文 | `src/ui/WindowRuntime.cpp:26879 — Sub_4AE7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1745 | 0x004AE850 | `sub_4AE850` | 有正文 | `src/ui/WindowRuntime.cpp:26885 — Sub_4AE850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1746 | 0x004AE8C0 | `sub_4AE8C0` | 有正文 | `src/ui/WindowRuntime.cpp:26891 — Sub_4AE8C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1747 | 0x004AE8D0 | `sub_4AE8D0` | 有正文 | `src/ui/WindowRuntime.cpp:26899 — Sub_4AE8D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1748 | 0x004AE9C0 | `sub_4AE9C0` | 有正文 | `src/ui/WindowRuntime.cpp:26921 — Sub_4AE9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1749 | 0x004AEA10 | `sub_4AEA10` | 有正文 | `src/ui/WindowRuntime.cpp:26935 — Sub_4AEA10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1750 | 0x004AEAE0 | `sub_4AEAE0` | 有正文 | `src/ui/WindowRuntime.cpp:28950 — Sub_4AEAE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1751 | 0x004AEC10 | `sub_4AEC10` | 有正文 | `src/ui/WindowRuntime.cpp:28977 — Sub_4AEC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1752 | 0x004AEC30 | `sub_4AEC30` | 有正文 | `src/ui/WindowRuntime.cpp:29188 — Sub_4AEC30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1753 | 0x004AED40 | `sub_4AED40` | 有正文 | `src/ui/WindowRuntime.cpp:29211 — Sub_4AED40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1754 | 0x004AEE00 | `sub_4AEE00` | 有正文 | `src/ui/WindowRuntime.cpp:29237 — Sub_4AEE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1755 | 0x004AF000 | `sub_4AF000` | 有正文 | `src/ui/WindowRuntime.cpp:29293 — Sub_4AF000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1756 | 0x004AF090 | `sub_4AF090` | 有正文 | `src/ui/WindowRuntime.cpp:29315 — Sub_4AF090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1757 | 0x004AF0C0 | `sub_4AF0C0` | 有正文 | `src/ui/WindowRuntime.cpp:29325 — Sub_4AF0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1758 | 0x004AF100 | `sub_4AF100` | 有正文 | `src/ui/WindowRuntime.cpp:29340 — Sub_4AF100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1759 | 0x004AF180 | `sub_4AF180` | 有正文 | `src/ui/WindowRuntime.cpp:28986 — Sub_4AF180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1760 | 0x004AF250 | `sub_4AF250` | 有正文 | `src/ui/WindowRuntime.cpp:29009 — Sub_4AF250` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1761 | 0x004AF650 | `sub_4AF650` | 有正文 | `src/ui/WindowRuntime.cpp:29091 — Sub_4AF650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1762 | 0x004AF960 | `sub_4AF960` | 有正文 | `src/ui/WindowRuntime.cpp:29357 — Sub_4AF960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1763 | 0x004AFA00 | `sub_4AFA00` | 有正文 | `src/ui/WindowRuntime.cpp:29134 — Sub_4AFA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1764 | 0x004AFA60 | `sub_4AFA60` | 有正文 | `src/ui/WindowRuntime.cpp:29170 — Sub_4AFA60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1765 | 0x004AFAE0 | `sub_4AFAE0` | 有正文 | `src/ui/WindowRuntime.cpp:29160 — Sub_4AFAE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1766 | 0x004AFB10 | `sub_4AFB10` | 有正文 | `src/ui/WindowRuntime.cpp:29401 — Sub_4AFB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1767 | 0x004AFC00 | `sub_4AFC00` | 有正文 | `src/ui/WindowRuntime.cpp:29424 — Sub_4AFC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1768 | 0x004AFCF0 | `sub_4AFCF0` | 有正文 | `src/ui/WindowRuntime.cpp:29447 — Sub_4AFCF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1769 | 0x004AFE20 | `sub_4AFE20` | 有正文 | `src/ui/WindowRuntime.cpp:29376 — Sub_4AFE20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1770 | 0x004AFEA0 | `sub_4AFEA0` | 有正文 | `src/ui/WindowRuntime.cpp:29389 — Sub_4AFEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1771 | 0x004AFED0 | `sub_4AFED0` | 有正文 | `src/ui/WindowRuntime.cpp:29146 — Sub_4AFED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1772 | 0x004AFF30 | `sub_4AFF30` | 有正文 | `src/ui/WindowRuntime.cpp:5468 — Sub_4AFF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1773 | 0x004AFF70 | `sub_4AFF70` | 有正文 | `src/ui/WindowRuntime.cpp:5840 — Sub_4AFF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1774 | 0x004AFFB0 | `sub_4AFFB0` | 有正文 | `src/ui/WindowRuntime.cpp:5482 — Sub_4AFFB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1775 | 0x004AFFE0 | `sub_4AFFE0` | 有正文 | `src/ui/WindowRuntime.cpp:12696 — Sub_4AFFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1776 | 0x004B0000 | `sub_4B0000` | 有正文 | `src/ui/WindowRuntime.cpp:1954 — Sub_4B0000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1777 | 0x004B0060 | `sub_4B0060` | 有正文 | `src/ui/WindowRuntime.cpp:5491 — Sub_4B0060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1778 | 0x004B0090 | `sub_4B0090` | 有正文 | `src/ui/WindowRuntime.cpp:5855 — Sub_4B0090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1779 | 0x004B00C0 | `sub_4B00C0` | 有正文 | `src/ui/WindowRuntime.cpp:5502 — Sub_4B00C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1780 | 0x004B00F0 | `sub_4B00F0` | 有正文 | `src/ui/WindowRuntime.cpp:12703 — Sub_4B00F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1781 | 0x004B0110 | `sub_4B0110` | 有正文 | `src/ui/WindowRuntime.cpp:5426 — Sub_4B0110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1782 | 0x004B0130 | `sub_4B0130` | 有正文 | `src/ui/WindowRuntime.cpp:5435 — Sub_4B0130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1783 | 0x004B0180 | `sub_4B0180` | 有正文 | `src/ui/WindowRuntime.cpp:5449 — Sub_4B0180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1784 | 0x004B01B0 | `sub_4B01B0` | 有正文 | `src/ui/WindowRuntime.cpp:5460 — Sub_4B01B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1785 | 0x004B0200 | `sub_4B0200` | 有正文 | `src/ui/WindowRuntime.cpp:5867 — Sub_4B0200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1786 | 0x004B0290 | `sub_4B0290` | 有正文 | `src/ui/WindowRuntime.cpp:5888 — Sub_4B0290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1787 | 0x004B08D0 | `sub_4B08D0` | 有正文 | `src/ui/WindowRuntime.cpp:6064 — Sub_4B08D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1788 | 0x004B09E0 | `sub_4B09E0` | 有正文 | `src/ui/WindowRuntime.cpp:7843 — Sub_4B09E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1789 | 0x004B0A50 | `sub_4B0A50` | 有正文 | `src/ui/WindowRuntime.cpp:12710 — Sub_4B0A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1790 | 0x004B0CF0 | `sub_4B0CF0` | 有正文 | `src/ui/WindowRuntime.cpp:12773 — Sub_4B0CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1791 | 0x004B0DB0 | `sub_4B0DB0` | 有正文 | `src/ui/WindowRuntime.cpp:12803 — Sub_4B0DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1792 | 0x004B0F10 | `NULL_FUNC` | 有正文 | `src/ui/WindowRuntime.cpp:1885 — Sub_4B0F10` | 原入口的 RET／固定返回已用当前 IDA 核实；不是业务占位；完整接线未验 |
| 1793 | 0x004B0F20 | `sub_4B0F20` | 有正文 | `src/ui/WindowRuntime.cpp:6292 — Sub_4B0F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1794 | 0x004B1260 | `sub_4B1260` | 有正文 | `src/ui/WindowRuntime.cpp:6344 — Sub_4B1260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1795 | 0x004B1900 | `sub_4B1900` | 有正文 | `src/ui/WindowRuntime.cpp:7681 — Sub_4B1900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1796 | 0x004B1940 | `sub_4B1940` | 有正文 | `src/ui/WindowRuntime.cpp:6677 — Sub_4B1940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1797 | 0x004B2060 | `sub_4B2060` | 有正文 | `src/ui/WindowRuntime.cpp:6858 — Sub_4B2060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1798 | 0x004B2480 | `sub_4B2480` | 有正文 | `src/ui/WindowRuntime.cpp:6988 — Sub_4B2480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1799 | 0x004B2500 | `sub_4B2500` | 有正文 | `src/ui/WindowRuntime.cpp:7004 — Sub_4B2500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1800 | 0x004B25D0 | `sub_4B25D0` | 有正文 | `src/ui/WindowRuntime.cpp:7044 — Sub_4B25D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1801 | 0x004B2710 | `sub_4B2710` | 有正文 | `src/ui/WindowRuntime.cpp:7126 — Sub_4B2710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1802 | 0x004B2730 | `sub_4B2730` | 有正文 | `src/ui/WindowRuntime.cpp:7138 — Sub_4B2730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1803 | 0x004B2E00 | `sub_4B2E00` | 有正文 | `src/ui/WindowRuntime.cpp:7083 — Sub_4B2E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1804 | 0x004B2F50 | `sub_4B2F50` | 有正文 | `src/ui/WindowRuntime.cpp:7132 — Sub_4B2F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1805 | 0x004B2F70 | `sub_4B2F70` | 有正文 | `src/ui/WindowRuntime.cpp:7314 — Sub_4B2F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1806 | 0x004B3430 | `sub_4B3430` | 有正文 | `src/ui/WindowRuntime.cpp:7451 — Sub_4B3430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1807 | 0x004B34C0 | `sub_4B34C0` | 有正文 | `src/ui/WindowRuntime.cpp:7470 — Sub_4B34C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1808 | 0x004B3580 | `sub_4B3580` | 有正文 | `src/ui/WindowRuntime.cpp:7502 — Sub_4B3580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1809 | 0x004B3610 | `sub_4B3610` | 有正文 | `src/ui/WindowRuntime.cpp:7521 — Sub_4B3610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1810 | 0x004B3B00 | `sub_4B3B00` | 有正文 | `src/ui/WindowRuntime.cpp:12580 — Sub_4B3B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1811 | 0x004B3BF0 | `sub_4B3BF0` | 有正文 | `src/ui/WindowRuntime.cpp:12610 — Sub_4B3BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1812 | 0x004B4070 | `sub_4B4070` | 有正文 | `src/ui/WindowRuntime.cpp:12343 — Sub_4B4070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1813 | 0x004B4140 | `sub_4B4140` | 有正文 | `src/ui/WindowRuntime.cpp:12370 — Sub_4B4140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1814 | 0x004B45A0 | `sub_4B45A0` | 有正文 | `src/ui/WindowRuntime.cpp:12463 — Sub_4B45A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1815 | 0x004B4680 | `sub_4B4680` | 有正文 | `src/ui/WindowRuntime.cpp:12493 — Sub_4B4680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1816 | 0x004B4B00 | `sub_4B4B00` | 有正文 | `src/ui/WindowRuntime.cpp:28149 — Sub_4B4B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1817 | 0x004B4B70 | `sub_4B4B70` | 有正文 | `src/ui/WindowRuntime.cpp:28169 — Sub_4B4B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1818 | 0x004B4B90 | `sub_4B4B90` | 有正文 | `src/ui/WindowRuntime.cpp:28178 — Sub_4B4B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1819 | 0x004B4BB0 | `sub_4B4BB0` | 有正文 | `src/ui/WindowRuntime.cpp:28187 — Sub_4B4BB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1820 | 0x004B4CA0 | `sub_4B4CA0` | 有正文 | `src/ui/WindowRuntime.cpp:28214 — Sub_4B4CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1821 | 0x004B4DB0 | `sub_4B4DB0` | 有正文 | `src/ui/WindowRuntime.cpp:28254 — Sub_4B4DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1822 | 0x004B4DF0 | `sub_4B4DF0` | 有正文 | `src/ui/WindowRuntime.cpp:28270 — Sub_4B4DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1823 | 0x004B4E20 | `sub_4B4E20` | 有正文 | `src/ui/WindowRuntime.cpp:28281 — Sub_4B4E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1824 | 0x004B5210 | `sub_4B5210` | 有正文 | `src/ui/WindowRuntime.cpp:28317 — Sub_4B5210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1825 | 0x004B53C0 | `sub_4B53C0` | 有正文 | `src/ui/WindowRuntime.cpp:28361 — Sub_4B53C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1826 | 0x004B5550 | `sub_4B5550` | 有正文 | `src/ui/WindowRuntime.cpp:28407 — Sub_4B5550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1827 | 0x004B5630 | `sub_4B5630` | 有正文 | `src/ui/WindowRuntime.cpp:28451 — Sub_4B5630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1828 | 0x004B5660 | `sub_4B5660` | 有正文 | `src/ui/WindowRuntime.cpp:28462 — Sub_4B5660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1829 | 0x004B5760 | `sub_4B5760` | 有正文 | `src/ui/WindowRuntime.cpp:28503 — Sub_4B5760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1830 | 0x004B57B0 | `sub_4B57B0` | 有正文 | `src/ui/WindowRuntime.cpp:28514 — Sub_4B57B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1831 | 0x004B5A40 | `sub_4B5A40` | 有正文 | `src/ui/WindowRuntime.cpp:28436 — Sub_4B5A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1832 | 0x004B5A90 | `sub_4B5A90` | 有正文 | `src/ui/WindowRuntime.cpp:18221 — Sub_4B5A90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1833 | 0x004B5AD0 | `sub_4B5AD0` | 有正文 | `src/ui/WindowRuntime.cpp:18236 — Sub_4B5AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1834 | 0x004B5B50 | `sub_4B5B50` | 有正文 | `src/ui/WindowRuntime.cpp:18255 — Sub_4B5B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1835 | 0x004B5B70 | `sub_4B5B70` | 有正文 | `src/ui/WindowRuntime.cpp:18263 — Sub_4B5B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1836 | 0x004B5C30 | `sub_4B5C30` | 有正文 | `src/ui/WindowRuntime.cpp:18283 — Sub_4B5C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1837 | 0x004B62A0 | `sub_4B62A0` | 有正文 | `src/ui/WindowRuntime.cpp:18404 — Sub_4B62A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1838 | 0x004B6970 | `sub_4B6970` | 有正文 | `src/ui/WindowRuntime.cpp:18459 — Sub_4B6970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1839 | 0x004B6C20 | `sub_4B6C20` | 有正文 | `src/ui/WindowRuntime.cpp:18450 — Sub_4B6C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1840 | 0x004B6C50 | `sub_4B6C50` | 有正文 | `src/ui/WindowRuntime.cpp:18522 — Sub_4B6C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1841 | 0x004B6DA0 | `sub_4B6DA0` | 有正文 | `src/ui/WindowRuntime.cpp:18567 — Sub_4B6DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1842 | 0x004B6DC0 | `sub_4B6DC0` | 有正文 | `src/ui/WindowRuntime.cpp:18609 — Sub_4B6DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1843 | 0x004B6E00 | `sub_4B6E00` | 有正文 | `src/ui/WindowRuntime.cpp:18623 — Sub_4B6E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1844 | 0x004B70D0 | `sub_4B70D0` | 有正文 | `src/ui/WindowRuntime.cpp:18677 — Sub_4B70D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1845 | 0x004B74E0 | `sub_4B74E0` | 有正文 | `src/ui/WindowRuntime.cpp:18770 — Sub_4B74E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1846 | 0x004B77E0 | `sub_4B77E0` | 有正文 | `src/ui/WindowRuntime.cpp:18834 — Sub_4B77E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1847 | 0x004B7AB0 | `sub_4B7AB0` | 有正文 | `src/ui/WindowRuntime.cpp:18986 — Sub_4B7AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1848 | 0x004B7EE0 | `sub_4B7EE0` | 有正文 | `src/ui/WindowRuntime.cpp:18993 — Sub_4B7EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1849 | 0x004B8310 | `sub_4B8310` | 有正文 | `src/ui/WindowRuntime.cpp:19000 — Sub_4B8310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1850 | 0x004B8670 | `sub_4B8670` | 有正文 | `src/ui/WindowRuntime.cpp:19054 — Sub_4B8670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1851 | 0x004B86C0 | `sub_4B86C0` | 有正文 | `src/ui/WindowRuntime.cpp:19070 — Sub_4B86C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1852 | 0x004B8730 | `sub_4B8730` | 有正文 | `src/ui/WindowRuntime.cpp:19093 — Sub_4B8730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1853 | 0x004B8780 | `sub_4B8780` | 有正文 | `src/ui/WindowRuntime.cpp:19114 — Sub_4B8780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1854 | 0x004B8B10 | `sub_4B8B10` | 有正文 | `src/ui/WindowRuntime.cpp:19176 — Sub_4B8B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1855 | 0x004B8BE0 | `sub_4B8BE0` | 有正文 | `src/ui/WindowRuntime.cpp:19203 — Sub_4B8BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1856 | 0x004B8C50 | `sub_4B8C50` | 有正文 | `src/ui/WindowRuntime.cpp:19224 — Sub_4B8C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1857 | 0x004B8C90 | `sub_4B8C90` | 有正文 | `src/ui/WindowRuntime.cpp:19241 — Sub_4B8C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1858 | 0x004B8FF0 | `sub_4B8FF0` | 有正文 | `src/ui/WindowRuntime.cpp:19298 — Sub_4B8FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1859 | 0x004B9090 | `sub_4B9090` | 有正文 | `src/ui/WindowRuntime.cpp:19317 — Sub_4B9090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1860 | 0x004B90E0 | `sub_4B90E0` | 有正文 | `src/ui/WindowRuntime.cpp:19336 — Sub_4B90E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1861 | 0x004B9110 | `sub_4B9110` | 有正文 | `src/ui/WindowRuntime.cpp:19352 — Sub_4B9110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1862 | 0x004B9270 | `sub_4B9270` | 有正文 | `src/ui/WindowRuntime.cpp:19362 — Sub_4B9270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1863 | 0x004B92C0 | `sub_4B92C0` | 有正文 | `src/ui/WindowRuntime.cpp:19372 — Sub_4B92C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1864 | 0x004B9310 | `sub_4B9310` | 有正文 | `src/ui/WindowRuntime.cpp:19392 — Sub_4B9310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1865 | 0x004B99D0 | `sub_4B99D0` | 有正文 | `src/ui/WindowRuntime.cpp:25456 — Sub_4B99D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1866 | 0x004B9B10 | `sub_4B9B10` | 有正文 | `src/ui/WindowRuntime.cpp:19577 — Sub_4B9B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1867 | 0x004B9F40 | `sub_4B9F40` | 有正文 | `src/ui/WindowRuntime.cpp:19636 — Sub_4B9F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1868 | 0x004BA950 | `sub_4BA950` | 有正文 | `src/ui/WindowRuntime.cpp:19907 — Sub_4BA950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1869 | 0x004BAA50 | `sub_4BAA50` | 有正文 | `src/ui/WindowRuntime.cpp:19942 — Sub_4BAA50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1870 | 0x004BAD10 | `sub_4BAD10` | 有正文 | `src/ui/WindowRuntime.cpp:20152 — Sub_4BAD10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1871 | 0x004BAE10 | `sub_4BAE10` | 有正文 | `src/ui/WindowRuntime.cpp:20198 — Sub_4BAE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1872 | 0x004BAF10 | `sub_4BAF10` | 有正文 | `src/ui/WindowRuntime.cpp:20244 — Sub_4BAF10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1873 | 0x004BB010 | `sub_4BB010` | 有正文 | `src/ui/WindowRuntime.cpp:20288 — Sub_4BB010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1874 | 0x004BB1D0 | `sub_4BB1D0` | 有正文 | `src/ui/WindowRuntime.cpp:20369 — Sub_4BB1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1875 | 0x004BB6E0 | `sub_4BB6E0` | 有正文 | `src/ui/WindowRuntime.cpp:20602 — Sub_4BB6E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1876 | 0x004BBED0 | `sub_4BBED0` | 有正文 | `src/ui/WindowRuntime.cpp:20953 — Sub_4BBED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1877 | 0x004BC060 | `sub_4BC060` | 有正文 | `src/ui/WindowRuntime.cpp:21009 — Sub_4BC060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1878 | 0x004BC150 | `sub_4BC150` | 有正文 | `src/ui/WindowRuntime.cpp:18573 — Sub_4BC150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1879 | 0x004BC230 | `sub_4BC230` | 有正文 | `src/ui/WindowRuntime.cpp:21044 — Sub_4BC230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1880 | 0x004BC660 | `sub_4BC660` | 有正文 | `src/ui/WindowRuntime.cpp:21177 — Sub_4BC660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1881 | 0x004BCEE0 | `sub_4BCEE0` | 有正文 | `src/ui/WindowRuntime.cpp:20066 — Sub_4BCEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1882 | 0x004BD100 | `sub_4BD100` | 有正文 | `src/ui/WindowRuntime.cpp:21427 — Sub_4BD100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1883 | 0x004BD500 | `sub_4BD500` | 有正文 | `src/ui/WindowRuntime.cpp:21521 — Sub_4BD500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1884 | 0x004BD540 | `sub_4BD540` | 有正文 | `src/ui/WindowRuntime.cpp:21531 — Sub_4BD540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1885 | 0x004BD680 | `sub_4BD680` | 有正文 | `src/ui/WindowRuntime.cpp:21567 — Sub_4BD680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1886 | 0x004BD810 | `sub_4BD810` | 有正文 | `src/ui/WindowRuntime.cpp:21673 — Sub_4BD810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1887 | 0x004BDAD0 | `sub_4BDAD0` | 有正文 | `src/ui/WindowRuntime.cpp:21679 — Sub_4BDAD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1888 | 0x004BDD90 | `sub_4BDD90` | 有正文 | `src/ui/WindowRuntime.cpp:21685 — Sub_4BDD90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1889 | 0x004BDF70 | `sub_4BDF70` | 有正文 | `src/ui/WindowRuntime.cpp:21732 — Sub_4BDF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1890 | 0x004BE200 | `sub_4BE200` | 有正文 | `src/ui/WindowRuntime.cpp:21783 — Sub_4BE200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1891 | 0x004BE3F0 | `sub_4BE3F0` | 有正文 | `src/ui/WindowRuntime.cpp:21833 — Sub_4BE3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1892 | 0x004BE600 | `sub_4BE600` | 有正文 | `src/ui/WindowRuntime.cpp:21886 — Sub_4BE600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1893 | 0x004BE930 | `sub_4BE930` | 有正文 | `src/ui/WindowRuntime.cpp:21961 — Sub_4BE930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1894 | 0x004BEC20 | `sub_4BEC20` | 有正文 | `src/ui/WindowRuntime.cpp:22015 — Sub_4BEC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1895 | 0x004BEF30 | `sub_4BEF30` | 有正文 | `src/ui/WindowRuntime.cpp:22085 — Sub_4BEF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1896 | 0x004BF160 | `sub_4BF160` | 有正文 | `src/ui/WindowRuntime.cpp:22134 — Sub_4BF160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1897 | 0x004BF2E0 | `sub_4BF2E0` | 有正文 | `src/ui/WindowRuntime.cpp:22166 — Sub_4BF2E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1898 | 0x004BF460 | `sub_4BF460` | 有正文 | `src/ui/WindowRuntime.cpp:22197 — Sub_4BF460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1899 | 0x004BF620 | `sub_4BF620` | 有正文 | `src/ui/WindowRuntime.cpp:22235 — Sub_4BF620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1900 | 0x004BF7C0 | `sub_4BF7C0` | 有正文 | `src/ui/WindowRuntime.cpp:22268 — Sub_4BF7C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1901 | 0x004BF950 | `sub_4BF950` | 有正文 | `src/ui/WindowRuntime.cpp:22312 — Sub_4BF950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1902 | 0x004BFBC0 | `sub_4BFBC0` | 有正文 | `src/ui/WindowRuntime.cpp:22398 — Sub_4BFBC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1903 | 0x004BFCB0 | `sub_4BFCB0` | 有正文 | `src/ui/WindowRuntime.cpp:22434 — Sub_4BFCB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1904 | 0x004BFF60 | `sub_4BFF60` | 有正文 | `src/ui/WindowRuntime.cpp:22548 — Sub_4BFF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1905 | 0x004C0160 | `sub_4C0160` | 有正文 | `src/ui/WindowRuntime.cpp:22626 — Sub_4C0160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1906 | 0x004C0380 | `sub_4C0380` | 有正文 | `src/ui/WindowRuntime.cpp:22708 — Sub_4C0380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1907 | 0x004C0590 | `sub_4C0590` | 有正文 | `src/ui/WindowRuntime.cpp:22780 — Sub_4C0590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1908 | 0x004C0800 | `sub_4C0800` | 有正文 | `src/ui/WindowRuntime.cpp:22876 — Sub_4C0800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1909 | 0x004C0910 | `sub_4C0910` | 有正文 | `src/ui/WindowRuntime.cpp:25133 — Sub_4C0910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1910 | 0x004C0A50 | `sub_4C0A50` | 有正文 | `src/ui/WindowRuntime.cpp:25174 — Sub_4C0A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1911 | 0x004C0BF0 | `sub_4C0BF0` | 有正文 | `src/ui/WindowRuntime.cpp:25243 — Sub_4C0BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1912 | 0x004C0CB0 | `sub_4C0CB0` | 有正文 | `src/ui/WindowRuntime.cpp:25271 — Sub_4C0CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1913 | 0x004C0EA0 | `sub_4C0EA0` | 有正文 | `src/ui/WindowRuntime.cpp:25335 — Sub_4C0EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1914 | 0x004C0F20 | `sub_4C0F20` | 有正文 | `src/ui/WindowRuntime.cpp:25353 — Sub_4C0F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1915 | 0x004C1390 | `sub_4C1390` | 有正文 | `src/ui/WindowRuntime.cpp:22920 — Sub_4C1390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1916 | 0x004C13B0 | `sub_4C13B0` | 有正文 | `src/ui/WindowRuntime.cpp:22929 — Sub_4C13B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1917 | 0x004C1700 | `sub_4C1700` | 有正文 | `src/ui/WindowRuntime.cpp:22998 — Sub_4C1700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1918 | 0x004C17B0 | `sub_4C17B0` | 有正文 | `src/ui/WindowRuntime.cpp:23014 — Sub_4C17B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1919 | 0x004C1AA0 | `sub_4C1AA0` | 有正文 | `src/ui/WindowRuntime.cpp:23056 — Sub_4C1AA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1920 | 0x004C1B00 | `sub_4C1B00` | 有正文 | `src/ui/WindowRuntime.cpp:23070 — Sub_4C1B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1921 | 0x004C1B70 | `sub_4C1B70` | 有正文 | `src/ui/WindowRuntime.cpp:23089 — Sub_4C1B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1922 | 0x004C1BC0 | `sub_4C1BC0` | 有正文 | `src/ui/WindowRuntime.cpp:23107 — Sub_4C1BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1923 | 0x004C1C50 | `sub_4C1C50` | 有正文 | `src/ui/WindowRuntime.cpp:23134 — Sub_4C1C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1924 | 0x004C1EC0 | `sub_4C1EC0` | 有正文 | `src/ui/WindowRuntime.cpp:23197 — Sub_4C1EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1925 | 0x004C1FB0 | `sub_4C1FB0` | 有正文 | `src/ui/WindowRuntime.cpp:7861 — Sub_4C1FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1926 | 0x004C2060 | `sub_4C2060` | 有正文 | `src/ui/WindowRuntime.cpp:23231 — Sub_4C2060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1927 | 0x004C21B0 | `sub_4C21B0` | 有正文 | `src/ui/WindowRuntime.cpp:23268 — Sub_4C21B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1928 | 0x004C2460 | `sub_4C2460` | 有正文 | `src/ui/WindowRuntime.cpp:23358 — Sub_4C2460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1929 | 0x004C2530 | `sub_4C2530` | 有正文 | `src/ui/WindowRuntime.cpp:23391 — Sub_4C2530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1930 | 0x004C25B0 | `sub_4C25B0` | 有正文 | `src/ui/WindowRuntime.cpp:23416 — Sub_4C25B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1931 | 0x004C2630 | `sub_4C2630` | 有正文 | `src/ui/WindowRuntime.cpp:23442 — Sub_4C2630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1932 | 0x004C2634 | `sub_4C2634` | 有承接 | `src/ui/WindowRuntime.cpp:23442 — Sub_4C2630` | 原表指向含索引前缀的 4C2630；入口片段并入实际父正文；完整业务／运行未验 |
| 1933 | 0x004C26B0 | `sub_4C26B0` | 有正文 | `src/ui/WindowRuntime.cpp:23466 — Sub_4C26B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1934 | 0x004C2760 | `sub_4C2760` | 有正文 | `src/ui/WindowRuntime.cpp:23497 — Sub_4C2760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1935 | 0x004C27B0 | `sub_4C27B0` | 有正文 | `src/ui/WindowRuntime.cpp:23511 — Sub_4C27B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1936 | 0x004C28C0 | `sub_4C28C0` | 有正文 | `src/ui/WindowRuntime.cpp:23546 — Sub_4C28C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1937 | 0x004C29E0 | `sub_4C29E0` | 有正文 | `src/ui/WindowRuntime.cpp:23590 — Sub_4C29E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1938 | 0x004C2B20 | `sub_4C2B20` | 有正文 | `src/ui/WindowRuntime.cpp:23637 — Sub_4C2B20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1939 | 0x004C2FA0 | `sub_4C2FA0` | 有正文 | `src/ui/WindowRuntime.cpp:23749 — Sub_4C2FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1940 | 0x004C33B0 | `sub_4C33B0` | 有正文 | `src/ui/WindowRuntime.cpp:23843 — Sub_4C33B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1941 | 0x004C3530 | `sub_4C3530` | 有正文 | `src/ui/WindowRuntime.cpp:23879 — Sub_4C3530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1942 | 0x004C3550 | `sub_4C3550` | 有正文 | `src/ui/WindowRuntime.cpp:23888 — Sub_4C3550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1943 | 0x004C39A0 | `sub_4C39A0` | 有正文 | `src/ui/WindowRuntime.cpp:23951 — Sub_4C39A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1944 | 0x004C3A50 | `sub_4C3A50` | 有正文 | `src/ui/WindowRuntime.cpp:23967 — Sub_4C3A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1945 | 0x004C3D40 | `sub_4C3D40` | 有正文 | `src/ui/WindowRuntime.cpp:24009 — Sub_4C3D40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1946 | 0x004C3DA0 | `sub_4C3DA0` | 有正文 | `src/ui/WindowRuntime.cpp:24023 — Sub_4C3DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1947 | 0x004C3E20 | `sub_4C3E20` | 有正文 | `src/ui/WindowRuntime.cpp:24042 — Sub_4C3E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1948 | 0x004C3E70 | `sub_4C3E70` | 有正文 | `src/ui/WindowRuntime.cpp:24060 — Sub_4C3E70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1949 | 0x004C3F00 | `sub_4C3F00` | 有正文 | `src/ui/WindowRuntime.cpp:24087 — Sub_4C3F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1950 | 0x004C4180 | `sub_4C4180` | 有正文 | `src/ui/WindowRuntime.cpp:24151 — Sub_4C4180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1951 | 0x004C4270 | `sub_4C4270` | 有正文 | `src/ui/WindowRuntime.cpp:24185 — Sub_4C4270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1952 | 0x004C43C0 | `sub_4C43C0` | 有正文 | `src/ui/WindowRuntime.cpp:24222 — Sub_4C43C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1953 | 0x004C4680 | `sub_4C4680` | 有正文 | `src/ui/WindowRuntime.cpp:24312 — Sub_4C4680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1954 | 0x004C46C0 | `sub_4C46C0` | 有正文 | `src/ui/WindowRuntime.cpp:24326 — Sub_4C46C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1955 | 0x004C4740 | `sub_4C4740` | 有正文 | `src/ui/WindowRuntime.cpp:24351 — Sub_4C4740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1956 | 0x004C47C0 | `sub_4C47C0` | 有正文 | `src/ui/WindowRuntime.cpp:24377 — Sub_4C47C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1957 | 0x004C4840 | `sub_4C4840` | 有正文 | `src/ui/WindowRuntime.cpp:24401 — Sub_4C4840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1958 | 0x004C48EB | `sub_4C48EB` | 不适用 | `当前 IDA：0x004C48EB..0x004C48F0 仅 5 条 NOP` | 元数据登记的填充片段，无业务正文；保留原行，不扩大到其他入口 |
| 1959 | 0x004C48F0 | `sub_4C48F0` | 有正文 | `src/ui/WindowRuntime.cpp:24432 — Sub_4C48F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1960 | 0x004C4940 | `sub_4C4940` | 有正文 | `src/ui/WindowRuntime.cpp:24446 — Sub_4C4940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1961 | 0x004C4A50 | `sub_4C4A50` | 有正文 | `src/ui/WindowRuntime.cpp:24481 — Sub_4C4A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1962 | 0x004C4B70 | `sub_4C4B70` | 有正文 | `src/ui/WindowRuntime.cpp:24525 — Sub_4C4B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1963 | 0x004C4CB0 | `sub_4C4CB0` | 有正文 | `src/ui/WindowRuntime.cpp:24572 — Sub_4C4CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1964 | 0x004C50C0 | `sub_4C50C0` | 有正文 | `src/ui/WindowRuntime.cpp:24684 — Sub_4C50C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1965 | 0x004C54D0 | `sub_4C54D0` | 有正文 | `src/ui/WindowRuntime.cpp:24778 — Sub_4C54D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1966 | 0x004C5650 | `sub_4C5650` | 有正文 | `src/CharacterSelectPreview.cpp:456 — Sub_4C5650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1967 | 0x004C56E0 | `sub_4C56E0` | 有正文 | `src/CharacterSelectPreview.cpp:425 — Sub_4C56E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1968 | 0x004C5790 | `sub_4C5790` | 有正文 | `src/ui/WindowRuntime.cpp:24814 — Sub_4C5790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1969 | 0x004C5850 | `sub_4C5850` | 有正文 | `src/ui/WindowRuntime.cpp:24835 — Sub_4C5850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1970 | 0x004C5B70 | `sub_4C5B70` | 有正文 | `src/ui/WindowRuntime.cpp:24905 — Sub_4C5B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1971 | 0x004C5C00 | `sub_4C5C00` | 有正文 | `src/ui/WindowRuntime.cpp:24923 — Sub_4C5C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1972 | 0x004C5C30 | `sub_4C5C30` | 有正文 | `src/ui/WindowRuntime.cpp:24932 — Sub_4C5C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1973 | 0x004C5D00 | `sub_4C5D00` | 有正文 | `src/ui/WindowRuntime.cpp:24968 — Sub_4C5D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1974 | 0x004C5DE0 | `sub_4C5DE0` | 有正文 | `src/ui/WindowRuntime.cpp:25003 — Sub_4C5DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1975 | 0x004C5EB0 | `sub_4C5EB0` | 有正文 | `src/ui/WindowRuntime.cpp:25037 — Sub_4C5EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1976 | 0x004C5F80 | `sub_4C5F80` | 有正文 | `src/ui/WindowRuntime.cpp:25071 — Sub_4C5F80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1977 | 0x004C6060 | `sub_4C6060` | 有正文 | `src/ui/WindowRuntime.cpp:25102 — Sub_4C6060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1978 | 0x004C61B0 | `sub_4C61B0` | 有正文 | `src/ui/WindowRuntime.cpp:8087 — Sub_4C61B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1979 | 0x004C63C0 | `sub_4C63C0` | 有正文 | `src/ui/WindowRuntime.cpp:8187 — Sub_4C63C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1980 | 0x004C6400 | `sub_4C6400` | 有正文 | `src/ui/WindowRuntime.cpp:8205 — Sub_4C6400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1981 | 0x004C6440 | `sub_4C6440` | 有正文 | `src/ui/WindowRuntime.cpp:8223 — Sub_4C6440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1982 | 0x004C6470 | `sub_4C6470` | 有正文 | `src/ui/WindowRuntime.cpp:8235 — Sub_4C6470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1983 | 0x004C6500 | `sub_4C6500` | 有正文 | `src/ui/WindowRuntime.cpp:8257 — Sub_4C6500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1984 | 0x004C6550 | `sub_4C6550` | 有正文 | `src/ui/WindowRuntime.cpp:8275 — Sub_4C6550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1985 | 0x004C6790 | `sub_4C6790` | 有正文 | `src/ui/WindowRuntime.cpp:8362 — Sub_4C6790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1986 | 0x004C6990 | `sub_4C6990` | 有正文 | `src/ui/WindowRuntime.cpp:8450 — Sub_4C6990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1987 | 0x004C69B0 | `sub_4C69B0` | 有正文 | `src/ui/WindowRuntime.cpp:8459 — Sub_4C69B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1988 | 0x004C6AC0 | `sub_4C6AC0` | 有正文 | `src/ui/WindowRuntime.cpp:8492 — Sub_4C6AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1989 | 0x004C6B10 | `sub_4C6B10` | 有正文 | `src/ui/WindowRuntime.cpp:8505 — Sub_4C6B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1990 | 0x004C6D90 | `sub_4C6D90` | 有正文 | `src/ui/WindowRuntime.cpp:8583 — Sub_4C6D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1991 | 0x004C6EC0 | `sub_4C6EC0` | 有正文 | `src/ui/WindowRuntime.cpp:8127 — Sub_4C6EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1992 | 0x004C71B0 | `sub_4C71B0` | 有正文 | `src/ui/WindowRuntime.cpp:8120 — Sub_4C71B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1993 | 0x004C71C0 | `sub_4C71C0` | 有正文 | `src/ui/WindowRuntime.cpp:8610 — Sub_4C71C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1994 | 0x004C7240 | `sub_4C7240` | 有正文 | `src/ui/WindowRuntime.cpp:8821 — Sub_4C7240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1995 | 0x004C7280 | `sub_4C7280` | 有正文 | `src/ui/WindowRuntime.cpp:8837 — Sub_4C7280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1996 | 0x004C72C0 | `sub_4C72C0` | 有正文 | `src/ui/WindowRuntime.cpp:8853 — Sub_4C72C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1997 | 0x004C7340 | `sub_4C7340` | 有正文 | `src/ui/WindowRuntime.cpp:8637 — Sub_4C7340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1998 | 0x004C7380 | `sub_4C7380` | 有正文 | `src/ui/WindowRuntime.cpp:8655 — Sub_4C7380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1999 | 0x004C75A0 | `sub_4C75A0` | 有正文 | `src/ui/WindowRuntime.cpp:11895 — Sub_4C75A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2000 | 0x004C78A0 | `sub_4C78A0` | 有正文 | `src/ui/WindowRuntime.cpp:8732 — Sub_4C78A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2001 | 0x004C7910 | `sub_4C7910` | 有正文 | `src/ui/WindowRuntime.cpp:8746 — Sub_4C7910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2002 | 0x004C7A10 | `sub_4C7A10` | 有正文 | `src/ui/WindowRuntime.cpp:8785 — Sub_4C7A10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2003 | 0x004C7A90 | `sub_4C7A90` | 有正文 | `src/ui/WindowRuntime.cpp:8808 — Sub_4C7A90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2004 | 0x004C7AD0 | `sub_4C7AD0` | 有正文 | `src/ui/WindowRuntime.cpp:15823 — Sub_4C7AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2005 | 0x004C7AE0 | `sub_4C7AE0` | 有正文 | `src/ui/WindowRuntime.cpp:15929 — ProcessWindowShortcuts_4C7AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2006 | 0x004C9060 | `sub_4C9060` | 有正文 | `src/ui/WindowRuntime.cpp:15614 — Sub_4C9060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2007 | 0x004C9130 | `sub_4C9130` | 有正文 | `src/ui/WindowRuntime.cpp:15692 — Sub_4C9130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2008 | 0x004C9170 | `sub_4C9170` | 有正文 | `src/ui/WindowRuntime.cpp:15579 — Sub_4C9170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2009 | 0x004C9240 | `sub_4C9240` | 有正文 | `src/ui/WindowRuntime.cpp:15518 — Sub_4C9240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2010 | 0x004C92B0 | `sub_4C92B0` | 有正文 | `src/ui/WindowRuntime.cpp:15649 — Sub_4C92B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2011 | 0x004C92D0 | `sub_4C92D0` | 有正文 | `src/ui/WindowRuntime.cpp:15656 — Sub_4C92D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2012 | 0x004C93A0 | `sub_4C93A0` | 有正文 | `src/ui/WindowRuntime.cpp:15539 — Sub_4C93A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2013 | 0x004C93E0 | `sub_4C93E0` | 有正文 | `src/ui/WindowRuntime.cpp:15551 — Sub_4C93E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2014 | 0x004C9490 | `sub_4C9490` | 有正文 | `src/world/MailState.cpp:690 — Sub_4C9490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2015 | 0x004C94B0 | `sub_4C94B0` | 有正文 | `src/ui/WindowRuntime.cpp:4498 — Sub_4C94B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2016 | 0x004C94D6 | `?classic@locale@std@@SAABV12@XZ_20` | 有承接 | `src/ui/WindowRuntime.cpp:4498 — Sub_4C94B0` | 原调用 4C9C70／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2017 | 0x004C94F0 | `sub_4C94F0` | 有正文 | `src/ui/WindowRuntime.cpp:4515 — Sub_4C94F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2018 | 0x004C96F0 | `sub_4C96F0` | 有正文 | `src/ui/WindowRuntime.cpp:4562 — Sub_4C96F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2019 | 0x004C9BE0 | `sub_4C9BE0` | 有正文 | `src/ui/WindowRuntime.cpp:4717 — Sub_4C9BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2020 | 0x004C9C70 | `sub_4C9C70` | 有正文 | `src/ui/WindowRuntime.cpp:4474 — Sub_4C9C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2021 | 0x004C9E40 | `sub_4C9E40` | 有正文 | `src/world/MailState.cpp:1101 — Sub_4C9E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2022 | 0x004C9EE0 | `sub_4C9EE0` | 有正文 | `src/ui/WindowRuntime.cpp:4759 — Sub_4C9EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2023 | 0x004C9F08 | `?classic@locale@std@@SAABV12@XZ_22` | 有承接 | `src/ui/WindowRuntime.cpp:4759 — Sub_4C9EE0` | 原调用空回调 509080／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2024 | 0x004C9F20 | `sub_4C9F20` | 有正文 | `src/ui/WindowRuntime.cpp:4735 — Sub_4C9F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2025 | 0x004CA1A0 | `sub_4CA1A0` | 有正文 | `src/ui/WindowRuntime.cpp:4809 — Sub_4CA1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2026 | 0x004CA300 | `sub_4CA300` | 有正文 | `src/ui/WindowRuntime.cpp:4771 — Sub_4CA300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2027 | 0x004CA330 | `sub_4CA330` | 有正文 | `src/world/MailState.cpp:1136 — Sub_4CA330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2028 | 0x004CA410 | `sub_4CA410` | 有正文 | `src/ui/WindowRuntime.cpp:4893 — Sub_4CA410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2029 | 0x004CA438 | `?classic@locale@std@@SAABV12@XZ_23` | 有承接 | `src/ui/WindowRuntime.cpp:4893 — Sub_4CA410` | 原调用 4CA450／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2030 | 0x004CA450 | `sub_4CA450` | 有正文 | `src/ui/WindowRuntime.cpp:4871 — Sub_4CA450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2031 | 0x004CA500 | `sub_4CA500` | 有正文 | `src/ui/WindowRuntime.cpp:4840 — Sub_4CA500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2032 | 0x004CA6F0 | `sub_4CA6F0` | 有正文 | `src/ui/WindowRuntime.cpp:4938 — Sub_4CA6F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2033 | 0x004CA9C0 | `sub_4CA9C0` | 有正文 | `src/ui/WindowRuntime.cpp:4980 — Sub_4CA9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2034 | 0x004CAB60 | `sub_4CAB60` | 有正文 | `src/ui/WindowRuntime.cpp:4906 — Sub_4CAB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2035 | 0x004CAB90 | `sub_4CAB90` | 有正文 | `src/ui/WindowRuntime.cpp:4914 — Sub_4CAB90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2036 | 0x004CAC60 | `sub_4CAC60` | 有正文 | `src/world/MailState.cpp:1164 — Sub_4CAC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2037 | 0x004CAF60 | `sub_4CAF60` | 有正文 | `src/ui/WindowRuntime.cpp:5050 — Sub_4CAF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2038 | 0x004CAF88 | `?classic@locale@std@@SAABV12@XZ_24` | 有承接 | `src/ui/WindowRuntime.cpp:5050 — Sub_4CAF60` | 原调用 4CAFA0／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2039 | 0x004CAFA0 | `sub_4CAFA0` | 有正文 | `src/ui/WindowRuntime.cpp:5014 — Sub_4CAFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2040 | 0x004CB2F0 | `sub_4CB2F0` | 有正文 | `src/ui/WindowRuntime.cpp:5077 — Sub_4CB2F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2041 | 0x004CB430 | `sub_4CB430` | 有正文 | `src/ui/WindowRuntime.cpp:4780 — Sub_4CB430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2042 | 0x004CB4A0 | `sub_4CB4A0` | 有正文 | `src/ui/WindowRuntime.cpp:4792 — Sub_4CB4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2043 | 0x004CB530 | `sub_4CB530` | 有正文 | `src/ui/WindowRuntime.cpp:5070 — Sub_4CB530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2044 | 0x004CB550 | `sub_4CB550` | 有正文 | `src/ui/WindowRuntime.cpp:5128 — Sub_4CB550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2045 | 0x004CB5D0 | `sub_4CB5D0` | 有正文 | `src/ui/WindowRuntime.cpp:5110 — Sub_4CB5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2046 | 0x004CB6A0 | `sub_4CB6A0` | 有正文 | `src/ui/WindowRuntime.cpp:5171 — Sub_4CB6A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2047 | 0x004CB920 | `sub_4CB920` | 有正文 | `src/ui/WindowRuntime.cpp:5148 — Sub_4CB920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2048 | 0x004CB9B0 | `sub_4CB9B0` | 有正文 | `src/ui/WindowRuntime.cpp:5060 — Sub_4CB9B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2049 | 0x004CB9E0 | `?classic@locale@std@@SAABV12@XZ_25` | 有正文 | `src/ui/WindowRuntime.cpp:5272 — Sub_4CB9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2050 | 0x004CB9F0 | `sub_4CB9F0` | 有正文 | `src/ui/WindowRuntime.cpp:5237 — Sub_4CB9F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2051 | 0x004CBD30 | `sub_4CBD30` | 有正文 | `src/ui/WindowRuntime.cpp:29836 — Sub_4CBD30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2052 | 0x004CBD90 | `sub_4CBD90` | 有正文 | `src/ui/WindowRuntime.cpp:29940 — Sub_4CBD90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2053 | 0x004CBDB0 | `sub_4CBDB0` | 有正文 | `src/ui/WindowRuntime.cpp:29950 — Sub_4CBDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2054 | 0x004CC340 | `sub_4CC340` | 有正文 | `src/ui/WindowRuntime.cpp:29903 — Sub_4CC340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2055 | 0x004CC540 | `sub_4CC540` | 有正文 | `src/ui/WindowRuntime.cpp:29860 — Sub_4CC540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2056 | 0x004CC6A0 | `sub_4CC6A0` | 有正文 | `src/ui/WindowRuntime.cpp:29565 — Sub_4CC6A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2057 | 0x004CC6C0 | `sub_4CC6C0` | 有正文 | `src/ui/WindowRuntime.cpp:29624 — Sub_4CC6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2058 | 0x004CC6E0 | `sub_4CC6E0` | 有正文 | `src/ui/WindowRuntime.cpp:29634 — Sub_4CC6E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2059 | 0x004CC9B0 | `sub_4CC9B0` | 有正文 | `src/ui/WindowRuntime.cpp:29692 — Sub_4CC9B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2060 | 0x004CCED0 | `sub_4CCED0` | 有正文 | `src/ui/WindowRuntime.cpp:29574 — Sub_4CCED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2061 | 0x004CD150 | `sub_4CD150` | 有正文 | `src/ui/WindowRuntime.cpp:29799 — Sub_4CD150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2062 | 0x004CD1E0 | `sub_4CD1E0` | 有正文 | `src/ui/WindowRuntime.cpp:29817 — Sub_4CD1E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2063 | 0x004CD220 | `sub_4CD220` | 有正文 | `src/ui/WindowRuntime.cpp:29827 — Sub_4CD220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2064 | 0x004CD240 | `sub_4CD240` | 有承接 | `src/ui/WindowRuntime.cpp:45569 — StallBrowsePetValuesStartup::StallBrowsePetValuesStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 2065 | 0x004CD250 | `sub_4CD250` | 有承接 | `src/ui/WindowRuntime.cpp:45589 — ConstructStallBrowsePetValues` | 固定成员构造／逆序销毁由实际数组 helper 承接；未记完整展开验收；完整业务／运行未验 |
| 2066 | 0x004CD2D0 | `sub_4CD2D0` | 有承接 | `src/world/WorldUpdateDependencies.hpp:659 — PetRuntimeRecord::~PetRuntimeRecord` | 主／浏览各独立数组及六成员自动逆序析构；旧回调适配退休；运行未验 |
| 2067 | 0x004CD360 | `sub_4CD360` | 有承接 | `src/world/WorldUpdateDependencies.hpp:657 — PetRuntimeRecord::PetRuntimeRecord` | 主／浏览五项各六成员由C++／std::array正序构造；实际存储独立，RNG序保持；运行未验 |
| 2068 | 0x004CD3E0 | `sub_4CD3E0` | 有正文 | `src/ui/WindowRuntime.cpp:45638 — Sub_4CD3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2069 | 0x004CD400 | `sub_4CD400` | 有正文 | `src/ui/WindowRuntime.cpp:45648 — Sub_4CD400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2070 | 0x004CD420 | `sub_4CD420` | 有正文 | `src/ui/WindowRuntime.cpp:4293 — Sub_4CD420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2071 | 0x004CD4F0 | `sub_4CD4F0` | 有正文 | `src/ui/WindowRuntime.cpp:3739 — Sub_4CD4F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2072 | 0x004CDB10 | `sub_4CDB10` | 有正文 | `src/ui/WindowRuntime.cpp:3941 — Sub_4CDB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2073 | 0x004CDE00 | `sub_4CDE00` | 有正文 | `src/ui/WindowRuntime.cpp:4318 — Sub_4CDE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2074 | 0x004CDE40 | `sub_4CDE40` | 有正文 | `src/ui/WindowRuntime.cpp:2188 — Sub_4CDE40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2075 | 0x004CDFA0 | `sub_4CDFA0` | 有正文 | `src/ui/WindowRuntime.cpp:2231 — Sub_4CDFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2076 | 0x004CE1F0 | `sub_4CE1F0` | 有正文 | `src/ui/WindowRuntime.cpp:2300 — Sub_4CE1F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2077 | 0x004CE2B0 | `sub_4CE2B0` | 有正文 | `src/ui/WindowRuntime.cpp:2330 — Sub_4CE2B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2078 | 0x004CE3F0 | `sub_4CE3F0` | 有正文 | `src/ui/WindowRuntime.cpp:2376 — Sub_4CE3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2079 | 0x004CE700 | `sub_4CE700` | 有正文 | `src/ui/WindowRuntime.cpp:4327 — Sub_4CE700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2080 | 0x004CE8A0 | `sub_4CE8A0` | 有正文 | `src/ui/WindowRuntime.cpp:2465 — Sub_4CE8A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2081 | 0x004CE8B0 | `sub_4CE8B0` | 有正文 | `src/ui/WindowRuntime.cpp:4373 — Sub_4CE8B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2082 | 0x004CE930 | `sub_4CE930` | 有正文 | `src/ui/WindowRuntime.cpp:4394 — Sub_4CE930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2083 | 0x004CECB0 | `sub_4CECB0` | 有正文 | `src/ui/WindowRuntime.cpp:4265 — Sub_4CECB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2084 | 0x004CED50 | `sub_4CED50` | 有正文 | `src/ui/WindowRuntime.cpp:2471 — Sub_4CED50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2085 | 0x004CED60 | `sub_4CED60` | 有正文 | `src/ui/WindowRuntime.cpp:4052 — Sub_4CED60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2086 | 0x004CF8A0 | `sub_4CF8A0` | 有正文 | `src/ui/WindowRuntime.cpp:4246 — Sub_4CF8A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2087 | 0x004CF940 | `sub_4CF940` | 有正文 | `src/ui/WindowRuntime.cpp:2559 — Sub_4CF940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2088 | 0x004CFC40 | `sub_4CFC40` | 有正文 | `src/ui/WindowRuntime.cpp:2479 — Sub_4CFC40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2089 | 0x004CFCA0 | `sub_4CFCA0` | 有正文 | `src/ui/WindowRuntime.cpp:2493 — Sub_4CFCA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2090 | 0x004CFD70 | `sub_4CFD70` | 有正文 | `src/ui/WindowRuntime.cpp:2516 — Sub_4CFD70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2091 | 0x004CFE70 | `sub_4CFE70` | 有正文 | `src/ui/WindowRuntime.cpp:2700 — Sub_4CFE70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2092 | 0x004D0230 | `sub_4D0230` | 有正文 | `src/ui/WindowRuntime.cpp:2791 — Sub_4D0230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2093 | 0x004D04A0 | `sub_4D04A0` | 有正文 | `src/ui/WindowRuntime.cpp:3227 — Sub_4D04A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2094 | 0x004D0990 | `sub_4D0990` | 有正文 | `src/ui/WindowRuntime.cpp:2832 — Sub_4D0990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2095 | 0x004D0A80 | `sub_4D0A80` | 有正文 | `src/ui/WindowRuntime.cpp:2858 — Sub_4D0A80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2096 | 0x004D0EB0 | `sub_4D0EB0` | 有正文 | `src/ui/WindowRuntime.cpp:2930 — Sub_4D0EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2097 | 0x004D16F0 | `sub_4D16F0` | 有正文 | `src/ui/WindowRuntime.cpp:3113 — Sub_4D16F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2098 | 0x004D1C00 | `sub_4D1C00` | 有正文 | `src/ui/WindowRuntime.cpp:3083 — Sub_4D1C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2099 | 0x004D1D50 | `sub_4D1D50` | 有正文 | `src/ui/WindowRuntime.cpp:3376 — Sub_4D1D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2100 | 0x004D2260 | `sub_4D2260` | 有正文 | `src/ui/WindowRuntime.cpp:3464 — Sub_4D2260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2101 | 0x004D27C0 | `sub_4D27C0` | 有正文 | `src/ui/WindowRuntime.cpp:2438 — Sub_4D27C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2102 | 0x004D2840 | `sub_4D2840` | 有正文 | `src/ui/WindowRuntime.cpp:3580 — Sub_4D2840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2103 | 0x004D3020 | `sub_4D3020` | 有正文 | `src/ui/WindowRuntime.cpp:5385 — Sub_4D3020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2104 | 0x004D3040 | `sub_4D3040` | 有正文 | `src/ui/WindowRuntime.cpp:5393 — Sub_4D3040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2105 | 0x004D3110 | `sub_4D3110` | 有正文 | `src/YobiRuntime.cpp:35403 — Sub_4D3110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2106 | 0x004D3170 | `sub_4D3170` | 有正文 | `src/world/SceneState.cpp:8512 — Sub_4D3170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2107 | 0x004D3180 | `sub_4D3180` | 有正文 | `src/LegacyUiRecord.cpp:2466 — Sub_4D3180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2108 | 0x004D31C0 | `sub_4D31C0` | 有正文 | `src/LegacyUiRecord.cpp:2478 — Sub_4D31C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2109 | 0x004D3210 | `sub_4D3210` | 有正文 | `src/LegacyUiRecord.cpp:2491 — Sub_4D3210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2110 | 0x004D3410 | `sub_4D3410` | 有正文 | `src/LegacyUiRecord.cpp:2553 — Sub_4D3410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2111 | 0x004D36B0 | `sub_4D36B0` | 有正文 | `src/LegacyUiRecord.cpp:5476 — Sub_4D36B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2112 | 0x004D36F0 | `sub_4D36F0` | 有正文 | `src/LegacyUiRecord.cpp:5487 — Sub_4D36F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2113 | 0x004D4960 | `sub_4D4960` | 有正文 | `src/LegacyUiRecord.cpp:5756 — Sub_4D4960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2114 | 0x004D4A50 | `sub_4D4A50` | 有正文 | `src/LegacyUiRecord.cpp:5779 — Sub_4D4A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2115 | 0x004D4ED0 | `sub_4D4ED0` | 有正文 | `src/LegacyUiRecord.cpp:5880 — Sub_4D4ED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2116 | 0x004D4F00 | `sub_4D4F00` | 有正文 | `src/LegacyUiRecord.cpp:5891 — Sub_4D4F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2117 | 0x004D5220 | `sub_4D5220` | 有正文 | `src/LegacyUiRecord.cpp:5960 — Sub_4D5220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2118 | 0x004D5250 | `sub_4D5250` | 有正文 | `src/LegacyUiRecord.cpp:5971 — Sub_4D5250` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2119 | 0x004D5580 | `sub_4D5580` | 有正文 | `src/LegacyUiRecord.cpp:6039 — Sub_4D5580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2120 | 0x004D55C0 | `sub_4D55C0` | 有正文 | `src/LegacyUiRecord.cpp:6050 — Sub_4D55C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2121 | 0x004D61E0 | `sub_4D61E0` | 有正文 | `src/LegacyUiRecord.cpp:6283 — Sub_4D61E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2122 | 0x004D62A0 | `sub_4D62A0` | 有正文 | `src/LegacyUiRecord.cpp:6300 — Sub_4D62A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2123 | 0x004D6640 | `sub_4D6640` | 有正文 | `src/YobiRuntime.cpp:35620 — Sub_4D6640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2124 | 0x004D6650 | `nullsub_3` | 空方法 | `src/YobiRuntime.cpp:35620 — Sub_4D6640` | 原父分配失败落入RET，当前父return nullptr定位保留；按用户决定标为空方法，不再核对此入口 |
| 2125 | 0x004D6690 | `sub_4D6690` | 有正文 | `src/YobiRuntime.cpp:35685 — Sub_4D6690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2126 | 0x004D66E0 | `sub_4D66E0` | 有正文 | `src/LegacyUiRecord.cpp:2634 — Sub_4D66E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2127 | 0x004D6710 | `sub_4D6710` | 有正文 | `src/LegacyUiRecord.cpp:2645 — Sub_4D6710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2128 | 0x004D6A30 | `sub_4D6A30` | 有正文 | `src/LegacyUiRecord.cpp:2730 — Sub_4D6A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2129 | 0x004D6A60 | `sub_4D6A60` | 有正文 | `src/LegacyUiRecord.cpp:2741 — Sub_4D6A60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2130 | 0x004D6E70 | `sub_4D6E70` | 有正文 | `src/LegacyUiRecord.cpp:2851 — Sub_4D6E70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2131 | 0x004D6EA0 | `sub_4D6EA0` | 有正文 | `src/LegacyUiRecord.cpp:2862 — Sub_4D6EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2132 | 0x004D7440 | `sub_4D7440` | 有正文 | `src/LegacyCatalogWindows.cpp:194 — Sub_4D7440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2133 | 0x004D7470 | `sub_4D7470` | 部分实现 | `src/LegacyCatalogWindows.cpp:206 — Sub_4D7470` | 正文已有；CA0478 数组仅有 extern 声明，当前未见定义／生产；C8AF84 页数仅初值与读取，容量和原生产待证；不以数组初值代运行期数据；本轮原IDA限定检索仅见首条已证记录及相关计数／图号的读取、取地址引用，未定位生产者；不外推间接写入不存在；本轮已加载IDA各段的十个已证W32地址字面量检索只命中同30处消费指令，未发现未建xref的地址存储槽；仍不排除动态地址／间接写入；LBR01原57C368表第80项(ID58)的57C9AC槽实际存本入口，槽为db而未建立入口xref；当前LegacyAudio.cpp:391同位只存地址编号，旧表实际调度仍待证 |
| 2134 | 0x004D7BF0 | `sub_4D7BF0` | 有正文 | `src/LegacyCatalogWindows.cpp:357 — Sub_4D7BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2135 | 0x004D7C20 | `sub_4D7C20` | 部分实现 | `src/LegacyCatalogWindows.cpp:368 — Sub_4D7C20` | 正文已有；消费 CA0478 目录，但当前数组仅有 extern 声明，未见定义／生产；旧目录容量与生产待证；不以数组初值代运行期数据；本轮原IDA限定检索仅见首条已证记录及相关计数／图号的读取、取地址引用，未定位生产者；不外推间接写入不存在；本轮已加载IDA各段的十个已证W32地址字面量检索只命中同30处消费指令，未发现未建xref的地址存储槽；仍不排除动态地址／间接写入；LBR01原57C368表第81项(ID59)的57C9C0槽实际存本入口，槽为db而未建立入口xref；当前LegacyAudio.cpp:392同位只存地址编号，旧表实际调度仍待证 |
| 2136 | 0x004D8200 | `sub_4D8200` | 有正文 | `src/LegacyCatalogWindows.cpp:499 — Sub_4D8200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2137 | 0x004D8230 | `sub_4D8230` | 部分实现 | `src/LegacyCatalogWindows.cpp:510 — Sub_4D8230` | 正文已有；消费 CA0478 目录，但当前数组仅有 extern 声明，未见定义／生产；旧目录容量与生产待证；不以数组初值代运行期数据；本轮原IDA限定检索仅见首条已证记录及相关计数／图号的读取、取地址引用，未定位生产者；不外推间接写入不存在；本轮已加载IDA各段的十个已证W32地址字面量检索只命中同30处消费指令，未发现未建xref的地址存储槽；仍不排除动态地址／间接写入；LBR01原57C368表第82项(ID60)的57C9D4槽实际存本入口，槽为db而未建立入口xref；当前LegacyAudio.cpp:393同位只存地址编号，旧表实际调度仍待证 |
| 2138 | 0x004D88C0 | `sub_4D88C0` | 部分实现 | `src/LegacyCatalogWindows.cpp:650 — Sub_4D88C0` | 正文已有；消费 CA0478 目录，但当前数组仅有 extern 声明，未见定义／生产；旧目录容量与生产待证；不以数组初值代运行期数据；本轮原IDA限定检索仅见首条已证记录及相关计数／图号的读取、取地址引用，未定位生产者；不外推间接写入不存在；本轮已加载IDA各段的十个已证W32地址字面量检索只命中同30处消费指令，未发现未建xref的地址存储槽；仍不排除动态地址／间接写入；LBR01原57C368表第83项(ID61)的57C9E8槽实际存本入口，槽为db而未建立入口xref；当前LegacyAudio.cpp:394同位只存地址编号，旧表实际调度仍待证 |
| 2139 | 0x004D8D70 | `sub_4D8D70` | 有正文 | `src/LegacyUiRecord.cpp:3468 — Sub_4D8D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2140 | 0x004D8DA0 | `sub_4D8DA0` | 有正文 | `src/LegacyUiRecord.cpp:3479 — Sub_4D8DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2141 | 0x004D91E0 | `sub_4D91E0` | 有正文 | `src/LegacyUiRecord.cpp:3602 — Sub_4D91E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2142 | 0x004D9220 | `sub_4D9220` | 有正文 | `src/LegacyUiRecord.cpp:3613 — Sub_4D9220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2143 | 0x004DA400 | `sub_4DA400` | 有正文 | `src/LegacyUiRecord.cpp:3865 — Sub_4DA400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2144 | 0x004DA580 | `sub_4DA580` | 有正文 | `src/LegacyUiRecord.cpp:3901 — Sub_4DA580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2145 | 0x004DAA00 | `sub_4DAA00` | 有正文 | `src/LegacyUiRecord.cpp:4003 — Sub_4DAA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2146 | 0x004DAA40 | `sub_4DAA40` | 有正文 | `src/LegacyUiRecord.cpp:4014 — Sub_4DAA40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2147 | 0x004DBD30 | `sub_4DBD30` | 有正文 | `src/LegacyUiRecord.cpp:4280 — Sub_4DBD30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2148 | 0x004DC290 | `sub_4DC290` | 有正文 | `src/LegacyUiRecord.cpp:4397 — Sub_4DC290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2149 | 0x004DC7A0 | `sub_4DC7A0` | 有正文 | `src/LegacyUiRecord.cpp:4508 — Sub_4DC7A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2150 | 0x004DC7E0 | `sub_4DC7E0` | 有正文 | `src/LegacyUiRecord.cpp:4520 — Sub_4DC7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2151 | 0x004DDA30 | `sub_4DDA30` | 有正文 | `src/LegacyUiRecord.cpp:4768 — Sub_4DDA30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2152 | 0x004DDB20 | `sub_4DDB20` | 有正文 | `src/LegacyUiRecord.cpp:4791 — Sub_4DDB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2153 | 0x004DDF80 | `sub_4DDF80` | 有正文 | `src/LegacyUiRecord.cpp:4898 — Sub_4DDF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2154 | 0x004DDFC0 | `sub_4DDFC0` | 有正文 | `src/LegacyUiRecord.cpp:4910 — Sub_4DDFC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2155 | 0x004DF210 | `sub_4DF210` | 有正文 | `src/LegacyUiRecord.cpp:5159 — Sub_4DF210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2156 | 0x004DF300 | `sub_4DF300` | 有正文 | `src/LegacyUiRecord.cpp:5182 — Sub_4DF300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2157 | 0x004DF730 | `sub_4DF730` | 有正文 | `src/LegacyUiRecord.cpp:5289 — Sub_4DF730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2158 | 0x004E0100 | `sub_4E0100` | 有正文 | `src/ui/WindowRuntime.cpp:12920 — Sub_4E0100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2159 | 0x004E0270 | `sub_4E0270` | 有正文 | `src/LegacyUiRecord.cpp:6581 — Sub_4E0270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2160 | 0x004E02B0 | `sub_4E02B0` | 有正文 | `src/ui/WindowRuntime.cpp:15196 — Sub_4E02B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2161 | 0x004E0380 | `sub_4E0380` | 有正文 | `src/LegacyUiRecord.cpp:6387 — Sub_4E0380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2162 | 0x004E0BC0 | `sub_4E0BC0` | 有正文 | `src/LegacyUiRecord.cpp:6596 — Sub_4E0BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2163 | 0x004E0C70 | `sub_4E0C70` | 有正文 | `src/LegacyUiRecord.cpp:6618 — Sub_4E0C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2164 | 0x004E0D10 | `sub_4E0D10` | 有正文 | `src/LegacyUiRecord.cpp:6633 — Sub_4E0D10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2165 | 0x004E0FF0 | `sub_4E0FF0` | 有正文 | `src/LegacyUiRecord.cpp:6698 — Sub_4E0FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2166 | 0x004E10F0 | `sub_4E10F0` | 有正文 | `src/LegacyUiRecord.cpp:6721 — Sub_4E10F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2167 | 0x004E1840 | `sub_4E1840` | 有正文 | `src/ui/WindowRuntime.cpp:1912 — Sub_4E1840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2168 | 0x004E18F0 | `sub_4E18F0` | 有正文 | `src/LegacyUiRecord.cpp:6819 — Sub_4E18F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2169 | 0x004E1980 | `sub_4E1980` | 有正文 | `src/LegacyUiRecord.cpp:6836 — Sub_4E1980` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2170 | 0x004E19E0 | `sub_4E19E0` | 有正文 | `src/LegacyUiRecord.cpp:6848 — Sub_4E19E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2171 | 0x004E1B10 | `sub_4E1B10` | 有正文 | `src/LegacyUiRecord.cpp:6876 — Sub_4E1B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2172 | 0x004E1EE0 | `sub_4E1EE0` | 有正文 | `src/LegacyUiRecord.cpp:6933 — Sub_4E1EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2173 | 0x004E2030 | `sub_4E2030` | 有正文 | `src/LegacyUiRecord.cpp:6960 — Sub_4E2030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2174 | 0x004E28F0 | `sub_4E28F0` | 有正文 | `src/LegacyUiRecord.cpp:7208 — Sub_4E28F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2175 | 0x004E2D90 | `sub_4E2D90` | 有正文 | `src/LegacyUiRecord.cpp:7341 — Sub_4E2D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2176 | 0x004E3750 | `sub_4E3750` | 有正文 | `src/LegacyUiRecord.cpp:7499 — Sub_4E3750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2177 | 0x004E4020 | `sub_4E4020` | 有正文 | `src/ui/WindowRuntime.cpp:39391 — Sub_4E4020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2178 | 0x004E4460 | `sub_4E4460` | 有正文 | `src/LegacyUiRecord.cpp:7616 — Sub_4E4460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2179 | 0x004E4860 | `sub_4E4860` | 有正文 | `src/ui/WindowRuntime.cpp:39276 — Sub_4E4860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2180 | 0x004E4A50 | `sub_4E4A50` | 有正文 | `src/LegacyUiRecord.cpp:3302 — Sub_4E4A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2181 | 0x004E4E60 | `sub_4E4E60` | 有正文 | `src/ui/WindowRuntime.cpp:12874 — Sub_4E4E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2182 | 0x004E4E80 | `sub_4E4E80` | 有正文 | `src/ui/WindowRuntime.cpp:12914 — Sub_4E4E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2183 | 0x004E4EA0 | `sub_4E4EA0` | 部分实现 | `src/LegacyCatalogWindows.cpp:104 — Sub_4E4EA0` | 正文已有；CB1D1C 名单仅 extern，当前未见定义／生产；数量 D30134 与页数 CF46D0 仅初值与读取，容量和原生产待证；不以数组初值代运行期数据；本轮原IDA限定检索仅见首条已证记录及相关计数／图号的读取、取地址引用，未定位生产者；不外推间接写入不存在；本轮已加载IDA各段的十个已证W32地址字面量检索只命中同30处消费指令，未发现未建xref的地址存储槽；仍不排除动态地址／间接写入；LBR01原57C368表第48项(ID51)的57C72C槽实际存本入口，槽为db而未建立入口xref；当前LegacyAudio.cpp:359同位只存地址编号，旧表实际调度仍待证 |
| 2184 | 0x004E52B0 | `sub_4E52B0` | 有正文 | `src/ui/WindowRuntime.cpp:12954 — Sub_4E52B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2185 | 0x004E54E0 | `sub_4E54E0` | 有正文 | `src/ui/WindowRuntime.cpp:2149 — Sub_4E54E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2186 | 0x004E5600 | `sub_4E5600` | 有正文 | `src/ui/WindowRuntime.cpp:2180 — Sub_4E5600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2187 | 0x004E5620 | `sub_4E5620` | 有正文 | `src/ui/WindowRuntime.cpp:12848 — Sub_4E5620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2188 | 0x004E5710 | `sub_4E5710` | 有正文 | `src/ui/WindowRuntime.cpp:15346 — Sub_4E5710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2189 | 0x004E5770 | `sub_4E5770` | 有正文 | `src/ui/WindowRuntime.cpp:15359 — Sub_4E5770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2190 | 0x004E5830 | `sub_4E5830` | 有正文 | `src/ui/WindowRuntime.cpp:15381 — Sub_4E5830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2191 | 0x004E5A40 | `sub_4E5A40` | 有正文 | `src/ui/WindowRuntime.cpp:15422 — Sub_4E5A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2192 | 0x004E5CD0 | `sub_4E5CD0` | 有正文 | `src/ui/WindowRuntime.cpp:15460 — Sub_4E5CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2193 | 0x004E5DA0 | `sub_4E5DA0` | 有正文 | `src/ui/WindowRuntime.cpp:1897 — Sub_4E5DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2194 | 0x004E5E40 | `sub_4E5E40` | 有正文 | `src/LegacyUiRecord.cpp:7710 — Sub_4E5E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2195 | 0x004E6CC0 | `sub_4E6CC0` | 有正文 | `src/LegacyUiRecord.cpp:7998 — Sub_4E6CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2196 | 0x004E6D70 | `sub_4E6D70` | 有正文 | `src/LegacyUiRecord.cpp:8020 — Sub_4E6D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2197 | 0x004E6E10 | `sub_4E6E10` | 有正文 | `src/LegacyUiRecord.cpp:8035 — Sub_4E6E10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2198 | 0x004E70B0 | `sub_4E70B0` | 有正文 | `src/LegacyUiRecord.cpp:8094 — Sub_4E70B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2199 | 0x004E7140 | `sub_4E7140` | 有正文 | `src/LegacyUiRecord.cpp:8111 — Sub_4E7140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2200 | 0x004E71A0 | `sub_4E71A0` | 有正文 | `src/LegacyUiRecord.cpp:8123 — Sub_4E71A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2201 | 0x004E7290 | `sub_4E7290` | 有正文 | `src/LegacyUiRecord.cpp:8146 — Sub_4E7290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2202 | 0x004E7380 | `sub_4E7380` | 有正文 | `src/LegacyUiRecord.cpp:8169 — Sub_4E7380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2203 | 0x004E7740 | `sub_4E7740` | 有正文 | `src/LegacyUiRecord.cpp:8226 — Sub_4E7740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2204 | 0x004E7870 | `sub_4E7870` | 有正文 | `src/LegacyUiRecord.cpp:8252 — Sub_4E7870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2205 | 0x004E7FC0 | `sub_4E7FC0` | 有正文 | `src/LegacyUiRecord.cpp:8350 — Sub_4E7FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2206 | 0x004E81B0 | `sub_4E81B0` | 有正文 | `src/LegacyUiRecord.cpp:8395 — Sub_4E81B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2207 | 0x004E82F0 | `sub_4E82F0` | 有正文 | `src/LegacyUiRecord.cpp:8431 — Sub_4E82F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2208 | 0x004E83D0 | `sub_4E83D0` | 有正文 | `src/LegacyUiRecord.cpp:8450 — Sub_4E83D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2209 | 0x004E8920 | `sub_4E8920` | 有正文 | `src/LegacyUiRecord.cpp:8594 — Sub_4E8920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2210 | 0x004E8AD0 | `sub_4E8AD0` | 有正文 | `src/LegacyUiRecord.cpp:8647 — Sub_4E8AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2211 | 0x004E9280 | `sub_4E9280` | 有正文 | `src/LegacyUiRecord.cpp:8742 — Sub_4E9280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2212 | 0x004E92A0 | `sub_4E92A0` | 有正文 | `src/LegacyUiRecord.cpp:8749 — Sub_4E92A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2213 | 0x004E97C0 | `sub_4E97C0` | 有正文 | `src/LegacyUiRecord.cpp:8877 — Sub_4E97C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2214 | 0x004E97E0 | `sub_4E97E0` | 有正文 | `src/LegacyUiRecord.cpp:8884 — Sub_4E97E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2215 | 0x004E9C60 | `sub_4E9C60` | 有正文 | `src/LegacyUiRecord.cpp:9014 — Sub_4E9C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2216 | 0x004E9C80 | `sub_4E9C80` | 有正文 | `src/LegacyUiRecord.cpp:9021 — Sub_4E9C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2217 | 0x004EA130 | `sub_4EA130` | 有正文 | `src/LegacyUiRecord.cpp:9141 — Sub_4EA130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2218 | 0x004EA1B0 | `sub_4EA1B0` | 有正文 | `src/LegacyUiRecord.cpp:9153 — Sub_4EA1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2219 | 0x004EA670 | `sub_4EA670` | 有正文 | `src/ui/WindowRuntime.cpp:6208 — Sub_4EA670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2220 | 0x004EA690 | `sub_4EA690` | 有正文 | `src/LegacyUiRecord.cpp:9271 — Sub_4EA690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2221 | 0x004EAA50 | `sub_4EAA50` | 有正文 | `src/LegacyUiRecord.cpp:9668 — Sub_4EAA50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2222 | 0x004EAAA0 | `sub_4EAAA0` | 有正文 | `src/LegacyUiRecord.cpp:9687 — Sub_4EAAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2223 | 0x004EB120 | `sub_4EB120` | 有正文 | `src/LegacyUiRecord.cpp:9807 — Sub_4EB120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2224 | 0x004EB1C0 | `sub_4EB1C0` | 有正文 | `src/LegacyUiRecord.cpp:9823 — Sub_4EB1C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2225 | 0x004EB890 | `sub_4EB890` | 有正文 | `src/LegacyUiRecord.cpp:9571 — Sub_4EB890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2226 | 0x004EBAA0 | `sub_4EBAA0` | 有正文 | `src/LegacyUiRecord.cpp:9611 — Sub_4EBAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2227 | 0x004EBD20 | `sub_4EBD20` | 有正文 | `src/LegacyUiRecord.cpp:9978 — Sub_4EBD20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2228 | 0x004EBF70 | `sub_4EBF70` | 有正文 | `src/YobiRuntime.cpp:35657 — Sub_4EBF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2229 | 0x004EBFD0 | `sub_4EBFD0` | 有正文 | `src/YobiRuntime.cpp:35676 — Sub_4EBFD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2230 | 0x004EC030 | `sub_4EC030` | 有正文 | `src/world/MailState.cpp:164 — CompareContactIndices_4EC030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2231 | 0x004EC1A0 | `sub_4EC1A0` | 有正文 | `src/world/MailState.cpp:283 — Sub_4EC1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2232 | 0x004EC1D0 | `sub_4EC1D0` | 有正文 | `src/LegacyUiRecord.cpp:9347 — Sub_4EC1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2233 | 0x004EC240 | `sub_4EC240` | 有正文 | `src/LegacyUiRecord.cpp:9370 — Sub_4EC240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2234 | 0x004ECDF0 | `sub_4ECDF0` | 有正文 | `src/LegacyUiRecord.cpp:1852 — Sub_4ECDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2235 | 0x004ECE00 | `sub_4ECE00` | 有正文 | `src/LegacyUiRecord.cpp:11598 — Sub_4ECE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2236 | 0x004ED010 | `sub_4ED010` | 有正文 | `src/LegacyUiRecord.cpp:1860 — Sub_4ED010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2237 | 0x004ED020 | `sub_4ED020` | 有正文 | `src/LegacyUiRecord.cpp:11653 — Sub_4ED020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2238 | 0x004ED670 | `sub_4ED670` | 有正文 | `src/world/SceneState.cpp:6176 — Sub_4ED670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2239 | 0x004ED6B0 | `sub_4ED6B0` | 有正文 | `src/ui/WindowRuntime.cpp:34450 — Sub_4ED6B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2240 | 0x004EDB20 | `sub_4EDB20` | 有正文 | `src/LegacyUiRecord.cpp:11783 — Sub_4EDB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2241 | 0x004EE130 | `sub_4EE130` | 有正文 | `src/YobiRuntime.cpp:35442 — Sub_4EE130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2242 | 0x004EE380 | `sub_4EE380` | 有正文 | `src/YobiRuntime.cpp:35800 — Sub_4EE380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2243 | 0x004EE670 | `sub_4EE670` | 有正文 | `src/YobiRuntime.cpp:35502 — Sub_4EE670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2244 | 0x004EE9E0 | `sub_4EE9E0` | 有正文 | `src/YobiRuntime.cpp:35423 — Sub_4EE9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2245 | 0x004EEA40 | `sub_4EEA40` | 有正文 | `src/ui/WindowRuntime.cpp:35085 — Sub_4EEA40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2246 | 0x004EF0F0 | `sub_4EF0F0` | 有正文 | `src/LegacyUiRecord.cpp:11934 — Sub_4EF0F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2247 | 0x004EF5F0 | `sub_4EF5F0` | 有正文 | `src/YobiRuntime.cpp:35779 — Sub_4EF5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2248 | 0x004EF710 | `sub_4EF710` | 有正文 | `src/ui/WindowRuntime.cpp:35488 — Sub_4EF710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2249 | 0x004EF750 | `sub_4EF750` | 有正文 | `src/ui/WindowRuntime.cpp:35387 — Sub_4EF750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2250 | 0x004EFB70 | `sub_4EFB70` | 有正文 | `src/LegacyUiRecord.cpp:2992 — Sub_4EFB70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2251 | 0x004F0200 | `sub_4F0200` | 有正文 | `src/ui/WindowRuntime.cpp:39146 — Sub_4F0200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2252 | 0x004F0460 | `sub_4F0460` | 有正文 | `src/LegacyUiRecord.cpp:3135 — Sub_4F0460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2253 | 0x004F0B40 | `sub_4F0B40` | 有正文 | `src/LegacyUiRecord.cpp:12054 — Sub_4F0B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2254 | 0x004F0CD0 | `sub_4F0CD0` | 有正文 | `src/LegacyUiRecord.cpp:3402 — Sub_4F0CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2255 | 0x004F0E20 | `sub_4F0E20` | 有正文 | `src/LegacyUiRecord.cpp:3435 — Sub_4F0E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2256 | 0x004F0F70 | `sub_4F0F70` | 有正文 | `src/LegacyUiRecord.cpp:12099 — Sub_4F0F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2257 | 0x004F1800 | `sub_4F1800` | 有正文 | `src/ui/WindowRuntime.cpp:34081 — Sub_4F1800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2258 | 0x004F1C40 | `sub_4F1C40` | 有正文 | `src/LegacyUiRecord.cpp:12229 — Sub_4F1C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2259 | 0x004F2130 | `sub_4F2130` | 有正文 | `src/LegacyUiRecord.cpp:12458 — Sub_4F2130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2260 | 0x004F24A0 | `sub_4F24A0` | 有正文 | `src/LegacyUiRecord.cpp:12532 — Sub_4F24A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2261 | 0x004F25C0 | `sub_4F25C0` | 有正文 | `src/LegacyUiRecord.cpp:12558 — Sub_4F25C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2262 | 0x004F2650 | `sub_4F2650` | 有正文 | `src/LegacyUiRecord.cpp:12578 — Sub_4F2650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2263 | 0x004F2720 | `sub_4F2720` | 有正文 | `src/LegacyUiRecord.cpp:12604 — Sub_4F2720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2264 | 0x004F2A30 | `sub_4F2A30` | 有正文 | `src/LegacyUiRecord.cpp:12666 — Sub_4F2A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2265 | 0x004F39F0 | `sub_4F39F0` | 有正文 | `src/ui/WindowRuntime.cpp:33847 — Sub_4F39F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2266 | 0x004F3ED0 | `sub_4F3ED0` | 有正文 | `src/LegacyUiRecord.cpp:12340 — Sub_4F3ED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2267 | 0x004F4430 | `sub_4F4430` | 有正文 | `src/ui/WindowRuntime.cpp:31898 — Sub_4F4430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2268 | 0x004F4470 | `sub_4F4470` | 有正文 | `src/LegacyUiRecord.cpp:13035 — Sub_4F4470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2269 | 0x004F4630 | `sub_4F4630` | 有正文 | `src/ui/WindowRuntime.cpp:1927 — Sub_4F4630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2270 | 0x004F4690 | `sub_4F4690` | 有正文 | `src/LegacyUiRecord.cpp:13115 — Sub_4F4690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2271 | 0x004F4720 | `sub_4F4720` | 有正文 | `src/LegacyUiRecord.cpp:13140 — Sub_4F4720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2272 | 0x004F4740 | `sub_4F4740` | 有正文 | `src/LegacyUiRecord.cpp:13152 — Sub_4F4740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2273 | 0x004F4760 | `sub_4F4760` | 有正文 | `src/LegacyUiRecord.cpp:13159 — Sub_4F4760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2274 | 0x004F4790 | `sub_4F4790` | 有正文 | `src/LegacyUiRecord.cpp:13171 — Sub_4F4790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2275 | 0x004F50D0 | `sub_4F50D0` | 有正文 | `src/LegacyUiRecord.cpp:13393 — Sub_4F50D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2276 | 0x004F5170 | `sub_4F5170` | 有正文 | `src/LegacyUiRecord.cpp:13414 — Sub_4F5170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2277 | 0x004F51A0 | `sub_4F51A0` | 有正文 | `src/LegacyUiRecord.cpp:13423 — Sub_4F51A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2278 | 0x004F51D0 | `sub_4F51D0` | 有正文 | `src/LegacyUiRecord.cpp:13433 — Sub_4F51D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2279 | 0x004F5A60 | `sub_4F5A60` | 有正文 | `src/LegacyUiRecord.cpp:14081 — Sub_4F5A60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2280 | 0x004F61C0 | `sub_4F61C0` | 有正文 | `src/LegacyUiRecord.cpp:14225 — Sub_4F61C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2281 | 0x004F6220 | `sub_4F6220` | 有正文 | `src/LegacyUiRecord.cpp:14242 — Sub_4F6220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2282 | 0x004F6BF0 | `sub_4F6BF0` | 有正文 | `src/LegacyUiRecord.cpp:14407 — Sub_4F6BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2283 | 0x004F6C60 | `sub_4F6C60` | 有正文 | `src/LegacyUiRecord.cpp:14421 — Sub_4F6C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2284 | 0x004F6CA0 | `sub_4F6CA0` | 有正文 | `src/LegacyUiRecord.cpp:14433 — Sub_4F6CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2285 | 0x004F7140 | `sub_4F7140` | 有正文 | `src/LegacyUiRecord.cpp:14542 — Sub_4F7140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2286 | 0x004F7150 | `sub_4F7150` | 有正文 | `src/LegacyUiRecord.cpp:14551 — Sub_4F7150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2287 | 0x004F7870 | `sub_4F7870` | 有正文 | `src/LegacyUiRecord.cpp:14687 — Sub_4F7870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2288 | 0x004F8340 | `sub_4F8340` | 有正文 | `src/LegacyUiRecord.cpp:14869 — Sub_4F8340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2289 | 0x004F83B0 | `sub_4F83B0` | 有正文 | `src/LegacyUiRecord.cpp:14888 — Sub_4F83B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2290 | 0x004F8550 | `sub_4F8550` | 有正文 | `src/LegacyUiRecord.cpp:14944 — Sub_4F8550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2291 | 0x004F8E40 | `sub_4F8E40` | 有正文 | `src/LegacyUiRecord.cpp:15057 — Sub_4F8E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2292 | 0x004F9B10 | `sub_4F9B10` | 有正文 | `src/LegacyUiRecord.cpp:15180 — Sub_4F9B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2293 | 0x004F9FF0 | `sub_4F9FF0` | 有正文 | `src/YobiRuntime.cpp:35638 — Sub_4F9FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2294 | 0x004FA050 | `sub_4FA050` | 有正文 | `src/LegacyUiRecord.cpp:15665 — Sub_4FA050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2295 | 0x004FA350 | `sub_4FA350` | 有正文 | `src/LegacyUiRecord.cpp:15625 — Sub_4FA350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2296 | 0x004FA8E0 | `sub_4FA8E0` | 有正文 | `src/world/SceneState.cpp:9376 — Sub_4FA8E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2297 | 0x004FACE0 | `sub_4FACE0` | 有正文 | `src/YobiRuntime.cpp:35738 — Sub_4FACE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2298 | 0x004FAED0 | `sub_4FAED0` | 有正文 | `src/LegacyUiRecord.cpp:15729 — Sub_4FAED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2299 | 0x004FAEF0 | `sub_4FAEF0` | 有正文 | `src/LegacyUiRecord.cpp:15738 — Sub_4FAEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2300 | 0x004FB8F0 | `MouseEvent::resetDrag` | 有正文 | `src/ui/WindowRuntime.cpp:2127 — Sub_4FB8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2301 | 0x004FB920 | `sub_4FB920` | 有正文 | `src/ui/WindowRuntime.cpp:2139 — Sub_4FB920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2302 | 0x004FB940 | `sub_4FB940` | 有正文 | `src/LegacyUiRecord.cpp:12993 — Sub_4FB940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2303 | 0x004FBA30 | `sub_4FBA30` | 有正文 | `src/ui/WindowRuntime.cpp:7884 — Sub_4FBA30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2304 | 0x004FBB30 | `sub_4FBB30` | 有正文 | `src/LegacyUiRecord.cpp:16005 — Sub_4FBB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2305 | 0x004FBB40 | `sub_4FBB40` | 有正文 | `src/LegacyUiRecord.cpp:16011 — Sub_4FBB40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2306 | 0x004FC420 | `sub_4FC420` | 有正文 | `src/LegacyUiRecord.cpp:17054 — Sub_4FC420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2307 | 0x004FC430 | `sub_4FC430` | 有正文 | `src/LegacyUiRecord.cpp:17060 — Sub_4FC430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2308 | 0x004FC7F0 | `sub_4FC7F0` | 有正文 | `src/LegacyUiRecord.cpp:1748 — Sub_4FC7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2309 | 0x004FC810 | `sub_4FC810` | 有正文 | `src/LegacyUiRecord.cpp:1756 — Sub_4FC810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2310 | 0x004FC830 | `sub_4FC830` | 有正文 | `src/LegacyUiRecord.cpp:1764 — Sub_4FC830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2311 | 0x004FC850 | `sub_4FC850` | 有正文 | `src/LegacyUiRecord.cpp:1772 — Sub_4FC850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2312 | 0x004FC870 | `sub_4FC870` | 有正文 | `src/LegacyUiRecord.cpp:1780 — Sub_4FC870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2313 | 0x004FC890 | `sub_4FC890` | 有正文 | `src/LegacyUiRecord.cpp:1788 — Sub_4FC890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2314 | 0x004FC8B0 | `sub_4FC8B0` | 有正文 | `src/LegacyUiRecord.cpp:1796 — Sub_4FC8B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2315 | 0x004FC8D0 | `sub_4FC8D0` | 有正文 | `src/LegacyUiRecord.cpp:1804 — Sub_4FC8D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2316 | 0x004FC8F0 | `sub_4FC8F0` | 有正文 | `src/LegacyUiRecord.cpp:17196 — Sub_4FC8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2317 | 0x004FCB90 | `sub_4FCB90` | 有正文 | `src/LegacyUiRecord.cpp:17252 — Sub_4FCB90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2318 | 0x004FCBA0 | `sub_4FCBA0` | 有正文 | `src/LegacyUiRecord.cpp:17258 — Sub_4FCBA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2319 | 0x004FD1A0 | `sub_4FD1A0` | 有正文 | `src/LegacyUiRecord.cpp:17484 — Sub_4FD1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2320 | 0x004FD4C0 | `sub_4FD4C0` | 有正文 | `src/DisplaySettings.cpp:15 — Sub_4FD4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2321 | 0x004FD510 | `sub_4FD510` | 有正文 | `src/DisplaySettings.cpp:27 — Sub_4FD510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2322 | 0x004FD550 | `sub_4FD550` | 有正文 | `src/DisplaySettings.cpp:39 — Sub_4FD550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2323 | 0x004FD5A0 | `sub_4FD5A0` | 有正文 | `src/DisplaySettings.cpp:53 — Sub_4FD5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2324 | 0x004FD5C0 | `sub_4FD5C0` | 有正文 | `src/DisplaySettings.cpp:63 — Sub_4FD5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2325 | 0x004FD5F0 | `sub_4FD5F0` | 有正文 | `src/DisplaySettings.cpp:72 — Sub_4FD5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2326 | 0x004FD640 | `sub_4FD640` | 有正文 | `src/DisplaySettings.cpp:85 — Sub_4FD640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2327 | 0x004FD690 | `sub_4FD690` | 有正文 | `src/LegacyUiRecord.cpp:15535 — Sub_4FD690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2328 | 0x004FD6A0 | `sub_4FD6A0` | 有正文 | `src/LegacyUiRecord.cpp:15543 — Sub_4FD6A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2329 | 0x004FD9F0 | `sub_4FD9F0` | 有正文 | `src/LegacyUiRecord.cpp:17547 — Sub_4FD9F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2330 | 0x004FDA00 | `sub_4FDA00` | 有正文 | `src/LegacyUiRecord.cpp:17553 — Sub_4FDA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2331 | 0x004FDBD0 | `sub_4FDBD0` | 有正文 | `src/LegacyUiRecord.cpp:17623 — Sub_4FDBD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2332 | 0x004FDD70 | `sub_4FDD70` | 有正文 | `src/DisplaySettings.cpp:105 — Sub_4FDD70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2333 | 0x004FDDC0 | `sub_4FDDC0` | 有正文 | `src/DisplaySettings.cpp:119 — Sub_4FDDC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2334 | 0x004FDE10 | `sub_4FDE10` | 有正文 | `src/LegacyUiRecord.cpp:17671 — Sub_4FDE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2335 | 0x004FDE20 | `sub_4FDE20` | 有正文 | `src/LegacyUiRecord.cpp:17677 — Sub_4FDE20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2336 | 0x004FE0E0 | `sub_4FE0E0` | 有正文 | `src/LegacyUiRecord.cpp:17783 — Sub_4FE0E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2337 | 0x004FE320 | `sub_4FE320` | 有正文 | `src/DisplaySettings.cpp:133 — Sub_4FE320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2338 | 0x004FE370 | `sub_4FE370` | 有正文 | `src/DisplaySettings.cpp:146 — Sub_4FE370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2339 | 0x004FE3B0 | `sub_4FE3B0` | 有正文 | `src/DisplaySettings.cpp:159 — Sub_4FE3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2340 | 0x004FE3E0 | `sub_4FE3E0` | 有正文 | `src/DisplaySettings.cpp:98 — Sub_4FE3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2341 | 0x004FE3F0 | `sub_4FE3F0` | 有正文 | `src/DisplaySettings.cpp:168 — Sub_4FE3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2342 | 0x004FE440 | `sub_4FE440` | 有正文 | `src/DisplaySettings.cpp:181 — Sub_4FE440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2343 | 0x004FE490 | `sub_4FE490` | 有正文 | `src/LegacyUiRecord.cpp:17842 — Sub_4FE490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2344 | 0x004FE4A0 | `sub_4FE4A0` | 有正文 | `src/LegacyUiRecord.cpp:17848 — Sub_4FE4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2345 | 0x004FE860 | `sub_4FE860` | 有正文 | `src/LegacyUiRecord.cpp:17990 — Sub_4FE860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2346 | 0x004FEAC0 | `sub_4FEAC0` | 有正文 | `src/DisplaySettings.cpp:194 — Sub_4FEAC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2347 | 0x004FEB00 | `sub_4FEB00` | 有正文 | `src/LegacyUiRecord.cpp:18053 — Sub_4FEB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2348 | 0x004FEB10 | `sub_4FEB10` | 有正文 | `src/LegacyUiRecord.cpp:18059 — Sub_4FEB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2349 | 0x004FEF30 | `sub_4FEF30` | 有正文 | `src/LegacyUiRecord.cpp:18225 — Sub_4FEF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2350 | 0x004FEFF0 | `sub_4FEFF0` | 有正文 | `src/DisplaySettings.cpp:204 — Sub_4FEFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2351 | 0x004FF020 | `sub_4FF020` | 有正文 | `src/LegacyUiRecord.cpp:18256 — Sub_4FF020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2352 | 0x004FF030 | `sub_4FF030` | 有正文 | `src/LegacyUiRecord.cpp:18262 — Sub_4FF030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2353 | 0x004FF1A0 | `sub_4FF1A0` | 有正文 | `src/LegacyUiRecord.cpp:18319 — Sub_4FF1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2354 | 0x004FF310 | `sub_4FF310` | 有正文 | `src/DisplaySettings.cpp:212 — Sub_4FF310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2355 | 0x004FF340 | `sub_4FF340` | 有正文 | `src/LegacyUiRecord.cpp:18364 — Sub_4FF340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2356 | 0x004FF350 | `sub_4FF350` | 有正文 | `src/LegacyUiRecord.cpp:18370 — Sub_4FF350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2357 | 0x004FF4C0 | `sub_4FF4C0` | 有正文 | `src/LegacyUiRecord.cpp:18427 — Sub_4FF4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2358 | 0x004FF620 | `sub_4FF620` | 有正文 | `src/DisplaySettings.cpp:221 — Sub_4FF620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2359 | 0x004FF650 | `sub_4FF650` | 有正文 | `src/LegacyUiRecord.cpp:16139 — Sub_4FF650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2360 | 0x00500370 | `sub_500370` | 有正文 | `src/LegacyUiRecord.cpp:16439 — Sub_500370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2361 | 0x005006F0 | `sub_5006F0` | 有正文 | `src/LegacyUiRecord.cpp:16537 — Sub_5006F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2362 | 0x005008C0 | `sub_5008C0` | 有正文 | `src/LegacyUiRecord.cpp:16605 — Sub_5008C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2363 | 0x005014A0 | `sub_5014A0` | 有正文 | `src/LegacyUiRecord.cpp:16762 — Sub_5014A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2364 | 0x00501F60 | `sub_501F60` | 有正文 | `src/LegacyUiRecord.cpp:1868 — Sub_501F60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2365 | 0x00501FD0 | `sub_501FD0` | 有正文 | `src/LegacyUiRecord.cpp:16872 — Sub_501FD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2366 | 0x00502930 | `sub_502930` | 有正文 | `src/LegacyUiRecord.cpp:18472 — Sub_502930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2367 | 0x00502940 | `sub_502940` | 有正文 | `src/LegacyUiRecord.cpp:18478 — Sub_502940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2368 | 0x005029B0 | `sub_5029B0` | 有正文 | `src/LegacyUiRecord.cpp:18502 — Sub_5029B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2369 | 0x00502B80 | `sub_502B80` | 有正文 | `src/LegacyUiRecord.cpp:18551 — Sub_502B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2370 | 0x00502C30 | `sub_502C30` | 有正文 | `src/LegacyUiRecord.cpp:18589 — Sub_502C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2371 | 0x00502C40 | `sub_502C40` | 有正文 | `src/LegacyUiRecord.cpp:18595 — Sub_502C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2372 | 0x00502C60 | `sub_502C60` | 有正文 | `src/LegacyUiRecord.cpp:18604 — Sub_502C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2373 | 0x00502C70 | `sub_502C70` | 有正文 | `src/LegacyUiRecord.cpp:18610 — Sub_502C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2374 | 0x00502C80 | `sub_502C80` | 有正文 | `src/LegacyUiRecord.cpp:18618 — Sub_502C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2375 | 0x00502C90 | `sub_502C90` | 有正文 | `src/LegacyUiRecord.cpp:18624 — Sub_502C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2376 | 0x00502CA0 | `sub_502CA0` | 有正文 | `src/LegacyUiRecord.cpp:18632 — Sub_502CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2377 | 0x00502CB0 | `sub_502CB0` | 有正文 | `src/LegacyUiRecord.cpp:18638 — Sub_502CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2378 | 0x00502D20 | `sub_502D20` | 有正文 | `src/LegacyUiRecord.cpp:18658 — Sub_502D20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2379 | 0x00502D30 | `sub_502D30` | 有正文 | `src/LegacyUiRecord.cpp:18664 — Sub_502D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2380 | 0x00502DA0 | `sub_502DA0` | 有正文 | `src/LegacyUiRecord.cpp:18670 — Sub_502DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2381 | 0x00502DB0 | `sub_502DB0` | 有正文 | `src/LegacyUiRecord.cpp:18676 — Sub_502DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2382 | 0x00502E20 | `sub_502E20` | 有正文 | `src/LegacyUiRecord.cpp:18682 — Sub_502E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2383 | 0x00502E30 | `sub_502E30` | 有正文 | `src/LegacyUiRecord.cpp:18688 — Sub_502E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2384 | 0x00502EA0 | `sub_502EA0` | 有正文 | `src/LegacyUiRecord.cpp:18694 — Sub_502EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2385 | 0x00502EB0 | `sub_502EB0` | 有正文 | `src/LegacyUiRecord.cpp:18700 — Sub_502EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2386 | 0x00502F20 | `sub_502F20` | 有正文 | `src/LegacyUiRecord.cpp:18706 — Sub_502F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2387 | 0x00502F30 | `sub_502F30` | 有正文 | `src/LegacyUiRecord.cpp:18712 — Sub_502F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2388 | 0x00502FA0 | `sub_502FA0` | 有正文 | `src/LegacyUiRecord.cpp:18718 — Sub_502FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2389 | 0x00502FB0 | `sub_502FB0` | 有正文 | `src/LegacyUiRecord.cpp:18724 — Sub_502FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2390 | 0x00503020 | `sub_503020` | 有正文 | `src/LegacyUiRecord.cpp:18730 — Sub_503020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2391 | 0x00503030 | `sub_503030` | 有正文 | `src/LegacyUiRecord.cpp:18736 — Sub_503030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2392 | 0x005030A0 | `sub_5030A0` | 有正文 | `src/LegacyUiRecord.cpp:18742 — Sub_5030A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2393 | 0x00503120 | `sub_503120` | 有正文 | `src/LegacyUiRecord.cpp:18764 — Sub_503120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2394 | 0x00503160 | `sub_503160` | 有正文 | `src/LegacyUiRecord.cpp:18775 — Sub_503160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2395 | 0x00503440 | `sub_503440` | 有正文 | `src/LegacyUiRecord.cpp:18883 — Sub_503440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2396 | 0x00503590 | `sub_503590` | 有正文 | `src/LegacyUiRecord.cpp:18937 — Sub_503590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2397 | 0x005035E0 | `sub_5035E0` | 有正文 | `src/LegacyUiRecord.cpp:18952 — Sub_5035E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2398 | 0x005036F0 | `sub_5036F0` | 有正文 | `src/LoginInput.cpp:201 — QueryLoginControlState_5036F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2399 | 0x00503780 | `sub_503780` | 有正文 | `src/LoginInput.cpp:255 — DrawLoginControl_503780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2400 | 0x005038B0 | `sub_5038B0` | 有正文 | `src/LegacyUiRecord.cpp:18991 — Sub_5038B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2401 | 0x00503990 | `sub_503990` | 有正文 | `src/CharacterModalButton.cpp:17 — Sub_503990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2402 | 0x00503A20 | `sub_503A20` | 有正文 | `src/CharacterModalButton.cpp:72 — Sub_503A20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2403 | 0x00503AB0 | `sub_503AB0` | 有正文 | `src/Input.cpp:823 — ExtractLegacyLine_503AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2404 | 0x00503B70 | `sub_503B70` | 有正文 | `src/LegacyUiRecord.cpp:15368 — Sub_503B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2405 | 0x00503BC0 | `sub_503BC0` | 有正文 | `src/LegacyUiRecord.cpp:15378 — Sub_503BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2406 | 0x00503BE0 | `sub_503BE0` | 有正文 | `src/LegacyUiRecord.cpp:15385 — Sub_503BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2407 | 0x00503C30 | `sub_503C30` | 有正文 | `src/LegacyUiRecord.cpp:15399 — Sub_503C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2408 | 0x00503C40 | `sub_503C40` | 有正文 | `src/LegacyUiRecord.cpp:15405 — Sub_503C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2409 | 0x00503C50 | `sub_503C50` | 有正文 | `src/LegacyUiRecord.cpp:15411 — Sub_503C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2410 | 0x00503C60 | `sub_503C60` | 有正文 | `src/LegacyUiRecord.cpp:15417 — Sub_503C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2411 | 0x00503C70 | `sub_503C70` | 有正文 | `src/LegacyUiRecord.cpp:15423 — Sub_503C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2412 | 0x00503C80 | `sub_503C80` | 有正文 | `src/render/SdlPresenter.cpp:543 — TakeScreenshotThrottled_503C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2413 | 0x00503CB0 | `sub_503CB0` | 有正文 | `src/ui/WindowRuntime.cpp:15492 — Sub_503CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2414 | 0x00503CD0 | `sub_503CD0` | 有正文 | `src/ui/WindowRuntime.cpp:15504 — Sub_503CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2415 | 0x00503E40 | `sub_503E40` | 有正文 | `src/Input.cpp:953 — IsCurrentContext_503E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2416 | 0x00503E60 | `sub_503E60` | 有正文 | `src/Input.cpp:1090 — IsChatContext_503E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2417 | 0x00503E80 | `sub_503E80` | 有正文 | `src/ui/WindowRuntime.cpp:45796 — ProcessSceneWindows_503E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2418 | 0x00504100 | `sub_504100` | 有正文 | `src/world/SceneState.cpp:10523 — Sub_504100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2419 | 0x00504120 | `Window::Render` | 有正文 | `src/ui/WindowRuntime.cpp:45027 — RenderWindows_504120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2420 | 0x00504180 | `sub_504180` | 有正文 | `src/LegacyAudio.cpp:796 — Sub_504180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2421 | 0x005042D0 | `sub_5042D0` | 有正文 | `src/LegacyAudio.cpp:872 — Sub_5042D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2422 | 0x00504380 | `sub_504380` | 有正文 | `src/LegacyAudio.cpp:912 — Sub_504380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2423 | 0x005043D0 | `sub_5043D0` | 有正文 | `src/LegacyAudio.cpp:930 — Sub_5043D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2424 | 0x00504410 | `sub_504410` | 有正文 | `src/LegacyAudio.cpp:777 — Sub_504410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2425 | 0x00504460 | `sub_504460` | 有正文 | `src/LegacyAudio.cpp:761 — Sub_504460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2426 | 0x005044C0 | `sub_5044C0` | 有正文 | `src/LegacyAudio.cpp:740 — Sub_5044C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2427 | 0x005044E0 | `sub_5044E0` | 有正文 | `src/YobiRuntime.cpp:34511 — Sub_5044E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2428 | 0x005046D0 | `sub_5046D0` | 有正文 | `src/YobiRuntime.cpp:1358 — UpdateModalAction_5046D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2429 | 0x005049A0 | `sub_5049A0` | 有正文 | `src/LegacyHitTest.cpp:12 — Sub_5049A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2430 | 0x00504A30 | `sub_504A30` | 有正文 | `src/world/SceneState.cpp:10504 — Sub_504A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2431 | 0x00504A50 | `sub_504A50` | 有正文 | `src/world/SceneState.cpp:6193 — Sub_504A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2432 | 0x00504AF0 | `sub_504AF0` | 有正文 | `src/LegacyAudio.cpp:860 — Sub_504AF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2433 | 0x00504B10 | `sub_504B10` | 有正文 | `src/world/SceneState.cpp:8623 — Sub_504B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2434 | 0x00504B60 | `sub_504B60` | 有正文 | `src/world/SceneState.cpp:8639 — Sub_504B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2435 | 0x00504BA0 | `sub_504BA0` | 有正文 | `src/LegacyUiRecord.cpp:13015 — Sub_504BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2436 | 0x00504C50 | `sub_504C50` | 有正文 | `src/YobiRuntime.cpp:35582 — Sub_504C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2437 | 0x00504EE0 | `sub_504EE0` | 有正文 | `src/LegacyUiRecord.cpp:13611 — Sub_504EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2438 | 0x00504F10 | `sub_504F10` | 有正文 | `src/world/SceneState.cpp:3412 — Sub_504F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2439 | 0x00504FA0 | `sub_504FA0` | 有正文 | `src/LegacyUiRecord.cpp:13621 — Sub_504FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2440 | 0x00505040 | `sub_505040` | 有正文 | `src/world/SceneState.cpp:3441 — Sub_505040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2441 | 0x005050B0 | `sub_5050B0` | 有正文 | `src/world/SceneState.cpp:3458 — Sub_5050B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2442 | 0x00505110 | `sub_505110` | 有正文 | `src/world/SceneState.cpp:3474 — Sub_505110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2443 | 0x00505170 | `sub_505170` | 有正文 | `src/LegacyUiRecord.cpp:13649 — Sub_505170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2444 | 0x00505BC0 | `sub_505BC0` | 有正文 | `src/LegacyUiRecord.cpp:13888 — Sub_505BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2445 | 0x00505C20 | `sub_505C20` | 有正文 | `src/LegacyUiRecord.cpp:13927 — Sub_505C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2446 | 0x005062A0 | `sub_5062A0` | 有正文 | `src/LegacyUiRecord.cpp:13906 — Sub_5062A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2447 | 0x00506340 | `sub_506340` | 有承接 | `src/platform/LegacyMouse.cpp:464 — MouseRuntimeStartup::MouseRuntimeStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 2448 | 0x00506350 | `unknown_libname_47` | 有正文 | `src/platform/LegacyMouse.cpp:448 — ConstructLegacyMouseRuntime_506350` | 正文已有，MUP01补已完成十成员前缀的C++异常清理；X／Y／八按钮原序及成功后登记保持，完整业务／SEH／二次异常／owner／SDK／运行未验 |
| 2449 | 0x00506530 | `sub_506530` | 有承接 | `include/cg/platform/LegacyMouse.hpp:18 — LegacyMouseState成员自动失败前缀清理／逆序析构` | 原完成前缀清理由原生聚合构造异常规则及成员析构承接；同一存储，原地址保留；原SEH／二次异常／完整owner与运行未验 |
| 2450 | 0x00506580 | `sub_506580` | 有正文 | `src/platform/LegacyMouseTracker.cpp:165 — ResetLegacyMouseTracker_506580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2451 | 0x005066D0 | `sub_5066D0` | 有正文 | `src/platform/LegacyMouseTracker.cpp:236 — ResetLegacyMouseTrackerDefaults_5066D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2452 | 0x005066F0 | `sub_5066F0` | 有正文 | `src/platform/LegacyMouseTracker.cpp:43 — InsertAfter_5066F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2453 | 0x00506720 | `sub_506720` | 有正文 | `src/platform/LegacyMouseTracker.cpp:62 — Detach_506720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2454 | 0x00506760 | `sub_506760` | 有正文 | `src/platform/LegacyMouseTracker.cpp:86 — TakeFreeHead_506760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2455 | 0x00506780 | `sub_506780` | 有正文 | `src/platform/LegacyMouseTracker.cpp:97 — EvictActiveTail_506780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2456 | 0x005067A0 | `sub_5067A0` | 有正文 | `src/platform/LegacyMouseTracker.cpp:108 — AllocateAndActivate_5067A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2457 | 0x005067F0 | `sub_5067F0` | 有正文 | `src/platform/LegacyMouseTracker.cpp:255 — TrackLegacyMouseDown_5067F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2458 | 0x00506870 | `sub_506870` | 有正文 | `src/platform/LegacyMouse.cpp:476 — ResetLegacyMouseState_506870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2459 | 0x00506890 | `MouseEvent::Reset_0` | 有正文 | `src/platform/LegacyMouse.cpp:494 — ResetLegacyMousePosition_506890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2460 | 0x00506900 | `MouseEvent::UpdateLDown` | 有正文 | `src/platform/LegacyMouse.cpp:502 — UpdateLegacyMouseLeftDown_506900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2461 | 0x005069E0 | `MouseEvent::UpdateLUp` | 有正文 | `src/platform/LegacyMouse.cpp:530 — UpdateLegacyMouseLeftUp_5069E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2462 | 0x00506A90 | `MouseEvent::UpdateLDbClick` | 有正文 | `src/platform/LegacyMouse.cpp:551 — UpdateLegacyMouseLeftDoubleClick_506A90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2463 | 0x00506B50 | `MouseEvent::UpdateRDown` | 有正文 | `src/platform/LegacyMouse.cpp:573 — UpdateLegacyMouseRightDown_506B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2464 | 0x00506C40 | `sub_506C40` | 有正文 | `src/platform/LegacyMouse.cpp:601 — UpdateLegacyMouseRightUp_506C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2465 | 0x00506CF0 | `sub_506CF0` | 有正文 | `src/platform/LegacyMouse.cpp:622 — UpdateLegacyMouseRightDoubleClick_506CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2466 | 0x00506DB0 | `sub_506DB0` | 有正文 | `src/platform/LegacyMouse.cpp:644 — AddLegacyMouseWheelDelta_506DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2467 | 0x00506DD0 | `sub_506DD0` | 有正文 | `src/platform/LegacyCursor.cpp:185 — ResetLegacyMouseTail_506DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2468 | 0x00506DE0 | `sub_506DE0` | 有正文 | `src/platform/LegacyCursor.cpp:166 — RequestCursorState_506DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2469 | 0x00506E00 | `World::CursorUpdate` | 有正文 | `src/platform/LegacyCursor.cpp:191 — UpdateCursor_506E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2470 | 0x00507090 | `sub_507090` | 有正文 | `src/render/GraphicPendingQueue.cpp:132 — DrawMatchingGraphicHover_507090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2471 | 0x00507140 | `sub_507140` | 有正文 | `src/render/GraphicHitTest.cpp:208 — Sub_507140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2472 | 0x00507C80 | `sub_507C80` | 有正文 | `src/platform/CommandOptions.cpp:253 — ParseIpOptions_507C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2473 | 0x00507D90 | `GetServerAddressInfo` | 有正文 | `src/LoginScene.cpp:431 — GetServerAddressInfo_507D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2474 | 0x00507DF0 | `NetworkLoop` | 有正文 | `src/network/NetworkRuntime.cpp:4580 — NetworkLoop_507DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2475 | 0x00508140 | `sub_508140` | 有正文 | `src/network/NetworkRuntime.cpp:4555 — InitializeNetworkRuntime_508140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2476 | 0x005081A0 | `sub_5081A0` | 有正文 | `src/network/NetworkRuntime.cpp:4828 — ShutdownNetworkRuntime_5081A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2477 | 0x005081E0 | `sub_5081E0` | 有正文 | `src/network/NetworkRuntime.cpp:4736 — AppendReceiveQueue_5081E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2478 | 0x00508230 | `sub_508230` | 有正文 | `src/network/NetworkRuntime.cpp:4760 — ConsumeReceiveQueue_508230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2479 | 0x00508280 | `sub_508280` | 有正文 | `src/network/NetworkRuntime.cpp:4779 — ConsumeSendQueue_508280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2480 | 0x005082D0 | `sub_5082D0` | 有正文 | `src/network/NetworkRuntime.cpp:727 — Sub_5082D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2481 | 0x00508310 | `sub_508310` | 有正文 | `src/network/NetworkRuntime.cpp:737 — Sub_508310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2482 | 0x00508350 | `sub_508350` | 有正文 | `src/LoginConnection.cpp:9 — ResetServerConnectionState_508350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2483 | 0x00508360 | `sub_508360` | 有正文 | `src/LoginState.cpp:111 — Sub_508360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2484 | 0x00508800 | `sub_508800` | 有正文 | `src/network/NetworkRuntime.cpp:1077 — HandleResponse_508800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2485 | 0x00508910 | `sub_508910` | 有正文 | `src/CharacterListRequest.cpp:148 — StartCharacterListRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2486 | 0x00508950 | `sub_508950` | 有正文 | `src/CharacterListRequest.cpp:163 — PollCharacterListRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2487 | 0x005089B0 | `sub_5089B0` | 有正文 | `src/CharacterListRequest.cpp:188 — HandleCharacterListResponse_5089B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2488 | 0x00508AF0 | `sub_508AF0` | 有正文 | `src/LoginRequest.cpp:97 — StartLoginRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2489 | 0x00508B50 | `sub_508B50` | 有正文 | `src/LoginRequest.cpp:122 — PollLoginRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2490 | 0x00508BC0 | `sub_508BC0` | 有正文 | `src/LoginRequest.cpp:149 — HandleLoginResponse_508BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2491 | 0x00508C40 | `sub_508C40` | 有正文 | `src/LoginRequest.cpp:186 — Sub_508C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2492 | 0x00508C90 | `sub_508C90` | 有正文 | `src/LoginRequest.cpp:200 — Sub_508C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2493 | 0x00508CF0 | `sub_508CF0` | 有正文 | `src/LoginRequest.cpp:223 — Sub_508CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2494 | 0x00508E00 | `sub_508E00` | 有正文 | `src/CharacterCreationRequest.cpp:34 — StartCharacterCreationRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2495 | 0x00508E90 | `sub_508E90` | 有正文 | `src/CharacterCreationRequest.cpp:61 — PollCharacterCreationRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2496 | 0x00508EE0 | `sub_508EE0` | 有正文 | `src/CharacterCreationRequest.cpp:81 — HandleCharacterCreationResponse_508EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2497 | 0x00508F70 | `sub_508F70` | 有正文 | `src/CharacterDeletionRequest.cpp:30 — StartCharacterDeletionRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2498 | 0x00508FC0 | `sub_508FC0` | 有正文 | `src/CharacterDeletionRequest.cpp:51 — PollCharacterDeletionRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2499 | 0x00509020 | `sub_509020` | 有正文 | `src/CharacterDeletionRequest.cpp:73 — HandleCharacterDeletionResponse_509020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2500 | 0x00509080 | `nullsub_1` | 空方法 | `src/Input.cpp:37 — NullSub_509080` | 已有CPP空方法与原RET记录保留；按用户决定标为空方法，不再核对此入口 |
| 2501 | 0x00509090 | `sub_509090` | 有正文 | `src/world/SceneState.cpp:6242 — Sub_509090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2502 | 0x00509100 | `sub_509100` | 有正文 | `src/world/SceneState.cpp:6265 — Sub_509100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2503 | 0x005093C0 | `sub_5093C0` | 有正文 | `src/world/SceneState.cpp:5897 — Sub_5093C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2504 | 0x005093F0 | `Map::FlushNew` | 有正文 | `src/world/SceneState.cpp:6371 — Sub_5093F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2505 | 0x00509550 | `sub_509550` | 有正文 | `src/world/SceneState.cpp:6424 — Sub_509550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2506 | 0x00509650 | `sub_509650` | 有正文 | `src/world/SceneState.cpp:3220 — Sub_509650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2507 | 0x00509BA0 | `sub_509BA0` | 有正文 | `src/world/SceneState.cpp:3312 — Sub_509BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2508 | 0x00509E30 | `sub_509E30` | 有正文 | `src/world/SceneState.cpp:3382 — Sub_509E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2509 | 0x00509EB0 | `sub_509EB0` | 有正文 | `src/world/SceneState.cpp:3488 — Sub_509EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2510 | 0x0050A120 | `sub_50A120` | 有正文 | `src/world/SceneState.cpp:3555 — Sub_50A120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2511 | 0x0050A300 | `sub_50A300` | 有正文 | `src/world/SceneState.cpp:4367 — Sub_50A300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2512 | 0x0050A3E0 | `sub_50A3E0` | 有正文 | `src/world/SceneState.cpp:2487 — Sub_50A3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2513 | 0x0050A6B0 | `sub_50A6B0` | 有正文 | `src/world/SceneState.cpp:4085 — Sub_50A6B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2514 | 0x0050AC20 | `sub_50AC20` | 有正文 | `src/world/SceneState.cpp:4184 — Sub_50AC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2515 | 0x0050AEF0 | `sub_50AEF0` | 有正文 | `src/YobiRuntime.cpp:35929 — Sub_50AEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2516 | 0x0050B0D0 | `sub_50B0D0` | 有正文 | `src/world/SceneState.cpp:4225 — Sub_50B0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2517 | 0x0050B380 | `sub_50B380` | 有正文 | `src/world/SceneState.cpp:2591 — Sub_50B380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2518 | 0x0050BCD0 | `sub_50BCD0` | 有正文 | `src/world/SceneState.cpp:2782 — Sub_50BCD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2519 | 0x0050BF20 | `sub_50BF20` | 有正文 | `src/world/SceneState.cpp:3742 — Sub_50BF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2520 | 0x0050BFA0 | `sub_50BFA0` | 有正文 | `src/world/SceneState.cpp:2903 — Sub_50BFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2521 | 0x0050BFC0 | `sub_50BFC0` | 有正文 | `src/world/SceneState.cpp:2916 — Sub_50BFC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2522 | 0x0050BFD0 | `sub_50BFD0` | 有正文 | `src/world/SceneState.cpp:3792 — Sub_50BFD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2523 | 0x0050C260 | `sub_50C260` | 有正文 | `src/world/SceneState.cpp:3818 — Sub_50C260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2524 | 0x0050C580 | `sub_50C580` | 有正文 | `src/world/SceneState.cpp:3602 — Sub_50C580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2525 | 0x0050C7A0 | `sub_50C7A0` | 有正文 | `src/world/SceneState.cpp:2934 — Sub_50C7A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2526 | 0x0050C7F0 | `sub_50C7F0` | 有正文 | `src/ui/WindowRuntime.cpp:45320 — Sub_50C7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2527 | 0x0050C870 | `sub_50C870` | 有正文 | `src/world/SceneState.cpp:7041 — Sub_50C870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2528 | 0x0050C8D0 | `sub_50C8D0` | 有正文 | `src/world/SceneState.cpp:8502 — Sub_50C8D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2529 | 0x0050C900 | `sub_50C900` | 有正文 | `src/world/SceneState.cpp:3887 — Sub_50C900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2530 | 0x0050C940 | `sub_50C940` | 有正文 | `src/world/SceneState.cpp:3035 — Sub_50C940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2531 | 0x0050CAE0 | `sub_50CAE0` | 有正文 | `src/world/SceneState.cpp:3090 — Sub_50CAE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2532 | 0x0050CE10 | `sub_50CE10` | 有正文 | `src/world/SceneState.cpp:2953 — Sub_50CE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2533 | 0x0050CF20 | `sub_50CF20` | 有正文 | `src/network/NetworkRuntime.cpp:766 — Sub_50CF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2534 | 0x0050CF60 | `sub_50CF60` | 有正文 | `src/network/NetworkRuntime.cpp:757 — Sub_50CF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2535 | 0x0050CF70 | `sub_50CF70` | 有正文 | `src/world/SceneState.cpp:2925 — Sub_50CF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2536 | 0x0050CF90 | `sub_50CF90` | 有正文 | `src/world/SceneState.cpp:6219 — Sub_50CF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2537 | 0x0050D000 | `sub_50D000` | 有正文 | `src/world/SceneState.cpp:3195 — Sub_50D000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2538 | 0x0050D080 | `sub_50D080` | 有正文 | `src/world/SceneState.cpp:5908 — Sub_50D080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2539 | 0x0050D120 | `sub_50D120` | 有正文 | `src/world/SceneState.cpp:5957 — Sub_50D120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2540 | 0x0050D3B0 | `sub_50D3B0` | 有正文 | `src/network/NetworkRuntime.cpp:787 — Sub_50D3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2541 | 0x0050D3C0 | `sub_50D3C0` | 有正文 | `src/world/SceneState.cpp:6042 — Sub_50D3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2542 | 0x0050D500 | `sub_50D500` | 有正文 | `src/world/SceneState.cpp:6121 — Sub_50D500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2543 | 0x0050D700 | `sub_50D700` | 有正文 | `src/network/NetworkRuntime.cpp:825 — Sub_50D700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2544 | 0x0050D740 | `sub_50D740` | 有正文 | `src/world/MailState.cpp:295 — Sub_50D740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2545 | 0x0050D8C0 | `sub_50D8C0` | 有正文 | `src/world/MailState.cpp:334 — Sub_50D8C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2546 | 0x0050D9E0 | `sub_50D9E0` | 有正文 | `src/world/MailState.cpp:362 — Sub_50D9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2547 | 0x0050DC70 | `sub_50DC70` | 有正文 | `src/world/MailState.cpp:638 — Sub_50DC70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2548 | 0x0050DD80 | `sub_50DD80` | 有正文 | `src/network/NetworkRuntime.cpp:845 — Sub_50DD80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2549 | 0x0050E110 | `sub_50E110` | 有正文 | `src/ui/WindowRuntime.cpp:45259 — Sub_50E110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2550 | 0x0050E1A0 | `sub_50E1A0` | 有正文 | `src/world/SceneState.cpp:2976 — Sub_50E1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2551 | 0x0050E210 | `sub_50E210` | 有正文 | `src/world/SceneState.cpp:3010 — Sub_50E210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2552 | 0x0050E310 | `sub_50E310` | 有正文 | `src/network/NetworkRuntime.cpp:797 — Sub_50E310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2553 | 0x0050E340 | `sub_50E340` | 有正文 | `src/ui/WindowRuntime.cpp:45346 — Sub_50E340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2554 | 0x0050E360 | `sub_50E360` | 有正文 | `src/ui/WindowRuntime.cpp:45355 — Sub_50E360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2555 | 0x0050E380 | `sub_50E380` | 有正文 | `src/ui/WindowRuntime.cpp:45364 — Sub_50E380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2556 | 0x0050E3A0 | `sub_50E3A0` | 有正文 | `src/ui/WindowRuntime.cpp:45373 — Sub_50E3A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2557 | 0x0050E3C0 | `sub_50E3C0` | 有正文 | `src/ui/WindowRuntime.cpp:45382 — Sub_50E3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2558 | 0x0050E3F0 | `sub_50E3F0` | 有正文 | `src/ui/WindowRuntime.cpp:45394 — Sub_50E3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2559 | 0x0050E440 | `sub_50E440` | 有正文 | `src/ui/WindowRuntime.cpp:45411 — Sub_50E440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2560 | 0x0050E470 | `sub_50E470` | 有正文 | `src/ui/WindowRuntime.cpp:45426 — Sub_50E470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2561 | 0x0050E4A0 | `sub_50E4A0` | 有正文 | `src/ui/WindowRuntime.cpp:45441 — Sub_50E4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2562 | 0x0050E4D0 | `sub_50E4D0` | 有正文 | `src/ui/WindowRuntime.cpp:45455 — Sub_50E4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2563 | 0x0050E500 | `sub_50E500` | 有正文 | `src/ui/WindowRuntime.cpp:45470 — Sub_50E500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2564 | 0x0050E530 | `sub_50E530` | 有正文 | `src/ui/WindowRuntime.cpp:45485 — Sub_50E530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2565 | 0x0050E570 | `sub_50E570` | 有正文 | `src/ui/WindowRuntime.cpp:45505 — Sub_50E570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2566 | 0x0050E5B0 | `sub_50E5B0` | 有正文 | `src/world/SceneState.cpp:1366 — Sub_50E5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2567 | 0x0050E5D0 | `sub_50E5D0` | 有正文 | `src/world/SceneState.cpp:1373 — Sub_50E5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2568 | 0x0050E630 | `sub_50E630` | 有正文 | `src/network/NetworkRuntime.cpp:813 — Sub_50E630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2569 | 0x0050E650 | `sub_50E650` | 有正文 | `src/world/SceneState.cpp:3001 — Sub_50E650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2570 | 0x0050E680 | `sub_50E680` | 有正文 | `src/LegacyAudio.cpp:1548 — Sub_50E680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2571 | 0x0050E6C0 | `sub_50E6C0` | 有正文 | `src/network/NetworkRuntime.cpp:747 — Sub_50E6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2572 | 0x0050E6E0 | `sub_50E6E0` | 有正文 | `src/network/NetworkRuntime.cpp:943 — Sub_50E6E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2573 | 0x0050E770 | `sub_50E770` | 有正文 | `src/CharacterSelectScene.cpp:203 — Sub_50E770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2574 | 0x0050E780 | `sub_50E780` | 有正文 | `src/network/NetworkRuntime.cpp:965 — Sub_50E780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2575 | 0x0050E7B0 | `sub_50E7B0` | 有正文 | `src/platform/LegacyMouseTracker.cpp:244 — ConfigureLegacyMouseTracker_50E7B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2576 | 0x0050E7D0 | `sub_50E7D0` | 有正文 | `src/ui/WindowRuntime.cpp:45293 — Sub_50E7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2577 | 0x0050E7E0 | `sub_50E7E0` | 有正文 | `src/ui/WindowRuntime.cpp:45299 — Sub_50E7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2578 | 0x0050E7F0 | `sub_50E7F0` | 有正文 | `src/ui/WindowRuntime.cpp:45305 — Sub_50E7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2579 | 0x0050E820 | `sub_50E820` | 有正文 | `src/ui/WindowRuntime.cpp:45314 — Sub_50E820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2580 | 0x0050E830 | `sub_50E830` | 有正文 | `src/network/NetworkRuntime.cpp:979 — Sub_50E830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2581 | 0x0050E850 | `sub_50E850` | 有正文 | `src/LegacyRecordOrder.cpp:147 — Sub_50E850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2582 | 0x0050EB40 | `sub_50EB40` | 有正文 | `src/LegacyRecordOrder.cpp:217 — Sub_50EB40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2583 | 0x0050EC50 | `sub_50EC50` | 有正文 | `src/LegacyRecordOrder.cpp:420 — UpdateOrderedRecordBatch_50EC50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2584 | 0x0050EEB0 | `sub_50EEB0` | 有正文 | `src/LegacyRecordOrder.cpp:389 — UpdateOrderedRecords_50EEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2585 | 0x0050EF20 | `sub_50EF20` | 有正文 | `src/LegacyRecordOrder.cpp:328 — ResetOrderedRecord_50EF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2586 | 0x0050EF50 | `sub_50EF50` | 有正文 | `src/LegacyRecordOrder.cpp:316 — CountRecordsValue08EqualOne_50EF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2587 | 0x0050EF70 | `sub_50EF70` | 有正文 | `src/LegacyRecordOrder.cpp:346 — ParseOrderedRecord_50EF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2588 | 0x0050F130 | `sub_50F130` | 有正文 | `src/network/NetworkRuntime.cpp:991 — Sub_50F130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2589 | 0x0050F190 | `sub_50F190` | 有正文 | `src/network/NetworkRuntime.cpp:1011 — Sub_50F190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2590 | 0x0050F280 | `sub_50F280` | 有正文 | `src/LegacyRecordOrder.cpp:251 — Sub_50F280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2591 | 0x0050F2A0 | `sub_50F2A0` | 有正文 | `src/world/MailState.cpp:1023 — Sub_50F2A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2592 | 0x0050F3A0 | `sub_50F3A0` | 有正文 | `src/world/MailState.cpp:1055 — Sub_50F3A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2593 | 0x0050F430 | `sub_50F430` | 有正文 | `src/world/MailState.cpp:760 — Sub_50F430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2594 | 0x0050F450 | `sub_50F450` | 有正文 | `src/world/MailState.cpp:1221 — Sub_50F450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2595 | 0x0050F7F0 | `sub_50F7F0` | 有正文 | `src/world/MailState.cpp:768 — Sub_50F7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2596 | 0x0050F8E0 | `sub_50F8E0` | 有正文 | `src/world/MailState.cpp:1075 — Sub_50F8E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2597 | 0x0050F990 | `sub_50F990` | 有正文 | `src/ui/WindowRuntime.cpp:45276 — Sub_50F990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2598 | 0x0050F9E0 | `sub_50F9E0` | 有正文 | `src/world/MailState.cpp:798 — Sub_50F9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2599 | 0x0050FC30 | `sub_50FC30` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2600 | 0x0050FE30 | `sub_50FE30` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2601 | 0x0050FF10 | `sub_50FF10` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2602 | 0x0050FF70 | `sub_50FF70` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2603 | 0x00510120 | `sub_510120` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2604 | 0x00510140 | `sub_510140` | 有正文 | `src/LoginRequest.cpp:91 — ReadRequestOutcomeFlag` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2605 | 0x00510150 | `sub_510150` | 有正文 | `src/world/SceneState.cpp:4022 — Sub_510150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2606 | 0x00510460 | `sub_510460` | 有正文 | `src/network/NetworkRuntime.cpp:1040 — Sub_510460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2607 | 0x00510490 | `sub_510490` | 有正文 | `src/network/NetworkRuntime.cpp:1052 — Sub_510490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2608 | 0x00510570 | `sub_510570` | 有正文 | `src/network/NetworkRuntime.cpp:1995 — Sub_510570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2609 | 0x00510630 | `sub_510630` | 有正文 | `src/network/NetworkRuntime.cpp:2207 — Sub_510630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2610 | 0x00510710 | `sub_510710` | 有正文 | `src/network/NetworkRuntime.cpp:2264 — Sub_510710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2611 | 0x00510810 | `sub_510810` | 有正文 | `src/network/NetworkRuntime.cpp:2407 — Sub_510810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2612 | 0x00510880 | `sub_510880` | 有正文 | `src/network/NetworkRuntime.cpp:2577 — Sub_510880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2613 | 0x005108F0 | `sub_5108F0` | 有正文 | `src/network/NetworkRuntime.cpp:4244 — Sub_5108F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2614 | 0x00510940 | `sub_510940` | 有正文 | `src/network/NetworkRuntime.cpp:2607 — Sub_510940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2615 | 0x005109B0 | `SendBattlePacket` | 有正文 | `src/network/NetworkRuntime.cpp:4143 — SendBattlePacket_5109B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2616 | 0x00510A00 | `sub_510A00` | 有正文 | `src/network/NetworkRuntime.cpp:4047 — Sub_510A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2617 | 0x00510AC0 | `sub_510AC0` | 有正文 | `src/network/NetworkRuntime.cpp:2330 — Sub_510AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2618 | 0x00510B80 | `sub_510B80` | 有正文 | `src/network/NetworkRuntime.cpp:2351 — Sub_510B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2619 | 0x00510CE0 | `sub_510CE0` | 有正文 | `src/network/NetworkRuntime.cpp:4007 — Sub_510CE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2620 | 0x00510DA0 | `sub_510DA0` | 有正文 | `src/network/NetworkRuntime.cpp:4159 — Sub_510DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2621 | 0x00510E30 | `sub_510E30` | 有正文 | `src/network/NetworkRuntime.cpp:3842 — Sub_510E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2622 | 0x00510EC0 | `SendItemMove` | 有正文 | `src/network/NetworkRuntime.cpp:4192 — Sub_510EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2623 | 0x00510F50 | `sub_510F50` | 有正文 | `src/network/NetworkRuntime.cpp:4225 — Sub_510F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2624 | 0x00510FA0 | `sub_510FA0` | 有正文 | `src/network/NetworkRuntime.cpp:2718 — Sub_510FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2625 | 0x00511030 | `sub_511030` | 有正文 | `src/network/NetworkRuntime.cpp:2437 — Sub_511030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2626 | 0x005110B0 | `sub_5110B0` | 有正文 | `src/network/NetworkRuntime.cpp:2783 — Sub_5110B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2627 | 0x00511190 | `sub_511190` | 有正文 | `src/network/NetworkRuntime.cpp:2470 — Sub_511190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2628 | 0x005111E0 | `sub_5111E0` | 有正文 | `src/network/NetworkRuntime.cpp:2637 — Sub_5111E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2629 | 0x00511250 | `sub_511250` | 有正文 | `src/network/NetworkRuntime.cpp:2486 — Sub_511250` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2630 | 0x005112C0 | `sub_5112C0` | 有正文 | `src/network/NetworkRuntime.cpp:2508 — Sub_5112C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2631 | 0x00511350 | `sub_511350` | 有正文 | `src/network/NetworkRuntime.cpp:2547 — Sub_511350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2632 | 0x005113C0 | `sub_5113C0` | 有正文 | `src/network/NetworkRuntime.cpp:2667 — Sub_5113C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2633 | 0x00511430 | `sub_511430` | 有正文 | `src/network/NetworkRuntime.cpp:2697 — Sub_511430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2634 | 0x00511480 | `sub_511480` | 有正文 | `src/network/NetworkRuntime.cpp:2744 — Sub_511480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2635 | 0x00511510 | `sub_511510` | 有正文 | `src/network/NetworkRuntime.cpp:2819 — Sub_511510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2636 | 0x005115F0 | `sub_5115F0` | 有正文 | `src/network/NetworkRuntime.cpp:2876 — Sub_5115F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2637 | 0x00511640 | `sub_511640` | 有正文 | `src/network/NetworkRuntime.cpp:2897 — Sub_511640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2638 | 0x00511760 | `sub_511760` | 有正文 | `src/network/NetworkRuntime.cpp:2972 — Sub_511760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2639 | 0x005117D0 | `sub_5117D0` | 有正文 | `src/network/NetworkRuntime.cpp:3002 — Sub_5117D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2640 | 0x00511840 | `sub_511840` | 有正文 | `src/network/NetworkRuntime.cpp:3032 — Sub_511840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2641 | 0x005118B0 | `sub_5118B0` | 有正文 | `src/network/NetworkRuntime.cpp:3062 — Sub_5118B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2642 | 0x00511900 | `sub_511900` | 有正文 | `src/network/NetworkRuntime.cpp:3084 — Sub_511900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2643 | 0x00511A00 | `sub_511A00` | 有正文 | `src/network/NetworkRuntime.cpp:3150 — Sub_511A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2644 | 0x00511A50 | `sub_511A50` | 有正文 | `src/network/NetworkRuntime.cpp:3171 — Sub_511A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2645 | 0x00511AE0 | `sub_511AE0` | 有正文 | `src/network/NetworkRuntime.cpp:3210 — Sub_511AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2646 | 0x00511BC0 | `sub_511BC0` | 有正文 | `src/network/NetworkRuntime.cpp:3267 — Sub_511BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2647 | 0x00511C30 | `sub_511C30` | 有正文 | `src/network/NetworkRuntime.cpp:3297 — Sub_511C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2648 | 0x00511C80 | `sub_511C80` | 有正文 | `src/network/NetworkRuntime.cpp:3318 — Sub_511C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2649 | 0x00511CD0 | `sub_511CD0` | 有正文 | `src/network/NetworkRuntime.cpp:1844 — Sub_511CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2650 | 0x00511D10 | `sub_511D10` | 有正文 | `src/network/NetworkRuntime.cpp:3339 — Sub_511D10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2651 | 0x00511D60 | `sub_511D60` | 有正文 | `src/network/NetworkRuntime.cpp:3360 — Sub_511D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2652 | 0x00511DD0 | `sub_511DD0` | 有正文 | `src/network/NetworkRuntime.cpp:3390 — Sub_511DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2653 | 0x00511E60 | `sub_511E60` | 有正文 | `src/network/NetworkRuntime.cpp:3429 — Sub_511E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2654 | 0x00511EF0 | `sub_511EF0` | 有正文 | `src/network/NetworkRuntime.cpp:3468 — Sub_511EF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2655 | 0x00511FB0 | `sub_511FB0` | 有正文 | `src/network/NetworkRuntime.cpp:1860 — Sub_511FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2656 | 0x00511FF0 | `sub_511FF0` | 有正文 | `src/network/NetworkRuntime.cpp:3516 — Sub_511FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2657 | 0x00512040 | `sub_512040` | 有正文 | `src/network/NetworkRuntime.cpp:3532 — Sub_512040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2658 | 0x005120D0 | `sub_5120D0` | 有正文 | `src/network/NetworkRuntime.cpp:3571 — Sub_5120D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2659 | 0x00512160 | `sub_512160` | 有正文 | `src/network/NetworkRuntime.cpp:3610 — Sub_512160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2660 | 0x00512190 | `sub_512190` | 有正文 | `src/network/NetworkRuntime.cpp:3620 — Sub_512190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2661 | 0x005121E0 | `sub_5121E0` | 有正文 | `src/network/NetworkRuntime.cpp:3641 — Sub_5121E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2662 | 0x00512230 | `sub_512230` | 有正文 | `src/network/NetworkRuntime.cpp:1876 — Sub_512230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2663 | 0x00512270 | `sub_512270` | 有正文 | `src/network/NetworkRuntime.cpp:3677 — Sub_512270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2664 | 0x005122C0 | `sub_5122C0` | 有正文 | `src/network/NetworkRuntime.cpp:3698 — Sub_5122C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2665 | 0x00512310 | `sub_512310` | 有正文 | `src/network/NetworkRuntime.cpp:3719 — Sub_512310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2666 | 0x00512360 | `sub_512360` | 有正文 | `src/network/NetworkRuntime.cpp:3740 — Sub_512360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2667 | 0x005123B0 | `sub_5123B0` | 有正文 | `src/network/NetworkRuntime.cpp:3761 — Sub_5123B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2668 | 0x00512400 | `sub_512400` | 有正文 | `src/network/NetworkRuntime.cpp:3782 — Sub_512400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2669 | 0x00512470 | `sub_512470` | 有正文 | `src/network/NetworkRuntime.cpp:3812 — Sub_512470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2670 | 0x005124E0 | `sub_5124E0` | 有正文 | `src/network/NetworkRuntime.cpp:3870 — Sub_5124E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2671 | 0x00512570 | `sub_512570` | 有正文 | `src/network/NetworkRuntime.cpp:1892 — Sub_512570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2672 | 0x005125B0 | `sub_5125B0` | 有正文 | `src/network/NetworkRuntime.cpp:3909 — Sub_5125B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2673 | 0x00512620 | `sub_512620` | 有正文 | `src/network/NetworkRuntime.cpp:3940 — Sub_512620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2674 | 0x00512720 | `sub_512720` | 有正文 | `src/network/NetworkRuntime.cpp:4260 — Sub_512720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2675 | 0x005127B0 | `sub_5127B0` | 有正文 | `src/network/NetworkRuntime.cpp:3662 — Sub_5127B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2676 | 0x00512800 | `sub_512800` | 有正文 | `src/network/NetworkRuntime.cpp:2101 — Sub_512800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2677 | 0x005128C0 | `sub_5128C0` | 有正文 | `src/network/NetworkRuntime.cpp:2152 — Sub_5128C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2678 | 0x00512AB0 | `sub_512AB0` | 有正文 | `src/network/NetworkRuntime.cpp:2082 — Sub_512AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2679 | 0x00512B00 | `sub_512B00` | 有正文 | `src/network/NetworkRuntime.cpp:2043 — Sub_512B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2680 | 0x00512B90 | `sub_512B90` | 有正文 | `src/network/NetworkRuntime.cpp:1796 — Sub_512B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2681 | 0x00512BD0 | `sub_512BD0` | 有正文 | `src/network/NetworkRuntime.cpp:1812 — Sub_512BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2682 | 0x00512C10 | `sub_512C10` | 有正文 | `src/network/NetworkRuntime.cpp:1777 — Sub_512C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2683 | 0x00512C60 | `sub_512C60` | 有正文 | `src/network/NetworkRuntime.cpp:1828 — Sub_512C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2684 | 0x00512CA0 | `sub_512CA0` | 有正文 | `src/network/NetworkRuntime.cpp:1759 — Report_512CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2685 | 0x00512CF0 | `sub_512CF0` | 有正文 | `src/network/NetworkRuntime.cpp:1908 — Sub_512CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2686 | 0x00512D30 | `sub_512D30` | 有正文 | `src/network/NetworkRuntime.cpp:1944 — Sub_512D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2687 | 0x00512DA0 | `sub_512DA0` | 有正文 | `src/network/NetworkRuntime.cpp:1974 — Sub_512DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2688 | 0x00512DF0 | `sub_512DF0` | 有正文 | `src/network/NetworkRuntime.cpp:1924 — Sub_512DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2689 | 0x00512E40 | `network_dispatch` | 有正文 | `src/network/NetworkRuntime.cpp:4388 — NetworkDispatch_512E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2690 | 0x00516B60 | `sub_516B60` | 有正文 | `src/network/NetworkRuntime.cpp:4521 — AllocateNetworkWorkspace_516B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2691 | 0x00516C90 | `sub_516C90` | 有正文 | `src/network/NetworkRuntime.cpp:4087 — Sub_516C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2692 | 0x00516D70 | `sub_516D70` | 有正文 | `src/network/NetworkRuntime.cpp:4134 — SendIuEPacket_516D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2693 | 0x00516DA0 | `sub_516DA0` | 有正文 | `src/network/NetworkRuntime.cpp:1174 — AllocateProtocolBuffers_516DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2694 | 0x00517040 | `sub_517040` | 有正文 | `src/network/NetworkRuntime.cpp:1327 — Sub_517040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2695 | 0x00517080 | `sub_517080` | 有正文 | `src/network/NetworkRuntime.cpp:1308 — CopyProtocolBytes_517080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2696 | 0x005170D0 | `sub_5170D0` | 有正文 | `src/network/NetworkRuntime.cpp:1361 — AppendProtocolText_5170D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2697 | 0x00517120 | `sub_517120` | 有正文 | `src/network/NetworkRuntime.cpp:1399 — FormatProtocolIntegerToken_517120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2698 | 0x00517160 | `sub_517160` | 有正文 | `src/network/NetworkRuntime.cpp:1423 — FormatProtocolTextToken_517160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2699 | 0x00517190 | `sub_517190` | 有正文 | `src/network/NetworkRuntime.cpp:1436 — ParseProtocolInteger_517190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2700 | 0x005171B0 | `sub_5171B0` | 有正文 | `src/network/NetworkRuntime.cpp:1707 — Sub_5171B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2701 | 0x005171F0 | `sub_5171F0` | 有正文 | `src/network/NetworkRuntime.cpp:1614 — EscapeProtocolText_5171F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2702 | 0x005172D0 | `sub_5172D0` | 有正文 | `src/network/NetworkRuntime.cpp:1637 — UnescapeProtocolText_5172D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2703 | 0x005173B0 | `receivedBufferSplit` | 有正文 | `src/network/NetworkRuntime.cpp:573 — SplitReceivedBuffer_5173B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2704 | 0x00517480 | `sub_517480` | 有正文 | `src/network/NetworkRuntime.cpp:1723 — SendProtocolBuffer_517480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2705 | 0x00517510 | `sub_517510` | 有正文 | `src/network/NetworkRuntime.cpp:1125 — BuildProtocolCommand_517510` | 当前本机服务端明文头采用原命令／空格；旧命令变换／密文状态尾按获批明文合同退休，完整发送未验 |
| 2706 | 0x00517670 | `sub_517670` | 有正文 | `src/network/NetworkRuntime.cpp:1343 — FormatProtocolInteger_517670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2707 | 0x00517690 | `sub_517690` | 有正文 | `src/network/NetworkRuntime.cpp:1351 — CopyProtocolBytesAndReturn_517690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2708 | 0x005176B0 | `sub_5176B0` | 有正文 | `src/network/NetworkRuntime.cpp:616 — Sub_5176B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2709 | 0x00517730 | `sub_517730` | 有正文 | `src/network/NetworkRuntime.cpp:660 — Sub_517730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2710 | 0x00517880 | `Yobi::FrameMove` | 有正文 | `src/YobiRuntime.cpp:34651 — Sub_517880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2711 | 0x00517DA0 | `Yobi::PlayFrame` | 有正文 | `src/YobiRuntime.cpp:34767 — PlayFrame_517DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2712 | 0x005182B0 | `sub_5182B0` | 有正文 | `src/YobiRuntime.cpp:5676 — Sub_5182B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2713 | 0x00518300 | `sub_518300` | 有正文 | `src/YobiRuntime.cpp:5697 — Sub_518300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2714 | 0x005183B0 | `sub_5183B0` | 有正文 | `src/YobiRuntime.cpp:36594 — Sub_5183B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2715 | 0x005183D0 | `sub_5183D0` | 有承接 | `src/world/SceneState.cpp:621 — PlayerRuntimeValuesStartup::PlayerRuntimeValuesStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 2716 | 0x005183E0 | `sub_5183E0` | 有正文 | `src/world/SceneState.cpp:606 — ConstructLegacyPlayerRuntime_5183E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2717 | 0x00518520 | `sub_518520` | 有承接 | `src/world/SceneState.cpp:633 — PetRuntimeValuesStartup::PetRuntimeValuesStartup` | LCI01已批准以原生拥有者构造／析构承接；同一真实载荷、原构造清理及中央位置保持，退出机制沿MSVC；仅静态核对，编译／运行未验 |
| 2718 | 0x00518530 | `sub_518530` | 有正文 | `src/world/SceneState.cpp:663 — ConstructPetRuntimeValues` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2719 | 0x00518560 | `sub_518560` | 有正文 | `src/world/SceneState.cpp:670 — DestroyPetRuntimeValues` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2720 | 0x00518580 | `sub_518580` | 有正文 | `src/world/SceneState.cpp:4344 — Sub_518580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2721 | 0x005185C0 | `sub_5185C0` | 有正文 | `src/world/SceneState.cpp:4284 — Sub_5185C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2722 | 0x00518600 | `sub_518600` | 有正文 | `src/world/SceneState.cpp:3670 — Sub_518600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2723 | 0x00518660 | `sub_518660` | 有正文 | `src/world/SceneState.cpp:3655 — Sub_518660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2724 | 0x005186A0 | `sub_5186A0` | 有正文 | `src/world/SceneState.cpp:1183 — Sub_5186A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2725 | 0x00518780 | `sub_518780` | 有正文 | `src/world/SceneState.cpp:1215 — Sub_518780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2726 | 0x00518870 | `sub_518870` | 有正文 | `src/world/SceneState.cpp:1265 — Sub_518870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2727 | 0x00518A40 | `sub_518A40` | 有正文 | `src/world/SceneState.cpp:1312 — Sub_518A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2728 | 0x00518BE0 | `sub_518BE0` | 有正文 | `src/world/SceneState.cpp:1253 — Sub_518BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2729 | 0x00518C20 | `sub_518C20` | 有正文 | `src/world/SceneState.cpp:1147 — InitializeAlbum_518C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2730 | 0x00518C60 | `sub_518C60` | 有正文 | `src/world/SceneState.cpp:8565 — Sub_518C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2731 | 0x00518C80 | `sub_518C80` | 有正文 | `src/world/MailState.cpp:852 — Sub_518C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2732 | 0x00518CB0 | `sub_518CB0` | 有正文 | `src/world/MailState.cpp:1380 — Sub_518CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2733 | 0x00518DA0 | `sub_518DA0` | 有正文 | `src/world/MailState.cpp:434 — Sub_518DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2734 | 0x00518F00 | `sub_518F00` | 有正文 | `src/world/MailState.cpp:501 — Sub_518F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2735 | 0x00519090 | `sub_519090` | 有正文 | `src/world/MailState.cpp:571 — Sub_519090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2736 | 0x005190F0 | `sub_5190F0` | 有正文 | `src/world/MailState.cpp:592 — Sub_5190F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2737 | 0x00519170 | `sub_519170` | 有正文 | `src/world/MailState.cpp:619 — Sub_519170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2738 | 0x005191C0 | `sub_5191C0` | 有正文 | `src/world/MailState.cpp:411 — Sub_5191C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2739 | 0x00519290 | `sub_519290` | 有正文 | `src/world/MailState.cpp:1441 — Sub_519290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2740 | 0x005192C0 | `sub_5192C0` | 有正文 | `src/world/MailState.cpp:1459 — Sub_5192C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2741 | 0x00519300 | `sub_519300` | 有正文 | `src/world/MailState.cpp:1407 — Sub_519300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2742 | 0x00519370 | `sub_519370` | 有正文 | `src/world/MailState.cpp:1513 — Sub_519370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2743 | 0x005194C0 | `sub_5194C0` | 有正文 | `src/world/MailState.cpp:1449 — Sub_5194C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2744 | 0x005194F0 | `sub_5194F0` | 有正文 | `src/world/MailState.cpp:1494 — Sub_5194F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2745 | 0x00519550 | `sub_519550` | 有正文 | `src/world/MailState.cpp:1424 — Sub_519550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2746 | 0x005195C0 | `sub_5195C0` | 有正文 | `src/LegacyRecordOrder.cpp:481 — FindActiveOrderedRecordIndex_5195C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2747 | 0x005195F0 | `sub_5195F0` | 有正文 | `src/world/MailState.cpp:1471 — Sub_5195F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2748 | 0x00519640 | `sub_519640` | 有正文 | `src/LegacyUiRecord.cpp:14071 — Sub_519640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2749 | 0x00519680 | `sub_519680` | 有正文 | `src/ui/WindowRuntime.cpp:1375 — Sub_519680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2750 | 0x005196B0 | `sub_5196B0` | 有正文 | `src/world/SceneState.cpp:3765 — Sub_5196B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2751 | 0x00519770 | `sub_519770` | 有正文 | `src/world/SceneState.cpp:4002 — Sub_519770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2752 | 0x00519840 | `sub_519840` | 有正文 | `src/world/SceneState.cpp:4396 — Sub_519840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2753 | 0x0051985F | `?underflow@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/world/SceneState.cpp:4396 — Sub_519840` | 原共享 -1 返回尾；当前父实际失败返回承接；完整业务／运行未验 |
| 2754 | 0x00519870 | `sub_519870` | 有正文 | `src/world/SceneState.cpp:4410 — Sub_519870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2755 | 0x005198A0 | `sub_5198A0` | 有正文 | `src/world/SceneState.cpp:4422 — Sub_5198A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2756 | 0x005198D0 | `sub_5198D0` | 有正文 | `src/world/SceneState.cpp:4440 — Sub_5198D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2757 | 0x005198F0 | `sub_5198F0` | 有正文 | `src/world/SceneState.cpp:4450 — Sub_5198F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2758 | 0x00519920 | `sub_519920` | 有正文 | `src/world/SceneState.cpp:4330 — Sub_519920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2759 | 0x00519950 | `sub_519950` | 有正文 | `src/world/SceneState.cpp:3642 — Sub_519950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2760 | 0x005199A0 | `sub_5199A0` | 有正文 | `src/world/SceneState.cpp:10338 — Sub_5199A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2761 | 0x00519A80 | `sub_519A80` | 有正文 | `src/world/SceneState.cpp:9795 — Sub_519A80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2762 | 0x00519CD0 | `sub_519CD0` | 有正文 | `src/world/SceneState.cpp:9909 — Sub_519CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2763 | 0x00519D50 | `sub_519D50` | 有正文 | `src/world/SceneState.cpp:10155 — Sub_519D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2764 | 0x00519E50 | `sub_519E50` | 有正文 | `src/world/SceneState.cpp:9934 — Sub_519E50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2765 | 0x00519EB0 | `sub_519EB0` | 有正文 | `src/world/SceneState.cpp:7034 — Sub_519EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2766 | 0x00519EC0 | `sub_519EC0` | 有正文 | `src/world/SceneState.cpp:9953 — Sub_519EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2767 | 0x00519EE0 | `sub_519EE0` | 有正文 | `src/YobiRuntime.cpp:37357 — Sub_519EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2768 | 0x0051A040 | `sub_51A040` | 有正文 | `src/world/SceneState.cpp:9959 — Sub_51A040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2769 | 0x0051A060 | `sub_51A060` | 有正文 | `src/world/SceneState.cpp:9970 — Sub_51A060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2770 | 0x0051A090 | `sub_51A090` | 有正文 | `src/world/SceneState.cpp:9848 — Sub_51A090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2771 | 0x0051A4B0 | `sub_51A4B0` | 有正文 | `src/world/SceneState.cpp:9901 — Sub_51A4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2772 | 0x0051A5A0 | `sub_51A5A0` | 有正文 | `src/world/SceneState.cpp:10171 — Sub_51A5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2773 | 0x0051A5B0 | `sub_51A5B0` | 有正文 | `src/world/SceneState.cpp:10266 — Sub_51A5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2774 | 0x0051A5E0 | `sub_51A5E0` | 有正文 | `src/world/SceneState.cpp:10179 — Sub_51A5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2775 | 0x0051A5F0 | `sub_51A5F0` | 有正文 | `src/world/SceneState.cpp:10186 — Sub_51A5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2776 | 0x0051A600 | `sub_51A600` | 有正文 | `src/world/SceneState.cpp:10192 — Sub_51A600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2777 | 0x0051A610 | `sub_51A610` | 有正文 | `src/world/SceneState.cpp:10198 — Sub_51A610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2778 | 0x0051A620 | `sub_51A620` | 有正文 | `src/world/SceneState.cpp:10204 — Sub_51A620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2779 | 0x0051A630 | `sub_51A630` | 有正文 | `src/world/SceneState.cpp:10210 — Sub_51A630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2780 | 0x0051A640 | `sub_51A640` | 有正文 | `src/YobiRuntime.cpp:37330 — Sub_51A640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2781 | 0x0051A660 | `sub_51A660` | 有正文 | `src/YobiRuntime.cpp:37340 — Sub_51A660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2782 | 0x0051A680 | `sub_51A680` | 有正文 | `src/YobiRuntime.cpp:37350 — Sub_51A680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2783 | 0x0051A6A0 | `sub_51A6A0` | 有正文 | `src/YobiRuntime.cpp:36021 — Sub_51A6A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2784 | 0x0051A840 | `sub_51A840` | 有正文 | `src/world/SceneState.cpp:9980 — Sub_51A840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2785 | 0x0051B080 | `sub_51B080` | 有正文 | `src/world/SceneState.cpp:8941 — Sub_51B080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2786 | 0x0051B0F0 | `sub_51B0F0` | 有正文 | `src/world/SceneState.cpp:8933 — Sub_51B0F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2787 | 0x0051B110 | `sub_51B110` | 有正文 | `src/LoginRequest.cpp:60 — Sub_51B110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2788 | 0x0051B140 | `sub_51B140` | 有正文 | `src/LoginRequest.cpp:69 — CharacterNameExists_51B140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2789 | 0x0051B1B0 | `sub_51B1B0` | 有正文 | `src/CharacterListRequest.cpp:54 — WriteCharacterRecord_51B1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2790 | 0x0051B3B0 | `sub_51B3B0` | 有正文 | `src/LoginRequest.cpp:50 — Sub_51B3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2791 | 0x0051B3E0 | `sub_51B3E0` | 有正文 | `src/world/SceneState.cpp:9383 — Sub_51B3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2792 | 0x0051B530 | `sub_51B530` | 有正文 | `src/world/SceneState.cpp:3850 — Sub_51B530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2793 | 0x0051B5F0 | `sub_51B5F0` | 有正文 | `src/world/SceneState.cpp:3871 — Sub_51B5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2794 | 0x0051B630 | `sub_51B630` | 有正文 | `src/world/SceneState.cpp:3901 — Sub_51B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2795 | 0x0051B740 | `sub_51B740` | 有正文 | `src/CharacterSelectPreview.cpp:293 — Sub_51B740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2796 | 0x0051B7D0 | `sub_51B7D0` | 有正文 | `src/CharacterSelectPreview.cpp:329 — Sub_51B7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2797 | 0x0051B910 | `sub_51B910` | 有正文 | `src/CharacterSelectPreview.cpp:383 — Sub_51B910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2798 | 0x0051BA10 | `sub_51BA10` | 有正文 | `src/CharacterSelectPreview.cpp:102 — Sub_51BA10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2799 | 0x0051BAC0 | `sub_51BAC0` | 有正文 | `src/CharacterSelectPreview.cpp:157 — Sub_51BAC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2800 | 0x0051BB80 | `sub_51BB80` | 有正文 | `src/CharacterSelectPreview.cpp:130 — Sub_51BB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2801 | 0x0051BC50 | `sub_51BC50` | 有正文 | `src/CharacterSelectPreview.cpp:261 — Sub_51BC50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2802 | 0x0051BD00 | `sub_51BD00` | 有正文 | `src/CharacterSelectPreview.cpp:185 — Sub_51BD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2803 | 0x0051BDB0 | `sub_51BDB0` | 有正文 | `src/world/SceneState.cpp:10216 — Sub_51BDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2804 | 0x0051BDC0 | `sub_51BDC0` | 有正文 | `src/world/SceneState.cpp:10222 — Sub_51BDC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2805 | 0x0051BDD0 | `sub_51BDD0` | 有正文 | `src/world/SceneState.cpp:10277 — Sub_51BDD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2806 | 0x0051BDF0 | `sub_51BDF0` | 有正文 | `src/world/SceneState.cpp:10285 — Sub_51BDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2807 | 0x0051BE10 | `sub_51BE10` | 有正文 | `src/world/SceneState.cpp:10232 — Sub_51BE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2808 | 0x0051BE20 | `sub_51BE20` | 有正文 | `src/world/SceneState.cpp:10238 — Sub_51BE20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2809 | 0x0051BE30 | `sub_51BE30` | 有正文 | `src/world/SceneState.cpp:10244 — Sub_51BE30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2810 | 0x0051BE40 | `sub_51BE40` | 有正文 | `src/world/SceneState.cpp:10254 — Sub_51BE40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2811 | 0x0051BE50 | `sub_51BE50` | 有正文 | `src/world/SceneState.cpp:10305 — Sub_51BE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2812 | 0x0051BEC0 | `sub_51BEC0` | 有正文 | `src/world/SceneState.cpp:10296 — Sub_51BEC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2813 | 0x0051BEE0 | `sub_51BEE0` | 有正文 | `src/world/SceneState.cpp:10260 — Sub_51BEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2814 | 0x0051BEF0 | `sub_51BEF0` | 有正文 | `src/world/SceneState.cpp:581 — Sub_51BEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2815 | 0x0051BF00 | `sub_51BF00` | 有正文 | `src/CharacterSelectPreview.cpp:218 — Sub_51BF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2816 | 0x0051BFF0 | `sub_51BFF0` | 有正文 | `src/CharacterSelectPreview.cpp:240 — Sub_51BFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2817 | 0x0051C0D0 | `mappingCharaImageNo` | 有正文 | `src/world/SceneState.cpp:10109 — Sub_51C0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2818 | 0x0051C410 | `sub_51C410` | 有正文 | `src/world/SceneState.cpp:657 — IsValidPetIndex_51C410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2819 | 0x0051C430 | `World::Update` | 有正文 | `src/world/WorldUpdate.cpp:28 — Update` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2820 | 0x0051C770 | `sub_51C770` | 有正文 | `src/world/SceneState.cpp:1080 — SetCurrentSceneType` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2821 | 0x0051C790 | `sub_51C790` | 有正文 | `src/world/SceneState.cpp:1088 — SetCurrentSceneTypeAndState` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2822 | 0x0051C7B0 | `sub_51C7B0` | 有正文 | `src/world/SceneState.cpp:1096 — SetPendingSceneType` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2823 | 0x0051C7D0 | `World::DrawNormalScene` | 有正文 | `src/world/SceneState.cpp:6460 — DrawNormalScene_51C7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2824 | 0x0051D5B0 | `sub_51D5B0` | 有正文 | `src/world/SceneState.cpp:7471 — Sub_51D5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2825 | 0x0051D640 | `sub_51D640` | 有正文 | `src/world/SceneState.cpp:6940 — Sub_51D640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2826 | 0x0051D6F0 | `sub_51D6F0` | 有正文 | `src/world/SceneState.cpp:6978 — Sub_51D6F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2827 | 0x0051D750 | `sub_51D750` | 有正文 | `src/world/SceneState.cpp:6965 — Sub_51D750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2828 | 0x0051D790 | `sub_51D790` | 有正文 | `src/world/SceneState.cpp:10557 — Sub_51D790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2829 | 0x0051E0D0 | `sub_51E0D0` | 有正文 | `src/world/SceneState.cpp:10849 — Sub_51E0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2830 | 0x0051EC10 | `sub_51EC10` | 有正文 | `src/world/SceneState.cpp:11291 — Sub_51EC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2831 | 0x0051EF80 | `sub_51EF80` | 有正文 | `src/world/SceneState.cpp:11448 — Sub_51EF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2832 | 0x0051F610 | `sub_51F610` | 有正文 | `src/world/SceneState.cpp:11624 — Sub_51F610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2833 | 0x0051F860 | `sub_51F860` | 有正文 | `src/world/SceneState.cpp:11771 — Sub_51F860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2834 | 0x0051FA70 | `sub_51FA70` | 有正文 | `src/world/SceneState.cpp:11777 — Sub_51FA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2835 | 0x0051FC90 | `sub_51FC90` | 有正文 | `src/world/SceneState.cpp:11783 — Sub_51FC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2836 | 0x0051FDE0 | `sub_51FDE0` | 有正文 | `src/world/WorldUpdate.cpp:360 — Sub_51FDE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2837 | 0x00520160 | `sub_520160` | 有正文 | `src/LoginSavedState.cpp:997 — Sub_520160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2838 | 0x005201B0 | `sub_5201B0` | 有正文 | `src/LoginSavedState.cpp:791 — RestoreCurrentSavedState_5201B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2839 | 0x00520200 | `sub_520200` | 有正文 | `src/LoginSavedState.cpp:1024 — Sub_520200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2840 | 0x005202E0 | `sub_5202E0` | 有正文 | `src/platform/LegacyModuleDigest.cpp:252 — ComputeExecutableDigest_5202E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2841 | 0x005203B0 | `sub_5203B0` | 有正文 | `src/LoginInput.cpp:163 — WriteInputValueFF494F_5203B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2842 | 0x005203E0 | `sub_5203E0` | 有正文 | `src/LoginInput.cpp:171 — ReadInputValueFF494F_5203E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2843 | 0x00520410 | `sub_520410` | 有正文 | `src/LoginInput.cpp:182 — WriteInputValueFF4982_520410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2844 | 0x00520440 | `sub_520440` | 有正文 | `src/LoginInput.cpp:190 — ReadInputValueFF4982_520440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2845 | 0x00520470 | `sub_520470` | 有正文 | `src/LoginSavedState.cpp:895 — Sub_520470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2846 | 0x005204C0 | `sub_5204C0` | 有正文 | `src/LoginSavedState.cpp:707 — RestoreAudioSettings_5204C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2847 | 0x00520510 | `sub_520510` | 有正文 | `src/LoginSavedState.cpp:836 — Sub_520510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2848 | 0x00520550 | `sub_520550` | 有正文 | `src/LoginSavedState.cpp:857 — Sub_520550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2849 | 0x00520590 | `sub_520590` | 有正文 | `src/LoginSavedState.cpp:927 — Sub_520590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2850 | 0x005205E0 | `sub_5205E0` | 有正文 | `src/LoginSavedState.cpp:726 — RestoreSavedScalarSettings_5205E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2851 | 0x00520640 | `sub_520640` | 有正文 | `src/LoginSavedState.cpp:953 — Sub_520640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2852 | 0x00520690 | `sub_520690` | 有正文 | `src/LoginSavedState.cpp:745 — RestoreSavedInputContexts_520690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2853 | 0x005206F0 | `sub_5206F0` | 有正文 | `src/LoginSavedState.cpp:975 — Sub_5206F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2854 | 0x00520700 | `sub_520700` | 有正文 | `src/LoginSavedState.cpp:765 — RestoreSavedByteState_520700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2855 | 0x00520720 | `sub_520720` | 有正文 | `src/LoginSavedState.cpp:986 — Sub_520720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2856 | 0x00520730 | `sub_520730` | 有正文 | `src/LoginSavedState.cpp:775 — RestoreCurrentSavedValue_520730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2857 | 0x00520750 | `sub_520750` | 有正文 | `src/LoginSavedState.cpp:873 — Sub_520750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2858 | 0x00520770 | `sub_520770` | 有正文 | `src/LoginSavedState.cpp:884 — Sub_520770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2859 | 0x00520790 | `Renderer::SortTexture` | 有正文 | `src/render/GraphicPendingQueue.cpp:395 — SortTextures_520790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2860 | 0x005208F0 | `sub_5208F0` | 有正文 | `src/render/GraphicPendingQueue.cpp:198 — BackupValues_5208F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2861 | 0x00520910 | `sub_520910` | 有正文 | `src/render/GraphicPendingQueue.cpp:208 — Sub_520910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2862 | 0x00520930 | `sub_520930` | 有正文 | `src/render/GraphicPendingQueue.cpp:214 — Sub_520930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2863 | 0x00520970 | `Renderer::DrawGraphic3` | 有正文 | `src/DrawGraphic.cpp:50 — DrawGraphic3_520970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2864 | 0x00520AD0 | `Renderer::FillDirtyArea` | 有正文 | `src/render/GraphicPendingQueue.cpp:286 — FillDirtyArea_520AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2865 | 0x00520B10 | `Graphic::LoadTexture` | 有正文 | `src/render/GraphicDxCache.cpp:528 — LoadGraphicTexture_520B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2866 | 0x00520DD0 | `Renderer::DrawAllQueued` | 有正文 | `src/render/DrawAllQueued.cpp:346 — DrawAllQueued_520DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2867 | 0x00521870 | `sub_521870` | 有正文 | `src/render/GraphicPendingQueue.cpp:315 — Sub_521870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2868 | 0x005219C0 | `sub_5219C0` | 有正文 | `src/render/GraphicPendingQueue.cpp:49 — CalculateRippleScreenDelta` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2869 | 0x00521A30 | `Renderer::QueueText` | 有正文 | `src/render/TextPendingQueue.cpp:50 — QueueText_521A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2870 | 0x00521AA0 | `sub_521AA0` | 有正文 | `src/render/GraphicPendingQueue.cpp:229 — QueueMap_521AA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2871 | 0x00521B10 | `Renderer::DrawGraphic` | 有正文 | `src/DrawGraphic.cpp:275 — DrawGraphic` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2872 | 0x00521C80 | `sub_521C80` | 有正文 | `src/DrawGraphic.cpp:166 — Sub_521C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2873 | 0x00521DD0 | `Renderer::DrawGraphic2` | 有正文 | `src/DrawGraphic.cpp:218 — DrawGraphic2_521DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2874 | 0x00521F20 | `World::RenderChara` | 有正文 | `src/YobiRuntime.cpp:37392 — RenderChara_521F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2875 | 0x00522180 | `Renderer::DrawGraphic_Hover` | 有正文 | `src/DrawGraphicHover.cpp:9 — DrawGraphicHover` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2876 | 0x00522210 | `sub_522210` | 有正文 | `src/render/GraphicPendingQueue.cpp:259 — Sub_522210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2877 | 0x005222A0 | `sub_5222A0` | 有正文 | `src/render/GraphicPendingQueue.cpp:30 — CompareGraphicCache3_5222A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2878 | 0x005222D0 | `Renderer::TakeGraphicDxCache` | 有正文 | `src/render/GraphicDxCache.cpp:433 — TakeGraphicDxCache_5222D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2879 | 0x00522570 | `Graphic::LoadGraphic` | 有正文 | `src/render/GraphicDxCache.cpp:362 — LoadGraphic_522570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2880 | 0x00522620 | `sub_522620` | 有正文 | `src/GraphicRead.cpp:371 — InitializeIndependentPaletteQueue_522620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2881 | 0x00522630 | `sub_522630` | 有正文 | `src/GraphicRead.cpp:198 — ConstructIndependentPaletteQueue_522630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2882 | 0x00522660 | `sub_522660` | 有正文 | `src/GraphicRead.cpp:205 — DestroyIndependentPaletteQueue_522660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2883 | 0x00522690 | `sub_522690` | 有正文 | `src/GraphicRead.cpp:378 — InitializePaletteDbfState_522690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2884 | 0x005226A0 | `unknown_libname_48` | 有正文 | `src/GraphicRead.cpp:211 — ConstructPaletteDbfState_5226A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2885 | 0x00522900 | `sub_522900` | 有正文 | `src/GraphicRead.cpp:228 — LoadIndependentPaletteIndex_522900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2886 | 0x00522B30 | `sub_522B30` | 有正文 | `src/GraphicRead.cpp:484 — InitializeIndependentPalettes_522B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2887 | 0x00522C50 | `sub_522C50` | 有正文 | `src/GraphicRead.cpp:528 — ReleaseIndependentPalettes_522C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2888 | 0x00522CB0 | `sub_522CB0` | 有正文 | `src/GraphicRead.cpp:294 — LoadIndependentPalette32_522CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2889 | 0x00523140 | `Graphic::LoadPatelet` | 有正文 | `src/GraphicRead.cpp:540 — RefreshIndependentPalette32_523140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2890 | 0x005231A0 | `Renderer::BuildSurfaceCache` | 有正文 | `src/render/GraphicDxCache.cpp:678 — BuildSurfaceCache_5231A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2891 | 0x00523310 | `sub_523310` | 有正文 | `src/render/GraphicDxCache.cpp:318 — ResetTextureNodes_523310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2892 | 0x00523350 | `sub_523350` | 有正文 | `src/render/GraphicDxCache.cpp:334 — InvalidateGraphicRefs_523350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2893 | 0x00523390 | `sub_523390` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2894 | 0x00523450 | `sub_523450` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2895 | 0x005234B0 | `nullsub_2` | 空方法 | `src/GraphicRead.cpp:173 — decodedBlocks.clear();` | 原RET 8非释放叶，指针载荷不归该叶释放，实际vector宿主构析定位保留；按用户决定标为空方法，不再核对此入口 |
| 2896 | 0x005234C0 | `sub_5234C0` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2897 | 0x005235B0 | `sub_5235B0` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | 原迭代器字段操作；当前三个实际 std::map 的迭代器由 STL 承接；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2898 | 0x005235C0 | `sub_5235C0` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2899 | 0x00523690 | `sub_523690` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2900 | 0x005236B0 | `Vector::void::insert` | 有承接 | `src/GraphicRead.cpp:262 — decodedBlocks.push_back(decoded);` | 原 vector<void*> 插入；当前实际 vector push_back，另有图号／请求集合消费；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2901 | 0x005238C0 | `sub_5238C0` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2902 | 0x00523C90 | `sub_523C90` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2903 | 0x00523CD0 | `sub_523CD0` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2904 | 0x00523D20 | `sub_523D20` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2905 | 0x00523F90 | `sub_523F90` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2906 | 0x00523FB0 | `sub_523FB0` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2907 | 0x00523FD0 | `sub_523FD0` | 有承接 | `src/GraphicRead.cpp:154 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2908 | 0x00524030 | `sub_524030` | 有承接 | `src/GraphicRead.cpp:160 — std::vector<std::uint8_t*> decodedBlocks;` | 原指针区间拷贝叶；当前实际 vector 维护元素序列；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2909 | 0x00524060 | `sub_524060` | 有正文 | `src/LegacyAudio.cpp:630 — LoadBgmConfig_524060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2910 | 0x005241A0 | `sub_5241A0` | 有正文 | `src/LegacyAudio.cpp:602 — InitializeAudio_5241A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2911 | 0x00524230 | `sub_524230` | 有正文 | `src/LegacyAudio.cpp:502 — Sub_524230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2912 | 0x0052B630 | `sub_52B630` | 有正文 | `src/LegacyAudio.cpp:481 — Sub_52B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2913 | 0x0052B710 | `sub_52B710` | 有正文 | `src/LegacyAudio.cpp:187 — ReadPcmWaveHeader_52B710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2914 | 0x0052B810 | `sub_52B810` | 有承接 | `src/platform/SdlAudioBackend.cpp:113 — LoadSample` | 原音频缓冲写入由实际 WAV 装载／PrepareChannel 承接；完整业务／运行未验 |
| 2915 | 0x0052B880 | `sub_52B880` | 有承接 | `src/platform/SdlAudioBackend.cpp:227 — PrepareChannel` | 原音频缓冲准备由实际 SDL 流与 WAV 装载承接；完整业务／运行未验 |
| 2916 | 0x0052B8E0 | `sub_52B8E0` | 有正文 | `src/LegacyAudio.cpp:450 — Sub_52B8E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2917 | 0x0052B940 | `sub_52B940` | 有正文 | `src/LegacyAudio.cpp:468 — Sub_52B940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2918 | 0x0052B990 | `sub_52B990` | 有正文 | `src/LegacyAudio.cpp:1358 — Sub_52B990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2919 | 0x0052B9E0 | `sub_52B9E0` | 有正文 | `src/LegacyAudio.cpp:973 — Sub_52B9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2920 | 0x0052BBE0 | `sub_52BBE0` | 有承接 | `src/platform/SdlAudioBackend.cpp:151 — FeedChannel` | 原流喂入由实际 SDL 音频回调承接；完整业务／运行未验 |
| 2921 | 0x0052BD40 | `SoundThreadMain` | 有承接 | `src/platform/SdlAudioBackend.cpp:61 — RunBgmStopWorker` | 原音乐线程职责由实际停止线程／音频回调承接；完整业务／运行未验 |
| 2922 | 0x0052BE60 | `World::LoadAndPlayBGM` | 有正文 | `src/LegacyAudio.cpp:1381 — LoadAndPlayBGM` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2923 | 0x0052C160 | `sub_52C160` | 有正文 | `src/LegacyAudio.cpp:416 — LoadStreamingBgm_52C160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2924 | 0x0052C3D0 | `sub_52C3D0` | 有正文 | `src/LegacyAudio.cpp:1466 — Sub_52C3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2925 | 0x0052C450 | `World::PlaySe` | 有正文 | `src/LegacyAudio.cpp:1050 — PlaySe_52C450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2926 | 0x0052C680 | `sub_52C680` | 有正文 | `src/LegacyAudio.cpp:1133 — Sub_52C680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2927 | 0x0052C910 | `sub_52C910` | 有正文 | `src/LegacyAudio.cpp:692 — UpdateBgmRequest_52C910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2928 | 0x0052C9C0 | `sub_52C9C0` | 有正文 | `src/LegacyAudio.cpp:681 — SelectBgmTarget_52C9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2929 | 0x0052C9E0 | `sub_52C9E0` | 有正文 | `src/LegacyAudio.cpp:1011 — Sub_52C9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2930 | 0x0052CB50 | `sub_52CB50` | 有正文 | `src/LegacyAudio.cpp:1245 — Sub_52CB50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2931 | 0x0052CCA0 | `sub_52CCA0` | 有正文 | `src/LegacyAudio.cpp:1298 — Sub_52CCA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2932 | 0x0052CCE0 | `sub_52CCE0` | 有正文 | `src/LegacyAudio.cpp:950 — Sub_52CCE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2933 | 0x0052CD30 | `sub_52CD30` | 有正文 | `src/platform/MainLoopSupport.cpp:598 — Sub_52CD30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2934 | 0x0052CD40 | `sub_52CD40` | 有正文 | `src/LegacyAudio.cpp:963 — Sub_52CD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2935 | 0x0052CD60 | `sub_52CD60` | 有正文 | `src/LegacyAudio.cpp:1331 — Sub_52CD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2936 | 0x0052CDE0 | `sub_52CDE0` | 有正文 | `src/LegacyAudio.cpp:995 — Sub_52CDE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2937 | 0x0052CEA0 | `sub_52CEA0` | 有正文 | `src/network/NetworkRuntime.cpp:1445 — FindNextDelimitedField_52CEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2938 | 0x0052CED0 | `sub_52CED0` | 有正文 | `src/network/NetworkRuntime.cpp:1513 — ExtractDelimitedField_52CED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2939 | 0x0052CF30 | `sub_52CF30` | 有正文 | `src/network/NetworkRuntime.cpp:1472 — CopyDelimitedField_52CF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2940 | 0x0052CFA0 | `sub_52CFA0` | 有正文 | `src/network/NetworkRuntime.cpp:1533 — ParseDelimitedInteger_52CFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2941 | 0x0052CFF0 | `sub_52CFF0` | 有正文 | `src/network/NetworkRuntime.cpp:1548 — ParseDelimitedIntegerStatus_52CFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2942 | 0x0052D050 | `base62Decode` | 有正文 | `src/network/NetworkRuntime.cpp:1565 — Base62Decode_52D050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2943 | 0x0052D0D0 | `sub_52D0D0` | 有正文 | `src/network/NetworkRuntime.cpp:1599 — ParseDelimitedBase62_52D0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2944 | 0x0052D120 | `sub_52D120` | 有正文 | `src/Input.cpp:771 — DecodeEscapeLetter_52D120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2945 | 0x0052D150 | `Unescape` | 有正文 | `src/Input.cpp:786 — UnescapeLegacyText_52D150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2946 | 0x0052D1B0 | `sub_52D1B0` | 有正文 | `src/Input.cpp:633 — EscapeLegacyText_52D1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2947 | 0x0052D2D0 | `sub_52D2D0` | 有正文 | `src/Input.cpp:760 — EscapeInputText_52D2D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2948 | 0x0052D2F0 | `Unescape2` | 有正文 | `src/Input.cpp:815 — UnescapeLegacyTextAndNotify_52D2F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2949 | 0x0052D310 | `sub_52D310` | 有正文 | `src/LoginSavedState.cpp:89 — EncodeLegacyBytes_52D310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2950 | 0x0052D460 | `sub_52D460` | 有正文 | `src/LoginSavedState.cpp:171 — DecodeLegacyBytes_52D460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2951 | 0x0052D570 | `sub_52D570` | 有正文 | `src/Input.cpp:889 — RemoveLegacyBytes_52D570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2952 | 0x0052D5C0 | `Graphic::readGraphicData` | 有正文 | `src/GraphicDecode.cpp:6 — ReadGraphicData` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2953 | 0x0052D790 | `sub_52D790` | 有正文 | `src/LegacyUiRecord.cpp:10034 — Sub_52D790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2954 | 0x0052D7B0 | `sub_52D7B0` | 有正文 | `src/LegacyUiRecord.cpp:10041 — Sub_52D7B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2955 | 0x0052DC60 | `sub_52DC60` | 有正文 | `src/LegacyUiRecord.cpp:10161 — Sub_52DC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2956 | 0x0052DCE0 | `sub_52DCE0` | 有正文 | `src/LegacyUiRecord.cpp:10173 — Sub_52DCE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2957 | 0x0052E180 | `sub_52E180` | 有正文 | `src/world/SceneState.cpp:3969 — Sub_52E180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2958 | 0x0052E1D0 | `sub_52E1D0` | 有正文 | `src/world/SceneState.cpp:3987 — Sub_52E1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2959 | 0x0052E240 | `sub_52E240` | 有正文 | `src/LegacyUiRecord.cpp:10290 — Sub_52E240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2960 | 0x0052E600 | `sub_52E600` | 有正文 | `src/LegacyUiRecord.cpp:10366 — Sub_52E600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2961 | 0x0052E650 | `sub_52E650` | 有正文 | `src/LegacyUiRecord.cpp:10385 — Sub_52E650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2962 | 0x0052ECB0 | `sub_52ECB0` | 有正文 | `src/LegacyUiRecord.cpp:10508 — Sub_52ECB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2963 | 0x0052ED50 | `sub_52ED50` | 有正文 | `src/LegacyUiRecord.cpp:10524 — Sub_52ED50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2964 | 0x0052F460 | `sub_52F460` | 有正文 | `src/LegacyUiRecord.cpp:10681 — Sub_52F460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2965 | 0x0052F670 | `sub_52F670` | 有正文 | `src/LegacyUiRecord.cpp:10721 — Sub_52F670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2966 | 0x0052F910 | `sub_52F910` | 有正文 | `src/LegacyUiRecord.cpp:10786 — Sub_52F910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2967 | 0x0052FB60 | `sub_52FB60` | 有正文 | `src/LegacyRecordOrder.cpp:270 — CompareOrderedRecords_52FB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2968 | 0x0052FCC0 | `sub_52FCC0` | 有正文 | `src/LegacyRecordOrder.cpp:304 — BuildOrderedRecordIndices_52FCC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2969 | 0x0052FCF0 | `sub_52FCF0` | 有正文 | `src/LegacyUiRecord.cpp:10842 — Sub_52FCF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2970 | 0x0052FD60 | `sub_52FD60` | 有正文 | `src/LegacyUiRecord.cpp:10865 — Sub_52FD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2971 | 0x00530B70 | `sub_530B70` | 有正文 | `src/LegacyUiRecord.cpp:11078 — Sub_530B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2972 | 0x00530BA0 | `sub_530BA0` | 有正文 | `src/LegacyUiRecord.cpp:11088 — Sub_530BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2973 | 0x00532430 | `sub_532430` | 有正文 | `src/LegacyUiRecord.cpp:11430 — Sub_532430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2974 | 0x00532450 | `sub_532450` | 有正文 | `src/LegacyUiRecord.cpp:11439 — Sub_532450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2975 | 0x00532D80 | `sub_532D80` | 有正文 | `src/LegacyRecordOrder.cpp:263 — Sub_532D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2976 | 0x00532E70 | `sub_532E70` | 有正文 | `src/platform/StartupCommon.cpp:28 — CreateSaveDirectories` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2977 | 0x00532ED0 | `sub_532ED0` | 有正文 | `src/LoginSavedState.cpp:233 — LoadSavedPhysicalRecord_532ED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2978 | 0x00532FC0 | `sub_532FC0` | 有正文 | `src/LoginSavedState.cpp:278 — WriteDefaultSavedFile_532FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2979 | 0x005331A0 | `sub_5331A0` | 有正文 | `src/LoginSavedState.cpp:392 — InitializeSavedPhysicalRecord_5331A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2980 | 0x00533300 | `sub_533300` | 有正文 | `src/LoginSavedState.cpp:808 — InitializeSavedState_533300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2981 | 0x00533360 | `sub_533360` | 有正文 | `src/LoginSavedState.cpp:468 — ResetSavedState_533360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2982 | 0x005333D0 | `sub_5333D0` | 有正文 | `src/LoginSavedState.cpp:491 — ProbeSavedStateLocation_5333D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2983 | 0x00533470 | `sub_533470` | 有正文 | `src/LoginSavedState.cpp:530 — RebuildSavedSlotMap_533470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2984 | 0x00533530 | `sub_533530` | 有正文 | `src/LoginSavedState.cpp:580 — MigrateLegacySavedFiles_533530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2985 | 0x00533570 | `sub_533570` | 有正文 | `src/LoginSavedState.cpp:1109 — Sub_533570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2986 | 0x005335B0 | `sub_5335B0` | 有正文 | `src/LoginSavedState.cpp:1132 — Sub_5335B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2987 | 0x005335F0 | `sub_5335F0` | 有正文 | `src/LoginSavedState.cpp:1178 — Sub_5335F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2988 | 0x00533640 | `sub_533640` | 有正文 | `src/LoginSavedState.cpp:1155 — Sub_533640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2989 | 0x00533680 | `sub_533680` | 有正文 | `src/LoginSavedState.cpp:1205 — Sub_533680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2990 | 0x005336D0 | `sub_5336D0` | 有正文 | `src/LoginSavedState.cpp:1064 — Sub_5336D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2991 | 0x00533700 | `sub_533700` | 有正文 | `src/LoginSavedState.cpp:610 — StoreCurrentRecordToSelectedPhysical_533700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2992 | 0x00533730 | `sub_533730` | 有正文 | `src/LoginSavedState.cpp:1232 — Sub_533730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2993 | 0x00533860 | `sub_533860` | 有正文 | `src/LoginSavedState.cpp:1070 — Sub_533860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2994 | 0x005338F0 | `sub_5338F0` | 有正文 | `src/LoginSavedState.cpp:519 — FindFreeSavedPhysicalSlot_5338F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2995 | 0x00533910 | `sub_533910` | 有正文 | `src/world/MailState.cpp:861 — Sub_533910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2996 | 0x00533950 | `sub_533950` | 有正文 | `src/world/MailState.cpp:872 — Sub_533950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2997 | 0x00533B00 | `sub_533B00` | 有正文 | `src/world/MailState.cpp:898 — Sub_533B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2998 | 0x00533C80 | `sub_533C80` | 有正文 | `src/world/MailState.cpp:927 — Sub_533C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2999 | 0x00534290 | `sub_534290` | 有正文 | `src/world/MailState.cpp:1013 — Sub_534290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3000 | 0x005342E0 | `sub_5342E0` | 有正文 | `src/ui/WindowRuntime.cpp:45044 — Sub_5342E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3001 | 0x005342F0 | `sub_5342F0` | 有正文 | `src/world/MailState.cpp:1119 — Sub_5342F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3002 | 0x00534370 | `sub_534370` | 有正文 | `src/world/MailState.cpp:667 — Sub_534370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3003 | 0x005343A0 | `sub_5343A0` | 有正文 | `src/world/MailState.cpp:679 — Sub_5343A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3004 | 0x005343C0 | `sub_5343C0` | 有正文 | `src/world/MailState.cpp:722 — Sub_5343C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3005 | 0x00534480 | `sub_534480` | 有正文 | `src/world/MailState.cpp:743 — Sub_534480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3006 | 0x00534520 | `sub_534520` | 有正文 | `src/world/MailState.cpp:697 — Sub_534520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3007 | 0x005345A0 | `sub_5345A0` | 有正文 | `src/world/MailState.cpp:1312 — Sub_5345A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3008 | 0x00534770 | `sub_534770` | 有正文 | `src/world/MailState.cpp:1342 — Sub_534770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3009 | 0x005347F0 | `sub_5347F0` | 有正文 | `src/world/MailState.cpp:1359 — Sub_5347F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3010 | 0x005348A0 | `sub_5348A0` | 有正文 | `src/world/MailState.cpp:1278 — Sub_5348A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3011 | 0x00534910 | `sub_534910` | 有正文 | `src/world/MailState.cpp:1295 — Sub_534910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3012 | 0x00534980 | `DirectDrawCreateEx` | 有承接 | `src/render/SdlPresenter.cpp:377 — CreateRendererPresenter` | 原 DirectDrawCreateEx import 跳板；当前渲染创建由实际 SDL presenter 承接；完整上层／运行未验 |
| 3013 | 0x00534986 | `ImmGetConversionStatus` | 有承接 | `src/platform/ImeLifecycle.cpp:37 — ImmGetConversionStatus` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3014 | 0x0053498C | `ImmAssociateContext` | 有承接 | `src/platform/ImeLifecycle.cpp:31 — ImmAssociateContext` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3015 | 0x00534992 | `ImmCreateContext` | 有承接 | `src/platform/ImeLifecycle.cpp:28 — ImmCreateContext` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3016 | 0x00534998 | `ImmReleaseContext` | 有承接 | `src/platform/ImeLifecycle.cpp:50 — ImmReleaseContext` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3017 | 0x0053499E | `ImmIsIME` | 有承接 | `src/platform/ImeLifecycle.cpp:65 — ImmIsIME` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3018 | 0x005349A4 | `ImmSetOpenStatus` | 有承接 | `src/platform/ImeLifecycle.cpp:79 — ImmSetOpenStatus` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3019 | 0x005349AA | `ImmGetDescriptionA` | 有承接 | `src/world/RenderInput.cpp:195 — ImmGetDescriptionW` | 原入口为对应 Windows／Winsock import 跳板；原A入口由UTL02对应W调用承接，当前真实SDK调用存在，完整调用方／运行未验 |
| 3020 | 0x005349B0 | `ImmGetCompositionStringA` | 有承接 | `src/platform/ImeComposition.cpp:26 — ImmGetCompositionStringW` | 原入口为对应 Windows／Winsock import 跳板；原A入口由UTL02对应W调用承接，当前真实SDK调用存在，完整调用方／运行未验 |
| 3021 | 0x005349B6 | `ImmGetCandidateListA` | 有承接 | `src/platform/WindowProcedure.cpp:437 — ImmGetCandidateListW` | 原入口为对应 Windows／Winsock import 跳板；原A入口由UTL02对应W调用承接，当前真实SDK调用存在，完整调用方／运行未验 |
| 3022 | 0x00534A30 | `send` | 有承接 | `src/network/NetworkRuntime.cpp:107 — send` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3023 | 0x00534A36 | `closesocket` | 有承接 | `src/network/NetworkRuntime.cpp:4643 — closesocket` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3024 | 0x00534A3C | `recv` | 有承接 | `src/network/NetworkRuntime.cpp:4625 — recv` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3025 | 0x00534A42 | `__WSAFDIsSet` | 有承接 | `src/network/NetworkRuntime.cpp:4621 — FD_ISSET` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3026 | 0x00534A48 | `select` | 有承接 | `src/network/NetworkRuntime.cpp:859 — select` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3027 | 0x00534A4E | `WSAStartup` | 有承接 | `src/network/NetworkRuntime.cpp:4558 — WSAStartup` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3028 | 0x00534A54 | `WSACleanup` | 有承接 | `src/network/NetworkRuntime.cpp:4840 — WSACleanup` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3029 | 0x00534A5A | `WSAGetLastError` | 有承接 | `src/network/NetworkRuntime.cpp:729 — WSAGetLastError` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3030 | 0x00534A60 | `connect` | 有承接 | `src/LoginState.cpp:131 — connect` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3031 | 0x00534A66 | `gethostbyname` | 有承接 | `src/LoginState.cpp:160 — gethostbyname` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3032 | 0x00534A6C | `inet_addr` | 有承接 | `src/LoginState.cpp:158 — inet_addr` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3033 | 0x00534A72 | `htons` | 有承接 | `src/LoginState.cpp:157 — htons` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3034 | 0x00534A78 | `setsockopt` | 有承接 | `src/LoginState.cpp:145 — setsockopt` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3035 | 0x00534A7E | `ioctlsocket` | 有承接 | `src/LoginState.cpp:141 — ioctlsocket` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3036 | 0x00534A84 | `socket` | 有承接 | `src/LoginState.cpp:131 — socket` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3037 | 0x00534A8A | `Module32Next` | 有承接 | `src/platform/MainLoopSupport.cpp:194 — Module32Next` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3038 | 0x00534A90 | `Module32First` | 有承接 | `src/platform/MainLoopSupport.cpp:170 — Module32First` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3039 | 0x00534A96 | `CreateToolhelp32Snapshot` | 有承接 | `src/platform/MainLoopSupport.cpp:163 — CreateToolhelp32Snapshot` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3040 | 0x00534A9C | `Process32Next` | 有承接 | `src/platform/MainLoopSupport.cpp:433 — Process32Next` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3041 | 0x00534AA2 | `Process32First` | 有承接 | `src/platform/MainLoopSupport.cpp:427 — Process32First` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3042 | 0x00534AA8 | `DirectSoundCreate` | 有承接 | `src/platform/SdlAudioBackend.cpp:278 — SdlAudioBackend::Open` | 原 DirectSoundCreate 为 import 跳板；当前 Sub_524230 实际调用 SDL 后端 Open；不造旧 SDK 包装，完整上层／运行未验 |
| 3043 | 0x00534AB0 | `sub_534AB0` | 已排除 | `src/platform/LegacyExternalKeyboard.cpp:171 — Sub_534AB0` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3044 | 0x00534B80 | `DialogFunc` | 已排除 | `src/platform/LegacyExternalKeyboard.cpp:45 — DialogFunc_534B80` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3045 | 0x00534EF0 | `sub_534EF0` | 有正文 | `src/platform/LegacyNetworkAdapter.cpp:16 — Sub_534EF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3046 | 0x00535090 | `sub_535090` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3047 | 0x00535290 | `sub_535290` | 已排除 | `src/platform/LegacyExternalKeyboardObject.cpp:19 — DeleteExternalKeyboardVirtual_535290` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3048 | 0x00535310 | `sub_535310` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3049 | 0x005354C0 | `sub_5354C0` | 已排除 | `src/platform/LegacyExternalKeyboardObject.cpp:41 — GetExternalKeyboardResult_5354C0` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3050 | 0x005354D0 | `sub_5354D0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3051 | 0x00535540 | `sub_535540` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3052 | 0x00536960 | `nullsub_4` | 空方法 | `src/platform/LegacyMouse.cpp:136 — EmptyLegacyExit_536960` | 已有CPP空方法与原RET记录保留；按用户决定标为空方法，不再核对此入口 |
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
| 3127 | 0x0053A200 | `sub_53A200` | 有承接 | `src/GraphicRead.cpp:275 — records.try_emplace(readWord(record), nullptr)；src/GraphicInfo.cpp:236 — BorrowGraphicFirstRecords().emplace(firstMapNo, firstRecord)` | 原 9 条指令为非空目标的两 W32 pair 复制，直接原引用含 45AF60／523D20；当前对应 map 插入构造实际 pair，键宽保持、指针原生拓宽；完整 owner／运行未验 |
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
| 3143 | 0x0053AF90 | `sub_53AF90` | 有正文 | `src/GraphicRead.cpp:385 — DecompressDbf_53AF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3144 | 0x0053B1D0 | `_fclose` | 有承接 | `src/platform/LegacyStdio.cpp:165 — CloseLegacyStream_53B1D0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3145 | 0x0053B226 | `_fgets` | 有承接 | `src/platform/LegacyStdio.cpp:32 — ReadLegacyStreamLine_53B226` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3146 | 0x0053B27D | `_rewind` | 有承接 | `src/platform/LegacyStdio.cpp:82 — RewindLegacyStream_53B27D` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3147 | 0x0053B2D1 | `__fsopen` | 有承接 | `src/platform/LegacyStdio.cpp:19 — OpenSharedLegacyStream_53B2D1` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3148 | 0x0053B2F1 | `_fopen` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3149 | 0x0053B304 | `_sprintf` | 有承接 | `src/CharacterListRequest.cpp:163 — std::sprintf` | DCF01直接MSVC／标准库承接，原包装退休；现有800处调用及原合同保持；完整业务／运行未验 |
| 3150 | 0x0053B356 | `_fputs` | 有承接 | `src/platform/LegacyStdio.cpp:62 — WriteLegacyStreamString_53B356` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3151 | 0x0053B399 | `__onexit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3152 | 0x0053B406 | `_atexit` | 有承接 | `src/platform/LegacyMouse.cpp:146 — RegisterLegacyPrimaryEmptyExit` | 原库入口沿真实std::atexit消费者／MSVC服务承接；CryptInt 24组另由LCI01原生构析承接，无手写登记；未记机器ABI或运行已验 |
| 3153 | 0x0053B418 | `___onexitinit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3154 | 0x0053B447 | `sub_53B447` | 有承接 | `CMakeLists.txt:164 — 宿主CRT启动；src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyFormat.hpp:5 — MSVC std::vfprintf直接声明；src/platform/LegacyFormat.hpp:5 — MSVC std::vfscanf直接声明` | 原53C8CD在C／C++初始化前从58F864间接调用；旧六回调／CPU探测／x87默认精度随MC01-C2退休，当前MSVC整体启动／格式／扫描服务；不新增旧回调槽或声明硬件状态等价 |
| 3155 | 0x0053B45E | `nullsub_6` | 空方法 | `—` | 原入口单RET，58F868／58F86C两数据槽记录保留；按用户决定标为空方法，结束职责／调用／承接核对，不补CPP包装 |
| 3156 | 0x0053B45F | `__cfltcvt_init_7` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3157 | 0x0053B498 | `__ftol` | 有承接 | `src/Palette.cpp:138 — TruncatedLowByte；src/render/LegacyTextureDrawing.cpp:132 — TruncateTextureCoordinate` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3158 | 0x0053B4BF | `sub_53B4BF` | 有正文 | `src/platform/MainLoopSupport.cpp:690 — SeedRandom_53B4BF` | LCG01标准线性同余引擎承接，原序列／共享状态保持；完整业务／接线／运行未验 |
| 3159 | 0x0053B4C9 | `rand` | 有正文 | `src/platform/MainLoopSupport.cpp:697 — NextRandom_53B4C9` | LCG01标准线性同余引擎承接，原序列／共享状态保持；完整业务／接线／运行未验 |
| 3160 | 0x0053B4E7 | `delete` | 有承接 | `src/platform/LegacyMemory.cpp:60 — FreeLegacyByteBuffer` | 当前 IDA 的原 delete 叶调用 free；本线实际通用释放使用 std::free，与已批准 malloc 分配族成组承接；完整 owner／运行未验 |
| 3161 | 0x0053B4F2 | `?_JumpToContinuation@@YGXPAXPAUEHRegistrationNode@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3162 | 0x0053B526 | `sub_53B526` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3163 | 0x0053B52D | `sub_53B52D` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3164 | 0x0053B534 | `?_UnwindNestedFrames@@YGXPAUEHRegistrationNode@@PAUEHExceptionRecord@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3165 | 0x0053B583 | `___CxxFrameHandler` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3166 | 0x0053B5B9 | `?_CallCatchBlock2@@YAPAXPAUEHRegistrationNode@@PBU_s_FuncInfo@@PAXHK@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3167 | 0x0053B60D | `?CatchGuardHandler@@YA?AW4_EXCEPTION_DISPOSITION@@PAUEHExceptionRecord@@PAUCatchGuardRN@@PAX2@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3168 | 0x0053B632 | `?_CallSETranslator@@YAHPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAX2PBU_s_FuncInfo@@H1@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3169 | 0x0053B6E6 | `?TranslatorGuardHandler@@YA?AW4_EXCEPTION_DISPOSITION@@PAUEHExceptionRecord@@PAUTranslatorGuardRN@@PAX2@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3170 | 0x0053B75B | `?_GetRangeOfTrysToCheck@@YAPBU_s_TryBlockMapEntry@@PBU_s_FuncInfo@@HHPAI1@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3171 | 0x0053B7D8 | `__global_unwind2` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3172 | 0x0053B7F8 | `__unwind_handler` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3173 | 0x0053B81A | `__local_unwind2` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3174 | 0x0053B882 | `__abnormal_termination` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3175 | 0x0053B8A5 | `__NLG_Notify1` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3176 | 0x0053B8AE | `__NLG_Notify` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3177 | 0x0053B8C6 | `_localtime` | 有承接 | `src/platform/LegacyTime.cpp:39 — LocalTime_53B8C6` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3178 | 0x0053BA26 | `_time` | 有承接 | `src/platform/LegacyTime.cpp:13 — ReadLegacyTime32_53BA26` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3179 | 0x0053BB02 | `_fflush` | 有承接 | `src/platform/LegacyStdio.cpp:159 — FlushLegacyStream_53BB02` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3180 | 0x0053BB3D | `__flush` | 有承接 | `src/platform/LegacyStdio.cpp:159 — FlushLegacyStream_53BB02` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3181 | 0x0053BB99 | `sub_53BB99` | 有承接 | `src/platform/LegacyStdio.cpp:171 — FlushAllLegacyStreams_53BB99` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3182 | 0x0053BBA2 | `_flsall` | 有承接 | `src/platform/LegacyStdio.cpp:171 — FlushAllLegacyStreams_53BB99` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3183 | 0x0053BC0F | `sub_53BC0F` | 有承接 | `src/Input.cpp:1380 — std::fprintf` | DCF01直接MSVC／标准库承接，原包装退休；现有11处调用及原合同保持；完整业务／运行未验 |
| 3184 | 0x0053BC41 | `_fseek` | 有承接 | `src/platform/LegacyStdio.cpp:75 — SeekLegacyStream_53BC41` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3185 | 0x0053BCCD | `_fread` | 有承接 | `src/platform/LegacyStdio.cpp:39 — ReadLegacyStream_53BCCD` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3186 | 0x0053BDC0 | `__alloca_probe` | 有承接 | `src/network/NetworkRuntime.cpp:221 — Sub_403C50；src/network/NetworkRuntime.cpp:303 — Sub_403E50` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3187 | 0x0053BDEF | `_isspace` | 有承接 | `src/Input.cpp:595 — std::isspace` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3188 | 0x0053BE17 | `_iscntrl` | 有承接 | `src/Input.cpp:598 — std::iscntrl` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3189 | 0x0053BE40 | `_strstr` | 有承接 | `src/CharacterSelectScene.cpp:925 — std::strstr` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3190 | 0x0053BEC0 | `_strncpy` | 有承接 | `src/platform/MainLoopSupport.cpp:521 — std::strncpy` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3191 | 0x0053BFC0 | `_strncmp` | 有承接 | `src/YobiRuntime.cpp:35904 — std::strncmp` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3192 | 0x0053BFF8 | `cAlloc` | 有承接 | `src/platform/LegacyMemory.cpp:19 — CallocateLegacyMemory_53BFF8` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3193 | 0x0053C0A9 | `free` | 有承接 | `src/ui/WindowRuntime.cpp:7908 — std::free` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3194 | 0x0053C112 | `_qsort` | 有承接 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420；src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3195 | 0x0053C266 | `_shortsort` | 有承接 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420；src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3196 | 0x0053C2B4 | `_swap` | 有承接 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420；src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3197 | 0x0053C2F0 | `_strchr` | 有承接 | `src/LoginCapability4Modal.cpp:90 — std::wcschr` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3198 | 0x0053C3AC | `_malloc` | 有承接 | `src/ui/WindowRuntime.cpp:7895 — std::malloc` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3199 | 0x0053C3BE | `__nh_malloc` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3200 | 0x0053C3EA | `sub_53C3EA` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)` | 原仅__nh_malloc调用并转旧两池／HeapAlloc；实际std::malloc整体承接分配及已批库内策略，不复刻模式，业务／owner／运行未验 |
| 3201 | 0x0053C45E | `??_L@YGXPAXIHP6EX0@Z1@Z` | 有承接 | `src/world/WorldObjectState.cpp:332 — ConstructWorldObjects` | LCI15无调用的CRT数组helper／筛选已删除；std::array构析／前缀清理／终止沿C++／MSVC，原身份保留；硬件SEH／完整运行未验 |
| 3202 | 0x0053C4E0 | `??_M@YGXPAXIHP6EX0@Z@Z` | 有承接 | `src/world/WorldObjectState.cpp:340 — DestroyWorldObjects` | LCI15无调用的CRT数组helper／筛选已删除；std::array构析／前缀清理／终止沿C++／MSVC，原身份保留；硬件SEH／完整运行未验 |
| 3203 | 0x0053C560 | `?__ArrayUnwind@@YGXPAXIHP6EX0@Z@Z` | 有承接 | `src/world/WorldObjectState.cpp:332 — ConstructWorldObjects` | LCI15无调用的CRT数组helper／筛选已删除；std::array构析／前缀清理／终止沿C++／MSVC，原身份保留；硬件SEH／完整运行未验 |
| 3204 | 0x0053C5BE | `_atol` | 有承接 | `include/cg/platform/LegacyTime.hpp:7 — std::atol` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3205 | 0x0053C649 | `_atoi` | 有承接 | `src/LoginScene.cpp:444 — std::atoi` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3206 | 0x0053C654 | `__mkdir` | 有承接 | `src/Input.cpp:1347 — ::_mkdir` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3207 | 0x0053C680 | `_fwrite` | 有承接 | `src/platform/LegacyStdio.cpp:47 — WriteLegacyStream_53C680` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3208 | 0x0053C790 | `_strncat` | 有承接 | `src/Input.cpp:293 — AppendText（UTF-16 STL正文插入）` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3209 | 0x0053C8B3 | `_fgetc` | 有承接 | `src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3210 | 0x0053C8CD | `__cinit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyMouse.cpp:77 — gSurfaceTransformCacheStartup` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3211 | 0x0053C8FA | `_exit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3212 | 0x0053C90B | `__exit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3213 | 0x0053C91C | `_doexit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3214 | 0x0053C9B5 | `__initterm` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3215 | 0x0053C9CF | `??2@YAPAXI@Z` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer；include/cg/platform/LegacyMemory.hpp:26 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3216 | 0x0053C9DD | `_mbstowcs` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3217 | 0x0053CADC | `_sscanf` | 有承接 | `src/platform/StartupCommon.cpp:16 — std::sscanf` | DCF01直接MSVC／标准库承接，原包装退休；现有21处调用及原合同保持；完整业务／运行未验 |
| 3218 | 0x0053CB10 | `__aulldiv` | 有承接 | `src/platform/LegacyFormat.cpp:21 — DivideLegacyUnsigned64_53CB10` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3219 | 0x0053CB78 | `_ftell` | 有承接 | `src/platform/LegacyStdio.cpp:69 — TellLegacyStream_53CB78` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3220 | 0x0053CCD0 | `__fstat` | 有承接 | `src/platform/LegacyTime.cpp:60 — ReadLegacyFileCreationTime32；include/cg/platform/LegacyTime.hpp:35 — ReadLegacyFileCreationTime32` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3221 | 0x0053CF50 | `__strcmpi` | 有承接 | `src/AnimationRuntime.cpp:395 — ::_stricmp` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3222 | 0x0053CFDC | `_fputc` | 有承接 | `src/platform/LegacyStdio.cpp:55 — WriteLegacyStreamByte_53CFDC` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3223 | 0x0053D000 | `__CIpow` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3224 | 0x0053D019 | `_pow` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3225 | 0x0053D022 | `sub_53D022` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3226 | 0x0053D1F5 | `sub_53D1F5` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3227 | 0x0053D21D | `__snprintf` | 有承接 | `src/LegacyUiRecord.cpp:3880 — ::_snprintf` | DCF01直接MSVC／标准库承接，原包装退休；现有9处调用及原合同保持；完整业务／运行未验 |
| 3228 | 0x0053D26E | `_strtok` | 有承接 | `src/ui/WindowRuntime.cpp:26906 — std::strtok` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3229 | 0x0053D30A | `__close` | 有承接 | `src/platform/LegacyLowIo.cpp:22 — CloseLegacyDescriptor_53D30A` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3230 | 0x0053D3BD | `__filelength` | 有承接 | `src/platform/LegacyLowIo.cpp:16 — GetLegacyDescriptorLength_53D3BD` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3231 | 0x0053D435 | `__open` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3232 | 0x0053D44C | `__sopen` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3233 | 0x0053D710 | `_memcpy` | 有承接 | `src/network/NetworkRuntime.cpp:4760 — ConsumeReceiveQueue_508230；src/network/NetworkRuntime.cpp:4817 — ConsumeSendQueue_508280` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3234 | 0x0053DA45 | `_strtol` | 有承接 | `include/cg/platform/LegacyTime.hpp:7 — std::strtol` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3235 | 0x0053DA5C | `_strtoxl` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3236 | 0x0053DC64 | `_tolower` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3237 | 0x0053DD2F | `__chdir` | 有承接 | `src/platform/StartupCommon.cpp:31 — ::_chdir` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3238 | 0x0053DDB5 | `start` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3239 | 0x0053DEAB | `__amsg_exit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3240 | 0x0053DED0 | `_fast_error_exit` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3241 | 0x0053DEF3 | `nullsub_7` | 空方法 | `—` | 原入口单RET、当前无已建入引用的记录保留；按用户决定标为空方法，结束职责／调用／承接核对，不补CPP包装 |
| 3242 | 0x0053DEF4 | `__input` | 有承接 | `src/platform/LegacyFormat.hpp:5 — std::vfscanf` | DCF01直接MSVC／标准库承接，原包装退休；无当前CPP消费者，头文件导入真实MSVC CRT提供者；完整业务／运行未验 |
| 3243 | 0x0053E919 | `__hextodec` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3244 | 0x0053E950 | `_fgetc_0` | 有承接 | `src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3245 | 0x0053E96A | `__un_inc` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3246 | 0x0053E981 | `__whiteout` | 有承接 | `src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3247 | 0x0053E9A5 | `___initstdio` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3248 | 0x0053EA4A | `___endstdio` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3249 | 0x0053EA5E | `__stbuf` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3250 | 0x0053EAEB | `__ftbuf` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3251 | 0x0053EB28 | `sub_53EB28` | 有承接 | `src/platform/LegacyFormat.hpp:5 — std::vfprintf` | DCF01直接MSVC／标准库承接，原包装退休；无当前CPP消费者，头文件导入真实MSVC CRT提供者；完整业务／运行未验 |
| 3252 | 0x0053F2C6 | `_write_char` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3253 | 0x0053F2FB | `_write_multi_char` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3254 | 0x0053F32C | `_write_string` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3255 | 0x0053F364 | `_get_int_arg` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3256 | 0x0053F371 | `_get_int64_arg` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3257 | 0x0053F381 | `_get_short_arg` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3258 | 0x0053F38F | `__freebuf` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3259 | 0x0053F3BA | `__filbuf` | 有承接 | `src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3260 | 0x0053F493 | `__lseek` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3261 | 0x0053F52D | `__ioinit` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3262 | 0x0053F6D8 | `__openfile` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3263 | 0x0053F848 | `__getstream` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3264 | 0x0053F8C0 | `__flsbuf` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3265 | 0x0053F9E0 | `_strlen` | 有承接 | `include/cg/platform/LegacyTime.hpp:8 — std::strlen` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3266 | 0x0053FA5B | `sub_53FA5B` | 有承接 | `src/platform/LegacyMemory.cpp:27 — ReallocateLegacyMemory_53FA5B` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3267 | 0x0053FCFB | `sub_53FCFB` | 有承接 | `src/platform/LegacyMemory.cpp:12 — GetLegacyAllocationSize_53FCFB` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3268 | 0x0053FD5C | `__setdefaultprecision` | 有承接 | `src/YobiRuntime.cpp:37392 — RenderChara_521F20；src/world/SceneState.cpp:10576 — Sub_51D790` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3269 | 0x0053FD6E | `__ms_p5_test_fdiv` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3270 | 0x0053FDAC | `__ms_p5_mp_test_fdiv` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3271 | 0x0053FDD5 | `__forcdecpt` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3272 | 0x0053FE2F | `__cropzeros` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3273 | 0x0053FE7D | `__positive` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3274 | 0x0053FE95 | `__fassign` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC std::vfscanf直接声明；src/platform/LegacyFormat.hpp:5 — MSVC std::sscanf／std::vsscanf直接声明` | 原输入扫描的浮点赋值回调，登记58FB58并由53E324调用，实际std::vfscanf／std::vsscanf承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3275 | 0x0053FED3 | `__cftoe` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3276 | 0x0053FFD7 | `__cftof` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3277 | 0x005400B5 | `__cftog` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3278 | 0x00540150 | `__cftoe_g` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3279 | 0x00540177 | `__cftof_g` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3280 | 0x0054019A | `__cfltcvt` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3281 | 0x005401EB | `__shift` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3282 | 0x00540210 | `___InternalCxxFrameHandler` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3283 | 0x005402AB | `?FindHandler@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PAXPBU_s_FuncInfo@@EH1@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3284 | 0x00540452 | `?FindHandlerForForeignException@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PAXPBU_s_FuncInfo@@HH1@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3285 | 0x005404FA | `?TypeMatch@@YAHPBU_s_HandlerType@@PBU_s_CatchableType@@PBU_s_ThrowInfo@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3286 | 0x00540557 | `___FrameUnwindToState` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3287 | 0x005405F5 | `unknown_libname_49` | 有承接 | `src/world/WorldObjectState.cpp:332 — ConstructWorldObjects` | LCI15无调用的CRT数组helper／筛选已删除；std::array构析／前缀清理／终止沿C++／MSVC，原身份保留；硬件SEH／完整运行未验 |
| 3288 | 0x0054060B | `?CatchIt@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PAXPBU_s_FuncInfo@@PBU_s_HandlerType@@PBU_s_CatchableType@@PBU_s_TryBlockMapEntry@@H1E@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3289 | 0x00540686 | `?CallCatchBlock@@YAPAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PBU_s_FuncInfo@@PAXHK@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3290 | 0x00540792 | `?ExFilterRethrow@@YAHPAU_EXCEPTION_POINTERS@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3291 | 0x005407BC | `?BuildCatchObject@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PBU_s_HandlerType@@PBU_s_CatchableType@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3292 | 0x00540980 | `unknown_libname_50` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3293 | 0x005409E7 | `?AdjustPointer@@YAPAXPAXABUPMD@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3294 | 0x00540A10 | `__CallSettingFrame@12` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3295 | 0x00540A5C | `sub_540A5C` | 有承接 | `src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3296 | 0x00540AB2 | `sub_540AB2` | 有承接 | `src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C；CMakeLists.txt:164 — 宿主VCRuntime` | 原八处引用均在C++ handler／展开链；读58FB74=540A5C、SEH保护后交终止；当前VCRuntime／实际std::terminate承接，不把标准流构造当本叶直接实现 |
| 3297 | 0x00540B08 | `___tzset` | 有承接 | `src/platform/LegacyTime.cpp:42 — ::_localtime32(&timestamp)；src/platform/LegacyTime.cpp:35 — ::_mktime32(&calendar)` | 原时区初始化包装仅由53B8C6／541160调用并转540B1D；实际::_localtime32／::_mktime32整体承接，不保原私有函数划分；MSVC库内策略沿获批边界，业务／寿命／运行未验 |
| 3298 | 0x00540B1D | `__tzset` | 有承接 | `src/platform/LegacyTime.cpp:42 — ::_localtime32(&timestamp)；src/platform/LegacyTime.cpp:35 — ::_mktime32(&calendar)` | 原时区刷新仅由540B08调用，原getenv／Win32时区及转换职责归同两实际MSVC日历服务；不模拟旧TZ缓存／私有全局或声称当前DLL逐叶调用，业务／运行未验 |
| 3299 | 0x00540D7B | `__isindst` | 有承接 | `src/platform/LegacyTime.cpp:42 — ::_localtime32(&timestamp)；src/platform/LegacyTime.cpp:35 — ::_mktime32(&calendar)` | 原夏令时判断仅在旧localtime／本地转时间戳链内消费，四次转540F27；由实际::_localtime32／::_mktime32整体承接，旧私有算法退休，完整日期／失败／运行未验 |
| 3300 | 0x00540F27 | `_cvtdate` | 有承接 | `src/platform/LegacyTime.cpp:42 — ::_localtime32(&timestamp)；src/platform/LegacyTime.cpp:35 — ::_mktime32(&calendar)` | 原夏令时转换仅由540D7B内四处调用；同两MSVC公共日历服务整体承接，不复刻旧辅助边界／存储，不宣原体或机器ABI等价，业务／运行未验 |
| 3301 | 0x00541067 | `_gmtime` | 有承接 | `src/platform/LegacyTime.cpp:42 — ::_localtime32(&timestamp)` | 原UTC分解仅在旧localtime内三处调用；当前实际::_localtime32整体承接该日历转换链，不宣称直接调用::_gmtime32或新增UTC接口，九S32借用字段及完整寿命／运行未验 |
| 3302 | 0x00541160 | `___loctotime_t` | 有承接 | `src/platform/LegacyTime.cpp:23 — ConvertLocalTime_541160` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3303 | 0x00541222 | `__commit` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3304 | 0x00541279 | `__write` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3305 | 0x00541426 | `__read` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3306 | 0x00541620 | `_memcpy_0` | 有承接 | `src/platform/LegacyMemory.cpp:33 — CopyLegacyMemory_541620` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3307 | 0x00541955 | `__isctype` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3308 | 0x005419D0 | `_memset` | 有承接 | `src/platform/LegacyMemory.cpp:41 — FillLegacyMemory_5419D0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3309 | 0x00541A28 | `__callnewh` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3310 | 0x00541A43 | `sub_541A43` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)；CMakeLists.txt:164 — 宿主Windows CRT进入WinMain` | 原仅堆选择器541A70使用的模块MZ／PE两字节读取；旧选择机制随MSVC启动／堆策略承接，不新增模块头API，非原机制等价／运行通过 |
| 3311 | 0x00541A70 | `sub_541A70` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)；CMakeLists.txt:164 — 宿主Windows CRT进入WinMain` | 原仅541BB8调用的OS／环境／模块堆选择；当前MSVC启动与实际标准分配库整体承接，不复刻旧环境选择器，业务／启动／运行未验 |
| 3312 | 0x00541BB8 | `sub_541BB8` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)；CMakeLists.txt:164 — 宿主Windows CRT进入WinMain` | 原仅旧start调用的HeapCreate／池初始化／失败HeapDestroy；当前MSVC宿主启动及实际标准分配族承接，不造旧heap或失败28分支，启动／SDK／运行未验 |
| 3313 | 0x00541C15 | `___sbh_heap_init` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3314 | 0x00541C5D | `___sbh_find_block` | 有承接 | `src/platform/LegacyMemory.cpp:60 — FreeLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3315 | 0x00541C88 | `sub_541C88` | 有承接 | `src/platform/LegacyMemory.cpp:62 — std::free(memory)；src/platform/LegacyMemory.cpp:29 — std::realloc(memory, byteCount)` | 原只在旧free／realloc内消费的私有回收／元数据辅助；实际std::free／std::realloc整体承接，不宣原虚存策略等价，完整owner／失败／运行未验 |
| 3316 | 0x00541FB1 | `___sbh_alloc_block` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3317 | 0x005422BA | `___sbh_alloc_new_region` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3318 | 0x0054236B | `___sbh_alloc_new_group` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3319 | 0x00542466 | `___sbh_resize_block` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3320 | 0x0054275C | `sub_54275C` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)；src/platform/LegacyMemory.cpp:22 — std::calloc(count, size)；src/platform/LegacyMemory.cpp:29 — std::realloc(memory, byteCount)` | 原启动及542A54消费的私有区域／页初始化与失败回收；实际标准分配族／宿主启动整体承接，不复刻旧页／块布局，全部owner／运行未验 |
| 3321 | 0x005428A0 | `sub_5428A0` | 有承接 | `src/platform/LegacyMemory.cpp:62 — std::free(memory)；src/platform/LegacyMemory.cpp:29 — std::realloc(memory, byteCount)` | 原仅5428F6转入的私有虚存／HeapFree释放链；实际标准释放／扩容库整体承接，不宣当前库保留旧划分，owner／正常寿命／运行未验 |
| 3322 | 0x005428F6 | `sub_5428F6` | 有承接 | `src/platform/LegacyMemory.cpp:62 — std::free(memory)；src/platform/LegacyMemory.cpp:29 — std::realloc(memory, byteCount)` | 原仅542A0F转入并调用5428A0的私有decommit／释放辅助；实际std::free／std::realloc整体承接，不模拟旧元数据，owner／失败／运行未验 |
| 3323 | 0x005429B8 | `sub_5429B8` | 有承接 | `src/platform/LegacyMemory.cpp:62 — std::free(memory)；src/platform/LegacyMemory.cpp:29 — std::realloc(memory, byteCount)；src/platform/LegacyMemory.cpp:14 — ::_msize(memory)` | 原仅旧free／realloc／msize消费的块定位辅助；相应实际MSVC公共服务整体承接，不推定旧物理容量／对齐或原体等价，owner／运行未验 |
| 3324 | 0x00542A0F | `sub_542A0F` | 有承接 | `src/platform/LegacyMemory.cpp:62 — std::free(memory)；src/platform/LegacyMemory.cpp:29 — std::realloc(memory, byteCount)` | 原仅旧free／realloc消费并转5428F6的回收辅助；实际标准服务整体承接，不新建池状态或包装，完整owner／失败／运行未验 |
| 3325 | 0x00542A54 | `sub_542A54` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)；src/platform/LegacyMemory.cpp:22 — std::calloc(count, size)；src/platform/LegacyMemory.cpp:29 — std::realloc(memory, byteCount)` | 原旧calloc／分配／realloc消费并转542C5C／54275C的私有分配链；实际标准分配族整体承接，库内策略沿MC01-C4，完整owner／运行未验 |
| 3326 | 0x00542C5C | `sub_542C5C` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)；src/platform/LegacyMemory.cpp:22 — std::calloc(count, size)；src/platform/LegacyMemory.cpp:29 — std::realloc(memory, byteCount)` | 原仅542A54内两处调用的私有分配辅助；实际标准分配族整体承接，不伪称同名新叶或旧算法等价，owner／SDK／运行未验 |
| 3327 | 0x00542D80 | `sub_542D80` | 有承接 | `src/platform/LegacyMemory.cpp:29 — std::realloc(memory, byteCount)` | 原仅旧realloc消费的私有扩容辅助；实际std::realloc整体承接，不复刻旧元数据／同名接口，完整块内容／失败／owner／运行未验 |
| 3328 | 0x00542E34 | `__except_handler3` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3329 | 0x00542EF1 | `__seh_longjmp_unwind@4` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3330 | 0x00542F10 | `__allmul` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC std::vfscanf直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3331 | 0x00542F44 | `sub_542F44` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入；src/platform/LegacyLowIo.cpp:24 — ::_close(descriptor)` | 原九个旧文件／目录父的系统错误→DOS错误／errno转换，当前实际MSVC文件／目录服务及同线程错误访问整体承接，不手写旧表／影子槽，业务错误消费／owner／运行未验 |
| 3332 | 0x00543097 | `zerotoxdone` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3333 | 0x005430E2 | `_expbigret` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3334 | 0x0054310E | `__ffexpm1` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3335 | 0x00543151 | `_isintTOS` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3336 | 0x00543176 | `_isintTOSret` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3337 | 0x00543177 | `notanint` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3338 | 0x0054317E | `evenint` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3339 | 0x00543185 | `_usepowhlp` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3340 | 0x00543390 | `__startTwoArgErrorHandling` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3341 | 0x005433A7 | `__startOneArgErrorHandling` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3342 | 0x005433F0 | `__twoToTOS` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3343 | 0x00543405 | `__load_CW` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3344 | 0x00543435 | `__fload_withFB` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3345 | 0x00543489 | `special_exp` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3346 | 0x005434C5 | `__check_overflow_exit` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3347 | 0x005434D9 | `__check_range_exit` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3348 | 0x0054357C | `__powhlp` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3349 | 0x005436AC | `__d_inttype` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3350 | 0x00543711 | `__alloc_osfhnd` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3351 | 0x005437A6 | `__set_osfhnd` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3352 | 0x0054381D | `__free_osfhnd` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3353 | 0x00543897 | `__get_osfhandle` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3354 | 0x005438D4 | `__chsize` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3355 | 0x00543A1A | `_toupper` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3356 | 0x00543AE8 | `___crtLCMapStringA` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3357 | 0x00543D0C | `_strncnt` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3358 | 0x00543D37 | `__mbctoupper` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3359 | 0x00543DB2 | `__XcptFilter` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3360 | 0x00543EF3 | `_xcptlookup` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3361 | 0x00543F36 | `__wincmdln` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3362 | 0x00543F8E | `__setenvp` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3363 | 0x00544047 | `__setargv` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3364 | 0x005440E0 | `_parse_cmdline` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3365 | 0x00544294 | `___crtGetEnvironmentStringsA` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3366 | 0x005443C6 | `__FF_MSGBANNER` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3367 | 0x005443FF | `sub_5443FF` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — std::terminate；CMakeLists.txt:164 — 宿主CRT启动／终止诊断` | 原五处引用来自旧致命错误／消息banner／abort，按错误码输出CRT弹窗或标准错误；当前MSVC库内诊断整体承接，旧R600x／钩子／缓存沿MC01-C3退休 |
| 3368 | 0x00544552 | `_mbtowc` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3369 | 0x00544620 | `__allshl` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC std::vfscanf直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3370 | 0x0054463F | `_ungetc` | 有承接 | `src/platform/LegacyStdio.cpp:95 — UnreadLegacyStreamByte_54463F` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3371 | 0x005446AD | `__fcloseall` | 有承接 | `src/platform/LegacyStdio.cpp:177 — CloseAllLegacyStreams_5446AD` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3372 | 0x00544705 | `__isatty` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3373 | 0x0054472B | `_wctomb` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3374 | 0x005447A0 | `__aullrem` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3375 | 0x00544815 | `__getbuf` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3376 | 0x00544859 | `__control87` | 有承接 | `src/YobiRuntime.cpp:37392 — RenderChara_521F20；src/world/SceneState.cpp:10576 — Sub_51D790` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3377 | 0x0054488E | `__controlfp` | 有承接 | `src/YobiRuntime.cpp:37392 — RenderChara_521F20；src/world/SceneState.cpp:10576 — Sub_51D790` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3378 | 0x005448A4 | `__abstract_cw` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3379 | 0x00544936 | `__hw_cw` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3380 | 0x005449BF | `__ZeroTail` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3381 | 0x00544A08 | `__IncMan` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3382 | 0x00544A5E | `__RoundMan` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3383 | 0x00544AEA | `__CopyMan` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3384 | 0x00544B05 | `__FillZeroMan` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3385 | 0x00544B11 | `__IsZeroMan` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3386 | 0x00544B2C | `__ShrMan` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3387 | 0x00544BB9 | `__ld12cvt` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC std::vfscanf直接声明；src/platform/LegacyFormat.hpp:5 — MSVC std::sscanf／std::vsscanf直接声明` | 原544D25／544D3B两输入转换叶的参数表转换支撑，实际std::vfscanf／std::vsscanf整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3388 | 0x00544D25 | `sub_544D25` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC std::vfscanf直接声明；src/platform/LegacyFormat.hpp:5 — MSVC std::sscanf／std::vsscanf直接声明` | 原12B中间值按5921C0的六W32表转换为双W32目标，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3389 | 0x00544D3B | `sub_544D3B` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC std::vfscanf直接声明；src/platform/LegacyFormat.hpp:5 — MSVC std::sscanf／std::vsscanf直接声明` | 原12B中间值按5921D8的六W32表转换为单W32目标，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3390 | 0x00544D51 | `sub_544D51` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC std::vfscanf直接声明；src/platform/LegacyFormat.hpp:5 — MSVC std::sscanf／std::vsscanf直接声明` | 原strgtold12→544D25链，仅53FE95双W32赋值分支使用，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3391 | 0x00544D7E | `sub_544D7E` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC std::vfscanf直接声明；src/platform/LegacyFormat.hpp:5 — MSVC std::sscanf／std::vsscanf直接声明` | 原strgtold12→544D3B链，仅53FE95单W32赋值分支使用，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3392 | 0x00544DB0 | `_strcpy` | 有承接 | `include/cg/platform/LegacyTime.hpp:8 — std::strcpy` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3393 | 0x00544DC0 | `_strcat` | 有承接 | `include/cg/platform/LegacyTime.hpp:8 — std::strcat` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3394 | 0x00544EA0 | `__fptostr` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3395 | 0x00544F17 | `__fltout` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3396 | 0x00544F7B | `___dtold` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3397 | 0x00545031 | `__fptrap` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3398 | 0x0054503A | `?__CxxUnhandledExceptionFilter@@YGJPAU_EXCEPTION_POINTERS@@@Z` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3399 | 0x00545080 | `sub_545080` | 有承接 | `CMakeLists.txt:164 — 宿主MSVC exe启动；src/platform/ClientEntry.cpp:263 — WinMain业务边界` | 原55317C C初始化槽安装54503A并保存旧filter；当前MSVC exe pre_cpp初始化持有默认C++异常filter，不再次安装旧入口；完整业务／运行未验 |
| 3400 | 0x00545091 | `sub_545091` | 有承接 | `CMakeLists.txt:164 — 宿主MSVC生命周期；src/platform/ClientEntry.cpp:263 — WinMain业务边界` | 原553198终止槽恢复保存filter；当前宿主拥有完整filter寿命，旧手工保存／恢复沿MC01-C3退休；既有WinMain报告临时层不冒充本叶 |
| 3401 | 0x0054509E | `?_ValidateRead@@YAHPBXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3402 | 0x005450BA | `?_ValidateWrite@@YAHPAXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3403 | 0x005450D6 | `?_ValidateExecute@@YAHP6GHXZ@Z` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3404 | 0x005450F0 | `_strcmp` | 有承接 | `include/cg/platform/LegacyTime.hpp:8 — std::strcmp` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3405 | 0x00545174 | `_abort` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3406 | 0x0054518B | `_getenv` | 有承接 | `include/cg/platform/LegacyTime.hpp:7 — std::getenv` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；无项目消费者，仅真实CRT导入；完整业务／运行未验 |
| 3407 | 0x00545208 | `___crtGetStringTypeA` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3408 | 0x00545360 | `unknown_libname_51` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3409 | 0x00545477 | `unknown_libname_52` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3410 | 0x00545916 | `unknown_libname_53` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3411 | 0x00545929 | `unknown_libname_54` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3412 | 0x00545A65 | `sub_545A65` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3413 | 0x00545B3C | `unknown_libname_63` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3414 | 0x00545B51 | `unknown_libname_64` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3415 | 0x00545B66 | `unknown_libname_65` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3416 | 0x00545D6C | `unknown_libname_66` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3417 | 0x00545E1E | `unknown_libname_67` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3418 | 0x00546024 | `unknown_libname_68` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3419 | 0x005460EB | `__87except` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3420 | 0x005461BB | `__set_exp` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3421 | 0x005461E4 | `__sptype` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3422 | 0x0054623E | `__decomp` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3423 | 0x005462FF | `__frnd` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3424 | 0x00546311 | `__fpclass` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3425 | 0x005463A3 | `__setmode` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3426 | 0x00546419 | `___strgtold12` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3427 | 0x005468EA | `__setmbcp` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3428 | 0x00546A83 | `_getSystemCP` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3429 | 0x00546ACD | `_CPtoLCID` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3430 | 0x00546B00 | `_setSBCS` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3431 | 0x00546B29 | `_setSBUpLow` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3432 | 0x00546CAE | `___initmbctable` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3433 | 0x00546CCA | `__ismbblead` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3434 | 0x00546CDB | `_x_ismbbtype` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3435 | 0x00546D0C | `___crtMessageBoxA` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3436 | 0x00546D95 | `___addl` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3437 | 0x00546DB6 | `___add_12` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3438 | 0x00546E14 | `___shl_12` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3439 | 0x00546E42 | `___shr_12` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3440 | 0x00546E6F | `___mtold12` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3441 | 0x00546F36 | `_$I10_OUTPUT` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3442 | 0x005471C9 | `_raise` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3443 | 0x005472F6 | `_siglookup` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3444 | 0x0054733B | `__mbsnbicoll` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3445 | 0x0054737A | `___wtomb_environ` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3446 | 0x005473E8 | `__raise_exc` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3447 | 0x0054769B | `__handle_exc` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3448 | 0x005478B2 | `__set_errno` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入；include/cg/platform/LegacyTime.hpp:17 — LegacyErrno_1085FE0` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3449 | 0x005478D8 | `sub_5478D8` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor的std::pow；CMakeLists.txt:164 — 宿主默认数学错误服务` | 原唯一5461A4在87except传错误记录，零返回后更新errno；当前实际标准数学服务及宿主默认_matherr承接，不保旧记录／硬件状态或新增零返回包装 |
| 3450 | 0x005478DB | `__statfp` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3451 | 0x005478E9 | `__clrfp` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3452 | 0x005478F8 | `__ctrlfp` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3453 | 0x0054791B | `__set_statfp` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3454 | 0x00547971 | `___ld12mul` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3455 | 0x00547B91 | `___multtenpow12` | 有承接 | `src/platform/LegacyFormat.hpp:5 — MSVC 直接声明；src/platform/LegacyFormat.hpp:5 — MSVC 直接声明` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3456 | 0x00547C0D | `___crtCompareStringA` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3457 | 0x00547E8A | `___crtsetenv` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3458 | 0x00548011 | `_findenv` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3459 | 0x00548069 | `_copy_environ` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3460 | 0x005480D0 | `__mbschr` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3461 | 0x00548143 | `__strdup` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3462 | 0x00548170 | `sub_548170` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3463 | 0x0054817A | `sub_54817A` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3464 | 0x00548193 | `sub_548193` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3465 | 0x0054819D | `sub_54819D` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3466 | 0x005481A7 | `sub_5481A7` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3467 | 0x005481C0 | `sub_5481C0` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3468 | 0x005481CA | `sub_5481CA` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3469 | 0x005481D4 | `sub_5481D4` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3470 | 0x005481EF | `sub_5481EF` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3471 | 0x005481F9 | `sub_5481F9` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3472 | 0x0054820E | `sub_54820E` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3473 | 0x00548218 | `sub_548218` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3474 | 0x00548233 | `sub_548233` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3475 | 0x0054823D | `sub_54823D` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3476 | 0x00548252 | `sub_548252` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:93 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3477 | 0x0054825C | `sub_54825C` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:93 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3478 | 0x00548275 | `sub_548275` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:93 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3479 | 0x0054827F | `sub_54827F` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:93 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3480 | 0x00548289 | `sub_548289` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:93 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3481 | 0x005482A4 | `sub_5482A4` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:93 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3482 | 0x005482AE | `sub_5482AE` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:96 — std::clog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3483 | 0x005482B8 | `sub_5482B8` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:96 — std::clog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3484 | 0x005482D3 | `sub_5482D3` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:96 — std::clog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3485 | 0x005482DD | `??0Init@ios_base@std@@QAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3486 | 0x0054843B | `nullsub_5` | 空方法 | `src/platform/LegacyMouse.cpp:141 — EmptyLegacyExit_54843B` | 已有CPP空方法与原RET记录保留；按用户决定标为空方法，不再核对此入口 |
| 3487 | 0x00548463 | `sub_548463` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3488 | 0x005484CD | `sub_5484CD` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3489 | 0x0054852C | `sub_54852C` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3490 | 0x00548537 | `sub_548537` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3491 | 0x0054859F | `sub_54859F` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3492 | 0x005485FE | `sub_5485FE` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3493 | 0x0054865E | `??0locale@std@@QAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3494 | 0x0054867D | `??1locale@std@@QAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3495 | 0x00548695 | `?_Decref@facet@locale@std@@QAEPAV123@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3496 | 0x005486B1 | `sub_5486B1` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3497 | 0x005486BF | `sub_5486BF` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3498 | 0x005486C5 | `sub_5486C5` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3499 | 0x005486C9 | `?uflow@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3500 | 0x005486E6 | `?xsgetn@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHPADH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3501 | 0x0054875A | `?xsputn@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHPBDH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3502 | 0x005487D0 | `nullsub_8` | 空方法 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 已有标准流／MSVC提供者定位保留；按用户决定标为空方法，不再核对此入口 |
| 3503 | 0x005487D3 | `sub_5487D3` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3504 | 0x005487DE | `sub_5487DE` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3505 | 0x00548843 | `?overflow@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3506 | 0x00548979 | `?pbackfail@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3507 | 0x00548A26 | `?underflow@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3508 | 0x00548A53 | `?uflow@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3509 | 0x00548B68 | `unknown_libname_73` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3510 | 0x00548BEA | `unknown_libname_74` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3511 | 0x00548CAC | `?setbuf@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEPAV?$basic_streambuf@DU?$char_traits@D@std@@@2@PADH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3512 | 0x00548CD7 | `sub_548CD7` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3513 | 0x00548CF6 | `sub_548CF6` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3514 | 0x00548D12 | `sub_548D12` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3515 | 0x00548D31 | `sub_548D31` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3516 | 0x00548D4D | `sub_548D4D` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3517 | 0x00548D69 | `?append@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ID@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3518 | 0x00548DC2 | `?erase@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@II@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3519 | 0x00548E29 | `?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEX_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3520 | 0x00548E65 | `?init@?$basic_ios@DU?$char_traits@D@std@@@std@@IAEXPAV?$basic_streambuf@DU?$char_traits@D@std@@@2@_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3521 | 0x00548EA5 | `?_Gndec@?$basic_streambuf@DU?$char_traits@D@std@@@std@@IAEPADXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3522 | 0x00548EB5 | `?_Gninc@?$basic_streambuf@DU?$char_traits@D@std@@@std@@IAEPADXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3523 | 0x00548EC5 | `unknown_libname_75` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3524 | 0x00548ED5 | `?close@?$basic_filebuf@DU?$char_traits@D@std@@@std@@QAEPAV12@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3525 | 0x00548EFC | `?_Init@?$basic_filebuf@DU?$char_traits@D@std@@@std@@IAEXPAU_iobuf@@W4_Initfl@12@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3526 | 0x00548F74 | `?_Freeze@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3527 | 0x00548FA0 | `?_Grow@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAE_NI_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3528 | 0x00549025 | `?_Split@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3529 | 0x00549055 | `?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@PBDI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3530 | 0x0054908A | `?_Copy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3531 | 0x00549147 | `?_Ungetc@std@@YA_NABDPAU_iobuf@@I@Z` | 有承接 | `src/platform/LegacyStdio.cpp:95 — UnreadLegacyStreamByte_54463F；src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3532 | 0x00549194 | `unknown_libname_76` | 有承接 | `src/platform/LegacyMouse.cpp:161 — RegisterLegacyEmptyExitPair；src/platform/LegacyMouse.cpp:414 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3533 | 0x005491BE | `sub_5491BE` | 有承接 | `src/platform/LegacyMouse.cpp:146 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3534 | 0x005491D6 | `sub_5491D6` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3535 | 0x005491E0 | `sub_5491E0` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3536 | 0x005491F6 | `sub_5491F6` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:92 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3537 | 0x00549200 | `sub_549200` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3538 | 0x0054920A | `sub_54920A` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3539 | 0x00549220 | `sub_549220` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3540 | 0x0054922A | `sub_54922A` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3541 | 0x00549234 | `sub_549234` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3542 | 0x0054924D | `sub_54924D` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3543 | 0x00549257 | `sub_549257` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3544 | 0x00549261 | `sub_549261` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3545 | 0x0054927A | `sub_54927A` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3546 | 0x00549284 | `sub_549284` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:114 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3547 | 0x0054928E | `sub_54928E` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:114 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3548 | 0x005492A7 | `sub_5492A7` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:114 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3549 | 0x005492B1 | `sub_5492B1` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3550 | 0x005492BB | `sub_5492BB` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3551 | 0x005492D6 | `sub_5492D6` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3552 | 0x005492E0 | `sub_5492E0` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3553 | 0x005492F5 | `sub_5492F5` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3554 | 0x005492FF | `sub_5492FF` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3555 | 0x0054931A | `sub_54931A` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:113 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3556 | 0x00549324 | `sub_549324` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3557 | 0x00549339 | `sub_549339` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:114 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3558 | 0x00549343 | `sub_549343` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:114 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3559 | 0x0054935E | `sub_54935E` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:114 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3560 | 0x00549368 | `sub_549368` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:117 — std::wclog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3561 | 0x00549372 | `sub_549372` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:117 — std::wclog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3562 | 0x0054938D | `sub_54938D` | 有承接 | `src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:117 — std::wclog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3563 | 0x00549397 | `??0_Winit@std@@QAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3564 | 0x00549525 | `sub_549525` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3565 | 0x0054958F | `sub_54958F` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3566 | 0x005495EE | `unknown_libname_77` | 有承接 | `src/platform/LegacyMouse.cpp:81 — FlushLegacyStandardStream` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3567 | 0x00549638 | `sub_549638` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3568 | 0x00549643 | `sub_549643` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3569 | 0x005496AB | `sub_5496AB` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3570 | 0x0054970A | `sub_54970A` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3571 | 0x0054976A | `sub_54976A` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3572 | 0x00549778 | `sub_549778` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3573 | 0x0054977F | `sub_54977F` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3574 | 0x00549782 | `sub_549782` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3575 | 0x00549787 | `?uflow@?$basic_streambuf@_WU?$char_traits@_W@std@@@std@@MAEGXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3576 | 0x005497A8 | `?xsgetn@?$basic_streambuf@GU?$char_traits@G@std@@@std@@MAEHPAGH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3577 | 0x0054982F | `?xsputn@?$basic_streambuf@GU?$char_traits@G@std@@@std@@MAEHPBGH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3578 | 0x005498B7 | `unknown_libname_78` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3579 | 0x005498EE | `unknown_libname_79` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3580 | 0x00549925 | `sub_549925` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3581 | 0x0054992A | `sub_54992A` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3582 | 0x00549935 | `sub_549935` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3583 | 0x0054999A | `?overflow@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGG@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3584 | 0x00549AE2 | `?pbackfail@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGG@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3585 | 0x00549B99 | `?underflow@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3586 | 0x00549BC7 | `?uflow@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3587 | 0x00549CE5 | `?setbuf@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEPAV?$basic_streambuf@GU?$char_traits@G@std@@@2@PAGH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3588 | 0x00549D13 | `unknown_libname_80` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3589 | 0x00549D2C | `sub_549D2C` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3590 | 0x00549D4B | `sub_549D4B` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3591 | 0x00549D67 | `sub_549D67` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3592 | 0x00549D86 | `sub_549D86` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3593 | 0x00549DA2 | `sub_549DA2` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3594 | 0x00549DBE | `?init@?$basic_ios@GU?$char_traits@G@std@@@std@@IAEXPAV?$basic_streambuf@GU?$char_traits@G@std@@@2@_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3595 | 0x00549E08 | `?gbump@?$basic_streambuf@GU?$char_traits@G@std@@@std@@IAEXH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3596 | 0x00549E1C | `?_Gndec@?$basic_streambuf@GU?$char_traits@G@std@@@std@@IAEPAGXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3597 | 0x00549E2D | `?_Gninc@?$basic_streambuf@GU?$char_traits@G@std@@@std@@IAEPAGXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3598 | 0x00549E3D | `unknown_libname_81` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3599 | 0x00549E51 | `unknown_libname_82` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3600 | 0x00549E61 | `?close@?$basic_filebuf@GU?$char_traits@G@std@@@std@@QAEPAV12@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3601 | 0x00549E88 | `?_Init@?$basic_filebuf@GU?$char_traits@G@std@@@std@@IAEXPAU_iobuf@@W4_Initfl@12@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3602 | 0x00549EDE | `unknown_libname_83` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3603 | 0x00549F20 | `unknown_libname_84` | 有承接 | `src/platform/LegacyMouse.cpp:161 — RegisterLegacyEmptyExitPair；src/platform/LegacyMouse.cpp:414 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3604 | 0x00549F4A | `sub_549F4A` | 有承接 | `src/platform/LegacyMouse.cpp:146 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3605 | 0x00549F62 | `?clear@ios_base@std@@QAEXH_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3606 | 0x00549FFE | `sub_549FFE` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3607 | 0x0054A062 | `sub_54A062` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3608 | 0x0054A06F | `sub_54A06F` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3609 | 0x0054A08C | `unknown_libname_85` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3610 | 0x0054A0C9 | `sub_54A0C9` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3611 | 0x0054A0E5 | `sub_54A0E5` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3612 | 0x0054A142 | `unknown_libname_86` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3613 | 0x0054A17F | `sub_54A17F` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3614 | 0x0054A19C | `sub_54A19C` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3615 | 0x0054A1B8 | `sub_54A1B8` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3616 | 0x0054A1D0 | `??1ios_base@std@@UAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3617 | 0x0054A220 | `??_Gios_base@std@@UAEPAXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3618 | 0x0054A23C | `?_Callfns@ios_base@std@@AAEXW4event@12@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3619 | 0x0054A25E | `?_Addstd@ios_base@std@@IAEXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3620 | 0x0054A293 | `?_Init@ios_base@std@@IAEXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3621 | 0x0054A2CB | `?_Tidy@ios_base@std@@AAEXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3622 | 0x0054A315 | `?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ABV12@II@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3623 | 0x0054A3E3 | `sub_54A3E3` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3624 | 0x0054A3ED | `nullsub_9` | 空方法 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 已有标准流／MSVC提供者定位保留；按用户决定标为空方法，不再核对此入口 |
| 3625 | 0x0054A3FA | `sub_54A3FA` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3626 | 0x0054A404 | `?_Init@locale@std@@CAPAV_Locimp@12@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3627 | 0x0054A4BD | `sub_54A4BD` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3628 | 0x0054A505 | `??0_Locimp@locale@std@@AAE@_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3629 | 0x0054A57E | `unknown_libname_87` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3630 | 0x0054A59B | `??_G_Locimp@locale@std@@UAEPAXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3631 | 0x0054A5B7 | `??1_Locimp@locale@std@@UAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3632 | 0x0054A638 | `unknown_libname_88` | 有承接 | `src/platform/LegacyMouse.cpp:161 — RegisterLegacyEmptyExitPair；src/platform/LegacyMouse.cpp:414 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3633 | 0x0054A662 | `sub_54A662` | 有承接 | `src/platform/LegacyMouse.cpp:146 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3634 | 0x0054A67A | `?_Xlen@std@@YAXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3635 | 0x0054A6D4 | `sub_54A6D4` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3636 | 0x0054A738 | `sub_54A738` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3637 | 0x0054A755 | `unknown_libname_89` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3638 | 0x0054A792 | `sub_54A792` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3639 | 0x0054A7AE | `sub_54A7AE` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3640 | 0x0054A80B | `unknown_libname_90` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3641 | 0x0054A848 | `sub_54A848` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3642 | 0x0054A865 | `sub_54A865` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3643 | 0x0054A881 | `sub_54A881` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3644 | 0x0054A899 | `?_Xran@std@@YAXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3645 | 0x0054A8F3 | `unknown_libname_91` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3646 | 0x0054A930 | `sub_54A930` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3647 | 0x0054A94D | `sub_54A94D` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3648 | 0x0054A969 | `sub_54A969` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3649 | 0x0054A981 | `unknown_libname_92` | 有承接 | `src/platform/LegacyMouse.cpp:161 — RegisterLegacyEmptyExitPair；src/platform/LegacyMouse.cpp:414 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3650 | 0x0054A9AB | `sub_54A9AB` | 有承接 | `src/platform/LegacyMouse.cpp:146 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3651 | 0x0054A9C3 | `__Mbrtowc` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3652 | 0x0054AAD5 | `_btowc` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain；include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3653 | 0x0054AB16 | `GetAdaptersAddresses` | 有承接 | `src/platform/LegacyNetworkAdapter.cpp:20 — GetAdaptersAddresses；src/platform/LegacyNetworkAdapter.cpp:16 — Sub_534EF0` | 原 GetAdaptersAddresses import 跳板；当前实际 Windows IP Helper API 调用存在，保留原适配器消费边界；完整网络／运行未验 |
| 3654 | 0x0054AB1C | `RtlUnwind` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3655 | 0x0054AB30 | `__strnicmp` | 有承接 | `src/platform/MainLoopSupport.cpp:223 — ::_strnicmp` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3656 | 0x0054ABDD | `__itoa` | 有承接 | `src/world/RenderInput.cpp:132 — std::to_wstring` | DCS01直接MSVC／标准库提供者，原纯转发包装退休；原字节域继续直接CRT，界面调用沿UTL02宽字符适配；完整业务／运行未验 |
| 3657 | 0x0054AC0A | `_xtoa` | 有承接 | `include/cg/platform/LegacyTime.hpp:6 — MSVC CRT标准库／当前线程状态直接导入` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3658 | 0x0054AC66 | `??1type_info@@UAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3659 | 0x0054AC7B | `??_Gtype_info@@UAEPAXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3660 | 0x0054AC98 | `__EH_prolog` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3661 | 0x0054ACB7 | `_fgetpos` | 有承接 | `src/platform/LegacyStdio.cpp:115 — GetLegacyStreamPosition_54ACB7` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3662 | 0x0054ACD9 | `_fsetpos` | 有承接 | `src/platform/LegacyStdio.cpp:129 — SetLegacyStreamPosition_54ACD9` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3663 | 0x0054ACF1 | `_setvbuf` | 有承接 | `src/platform/LegacyStdio.cpp:88 — SetLegacyStreamBuffer_54ACF1` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3664 | 0x0054AD92 | `_fputwc` | 有承接 | `src/platform/LegacyStdio.cpp:139 — WriteLegacyStreamWide_54AD92` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3665 | 0x0054AE87 | `_ungetwc` | 有承接 | `src/platform/LegacyStdio.cpp:152 — UnreadLegacyStreamWide_54AE87` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3666 | 0x0054AF70 | `_fgetwc` | 有承接 | `src/platform/LegacyStdio.cpp:146 — ReadLegacyStreamWide_54AF70` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3667 | 0x0054B05D | `sub_54B05D` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3668 | 0x0054B079 | `??0exception@@QAE@ABQBD@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3669 | 0x0054B0B6 | `??0exception@@QAE@ABV0@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3670 | 0x0054B100 | `sub_54B100` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3671 | 0x0054B116 | `?what@exception@@UBEPBDXZ` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3672 | 0x0054B123 | `__CxxThrowException@8` | 有承接 | `src/platform/LegacyMouse.cpp:90 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:111 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3673 | 0x0054B160 | `_memcmp` | 有承接 | `src/platform/LegacyMemory.cpp:49 — CompareLegacyMemory_54B160` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3674 | 0x0054B1AC | `retnull_0` | 有承接 | `src/platform/LegacyMemory.cpp:49 — CompareLegacyMemory_54B160；include/cg/platform/LegacyMemory.hpp:21 — CompareLegacyMemory_54B160` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3675 | 0x0054B20C | `___termcon` | 有承接 | `src/platform/ClientEntry.cpp:263 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3676 | 0x0054B239 | `__ftelli64` | 有承接 | `src/platform/LegacyStdio.cpp:102 — TellLegacyStream64_54B239` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3677 | 0x0054B3C0 | `__fseeki64` | 有承接 | `src/platform/LegacyStdio.cpp:108 — SeekLegacyStream64_54B3C0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3678 | 0x0054B450 | `__flswbuf` | 有承接 | `src/platform/LegacyStdio.cpp:139 — WriteLegacyStreamWide_54AD92` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3679 | 0x0054B574 | `__filwbuf` | 有承接 | `src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3680 | 0x0054B658 | `__lseeki64` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
