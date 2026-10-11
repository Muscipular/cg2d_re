# IDA 函数全量实现表

> 2026-10-12当前源码核对版。当前 cg2d.exe 已打开 IDA 会话共有 3680 个唯一函数入口；本表按地址升序逐行保留当前名称，名称只作 Markdown 显示转义。状态依据当前 CPP/HPP 定义、明确的共享／内联操作、实际宿主／SDK 服务及对应原入口事实；未继承旧台账的实施状态。
> 本表是后续判断函数实施状态和选择补码入口的唯一依据。旧资料只用于候选符号定位；正文存在、承接、接线／数据缺项和完整验收分开。状态口径沿[本阶段 SPEC](../doc/cg2d_ida_function_implementation_table_spec.md)。

| 状态 | 实际入口数 |
|---|---:|
| 有正文 | 2911 |
| 有承接 | 648 |
| 部分实现 | 5 |
| 未实现 | 0 |
| 待确认 | 2 |
| 已排除 | 113 |
| 不适用 | 1 |

静态验证：3680 行的入口与当前 IDA 地址集合、当前名称逐项一致；序号连续、入口唯一有序、状态合法，3966 处当前源码提供者引用的路径、行号和符号／操作已核可读；UTF-8 无 BOM／LF。此验证不替代行为差异、接线或运行验收。

所有 3680 行均已标记。待确认表示当前对应关系或具体承接证据不足，既不猜为未实现，也不算已有实现；未实现数为零只表示本轮尚未确认缺正文入口，不表示不存在开发缺口。部分实现行已明确当前源码的数据定义／生产缺项。

提供者采用本线相对路径、实际符号／操作与定位行号。库内叶的“有承接”仅指当前真实 MSVC／STL／SDL／SDK 服务或已证内联操作，不声称复制旧私有算法或机器 ABI。现有正文只确认存在与非占位，未例行重做完整差异核对；所有完整业务、owner、接线及运行门禁仍按各自合同验收。未见调用仅为当前 CPP/HPP 定位事实，不声称原入口不可达。

| 序号 | IDA 地址 | IDA 函数名 | 是否有实现 | 当前提供者 | 接线／缺项 |
|---:|---|---|---|---|---|
| 1 | 0x00401000 | `sub_401000` | 有正文 | `src/Blacklist.cpp:143 — PersistAndReset_401000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2 | 0x00401030 | `sub_401030` | 有正文 | `src/Blacklist.cpp:83 — LoadBlacklistFile` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 3 | 0x00401170 | `sub_401170` | 有正文 | `src/Blacklist.cpp:76 — BlacklistRecordAt_401170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 4 | 0x00401190 | `sub_401190` | 有正文 | `src/Blacklist.cpp:155 — GetBlacklistCount_401190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 5 | 0x004011A0 | `sub_4011A0` | 有正文 | `src/Blacklist.cpp:41 — PersistBlacklistFile_4011A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 6 | 0x00401270 | `sub_401270` | 有正文 | `src/Blacklist.cpp:180 — AddBlacklistRecord_401270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 7 | 0x004012F0 | `sub_4012F0` | 有正文 | `src/Blacklist.cpp:221 — RemoveBlacklistRecord_4012F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 8 | 0x004013D0 | `sub_4013D0` | 有正文 | `src/Blacklist.cpp:161 — ContainsBlacklistRecord_4013D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 9 | 0x00401470 | `sub_401470` | 有正文 | `src/Blacklist.cpp:270 — ClearBlacklistRecords_401470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 10 | 0x004014B0 | `sub_4014B0` | 有正文 | `src/LegacyUiRecord.cpp:15102 — Sub_4014B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 11 | 0x00401960 | `sub_401960` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 12 | 0x00401AB0 | `sub_401AB0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 13 | 0x00401C10 | `sub_401C10` | 有正文 | `src/platform/LegacyCryptInt.cpp:33 — InitializeLegacyStartupValues_401C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 14 | 0x00401C20 | `unknown_libname_1` | 有正文 | `src/platform/LegacyCryptInt.cpp:40 — ConstructStartupValues_401C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 15 | 0x00401C50 | `unknown_libname_2` | 有正文 | `src/platform/LegacyCryptInt.cpp:47 — DestroyStartupValues_401C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 16 | 0x00401C70 | `sub_401C70` | 有正文 | `src/platform/LegacyCryptInt.cpp:54 — ConstructZero_401C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 17 | 0x00401CA0 | `sub_401CA0` | 有正文 | `src/platform/LegacyCryptInt.cpp:258 — DestroyAndFree_401CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 18 | 0x00401CC0 | `CryptInt_Init` | 有正文 | `src/platform/LegacyCryptInt.cpp:64 — ConstructFromInt_401CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 19 | 0x00401CF0 | `CryptInt_delete` | 有正文 | `src/platform/LegacyCryptInt.cpp:76 — Destroy_401CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 20 | 0x00401D00 | `CryptInt::eq` | 有正文 | `src/platform/LegacyCryptInt.cpp:172 — Equal_401D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 21 | 0x00401D10 | `sub_401D10` | 有承接 | `src/YobiRuntime.cpp:34406 — Read_401EC0(&action->mapY1F0) != floor` | != 比较内联；共享 Read_401EC0，不重建包装；完整业务／运行未验 |
| 22 | 0x00401D20 | `CryptInt::neq` | 有正文 | `src/platform/LegacyCryptInt.cpp:178 — NotEqual_401D20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 23 | 0x00401D40 | `sub_401D40` | 有正文 | `src/platform/LegacyCryptInt.cpp:185 — Greater_401D40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 24 | 0x00401D50 | `sub_401D50` | 有正文 | `src/platform/LegacyCryptInt.cpp:191 — GreaterCrypt_401D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 25 | 0x00401D70 | `CryptInt::gte` | 有正文 | `src/platform/LegacyCryptInt.cpp:198 — GreaterEqual_401D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 26 | 0x00401D80 | `sub_401D80` | 有正文 | `src/platform/LegacyCryptInt.cpp:204 — GreaterEqualCrypt_401D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 27 | 0x00401DA0 | `CryptInt::Lte` | 有正文 | `src/platform/LegacyCryptInt.cpp:211 — LessEqual_401DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 28 | 0x00401DB0 | `CryptInt::lt` | 有正文 | `src/platform/LegacyCryptInt.cpp:217 — Less_401DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 29 | 0x00401DC0 | `sub_401DC0` | 有正文 | `src/platform/LegacyCryptInt.cpp:223 — LessCrypt_401DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 30 | 0x00401DE0 | `sub_401DE0` | 有正文 | `src/platform/LegacyCryptInt.cpp:122 — Increment_401DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 31 | 0x00401E00 | `sub_401E00` | 有承接 | `src/YobiRuntime.cpp:3404 — Sub_446B50` | S16 加值在实际坐标计算中内联；共享 Read_401EC0；完整业务／运行未验 |
| 32 | 0x00401E10 | `CryptInt::add` | 有正文 | `src/platform/LegacyCryptInt.cpp:131 — Add_401E10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 33 | 0x00401E20 | `sub_401E20` | 有正文 | `src/platform/LegacyCryptInt.cpp:137 — AddAssign_401E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 34 | 0x00401E40 | `sub_401E40` | 有正文 | `src/platform/LegacyCryptInt.cpp:144 — SubtractAssign_401E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 35 | 0x00401E60 | `CryptInt::dec` | 有正文 | `src/platform/LegacyCryptInt.cpp:151 — Subtract_401E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 36 | 0x00401E70 | `CryptInt::dec_0` | 有正文 | `src/platform/LegacyCryptInt.cpp:158 — Difference_401E70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 37 | 0x00401E90 | `CryptInt::mul` | 有正文 | `src/platform/LegacyCryptInt.cpp:165 — Multiply_401E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 38 | 0x00401EA0 | `CryptInt::GetVal` | 有承接 | `src/platform/LegacyCryptInt.cpp:104 — Read_401EC0` | 原入口为跳转到 401EC0；共用同一读取正文；完整业务／运行未验 |
| 39 | 0x00401EB0 | `sub_401EB0` | 有承接 | `src/YobiRuntime.cpp:5379 — Sub_4424B0` | 原读取后 fild 转换；当前实际读取后转 float／double；完整业务／运行未验 |
| 40 | 0x00401EC0 | `sub_401EC0` | 有正文 | `src/platform/LegacyCryptInt.cpp:104 — Read_401EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 41 | 0x00401F50 | `CryptInt::SetNorVal` | 有正文 | `src/platform/LegacyCryptInt.cpp:82 — SetNorVal_401F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 42 | 0x00401F60 | `sub_401F60` | 有承接 | `src/YobiRuntime.cpp:34406 — Read_401EC0(&action->mapY1F0) != floor` | 标量与读取结果 != 在消费者中内联；完整业务／运行未验 |
| 43 | 0x00401F80 | `CryptInt::gt` | 有正文 | `src/platform/LegacyCryptInt.cpp:230 — IntGreaterCrypt_401F80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 44 | 0x00401FA0 | `sub_401FA0` | 有正文 | `src/platform/LegacyCryptInt.cpp:236 — IntLessCryptRaw_401FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 45 | 0x00401FC0 | `CrypyInt::lte` | 有正文 | `src/platform/LegacyCryptInt.cpp:245 — IntLessEqualCrypt_401FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 46 | 0x00401FE0 | `CryptInt::Dec2` | 有正文 | `src/platform/LegacyCryptInt.cpp:251 — IntMinusCrypt_401FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 47 | 0x00402000 | `sub_402000` | 有承接 | `src/YobiRuntime.cpp:37002 — Equal_401D00(&action->mapX200, x)` | 原标量 == 读取值；当前共享 Equal_401D00；完整业务／运行未验 |
| 48 | 0x00402020 | `sub_402020` | 有正文 | `src/LegacyUiRecord.cpp:1826 — ReloadLegacyLog_402020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 49 | 0x004021A0 | `sub_4021A0` | 有正文 | `src/LegacyUiRecord.cpp:1878 — Sub_4021A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 50 | 0x00402B30 | `sub_402B30` | 有正文 | `src/LegacyUiRecord.cpp:15031 — Sub_402B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 51 | 0x00402B70 | `sub_402B70` | 有正文 | `src/platform/LegacyModuleDigest.cpp:69 — Initialize` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 52 | 0x00402BA0 | `sub_402BA0` | 有承接 | `src/platform/LegacyModuleDigest.cpp:191 — StoreLittleEndian32(context.state[index], digest.data() + index * 4)` | 原四 W32 的小端输出在当前 Finalize 的四次循环中承接，共用实际 StoreLittleEndian32；不以单次四字节 helper 冒充原整根正文；完整摘要链／运行未验 |
| 53 | 0x00402BC0 | `sub_402BC0` | 有正文 | `src/platform/LegacyModuleDigest.cpp:173 — Finalize` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 54 | 0x00402C50 | `sub_402C50` | 有正文 | `src/platform/LegacyModuleDigest.cpp:129 — Update` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 55 | 0x00402D30 | `sub_402D30` | 有正文 | `src/platform/LegacyModuleDigest.cpp:84 — Transform` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 56 | 0x00403660 | `sub_403660` | 有正文 | `src/platform/LegacyModuleDigest.cpp:196 — ComputeFileDigest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 57 | 0x00403750 | `sub_403750` | 有正文 | `src/platform/LegacyModuleDigest.cpp:237 — EncodeLowerHex` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 58 | 0x004037D0 | `sub_4037D0` | 有正文 | `src/network/NetworkRuntime.cpp:1140 — ResetProtocolState_4037D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 59 | 0x004038A0 | `sub_4038A0` | 有正文 | `src/network/NetworkRuntime.cpp:422 — Sub_4038A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 60 | 0x00403A70 | `sub_403A70` | 有正文 | `src/network/NetworkRuntime.cpp:505 — Sub_403A70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 61 | 0x00403C50 | `sub_403C50` | 有正文 | `src/network/NetworkRuntime.cpp:217 — Sub_403C50` | 保留旧W16帧正文，当前CPP无调用；活动LF接收由ReceivePlaintextProtocolLine另接，旧帧状态不作为当前接线；当前窗口报告采样／清零另在QueuePlaintextProtocolBytes承接 |
| 62 | 0x00403E50 | `sub_403E50` | 有正文 | `src/network/NetworkRuntime.cpp:302 — Sub_403E50` | 保留旧W16帧正文，当前CPP无调用；活动明文收发由ReceivePlaintextProtocolLine／QueuePlaintextProtocolBytes另接，窗口报告状态采样／清零已接回活动发送路径；不等于原帧整根或网络验收通过 |
| 63 | 0x00404060 | `sub_404060` | 有正文 | `src/Input.cpp:583 — IsRejectedInputText_404060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 64 | 0x00404120 | `sub_404120` | 有正文 | `src/platform/MainLoopSupport.cpp:245 — InitializeLegacyProtectionChecksum1_404120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 65 | 0x00404130 | `unknown_libname_3` | 有正文 | `src/platform/MainLoopSupport.cpp:252 — ConstructProtectionChecksum1_404130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 66 | 0x00404150 | `unknown_libname_4` | 有正文 | `src/platform/MainLoopSupport.cpp:258 — DestroyProtectionChecksum1_404150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 67 | 0x00404160 | `sub_404160` | 有正文 | `src/platform/MainLoopSupport.cpp:264 — InitializeLegacyProtectionChecksum2_404160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 68 | 0x00404170 | `unknown_libname_5` | 有正文 | `src/platform/MainLoopSupport.cpp:271 — ConstructProtectionChecksum2_404170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 69 | 0x00404190 | `unknown_libname_6` | 有正文 | `src/platform/MainLoopSupport.cpp:277 — DestroyProtectionChecksum2_404190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 70 | 0x004041A0 | `sub_4041A0` | 有正文 | `src/platform/MainLoopSupport.cpp:283 — InitializeLegacyProtectionProcessState_4041A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 71 | 0x004041B0 | `unknown_libname_7` | 有正文 | `src/platform/MainLoopSupport.cpp:290 — ConstructProtectionProcessState_4041B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 72 | 0x004041D0 | `unknown_libname_8` | 有正文 | `src/platform/MainLoopSupport.cpp:296 — DestroyProtectionProcessState_4041D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 73 | 0x004041E0 | `sub_4041E0` | 有正文 | `src/platform/MainLoopSupport.cpp:302 — InitializeProtection_4041E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 74 | 0x004042B0 | `sub_4042B0` | 有正文 | `src/platform/MainLoopSupport.cpp:548 — ReleaseProtection_4042B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 75 | 0x004042D0 | `sub_4042D0` | 有正文 | `src/platform/MainLoopSupport.cpp:134 — CheckProcessModules_4042D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 76 | 0x004043B0 | `sub_4043B0` | 有正文 | `src/platform/MainLoopSupport.cpp:337 — CheckProtection_4043B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 77 | 0x00404600 | `sub_404600` | 有正文 | `src/platform/MainLoopSupport.cpp:454 — ParseProtectionTables_404600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 78 | 0x00404880 | `sub_404880` | 有正文 | `src/platform/MainLoopSupport.cpp:177 — CheckDebugger_404880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 79 | 0x004048D0 | `sub_4048D0` | 有正文 | `src/platform/MainLoopSupport.cpp:576 — ResetProtection_4048D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 80 | 0x004048E0 | `sub_4048E0` | 有正文 | `src/platform/MainLoopSupport.cpp:606 — CheckWindows_4048E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 81 | 0x00404940 | `EnumFunc` | 有正文 | `src/platform/MainLoopSupport.cpp:198 — CheckWindowsCallback_404940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 82 | 0x00404A40 | `sub_404A40` | 有正文 | `src/YobiRuntime.cpp:1598 — Sub_404A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 83 | 0x00404AE0 | `YobiAction::new` | 有正文 | `src/YobiRuntime.cpp:1627 — NewYobiAction_404AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 84 | 0x00404C50 | `Yobi::UpdateAll` | 有正文 | `src/YobiRuntime.cpp:6768 — YobiUpdateAll_404C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 85 | 0x00404D00 | `Yobi::MarkDeath` | 有正文 | `src/YobiRuntime.cpp:6807 — MarkDeath` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 86 | 0x00404D10 | `sub_404D10` | 有正文 | `src/YobiRuntime.cpp:1421 — DeleteYobiAction_404D10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 87 | 0x00404D30 | `YobiClearUp` | 有正文 | `src/YobiRuntime.cpp:34281 — YobiClearUp_404D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 88 | 0x00404D60 | `sub_404D60` | 有正文 | `src/YobiRuntime.cpp:34292 — Sub_404D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 89 | 0x00404D90 | `SelectIntValue` | 有正文 | `src/YobiRuntime.cpp:12096 — SelectIntValue_404D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 90 | 0x00404DB0 | `BattleEntry::EffectUpdate` | 有正文 | `src/YobiRuntime.cpp:23566 — UpdateBattleActor_404DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 91 | 0x00405B40 | `GetPetPosition` | 有正文 | `src/world/SceneState.cpp:8686 — GetBattlePetPosition_405B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 92 | 0x00405B60 | `sub_405B60` | 有正文 | `src/YobiRuntime.cpp:26963 — ConsumeBattleFpCost_405B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 93 | 0x00405BD0 | `CompareFunction` | 有正文 | `src/YobiRuntime.cpp:23259 — CompareBattleDrawActions_405BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 94 | 0x00405C30 | `Yobi::sort` | 有正文 | `src/YobiRuntime.cpp:23287 — SortBattleActions_405C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 95 | 0x00405CB0 | `sub_405CB0` | 有正文 | `src/YobiRuntime.cpp:23319 — Sub_405CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 96 | 0x00405E60 | `sub_405E60` | 有正文 | `src/YobiRuntime.cpp:23355 — NormalizeBattleAngle_405E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 97 | 0x00405EA0 | `YobiAction::UpdateXYByRad` | 有正文 | `src/YobiRuntime.cpp:23367 — MoveBattleByAngle_405EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 98 | 0x00405ED0 | `sub_405ED0` | 有正文 | `src/YobiRuntime.cpp:23378 — GetBattleRelativeDirection_405ED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 99 | 0x00406060 | `sub_406060` | 有正文 | `src/YobiRuntime.cpp:23421 — GetBattleAngleDifference_406060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 100 | 0x004060D0 | `sub_4060D0` | 有正文 | `src/YobiRuntime.cpp:23440 — TurnBattleTowardPosition_4060D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 101 | 0x00406190 | `YobiAction::UpdateRad` | 有正文 | `src/YobiRuntime.cpp:23472 — TurnBattleToward_406190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 102 | 0x00406260 | `sub_406260` | 有正文 | `src/YobiRuntime.cpp:29791 — Sub_406260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 103 | 0x004062A0 | `sub_4062A0` | 有正文 | `src/YobiRuntime.cpp:23508 — GetBattleDistanceToPosition_4062A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 104 | 0x00406300 | `sub_406300` | 有正文 | `src/YobiRuntime.cpp:23523 — GetBattleDistance_406300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 105 | 0x00406360 | `sub_406360` | 有正文 | `src/YobiRuntime.cpp:23539 — CountBattleActiveEntries_406360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 106 | 0x00406390 | `sub_406390` | 有正文 | `src/YobiRuntime.cpp:32214 — ApplyBattleEffectFlags_406390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 107 | 0x004065A0 | `sub_4065A0` | 有正文 | `src/YobiRuntime.cpp:23551 — SeedBattleRandom_4065A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 108 | 0x004065E0 | `sub_4065E0` | 有正文 | `src/YobiRuntime.cpp:25249 — ProbeBattleAttackEvent_4065E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 109 | 0x00406630 | `sub_406630` | 有正文 | `src/YobiRuntime.cpp:29782 — UpdateBattleAction236_406630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 110 | 0x00406650 | `sub_406650` | 有正文 | `src/YobiRuntime.cpp:24288 — UpdateBattleAction0_406650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 111 | 0x00406720 | `sub_406720` | 有正文 | `src/YobiRuntime.cpp:24327 — UpdateBattleAction1_406720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 112 | 0x00406760 | `sub_406760` | 有正文 | `src/YobiRuntime.cpp:24343 — UpdateBattleAction2_406760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 113 | 0x004067F0 | `sub_4067F0` | 有正文 | `src/YobiRuntime.cpp:24538 — UpdateBattleAction11_4067F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 114 | 0x00406930 | `sub_406930` | 有正文 | `src/YobiRuntime.cpp:24595 — UpdateBattleAction12_406930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 115 | 0x00406A80 | `sub_406A80` | 有正文 | `src/YobiRuntime.cpp:24639 — UpdateBattleAction13_406A80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 116 | 0x00406BA0 | `sub_406BA0` | 有正文 | `src/YobiRuntime.cpp:26392 — UpdateBattleAction25_406BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 117 | 0x00406C30 | `sub_406C30` | 有正文 | `src/YobiRuntime.cpp:24367 — UpdateBattleAction3_406C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 118 | 0x00406CD0 | `sub_406CD0` | 有正文 | `src/YobiRuntime.cpp:24426 — UpdateBattleAction6_406CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 119 | 0x00406D70 | `sub_406D70` | 有正文 | `src/YobiRuntime.cpp:24387 — UpdateBattleAction4_406D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 120 | 0x00406E10 | `Battle::Boomerange::runEffect` | 有正文 | `src/YobiRuntime.cpp:25270 — UpdateBattleAction15_406E10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 121 | 0x004070E0 | `Battle::SpiracleShot::runEffect` | 有正文 | `src/YobiRuntime.cpp:25435 — UpdateBattleAction70_4070E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 122 | 0x004071C0 | `Battle::BlastWave::runEffect` | 有正文 | `src/YobiRuntime.cpp:25459 — UpdateBattleAction249_4071C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 123 | 0x004072E0 | `sub_4072E0` | 有正文 | `src/YobiRuntime.cpp:25496 — UpdateBattleAction16_4072E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 124 | 0x00408170 | `Battle::BowShot::runEffect` | 有正文 | `src/YobiRuntime.cpp:25354 — UpdateBattleAction18_408170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 125 | 0x004082F0 | `Battle::RandomShot::runEffect` | 有正文 | `src/YobiRuntime.cpp:25396 — UpdateBattleAction19_4082F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 126 | 0x00408410 | `sub_408410` | 有正文 | `src/YobiRuntime.cpp:25922 — UpdateBattleAction20_408410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 127 | 0x00408750 | `sub_408750` | 有正文 | `src/YobiRuntime.cpp:26025 — UpdateBattleAction21_408750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 128 | 0x004087C0 | `sub_4087C0` | 有正文 | `src/YobiRuntime.cpp:26043 — UpdateBattleAction22_4087C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 129 | 0x004090F0 | `sub_4090F0` | 有正文 | `src/YobiRuntime.cpp:26297 — UpdateBattleAction23_4090F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 130 | 0x00409380 | `sub_409380` | 有正文 | `src/YobiRuntime.cpp:26379 — UpdateBattleAction24_409380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 131 | 0x004093B0 | `sub_4093B0` | 有正文 | `src/YobiRuntime.cpp:25911 — UpdateBattleAction17_4093B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 132 | 0x00409410 | `sub_409410` | 有正文 | `src/YobiRuntime.cpp:24448 — UpdateBattleAction7_409410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 133 | 0x004094A0 | `sub_4094A0` | 有正文 | `src/YobiRuntime.cpp:24471 — UpdateBattleAction8_4094A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 134 | 0x00409510 | `sub_409510` | 有正文 | `src/YobiRuntime.cpp:24489 — UpdateBattleAction9_409510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 135 | 0x00409610 | `sub_409610` | 有正文 | `src/YobiRuntime.cpp:24522 — UpdateBattleAction10_409610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 136 | 0x00409680 | `sub_409680` | 有正文 | `src/YobiRuntime.cpp:24682 — UpdateBattleAction14_409680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 137 | 0x0040A9E0 | `sub_40A9E0` | 有正文 | `src/YobiRuntime.cpp:26424 — UpdateBattleAction26_40A9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 138 | 0x0040AA70 | `sub_40AA70` | 有正文 | `src/YobiRuntime.cpp:26444 — UpdateBattleAction27_40AA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 139 | 0x0040ACF0 | `sub_40ACF0` | 有正文 | `src/YobiRuntime.cpp:26530 — UpdateBattleAction28_40ACF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 140 | 0x0040AE50 | `sub_40AE50` | 有正文 | `src/YobiRuntime.cpp:26582 — UpdateBattleAction29_40AE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 141 | 0x0040AF30 | `sub_40AF30` | 有正文 | `src/YobiRuntime.cpp:26613 — UpdateBattleAction30_40AF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 142 | 0x0040B020 | `sub_40B020` | 有正文 | `src/YobiRuntime.cpp:26644 — UpdateBattleAction31_40B020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 143 | 0x0040B130 | `sub_40B130` | 有正文 | `src/YobiRuntime.cpp:27038 — UpdateBattleAction45_40B130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 144 | 0x0040B180 | `sub_40B180` | 有正文 | `src/YobiRuntime.cpp:33757 — UpdateBattleAction251_40B180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 145 | 0x0040B200 | `sub_40B200` | 有正文 | `src/YobiRuntime.cpp:27056 — UpdateBattleAction46_40B200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 146 | 0x0040B3C0 | `sub_40B3C0` | 有正文 | `src/YobiRuntime.cpp:27196 — UpdateBattleAction50_40B3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 147 | 0x0040B3F0 | `sub_40B3F0` | 有正文 | `src/YobiRuntime.cpp:27134 — UpdateBattleAction47_40B3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 148 | 0x0040B410 | `sub_40B410` | 有正文 | `src/YobiRuntime.cpp:26683 — UpdateBattleAction32_40B410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 149 | 0x0040B450 | `sub_40B450` | 有正文 | `src/YobiRuntime.cpp:26694 — UpdateBattleAction33_40B450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 150 | 0x0040B490 | `sub_40B490` | 有正文 | `src/YobiRuntime.cpp:27143 — UpdateBattleAction48_40B490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 151 | 0x0040B5B0 | `sub_40B5B0` | 有正文 | `src/YobiRuntime.cpp:27185 — UpdateBattleAction49_40B5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 152 | 0x0040B5E0 | `sub_40B5E0` | 有正文 | `src/YobiRuntime.cpp:26705 — UpdateBattleAction34_40B5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 153 | 0x0040B930 | `sub_40B930` | 有正文 | `src/YobiRuntime.cpp:26797 — UpdateBattleAction35_40B930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 154 | 0x0040BAD0 | `sub_40BAD0` | 有正文 | `src/YobiRuntime.cpp:26855 — UpdateBattleAction36_40BAD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 155 | 0x0040BB20 | `sub_40BB20` | 有正文 | `src/YobiRuntime.cpp:26871 — UpdateBattleAction37_40BB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 156 | 0x0040BB70 | `sub_40BB70` | 有正文 | `src/YobiRuntime.cpp:26887 — UpdateBattleAction38_40BB70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 157 | 0x0040BBC0 | `sub_40BBC0` | 有正文 | `src/YobiRuntime.cpp:26903 — UpdateBattleAction39_40BBC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 158 | 0x0040BD40 | `sub_40BD40` | 有正文 | `src/YobiRuntime.cpp:26955 — UpdateBattleAction40_40BD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 159 | 0x0040BD60 | `sub_40BD60` | 有正文 | `src/YobiRuntime.cpp:26988 — UpdateBattleAction41_40BD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 160 | 0x0040BDB0 | `sub_40BDB0` | 有正文 | `src/YobiRuntime.cpp:27002 — UpdateBattleAction42_40BDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 161 | 0x0040BDF0 | `sub_40BDF0` | 有正文 | `src/YobiRuntime.cpp:27027 — UpdateBattleAction44_40BDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 162 | 0x0040BE30 | `sub_40BE30` | 有正文 | `src/YobiRuntime.cpp:27014 — UpdateBattleAction43_40BE30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 163 | 0x0040BE70 | `sub_40BE70` | 有正文 | `src/YobiRuntime.cpp:27206 — UpdateBattleAction51_40BE70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 164 | 0x0040BE90 | `sub_40BE90` | 有正文 | `src/YobiRuntime.cpp:27215 — UpdateBattleAction52_40BE90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 165 | 0x0040BF20 | `sub_40BF20` | 有正文 | `src/YobiRuntime.cpp:27238 — UpdateBattleAction53_40BF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 166 | 0x0040BFE0 | `sub_40BFE0` | 有正文 | `src/YobiRuntime.cpp:27269 — UpdateBattleAction54_40BFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 167 | 0x0040C0C0 | `sub_40C0C0` | 有正文 | `src/YobiRuntime.cpp:27302 — UpdateBattleAction55_40C0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 168 | 0x0040C0E0 | `sub_40C0E0` | 有正文 | `src/YobiRuntime.cpp:27516 — UpdateBattleAction68_40C0E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 169 | 0x0040C120 | `sub_40C120` | 有正文 | `src/YobiRuntime.cpp:27528 — UpdateBattleAction69_40C120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 170 | 0x0040C160 | `sub_40C160` | 有正文 | `src/YobiRuntime.cpp:27311 — UpdateBattleAction56_40C160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 171 | 0x0040C1A0 | `sub_40C1A0` | 有正文 | `src/YobiRuntime.cpp:27324 — UpdateBattleAction57_40C1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 172 | 0x0040C1E0 | `sub_40C1E0` | 有正文 | `src/YobiRuntime.cpp:27336 — UpdateBattleAction58_40C1E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 173 | 0x0040C220 | `sub_40C220` | 有正文 | `src/YobiRuntime.cpp:27360 — UpdateBattleAction60_40C220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 174 | 0x0040C240 | `sub_40C240` | 有正文 | `src/YobiRuntime.cpp:27348 — UpdateBattleAction59_40C240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 175 | 0x0040C280 | `sub_40C280` | 有正文 | `src/YobiRuntime.cpp:27368 — UpdateBattleAction61_40C280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 176 | 0x0040C2B0 | `sub_40C2B0` | 有正文 | `src/YobiRuntime.cpp:27378 — UpdateBattleAction62_40C2B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 177 | 0x0040C2E0 | `sub_40C2E0` | 有正文 | `src/YobiRuntime.cpp:27388 — UpdateBattleAction63_40C2E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 178 | 0x0040C380 | `sub_40C380` | 有正文 | `src/YobiRuntime.cpp:27428 — UpdateBattleAction65_40C380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 179 | 0x0040C480 | `sub_40C480` | 有正文 | `src/YobiRuntime.cpp:27478 — UpdateBattleAction66_40C480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 180 | 0x0040C4D0 | `sub_40C4D0` | 有正文 | `src/YobiRuntime.cpp:27540 — UpdateBattleAction71_40C4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 181 | 0x0040C520 | `sub_40C520` | 有正文 | `src/YobiRuntime.cpp:27747 — UpdateBattleAction86_40C520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 182 | 0x0040C780 | `sub_40C780` | 有正文 | `src/YobiRuntime.cpp:27821 — UpdateBattleAction87_40C780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 183 | 0x0040C8D0 | `sub_40C8D0` | 有正文 | `src/YobiRuntime.cpp:27862 — UpdateBattleAction88_40C8D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 184 | 0x0040C8F0 | `sub_40C8F0` | 有正文 | `src/YobiRuntime.cpp:27871 — UpdateBattleAction89_40C8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 185 | 0x0040C910 | `sub_40C910` | 有正文 | `src/YobiRuntime.cpp:27879 — UpdateBattleAction90_40C910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 186 | 0x0040CAA0 | `sub_40CAA0` | 有正文 | `src/YobiRuntime.cpp:27558 — UpdateBattleAction72_40CAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 187 | 0x0040CAC0 | `sub_40CAC0` | 有正文 | `src/YobiRuntime.cpp:33973 — AdvanceBattleLinkedCast_40CAC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 188 | 0x0040CAF0 | `sub_40CAF0` | 有正文 | `src/YobiRuntime.cpp:33983 — AdvanceBattleEffectAndCast_40CAF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 189 | 0x0040CB10 | `sub_40CB10` | 有正文 | `src/YobiRuntime.cpp:27568 — UpdateBattleAction75_40CB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 190 | 0x0040CB30 | `sub_40CB30` | 有正文 | `src/YobiRuntime.cpp:27577 — UpdateBattleAction76_40CB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 191 | 0x0040CB50 | `sub_40CB50` | 有正文 | `src/YobiRuntime.cpp:27586 — UpdateBattleAction77_40CB50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 192 | 0x0040CB80 | `sub_40CB80` | 有正文 | `src/YobiRuntime.cpp:27595 — UpdateBattleAction78_40CB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 193 | 0x0040CBB0 | `sub_40CBB0` | 有正文 | `src/YobiRuntime.cpp:27420 — UpdateBattleAction64_40CBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 194 | 0x0040CBD0 | `sub_40CBD0` | 有正文 | `src/YobiRuntime.cpp:27604 — UpdateBattleAction79_40CBD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 195 | 0x0040CC80 | `sub_40CC80` | 有正文 | `src/YobiRuntime.cpp:27634 — UpdateBattleAction80_40CC80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 196 | 0x0040CCB0 | `sub_40CCB0` | 有正文 | `src/YobiRuntime.cpp:27645 — UpdateBattleAction81_40CCB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 197 | 0x0040CCE0 | `sub_40CCE0` | 有正文 | `src/YobiRuntime.cpp:27656 — UpdateBattleAction82_40CCE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 198 | 0x0040CD10 | `sub_40CD10` | 有正文 | `src/YobiRuntime.cpp:27667 — UpdateBattleAction83_40CD10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 199 | 0x0040CD70 | `sub_40CD70` | 有正文 | `src/YobiRuntime.cpp:27684 — UpdateBattleAction84_40CD70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 200 | 0x0040CDE0 | `sub_40CDE0` | 有正文 | `src/YobiRuntime.cpp:27705 — UpdateBattleAction85_40CDE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 201 | 0x0040CF30 | `sub_40CF30` | 有正文 | `src/YobiRuntime.cpp:27929 — UpdateBattleAction91_40CF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 202 | 0x0040CF50 | `sub_40CF50` | 有正文 | `src/YobiRuntime.cpp:27938 — UpdateBattleAction92_40CF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 203 | 0x0040CF70 | `sub_40CF70` | 有正文 | `src/YobiRuntime.cpp:27947 — UpdateBattleAction93_40CF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 204 | 0x0040CFA0 | `sub_40CFA0` | 有正文 | `src/YobiRuntime.cpp:24407 — UpdateBattleAction5_40CFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 205 | 0x0040D010 | `sub_40D010` | 有正文 | `src/YobiRuntime.cpp:27959 — UpdateBattleAction94_40D010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 206 | 0x0040D0B0 | `sub_40D0B0` | 有正文 | `src/YobiRuntime.cpp:27980 — UpdateBattleAction95_40D0B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 207 | 0x0040D170 | `sub_40D170` | 有正文 | `src/YobiRuntime.cpp:28011 — UpdateBattleAction96_40D170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 208 | 0x0040D1A0 | `sub_40D1A0` | 有正文 | `src/YobiRuntime.cpp:28021 — UpdateBattleAction97_40D1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 209 | 0x0040D1D0 | `sub_40D1D0` | 有正文 | `src/YobiRuntime.cpp:28031 — UpdateBattleAction98_40D1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 210 | 0x0040D390 | `sub_40D390` | 有正文 | `src/YobiRuntime.cpp:28083 — UpdateBattleAction99_40D390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 211 | 0x0040D3C0 | `sub_40D3C0` | 有正文 | `src/YobiRuntime.cpp:28094 — UpdateBattleAction100_40D3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 212 | 0x0040D580 | `sub_40D580` | 有正文 | `src/YobiRuntime.cpp:28148 — UpdateBattleAction101_40D580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 213 | 0x0040D6C0 | `sub_40D6C0` | 有正文 | `src/YobiRuntime.cpp:28188 — UpdateBattleAction102_40D6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 214 | 0x0040DBE0 | `sub_40DBE0` | 有正文 | `src/YobiRuntime.cpp:30698 — UpdateBattleAction103_40DBE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 215 | 0x0040DC90 | `sub_40DC90` | 有正文 | `src/YobiRuntime.cpp:28336 — UpdateBattleAction104_40DC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 216 | 0x0040DE40 | `sub_40DE40` | 有正文 | `src/YobiRuntime.cpp:28385 — UpdateBattleAction105_40DE40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 217 | 0x0040DF50 | `sub_40DF50` | 有正文 | `src/YobiRuntime.cpp:28428 — UpdateBattleAction106_40DF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 218 | 0x0040DF70 | `sub_40DF70` | 有正文 | `src/YobiRuntime.cpp:28436 — UpdateBattleAction107_40DF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 219 | 0x0040DF80 | `sub_40DF80` | 有正文 | `src/YobiRuntime.cpp:28443 — UpdateBattleAction108_40DF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 220 | 0x0040E4B0 | `sub_40E4B0` | 有正文 | `src/YobiRuntime.cpp:30724 — UpdateBattleAction109_40E4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 221 | 0x0040E560 | `sub_40E560` | 有正文 | `src/YobiRuntime.cpp:28593 — UpdateBattleAction110_40E560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 222 | 0x0040E970 | `sub_40E970` | 有正文 | `src/YobiRuntime.cpp:28725 — UpdateBattleAction111_40E970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 223 | 0x0040EB00 | `sub_40EB00` | 有正文 | `src/YobiRuntime.cpp:30751 — UpdateBattleAction112_40EB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 224 | 0x0040EFE0 | `sub_40EFE0` | 有正文 | `src/YobiRuntime.cpp:28777 — UpdateBattleAction113_40EFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 225 | 0x0040F0A0 | `sub_40F0A0` | 有正文 | `src/YobiRuntime.cpp:28811 — UpdateBattleAction114_40F0A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 226 | 0x0040F790 | `sub_40F790` | 有正文 | `src/YobiRuntime.cpp:29496 — UpdateBattleAction154_40F790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 227 | 0x0040F800 | `sub_40F800` | 有正文 | `src/YobiRuntime.cpp:29512 — UpdateBattleAction155_40F800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 228 | 0x0040FB30 | `sub_40FB30` | 有正文 | `src/YobiRuntime.cpp:29621 — UpdateBattleAction156_40FB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 229 | 0x0040FB80 | `sub_40FB80` | 有正文 | `src/YobiRuntime.cpp:28996 — UpdateBattleAction115_40FB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 230 | 0x0040FC10 | `sub_40FC10` | 有正文 | `src/YobiRuntime.cpp:29012 — UpdateBattleFactoryAction_40FC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 231 | 0x0040FE50 | `sub_40FE50` | 有正文 | `src/YobiRuntime.cpp:33781 — UpdateBattleAction253_40FE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 232 | 0x0040FEB0 | `sub_40FEB0` | 有正文 | `src/YobiRuntime.cpp:33794 — UpdateBattleAction254_40FEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 233 | 0x0040FEF0 | `sub_40FEF0` | 有正文 | `src/YobiRuntime.cpp:29483 — UpdateBattleAction148_40FEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 234 | 0x0040FF40 | `sub_40FF40` | 有正文 | `src/YobiRuntime.cpp:29081 — UpdateBattleAction138_40FF40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 235 | 0x0040FF90 | `sub_40FF90` | 有正文 | `src/YobiRuntime.cpp:29063 — UpdateBattleActions127To129_40FF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 236 | 0x00410000 | `sub_410000` | 有正文 | `src/YobiRuntime.cpp:29095 — UpdateBattleAction139_410000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 237 | 0x004106B0 | `sub_4106B0` | 有正文 | `src/YobiRuntime.cpp:27491 — UpdateBattleAction67_4106B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 238 | 0x00410730 | `sub_410730` | 有正文 | `src/YobiRuntime.cpp:29308 — UpdateBattleAction140_410730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 239 | 0x004107C0 | `sub_4107C0` | 有正文 | `src/YobiRuntime.cpp:29753 — UpdateBattleAction159_4107C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 240 | 0x00410B00 | `sub_410B00` | 有正文 | `src/YobiRuntime.cpp:29760 — UpdateBattleAction160_410B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 241 | 0x00410E30 | `sub_410E30` | 有正文 | `src/YobiRuntime.cpp:29767 — UpdateBattleAction162_410E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 242 | 0x00410E90 | `sub_410E90` | 有正文 | `src/YobiRuntime.cpp:29323 — UpdateBattleAction141_410E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 243 | 0x00410F70 | `sub_410F70` | 有正文 | `src/YobiRuntime.cpp:29363 — UpdateBattleAction142_410F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 244 | 0x00410FC0 | `sub_410FC0` | 有正文 | `src/YobiRuntime.cpp:29375 — UpdateBattleAction143_410FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 245 | 0x00411240 | `sub_411240` | 有正文 | `src/YobiRuntime.cpp:29434 — UpdateBattleAction144_411240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 246 | 0x00411290 | `sub_411290` | 有正文 | `src/YobiRuntime.cpp:29447 — UpdateBattleAction145_411290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 247 | 0x00411320 | `sub_411320` | 有正文 | `src/YobiRuntime.cpp:29466 — UpdateBattleAction147_411320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 248 | 0x004113A0 | `sub_4113A0` | 有正文 | `src/YobiRuntime.cpp:29960 — UpdateBattleAction163_4113A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 249 | 0x00411A50 | `sub_411A50` | 有正文 | `src/YobiRuntime.cpp:30168 — UpdateBattleAction164_411A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 250 | 0x00411CB0 | `sub_411CB0` | 有正文 | `src/YobiRuntime.cpp:30328 — UpdateBattleAction166_411CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 251 | 0x00411D30 | `sub_411D30` | 有正文 | `src/YobiRuntime.cpp:30241 — UpdateBattleAction165_411D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 252 | 0x00411FA0 | `sub_411FA0` | 有正文 | `src/YobiRuntime.cpp:30345 — UpdateBattleAction167_411FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 253 | 0x00411FC0 | `sub_411FC0` | 有正文 | `src/YobiRuntime.cpp:30352 — UpdateBattleAction168_411FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 254 | 0x00411FE0 | `sub_411FE0` | 有正文 | `src/YobiRuntime.cpp:30359 — UpdateBattleAction169_411FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 255 | 0x00412000 | `sub_412000` | 有正文 | `src/YobiRuntime.cpp:30404 — UpdateBattleAction170_412000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 256 | 0x00412020 | `sub_412020` | 有正文 | `src/YobiRuntime.cpp:30411 — UpdateBattleAction171_412020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 257 | 0x004120F0 | `sub_4120F0` | 有正文 | `src/YobiRuntime.cpp:30435 — UpdateBattleActions172_173_4120F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 258 | 0x004121D0 | `sub_4121D0` | 有正文 | `src/YobiRuntime.cpp:30460 — UpdateBattleAction174_4121D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 259 | 0x00412210 | `sub_412210` | 有正文 | `src/YobiRuntime.cpp:30472 — UpdateBattleAction175_412210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 260 | 0x00412260 | `sub_412260` | 有正文 | `src/YobiRuntime.cpp:30484 — UpdateBattleAction176_412260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 261 | 0x00412290 | `sub_412290` | 有正文 | `src/YobiRuntime.cpp:30496 — UpdateBattleAction177_412290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 262 | 0x004122C0 | `sub_4122C0` | 有正文 | `src/YobiRuntime.cpp:30505 — UpdateBattleAction178_4122C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 263 | 0x004122E0 | `sub_4122E0` | 有正文 | `src/YobiRuntime.cpp:30512 — UpdateBattleAction179_4122E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 264 | 0x00412310 | `sub_412310` | 有正文 | `src/YobiRuntime.cpp:30521 — UpdateBattleActions180_203_412310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 265 | 0x004124E0 | `sub_4124E0` | 有正文 | `src/YobiRuntime.cpp:30563 — UpdateBattleAction204_4124E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 266 | 0x004127B0 | `sub_4127B0` | 有正文 | `src/YobiRuntime.cpp:30665 — UpdateBattleAction205_4127B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 267 | 0x004127F0 | `sub_4127F0` | 有正文 | `src/YobiRuntime.cpp:30676 — UpdateBattleAction206_4127F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 268 | 0x00412830 | `sub_412830` | 有正文 | `src/YobiRuntime.cpp:30686 — UpdateBattleAction207_412830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 269 | 0x00412870 | `sub_412870` | 有正文 | `src/YobiRuntime.cpp:32266 — SetBattleAction208Target_412870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 270 | 0x00412900 | `sub_412900` | 有正文 | `src/YobiRuntime.cpp:32285 — UpdateBattleAction209_412900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 271 | 0x00412F70 | `sub_412F70` | 有正文 | `src/YobiRuntime.cpp:32503 — UpdateBattleAction210_412F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 272 | 0x00413070 | `sub_413070` | 有正文 | `src/YobiRuntime.cpp:32526 — ApplyBattleSceneEffects_413070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 273 | 0x00413110 | `sub_413110` | 有正文 | `src/YobiRuntime.cpp:32568 — UpdateBattleAction212_413110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 274 | 0x004131B0 | `sub_4131B0` | 有正文 | `src/YobiRuntime.cpp:32594 — UpdateBattleAction213_4131B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 275 | 0x004132B0 | `sub_4132B0` | 有正文 | `src/YobiRuntime.cpp:32640 — UpdateBattleAction214_4132B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 276 | 0x004133D0 | `sub_4133D0` | 有正文 | `src/YobiRuntime.cpp:32681 — UpdateBattleAction215_4133D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 277 | 0x00413400 | `sub_413400` | 有正文 | `src/YobiRuntime.cpp:32691 — UpdateBattleAction216_413400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 278 | 0x00413550 | `sub_413550` | 有正文 | `src/YobiRuntime.cpp:32747 — UpdateBattleAction218_413550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 279 | 0x00413750 | `sub_413750` | 有正文 | `src/YobiRuntime.cpp:32879 — UpdateBattleAction219_413750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 280 | 0x00413980 | `sub_413980` | 有正文 | `src/YobiRuntime.cpp:32904 — UpdateBattleAction220_413980` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 281 | 0x00413B40 | `sub_413B40` | 有正文 | `src/YobiRuntime.cpp:32913 — UpdateBattleAction221_413B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 282 | 0x00413E90 | `sub_413E90` | 有正文 | `src/YobiRuntime.cpp:33020 — UpdateBattleAction222_413E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 283 | 0x00413FE0 | `sub_413FE0` | 有正文 | `src/YobiRuntime.cpp:33058 — UpdateBattleAction223_413FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 284 | 0x00414050 | `sub_414050` | 有正文 | `src/YobiRuntime.cpp:31021 — UpdateBattleAction224_414050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 285 | 0x004150F0 | `sub_4150F0` | 有正文 | `src/YobiRuntime.cpp:31637 — UpdateBattleAction225_4150F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 286 | 0x004154C0 | `sub_4154C0` | 有正文 | `src/YobiRuntime.cpp:31394 — UpdateBattleAction226_4154C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 287 | 0x00415940 | `sub_415940` | 有正文 | `src/YobiRuntime.cpp:31487 — UpdateBattleAction227_415940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 288 | 0x00416000 | `sub_416000` | 有正文 | `src/YobiRuntime.cpp:31756 — UpdateBattleAction228_416000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 289 | 0x00416020 | `sub_416020` | 有正文 | `src/YobiRuntime.cpp:31765 — UpdateBattleAction229_416020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 290 | 0x004160A0 | `sub_4160A0` | 有正文 | `src/YobiRuntime.cpp:31781 — UpdateBattleAction230_4160A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 291 | 0x00416150 | `sub_416150` | 有正文 | `src/YobiRuntime.cpp:31803 — UpdateBattleAction231_416150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 292 | 0x00416220 | `sub_416220` | 有正文 | `src/YobiRuntime.cpp:31834 — UpdateBattleAction232_416220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 293 | 0x00416860 | `sub_416860` | 有正文 | `src/YobiRuntime.cpp:32011 — UpdateBattleAction233_416860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 294 | 0x00416AB0 | `sub_416AB0` | 有正文 | `src/YobiRuntime.cpp:32090 — UpdateBattleAction234_416AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 295 | 0x00416DB0 | `sub_416DB0` | 有正文 | `src/YobiRuntime.cpp:32258 — ApplyBattleCastEffects_416DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 296 | 0x00416DD0 | `sub_416DD0` | 有正文 | `src/YobiRuntime.cpp:33080 — UpdateBattleAction237_416DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 297 | 0x00417410 | `sub_417410` | 有正文 | `src/YobiRuntime.cpp:33267 — UpdateBattleActions238_239_417410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 298 | 0x00417AC0 | `sub_417AC0` | 有正文 | `src/YobiRuntime.cpp:33466 — UpdateBattleAction240_417AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 299 | 0x00417B80 | `sub_417B80` | 有正文 | `src/YobiRuntime.cpp:30884 — UpdateBattleAction241_417B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 300 | 0x00417CC0 | `sub_417CC0` | 有正文 | `src/YobiRuntime.cpp:30925 — UpdateBattleAction242_417CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 301 | 0x00417D80 | `sub_417D80` | 有正文 | `src/YobiRuntime.cpp:33494 — UpdateBattleAction243_417D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 302 | 0x00418550 | `sub_418550` | 有正文 | `src/YobiRuntime.cpp:33694 — UpdateBattleAction244_418550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 303 | 0x00418580 | `sub_418580` | 有正文 | `src/YobiRuntime.cpp:33704 — UpdateBattleAction245_418580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 304 | 0x004185C0 | `sub_4185C0` | 有正文 | `src/YobiRuntime.cpp:33716 — UpdateBattleAction246_4185C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 305 | 0x004185F0 | `sub_4185F0` | 有正文 | `src/YobiRuntime.cpp:33726 — UpdateBattleAction247_4185F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 306 | 0x00418620 | `sub_418620` | 有正文 | `src/YobiRuntime.cpp:33736 — UpdateBattleAction248_418620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 307 | 0x004186B0 | `sub_4186B0` | 有正文 | `src/YobiRuntime.cpp:33807 — UpdateBattleAction255_4186B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 308 | 0x00418B50 | `sub_418B50` | 有正文 | `src/YobiRuntime.cpp:33966 — UpdateBattleAction256_418B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 309 | 0x00418B70 | `Battle::SpiracleShot::newEffect` | 有正文 | `src/YobiRuntime.cpp:9636 — NewBattleSpiracleShot_418B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 310 | 0x00418D80 | `YobiBattleEntry::ProjectionUpdate` | 有正文 | `src/YobiRuntime.cpp:9568 — UpdateBattleProjection_418D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 311 | 0x00418DF0 | `YobiBattleEntry::newProjection` | 有正文 | `src/YobiRuntime.cpp:9589 — NewBattleProjection_418DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 312 | 0x00418F10 | `sub_418F10` | 有正文 | `src/YobiRuntime.cpp:9477 — UpdateBattleArcProjection_418F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 313 | 0x00418FF0 | `sub_418FF0` | 有正文 | `src/YobiRuntime.cpp:9517 — NewBattleArcProjection_418FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 314 | 0x00419180 | `Battle::BlastWave::newEffect` | 有正文 | `src/YobiRuntime.cpp:9707 — NewBattleBlastWave_419180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 315 | 0x00419390 | `Battle::RangeHit::newEffect` | 有正文 | `src/YobiRuntime.cpp:9778 — NewBattleRangeHit_419390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 316 | 0x004195F0 | `Battle::Boomerange::newEffect` | 有正文 | `src/YobiRuntime.cpp:9945 — NewBattleBoomerang_4195F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 317 | 0x00419BE0 | `sub_419BE0` | 有正文 | `src/YobiRuntime.cpp:10848 — UpdateBattleTransitionEffect_419BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 318 | 0x00419C10 | `sub_419C10` | 有正文 | `src/YobiRuntime.cpp:10858 — NewBattleTransitionEffect_419C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 319 | 0x00419CB0 | `sub_419CB0` | 有正文 | `src/YobiRuntime.cpp:10887 — UpdateBattleOffsetFollowEffect_419CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 320 | 0x00419CF0 | `sub_419CF0` | 有正文 | `src/YobiRuntime.cpp:10904 — NewBattleOffsetFollowEffect_419CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 321 | 0x00419DA0 | `sub_419DA0` | 有正文 | `src/YobiRuntime.cpp:9855 — NewBattleArcShot_419DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 322 | 0x00419FA0 | `sub_419FA0` | 有正文 | `src/YobiRuntime.cpp:10268 — UpdateBattleOneShotEffect_419FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 323 | 0x00419FC0 | `sub_419FC0` | 有正文 | `src/YobiRuntime.cpp:10276 — NewBattleRandomEffect_419FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 324 | 0x0041A0D0 | `sub_41A0D0` | 有正文 | `src/YobiRuntime.cpp:10309 — NewBattleScreenEffect_41A0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 325 | 0x0041A160 | `sub_41A160` | 有正文 | `src/YobiRuntime.cpp:7084 — UpdateBattleCompletingEffect_41A160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 326 | 0x0041A1C0 | `sub_41A1C0` | 有正文 | `src/YobiRuntime.cpp:7103 — UpdateBattleParentEffect_41A1C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 327 | 0x0041A200 | `sub_41A200` | 有正文 | `src/YobiRuntime.cpp:7117 — UpdateBattleRisingEffect_41A200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 328 | 0x0041A270 | `sub_41A270` | 有正文 | `src/YobiRuntime.cpp:7141 — UpdateBattleScalingEffect_41A270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 329 | 0x0041A310 | `sub_41A310` | 有正文 | `src/YobiRuntime.cpp:7171 — UpdateBattleRandomEmitter_41A310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 330 | 0x0041A530 | `sub_41A530` | 有正文 | `src/YobiRuntime.cpp:7227 — UpdateBattleFallingEffect_41A530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 331 | 0x0041A590 | `sub_41A590` | 有正文 | `src/YobiRuntime.cpp:7242 — UpdateBattleReverseEmitter_41A590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 332 | 0x0041A7D0 | `sub_41A7D0` | 有正文 | `src/YobiRuntime.cpp:7295 — UpdateBattleIndexedEmitter_41A7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 333 | 0x0041A9E0 | `sub_41A9E0` | 有正文 | `src/YobiRuntime.cpp:7371 — UpdateBattleImpactEffect_41A9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 334 | 0x0041AF00 | `sub_41AF00` | 有正文 | `src/YobiRuntime.cpp:7538 — UpdateBattleImpactSoundEffect_41AF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 335 | 0x0041B440 | `sub_41B440` | 有正文 | `src/YobiRuntime.cpp:7706 — UpdateBattleFrameEventEffect_41B440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 336 | 0x0041BB00 | `sub_41BB00` | 有正文 | `src/YobiRuntime.cpp:7912 — UpdateBattleMovingImpactEffect_41BB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 337 | 0x0041C3B0 | `sub_41C3B0` | 有正文 | `src/YobiRuntime.cpp:8163 — UpdateBattleHomingImpactEffect_41C3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 338 | 0x0041CD00 | `sub_41CD00` | 有正文 | `src/YobiRuntime.cpp:8595 — NewBattleEffect_41CD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 339 | 0x0041E4D0 | `sub_41E4D0` | 有正文 | `src/YobiRuntime.cpp:10132 — UpdateBattleScatterEffect_41E4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 340 | 0x0041E510 | `sub_41E510` | 有正文 | `src/YobiRuntime.cpp:10146 — NewBattleScatterEffect_41E510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 341 | 0x0041E5C0 | `sub_41E5C0` | 有正文 | `src/YobiRuntime.cpp:1843 — UpdateBattleAfterimage_41E5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 342 | 0x0041E5F0 | `sub_41E5F0` | 有正文 | `src/YobiRuntime.cpp:1855 — NewBattleAfterimage_41E5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 343 | 0x0041E640 | `sub_41E640` | 有正文 | `src/YobiRuntime.cpp:10171 — UpdateBattlePositionEffect_41E640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 344 | 0x0041E680 | `sub_41E680` | 有正文 | `src/YobiRuntime.cpp:10183 — NewBattlePositionEffect_41E680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 345 | 0x0041E720 | `sub_41E720` | 有正文 | `src/YobiRuntime.cpp:10210 — NewBattleCoordinateEffect_41E720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 346 | 0x0041E7C0 | `sub_41E7C0` | 有正文 | `src/YobiRuntime.cpp:10236 — NewBattleTrailEffect_41E7C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 347 | 0x0041E890 | `sub_41E890` | 有正文 | `src/YobiRuntime.cpp:9128 — DrawBattleTiledEffect_41E890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 348 | 0x0041E920 | `sub_41E920` | 有正文 | `src/YobiRuntime.cpp:9155 — UpdateBattleTiledEffect_41E920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 349 | 0x0041E9D0 | `sub_41E9D0` | 有正文 | `src/YobiRuntime.cpp:9189 — NewBattleTiledEffect_41E9D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 350 | 0x0041EAB0 | `sub_41EAB0` | 有正文 | `src/YobiRuntime.cpp:1823 — CloneBattleAction_41EAB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 351 | 0x0041EB10 | `sub_41EB10` | 有正文 | `src/YobiRuntime.cpp:1709 — CopyYobiAction_41EB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 352 | 0x0041F110 | `sub_41F110` | 有正文 | `src/YobiRuntime.cpp:10334 — UpdateBattleNumber_41F110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 353 | 0x0041F1A0 | `sub_41F1A0` | 有正文 | `src/YobiRuntime.cpp:10367 — NewBattleNumber_41F1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 354 | 0x0041F450 | `sub_41F450` | 有正文 | `src/YobiRuntime.cpp:10520 — UpdateBattleFloatingIcon_41F450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 355 | 0x0041F4C0 | `CreateSubYobi` | 有正文 | `src/YobiRuntime.cpp:10547 — NewBattleFloatingIcon_41F4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 356 | 0x0041F570 | `sub_41F570` | 有正文 | `src/YobiRuntime.cpp:10686 — UpdateBattleHitEffect_41F570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 357 | 0x0041F5B0 | `Battle::Boomerange::newHitEffect` | 有正文 | `src/YobiRuntime.cpp:10703 — NewBattleHitEffect_41F5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 358 | 0x0041F660 | `BattleYobi::Debuff::newEffect` | 有正文 | `src/YobiRuntime.cpp:10736 — NewBattleDebuffEffect_41F660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 359 | 0x0041F800 | `BattleYobi::Debuff::UpdateEffect` | 有正文 | `src/YobiRuntime.cpp:10789 — UpdateBattleDebuffEffect_41F800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 360 | 0x0041F860 | `sub_41F860` | 有正文 | `src/YobiRuntime.cpp:10810 — NewBattleReverseEffect_41F860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 361 | 0x0041F950 | `sub_41F950` | 有正文 | `src/YobiRuntime.cpp:10937 — UpdateBattleCenteredEffect_41F950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 362 | 0x0041FA00 | `sub_41FA00` | 有正文 | `src/YobiRuntime.cpp:10961 — NewBattleCenteredEffect_41FA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 363 | 0x0041FB20 | `sub_41FB20` | 有正文 | `src/YobiRuntime.cpp:11001 — UpdateBattleAttachedEffect_41FB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 364 | 0x0041FBE0 | `sub_41FBE0` | 有正文 | `src/YobiRuntime.cpp:11040 — NewBattleAttachedEffect_41FBE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 365 | 0x0041FCD0 | `sub_41FCD0` | 有正文 | `src/YobiRuntime.cpp:11072 — UpdateBattleStatusEffect_41FCD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 366 | 0x0041FE00 | `sub_41FE00` | 有正文 | `src/YobiRuntime.cpp:11133 — NewBattleStatusEffect_41FE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 367 | 0x0041FF90 | `sub_41FF90` | 有正文 | `src/YobiRuntime.cpp:11188 — UpdateBattleFieldEffect_41FF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 368 | 0x0041FFE0 | `sub_41FFE0` | 有正文 | `src/YobiRuntime.cpp:11206 — NewBattleFieldEffect_41FFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 369 | 0x00420100 | `sub_420100` | 有正文 | `src/YobiRuntime.cpp:6915 — NewBattleSequence_420100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 370 | 0x00420300 | `sub_420300` | 有正文 | `src/YobiRuntime.cpp:6994 — UpdateBattleSequence_420300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 371 | 0x00420590 | `sub_420590` | 有正文 | `src/YobiRuntime.cpp:6977 — MoveBattleSequence_420590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 372 | 0x004205F0 | `sub_4205F0` | 有正文 | `src/YobiRuntime.cpp:9912 — NewBattleCastCopy_4205F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 373 | 0x004206C0 | `sub_4206C0` | 有正文 | `src/YobiRuntime.cpp:10444 — NewBattleTimeNumber_4206C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 374 | 0x004208F0 | `sub_4208F0` | 有正文 | `src/YobiRuntime.cpp:10579 — NewBattleRebirthEffects_4208F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 375 | 0x004209C0 | `sub_4209C0` | 有正文 | `src/YobiRuntime.cpp:10606 — NewBattleAttributeEffects_4209C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 376 | 0x00420D50 | `sub_420D50` | 有正文 | `src/YobiRuntime.cpp:7337 — NewBattleRisingEffect_420D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 377 | 0x00420F10 | `sub_420F10` | 有正文 | `src/YobiRuntime.cpp:30366 — SyncBattleAttachmentAlpha_420F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 378 | 0x00421480 | `sub_421480` | 有正文 | `src/YobiRuntime.cpp:11253 — UpdateBattleBackgroundFade_421480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 379 | 0x00421570 | `sub_421570` | 有正文 | `src/YobiRuntime.cpp:11311 — NewBattleBackgroundFade_421570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 380 | 0x00421610 | `sub_421610` | 有正文 | `src/YobiRuntime.cpp:11435 — NewBattleScreenEffect_421610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 381 | 0x004216B0 | `sub_4216B0` | 有正文 | `src/YobiRuntime.cpp:11463 — UpdateBattleScreenEmitter_4216B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 382 | 0x00421700 | `sub_421700` | 有正文 | `src/YobiRuntime.cpp:11478 — NewBattleScreenEmitter_421700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 383 | 0x00421750 | `sub_421750` | 有正文 | `src/YobiRuntime.cpp:6821 — UpdateBattleText_421750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 384 | 0x004217F0 | `sub_4217F0` | 有正文 | `src/YobiRuntime.cpp:6850 — NewBattleText_4217F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 385 | 0x004219D0 | `sub_4219D0` | 有正文 | `src/YobiRuntime.cpp:8424 — UpdateBattleCarryingEffect_4219D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 386 | 0x00421F90 | `sub_421F90` | 有正文 | `src/YobiRuntime.cpp:11496 — UpdateBattleAttachment_421F90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 387 | 0x00421FB0 | `sub_421FB0` | 有正文 | `src/YobiRuntime.cpp:11507 — SyncBattleRider_421FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 388 | 0x004221F0 | `sub_4221F0` | 有正文 | `src/YobiRuntime.cpp:11781 — NewBattleRider_4221F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 389 | 0x004222B0 | `YobiChara::SetRider` | 有正文 | `src/YobiRuntime.cpp:11880 — SetBattleRider_4222B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 390 | 0x00422370 | `sub_422370` | 有正文 | `src/YobiRuntime.cpp:11344 — RestoreBattleStateEffects_422370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 391 | 0x004225D0 | `sub_4225D0` | 有正文 | `src/YobiRuntime.cpp:11592 — SyncBattleGear_4225D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 392 | 0x00422780 | `sub_422780` | 有正文 | `src/YobiRuntime.cpp:11673 — SyncBattleAuxGear_422780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 393 | 0x00422A30 | `sub_422A30` | 有正文 | `src/YobiRuntime.cpp:11812 — NewBattleGear_422A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 394 | 0x00422B00 | `sub_422B00` | 有正文 | `src/YobiRuntime.cpp:11844 — NewBattleAuxGear_422B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 395 | 0x00422BC0 | `sub_422BC0` | 有正文 | `src/YobiRuntime.cpp:11912 — SetBattleGear_422BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 396 | 0x00422C20 | `sub_422C20` | 有正文 | `src/YobiRuntime.cpp:11933 — SetBattleGear_422C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 397 | 0x00422C90 | `sub_422C90` | 有正文 | `src/YobiRuntime.cpp:11955 — SetBattleAuxGear_422C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 398 | 0x00422E50 | `sub_422E50` | 有正文 | `src/YobiRuntime.cpp:9213 — InitBattleTargetEffect_422E50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 399 | 0x00423040 | `sub_423040` | 有正文 | `src/YobiRuntime.cpp:9275 — InitBattleOffsetEffect_423040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 400 | 0x004231D0 | `sub_4231D0` | 有正文 | `src/YobiRuntime.cpp:9325 — InitBattleCenterEffect_4231D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 401 | 0x00423350 | `sub_423350` | 有正文 | `src/YobiRuntime.cpp:9375 — InitBattleLowerEffect_423350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 402 | 0x004234B0 | `sub_4234B0` | 有正文 | `src/YobiRuntime.cpp:9422 — InitBattleScreenYEffect_4234B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 403 | 0x00423640 | `sub_423640` | 有正文 | `src/platform/MainLoopSupport.cpp:688 — Sub_423640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 404 | 0x004237E0 | `GetBattlePosition` | 有正文 | `src/YobiRuntime.cpp:22990 — GetBattlePosition_4237E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 405 | 0x00423800 | `sub_423800` | 有正文 | `src/YobiRuntime.cpp:12041 — MatchBattleAlbum_423800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 406 | 0x004238F0 | `sub_4238F0` | 有正文 | `src/YobiRuntime.cpp:12067 — UpdateBattleAlbumFlag_4238F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 407 | 0x00423960 | `Battle::ParsePacket_M::GetHeader2` | 有正文 | `src/YobiRuntime.cpp:14549 — GetBattleMObjectHeader_423960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 408 | 0x004239D0 | `Battle::ParsePacket_M_RID` | 有正文 | `src/YobiRuntime.cpp:22196 — ParseBattleMRide_4239D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 409 | 0x004242B0 | `sub_4242B0` | 有正文 | `src/YobiRuntime.cpp:12106 — AppendBattleRedirect_4242B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 410 | 0x00424500 | `sub_424500` | 有正文 | `src/YobiRuntime.cpp:12146 — AppendBattleSkillTail_424500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 411 | 0x00424620 | `sub_424620` | 有正文 | `src/YobiRuntime.cpp:12182 — TakeBattleMSkillAttackTargets_424620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 412 | 0x00426FC0 | `sub_426FC0` | 有正文 | `src/YobiRuntime.cpp:13299 — TakeBattleMSkillTargets_426FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 413 | 0x00427470 | `Yobi::BattleSceneUpdate` | 有正文 | `src/YobiRuntime.cpp:22002 — UpdateBattleEvent_427470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 414 | 0x00438230 | `sub_438230` | 有承接 | `src/YobiRuntime.cpp:20082 — targetPayload->value13D8 != 1 \|\| targetPayload->value13DC != statusType` | 原 type=1 且 value 匹配；当前战斗 M 消费内联；完整业务／运行未验 |
| 415 | 0x00438250 | `sub_438250` | 有承接 | `src/YobiRuntime.cpp:20426 — targetPayload->value13D8 != 3 \|\| targetPayload->value13DC != type` | 原 type=3 且 value 匹配；当前战斗 M 消费内联；完整业务／运行未验 |
| 416 | 0x00438270 | `sub_438270` | 有承接 | `src/platform/LegacyCryptInt.cpp:87 — CopyPayload` | 原载荷逐字段复制；共享实际 CopyPayload；完整业务／运行未验 |
| 417 | 0x00438290 | `YobiBattleChara::new` | 有正文 | `src/YobiRuntime.cpp:22157 — NewBattleActor_438290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 418 | 0x004383B0 | `sub_4383B0` | 有正文 | `src/YobiRuntime.cpp:22125 — NewBattleScene_4383B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 419 | 0x004383F0 | `sub_4383F0` | 有正文 | `src/YobiRuntime.cpp:22141 — InitializeBattleScene_4383F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 420 | 0x00438450 | `getIntFromBattleBuffC1` | 有正文 | `src/YobiRuntime.cpp:22478 — TakeBattleCInteger_438450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 421 | 0x004384C0 | `getCharFromBattleBuffC1` | 有正文 | `src/YobiRuntime.cpp:22510 — TakeBattleCString_4384C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 422 | 0x00438550 | `Battle::ParsePacket_C` | 有正文 | `src/YobiRuntime.cpp:22552 — ParseBattleC_438550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 423 | 0x00438F70 | `Battle::ParsePacket_M::TakeInt` | 有正文 | `src/YobiRuntime.cpp:13449 — TakeBattleMInteger_438F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 424 | 0x00438FE0 | `Battle::ParsePacket_M::TakeStr2` | 有正文 | `src/YobiRuntime.cpp:13481 — TakeBattleMString_438FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 425 | 0x00439070 | `Battle::ParsePacket_M::GetHeader` | 有正文 | `src/YobiRuntime.cpp:13423 — GetBattleMHeader_439070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 426 | 0x004390E0 | `InitBattleEntry` | 有正文 | `src/YobiRuntime.cpp:29804 — InitBattleCastList_4390E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 427 | 0x004391E0 | `sub_4391E0` | 有正文 | `src/YobiRuntime.cpp:29836 — ResetBattleCastList_4391E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 428 | 0x004392A0 | `BattleScene::AppendCastList` | 有正文 | `src/YobiRuntime.cpp:29858 — AppendBattleCast_4392A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 429 | 0x004392F0 | `BattleScene::AppendCastList2` | 有正文 | `src/YobiRuntime.cpp:29874 — AppendBattleCastAt_4392F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 430 | 0x00439340 | `sub_439340` | 有正文 | `src/YobiRuntime.cpp:29891 — AdvanceBattleCastState_439340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 431 | 0x00439380 | `BattleEntry::MoveState` | 有正文 | `src/YobiRuntime.cpp:33990 — MoveBattleCastState_439380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 432 | 0x004393A0 | `BattleEntry::AppendEffectList` | 有正文 | `src/YobiRuntime.cpp:29909 — AppendBattleEffectList_4393A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 433 | 0x00439400 | `BattleEntry::AppendEffect` | 有正文 | `src/YobiRuntime.cpp:29925 — AppendBattleEffect_439400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 434 | 0x00439470 | `BattleEntry::MoveEffect` | 有正文 | `src/YobiRuntime.cpp:32186 — AdvanceBattleEffect_439470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 435 | 0x004394A0 | `sub_4394A0` | 有正文 | `src/YobiRuntime.cpp:34000 — CheckBattleTurnReady_4394A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 436 | 0x00439600 | `Battle::ParsePacket_M::TakeInt2` | 有正文 | `src/YobiRuntime.cpp:13527 — TakeBattleMPreviewInteger_439600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 437 | 0x00439670 | `Battle::ParsePacket_M::TakeStr3` | 有正文 | `src/YobiRuntime.cpp:13559 — TakeBattleMPreviewString_439670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 438 | 0x00439700 | `Battle::ParsePacket_M::GetHeader3` | 有正文 | `src/YobiRuntime.cpp:13604 — GetBattleMPreviewHeader_439700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 439 | 0x00439770 | `Battle::ParsePacket_M::handleRID` | 有正文 | `src/YobiRuntime.cpp:13742 — ScanBattleMRideParticipants_439770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 440 | 0x00439920 | `sub_439920` | 有正文 | `src/YobiRuntime.cpp:13817 — ScanBattleMAttackParticipants_439920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 441 | 0x00439B30 | `Battle::ParsePacket_M::Most` | 有正文 | `src/YobiRuntime.cpp:13889 — ScanBattleMParticipants_439B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 442 | 0x0043B290 | `sub_43B290` | 有正文 | `src/YobiRuntime.cpp:14348 — CheckBattleMParticipants_43B290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 443 | 0x0043B360 | `sub_43B360` | 有正文 | `src/YobiRuntime.cpp:14390 — DispatchBattleEvents_43B360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 444 | 0x0043B610 | `sub_43B610` | 有正文 | `src/YobiRuntime.cpp:22093 — NewBattleEvent_43B610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 445 | 0x0043B6C0 | `sub_43B6C0` | 有正文 | `src/YobiRuntime.cpp:22902 — FinishBattleEvent_43B6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 446 | 0x0043B7F0 | `sub_43B7F0` | 有正文 | `src/YobiRuntime.cpp:22961 — AppendBattleQueuedTarget_43B7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 447 | 0x0043B830 | `sub_43B830` | 有正文 | `src/YobiRuntime.cpp:32174 — AdvanceBattleQueuedTarget_43B830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 448 | 0x0043B860 | `Battle::ParsePacket_M::TakeInt4` | 有正文 | `src/YobiRuntime.cpp:14520 — PeekBattleMInteger_43B860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 449 | 0x0043B8C0 | `Battle::ParsePacket_M::TakeStr` | 有正文 | `src/YobiRuntime.cpp:13697 — TakeBattleMStringAt_43B8C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 450 | 0x0043B940 | `Battle::ParsePacket_M::TakeInt3` | 有正文 | `src/YobiRuntime.cpp:13630 — TakeBattleMIntegerAt_43B940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 451 | 0x0043B9B0 | `sub_43B9B0` | 有正文 | `src/YobiRuntime.cpp:13661 — FindBattleMTarget_43B9B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 452 | 0x0043BAA0 | `sub_43BAA0` | 有正文 | `src/YobiRuntime.cpp:23003 — Sub_43BAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 453 | 0x0043BC10 | `sub_43BC10` | 有正文 | `src/YobiRuntime.cpp:29940 — AppendBattleAuxEffects_43BC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 454 | 0x0043BC90 | `sub_43BC90` | 有正文 | `src/YobiRuntime.cpp:32198 — TakeBattleAuxEffects_43BC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 455 | 0x0043BD00 | `BattleScene::HasAppendEffect` | 有正文 | `src/YobiRuntime.cpp:23044 — IsBattleAppendEffectSkill_43BD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 456 | 0x0043BE00 | `BattleScene::AddAppendEffect` | 有正文 | `src/YobiRuntime.cpp:23067 — AddBattleAppendEffect_43BE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 457 | 0x0043BEA0 | `sub_43BEA0` | 有正文 | `src/YobiRuntime.cpp:23105 — FindBattleAppendEffectTechnique_43BEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 458 | 0x0043BED0 | `sub_43BED0` | 有正文 | `src/YobiRuntime.cpp:23119 — ClearBattleAppendEffects_43BED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 459 | 0x0043BF00 | `sub_43BF00` | 有正文 | `src/YobiRuntime.cpp:22974 — GetBattleValueBand_43BF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 460 | 0x0043BF30 | `sub_43BF30` | 有正文 | `src/YobiRuntime.cpp:13792 — TakeBattleMAuxGraphics_43BF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 461 | 0x0043BFF0 | `sub_43BFF0` | 有正文 | `src/YobiRuntime.cpp:23145 — Sub_43BFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 462 | 0x0043C040 | `sub_43C040` | 有正文 | `src/YobiRuntime.cpp:23161 — Sub_43C040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 463 | 0x0043C060 | `BattleScene::FlushEntry` | 有正文 | `src/YobiRuntime.cpp:34255 — FlushBattleEntries_43C060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 464 | 0x0043C0A0 | `sub_43C0A0` | 有正文 | `src/YobiRuntime.cpp:23171 — DrawBattleEntryHover_43C0A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 465 | 0x0043C120 | `BattleScene::GetSelectedEntry` | 有正文 | `src/YobiRuntime.cpp:23195 — GetSelectedBattleEntry_43C120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 466 | 0x0043C3E0 | `sub_43C3E0` | 有正文 | `src/ui/WindowRuntime.cpp:37512 — Sub_43C3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 467 | 0x0043C490 | `sub_43C490` | 有正文 | `src/ui/WindowRuntime.cpp:37537 — Sub_43C490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 468 | 0x0043C4B0 | `Battle::GetBattlePetIndex` | 有正文 | `src/world/SceneState.cpp:4312 — Sub_43C4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 469 | 0x0043C4E0 | `BattleScene::InitCommand` | 有正文 | `src/world/SceneState.cpp:4324 — Sub_43C4E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 470 | 0x0043C560 | `BattleScene::CheckCommand` | 有正文 | `src/YobiRuntime.cpp:34147 — CheckBattleCommand_43C560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 471 | 0x0043C8F0 | `sub_43C8F0` | 有正文 | `src/world/SceneState.cpp:10707 — Sub_43C8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 472 | 0x0043C910 | `sub_43C910` | 有正文 | `src/YobiRuntime.cpp:34056 — Sub_43C910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 473 | 0x0043C950 | `sub_43C950` | 有正文 | `src/ui/WindowRuntime.cpp:31299 — Sub_43C950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 474 | 0x0043C9A0 | `sub_43C9A0` | 有正文 | `src/YobiRuntime.cpp:34071 — Sub_43C9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 475 | 0x0043CA50 | `sub_43CA50` | 有正文 | `src/YobiRuntime.cpp:34105 — Sub_43CA50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 476 | 0x0043CA80 | `sub_43CA80` | 有正文 | `src/YobiRuntime.cpp:34119 — Sub_43CA80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 477 | 0x0043CB10 | `sub_43CB10` | 有正文 | `src/world/MapSceneDrawing.cpp:335 — Sub_43CB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 478 | 0x0043CBB0 | `World::DrawBattleScene` | 有正文 | `src/world/SceneState.cpp:7161 — DrawBattleScene_43CBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 479 | 0x0043D760 | `sub_43D760` | 有正文 | `src/world/WorldObjectState.cpp:382 — InitializeLegacyWorldObjects_43D760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 480 | 0x0043D770 | `sub_43D770` | 有正文 | `src/world/WorldObjectState.cpp:389 — ConstructWorldObjects` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 481 | 0x0043D7A0 | `sub_43D7A0` | 有正文 | `src/world/WorldObjectState.cpp:398 — DestroyWorldObjects` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 482 | 0x0043D7C0 | `sub_43D7C0` | 有正文 | `src/world/WorldObjectState.cpp:342 — ConstructLegacyWorldElement_43D7C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 483 | 0x0043D810 | `sub_43D810` | 有正文 | `src/world/WorldObjectState.cpp:355 — DestroyLegacyWorldElement_43D810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 484 | 0x0043D860 | `sub_43D860` | 有正文 | `src/YobiRuntime.cpp:36690 — Sub_43D860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 485 | 0x0043D970 | `sub_43D970` | 有正文 | `src/YobiRuntime.cpp:36855 — Sub_43D970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 486 | 0x0043D9A0 | `sub_43D9A0` | 有正文 | `src/YobiRuntime.cpp:36867 — Sub_43D9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 487 | 0x0043D9D0 | `sub_43D9D0` | 有正文 | `src/YobiRuntime.cpp:36879 — Sub_43D9D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 488 | 0x0043DA10 | `sub_43DA10` | 有正文 | `src/YobiRuntime.cpp:36894 — Sub_43DA10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 489 | 0x0043DA40 | `sub_43DA40` | 有正文 | `src/YobiRuntime.cpp:36906 — Sub_43DA40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 490 | 0x0043DA70 | `sub_43DA70` | 有正文 | `src/YobiRuntime.cpp:36918 — Sub_43DA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 491 | 0x0043DAA0 | `sub_43DAA0` | 有正文 | `src/YobiRuntime.cpp:36930 — Sub_43DAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 492 | 0x0043DAD0 | `sub_43DAD0` | 有正文 | `src/YobiRuntime.cpp:36942 — Sub_43DAD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 493 | 0x0043DB00 | `sub_43DB00` | 有正文 | `src/YobiRuntime.cpp:36954 — Sub_43DB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 494 | 0x0043DB30 | `sub_43DB30` | 有正文 | `src/YobiRuntime.cpp:36966 — Sub_43DB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 495 | 0x0043DB60 | `BattleEntry::showHpBar` | 有正文 | `src/YobiRuntime.cpp:6485 — ShowHpBar_43DB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 496 | 0x0043DD00 | `sub_43DD00` | 有正文 | `src/YobiRuntime.cpp:4921 — Sub_43DD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 497 | 0x0043E490 | `sub_43E490` | 有正文 | `src/YobiRuntime.cpp:5093 — Sub_43E490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 498 | 0x0043EFC0 | `sub_43EFC0` | 有正文 | `src/YobiRuntime.cpp:5319 — Sub_43EFC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 499 | 0x0043F180 | `sub_43F180` | 有正文 | `src/YobiRuntime.cpp:5744 — Sub_43F180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 500 | 0x0043F1B0 | `sub_43F1B0` | 有正文 | `src/YobiRuntime.cpp:5763 — Sub_43F1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 501 | 0x0043F1F0 | `sub_43F1F0` | 有正文 | `src/YobiRuntime.cpp:5786 — Sub_43F1F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 502 | 0x0043F220 | `sub_43F220` | 有正文 | `src/YobiRuntime.cpp:5804 — Sub_43F220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 503 | 0x0043F230 | `sub_43F230` | 有正文 | `src/YobiRuntime.cpp:5810 — Sub_43F230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 504 | 0x0043F290 | `sub_43F290` | 有正文 | `src/YobiRuntime.cpp:2425 — Sub_43F290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 505 | 0x0043F330 | `YobiMapChara::new` | 有正文 | `src/YobiRuntime.cpp:2297 — Sub_43F330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 506 | 0x0043F520 | `YobiMapChara::Update` | 有正文 | `src/YobiRuntime.cpp:2771 — Sub_43F520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 507 | 0x0043F910 | `sub_43F910` | 有正文 | `src/YobiRuntime.cpp:2516 — Sub_43F910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 508 | 0x0043FD90 | `sub_43FD90` | 有正文 | `src/YobiRuntime.cpp:5832 — Sub_43FD90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 509 | 0x0043FE00 | `sub_43FE00` | 有正文 | `src/YobiRuntime.cpp:2457 — Sub_43FE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 510 | 0x0043FEB0 | `sub_43FEB0` | 有正文 | `src/YobiRuntime.cpp:2360 — Sub_43FEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 511 | 0x00440070 | `sub_440070` | 有正文 | `src/YobiRuntime.cpp:2880 — Sub_440070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 512 | 0x00440440 | `sub_440440` | 有正文 | `src/YobiRuntime.cpp:2600 — Sub_440440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 513 | 0x004408C0 | `sub_4408C0` | 有正文 | `src/YobiRuntime.cpp:2411 — Sub_4408C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 514 | 0x00440920 | `sub_440920` | 有正文 | `src/YobiRuntime.cpp:2970 — Sub_440920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 515 | 0x00440B50 | `sub_440B50` | 有正文 | `src/world/SceneState.cpp:8649 — Sub_440B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 516 | 0x00440B70 | `sub_440B70` | 有正文 | `src/YobiRuntime.cpp:6628 — Sub_440B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 517 | 0x00440C50 | `sub_440C50` | 有正文 | `src/YobiRuntime.cpp:6534 — Sub_440C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 518 | 0x00440F50 | `sub_440F50` | 有正文 | `src/YobiRuntime.cpp:6672 — Sub_440F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 519 | 0x00440FE0 | `sub_440FE0` | 有正文 | `src/YobiRuntime.cpp:6708 — Sub_440FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 520 | 0x00440FF0 | `sub_440FF0` | 有正文 | `src/world/SceneState.cpp:2432 — Sub_440FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 521 | 0x004410E0 | `sub_4410E0` | 有正文 | `src/world/SceneState.cpp:8656 — Sub_4410E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 522 | 0x004410F0 | `YobiGear::new` | 有正文 | `src/YobiRuntime.cpp:6752 — NewYobiGear_4410F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 523 | 0x00441130 | `YobiGearPet::new` | 有正文 | `src/YobiRuntime.cpp:6716 — NewYobiGearPet_441130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 524 | 0x004412D0 | `sub_4412D0` | 有正文 | `src/YobiRuntime.cpp:5957 — Sub_4412D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 525 | 0x00441430 | `sub_441430` | 有正文 | `src/YobiRuntime.cpp:6003 — Sub_441430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 526 | 0x00441570 | `YobiMapChara::DrawGear` | 有正文 | `src/YobiRuntime.cpp:6044 — DrawGear_441570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 527 | 0x004417A0 | `sub_4417A0` | 有正文 | `src/YobiRuntime.cpp:5899 — Sub_4417A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 528 | 0x004417E0 | `sub_4417E0` | 有正文 | `src/YobiRuntime.cpp:5129 — Sub_4417E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 529 | 0x00441DB0 | `sub_441DB0` | 有正文 | `src/YobiRuntime.cpp:3039 — Sub_441DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 530 | 0x00441F60 | `sub_441F60` | 有正文 | `src/YobiRuntime.cpp:4398 — Sub_441F60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 531 | 0x00441FC0 | `sub_441FC0` | 有正文 | `src/YobiRuntime.cpp:4421 — Sub_441FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 532 | 0x00442110 | `sub_442110` | 有正文 | `src/YobiRuntime.cpp:4486 — Sub_442110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 533 | 0x004422F0 | `sub_4422F0` | 有正文 | `src/YobiRuntime.cpp:4531 — Sub_4422F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 534 | 0x00442460 | `sub_442460` | 有正文 | `src/YobiRuntime.cpp:4579 — Sub_442460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 535 | 0x004424B0 | `sub_4424B0` | 有正文 | `src/YobiRuntime.cpp:5379 — Sub_4424B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 536 | 0x00442730 | `sub_442730` | 有正文 | `src/YobiRuntime.cpp:5480 — Sub_442730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 537 | 0x004428F0 | `sub_4428F0` | 有正文 | `src/YobiRuntime.cpp:5550 — Sub_4428F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 538 | 0x00442AB0 | `sub_442AB0` | 有正文 | `src/YobiRuntime.cpp:4366 — Sub_442AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 539 | 0x00442BD0 | `sub_442BD0` | 有正文 | `src/YobiRuntime.cpp:37308 — Sub_442BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 540 | 0x00442C00 | `sub_442C00` | 有正文 | `src/YobiRuntime.cpp:36769 — Sub_442C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 541 | 0x00442C40 | `sub_442C40` | 有正文 | `src/YobiRuntime.cpp:37283 — Sub_442C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 542 | 0x00442C70 | `sub_442C70` | 有正文 | `src/YobiRuntime.cpp:37295 — Sub_442C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 543 | 0x00442CA0 | `sub_442CA0` | 有正文 | `src/YobiRuntime.cpp:36783 — Sub_442CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 544 | 0x00442CD0 | `sub_442CD0` | 有正文 | `src/YobiRuntime.cpp:36795 — Sub_442CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 545 | 0x00442D00 | `sub_442D00` | 有正文 | `src/YobiRuntime.cpp:36807 — Sub_442D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 546 | 0x00442D30 | `sub_442D30` | 有正文 | `src/YobiRuntime.cpp:36819 — Sub_442D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 547 | 0x00442D60 | `sub_442D60` | 有正文 | `src/YobiRuntime.cpp:36831 — Sub_442D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 548 | 0x00442D90 | `sub_442D90` | 有正文 | `src/YobiRuntime.cpp:36843 — Sub_442D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 549 | 0x00442DC0 | `sub_442DC0` | 有正文 | `src/YobiRuntime.cpp:36724 — Sub_442DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 550 | 0x00442F70 | `sub_442F70` | 有正文 | `src/YobiRuntime.cpp:6085 — Sub_442F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 551 | 0x00444000 | `sub_444000` | 有正文 | `src/YobiRuntime.cpp:5908 — Sub_444000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 552 | 0x004441A0 | `sub_4441A0` | 有正文 | `src/YobiRuntime.cpp:3275 — Sub_4441A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 553 | 0x00444260 | `sub_444260` | 有正文 | `src/YobiRuntime.cpp:3310 — Sub_444260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 554 | 0x00444410 | `sub_444410` | 有正文 | `src/YobiRuntime.cpp:36978 — Sub_444410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 555 | 0x00444FA0 | `World::FindObjectIndex` | 有正文 | `src/world/WorldObjectState.cpp:82 — Sub_444FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 556 | 0x00445030 | `sub_445030` | 有正文 | `src/YobiRuntime.cpp:37712 — Sub_445030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 557 | 0x004450D0 | `sub_4450D0` | 有正文 | `src/YobiRuntime.cpp:37741 — Sub_4450D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 558 | 0x00445160 | `sub_445160` | 有正文 | `src/YobiRuntime.cpp:37768 — Sub_445160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 559 | 0x004451F0 | `sub_4451F0` | 有正文 | `src/YobiRuntime.cpp:37795 — Sub_4451F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 560 | 0x004452A0 | `sub_4452A0` | 有正文 | `src/YobiRuntime.cpp:37825 — Sub_4452A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 561 | 0x00445350 | `sub_445350` | 有正文 | `src/YobiRuntime.cpp:37417 — Sub_445350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 562 | 0x00445A60 | `sub_445A60` | 有正文 | `src/world/WorldObjectState.cpp:180 — Sub_445A60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 563 | 0x00445D20 | `sub_445D20` | 有正文 | `src/YobiRuntime.cpp:37578 — Sub_445D20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 564 | 0x00445F40 | `sub_445F40` | 有正文 | `src/YobiRuntime.cpp:37647 — Sub_445F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 565 | 0x00446130 | `World::TakeEmptyObjectIndex` | 有正文 | `src/world/WorldObjectState.cpp:113 — Sub_446130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 566 | 0x004461E0 | `sub_4461E0` | 有正文 | `src/world/WorldObjectState.cpp:145 — Sub_4461E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 567 | 0x00446210 | `sub_446210` | 有正文 | `src/world/WorldObjectState.cpp:160 — Sub_446210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 568 | 0x004462B0 | `World::FindObjectYobi` | 有正文 | `src/world/WorldObjectState.cpp:105 — FindObjectYobi_4462B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 569 | 0x004462E0 | `sub_4462E0` | 有正文 | `src/world/WorldObjectState.cpp:255 — Sub_4462E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 570 | 0x00446470 | `sub_446470` | 有正文 | `src/world/WorldObjectState.cpp:294 — Sub_446470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 571 | 0x004464D0 | `sub_4464D0` | 有正文 | `src/world/WorldObjectState.cpp:313 — Sub_4464D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 572 | 0x00446610 | `sub_446610` | 有正文 | `src/world/WorldObjectState.cpp:41 — Sub_446610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 573 | 0x00446640 | `sub_446640` | 有正文 | `src/YobiRuntime.cpp:37369 — Sub_446640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 574 | 0x004467D0 | `sub_4467D0` | 有正文 | `src/YobiRuntime.cpp:37328 — Sub_4467D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 575 | 0x004468F0 | `sub_4468F0` | 有正文 | `src/world/WorldObjectState.cpp:52 — Sub_4468F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 576 | 0x00446900 | `sub_446900` | 有正文 | `src/world/WorldObjectState.cpp:58 — Sub_446900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 577 | 0x00446970 | `sub_446970` | 有正文 | `src/YobiRuntime.cpp:891 — Sub_446970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 578 | 0x00446990 | `sub_446990` | 有正文 | `src/YobiRuntime.cpp:900 — Sub_446990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 579 | 0x004469C0 | `sub_4469C0` | 有正文 | `src/YobiRuntime.cpp:3349 — Sub_4469C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 580 | 0x00446B50 | `sub_446B50` | 有正文 | `src/YobiRuntime.cpp:3404 — Sub_446B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 581 | 0x00446C10 | `sub_446C10` | 有正文 | `src/YobiRuntime.cpp:3439 — Sub_446C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 582 | 0x00446D80 | `sub_446D80` | 有正文 | `src/YobiRuntime.cpp:3680 — Sub_446D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 583 | 0x00446FA0 | `sub_446FA0` | 有正文 | `src/YobiRuntime.cpp:3743 — Sub_446FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 584 | 0x00447070 | `sub_447070` | 有正文 | `src/YobiRuntime.cpp:3779 — Sub_447070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 585 | 0x00447230 | `sub_447230` | 有正文 | `src/YobiRuntime.cpp:3806 — Sub_447230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 586 | 0x004473B0 | `sub_4473B0` | 有正文 | `src/YobiRuntime.cpp:3167 — Sub_4473B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 587 | 0x004474E0 | `sub_4474E0` | 有正文 | `src/YobiRuntime.cpp:3220 — Sub_4474E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 588 | 0x00447610 | `sub_447610` | 有正文 | `src/YobiRuntime.cpp:3829 — Sub_447610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 589 | 0x00447820 | `sub_447820` | 有正文 | `src/YobiRuntime.cpp:3861 — Sub_447820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 590 | 0x00447A20 | `sub_447A20` | 有正文 | `src/YobiRuntime.cpp:3889 — Sub_447A20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 591 | 0x00447C40 | `sub_447C40` | 有正文 | `src/YobiRuntime.cpp:3895 — Sub_447C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 592 | 0x00447E60 | `sub_447E60` | 有正文 | `src/YobiRuntime.cpp:3901 — Sub_447E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 593 | 0x00448040 | `sub_448040` | 有正文 | `src/YobiRuntime.cpp:3093 — Sub_448040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 594 | 0x00448240 | `sub_448240` | 有正文 | `src/world/SceneState.cpp:1054 — IsMapFloorAllowed_448240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 595 | 0x00448290 | `sub_448290` | 有正文 | `src/YobiRuntime.cpp:34972 — Sub_448290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 596 | 0x00448370 | `sub_448370` | 有正文 | `src/YobiRuntime.cpp:34301 — Sub_448370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 597 | 0x004483A0 | `sub_4483A0` | 有正文 | `src/YobiRuntime.cpp:34849 — Sub_4483A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 598 | 0x00448810 | `sub_448810` | 有正文 | `src/YobiRuntime.cpp:34509 — Sub_448810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 599 | 0x00448A50 | `sub_448A50` | 有正文 | `src/YobiRuntime.cpp:34403 — Sub_448A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 600 | 0x00448BF0 | `sub_448BF0` | 有正文 | `src/YobiRuntime.cpp:34456 — Sub_448BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 601 | 0x00448D90 | `sub_448D90` | 有正文 | `src/YobiRuntime.cpp:34579 — Sub_448D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 602 | 0x00449160 | `sub_449160` | 有正文 | `src/YobiRuntime.cpp:35078 — Sub_449160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 603 | 0x004493C0 | `sub_4493C0` | 有正文 | `src/world/SceneState.cpp:1069 — Sub_4493C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 604 | 0x00449400 | `sub_449400` | 有正文 | `src/YobiRuntime.cpp:35023 — Sub_449400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 605 | 0x00449570 | `sub_449570` | 有正文 | `src/YobiRuntime.cpp:34317 — Sub_449570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 606 | 0x00449670 | `sub_449670` | 有正文 | `src/YobiRuntime.cpp:35010 — Sub_449670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 607 | 0x004496C0 | `sub_4496C0` | 有正文 | `src/YobiRuntime.cpp:34357 — Sub_4496C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 608 | 0x00449700 | `sub_449700` | 有正文 | `src/YobiRuntime.cpp:34760 — Sub_449700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 609 | 0x00449A00 | `sub_449A00` | 有正文 | `src/YobiRuntime.cpp:34669 — Sub_449A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 610 | 0x00449BC0 | `sub_449BC0` | 有正文 | `src/YobiRuntime.cpp:34741 — Sub_449BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 611 | 0x00449C10 | `sub_449C10` | 有正文 | `src/YobiRuntime.cpp:34727 — Sub_449C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 612 | 0x00449C90 | `sub_449C90` | 有正文 | `src/YobiRuntime.cpp:37254 — Sub_449C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 613 | 0x00449CC0 | `sub_449CC0` | 有正文 | `src/YobiRuntime.cpp:37270 — Sub_449CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 614 | 0x00449CF0 | `sub_449CF0` | 有正文 | `src/Input.cpp:1109 — InitializeContext_449CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 615 | 0x00449DA0 | `sub_449DA0` | 有正文 | `src/Input.cpp:1300 — Sub_449DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 616 | 0x00449FA0 | `sub_449FA0` | 有正文 | `src/Input.cpp:1276 — Sub_449FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 617 | 0x0044A090 | `sub_44A090` | 有正文 | `src/Input.cpp:1251 — Sub_44A090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 618 | 0x0044A160 | `sub_44A160` | 有正文 | `src/Input.cpp:279 — LoadInitialText_44A160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 619 | 0x0044A1B0 | `sub_44A1B0` | 有正文 | `src/Input.cpp:1479 — BrowseChatHistory_44A1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 620 | 0x0044A410 | `sub_44A410` | 有正文 | `src/InputCaret.cpp:124 — Sub_44A410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 621 | 0x0044A540 | `sub_44A540` | 有正文 | `src/Input.cpp:396 — Sub_44A540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 622 | 0x0044A580 | `sub_44A580` | 有正文 | `src/Input.cpp:416 — Sub_44A580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 623 | 0x0044A5D0 | `sub_44A5D0` | 有正文 | `src/Input.cpp:299 — Sub_44A5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 624 | 0x0044A7D0 | `sub_44A7D0` | 有正文 | `src/Input.cpp:437 — Sub_44A7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 625 | 0x0044A990 | `sub_44A990` | 有正文 | `src/Input.cpp:523 — Sub_44A990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 626 | 0x0044AA90 | `sub_44AA90` | 有正文 | `src/Input.cpp:553 — Sub_44AA90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 627 | 0x0044AB60 | `sub_44AB60` | 有正文 | `src/Input.cpp:1550 — Sub_44AB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 628 | 0x0044ADB0 | `Input::AppendChar` | 有正文 | `src/Input.cpp:924 — AppendChar_44ADB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 629 | 0x0044B050 | `World::AppendMsgToCharWindow` | 有正文 | `src/Input.cpp:1406 — Sub_44B050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 630 | 0x0044B170 | `sub_44B170` | 有正文 | `src/ui/WindowRuntime.cpp:30985 — Sub_44B170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 631 | 0x0044B190 | `sub_44B190` | 有正文 | `src/Input.cpp:155 — ClassifyLastAnsiUnit_44B190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 632 | 0x0044B1D0 | `sub_44B1D0` | 有正文 | `src/Input.cpp:245 — ClassifyAnsiBoundary_44B1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 633 | 0x0044B250 | `sub_44B250` | 有承接 | `src/Input.cpp:429 — (static_cast<unsigned char>(context->text[caret]) & 0x80u) != 0u` | 首字节高位决定 1／2 字节步长；实际输入消费者内联；完整业务／运行未验 |
| 634 | 0x0044B260 | `sub_44B260` | 有正文 | `src/render/Font.cpp:271 — MeasureDefaultText_44B260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 635 | 0x0044B2A0 | `TextMeasure` | 有正文 | `src/render/Font.cpp:295 — TextMeasure` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 636 | 0x0044B2E0 | `Font::measure` | 有正文 | `src/render/Font.cpp:225 — Font::measure` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 637 | 0x0044B330 | `Input::SetContext` | 有正文 | `src/Input.cpp:1080 — SetContext_44B330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 638 | 0x0044B340 | `sub_44B340` | 有正文 | `src/Input.cpp:1056 — ProcessPresetHotkeys_44B340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 639 | 0x0044B440 | `sub_44B440` | 有正文 | `src/Input.cpp:192 — ClassifyLegacyBoundary_44B440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 640 | 0x0044B4A0 | `sub_44B4A0` | 有正文 | `src/Input.cpp:219 — FindLegacyByte_44B4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 641 | 0x0044B570 | `sub_44B570` | 有正文 | `src/Input.cpp:175 — ClassifyChatSplit_44B570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 642 | 0x0044B5A0 | `sub_44B5A0` | 有正文 | `src/platform/MainLoopSupport.cpp:558 — ResetSystemLog_44B5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 643 | 0x0044B5C0 | `sub_44B5C0` | 有正文 | `src/LoginState.cpp:118 — Sub_44B5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 644 | 0x0044B600 | `sub_44B600` | 有承接 | `src/ui/WindowRuntime.cpp:2942 — std::bit_cast<std::int8_t>(platform::gSupportedRenderMode)` | 原 W8 读取；当前 Sub_493CB0 直接读状态；完整业务／运行未验 |
| 645 | 0x0044B610 | `hasIDirect3D7` | 有正文 | `src/render/RendererResources.cpp:154 — HasLegacy3DDevice_44B610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 646 | 0x0044B630 | `Renderer::Init` | 有正文 | `src/render/RendererResources.cpp:218 — RendererInit_44B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 647 | 0x0044C3C0 | `Renderer::Destroy` | 有正文 | `src/render/SdlPresenter.cpp:555 — RendererDestroy_44C3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 648 | 0x0044C560 | `Renderer::Flip` | 有正文 | `src/render/SdlPresenter.cpp:398 — Flip_44C560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 649 | 0x0044C6B0 | `Renderer::TestCooperativeLevel` | 有正文 | `src/render/SdlPresenter.cpp:615 — TestCooperativeLevel_44C6B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 650 | 0x0044C6D0 | `Renderer::MakeSurface` | 有正文 | `src/render/GraphicDxCache.cpp:242 — CreateTextureSurface_44C6D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 651 | 0x0044C8B0 | `Renderer::CreateSurrface` | 有正文 | `src/render/GraphicDxCache.cpp:281 — CreatePlainTextureSurface_44C8B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 652 | 0x0044C9A0 | `Renderer::FillRect` | 有正文 | `src/render/LegacyTextureDrawing.cpp:273 — FillRect_44C9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 653 | 0x0044CC20 | `sub_44CC20` | 有正文 | `src/render/GraphicDxCache.cpp:219 — InitializeSurfaceTransformCache629300_44CC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 654 | 0x0044CC30 | `sub_44CC30` | 有正文 | `src/render/GraphicDxCache.cpp:191 — ConstructSurfaceTransformCache629300_44CC30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 655 | 0x0044CC60 | `sub_44CC60` | 有正文 | `src/render/GraphicDxCache.cpp:205 — DestroySurfaceTransformCache629300_44CC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 656 | 0x0044CD30 | `sub_44CD30` | 有正文 | `src/render/GraphicDxCache.cpp:226 — InitializeSurfaceTransformCache62A630_44CD30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 657 | 0x0044CD40 | `sub_44CD40` | 有正文 | `src/render/GraphicDxCache.cpp:198 — ConstructSurfaceTransformCache62A630_44CD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 658 | 0x0044CD70 | `unknown_libname_9` | 有正文 | `src/render/GraphicDxCache.cpp:211 — DestroySurfaceTransformCache62A630_44CD70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 659 | 0x0044CD80 | `sub_44CD80` | 有正文 | `src/render/GraphicDxCache.cpp:829 — TransformTextureSurface_44CD80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 660 | 0x0044D3F0 | `sub_44D3F0` | 有承接 | `src/render/GraphicDxCache.cpp:153 — struct SurfaceTransformKeyLess` | 实际 operator() 比较器；关联原入口；完整业务／运行未验 |
| 661 | 0x0044D490 | `sub_44D490` | 有正文 | `src/render/GraphicDxCache.cpp:890 — ClearSurfaceTransformCache_44D490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
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
| 677 | 0x0044FF40 | `sub_44FF40` | 有承接 | `src/render/GraphicDxCache.cpp:776 — LoadTextureFromBuffer32_450400` | 32 位空 palette 分支由 CPU 载荷承接；低色深分支已排除；完整业务／运行未验 |
| 678 | 0x00450190 | `sub_450190` | 有承接 | `src/render/GraphicDxCache.cpp:776 — LoadTextureFromBuffer32_450400` | 32 位非空 palette 分支由 CPU 载荷承接；低色深分支已排除；完整业务／运行未验 |
| 679 | 0x00450400 | `Texture::LoadFromBuffer` | 有正文 | `src/render/GraphicDxCache.cpp:776 — LoadTextureFromBuffer32_450400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 680 | 0x00450470 | `Renderer::InitPalette` | 有正文 | `src/Palette.cpp:181 — InitPalette32` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 681 | 0x00450A80 | `sub_450A80` | 有正文 | `src/Palette.cpp:280 — ReadPalette` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 682 | 0x00450B70 | `sub_450B70` | 有正文 | `src/Palette.cpp:313 — SelectPalette` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 683 | 0x00450BB0 | `sub_450BB0` | 有正文 | `src/Palette.cpp:328 — UpdatePalette_450BB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 684 | 0x00451190 | `sub_451190` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 685 | 0x00451220 | `sub_451220` | 有正文 | `src/Palette.cpp:504 — RebuildPalette32` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 686 | 0x00451270 | `sub_451270` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 687 | 0x004512C0 | `Renderer::ClearSurface` | 有正文 | `src/render/SurfaceState.cpp:536 — ClearSurface_4512C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 688 | 0x00451310 | `Texture::FillBlack` | 有正文 | `src/render/GraphicDxCache.cpp:739 — FillTextureBlack_451310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 689 | 0x00451350 | `sub_451350` | 有正文 | `src/render/SurfaceState.cpp:547 — Sub_451350` | 32 位实际正文；低色深分支已具名排除；完整调色板／渲染运行未验 |
| 690 | 0x00451D80 | `sub_451D80` | 有正文 | `src/GraphicRead.cpp:612 — GetGraphicAutoMapColor_451D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 691 | 0x00451E90 | `sub_451E90` | 有正文 | `src/Palette.cpp:18 — FindNearestPaletteColor_451E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 692 | 0x00451FA0 | `sub_451FA0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 693 | 0x004522F0 | `sub_4522F0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 694 | 0x00452840 | `sub_452840` | 有正文 | `src/render/SurfaceState.cpp:214 — DrawMapEffects32_452840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 695 | 0x00452CF0 | `sub_452CF0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 696 | 0x00453580 | `sub_453580` | 有正文 | `src/render/SurfaceState.cpp:387 — DrawMapEffects_453580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 697 | 0x004535B0 | `sub_4535B0` | 有正文 | `src/render/SurfaceState.cpp:693 — BlitSurfaceToBack_4535B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 698 | 0x004536D0 | `Renderer::IsLost` | 有正文 | `src/render/SdlPresenter.cpp:574 — IsLost_4536D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 699 | 0x00453760 | `takeScreenshot` | 有正文 | `src/render/SdlPresenter.cpp:456 — TakeScreenshot_453760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 700 | 0x00453DE0 | `sub_453DE0` | 有正文 | `src/Palette.cpp:168 — CountMaskBits_453DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 701 | 0x00453E00 | `sub_453E00` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 702 | 0x00453E10 | `unknown_libname_10` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 703 | 0x00453E30 | `unknown_libname_11` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 704 | 0x00453E40 | `sub_453E40` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 705 | 0x00453E50 | `unknown_libname_12` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 706 | 0x00453E70 | `unknown_libname_13` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原流 Init／退出职责由实际 MSVC 标准流及本线配置、pubsync 退出顺序承接；无旧私有包装；完整退出／运行未验 |
| 707 | 0x00453E80 | `sub_453E80` | 有正文 | `src/render/SdlPresenter.cpp:408 — SaveScreenshotBitmap_453E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 708 | 0x00453FB0 | `sub_453FB0` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 709 | 0x00454050 | `sub_454050` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 710 | 0x00454420 | `sub_454420` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 711 | 0x004544F0 | `sub_4544F0` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 712 | 0x00454730 | `sub_454730` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 713 | 0x00454780 | `sub_454780` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 714 | 0x004547C0 | `sub_4547C0` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 715 | 0x00454820 | `sub_454820` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 716 | 0x00454880 | `sub_454880` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 717 | 0x004548A0 | `sub_4548A0` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 718 | 0x00454B10 | `sub_454B10` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 719 | 0x00454B30 | `sub_454B30` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 720 | 0x00454B90 | `sub_454B90` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 721 | 0x00454BE0 | `sub_454BE0` | 有承接 | `src/render/GraphicDxCache.cpp:168 — using SurfaceTransformCache` | SurfaceTransformCache 的实际 std::map 构析、find／operator[]／erase、构析及键复制；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 722 | 0x00454C00 | `sub_454C00` | 有正文 | `src/platform/LegacyMouse.cpp:142 — InitializeLegacyEmptyExit_454C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
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
| 734 | 0x00455490 | `sub_455490` | 有正文 | `src/LegacyUiRecord.cpp:2130 — Sub_455490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 735 | 0x004554A0 | `sub_4554A0` | 有正文 | `src/LegacyUiRecord.cpp:2136 — Sub_4554A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 736 | 0x004554C0 | `sub_4554C0` | 有正文 | `src/LegacyUiRecord.cpp:2145 — Sub_4554C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 737 | 0x004554D0 | `sub_4554D0` | 有正文 | `src/LegacyUiRecord.cpp:2151 — Sub_4554D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 738 | 0x00455540 | `sub_455540` | 有正文 | `src/LegacyUiRecord.cpp:2157 — Sub_455540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 739 | 0x00455550 | `sub_455550` | 有正文 | `src/LegacyUiRecord.cpp:2163 — Sub_455550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 740 | 0x004555C0 | `sub_4555C0` | 有正文 | `src/LegacyUiRecord.cpp:2169 — Sub_4555C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 741 | 0x004555D0 | `sub_4555D0` | 有正文 | `src/LegacyUiRecord.cpp:2175 — Sub_4555D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 742 | 0x00455640 | `sub_455640` | 有正文 | `src/LegacyUiRecord.cpp:2181 — Sub_455640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 743 | 0x00455650 | `sub_455650` | 有正文 | `src/LegacyUiRecord.cpp:2187 — Sub_455650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 744 | 0x004556C0 | `sub_4556C0` | 有正文 | `src/LegacyUiRecord.cpp:2193 — Sub_4556C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 745 | 0x004556D0 | `sub_4556D0` | 有正文 | `src/LegacyUiRecord.cpp:2199 — Sub_4556D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 746 | 0x00455740 | `sub_455740` | 有正文 | `src/LegacyUiRecord.cpp:2205 — Sub_455740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 747 | 0x00455750 | `sub_455750` | 有正文 | `src/LegacyUiRecord.cpp:2211 — Sub_455750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 748 | 0x004557C0 | `sub_4557C0` | 有正文 | `src/LegacyUiRecord.cpp:2217 — Sub_4557C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 749 | 0x004557D0 | `sub_4557D0` | 有正文 | `src/LegacyUiRecord.cpp:2223 — Sub_4557D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 750 | 0x00455840 | `sub_455840` | 有正文 | `src/LegacyUiRecord.cpp:2243 — Sub_455840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 751 | 0x00455850 | `sub_455850` | 有正文 | `src/LegacyUiRecord.cpp:2249 — Sub_455850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 752 | 0x00455930 | `sub_455930` | 有正文 | `src/LegacyUiRecord.cpp:2288 — Sub_455930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 753 | 0x00455940 | `sub_455940` | 有正文 | `src/LegacyUiRecord.cpp:2294 — Sub_455940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 754 | 0x00455A90 | `canWatchBattle` | 有正文 | `src/world/SceneState.cpp:9238 — Sub_455A90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 755 | 0x00455AC0 | `___setargv` | 有正文 | `src/world/SceneState.cpp:10577 — Sub_455AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 756 | 0x00455AD0 | `___setargv_0` | 有正文 | `src/world/SceneState.cpp:10583 — Sub_455AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 757 | 0x00455AE0 | `___setargv_1` | 有正文 | `src/world/SceneState.cpp:10589 — Sub_455AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 758 | 0x00455AF0 | `___setargv_2` | 有正文 | `src/world/SceneState.cpp:10595 — Sub_455AF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 759 | 0x00455B00 | `___setargv_3` | 有正文 | `src/world/SceneState.cpp:10601 — Sub_455B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 760 | 0x00455B10 | `sub_455B10` | 有正文 | `src/world/SceneState.cpp:10607 — Sub_455B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 761 | 0x00455B20 | `sub_455B20` | 有正文 | `src/world/SceneState.cpp:10613 — Sub_455B20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 762 | 0x00455B40 | `___setargv_6` | 有正文 | `src/world/SceneState.cpp:10621 — Sub_455B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 763 | 0x00455B50 | `sub_455B50` | 有正文 | `src/world/SceneState.cpp:10627 — Sub_455B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 764 | 0x00455B70 | `sub_455B70` | 有正文 | `src/world/SceneState.cpp:10635 — Sub_455B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 765 | 0x00455B90 | `sub_455B90` | 有正文 | `src/world/SceneState.cpp:10643 — Sub_455B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 766 | 0x00455BB0 | `sub_455BB0` | 有正文 | `src/world/SceneState.cpp:10651 — Sub_455BB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 767 | 0x00455BD0 | `sub_455BD0` | 有正文 | `src/world/SceneState.cpp:10659 — Sub_455BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 768 | 0x00455C60 | `sub_455C60` | 有正文 | `src/world/SceneState.cpp:5974 — Sub_455C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 769 | 0x00455C70 | `sub_455C70` | 有正文 | `src/world/SceneState.cpp:1158 — NextRotationValue_455C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 770 | 0x00455C90 | `sub_455C90` | 有正文 | `src/world/WorldUpdate.cpp:232 — Sub_455C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 771 | 0x00455FC0 | `sub_455FC0` | 有正文 | `src/world/SceneState.cpp:5980 — Sub_455FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 772 | 0x00456050 | `sub_456050` | 有正文 | `src/ResourcePaths.cpp:186 — BuildResourcePaths` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 773 | 0x00456190 | `sub_456190` | 有正文 | `src/render/Font.cpp:93 — InitializeFont_456190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 774 | 0x00456530 | `sub_456530` | 有正文 | `src/render/Font.cpp:157 — ReleaseFont_456530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 775 | 0x00456560 | `Renderer::DrawText3` | 有正文 | `src/render/TextPendingQueue.cpp:111 — DrawText3_456560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 776 | 0x00456670 | `Renderer::DrawText2` | 有正文 | `src/render/TextPendingQueue.cpp:123 — DrawText2_456670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 777 | 0x00456780 | `Renderer::DrawText` | 有正文 | `src/render/TextPendingQueue.cpp:184 — DrawText_456780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 778 | 0x004568A0 | `Renderer::DrawInputText` | 有正文 | `src/render/InputText.cpp:55 — DrawInputText_4568A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 779 | 0x00456D30 | `Renderer::CacheReset` | 有正文 | `src/render/GraphicCacheState.cpp:17 — ResetGraphicCache_456D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 780 | 0x00456D50 | `sub_456D50` | 有正文 | `src/render/GraphicCacheState.cpp:10 — ClearTextQueueTail_456D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 781 | 0x00456D70 | `Renderer::RenderTextToSurface` | 有正文 | `src/render/Font.cpp:167 — RenderTextToSurface_456D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 782 | 0x00456FD0 | `sub_456FD0` | 有正文 | `src/world/SceneState.cpp:727 — InitializeLegacySceneTransitionValue_456FD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 783 | 0x00456FE0 | `unknown_libname_15` | 有正文 | `src/world/SceneState.cpp:734 — ConstructSceneTransitionValue` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 784 | 0x00457000 | `unknown_libname_16` | 有正文 | `src/world/SceneState.cpp:740 — DestroySceneTransitionValue` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 785 | 0x00457010 | `sub_457010` | 有正文 | `src/platform/MainLoopSupport.cpp:589 — RequestRender_457010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 786 | 0x00457020 | `MainLoop` | 有正文 | `src/platform/MainLoop.cpp:51 — MainLoop` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 787 | 0x004574E0 | `GraphicAlloc` | 有正文 | `src/GraphicAlloc.cpp:20 — GraphicAlloc_4574E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 788 | 0x00457670 | `sub_457670` | 有正文 | `src/GraphicAlloc.cpp:97 — Sub_457670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 789 | 0x004576B0 | `sub_4576B0` | 有正文 | `src/platform/ClientInitialization.cpp:186 — InitializeClient_4576B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 790 | 0x00457A40 | `sub_457A40` | 有正文 | `src/platform/ClientShutdown.cpp:42 — ShutdownClient_457A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 791 | 0x00457AB0 | `KeepAlive` | 有正文 | `src/platform/MainLoopSupport.cpp:633 — KeepAlive_457AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 792 | 0x00457B10 | `InitLoading` | 有正文 | `src/platform/LoadingScreen.cpp:32 — InitLoading_457B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 793 | 0x00458390 | `sub_458390` | 有正文 | `src/platform/ClientInitialization.cpp:108 — FadeInLoading_458390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 794 | 0x004585F0 | `sub_4585F0` | 有正文 | `src/platform/ClientInitialization.cpp:154 — FadeOutLoading_4585F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 795 | 0x004587D0 | `sub_4587D0` | 有正文 | `src/platform/MainLoopWindowEnumeration.cpp:76 — EnumerateWindows_4587D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 796 | 0x00458840 | `sub_458840` | 有正文 | `src/platform/MainLoopWindowEnumeration.cpp:21 — EnumerateWindowsCallback_458840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 797 | 0x004589A0 | `sub_4589A0` | 有承接 | `src/network/NetworkRuntime.cpp:173 — platform::gWindowReportValue > 0` | 活动QueuePlaintextProtocolBytes内联承接；轮询／容量门禁通过后按首标记优先、报告值S32>0／登录状态1／截止W32按S32<0采样并清零；原命令字段LF保持，完整业务／运行未验 |
| 798 | 0x004589B0 | `sub_4589B0` | 有承接 | `src/network/NetworkRuntime.cpp:176 — platform::gWindowReportValue = 0` | 活动QueuePlaintextProtocolBytes内联承接；轮询／容量门禁通过后按首标记优先、报告值S32>0／登录状态1／截止W32按S32<0采样并清零；原命令字段LF保持，完整业务／运行未验 |
| 799 | 0x004589C0 | `sub_4589C0` | 有承接 | `src/network/NetworkRuntime.cpp:175 — std::bit_cast<std::int32_t>(platform::gWindowReportDeadline) < 0` | 活动QueuePlaintextProtocolBytes内联承接；轮询／容量门禁通过后按首标记优先、报告值S32>0／登录状态1／截止W32按S32<0采样并清零；原命令字段LF保持，完整业务／运行未验 |
| 800 | 0x004589D0 | `sub_4589D0` | 有正文 | `src/platform/MainLoopSupport.cpp:779 — ReadSystemSeconds_4589D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 801 | 0x00458A10 | `sub_458A10` | 有正文 | `src/platform/MainLoopSupport.cpp:792 — CheckSystemSeconds_458A10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 802 | 0x00458A40 | `sub_458A40` | 有正文 | `src/platform/MainLoop.cpp:22 — ShowModeChange_458A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 803 | 0x00458AE0 | `sub_458AE0` | 有正文 | `src/LegacyAudio.cpp:1559 — Sub_458AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 804 | 0x00458DC0 | `sub_458DC0` | 有正文 | `src/platform/MainLoopSupport.cpp:598 — CheckLegacyDxCapability_458DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 805 | 0x00458DE0 | `World::UpdateNRTime` | 有正文 | `src/world/GameTime.cpp:16 — UpdateGameTime` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 806 | 0x00458EC0 | `Time::GetCate` | 有正文 | `src/world/GameTime.cpp:43 — GetCate_458EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 807 | 0x00458F00 | `sub_458F00` | 有正文 | `src/world/SceneState.cpp:7121 — Sub_458F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 808 | 0x00458F40 | `sub_458F40` | 有正文 | `src/world/SceneState.cpp:7094 — Sub_458F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 809 | 0x00458FD0 | `sub_458FD0` | 有正文 | `src/platform/ImeLifecycle.cpp:18 — InitializeIme_458FD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 810 | 0x00459040 | `sub_459040` | 有正文 | `src/platform/ImeLifecycle.cpp:46 — ShutdownIme_459040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 811 | 0x00459090 | `sub_459090` | 有正文 | `src/platform/ImeLifecycle.cpp:62 — IsImeKeyboardLayout_459090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 812 | 0x004590A0 | `sub_4590A0` | 有正文 | `src/platform/ImeLifecycle.cpp:71 — ResetImeOpenStatus_4590A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 813 | 0x004590D0 | `sub_4590D0` | 有正文 | `src/platform/ImeLifecycle.cpp:92 — BorrowImeDescription_4590D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 814 | 0x004590E0 | `World::RenderInput` | 有正文 | `src/world/RenderInput.cpp:229 — RenderInput_4590E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 815 | 0x00459580 | `sub_459580` | 有正文 | `src/platform/ImeComposition.cpp:22 — Sub_459580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 816 | 0x004596D0 | `sub_4596D0` | 有正文 | `src/InputKeyDispatch.cpp:12 — ProcessKeyInput_4596D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 817 | 0x00459760 | `sub_459760` | 有正文 | `src/platform/WindowProcedure.cpp:42 — ClearKeyFlags_459760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 818 | 0x00459780 | `resetKeyState` | 有正文 | `src/platform/WindowProcedure.cpp:51 — ResetKeyState_459780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 819 | 0x004597A0 | `sub_4597A0` | 有正文 | `src/platform/WindowProcedure.cpp:66 — ReadKeyFlag4_4597A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 820 | 0x004597B0 | `sub_4597B0` | 有正文 | `src/platform/WindowProcedure.cpp:60 — ReadKeyPressFlag_4597B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 821 | 0x004597C0 | `sub_4597C0` | 有正文 | `src/platform/WindowProcedure.cpp:72 — ReadControlKeyPressFlag_4597C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 822 | 0x004597E0 | `sub_4597E0` | 有正文 | `src/GraphicInfo.cpp:83 — InitializeGraphicMapNumbers_4597E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 823 | 0x004597F0 | `sub_4597F0` | 有正文 | `src/GraphicInfo.cpp:55 — ConstructGraphicMapNumbers_4597F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 824 | 0x00459820 | `sub_459820` | 有正文 | `src/GraphicInfo.cpp:62 — DestroyGraphicMapNumbers_459820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 825 | 0x00459850 | `sub_459850` | 有正文 | `src/GraphicInfo.cpp:90 — InitializeGraphicFirstRecords_459850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 826 | 0x00459860 | `sub_459860` | 有正文 | `src/GraphicInfo.cpp:68 — ConstructGraphicFirstRecords_459860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 827 | 0x00459890 | `sub_459890` | 有正文 | `src/GraphicInfo.cpp:75 — DestroyGraphicFirstRecords_459890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 828 | 0x00459960 | `GraphicInit` | 有正文 | `src/GraphicInfo.cpp:97 — GraphicInit_459960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 829 | 0x0045A130 | `sub_45A130` | 有正文 | `src/ResourcePaths.cpp:211 — LoadAutoMapColor_45A130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 830 | 0x0045A190 | `sub_45A190` | 有正文 | `src/ResourcePaths.cpp:111 — Sub_45A190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 831 | 0x0045A290 | `sub_45A290` | 有正文 | `src/ResourcePaths.cpp:88 — Sub_45A290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 832 | 0x0045A420 | `sub_45A420` | 有正文 | `src/ResourcePaths.cpp:55 — Sub_45A420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 833 | 0x0045A6C0 | `sub_45A6C0` | 有正文 | `src/GraphicRead.cpp:663 — CloseGraphicFiles` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 834 | 0x0045A710 | `Graphic::getXY` | 有正文 | `src/GraphicInfo.cpp:247 — GetGraphicXY` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 835 | 0x0045A760 | `Graphic::getWH` | 有正文 | `src/GraphicInfo.cpp:262 — GetGraphicWH` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 836 | 0x0045A7B0 | `Graphic::getSxSy` | 有正文 | `src/GraphicInfo.cpp:277 — GetGraphicSxSy` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 837 | 0x0045A810 | `Graphic::getFlag1` | 有正文 | `src/GraphicInfo.cpp:292 — GetGraphicFlag1` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 838 | 0x0045A860 | `Graphic::getFlag2` | 有正文 | `src/GraphicInfo.cpp:305 — GetGraphicFlag2` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 839 | 0x0045A8B0 | `Graphic::getMapNoIndex` | 有正文 | `src/GraphicInfo.cpp:318 — GetGraphicMapNoIndex` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 840 | 0x0045A8D0 | `Graphic::getMapNo` | 有正文 | `src/GraphicInfo.cpp:360 — GetGraphicMapNo` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 841 | 0x0045A900 | `Graphic::readGraphicDataByIndex` | 有正文 | `src/GraphicRead.cpp:571 — ReadGraphicDataByIndex` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
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
| 868 | 0x0045BF60 | `CoordLoad` | 有正文 | `src/AnimationRuntime.cpp:407 — CoordLoad_45BF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 869 | 0x0045C040 | `CoordLoadInit` | 有正文 | `src/AnimationRuntime.cpp:387 — CoordLoadInit_45C040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 870 | 0x0045C150 | `CoordInit` | 有正文 | `src/AnimationRuntime.cpp:347 — CoordInit_45C150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 871 | 0x0045C2D0 | `sub_45C2D0` | 有正文 | `src/LoginOuterScene.cpp:35 — ProcessOuterScene_45C2D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 872 | 0x0045C5A0 | `sub_45C5A0` | 有正文 | `src/LoginModeScene.cpp:32 — ProcessModeScene_45C5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 873 | 0x0045C7C0 | `sub_45C7C0` | 有正文 | `src/LoginInput.cpp:321 — InitializeInputContexts_45C7C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 874 | 0x0045C970 | `sub_45C970` | 有正文 | `src/GraphicInfo.cpp:325 — ProbePaletteCapability_45C970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 875 | 0x0045CA20 | `sub_45CA20` | 有正文 | `src/LoginInputController.cpp:278 — ProcessLoginInput_45CA20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 876 | 0x0045CE00 | `sub_45CE00` | 有正文 | `src/LoginMessage.cpp:73 — ResetLoginMessage` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 877 | 0x0045CE10 | `sub_45CE10` | 有正文 | `src/LoginMessage.cpp:79 — UpdateLoginMessage` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 878 | 0x0045D720 | `sub_45D720` | 有正文 | `src/Input.cpp:125 — Sub_45D720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 879 | 0x0045D760 | `sub_45D760` | 有正文 | `src/Input.cpp:141 — Sub_45D760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 880 | 0x0045D7A0 | `sub_45D7A0` | 有正文 | `src/render/TextPendingQueue.cpp:57 — FindClickedTextIndex_45D7A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 881 | 0x0045D7D0 | `sub_45D7D0` | 有正文 | `src/CharacterAuthenticationFailure.cpp:27 — ResetAuthenticationFailure_45D7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 882 | 0x0045D7E0 | `sub_45D7E0` | 有正文 | `src/CharacterAuthenticationFailure.cpp:33 — UpdateAuthenticationFailure_45D7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 883 | 0x0045D980 | `sub_45D980` | 有正文 | `src/LoginState.cpp:329 — ResetValues_45D980` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 884 | 0x0045D990 | `sub_45D990` | 有正文 | `src/LoginState.cpp:337 — UpdateServerConnection` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 885 | 0x0045DBF0 | `sub_45DBF0` | 有正文 | `src/CharacterDeletionProgress.cpp:31 — ResetCharacterDeletionProgress_45DBF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 886 | 0x0045DC00 | `sub_45DC00` | 有正文 | `src/CharacterDeletionProgress.cpp:37 — UpdateCharacterDeletionProgress_45DC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 887 | 0x0045DD50 | `sub_45DD50` | 有正文 | `src/CharacterListProgress.cpp:38 — ResetCharacterListProgress_45DD50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 888 | 0x0045DD60 | `sub_45DD60` | 有正文 | `src/CharacterListProgress.cpp:44 — UpdateCharacterListProgress_45DD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 889 | 0x0045DEE0 | `sub_45DEE0` | 有正文 | `src/CharacterDeletionConfirmation.cpp:34 — ProcessConfirmationDialog_45DEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 890 | 0x0045E090 | `sub_45E090` | 有正文 | `src/CharacterDeletionConfirmation.cpp:137 — ProcessDefaultConfirmationDialog_45E090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 891 | 0x0045E0C0 | `sub_45E0C0` | 有正文 | `src/CharacterDeletionConfirmation.cpp:147 — ProcessCharacterDeletionWarning_45E0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 892 | 0x0045E3D0 | `sub_45E3D0` | 有正文 | `src/CharacterVersionUpgrade.cpp:54 — ProcessLegacyVersionScene_45E3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 893 | 0x0045E6E0 | `sub_45E6E0` | 有正文 | `src/YobiRuntime.cpp:35865 — Sub_45E6E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 894 | 0x0045E770 | `sub_45E770` | 有正文 | `src/Input.cpp:673 — Sub_45E770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 895 | 0x0045E7B0 | `sub_45E7B0` | 有正文 | `src/CharacterSelectScene.cpp:209 — Sub_45E7B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 896 | 0x0045E7D0 | `sub_45E7D0` | 有正文 | `src/CharacterSelectScene.cpp:218 — Sub_45E7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 897 | 0x0045E960 | `sub_45E960` | 有正文 | `src/LoginState.cpp:439 — ProcessState_45E960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 898 | 0x0045EA50 | `sub_45EA50` | 有正文 | `src/LoginState.cpp:479 — ResetLoginProgress` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 899 | 0x0045EA60 | `sub_45EA60` | 有正文 | `src/LoginState.cpp:485 — UpdateLoginProgress` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 900 | 0x0045EBB0 | `sub_45EBB0` | 有正文 | `src/world/SceneState.cpp:12072 — Sub_45EBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 901 | 0x0045ED80 | `sub_45ED80` | 有正文 | `src/world/SceneState.cpp:12150 — Sub_45ED80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 902 | 0x0045ED90 | `sub_45ED90` | 有正文 | `src/world/SceneState.cpp:12156 — Sub_45ED90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 903 | 0x0045EEE0 | `sub_45EEE0` | 有正文 | `src/world/SceneState.cpp:10726 — Sub_45EEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 904 | 0x0045EF80 | `sub_45EF80` | 有正文 | `src/LoginCapability3Scene.cpp:70 — Sub_45EF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 905 | 0x0045EFD0 | `sub_45EFD0` | 有正文 | `src/LegacyUiRecord.cpp:18642 — Sub_45EFD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 906 | 0x0045F090 | `sub_45F090` | 有正文 | `src/DrawGraphic.cpp:129 — Sub_45F090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 907 | 0x0045F100 | `sub_45F100` | 有正文 | `src/LegacyUiRecord.cpp:18636 — Sub_45F100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 908 | 0x0045F190 | `sub_45F190` | 有正文 | `src/LoginCapability3Scene.cpp:207 — Sub_45F190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 909 | 0x0045F970 | `sub_45F970` | 有正文 | `src/LoginInput.cpp:418 — InitializeCapabilityInput_45F970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 910 | 0x0045FA70 | `sub_45FA70` | 有正文 | `src/LoginCapability3InputController.cpp:78 — ProcessCapability3Input_45FA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 911 | 0x0045FEA0 | `LoginScene::Process` | 有正文 | `src/LoginScene.cpp:480 — ProcessScene_45FEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 912 | 0x00460D00 | `sub_460D00` | 有正文 | `src/CharacterSelectPreview.cpp:479 — ProcessCharacterPreview_460D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 913 | 0x00460FF0 | `sub_460FF0` | 有正文 | `src/CharacterSelectScene.cpp:1478 — ProcessScene_460FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 914 | 0x00461B30 | `sub_461B30` | 有正文 | `src/CharacterSelectSlot.cpp:85 — ProcessCharacterSlot_461B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 915 | 0x00462090 | `sub_462090` | 有正文 | `src/CharacterSelectModal.cpp:26 — ProcessCharacterModal_462090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 916 | 0x00462330 | `sub_462330` | 有正文 | `src/CharacterVersionUpgrade.cpp:159 — ProcessScene_462330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 917 | 0x004626A0 | `sub_4626A0` | 有正文 | `src/CharacterSelectScene.cpp:275 — ProcessCreationScene_4626A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 918 | 0x00462870 | `sub_462870` | 有正文 | `src/CharacterSelectScene.cpp:395 — Sub_462870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 919 | 0x00462880 | `sub_462880` | 有正文 | `src/CharacterSelectScene.cpp:402 — Sub_462880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 920 | 0x00462930 | `sub_462930` | 有正文 | `src/CharacterSelectScene.cpp:435 — Sub_462930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 921 | 0x00463100 | `sub_463100` | 有正文 | `src/CharacterSelectScene.cpp:670 — Sub_463100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 922 | 0x004631D0 | `sub_4631D0` | 有正文 | `src/CharacterSelectScene.cpp:705 — Sub_4631D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 923 | 0x00464290 | `sub_464290` | 有正文 | `src/CharacterSelectScene.cpp:1196 — Sub_464290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 924 | 0x004642C0 | `sub_4642C0` | 有正文 | `src/CharacterSelectScene.cpp:1120 — Sub_4642C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 925 | 0x00464380 | `sub_464380` | 有正文 | `src/CharacterSelectScene.cpp:1157 — Sub_464380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 926 | 0x00464440 | `sub_464440` | 有正文 | `src/LoginCapability4Modal.cpp:74 — ResetCapability4Modal_464440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 927 | 0x00464450 | `sub_464450` | 有正文 | `src/LoginCapability4Modal.cpp:80 — UpdateCapability4Modal_464450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 928 | 0x00464840 | `sub_464840` | 有正文 | `src/LoginCapability4Scene.cpp:69 — Sub_464840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 929 | 0x00464E30 | `sub_464E30` | 有正文 | `src/LoginInputController.cpp:98 — ProcessCapability4Input_464E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 930 | 0x00465320 | `sub_465320` | 有正文 | `src/LoginInput.cpp:472 — InitializeCapability4Input_465320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 931 | 0x00465430 | `sub_465430` | 有正文 | `src/LoginInput.cpp:554 — ProcessCapability4SavedSlotSelector_465430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 932 | 0x00465740 | `sub_465740` | 有正文 | `src/LoginInput.cpp:533 — SaveAndSwitchCapability4InputSlot_465740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 933 | 0x00465790 | `_WinMain@16` | 有正文 | `src/platform/ClientEntry.cpp:242 — WinMain` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 934 | 0x004659F0 | `sub_4659F0` | 有正文 | `src/platform/StartupCommon.cpp:12 — ParseClientVersion` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 935 | 0x00465A20 | `sub_465A20` | 有正文 | `src/platform/StartupCommon.cpp:18 — CaptureDesktopColor` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 936 | 0x00465A30 | `sub_465A30` | 有正文 | `src/platform/WindowLifecycle.cpp:11 — SetWindowedDesktopColor` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 937 | 0x00465A80 | `ReadSetupIni` | 有正文 | `src/platform/SetupIni.cpp:15 — ReadSetupIni` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 938 | 0x00465F00 | `ParseCmdOpt` | 有正文 | `src/platform/CommandOptions.cpp:60 — ParseCommandOptions` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 939 | 0x004669F0 | `WndInit` | 有正文 | `src/platform/WindowLifecycle.cpp:19 — InitializeWindow` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 940 | 0x00466B80 | `Renderer::Rebuild` | 有正文 | `src/platform/RendererRebuild.cpp:20 — RebuildRenderer_466B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 941 | 0x00466D50 | `sub_466D50` | 有正文 | `src/platform/WindowLifecycle.cpp:75 — PumpWindowMessages` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 942 | 0x00466DC0 | `WIN_PROC` | 有正文 | `src/platform/WindowProcedure.cpp:139 — WindowProcedure` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 943 | 0x00467B90 | `sub_467B90` | 有正文 | `src/world/SceneState.cpp:913 — InitializeLegacyPlayerCoordinates_467B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 944 | 0x00467BA0 | `unknown_libname_17` | 有正文 | `src/world/SceneState.cpp:748 — ConstructLegacyPlayerCoordinates_467BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 945 | 0x00467BD0 | `sub_467BD0` | 有正文 | `src/world/SceneState.cpp:755 — DestroyLegacyPlayerCoordinates_467BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 946 | 0x00467BF0 | `sub_467BF0` | 有正文 | `src/world/SceneState.cpp:921 — InitializeLegacyPlayerSnapshotX_467BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 947 | 0x00467C00 | `unknown_libname_19` | 有正文 | `src/world/SceneState.cpp:762 — ConstructLegacyPlayerSnapshotX_467C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 948 | 0x00467C20 | `unknown_libname_20` | 有正文 | `src/world/SceneState.cpp:768 — DestroyLegacyPlayerSnapshotX_467C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 949 | 0x00467C30 | `sub_467C30` | 有正文 | `src/world/SceneState.cpp:929 — InitializeLegacyPlayerSnapshotY_467C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 950 | 0x00467C40 | `unknown_libname_21` | 有正文 | `src/world/SceneState.cpp:774 — ConstructLegacyPlayerSnapshotY_467C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 951 | 0x00467C60 | `unknown_libname_22` | 有正文 | `src/world/SceneState.cpp:780 — DestroyLegacyPlayerSnapshotY_467C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 952 | 0x00467C70 | `sub_467C70` | 有正文 | `src/world/SceneState.cpp:937 — InitializeLegacyWorldCoordinateState_467C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 953 | 0x00467C80 | `unknown_libname_23` | 有正文 | `src/world/SceneState.cpp:786 — ConstructLegacyWorldCoordinateState_467C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 954 | 0x00467CA0 | `unknown_libname_24` | 有正文 | `src/world/SceneState.cpp:792 — DestroyLegacyWorldCoordinateState_467CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 955 | 0x00467CB0 | `sub_467CB0` | 有正文 | `src/world/SceneState.cpp:945 — InitializeLegacyWorldCoordinateState_467CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 956 | 0x00467CC0 | `unknown_libname_25` | 有正文 | `src/world/SceneState.cpp:798 — ConstructLegacyWorldCoordinateState_467CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 957 | 0x00467CE0 | `unknown_libname_26` | 有正文 | `src/world/SceneState.cpp:804 — DestroyLegacyWorldCoordinateState_467CE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 958 | 0x00467CF0 | `sub_467CF0` | 有正文 | `src/world/SceneState.cpp:953 — InitializeLegacyWorldCoordinatePair_467CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 959 | 0x00467D00 | `unknown_libname_27` | 有正文 | `src/world/SceneState.cpp:810 — ConstructLegacyWorldCoordinatePair_467D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 960 | 0x00467D30 | `unknown_libname_28` | 有正文 | `src/world/SceneState.cpp:817 — DestroyLegacyWorldCoordinatePair_467D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 961 | 0x00467D50 | `sub_467D50` | 有正文 | `src/world/SceneState.cpp:961 — InitializeLegacyWorldMoveTargets_467D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 962 | 0x00467D60 | `sub_467D60` | 有正文 | `src/world/SceneState.cpp:824 — ConstructLegacyWorldMoveTargets_467D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 963 | 0x00467D90 | `sub_467D90` | 有正文 | `src/world/SceneState.cpp:831 — DestroyLegacyWorldMoveTargets_467D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 964 | 0x00467DB0 | `sub_467DB0` | 有正文 | `src/world/SceneState.cpp:969 — InitializeLegacyWorldMoveState_467DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 965 | 0x00467DC0 | `unknown_libname_31` | 有正文 | `src/world/SceneState.cpp:838 — ConstructLegacyWorldMoveState_467DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 966 | 0x00467DE0 | `unknown_libname_32` | 有正文 | `src/world/SceneState.cpp:844 — DestroyLegacyWorldMoveState_467DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 967 | 0x00467DF0 | `sub_467DF0` | 有正文 | `src/world/SceneState.cpp:977 — InitializeLegacyWorldMoveState_467DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 968 | 0x00467E00 | `unknown_libname_33` | 有正文 | `src/world/SceneState.cpp:850 — ConstructLegacyWorldMoveState_467E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 969 | 0x00467E20 | `unknown_libname_34` | 有正文 | `src/world/SceneState.cpp:856 — DestroyLegacyWorldMoveState_467E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 970 | 0x00467E30 | `sub_467E30` | 有正文 | `src/world/SceneState.cpp:985 — InitializeLegacyWorldMoveState_467E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 971 | 0x00467E40 | `unknown_libname_35` | 有正文 | `src/world/SceneState.cpp:862 — ConstructLegacyWorldMoveState_467E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 972 | 0x00467E60 | `unknown_libname_36` | 有正文 | `src/world/SceneState.cpp:868 — DestroyLegacyWorldMoveState_467E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 973 | 0x00467E70 | `sub_467E70` | 有正文 | `src/world/SceneState.cpp:993 — InitializeLegacyWorldMoveState_467E70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 974 | 0x00467E80 | `unknown_libname_37` | 有正文 | `src/world/SceneState.cpp:874 — ConstructLegacyWorldMoveState_467E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 975 | 0x00467EA0 | `unknown_libname_38` | 有正文 | `src/world/SceneState.cpp:880 — DestroyLegacyWorldMoveState_467EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 976 | 0x00467EB0 | `sub_467EB0` | 有正文 | `src/world/SceneState.cpp:1001 — InitializeLegacyWorldMoveState_467EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 977 | 0x00467EC0 | `unknown_libname_39` | 有正文 | `src/world/SceneState.cpp:886 — ConstructLegacyWorldMoveState_467EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 978 | 0x00467EE0 | `unknown_libname_40` | 有正文 | `src/world/SceneState.cpp:892 — DestroyLegacyWorldMoveState_467EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 979 | 0x00467EF0 | `sub_467EF0` | 有正文 | `src/world/SceneState.cpp:1009 — InitializeLegacyRippleGate_467EF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 980 | 0x00467F00 | `unknown_libname_41` | 有正文 | `src/world/SceneState.cpp:898 — ConstructLegacyRippleGate_467F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 981 | 0x00467F20 | `unknown_libname_42` | 有正文 | `src/world/SceneState.cpp:904 — DestroyLegacyRippleGate_467F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 982 | 0x00467F30 | `sub_467F30` | 有正文 | `src/world/SceneState.cpp:5658 — Sub_467F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 983 | 0x004681B0 | `sub_4681B0` | 有正文 | `src/world/SceneState.cpp:5768 — Sub_4681B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 984 | 0x00468200 | `sub_468200` | 有正文 | `src/world/SceneState.cpp:5519 — Sub_468200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 985 | 0x00468290 | `sub_468290` | 有正文 | `src/world/SceneState.cpp:5786 — Sub_468290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 986 | 0x004682E0 | `sub_4682E0` | 有正文 | `src/world/SceneState.cpp:7853 — Sub_4682E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 987 | 0x00468330 | `sub_468330` | 有正文 | `src/world/SceneState.cpp:5564 — Sub_468330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 988 | 0x00468520 | `sub_468520` | 有正文 | `src/world/SceneState.cpp:4775 — Sub_468520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 989 | 0x00468570 | `Map::SaveToDisk` | 有正文 | `src/world/SceneState.cpp:4790 — Sub_468570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 990 | 0x004687B0 | `sub_4687B0` | 有正文 | `src/world/SceneState.cpp:4846 — Sub_4687B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 991 | 0x004688A0 | `sub_4688A0` | 有正文 | `src/world/SceneState.cpp:4879 — Sub_4688A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 992 | 0x00468C00 | `sub_468C00` | 有正文 | `src/world/SceneState.cpp:5810 — Sub_468C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 993 | 0x00468C40 | `sub_468C40` | 有正文 | `src/world/SceneState.cpp:5147 — Sub_468C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 994 | 0x00468F30 | `sub_468F30` | 有正文 | `src/world/SceneState.cpp:8187 — Sub_468F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 995 | 0x00468F70 | `sub_468F70` | 有正文 | `src/world/SceneState.cpp:8205 — Sub_468F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 996 | 0x00468FF0 | `sub_468FF0` | 有正文 | `src/world/SceneState.cpp:1130 — CopyWorldProjectionState_468FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 997 | 0x00469020 | `sub_469020` | 有正文 | `src/world/MapSceneDrawing.cpp:328 — Sub_469020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 998 | 0x00469030 | `sub_469030` | 有正文 | `src/world/SceneState.cpp:7630 — Sub_469030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 999 | 0x004691D0 | `sub_4691D0` | 有正文 | `src/world/SceneState.cpp:5913 — Sub_4691D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1000 | 0x00469240 | `sub_469240` | 有正文 | `src/world/SceneState.cpp:5853 — Sub_469240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1001 | 0x00469290 | `sub_469290` | 有正文 | `src/world/SceneState.cpp:5867 — Sub_469290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1002 | 0x004693F0 | `sub_4693F0` | 有正文 | `src/world/SceneState.cpp:7704 — Sub_4693F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1003 | 0x00469810 | `sub_469810` | 有正文 | `src/world/SceneState.cpp:5833 — Sub_469810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1004 | 0x00469870 | `sub_469870` | 有正文 | `src/world/SceneState.cpp:4961 — Sub_469870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1005 | 0x00469C90 | `World::CharaMove` | 有正文 | `src/world/SceneState.cpp:9611 — Sub_469C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1006 | 0x00469FC0 | `sub_469FC0` | 有正文 | `src/world/SceneState.cpp:9730 — Sub_469FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1007 | 0x0046A190 | `sub_46A190` | 有正文 | `src/YobiRuntime.cpp:37889 — Sub_46A190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1008 | 0x0046A410 | `sub_46A410` | 有正文 | `src/world/SceneState.cpp:9827 — Sub_46A410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1009 | 0x0046A5A0 | `sub_46A5A0` | 有正文 | `src/world/SceneState.cpp:9101 — Sub_46A5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1010 | 0x0046A5F0 | `sub_46A5F0` | 有正文 | `src/YobiRuntime.cpp:38054 — Sub_46A5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1011 | 0x0046A700 | `sub_46A700` | 有正文 | `src/world/SceneState.cpp:9886 — Sub_46A700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1012 | 0x0046A940 | `sub_46A940` | 有正文 | `src/world/SceneState.cpp:9121 — Sub_46A940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1013 | 0x0046A990 | `sub_46A990` | 有正文 | `src/world/SceneState.cpp:8996 — Sub_46A990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1014 | 0x0046A9E0 | `sub_46A9E0` | 有正文 | `src/world/SceneState.cpp:7896 — Sub_46A9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1015 | 0x0046AA00 | `sub_46AA00` | 有正文 | `src/world/SceneState.cpp:7905 — Sub_46AA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1016 | 0x0046AB60 | `sub_46AB60` | 有正文 | `src/world/SceneState.cpp:8102 — Sub_46AB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1017 | 0x0046ABD0 | `sub_46ABD0` | 有正文 | `src/world/SceneState.cpp:7997 — Sub_46ABD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1018 | 0x0046AC10 | `sub_46AC10` | 有正文 | `src/world/SceneState.cpp:8014 — Sub_46AC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1019 | 0x0046AC90 | `sub_46AC90` | 有正文 | `src/world/SceneState.cpp:8137 — Sub_46AC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1020 | 0x0046AD00 | `sub_46AD00` | 有正文 | `src/world/SceneState.cpp:7827 — Sub_46AD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1021 | 0x0046ADA0 | `sub_46ADA0` | 有正文 | `src/world/SceneState.cpp:8063 — Sub_46ADA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1022 | 0x0046AE60 | `sub_46AE60` | 有正文 | `src/world/SceneState.cpp:5822 — Sub_46AE60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1023 | 0x0046AEA0 | `sub_46AEA0` | 有正文 | `src/world/SceneState.cpp:8156 — Sub_46AEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1024 | 0x0046AF30 | `sub_46AF30` | 有正文 | `src/world/SceneState.cpp:5114 — Sub_46AF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1025 | 0x0046AFE0 | `sub_46AFE0` | 有正文 | `src/world/SceneState.cpp:9028 — Sub_46AFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1026 | 0x0046B1B0 | `Player::MoveOneStep` | 有正文 | `src/YobiRuntime.cpp:37990 — MovePlayerOneStep_46B1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1027 | 0x0046B370 | `sub_46B370` | 有正文 | `src/world/SceneState.cpp:9568 — Sub_46B370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1028 | 0x0046B4B0 | `sub_46B4B0` | 有正文 | `src/YobiRuntime.cpp:37854 — Sub_46B4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1029 | 0x0046B580 | `sub_46B580` | 有正文 | `src/world/SceneState.cpp:8353 — Sub_46B580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1030 | 0x0046B5C0 | `sub_46B5C0` | 有正文 | `src/world/SceneState.cpp:8367 — Sub_46B5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1031 | 0x0046B6F0 | `sub_46B6F0` | 有正文 | `src/world/SceneState.cpp:8420 — Sub_46B6F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1032 | 0x0046BC90 | `sub_46BC90` | 有正文 | `src/world/SceneState.cpp:7980 — Sub_46BC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1033 | 0x0046BCF0 | `sub_46BCF0` | 有正文 | `src/world/SceneState.cpp:8047 — Sub_46BCF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1034 | 0x0046BD20 | `sub_46BD20` | 有正文 | `src/world/SceneState.cpp:8623 — Sub_46BD20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1035 | 0x0046BD40 | `sub_46BD40` | 有正文 | `src/world/SceneState.cpp:10688 — Sub_46BD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1036 | 0x0046BD60 | `sub_46BD60` | 有正文 | `src/world/MapDrawCache.cpp:348 — Sub_46BD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1037 | 0x0046BE50 | `sub_46BE50` | 有正文 | `src/world/MapDrawCache.cpp:373 — Sub_46BE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1038 | 0x0046C060 | `YobiMapChara::DrawGearNormal` | 有正文 | `src/world/MapDrawCache.cpp:440 — DrawGearNormal_46C060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1039 | 0x0046C1C0 | `sub_46C1C0` | 有正文 | `src/world/MapDrawCache.cpp:489 — Sub_46C1C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1040 | 0x0046C2F0 | `sub_46C2F0` | 有正文 | `src/world/MapDrawCache.cpp:482 — Sub_46C2F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1041 | 0x0046C300 | `sub_46C300` | 有正文 | `src/world/MapDrawCache.cpp:100 — CompareMapObject_46C300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1042 | 0x0046C420 | `sub_46C420` | 有正文 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1043 | 0x0046C440 | `sub_46C440` | 有正文 | `src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1044 | 0x0046C4D0 | `sub_46C4D0` | 有正文 | `src/world/MapDrawCache.cpp:186 — MergeMapDrawCache_46C4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1045 | 0x0046C760 | `sub_46C760` | 有正文 | `src/world/SceneState.cpp:5424 — Sub_46C760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1046 | 0x0046C840 | `sub_46C840` | 有正文 | `src/world/SceneState.cpp:5250 — Sub_46C840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1047 | 0x0046CDF0 | `sub_46CDF0` | 有正文 | `src/world/SceneState.cpp:8341 — Sub_46CDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1048 | 0x0046CE30 | `sub_46CE30` | 有正文 | `src/world/SceneState.cpp:1084 — ProjectWorldToScreen_46CE30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1049 | 0x0046CE80 | `sub_46CE80` | 有正文 | `src/world/SceneState.cpp:1096 — ProjectScreenToWorld_46CE80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1050 | 0x0046CEC0 | `sub_46CEC0` | 有正文 | `src/world/SceneState.cpp:5547 — Sub_46CEC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1051 | 0x0046CEE0 | `sub_46CEE0` | 有正文 | `src/world/SceneState.cpp:5554 — Sub_46CEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1052 | 0x0046CF00 | `sub_46CF00` | 有正文 | `src/world/SceneState.cpp:8226 — Sub_46CF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1053 | 0x0046CF20 | `sub_46CF20` | 有正文 | `src/world/SceneState.cpp:8238 — Sub_46CF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1054 | 0x0046CF60 | `sub_46CF60` | 有正文 | `src/world/MapSceneDrawing.cpp:196 — Sub_46CF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1055 | 0x0046D9C0 | `sub_46D9C0` | 有正文 | `src/world/SceneState.cpp:7800 — Sub_46D9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1056 | 0x0046DA80 | `sub_46DA80` | 有正文 | `src/world/SceneState.cpp:7873 — Sub_46DA80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1057 | 0x0046DB00 | `sub_46DB00` | 有正文 | `src/world/SceneState.cpp:5459 — Sub_46DB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1058 | 0x0046DC60 | `sub_46DC60` | 有正文 | `src/world/SceneState.cpp:5450 — Sub_46DC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1059 | 0x0046DC90 | `sub_46DC90` | 有正文 | `src/world/SceneState.cpp:5497 — Sub_46DC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1060 | 0x0046DD40 | `sub_46DD40` | 有正文 | `src/YobiRuntime.cpp:1560 — Sub_46DD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1061 | 0x0046DD90 | `sub_46DD90` | 有正文 | `src/YobiRuntime.cpp:5717 — Sub_46DD90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1062 | 0x0046DE50 | `sub_46DE50` | 有正文 | `src/YobiRuntime.cpp:5652 — Sub_46DE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1063 | 0x0046DFF0 | `sub_46DFF0` | 有正文 | `src/world/MapDrawCache.cpp:407 — Sub_46DFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1064 | 0x0046E0D0 | `sub_46E0D0` | 有正文 | `src/world/SceneState.cpp:2140 — Sub_46E0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1065 | 0x0046E2A0 | `sub_46E2A0` | 有正文 | `src/world/SceneState.cpp:1492 — Sub_46E2A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1066 | 0x0046E340 | `sub_46E340` | 有正文 | `src/world/SceneState.cpp:1525 — Sub_46E340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1067 | 0x0046E390 | `sub_46E390` | 有正文 | `src/world/SceneState.cpp:1549 — Sub_46E390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1068 | 0x0046E3E0 | `sub_46E3E0` | 有正文 | `src/world/SceneState.cpp:1575 — Sub_46E3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1069 | 0x0046E480 | `sub_46E480` | 有正文 | `src/world/SceneState.cpp:1767 — Sub_46E480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1070 | 0x0046E4A0 | `sub_46E4A0` | 有正文 | `src/world/SceneState.cpp:2130 — Sub_46E4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1071 | 0x0046E4C0 | `sub_46E4C0` | 有正文 | `src/world/SceneState.cpp:2040 — Sub_46E4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1072 | 0x0046E4E0 | `sub_46E4E0` | 有正文 | `src/world/SceneState.cpp:1776 — Sub_46E4E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1073 | 0x0046E6C0 | `sub_46E6C0` | 有正文 | `src/world/SceneState.cpp:1842 — Sub_46E6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1074 | 0x0046E940 | `sub_46E940` | 有正文 | `src/world/SceneState.cpp:1906 — Sub_46E940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1075 | 0x0046EBB0 | `sub_46EBB0` | 有正文 | `src/world/SceneState.cpp:1975 — Sub_46EBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1076 | 0x0046ED50 | `sub_46ED50` | 有正文 | `src/world/SceneState.cpp:2047 — Sub_46ED50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1077 | 0x0046F000 | `sub_46F000` | 有正文 | `src/world/SceneState.cpp:2254 — Sub_46F000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1078 | 0x0046F090 | `sub_46F090` | 有正文 | `src/world/SceneState.cpp:2205 — Sub_46F090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1079 | 0x0046F160 | `sub_46F160` | 有正文 | `src/world/SceneState.cpp:2184 — Sub_46F160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1080 | 0x0046F1D0 | `sub_46F1D0` | 有正文 | `src/world/SceneState.cpp:2282 — Sub_46F1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1081 | 0x0046F680 | `World::RendererCursorGrid` | 有正文 | `src/world/SceneState.cpp:9143 — Sub_46F680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1082 | 0x0046F8F0 | `sub_46F8F0` | 有正文 | `src/world/SceneState.cpp:8250 — Sub_46F8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1083 | 0x0046F930 | `sub_46F930` | 有正文 | `src/world/SceneState.cpp:8271 — Sub_46F930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1084 | 0x0046F950 | `sub_46F950` | 有正文 | `src/world/SceneState.cpp:8280 — Sub_46F950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1085 | 0x0046F970 | `sub_46F970` | 有正文 | `src/world/SceneState.cpp:8287 — Sub_46F970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1086 | 0x0046FAF0 | `RAND` | 有正文 | `src/platform/MainLoopSupport.cpp:676 — RandomRange_46FAF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1087 | 0x0046FB20 | `sub_46FB20` | 有正文 | `src/world/SceneState.cpp:1144 — InitializeRotationTable_46FB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1088 | 0x0046FB80 | `sub_46FB80` | 有正文 | `src/world/SceneState.cpp:8330 — Sub_46FB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1089 | 0x0046FBA0 | `sub_46FBA0` | 有正文 | `src/ui/WindowRuntime.cpp:41575 — Sub_46FBA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1090 | 0x0046FBC0 | `sub_46FBC0` | 有正文 | `src/ui/WindowRuntime.cpp:31271 — Sub_46FBC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1091 | 0x0046FBD0 | `sub_46FBD0` | 有正文 | `src/ui/WindowRuntime.cpp:41584 — Sub_46FBD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1092 | 0x0046FC60 | `sub_46FC60` | 有正文 | `src/ui/WindowRuntime.cpp:41791 — Sub_46FC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1093 | 0x00470050 | `sub_470050` | 有正文 | `src/world/SceneState.cpp:9251 — Sub_470050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1094 | 0x00470150 | `sub_470150` | 有正文 | `src/world/SceneState.cpp:9283 — Sub_470150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1095 | 0x00470270 | `sub_470270` | 有正文 | `src/world/SceneState.cpp:9318 — Sub_470270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1096 | 0x004703C0 | `sub_4703C0` | 有正文 | `src/world/SceneState.cpp:9358 — Sub_4703C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1097 | 0x004704A0 | `sub_4704A0` | 有正文 | `src/world/SceneState.cpp:9388 — Sub_4704A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1098 | 0x004705D0 | `sub_4705D0` | 有正文 | `src/ui/WindowRuntime.cpp:16461 — InitializeWindowSystem_4705D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1099 | 0x004705F0 | `sub_4705F0` | 有正文 | `src/platform/ClientShutdown.cpp:31 — Sub_4705F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1100 | 0x00470600 | `sub_470600` | 有正文 | `src/ui/WindowRuntime.cpp:30359 — InitializeLoginWindows_470600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1101 | 0x00470670 | `sub_470670` | 有正文 | `src/world/SceneState.cpp:9424 — Sub_470670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1102 | 0x004708A0 | `sub_4708A0` | 有正文 | `src/ui/WindowRuntime.cpp:41751 — Sub_4708A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1103 | 0x00470910 | `sub_470910` | 有正文 | `src/ui/WindowRuntime.cpp:41771 — Sub_470910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1104 | 0x00470980 | `sub_470980` | 有正文 | `src/ui/WindowRuntime.cpp:16441 — Sub_470980` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1105 | 0x00470A80 | `sub_470A80` | 有正文 | `src/ui/WindowRuntime.cpp:41887 — Sub_470A80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1106 | 0x00470A9F | `?classic@locale@std@@SAABV12@XZ_0` | 有承接 | `src/ui/WindowRuntime.cpp:41887 — Sub_470A80` | 原共享尾为调用 470AC0 后返回 1；并入实际父正文；完整业务／运行未验 |
| 1107 | 0x00470AB0 | `?classic@locale@std@@SAABV12@XZ_1` | 有正文 | `src/ui/WindowRuntime.cpp:41898 — Sub_470AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1108 | 0x00470AC0 | `sub_470AC0` | 有正文 | `src/ui/WindowRuntime.cpp:41906 — Sub_470AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1109 | 0x00471270 | `UIWindowInstance::free` | 有正文 | `src/ui/WindowRuntime.cpp:46174 — FreeWindow_471270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1110 | 0x00471440 | `Window::open` | 有正文 | `src/ui/WindowRuntime.cpp:45530 — OpenWindow_471440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1111 | 0x004718F0 | `sub_4718F0` | 有正文 | `src/ui/WindowRuntime.cpp:41968 — Sub_4718F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1112 | 0x00471DF0 | `sub_471DF0` | 有正文 | `src/ui/WindowRuntime.cpp:42025 — Sub_471DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1113 | 0x00472160 | `Windows::close` | 有正文 | `src/ui/WindowRuntime.cpp:46251 — CloseWindow_472160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1114 | 0x00472190 | `Window::CloseAll` | 有正文 | `src/ui/WindowRuntime.cpp:46260 — CloseAllWindows_472190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1115 | 0x00472200 | `sub_472200` | 有正文 | `src/ui/WindowRuntime.cpp:15846 — Sub_472200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1116 | 0x00472220 | `sub_472220` | 有正文 | `src/ui/WindowRuntime.cpp:15854 — Sub_472220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1117 | 0x00472240 | `Window::CloseByCategory` | 有正文 | `src/ui/WindowRuntime.cpp:46353 — CloseWindowsByCategory_472240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1118 | 0x004722A0 | `MouseEvent::Reset` | 有正文 | `src/ui/WindowRuntime.cpp:15864 — Sub_4722A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1119 | 0x004722E0 | `sub_4722E0` | 有正文 | `src/ui/WindowRuntime.cpp:15878 — Sub_4722E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1120 | 0x004722F0 | `sub_4722F0` | 有正文 | `src/ui/WindowRuntime.cpp:15891 — Sub_4722F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1121 | 0x004723D0 | `sub_4723D0` | 有正文 | `src/ui/WindowRuntime.cpp:42099 — Sub_4723D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1122 | 0x004729B0 | `Window::DrawCurrentWindow` | 有正文 | `src/ui/WindowRuntime.cpp:46375 — DrawCurrentWindow_4729B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1123 | 0x00472D20 | `Window::ProsseFocus` | 有正文 | `src/ui/WindowRuntime.cpp:41073 — Sub_472D20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1124 | 0x00472F00 | `Mouse::isInRect` | 有正文 | `src/ui/WindowRuntime.cpp:40933 — Sub_472F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1125 | 0x00472F40 | `sub_472F40` | 有正文 | `src/world/SceneState.cpp:2883 — Sub_472F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1126 | 0x00472F70 | `sub_472F70` | 有正文 | `src/world/SceneState.cpp:2894 — Sub_472F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1127 | 0x00472FA0 | `sub_472FA0` | 有正文 | `src/world/SceneState.cpp:2900 — Sub_472FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1128 | 0x00472FD0 | `sub_472FD0` | 有正文 | `src/world/SceneState.cpp:2906 — Sub_472FD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1129 | 0x00473000 | `sub_473000` | 有正文 | `src/world/SceneState.cpp:2912 — Sub_473000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1130 | 0x00473030 | `sub_473030` | 有正文 | `src/world/SceneState.cpp:2918 — Sub_473030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1131 | 0x00473080 | `sub_473080` | 有正文 | `src/ui/WindowRuntime.cpp:42074 — Sub_473080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1132 | 0x004730A0 | `Window::sortLayer` | 有正文 | `src/ui/WindowRuntime.cpp:45068 — Sub_4730A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1133 | 0x004731B0 | `sub_4731B0` | 有正文 | `src/ui/WindowRuntime.cpp:42085 — Sub_4731B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1134 | 0x004731E0 | `sub_4731E0` | 有正文 | `src/ui/WindowRuntime.cpp:42289 — Sub_4731E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1135 | 0x00473560 | `Window::ProcessChildElement` | 有正文 | `src/ui/WindowRuntime.cpp:45168 — Sub_473560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1136 | 0x00473840 | `sub_473840` | 有正文 | `src/ui/WindowRuntime.cpp:41161 — Sub_473840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1137 | 0x00473E40 | `MouseEvent::Update` | 有正文 | `src/ui/WindowRuntime.cpp:45117 — Sub_473E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1138 | 0x00473F80 | `Windows::Update` | 有正文 | `src/ui/WindowRuntime.cpp:45427 — UpdateWindows_473F80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1139 | 0x004740F0 | `sub_4740F0` | 有正文 | `src/ui/WindowRuntime.cpp:45352 — Sub_4740F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1140 | 0x00474170 | `sub_474170` | 有正文 | `src/ui/WindowRuntime.cpp:5699 — Sub_474170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1141 | 0x004741A0 | `sub_4741A0` | 有正文 | `src/ui/WindowRuntime.cpp:45238 — Sub_4741A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1142 | 0x00474270 | `sub_474270` | 有正文 | `src/ui/WindowRuntime.cpp:40636 — Sub_474270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1143 | 0x004742B0 | `sub_4742B0` | 有正文 | `src/ui/WindowRuntime.cpp:25795 — Sub_4742B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1144 | 0x004742C0 | `sub_4742C0` | 有正文 | `src/ui/WindowRuntime.cpp:40652 — Sub_4742C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1145 | 0x00474310 | `sub_474310` | 有正文 | `src/ui/WindowRuntime.cpp:42612 — Sub_474310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1146 | 0x00474360 | `sub_474360` | 有正文 | `src/ui/WindowRuntime.cpp:42790 — Sub_474360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1147 | 0x004743B0 | `sub_4743B0` | 有正文 | `src/ui/WindowRuntime.cpp:40675 — Sub_4743B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1148 | 0x004744A0 | `sub_4744A0` | 有正文 | `src/ui/WindowRuntime.cpp:45053 — Sub_4744A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1149 | 0x004744F0 | `sub_4744F0` | 有正文 | `src/ui/WindowRuntime.cpp:44915 — Sub_4744F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1150 | 0x00474540 | `?classic@locale@std@@SAABV12@XZ_2` | 有正文 | `src/ui/WindowRuntime.cpp:42605 — Sub_474540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1151 | 0x00474550 | `sub_474550` | 有正文 | `src/ui/WindowRuntime.cpp:7713 — Sub_474550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1152 | 0x00474FB0 | `sub_474FB0` | 有正文 | `src/ui/WindowRuntime.cpp:42463 — Sub_474FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1153 | 0x004758D0 | `YobiWindow::Update` | 有正文 | `src/YobiRuntime.cpp:1881 — Sub_4758D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1154 | 0x00475950 | `Yobi::newWindow` | 有正文 | `src/YobiRuntime.cpp:1912 — Sub_475950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1155 | 0x00475AA0 | `Yobi::newWindow2` | 有正文 | `src/ui/WindowRuntime.cpp:45700 — CreateWindowAnimation_475AA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1156 | 0x00475C10 | `sub_475C10` | 有正文 | `src/ui/WindowRuntime.cpp:15969 — RestoreWindowPositions_475C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1157 | 0x00475D40 | `Window::GetFocusWinId` | 有正文 | `src/ui/WindowRuntime.cpp:40948 — Sub_475D40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1158 | 0x00475D50 | `sub_475D50` | 有正文 | `src/ui/WindowRuntime.cpp:40954 — Sub_475D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1159 | 0x00475D60 | `sub_475D60` | 有正文 | `src/ui/WindowRuntime.cpp:40960 — Sub_475D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1160 | 0x00475D70 | `sub_475D70` | 有正文 | `src/ui/WindowRuntime.cpp:40966 — Sub_475D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1161 | 0x00475D80 | `sub_475D80` | 有正文 | `src/ui/WindowRuntime.cpp:40972 — Sub_475D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1162 | 0x00475D90 | `sub_475D90` | 有正文 | `src/ui/WindowRuntime.cpp:40978 — Sub_475D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1163 | 0x00475DA0 | `sub_475DA0` | 有正文 | `src/ui/WindowRuntime.cpp:40985 — Sub_475DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1164 | 0x00475DF0 | `sub_475DF0` | 有正文 | `src/ui/WindowRuntime.cpp:42783 — Sub_475DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1165 | 0x00475E00 | `sub_475E00` | 有正文 | `src/ui/WindowRuntime.cpp:42626 — Sub_475E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1166 | 0x004763B0 | `sub_4763B0` | 有正文 | `src/ui/WindowRuntime.cpp:41048 — Sub_4763B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1167 | 0x004763E0 | `MouseEvent::reset` | 有正文 | `src/ui/WindowRuntime.cpp:41061 — Sub_4763E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1168 | 0x00476410 | `sub_476410` | 有正文 | `src/ui/WindowRuntime.cpp:46151 — CancelWindowDrag_476410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1169 | 0x00476490 | `MouseEvent::setupEventCallback` | 有正文 | `src/ui/WindowRuntime.cpp:41004 — Sub_476490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1170 | 0x00476500 | `InvDragReset` | 有正文 | `src/ui/WindowRuntime.cpp:41026 — Sub_476500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1171 | 0x00476570 | `sub_476570` | 有正文 | `src/ui/WindowRuntime.cpp:45292 — Sub_476570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1172 | 0x00476680 | `sub_476680` | 有正文 | `src/world/SceneState.cpp:2458 — Sub_476680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1173 | 0x00476740 | `sub_476740` | 有正文 | `src/Input.cpp:634 — Sub_476740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1174 | 0x004767E0 | `sub_4767E0` | 有正文 | `src/ui/WindowRuntime.cpp:42719 — Sub_4767E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1175 | 0x00477160 | `sub_477160` | 有正文 | `src/ui/WindowRuntime.cpp:46084 — InitializeLegacyNumberSpacing_477160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1176 | 0x00477170 | `sub_477170` | 有正文 | `src/ui/WindowRuntime.cpp:46091 — InitializeNumberSpacing_477170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1177 | 0x004771B0 | `sub_4771B0` | 有正文 | `src/ui/WindowRuntime.cpp:31191 — Sub_4771B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1178 | 0x00477460 | `sub_477460` | 有正文 | `src/ui/WindowRuntime.cpp:31258 — Sub_477460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1179 | 0x00477490 | `Window::FindByWinId` | 有正文 | `src/ui/WindowRuntime.cpp:30432 — FindWindowById_477490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1180 | 0x004774C0 | `sub_4774C0` | 有正文 | `src/ui/WindowRuntime.cpp:45034 — Sub_4774C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1181 | 0x0047752C | `?classic@locale@std@@SAABV12@XZ_4` | 有承接 | `src/ui/WindowRuntime.cpp:45034 — Sub_4774C0` | 原共享尾为调用 478010 后返回 1；并入实际父正文；完整业务／运行未验 |
| 1182 | 0x00477540 | `sub_477540` | 有正文 | `src/ui/WindowRuntime.cpp:44500 — Sub_477540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1183 | 0x00477600 | `sub_477600` | 有正文 | `src/ui/WindowRuntime.cpp:44525 — Sub_477600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1184 | 0x00477710 | `sub_477710` | 有正文 | `src/ui/WindowRuntime.cpp:44559 — Sub_477710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1185 | 0x00478010 | `sub_478010` | 有正文 | `src/ui/WindowRuntime.cpp:44978 — Sub_478010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1186 | 0x00478320 | `?classic@locale@std@@SAABV12@XZ_5` | 有正文 | `src/ui/WindowRuntime.cpp:44908 — Sub_478320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1187 | 0x00478330 | `sub_478330` | 有正文 | `src/ui/WindowRuntime.cpp:5280 — Sub_478330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1188 | 0x00478A10 | `sub_478A10` | 有正文 | `src/ui/WindowRuntime.cpp:44815 — Sub_478A10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1189 | 0x004792D0 | `sub_4792D0` | 有正文 | `src/ui/WindowRuntime.cpp:44929 — Sub_4792D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1190 | 0x00479610 | `sub_479610` | 有正文 | `src/ui/WindowRuntime.cpp:15952 — Sub_479610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1191 | 0x00479640 | `sub_479640` | 有正文 | `src/ui/WindowRuntime.cpp:15934 — Sub_479640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1192 | 0x00479650 | `___inc_tmpoff` | 有正文 | `src/ui/WindowRuntime.cpp:39460 — Sub_479650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1193 | 0x00479660 | `sub_479660` | 有正文 | `src/ui/WindowRuntime.cpp:39466 — Sub_479660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1194 | 0x004796C0 | `sub_4796C0` | 有正文 | `src/ui/WindowRuntime.cpp:15940 — Sub_4796C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1195 | 0x004796D0 | `sub_4796D0` | 有正文 | `src/ui/WindowRuntime.cpp:34526 — Sub_4796D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1196 | 0x004796E0 | `sub_4796E0` | 有正文 | `src/ui/WindowRuntime.cpp:34746 — Sub_4796E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1197 | 0x004796F0 | `sub_4796F0` | 有正文 | `src/ui/WindowRuntime.cpp:15946 — Sub_4796F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1198 | 0x00479700 | `sub_479700` | 有正文 | `src/ui/WindowRuntime.cpp:35986 — Sub_479700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1199 | 0x004797A0 | `sub_4797A0` | 有正文 | `src/ui/WindowRuntime.cpp:36013 — Sub_4797A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1200 | 0x004797C0 | `sub_4797C0` | 有正文 | `src/ui/WindowRuntime.cpp:2027 — Sub_4797C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1201 | 0x00479820 | `sub_479820` | 有正文 | `src/ui/WindowRuntime.cpp:35583 — Sub_479820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1202 | 0x00479860 | `GetPetCrystalLv` | 有正文 | `src/ui/WindowRuntime.cpp:35597 — Sub_479860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1203 | 0x00479920 | `sub_479920` | 有正文 | `src/ui/WindowRuntime.cpp:35614 — Sub_479920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1204 | 0x004799D0 | `sub_4799D0` | 有正文 | `src/ui/WindowRuntime.cpp:35631 — Sub_4799D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1205 | 0x00479A00 | `sub_479A00` | 有正文 | `src/ui/WindowRuntime.cpp:35638 — Sub_479A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1206 | 0x00479A70 | `sub_479A70` | 有正文 | `src/ui/WindowRuntime.cpp:35656 — Sub_479A70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1207 | 0x00479AB0 | `sub_479AB0` | 有正文 | `src/ui/WindowRuntime.cpp:35667 — Sub_479AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1208 | 0x00479B00 | `sub_479B00` | 有正文 | `src/ui/WindowRuntime.cpp:35683 — Sub_479B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1209 | 0x00479B80 | `sub_479B80` | 有正文 | `src/ui/WindowRuntime.cpp:36076 — Sub_479B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1210 | 0x00479C80 | `sub_479C80` | 有正文 | `src/ui/WindowRuntime.cpp:36213 — Sub_479C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1211 | 0x00479DA0 | `sub_479DA0` | 有正文 | `src/ui/WindowRuntime.cpp:39195 — Sub_479DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1212 | 0x00479EA0 | `sub_479EA0` | 有正文 | `src/ui/WindowRuntime.cpp:36265 — Sub_479EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1213 | 0x0047A000 | `sub_47A000` | 有正文 | `src/ui/WindowRuntime.cpp:36323 — Sub_47A000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1214 | 0x0047A140 | `sub_47A140` | 有正文 | `src/ui/WindowRuntime.cpp:39445 — Sub_47A140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1215 | 0x0047A1A0 | `sub_47A1A0` | 有正文 | `src/ui/WindowRuntime.cpp:38529 — Sub_47A1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1216 | 0x0047A320 | `sub_47A320` | 有正文 | `src/ui/WindowRuntime.cpp:38576 — Sub_47A320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1217 | 0x0047A490 | `sub_47A490` | 有正文 | `src/ui/WindowRuntime.cpp:38289 — Sub_47A490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1218 | 0x0047A4C0 | `sub_47A4C0` | 有正文 | `src/ui/WindowRuntime.cpp:38302 — Sub_47A4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1219 | 0x0047A500 | `BattleScene::SelectionCallback2` | 有正文 | `src/ui/WindowRuntime.cpp:38314 — Sub_47A500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1220 | 0x0047A540 | `BattleScene::OpenSkillWindow` | 有正文 | `src/ui/WindowRuntime.cpp:38623 — Sub_47A540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1221 | 0x0047A580 | `BattleScene::OpenSkillWindow2` | 有正文 | `src/ui/WindowRuntime.cpp:38635 — Sub_47A580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1222 | 0x0047A5C0 | `sub_47A5C0` | 有正文 | `src/ui/WindowRuntime.cpp:38693 — Sub_47A5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1223 | 0x0047A620 | `sub_47A620` | 有正文 | `src/ui/WindowRuntime.cpp:38830 — Sub_47A620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1224 | 0x0047A7A0 | `sub_47A7A0` | 有正文 | `src/ui/WindowRuntime.cpp:38885 — Sub_47A7A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1225 | 0x0047B110 | `sub_47B110` | 有正文 | `src/ui/WindowRuntime.cpp:38647 — Sub_47B110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1226 | 0x0047B210 | `sub_47B210` | 有正文 | `src/ui/WindowRuntime.cpp:38716 — Sub_47B210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1227 | 0x0047B3D0 | `sub_47B3D0` | 有正文 | `src/ui/WindowRuntime.cpp:39067 — Sub_47B3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1228 | 0x0047B470 | `sub_47B470` | 有正文 | `src/ui/WindowRuntime.cpp:39095 — Sub_47B470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1229 | 0x0047B520 | `sub_47B520` | 有正文 | `src/ui/WindowRuntime.cpp:39125 — Sub_47B520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1230 | 0x0047B5D0 | `sub_47B5D0` | 有正文 | `src/ui/WindowRuntime.cpp:39152 — Sub_47B5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1231 | 0x0047B680 | `sub_47B680` | 有正文 | `src/ui/WindowRuntime.cpp:39182 — Sub_47B680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1232 | 0x0047B6C0 | `sub_47B6C0` | 有正文 | `src/ui/WindowRuntime.cpp:35953 — Sub_47B6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1233 | 0x0047B820 | `sub_47B820` | 有正文 | `src/ui/WindowRuntime.cpp:36019 — Sub_47B820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1234 | 0x0047B9B0 | `sub_47B9B0` | 有正文 | `src/ui/WindowRuntime.cpp:36124 — Sub_47B9B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1235 | 0x0047BC00 | `sub_47BC00` | 有正文 | `src/ui/WindowRuntime.cpp:39282 — Sub_47BC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1236 | 0x0047C240 | `sub_47C240` | 有正文 | `src/ui/WindowRuntime.cpp:36182 — Sub_47C240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1237 | 0x0047C3A0 | `sub_47C3A0` | 有正文 | `src/ui/WindowRuntime.cpp:39226 — Sub_47C3A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1238 | 0x0047C570 | `sub_47C570` | 有正文 | `src/ui/WindowRuntime.cpp:36369 — Sub_47C570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1239 | 0x0047C700 | `sub_47C700` | 有正文 | `src/ui/WindowRuntime.cpp:36431 — Sub_47C700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1240 | 0x0047C950 | `sub_47C950` | 有正文 | `src/ui/WindowRuntime.cpp:38766 — Sub_47C950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1241 | 0x0047CB00 | `sub_47CB00` | 有正文 | `src/ui/WindowRuntime.cpp:38328 — Sub_47CB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1242 | 0x0047CC80 | `sub_47CC80` | 有正文 | `src/ui/WindowRuntime.cpp:38404 — Sub_47CC80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1243 | 0x0047CE70 | `sub_47CE70` | 有正文 | `src/ui/WindowRuntime.cpp:38367 — Sub_47CE70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1244 | 0x0047CFE0 | `sub_47CFE0` | 有正文 | `src/ui/WindowRuntime.cpp:38469 — Sub_47CFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1245 | 0x0047D170 | `sub_47D170` | 有正文 | `src/ui/WindowRuntime.cpp:34036 — Sub_47D170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1246 | 0x0047D220 | `sub_47D220` | 有正文 | `src/ui/WindowRuntime.cpp:34060 — Sub_47D220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1247 | 0x0047D280 | `sub_47D280` | 有正文 | `src/ui/WindowRuntime.cpp:35222 — Sub_47D280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1248 | 0x0047D2E0 | `sub_47D2E0` | 有正文 | `src/ui/WindowRuntime.cpp:34711 — Sub_47D2E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1249 | 0x0047D490 | `sub_47D490` | 有正文 | `src/ui/WindowRuntime.cpp:5542 — Sub_47D490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1250 | 0x0047D4A0 | `sub_47D4A0` | 有正文 | `src/ui/WindowRuntime.cpp:28205 — Sub_47D4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1251 | 0x0047D4B0 | `sub_47D4B0` | 有正文 | `src/ui/WindowRuntime.cpp:33594 — Sub_47D4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1252 | 0x0047D4F0 | `sub_47D4F0` | 有正文 | `src/ui/WindowRuntime.cpp:33603 — Sub_47D4F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1253 | 0x0047D530 | `sub_47D530` | 有正文 | `src/ui/WindowRuntime.cpp:33614 — Sub_47D530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1254 | 0x0047D5B0 | `sub_47D5B0` | 有正文 | `src/ui/WindowRuntime.cpp:33632 — Sub_47D5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1255 | 0x0047D5F0 | `sub_47D5F0` | 有正文 | `src/ui/WindowRuntime.cpp:31706 — Sub_47D5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1256 | 0x0047D780 | `sub_47D780` | 有正文 | `src/ui/WindowRuntime.cpp:31730 — Sub_47D780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1257 | 0x0047D8F0 | `sub_47D8F0` | 有正文 | `src/ui/WindowRuntime.cpp:31750 — Sub_47D8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1258 | 0x0047DA70 | `sub_47DA70` | 有正文 | `src/ui/WindowRuntime.cpp:31774 — Sub_47DA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1259 | 0x0047DB50 | `sub_47DB50` | 有正文 | `src/ui/WindowRuntime.cpp:31795 — Sub_47DB50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1260 | 0x0047DC20 | `sub_47DC20` | 有正文 | `src/ui/WindowRuntime.cpp:1923 — Sub_47DC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1261 | 0x0047DCD0 | `sub_47DCD0` | 有正文 | `src/ui/WindowRuntime.cpp:1961 — Sub_47DCD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1262 | 0x0047DE00 | `sub_47DE00` | 有正文 | `src/ui/WindowRuntime.cpp:32431 — Sub_47DE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1263 | 0x0047DF30 | `sub_47DF30` | 有正文 | `src/ui/WindowRuntime.cpp:32469 — Sub_47DF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1264 | 0x0047DF50 | `sub_47DF50` | 有正文 | `src/ui/WindowRuntime.cpp:32201 — Sub_47DF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1265 | 0x0047E1B0 | `sub_47E1B0` | 有正文 | `src/ui/WindowRuntime.cpp:31504 — Sub_47E1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1266 | 0x0047E5E0 | `sub_47E5E0` | 有正文 | `src/ui/WindowRuntime.cpp:34127 — Sub_47E5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1267 | 0x0047EBE0 | `sub_47EBE0` | 有正文 | `src/ui/WindowRuntime.cpp:34224 — Sub_47EBE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1268 | 0x0047EC00 | `sub_47EC00` | 有正文 | `src/ui/WindowRuntime.cpp:33521 — Sub_47EC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1269 | 0x0047EF00 | `sub_47EF00` | 有正文 | `src/ui/WindowRuntime.cpp:31541 — Sub_47EF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1270 | 0x0047F3D0 | `sub_47F3D0` | 有正文 | `src/ui/WindowRuntime.cpp:34345 — Sub_47F3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1271 | 0x0047FAC0 | `sub_47FAC0` | 有正文 | `src/ui/WindowRuntime.cpp:34460 — Sub_47FAC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1272 | 0x0047FAE0 | `sub_47FAE0` | 有正文 | `src/ui/WindowRuntime.cpp:31814 — Sub_47FAE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1273 | 0x0047FD00 | `sub_47FD00` | 有正文 | `src/ui/WindowRuntime.cpp:31832 — Sub_47FD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1274 | 0x0047FF20 | `sub_47FF20` | 有正文 | `src/ui/WindowRuntime.cpp:31850 — Sub_47FF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1275 | 0x004801B0 | `sub_4801B0` | 有正文 | `src/ui/WindowRuntime.cpp:31599 — Sub_4801B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1276 | 0x00480600 | `sub_480600` | 有正文 | `src/ui/WindowRuntime.cpp:31637 — Sub_480600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1277 | 0x00480A00 | `sub_480A00` | 有正文 | `src/ui/WindowRuntime.cpp:31867 — Sub_480A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1278 | 0x00480C20 | `sub_480C20` | 有正文 | `src/ui/WindowRuntime.cpp:31673 — Sub_480C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1279 | 0x00481010 | `sub_481010` | 有正文 | `src/ui/WindowRuntime.cpp:33964 — Sub_481010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1280 | 0x004810A0 | `sub_4810A0` | 有正文 | `src/ui/WindowRuntime.cpp:33644 — Sub_4810A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1281 | 0x00482000 | `sub_482000` | 有正文 | `src/ui/WindowRuntime.cpp:33988 — Sub_482000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1282 | 0x00482270 | `sub_482270` | 有正文 | `src/ui/WindowRuntime.cpp:32487 — Sub_482270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1283 | 0x00482F70 | `sub_482F70` | 有正文 | `src/ui/WindowRuntime.cpp:32475 — Sub_482F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1284 | 0x00482FA0 | `sub_482FA0` | 有正文 | `src/ui/WindowRuntime.cpp:32797 — Sub_482FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1285 | 0x004830B0 | `sub_4830B0` | 有正文 | `src/ui/WindowRuntime.cpp:32845 — Sub_4830B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1286 | 0x00483150 | `sub_483150` | 有正文 | `src/ui/WindowRuntime.cpp:32868 — Sub_483150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1287 | 0x00483E30 | `sub_483E30` | 有正文 | `src/ui/WindowRuntime.cpp:32275 — Sub_483E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1288 | 0x00483EB0 | `sub_483EB0` | 有正文 | `src/ui/WindowRuntime.cpp:31886 — Sub_483EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1289 | 0x004847C0 | `sub_4847C0` | 有正文 | `src/ui/WindowRuntime.cpp:33357 — Sub_4847C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1290 | 0x00484D60 | `sub_484D60` | 有正文 | `src/ui/WindowRuntime.cpp:34079 — Sub_484D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1291 | 0x00484E90 | `sub_484E90` | 有正文 | `src/ui/WindowRuntime.cpp:34298 — Sub_484E90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1292 | 0x00484FB0 | `sub_484FB0` | 有正文 | `src/ui/WindowRuntime.cpp:33237 — Sub_484FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1293 | 0x00485110 | `sub_485110` | 有正文 | `src/ui/WindowRuntime.cpp:40892 — Sub_485110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1294 | 0x00485160 | `sub_485160` | 有正文 | `src/ui/WindowRuntime.cpp:33280 — Sub_485160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1295 | 0x00485200 | `sub_485200` | 有正文 | `src/ui/WindowRuntime.cpp:33340 — Sub_485200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1296 | 0x00485350 | `sub_485350` | 有正文 | `src/ui/WindowRuntime.cpp:33349 — Sub_485350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1297 | 0x004854A0 | `sub_4854A0` | 有正文 | `src/ui/WindowRuntime.cpp:34532 — Sub_4854A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1298 | 0x00485700 | `sub_485700` | 有正文 | `src/ui/WindowRuntime.cpp:34627 — Sub_485700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1299 | 0x00485890 | `sub_485890` | 有正文 | `src/ui/WindowRuntime.cpp:34656 — Sub_485890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1300 | 0x00485AF0 | `sub_485AF0` | 有正文 | `src/ui/WindowRuntime.cpp:34752 — Sub_485AF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1301 | 0x00486020 | `sub_486020` | 有正文 | `src/ui/WindowRuntime.cpp:34836 — Sub_486020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1302 | 0x00486040 | `sub_486040` | 有正文 | `src/ui/WindowRuntime.cpp:34905 — Sub_486040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1303 | 0x00486CF0 | `sub_486CF0` | 有正文 | `src/ui/WindowRuntime.cpp:35129 — Sub_486CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1304 | 0x00486E50 | `sub_486E50` | 有正文 | `src/ui/WindowRuntime.cpp:35168 — Sub_486E50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1305 | 0x00486F20 | `sub_486F20` | 有正文 | `src/ui/WindowRuntime.cpp:35199 — Sub_486F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1306 | 0x00486FE0 | `sub_486FE0` | 有正文 | `src/ui/WindowRuntime.cpp:35704 — Sub_486FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1307 | 0x00487020 | `sub_487020` | 有正文 | `src/ui/WindowRuntime.cpp:39485 — Sub_487020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1308 | 0x004871B0 | `sub_4871B0` | 有正文 | `src/ui/WindowRuntime.cpp:39535 — Sub_4871B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1309 | 0x004872E0 | `sub_4872E0` | 有正文 | `src/ui/WindowRuntime.cpp:35713 — Sub_4872E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1310 | 0x004874B0 | `sub_4874B0` | 有正文 | `src/ui/WindowRuntime.cpp:35761 — Sub_4874B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1311 | 0x004875E0 | `sub_4875E0` | 有正文 | `src/ui/WindowRuntime.cpp:35262 — Sub_4875E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1312 | 0x00487660 | `sub_487660` | 有正文 | `src/ui/WindowRuntime.cpp:35283 — Sub_487660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1313 | 0x00487AE0 | `sub_487AE0` | 有正文 | `src/ui/WindowRuntime.cpp:35381 — Sub_487AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1314 | 0x00487B80 | `sub_487B80` | 有正文 | `src/ui/WindowRuntime.cpp:35401 — Sub_487B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1315 | 0x00487D60 | `sub_487D60` | 有正文 | `src/ui/WindowRuntime.cpp:35431 — Sub_487D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1316 | 0x00487E80 | `sub_487E80` | 有正文 | `src/ui/WindowRuntime.cpp:35467 — Sub_487E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1317 | 0x00487F30 | `sub_487F30` | 有正文 | `src/ui/WindowRuntime.cpp:35238 — Sub_487F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1318 | 0x004880D0 | `sub_4880D0` | 有正文 | `src/ui/WindowRuntime.cpp:32291 — Sub_4880D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1319 | 0x00488160 | `sub_488160` | 有正文 | `src/ui/WindowRuntime.cpp:32311 — Sub_488160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1320 | 0x00488180 | `sub_488180` | 有正文 | `src/ui/WindowRuntime.cpp:32325 — Sub_488180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1321 | 0x004881D0 | `sub_4881D0` | 有正文 | `src/ui/WindowRuntime.cpp:32338 — Sub_4881D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1322 | 0x004883F0 | `sub_4883F0` | 有正文 | `src/ui/WindowRuntime.cpp:46040 — InitializeLegacyFirstItemSelector_4883F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1323 | 0x00488400 | `unknown_libname_44` | 有正文 | `src/ui/WindowRuntime.cpp:46011 — ConstructLegacyFirstItemSelector_488400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1324 | 0x00488420 | `sub_488420` | 有正文 | `src/ui/WindowRuntime.cpp:46018 — ReleaseLegacyFirstItemSelectorAtExit_488420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1325 | 0x00488510 | `sub_488510` | 有正文 | `src/ui/WindowRuntime.cpp:8985 — Sub_488510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1326 | 0x004885A0 | `sub_4885A0` | 有正文 | `src/ui/WindowRuntime.cpp:5709 — Sub_4885A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1327 | 0x004886E0 | `sub_4886E0` | 有正文 | `src/ui/WindowRuntime.cpp:5744 — Sub_4886E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1328 | 0x00488800 | `sub_488800` | 有正文 | `src/ui/WindowRuntime.cpp:5786 — Sub_488800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1329 | 0x004888C0 | `sub_4888C0` | 有正文 | `src/ui/WindowRuntime.cpp:8907 — Sub_4888C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1330 | 0x00488930 | `sub_488930` | 有正文 | `src/ui/WindowRuntime.cpp:8922 — Sub_488930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1331 | 0x00488950 | `sub_488950` | 有正文 | `src/ui/WindowRuntime.cpp:8928 — Sub_488950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1332 | 0x00488970 | `sub_488970` | 有正文 | `src/ui/WindowRuntime.cpp:8934 — Sub_488970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1333 | 0x00488990 | `sub_488990` | 有正文 | `src/ui/WindowRuntime.cpp:9009 — Sub_488990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1334 | 0x00488B10 | `sub_488B10` | 有正文 | `src/ui/WindowRuntime.cpp:9064 — Sub_488B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1335 | 0x00488C50 | `sub_488C50` | 有正文 | `src/ui/WindowRuntime.cpp:9114 — Sub_488C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1336 | 0x00488CF0 | `sub_488CF0` | 有正文 | `src/ui/WindowRuntime.cpp:9133 — Sub_488CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1337 | 0x00488D30 | `sub_488D30` | 有正文 | `src/ui/WindowRuntime.cpp:9144 — Sub_488D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1338 | 0x00488E60 | `sub_488E60` | 有正文 | `src/ui/WindowRuntime.cpp:9182 — Sub_488E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1339 | 0x00488E80 | `sub_488E80` | 有正文 | `src/ui/WindowRuntime.cpp:9189 — Sub_488E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1340 | 0x00488EA0 | `sub_488EA0` | 有正文 | `src/ui/WindowRuntime.cpp:8941 — Sub_488EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1341 | 0x00488EC0 | `sub_488EC0` | 有正文 | `src/ui/WindowRuntime.cpp:8948 — Sub_488EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1342 | 0x00488FB0 | `sub_488FB0` | 有正文 | `src/ui/WindowRuntime.cpp:11511 — Sub_488FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1343 | 0x00489760 | `sub_489760` | 有正文 | `src/ui/WindowRuntime.cpp:11710 — Sub_489760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1344 | 0x00489EE0 | `sub_489EE0` | 有正文 | `src/ui/WindowRuntime.cpp:12084 — Sub_489EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1345 | 0x00489F00 | `sub_489F00` | 有正文 | `src/ui/WindowRuntime.cpp:12091 — Sub_489F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1346 | 0x0048A6C0 | `sub_48A6C0` | 有正文 | `src/ui/WindowRuntime.cpp:8862 — Sub_48A6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1347 | 0x0048A830 | `sub_48A830` | 有正文 | `src/ui/WindowRuntime.cpp:5548 — Sub_48A830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1348 | 0x0048A8C0 | `sub_48A8C0` | 有正文 | `src/ui/WindowRuntime.cpp:5572 — Sub_48A8C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1349 | 0x0048A900 | `sub_48A900` | 有正文 | `src/ui/WindowRuntime.cpp:5586 — Sub_48A900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1350 | 0x0048A950 | `sub_48A950` | 有正文 | `src/ui/WindowRuntime.cpp:5599 — Sub_48A950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1351 | 0x0048AA60 | `sub_48AA60` | 有正文 | `src/ui/WindowRuntime.cpp:5642 — Sub_48AA60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1352 | 0x0048AB10 | `sub_48AB10` | 有正文 | `src/ui/WindowRuntime.cpp:5671 — Sub_48AB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1353 | 0x0048ABC0 | `sub_48ABC0` | 有正文 | `src/YobiRuntime.cpp:1966 — Sub_48ABC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1354 | 0x0048AC60 | `sub_48AC60` | 有正文 | `src/YobiRuntime.cpp:2003 — Sub_48AC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1355 | 0x0048AEA0 | `sub_48AEA0` | 有正文 | `src/ui/WindowRuntime.cpp:39643 — Sub_48AEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1356 | 0x0048AF00 | `sub_48AF00` | 有正文 | `src/ui/WindowRuntime.cpp:39664 — Sub_48AF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1357 | 0x0048B010 | `sub_48B010` | 有正文 | `src/ui/WindowRuntime.cpp:39749 — Sub_48B010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1358 | 0x0048B0A0 | `sub_48B0A0` | 有正文 | `src/ui/WindowRuntime.cpp:39776 — Sub_48B0A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1359 | 0x0048B1A0 | `sub_48B1A0` | 有正文 | `src/ui/WindowRuntime.cpp:12244 — Sub_48B1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1360 | 0x0048B350 | `sub_48B350` | 有正文 | `src/ui/WindowRuntime.cpp:12287 — Sub_48B350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1361 | 0x0048B450 | `sub_48B450` | 有正文 | `src/ui/WindowRuntime.cpp:8044 — Sub_48B450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1362 | 0x0048B560 | `sub_48B560` | 有正文 | `src/ui/WindowRuntime.cpp:12316 — Sub_48B560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1363 | 0x0048B750 | `sub_48B750` | 有正文 | `src/ui/WindowRuntime.cpp:12363 — Sub_48B750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1364 | 0x0048B810 | `sub_48B810` | 有正文 | `src/ui/WindowRuntime.cpp:7952 — Sub_48B810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1365 | 0x0048B930 | `sub_48B930` | 有正文 | `src/ui/WindowRuntime.cpp:7998 — Sub_48B930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1366 | 0x0048BA60 | `sub_48BA60` | 有正文 | `src/ui/WindowRuntime.cpp:2044 — Sub_48BA60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1367 | 0x0048BB30 | `sub_48BB30` | 有正文 | `src/ui/WindowRuntime.cpp:5513 — Sub_48BB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1368 | 0x0048BB70 | `sub_48BB70` | 有正文 | `src/ui/WindowRuntime.cpp:5525 — Sub_48BB70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1369 | 0x0048BB90 | `sub_48BB90` | 有正文 | `src/ui/WindowRuntime.cpp:5817 — Sub_48BB90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1370 | 0x0048BC60 | `sub_48BC60` | 有正文 | `src/ui/WindowRuntime.cpp:5531 — Sub_48BC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1371 | 0x0048BC90 | `sub_48BC90` | 有正文 | `src/ui/WindowRuntime.cpp:6584 — Sub_48BC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1372 | 0x0048BF10 | `sub_48BF10` | 有正文 | `src/ui/WindowRuntime.cpp:6674 — Sub_48BF10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1373 | 0x0048BF30 | `sub_48BF30` | 有正文 | `src/ui/WindowRuntime.cpp:6495 — Sub_48BF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1374 | 0x0048C130 | `sub_48C130` | 有正文 | `src/ui/WindowRuntime.cpp:6576 — Sub_48C130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1375 | 0x0048C150 | `sub_48C150` | 有正文 | `src/ui/WindowRuntime.cpp:25776 — Sub_48C150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1376 | 0x0048C1B0 | `sub_48C1B0` | 有正文 | `src/ui/WindowRuntime.cpp:43254 — Sub_48C1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1377 | 0x0048C1CF | `sub_48C1CF` | 有承接 | `src/ui/WindowRuntime.cpp:43254 — Sub_48C1B0` | 原共享尾为调用 48C920 后返回 1；并入实际父正文；完整业务／运行未验 |
| 1378 | 0x0048C1E0 | `sub_48C1E0` | 有正文 | `src/ui/WindowRuntime.cpp:42848 — Sub_48C1E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1379 | 0x0048C920 | `sub_48C920` | 有正文 | `src/ui/WindowRuntime.cpp:43134 — Sub_48C920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1380 | 0x0048D460 | `sub_48D460` | 有正文 | `src/ui/WindowRuntime.cpp:43071 — Sub_48D460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1381 | 0x0048D4E0 | `sub_48D4E0` | 有正文 | `src/ui/WindowRuntime.cpp:42833 — Sub_48D4E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1382 | 0x0048D540 | `sub_48D540` | 有正文 | `src/ui/WindowRuntime.cpp:6218 — Sub_48D540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1383 | 0x0048D9A0 | `sub_48D9A0` | 有正文 | `src/ui/WindowRuntime.cpp:43496 — Sub_48D9A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1384 | 0x0048DAB0 | `sub_48DAB0` | 有正文 | `src/ui/WindowRuntime.cpp:43659 — Sub_48DAB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1385 | 0x0048DBB0 | `sub_48DBB0` | 有正文 | `src/ui/WindowRuntime.cpp:43823 — Sub_48DBB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1386 | 0x0048DCB0 | `sub_48DCB0` | 有正文 | `src/ui/WindowRuntime.cpp:43858 — Sub_48DCB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1387 | 0x0048DCC0 | `sub_48DCC0` | 有正文 | `src/ui/WindowRuntime.cpp:43866 — Sub_48DCC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1388 | 0x0048DD00 | `sub_48DD00` | 有正文 | `src/ui/WindowRuntime.cpp:43878 — Sub_48DD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1389 | 0x0048DD40 | `sub_48DD40` | 有正文 | `src/ui/WindowRuntime.cpp:43890 — Sub_48DD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1390 | 0x0048DD80 | `sub_48DD80` | 有正文 | `src/ui/WindowRuntime.cpp:44060 — Sub_48DD80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1391 | 0x0048E430 | `sub_48E430` | 有正文 | `src/ui/WindowRuntime.cpp:44205 — Sub_48E430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1392 | 0x0048ECA0 | `sub_48ECA0` | 有正文 | `src/ui/WindowRuntime.cpp:43913 — Sub_48ECA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1393 | 0x0048F380 | `sub_48F380` | 有正文 | `src/ui/WindowRuntime.cpp:44352 — Sub_48F380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1394 | 0x0048F410 | `sub_48F410` | 有正文 | `src/ui/WindowRuntime.cpp:44374 — Sub_48F410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1395 | 0x0048F620 | `sub_48F620` | 有正文 | `src/ui/WindowRuntime.cpp:44418 — Sub_48F620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1396 | 0x0048F690 | `sub_48F690` | 有正文 | `src/ui/WindowRuntime.cpp:44440 — Sub_48F690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1397 | 0x0048F6C0 | `sub_48F6C0` | 有正文 | `src/ui/WindowRuntime.cpp:44448 — Sub_48F6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1398 | 0x0048F8F0 | `sub_48F8F0` | 有正文 | `src/ui/WindowRuntime.cpp:43332 — Sub_48F8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1399 | 0x00490780 | `sub_490780` | 有正文 | `src/ui/WindowRuntime.cpp:43537 — Sub_490780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1400 | 0x00491AA0 | `sub_491AA0` | 有正文 | `src/ui/WindowRuntime.cpp:43694 — Sub_491AA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1401 | 0x004920F0 | `sub_4920F0` | 有正文 | `src/ui/WindowRuntime.cpp:43784 — Sub_4920F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1402 | 0x00492250 | `sub_492250` | 有正文 | `src/world/SceneState.cpp:4373 — Sub_492250` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1403 | 0x00492260 | `sub_492260` | 有正文 | `src/ui/WindowRuntime.cpp:43274 — Sub_492260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1404 | 0x00492270 | `sub_492270` | 有正文 | `src/ui/WindowRuntime.cpp:43280 — Sub_492270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1405 | 0x00492280 | `sub_492280` | 有正文 | `src/ui/WindowRuntime.cpp:43286 — Sub_492280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1406 | 0x00492350 | `sub_492350` | 有正文 | `src/ui/WindowRuntime.cpp:43306 — Sub_492350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1407 | 0x00492390 | `sub_492390` | 有正文 | `src/ui/WindowRuntime.cpp:43318 — Sub_492390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1408 | 0x004923D0 | `sub_4923D0` | 有正文 | `src/ui/WindowRuntime.cpp:16471 — Sub_4923D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1409 | 0x004923F0 | `sub_4923F0` | 有正文 | `src/ui/WindowRuntime.cpp:16547 — Sub_4923F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1410 | 0x00492410 | `sub_492410` | 有正文 | `src/ui/WindowRuntime.cpp:16853 — Sub_492410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1411 | 0x00492430 | `sub_492430` | 有正文 | `src/ui/WindowRuntime.cpp:16557 — Sub_492430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1412 | 0x00492520 | `sub_492520` | 有正文 | `src/ui/WindowRuntime.cpp:16586 — Sub_492520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1413 | 0x00492840 | `sub_492840` | 有正文 | `src/ui/WindowRuntime.cpp:16668 — Sub_492840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1414 | 0x00492880 | `sub_492880` | 有正文 | `src/ui/WindowRuntime.cpp:16863 — Sub_492880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1415 | 0x00492C80 | `sub_492C80` | 有正文 | `src/ui/WindowRuntime.cpp:16481 — Sub_492C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1416 | 0x00492F70 | `sub_492F70` | 有正文 | `src/ui/WindowRuntime.cpp:16715 — Sub_492F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1417 | 0x004939A0 | `sub_4939A0` | 有正文 | `src/ui/WindowRuntime.cpp:16692 — Sub_4939A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1418 | 0x00493A70 | `sub_493A70` | 有正文 | `src/ui/WindowRuntime.cpp:16970 — Sub_493A70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1419 | 0x00493AF0 | `sub_493AF0` | 有正文 | `src/ui/WindowRuntime.cpp:16989 — Sub_493AF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1420 | 0x00493B70 | `sub_493B70` | 有正文 | `src/ui/WindowRuntime.cpp:17009 — Sub_493B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1421 | 0x00493CB0 | `sub_493CB0` | 有正文 | `src/ui/WindowRuntime.cpp:17043 — Sub_493CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1422 | 0x00493D60 | `sub_493D60` | 有正文 | `src/ui/WindowRuntime.cpp:17068 — Sub_493D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1423 | 0x00493DB0 | `sub_493DB0` | 有正文 | `src/ui/WindowRuntime.cpp:17082 — Sub_493DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1424 | 0x00493DF9 | `sub_493DF9` | 有承接 | `src/ui/WindowRuntime.cpp:17082 — Sub_493DB0` | 原绘图／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 1425 | 0x00493E10 | `sub_493E10` | 有正文 | `src/ui/WindowRuntime.cpp:17104 — Sub_493E10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1426 | 0x00493E50 | `sub_493E50` | 有正文 | `src/ui/WindowRuntime.cpp:17279 — Sub_493E50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1427 | 0x00493EE0 | `sub_493EE0` | 有正文 | `src/ui/WindowRuntime.cpp:17117 — Sub_493EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1428 | 0x00494000 | `sub_494000` | 有正文 | `src/ui/WindowRuntime.cpp:17150 — Sub_494000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1429 | 0x00494180 | `sub_494180` | 有正文 | `src/ui/WindowRuntime.cpp:17195 — Sub_494180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1430 | 0x00494370 | `sub_494370` | 有正文 | `src/ui/WindowRuntime.cpp:17241 — Sub_494370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1431 | 0x004944A0 | `sub_4944A0` | 有正文 | `src/ui/WindowRuntime.cpp:17302 — Sub_4944A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1432 | 0x00494550 | `sub_494550` | 有正文 | `src/ui/WindowRuntime.cpp:17328 — Sub_494550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1433 | 0x00494620 | `sub_494620` | 有正文 | `src/ui/WindowRuntime.cpp:17358 — Sub_494620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1434 | 0x00494720 | `sub_494720` | 有正文 | `src/ui/WindowRuntime.cpp:17395 — Sub_494720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1435 | 0x004947F0 | `sub_4947F0` | 有正文 | `src/ui/WindowRuntime.cpp:17424 — Sub_4947F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1436 | 0x004948F0 | `sub_4948F0` | 有正文 | `src/ui/WindowRuntime.cpp:17458 — Sub_4948F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1437 | 0x004949C0 | `sub_4949C0` | 有正文 | `src/ui/WindowRuntime.cpp:17484 — Sub_4949C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1438 | 0x00494AE0 | `sub_494AE0` | 有正文 | `src/ui/WindowRuntime.cpp:17521 — Sub_494AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1439 | 0x00494C10 | `sub_494C10` | 有正文 | `src/ui/WindowRuntime.cpp:17558 — Sub_494C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1440 | 0x00494D30 | `sub_494D30` | 有正文 | `src/ui/WindowRuntime.cpp:17595 — Sub_494D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1441 | 0x00494E60 | `sub_494E60` | 有正文 | `src/ui/WindowRuntime.cpp:17632 — Sub_494E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1442 | 0x00494F30 | `sub_494F30` | 有正文 | `src/ui/WindowRuntime.cpp:17662 — Sub_494F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1443 | 0x00495000 | `sub_495000` | 有正文 | `src/ui/WindowRuntime.cpp:17692 — Sub_495000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1444 | 0x00495130 | `sub_495130` | 有正文 | `src/ui/WindowRuntime.cpp:17898 — Sub_495130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1445 | 0x004951C0 | `sub_4951C0` | 有正文 | `src/ui/WindowRuntime.cpp:17918 — Sub_4951C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1446 | 0x004953B0 | `sub_4953B0` | 有正文 | `src/ui/WindowRuntime.cpp:17998 — Sub_4953B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1447 | 0x00495450 | `sub_495450` | 有正文 | `src/ui/WindowRuntime.cpp:18017 — Sub_495450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1448 | 0x00495500 | `sub_495500` | 有正文 | `src/ui/WindowRuntime.cpp:18049 — Sub_495500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1449 | 0x004955C0 | `sub_4955C0` | 有正文 | `src/ui/WindowRuntime.cpp:18084 — Sub_4955C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1450 | 0x00495670 | `sub_495670` | 有正文 | `src/ui/WindowRuntime.cpp:18119 — Sub_495670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1451 | 0x004957B0 | `sub_4957B0` | 有正文 | `src/ui/WindowRuntime.cpp:18174 — Sub_4957B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1452 | 0x004958D0 | `sub_4958D0` | 有正文 | `src/ui/WindowRuntime.cpp:18215 — Sub_4958D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1453 | 0x004959F0 | `sub_4959F0` | 有正文 | `src/ui/WindowRuntime.cpp:18256 — Sub_4959F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1454 | 0x00495B10 | `sub_495B10` | 有正文 | `src/ui/WindowRuntime.cpp:18297 — Sub_495B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1455 | 0x00495C20 | `sub_495C20` | 有正文 | `src/ui/WindowRuntime.cpp:18337 — Sub_495C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1456 | 0x00495D70 | `sub_495D70` | 有正文 | `src/ui/WindowRuntime.cpp:41609 — Sub_495D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1457 | 0x00495F00 | `sub_495F00` | 有正文 | `src/ui/WindowRuntime.cpp:41675 — Sub_495F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1458 | 0x00495F50 | `sub_495F50` | 有正文 | `src/ui/WindowRuntime.cpp:41695 — Sub_495F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1459 | 0x00495F70 | `sub_495F70` | 有正文 | `src/ui/WindowRuntime.cpp:41704 — Sub_495F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1460 | 0x00495FC0 | `sub_495FC0` | 有正文 | `src/ui/WindowRuntime.cpp:41716 — Sub_495FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1461 | 0x00495FE0 | `sub_495FE0` | 有正文 | `src/ui/WindowRuntime.cpp:41723 — Sub_495FE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1462 | 0x00496570 | `sub_496570` | 有正文 | `src/ui/WindowRuntime.cpp:38068 — Sub_496570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1463 | 0x004965D0 | `sub_4965D0` | 有正文 | `src/ui/WindowRuntime.cpp:38083 — Sub_4965D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1464 | 0x004966B0 | `BattleScene::SetCommandType` | 有正文 | `src/ui/WindowRuntime.cpp:36500 — Sub_4966B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1465 | 0x00496AE0 | `BattleScene::closeAllWin` | 有正文 | `src/ui/WindowRuntime.cpp:36734 — Sub_496AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1466 | 0x00496B90 | `sub_496B90` | 有正文 | `src/ui/WindowRuntime.cpp:36601 — Sub_496B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1467 | 0x00496D50 | `sub_496D50` | 有正文 | `src/ui/WindowRuntime.cpp:36672 — Sub_496D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1468 | 0x00496F10 | `BattleScene::CloseAllSubWindow` | 有正文 | `src/ui/WindowRuntime.cpp:36776 — Sub_496F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1469 | 0x00496FB0 | `sub_496FB0` | 有正文 | `src/ui/WindowRuntime.cpp:37135 — Sub_496FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1470 | 0x00497000 | `sub_497000` | 有正文 | `src/ui/WindowRuntime.cpp:37238 — Sub_497000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1471 | 0x004970B0 | `sub_4970B0` | 有正文 | `src/ui/WindowRuntime.cpp:37152 — Sub_4970B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1472 | 0x00497100 | `sub_497100` | 有正文 | `src/ui/WindowRuntime.cpp:37169 — Sub_497100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1473 | 0x004971C0 | `sub_4971C0` | 有正文 | `src/ui/WindowRuntime.cpp:37196 — Sub_4971C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1474 | 0x00497210 | `sub_497210` | 有正文 | `src/ui/WindowRuntime.cpp:37213 — Sub_497210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1475 | 0x004972B0 | `sub_4972B0` | 有正文 | `src/ui/WindowRuntime.cpp:36809 — Sub_4972B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1476 | 0x004973C0 | `sub_4973C0` | 有正文 | `src/ui/WindowRuntime.cpp:36848 — Sub_4973C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1477 | 0x004974C0 | `sub_4974C0` | 有正文 | `src/ui/WindowRuntime.cpp:37265 — Sub_4974C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1478 | 0x004975A0 | `sub_4975A0` | 有正文 | `src/ui/WindowRuntime.cpp:37297 — Sub_4975A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1479 | 0x00497770 | `sub_497770` | 有正文 | `src/ui/WindowRuntime.cpp:36886 — Sub_497770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1480 | 0x00497900 | `sub_497900` | 有正文 | `src/ui/WindowRuntime.cpp:36940 — Sub_497900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1481 | 0x00497A60 | `sub_497A60` | 有正文 | `src/ui/WindowRuntime.cpp:36994 — Sub_497A60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1482 | 0x00497B80 | `sub_497B80` | 有正文 | `src/ui/WindowRuntime.cpp:37035 — Sub_497B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1483 | 0x00497C80 | `sub_497C80` | 有正文 | `src/ui/WindowRuntime.cpp:37074 — Sub_497C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1484 | 0x00497E30 | `sub_497E30` | 有正文 | `src/ui/WindowRuntime.cpp:37361 — Sub_497E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1485 | 0x00497FF0 | `sub_497FF0` | 有正文 | `src/ui/WindowRuntime.cpp:37413 — Sub_497FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1486 | 0x00498160 | `sub_498160` | 有正文 | `src/ui/WindowRuntime.cpp:37462 — Sub_498160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1487 | 0x004982C0 | `BattleScene::DrawCountDown` | 有正文 | `src/ui/WindowRuntime.cpp:37990 — Sub_4982C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1488 | 0x00498520 | `sub_498520` | 有正文 | `src/ui/WindowRuntime.cpp:37545 — Sub_498520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1489 | 0x004990A0 | `sub_4990A0` | 有正文 | `src/YobiRuntime.cpp:23132 — Sub_4990A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1490 | 0x004990F0 | `sub_4990F0` | 有正文 | `src/ui/WindowRuntime.cpp:40253 — Sub_4990F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1491 | 0x004991C0 | `sub_4991C0` | 有正文 | `src/ui/WindowRuntime.cpp:40282 — Sub_4991C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1492 | 0x004996C0 | `sub_4996C0` | 有正文 | `src/ui/WindowRuntime.cpp:40400 — Sub_4996C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1493 | 0x00499750 | `sub_499750` | 有正文 | `src/ui/WindowRuntime.cpp:40423 — Sub_499750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1494 | 0x00499790 | `sub_499790` | 有正文 | `src/ui/WindowRuntime.cpp:40435 — Sub_499790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1495 | 0x00499830 | `sub_499830` | 有正文 | `src/ui/WindowRuntime.cpp:40476 — Sub_499830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1496 | 0x00499880 | `sub_499880` | 有正文 | `src/ui/WindowRuntime.cpp:40456 — Sub_499880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1497 | 0x004998F0 | `sub_4998F0` | 有正文 | `src/ui/WindowRuntime.cpp:46048 — InitializeLegacySecondItemSelector_4998F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1498 | 0x00499900 | `unknown_libname_45` | 有正文 | `src/ui/WindowRuntime.cpp:46024 — ConstructLegacySecondItemSelector_499900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1499 | 0x00499920 | `unknown_libname_46` | 有正文 | `src/ui/WindowRuntime.cpp:46031 — ReleaseLegacySecondItemSelectorAtExit_499920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1500 | 0x00499930 | `sub_499930` | 有正文 | `src/ui/WindowRuntime.cpp:10670 — Sub_499930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1501 | 0x00499950 | `sub_499950` | 有正文 | `src/ui/WindowRuntime.cpp:10676 — Sub_499950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1502 | 0x00499970 | `sub_499970` | 有正文 | `src/ui/WindowRuntime.cpp:10682 — Sub_499970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1503 | 0x00499990 | `sub_499990` | 有正文 | `src/ui/WindowRuntime.cpp:10749 — Sub_499990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1504 | 0x004999D0 | `sub_4999D0` | 有正文 | `src/ui/WindowRuntime.cpp:10762 — Sub_4999D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1505 | 0x00499B00 | `sub_499B00` | 有正文 | `src/ui/WindowRuntime.cpp:10689 — Sub_499B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1506 | 0x00499B20 | `sub_499B20` | 有正文 | `src/ui/WindowRuntime.cpp:10696 — Sub_499B20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1507 | 0x00499B40 | `sub_499B40` | 有正文 | `src/ui/WindowRuntime.cpp:10703 — Sub_499B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1508 | 0x00499B60 | `sub_499B60` | 有正文 | `src/ui/WindowRuntime.cpp:10711 — Sub_499B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1509 | 0x00499C30 | `sub_499C30` | 有正文 | `src/ui/WindowRuntime.cpp:10804 — Sub_499C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1510 | 0x0049AC40 | `sub_49AC40` | 有正文 | `src/ui/WindowRuntime.cpp:11248 — Sub_49AC40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1511 | 0x0049B610 | `sub_49B610` | 有正文 | `src/ui/WindowRuntime.cpp:11241 — Sub_49B610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1512 | 0x0049B630 | `sub_49B630` | 有正文 | `src/ui/WindowRuntime.cpp:38112 — Sub_49B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1513 | 0x0049B670 | `sub_49B670` | 有正文 | `src/ui/WindowRuntime.cpp:38152 — Sub_49B670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1514 | 0x0049BA10 | `sub_49BA10` | 有正文 | `src/ui/WindowRuntime.cpp:38122 — Sub_49BA10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1515 | 0x0049BB00 | `sub_49BB00` | 有正文 | `src/ui/WindowRuntime.cpp:38223 — Sub_49BB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1516 | 0x0049BD40 | `sub_49BD40` | 有正文 | `src/ui/WindowRuntime.cpp:31310 — Sub_49BD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1517 | 0x0049BDB0 | `sub_49BDB0` | 有正文 | `src/ui/WindowRuntime.cpp:31331 — Sub_49BDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1518 | 0x0049BE00 | `sub_49BE00` | 有正文 | `src/YobiRuntime.cpp:1542 — DrawNineGrid_49BE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1519 | 0x0049C4D0 | `sub_49C4D0` | 有正文 | `src/ui/WindowRuntime.cpp:9196 — Sub_49C4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1520 | 0x0049C510 | `sub_49C510` | 有正文 | `src/ui/WindowRuntime.cpp:9211 — Sub_49C510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1521 | 0x0049C5C0 | `sub_49C5C0` | 有正文 | `src/YobiRuntime.cpp:35144 — Sub_49C5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1522 | 0x0049C5E0 | `sub_49C5E0` | 有正文 | `src/YobiRuntime.cpp:35150 — Sub_49C5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1523 | 0x0049C600 | `Window::CreateWithAnime` | 有正文 | `src/ui/WindowRuntime.cpp:9244 — Sub_49C600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1524 | 0x0049C710 | `sub_49C710` | 有正文 | `src/ui/WindowRuntime.cpp:9289 — Sub_49C710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1525 | 0x0049CAD0 | `sub_49CAD0` | 有正文 | `src/ui/WindowRuntime.cpp:9409 — Sub_49CAD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1526 | 0x0049CDF0 | `sub_49CDF0` | 有正文 | `src/ui/WindowRuntime.cpp:9535 — Sub_49CDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1527 | 0x0049CE70 | `sub_49CE70` | 有正文 | `src/ui/WindowRuntime.cpp:9563 — Sub_49CE70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1528 | 0x0049CF70 | `sub_49CF70` | 有正文 | `src/ui/WindowRuntime.cpp:9608 — Sub_49CF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1529 | 0x0049D2E0 | `World::OpenDialog` | 有正文 | `src/ui/WindowRuntime.cpp:9863 — Sub_49D2E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1530 | 0x0049D8F0 | `sub_49D8F0` | 有正文 | `src/ui/WindowRuntime.cpp:10074 — Sub_49D8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1531 | 0x0049DA30 | `sub_49DA30` | 有正文 | `src/ui/WindowRuntime.cpp:9723 — Sub_49DA30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1532 | 0x0049DDB0 | `sub_49DDB0` | 有正文 | `src/ui/WindowRuntime.cpp:9801 — Sub_49DDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1533 | 0x0049DF30 | `sub_49DF30` | 有正文 | `src/ui/WindowRuntime.cpp:10142 — Sub_49DF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1534 | 0x0049E1D0 | `sub_49E1D0` | 有正文 | `src/ui/WindowRuntime.cpp:10222 — Sub_49E1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1535 | 0x0049E400 | `sub_49E400` | 有正文 | `src/ui/WindowRuntime.cpp:10282 — Sub_49E400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1536 | 0x0049E750 | `sub_49E750` | 有正文 | `src/ui/WindowRuntime.cpp:31278 — Sub_49E750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1537 | 0x0049E800 | `sub_49E800` | 有正文 | `src/ui/WindowRuntime.cpp:12926 — Sub_49E800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1538 | 0x0049E8F0 | `sub_49E8F0` | 有正文 | `src/ui/WindowRuntime.cpp:40591 — Sub_49E8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1539 | 0x0049E990 | `sub_49E990` | 有正文 | `src/ui/WindowRuntime.cpp:40582 — Sub_49E990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1540 | 0x0049EAA0 | `sub_49EAA0` | 有正文 | `src/ui/WindowRuntime.cpp:40609 — Sub_49EAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1541 | 0x0049EB50 | `sub_49EB50` | 有正文 | `src/ui/WindowRuntime.cpp:40737 — Sub_49EB50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1542 | 0x0049EC00 | `sub_49EC00` | 有正文 | `src/ui/WindowRuntime.cpp:40766 — Sub_49EC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1543 | 0x0049ECC0 | `sub_49ECC0` | 有正文 | `src/ui/WindowRuntime.cpp:40794 — Sub_49ECC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1544 | 0x0049ED80 | `sub_49ED80` | 有正文 | `src/ui/WindowRuntime.cpp:40821 — Sub_49ED80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1545 | 0x0049EE60 | `sub_49EE60` | 有正文 | `src/ui/WindowRuntime.cpp:40855 — Sub_49EE60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1546 | 0x0049EEB0 | `sub_49EEB0` | 有正文 | `src/ui/WindowRuntime.cpp:40868 — Sub_49EEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1547 | 0x0049EF60 | `sub_49EF60` | 有正文 | `src/ui/WindowRuntime.cpp:40910 — Sub_49EF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1548 | 0x0049F000 | `sub_49F000` | 有正文 | `src/ui/WindowRuntime.cpp:39905 — Sub_49F000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1549 | 0x0049F040 | `sub_49F040` | 有正文 | `src/ui/WindowRuntime.cpp:39917 — Sub_49F040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1550 | 0x0049F060 | `sub_49F060` | 有正文 | `src/ui/WindowRuntime.cpp:39926 — Sub_49F060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1551 | 0x0049F0A0 | `sub_49F0A0` | 有正文 | `src/ui/WindowRuntime.cpp:39939 — Sub_49F0A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1552 | 0x0049F1A0 | `sub_49F1A0` | 有正文 | `src/ui/WindowRuntime.cpp:39960 — Sub_49F1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1553 | 0x0049F260 | `sub_49F260` | 有正文 | `src/ui/WindowRuntime.cpp:39985 — Sub_49F260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1554 | 0x0049F310 | `sub_49F310` | 有正文 | `src/ui/WindowRuntime.cpp:40014 — Sub_49F310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1555 | 0x0049F3D0 | `sub_49F3D0` | 有正文 | `src/ui/WindowRuntime.cpp:40042 — Sub_49F3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1556 | 0x0049F490 | `sub_49F490` | 有正文 | `src/ui/WindowRuntime.cpp:40069 — Sub_49F490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1557 | 0x0049F570 | `sub_49F570` | 有正文 | `src/ui/WindowRuntime.cpp:40103 — Sub_49F570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1558 | 0x0049F5C0 | `sub_49F5C0` | 有正文 | `src/ui/WindowRuntime.cpp:40116 — Sub_49F5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1559 | 0x0049F680 | `sub_49F680` | 有正文 | `src/ui/WindowRuntime.cpp:40147 — Sub_49F680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1560 | 0x0049F760 | `sub_49F760` | 有正文 | `src/ui/WindowRuntime.cpp:40175 — Sub_49F760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1561 | 0x0049F790 | `sub_49F790` | 有正文 | `src/ui/WindowRuntime.cpp:40186 — Sub_49F790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1562 | 0x0049F7B0 | `sub_49F7B0` | 有正文 | `src/ui/WindowRuntime.cpp:40195 — Sub_49F7B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1563 | 0x0049F7E0 | `sub_49F7E0` | 有正文 | `src/ui/WindowRuntime.cpp:40205 — Sub_49F7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1564 | 0x0049FA40 | `sub_49FA40` | 有正文 | `src/ui/WindowRuntime.cpp:40492 — Sub_49FA40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1565 | 0x0049FB00 | `sub_49FB00` | 有正文 | `src/ui/WindowRuntime.cpp:40523 — Sub_49FB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1566 | 0x0049FBC0 | `sub_49FBC0` | 有正文 | `src/ui/WindowRuntime.cpp:40555 — Sub_49FBC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1567 | 0x0049FCA0 | `sub_49FCA0` | 有正文 | `src/ui/WindowRuntime.cpp:10346 — Sub_49FCA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1568 | 0x0049FD20 | `sub_49FD20` | 有正文 | `src/ui/WindowRuntime.cpp:10370 — Sub_49FD20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1569 | 0x0049FE40 | `sub_49FE40` | 有正文 | `src/ui/WindowRuntime.cpp:10396 — Sub_49FE40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1570 | 0x0049FF70 | `sub_49FF70` | 有正文 | `src/ui/WindowRuntime.cpp:10434 — Sub_49FF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1571 | 0x0049FFA0 | `sub_49FFA0` | 有正文 | `src/ui/WindowRuntime.cpp:10442 — Sub_49FFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1572 | 0x004A00C0 | `sub_4A00C0` | 有正文 | `src/ui/WindowRuntime.cpp:10475 — Sub_4A00C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1573 | 0x004A0140 | `sub_4A0140` | 有正文 | `src/ui/WindowRuntime.cpp:10495 — Sub_4A0140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1574 | 0x004A0280 | `sub_4A0280` | 有正文 | `src/ui/WindowRuntime.cpp:10533 — Sub_4A0280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1575 | 0x004A0300 | `sub_4A0300` | 有正文 | `src/ui/WindowRuntime.cpp:10581 — Sub_4A0300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1576 | 0x004A0420 | `sub_4A0420` | 有正文 | `src/ui/WindowRuntime.cpp:10544 — Sub_4A0420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1577 | 0x004A04A0 | `sub_4A04A0` | 有正文 | `src/ui/WindowRuntime.cpp:10605 — Sub_4A04A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1578 | 0x004A05C0 | `sub_4A05C0` | 有正文 | `src/ui/WindowRuntime.cpp:10555 — Sub_4A05C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1579 | 0x004A0640 | `sub_4A0640` | 有正文 | `src/ui/WindowRuntime.cpp:10629 — Sub_4A0640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1580 | 0x004A0700 | `sub_4A0700` | 有正文 | `src/ui/WindowRuntime.cpp:10566 — Sub_4A0700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1581 | 0x004A0790 | `sub_4A0790` | 有正文 | `src/ui/WindowRuntime.cpp:10641 — Sub_4A0790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1582 | 0x004A0850 | `sub_4A0850` | 有正文 | `src/ui/WindowRuntime.cpp:10653 — Sub_4A0850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1583 | 0x004A0940 | `sub_4A0940` | 有正文 | `src/ui/WindowRuntime.cpp:29817 — Sub_4A0940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1584 | 0x004A0970 | `sub_4A0970` | 有正文 | `src/ui/WindowRuntime.cpp:26073 — Sub_4A0970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1585 | 0x004A09B0 | `sub_4A09B0` | 有正文 | `src/ui/WindowRuntime.cpp:26088 — Sub_4A09B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1586 | 0x004A09E0 | `sub_4A09E0` | 有正文 | `src/ui/WindowRuntime.cpp:26098 — Sub_4A09E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1587 | 0x004A1690 | `sub_4A1690` | 有正文 | `src/ui/WindowRuntime.cpp:26333 — Sub_4A1690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1588 | 0x004A16B0 | `sub_4A16B0` | 有正文 | `src/ui/WindowRuntime.cpp:26340 — Sub_4A16B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1589 | 0x004A1780 | `sub_4A1780` | 有正文 | `src/ui/WindowRuntime.cpp:26367 — Sub_4A1780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1590 | 0x004A1C30 | `sub_4A1C30` | 有正文 | `src/ui/WindowRuntime.cpp:26432 — Sub_4A1C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1591 | 0x004A1EF0 | `sub_4A1EF0` | 有正文 | `src/ui/WindowRuntime.cpp:26477 — Sub_4A1EF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1592 | 0x004A2610 | `sub_4A2610` | 有正文 | `src/ui/WindowRuntime.cpp:26570 — Sub_4A2610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1593 | 0x004A2700 | `sub_4A2700` | 有正文 | `src/ui/WindowRuntime.cpp:28811 — Sub_4A2700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1594 | 0x004A2870 | `sub_4A2870` | 有正文 | `src/ui/WindowRuntime.cpp:26600 — Sub_4A2870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1595 | 0x004A2B00 | `sub_4A2B00` | 有正文 | `src/ui/WindowRuntime.cpp:26752 — Sub_4A2B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1596 | 0x004A2BD0 | `sub_4A2BD0` | 有正文 | `src/ui/WindowRuntime.cpp:26642 — Sub_4A2BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1597 | 0x004A2C60 | `sub_4A2C60` | 有正文 | `src/ui/WindowRuntime.cpp:28878 — Sub_4A2C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1598 | 0x004A2CC0 | `sub_4A2CC0` | 有正文 | `src/ui/WindowRuntime.cpp:28900 — Sub_4A2CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1599 | 0x004A3010 | `sub_4A3010` | 有正文 | `src/ui/WindowRuntime.cpp:28973 — Sub_4A3010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1600 | 0x004A3070 | `sub_4A3070` | 有正文 | `src/ui/WindowRuntime.cpp:28987 — Sub_4A3070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1601 | 0x004A30D0 | `sub_4A30D0` | 有正文 | `src/ui/WindowRuntime.cpp:29001 — Sub_4A30D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1602 | 0x004A3100 | `sub_4A3100` | 有正文 | `src/ui/WindowRuntime.cpp:29017 — Sub_4A3100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1603 | 0x004A3160 | `sub_4A3160` | 有正文 | `src/ui/WindowRuntime.cpp:29031 — Sub_4A3160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1604 | 0x004A3190 | `sub_4A3190` | 有正文 | `src/ui/WindowRuntime.cpp:29047 — Sub_4A3190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1605 | 0x004A31C0 | `sub_4A31C0` | 有正文 | `src/ui/WindowRuntime.cpp:29063 — Sub_4A31C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1606 | 0x004A31E0 | `sub_4A31E0` | 有正文 | `src/ui/WindowRuntime.cpp:29072 — Sub_4A31E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1607 | 0x004A3230 | `sub_4A3230` | 有正文 | `src/ui/WindowRuntime.cpp:29090 — Sub_4A3230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1608 | 0x004A3260 | `sub_4A3260` | 有正文 | `src/ui/WindowRuntime.cpp:29106 — Sub_4A3260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1609 | 0x004A32B0 | `sub_4A32B0` | 有正文 | `src/ui/WindowRuntime.cpp:29804 — Sub_4A32B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1610 | 0x004A32E0 | `sub_4A32E0` | 有正文 | `src/ui/WindowRuntime.cpp:26963 — Sub_4A32E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1611 | 0x004A33C0 | `sub_4A33C0` | 有正文 | `src/ui/WindowRuntime.cpp:27001 — Sub_4A33C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1612 | 0x004A3480 | `sub_4A3480` | 有正文 | `src/ui/WindowRuntime.cpp:27011 — Sub_4A3480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1613 | 0x004A3540 | `sub_4A3540` | 有正文 | `src/ui/WindowRuntime.cpp:27021 — Sub_4A3540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1614 | 0x004A3600 | `sub_4A3600` | 有正文 | `src/ui/WindowRuntime.cpp:27036 — Sub_4A3600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1615 | 0x004A3660 | `sub_4A3660` | 有正文 | `src/ui/WindowRuntime.cpp:27052 — Sub_4A3660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1616 | 0x004A36B0 | `sub_4A36B0` | 有正文 | `src/ui/WindowRuntime.cpp:27065 — Sub_4A36B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1617 | 0x004A3760 | `sub_4A3760` | 有正文 | `src/ui/WindowRuntime.cpp:27082 — Sub_4A3760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1618 | 0x004A3780 | `sub_4A3780` | 有正文 | `src/ui/WindowRuntime.cpp:26787 — Sub_4A3780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1619 | 0x004A37E0 | `sub_4A37E0` | 有正文 | `src/ui/WindowRuntime.cpp:26663 — Sub_4A37E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1620 | 0x004A3820 | `sub_4A3820` | 有正文 | `src/ui/WindowRuntime.cpp:29769 — Sub_4A3820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1621 | 0x004A38C0 | `sub_4A38C0` | 有正文 | `src/ui/WindowRuntime.cpp:26734 — Sub_4A38C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1622 | 0x004A3950 | `sub_4A3950` | 有正文 | `src/ui/WindowRuntime.cpp:29742 — Sub_4A3950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1623 | 0x004A3960 | `sub_4A3960` | 有正文 | `src/ui/WindowRuntime.cpp:29748 — Sub_4A3960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1624 | 0x004A3990 | `sub_4A3990` | 有正文 | `src/ui/WindowRuntime.cpp:29759 — Sub_4A3990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1625 | 0x004A39C0 | `sub_4A39C0` | 有正文 | `src/ui/WindowRuntime.cpp:26802 — Sub_4A39C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1626 | 0x004A3C70 | `sub_4A3C70` | 有正文 | `src/ui/WindowRuntime.cpp:26911 — Sub_4A3C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1627 | 0x004A40C0 | `sub_4A40C0` | 有正文 | `src/ui/WindowRuntime.cpp:26676 — Sub_4A40C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1628 | 0x004A4180 | `sub_4A4180` | 有正文 | `src/ui/WindowRuntime.cpp:27200 — Sub_4A4180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1629 | 0x004A41A0 | `sub_4A41A0` | 有正文 | `src/ui/WindowRuntime.cpp:27209 — Sub_4A41A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1630 | 0x004A4210 | `sub_4A4210` | 有正文 | `src/ui/WindowRuntime.cpp:1840 — Sub_4A4210` | 原入口的 RET／固定返回已用当前 IDA 核实；不是业务占位；完整接线未验 |
| 1631 | 0x004A4220 | `sub_4A4220` | 有正文 | `src/ui/WindowRuntime.cpp:27226 — Sub_4A4220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1632 | 0x004A42C0 | `sub_4A42C0` | 有正文 | `src/ui/WindowRuntime.cpp:27245 — Sub_4A42C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1633 | 0x004A4370 | `sub_4A4370` | 有正文 | `src/ui/WindowRuntime.cpp:27269 — Sub_4A4370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1634 | 0x004A4420 | `sub_4A4420` | 有正文 | `src/ui/WindowRuntime.cpp:27293 — Sub_4A4420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1635 | 0x004A4540 | `sub_4A4540` | 有正文 | `src/ui/WindowRuntime.cpp:27326 — Sub_4A4540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1636 | 0x004A45E0 | `sub_4A45E0` | 有正文 | `src/ui/WindowRuntime.cpp:27343 — Sub_4A45E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1637 | 0x004A4620 | `sub_4A4620` | 有正文 | `src/ui/WindowRuntime.cpp:27403 — Sub_4A4620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1638 | 0x004A4650 | `sub_4A4650` | 有正文 | `src/ui/WindowRuntime.cpp:27416 — Sub_4A4650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1639 | 0x004A4660 | `sub_4A4660` | 有正文 | `src/ui/WindowRuntime.cpp:27424 — Sub_4A4660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1640 | 0x004A4710 | `sub_4A4710` | 有正文 | `src/ui/WindowRuntime.cpp:27451 — Sub_4A4710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1641 | 0x004A47C0 | `sub_4A47C0` | 有正文 | `src/ui/WindowRuntime.cpp:27478 — Sub_4A47C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1642 | 0x004A48A0 | `sub_4A48A0` | 有正文 | `src/ui/WindowRuntime.cpp:27514 — Sub_4A48A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1643 | 0x004A49B0 | `sub_4A49B0` | 有正文 | `src/ui/WindowRuntime.cpp:27565 — Sub_4A49B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1644 | 0x004A4FC0 | `sub_4A4FC0` | 有正文 | `src/ui/WindowRuntime.cpp:27644 — Sub_4A4FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1645 | 0x004A5390 | `sub_4A5390` | 有正文 | `src/ui/WindowRuntime.cpp:25760 — Sub_4A5390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1646 | 0x004A53D0 | `sub_4A53D0` | 有正文 | `src/ui/WindowRuntime.cpp:15884 — Sub_4A53D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1647 | 0x004A53F0 | `sub_4A53F0` | 有正文 | `src/ui/WindowRuntime.cpp:27721 — Sub_4A53F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1648 | 0x004A5650 | `sub_4A5650` | 有正文 | `src/ui/WindowRuntime.cpp:27817 — Sub_4A5650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1649 | 0x004A56F0 | `sub_4A56F0` | 有正文 | `src/ui/WindowRuntime.cpp:28072 — Sub_4A56F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1650 | 0x004A58E0 | `sub_4A58E0` | 有正文 | `src/ui/WindowRuntime.cpp:27846 — Sub_4A58E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1651 | 0x004A5970 | `sub_4A5970` | 有正文 | `src/ui/WindowRuntime.cpp:27870 — Sub_4A5970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1652 | 0x004A6160 | `sub_4A6160` | 有正文 | `src/ui/WindowRuntime.cpp:28029 — Sub_4A6160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1653 | 0x004A6300 | `sub_4A6300` | 有正文 | `src/ui/WindowRuntime.cpp:28142 — Sub_4A6300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1654 | 0x004A6450 | `sub_4A6450` | 有正文 | `src/ui/WindowRuntime.cpp:28226 — Sub_4A6450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1655 | 0x004A6AC0 | `sub_4A6AC0` | 有正文 | `src/ui/WindowRuntime.cpp:28346 — Sub_4A6AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1656 | 0x004A6D00 | `sub_4A6D00` | 有正文 | `src/ui/WindowRuntime.cpp:977 — Sub_4A6D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1657 | 0x004A6D30 | `sub_4A6D30` | 有正文 | `src/ui/WindowRuntime.cpp:28211 — Sub_4A6D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1658 | 0x004A6D50 | `sub_4A6D50` | 有正文 | `src/ui/WindowRuntime.cpp:28217 — Sub_4A6D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1659 | 0x004A6D80 | `sub_4A6D80` | 有正文 | `src/ui/WindowRuntime.cpp:30384 — Sub_4A6D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1660 | 0x004A6DB0 | `sub_4A6DB0` | 有正文 | `src/ui/WindowRuntime.cpp:30396 — Sub_4A6DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1661 | 0x004A6DD0 | `sub_4A6DD0` | 有正文 | `src/ui/WindowRuntime.cpp:30612 — Sub_4A6DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1662 | 0x004A6EA0 | `sub_4A6EA0` | 有正文 | `src/ui/WindowRuntime.cpp:30862 — Sub_4A6EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1663 | 0x004A6FC0 | `sub_4A6FC0` | 有正文 | `src/ui/WindowRuntime.cpp:30643 — Sub_4A6FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1664 | 0x004A7590 | `sub_4A7590` | 有正文 | `src/ui/WindowRuntime.cpp:31034 — Sub_4A7590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1665 | 0x004A7640 | `sub_4A7640` | 有正文 | `src/ui/WindowRuntime.cpp:31046 — Sub_4A7640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1666 | 0x004A76E0 | `sub_4A76E0` | 有正文 | `src/Input.cpp:1395 — Sub_4A76E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1667 | 0x004A7710 | `___setargv_13` | 有正文 | `src/ui/WindowRuntime.cpp:30994 — Sub_4A7710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1668 | 0x004A7720 | `sub_4A7720` | 有正文 | `src/ui/WindowRuntime.cpp:31000 — Sub_4A7720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1669 | 0x004A7730 | `___setargv_14` | 有正文 | `src/ui/WindowRuntime.cpp:31006 — Sub_4A7730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1670 | 0x004A7740 | `sub_4A7740` | 有正文 | `src/ui/WindowRuntime.cpp:31012 — Sub_4A7740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1671 | 0x004A7750 | `sub_4A7750` | 有正文 | `src/ui/WindowRuntime.cpp:31055 — Sub_4A7750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1672 | 0x004A7B00 | `sub_4A7B00` | 有正文 | `src/ui/WindowRuntime.cpp:45371 — Sub_4A7B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1673 | 0x004A7C40 | `sub_4A7C40` | 有正文 | `src/ui/WindowRuntime.cpp:30472 — Sub_4A7C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1674 | 0x004A80E0 | `sub_4A80E0` | 有正文 | `src/ui/WindowRuntime.cpp:45500 — ReadChatInputByteWidth_4A80E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1675 | 0x004A80F0 | `sub_4A80F0` | 有正文 | `src/ui/WindowRuntime.cpp:30942 — Sub_4A80F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1676 | 0x004A8140 | `sub_4A8140` | 有正文 | `src/ui/WindowRuntime.cpp:30445 — Sub_4A8140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1677 | 0x004A81D0 | `sub_4A81D0` | 有正文 | `src/ui/WindowRuntime.cpp:30956 — Sub_4A81D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1678 | 0x004A8240 | `sub_4A8240` | 有正文 | `src/ui/WindowRuntime.cpp:30911 — Sub_4A8240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1679 | 0x004A82C0 | `sub_4A82C0` | 有正文 | `src/ui/WindowRuntime.cpp:30932 — Sub_4A82C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1680 | 0x004A82E0 | `sub_4A82E0` | 有正文 | `src/ui/WindowRuntime.cpp:31018 — Sub_4A82E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1681 | 0x004A8330 | `sub_4A8330` | 有正文 | `src/ui/WindowRuntime.cpp:30406 — Sub_4A8330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1682 | 0x004A83A0 | `sub_4A83A0` | 有正文 | `src/ui/WindowRuntime.cpp:13062 — Sub_4A83A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1683 | 0x004A83D0 | `sub_4A83D0` | 有正文 | `src/ui/WindowRuntime.cpp:13694 — Sub_4A83D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1684 | 0x004A84A0 | `sub_4A84A0` | 有正文 | `src/ui/WindowRuntime.cpp:15358 — Sub_4A84A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1685 | 0x004A9020 | `sub_4A9020` | 有正文 | `src/ui/WindowRuntime.cpp:13721 — Sub_4A9020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1686 | 0x004A9160 | `sub_4A9160` | 有正文 | `src/ui/WindowRuntime.cpp:13756 — Sub_4A9160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1687 | 0x004A92D0 | `sub_4A92D0` | 有正文 | `src/ui/WindowRuntime.cpp:13848 — Sub_4A92D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1688 | 0x004A9350 | `sub_4A9350` | 有正文 | `src/ui/WindowRuntime.cpp:14988 — Sub_4A9350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1689 | 0x004A93E0 | `sub_4A93E0` | 有正文 | `src/ui/WindowRuntime.cpp:13868 — Sub_4A93E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1690 | 0x004A94A0 | `sub_4A94A0` | 有正文 | `src/ui/WindowRuntime.cpp:13893 — Sub_4A94A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1691 | 0x004A9560 | `sub_4A9560` | 有正文 | `src/ui/WindowRuntime.cpp:13918 — Sub_4A9560` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1692 | 0x004A9580 | `sub_4A9580` | 有正文 | `src/ui/WindowRuntime.cpp:13925 — Sub_4A9580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1693 | 0x004A95A0 | `sub_4A95A0` | 有正文 | `src/ui/WindowRuntime.cpp:13932 — Sub_4A95A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1694 | 0x004A96D0 | `sub_4A96D0` | 有正文 | `src/ui/WindowRuntime.cpp:13962 — Sub_4A96D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1695 | 0x004A9820 | `sub_4A9820` | 有正文 | `src/ui/WindowRuntime.cpp:13995 — Sub_4A9820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1696 | 0x004A9990 | `sub_4A9990` | 有正文 | `src/ui/WindowRuntime.cpp:14029 — Sub_4A9990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1697 | 0x004A9B00 | `sub_4A9B00` | 有正文 | `src/ui/WindowRuntime.cpp:14896 — Sub_4A9B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1698 | 0x004A9D10 | `sub_4A9D10` | 有正文 | `src/ui/WindowRuntime.cpp:14063 — Sub_4A9D10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1699 | 0x004AA3D0 | `sub_4AA3D0` | 有正文 | `src/ui/WindowRuntime.cpp:14245 — Sub_4AA3D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1700 | 0x004AA9C0 | `sub_4AA9C0` | 有正文 | `src/ui/WindowRuntime.cpp:14438 — Sub_4AA9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1701 | 0x004AAA70 | `sub_4AAA70` | 有正文 | `src/ui/WindowRuntime.cpp:14468 — Sub_4AAA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1702 | 0x004AAB20 | `sub_4AAB20` | 有正文 | `src/ui/WindowRuntime.cpp:14498 — Sub_4AAB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1703 | 0x004AAB70 | `sub_4AAB70` | 有正文 | `src/ui/WindowRuntime.cpp:6115 — Sub_4AAB70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1704 | 0x004AAEC0 | `sub_4AAEC0` | 有正文 | `src/ui/WindowRuntime.cpp:14512 — Sub_4AAEC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1705 | 0x004AB400 | `sub_4AB400` | 有正文 | `src/ui/WindowRuntime.cpp:6162 — Sub_4AB400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1706 | 0x004AB710 | `sub_4AB710` | 有正文 | `src/ui/WindowRuntime.cpp:14663 — Sub_4AB710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1707 | 0x004ABB20 | `sub_4ABB20` | 有正文 | `src/ui/WindowRuntime.cpp:14800 — Sub_4ABB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1708 | 0x004ABE60 | `sub_4ABE60` | 有正文 | `src/YobiRuntime.cpp:2078 — Sub_4ABE60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1709 | 0x004ABEF0 | `sub_4ABEF0` | 有正文 | `src/YobiRuntime.cpp:2110 — Sub_4ABEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1710 | 0x004AC0C0 | `sub_4AC0C0` | 有正文 | `src/ui/WindowRuntime.cpp:13074 — Sub_4AC0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1711 | 0x004AC0F0 | `sub_4AC0F0` | 有正文 | `src/ui/WindowRuntime.cpp:13802 — Sub_4AC0F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1712 | 0x004AC140 | `sub_4AC140` | 有正文 | `src/ui/WindowRuntime.cpp:13817 — Sub_4AC140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1713 | 0x004AC1B0 | `sub_4AC1B0` | 有正文 | `src/ui/WindowRuntime.cpp:13832 — Sub_4AC1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1714 | 0x004AC1EC | `?classic@locale@std@@SAABV12@XZ_18` | 有承接 | `src/ui/WindowRuntime.cpp:13832 — Sub_4AC1B0` | 原绘图／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 1715 | 0x004AC200 | `sub_4AC200` | 有正文 | `src/ui/WindowRuntime.cpp:14965 — Sub_4AC200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1716 | 0x004AC290 | `sub_4AC290` | 有正文 | `src/ui/WindowRuntime.cpp:15010 — Sub_4AC290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1717 | 0x004AC890 | `sub_4AC890` | 有正文 | `src/ui/WindowRuntime.cpp:15166 — Sub_4AC890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1718 | 0x004ACDF0 | `sub_4ACDF0` | 有正文 | `src/YobiRuntime.cpp:2181 — Sub_4ACDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1719 | 0x004ACE80 | `sub_4ACE80` | 有正文 | `src/YobiRuntime.cpp:2214 — Sub_4ACE80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1720 | 0x004AD060 | `sub_4AD060` | 有正文 | `src/ui/WindowRuntime.cpp:2019 — Sub_4AD060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1721 | 0x004AD090 | `sub_4AD090` | 有正文 | `src/ui/WindowRuntime.cpp:13087 — Sub_4AD090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1722 | 0x004AD0F0 | `sub_4AD0F0` | 有正文 | `src/ui/WindowRuntime.cpp:13100 — Sub_4AD0F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1723 | 0x004AD180 | `sub_4AD180` | 有正文 | `src/ui/WindowRuntime.cpp:13190 — Sub_4AD180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1724 | 0x004AD470 | `sub_4AD470` | 有正文 | `src/ui/WindowRuntime.cpp:13209 — Sub_4AD470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1725 | 0x004AD780 | `sub_4AD780` | 有正文 | `src/ui/WindowRuntime.cpp:13231 — Sub_4AD780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1726 | 0x004AD800 | `sub_4AD800` | 有正文 | `src/ui/WindowRuntime.cpp:13256 — Sub_4AD800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1727 | 0x004AD9F0 | `sub_4AD9F0` | 有正文 | `src/ui/WindowRuntime.cpp:13312 — Sub_4AD9F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1728 | 0x004ADAA0 | `sub_4ADAA0` | 有正文 | `src/ui/WindowRuntime.cpp:13334 — Sub_4ADAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1729 | 0x004ADBD0 | `sub_4ADBD0` | 有正文 | `src/ui/WindowRuntime.cpp:13362 — Sub_4ADBD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1730 | 0x004ADC80 | `sub_4ADC80` | 有正文 | `src/ui/WindowRuntime.cpp:13391 — Sub_4ADC80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1731 | 0x004ADD50 | `sub_4ADD50` | 有正文 | `src/ui/WindowRuntime.cpp:13420 — Sub_4ADD50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1732 | 0x004ADE10 | `sub_4ADE10` | 有正文 | `src/ui/WindowRuntime.cpp:13447 — Sub_4ADE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1733 | 0x004ADEF0 | `sub_4ADEF0` | 有正文 | `src/ui/WindowRuntime.cpp:13477 — Sub_4ADEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1734 | 0x004ADF40 | `sub_4ADF40` | 有正文 | `src/ui/WindowRuntime.cpp:13490 — Sub_4ADF40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1735 | 0x004AE170 | `sub_4AE170` | 有正文 | `src/ui/WindowRuntime.cpp:13538 — Sub_4AE170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1736 | 0x004AE260 | `sub_4AE260` | 有正文 | `src/ui/WindowRuntime.cpp:13575 — Sub_4AE260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1737 | 0x004AE350 | `sub_4AE350` | 有正文 | `src/ui/WindowRuntime.cpp:13599 — Sub_4AE350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1738 | 0x004AE5B0 | `sub_4AE5B0` | 有正文 | `src/ui/WindowRuntime.cpp:13682 — Sub_4AE5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1739 | 0x004AE5F0 | `sub_4AE5F0` | 有正文 | `src/ui/WindowRuntime.cpp:27091 — Sub_4AE5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1740 | 0x004AE620 | `sub_4AE620` | 有正文 | `src/ui/WindowRuntime.cpp:27105 — Sub_4AE620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1741 | 0x004AE690 | `sub_4AE690` | 有正文 | `src/ui/WindowRuntime.cpp:27111 — Sub_4AE690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1742 | 0x004AE700 | `sub_4AE700` | 有正文 | `src/ui/WindowRuntime.cpp:27117 — Sub_4AE700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1743 | 0x004AE770 | `sub_4AE770` | 有正文 | `src/ui/WindowRuntime.cpp:27123 — Sub_4AE770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1744 | 0x004AE7E0 | `sub_4AE7E0` | 有正文 | `src/ui/WindowRuntime.cpp:27129 — Sub_4AE7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1745 | 0x004AE850 | `sub_4AE850` | 有正文 | `src/ui/WindowRuntime.cpp:27135 — Sub_4AE850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1746 | 0x004AE8C0 | `sub_4AE8C0` | 有正文 | `src/ui/WindowRuntime.cpp:27141 — Sub_4AE8C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1747 | 0x004AE8D0 | `sub_4AE8D0` | 有正文 | `src/ui/WindowRuntime.cpp:27149 — Sub_4AE8D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1748 | 0x004AE9C0 | `sub_4AE9C0` | 有正文 | `src/ui/WindowRuntime.cpp:27171 — Sub_4AE9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1749 | 0x004AEA10 | `sub_4AEA10` | 有正文 | `src/ui/WindowRuntime.cpp:27185 — Sub_4AEA10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1750 | 0x004AEAE0 | `sub_4AEAE0` | 有正文 | `src/ui/WindowRuntime.cpp:29204 — Sub_4AEAE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1751 | 0x004AEC10 | `sub_4AEC10` | 有正文 | `src/ui/WindowRuntime.cpp:29231 — Sub_4AEC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1752 | 0x004AEC30 | `sub_4AEC30` | 有正文 | `src/ui/WindowRuntime.cpp:29443 — Sub_4AEC30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1753 | 0x004AED40 | `sub_4AED40` | 有正文 | `src/ui/WindowRuntime.cpp:29466 — Sub_4AED40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1754 | 0x004AEE00 | `sub_4AEE00` | 有正文 | `src/ui/WindowRuntime.cpp:29492 — Sub_4AEE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1755 | 0x004AF000 | `sub_4AF000` | 有正文 | `src/ui/WindowRuntime.cpp:29549 — Sub_4AF000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1756 | 0x004AF090 | `sub_4AF090` | 有正文 | `src/ui/WindowRuntime.cpp:29571 — Sub_4AF090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1757 | 0x004AF0C0 | `sub_4AF0C0` | 有正文 | `src/ui/WindowRuntime.cpp:29581 — Sub_4AF0C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1758 | 0x004AF100 | `sub_4AF100` | 有正文 | `src/ui/WindowRuntime.cpp:29596 — Sub_4AF100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1759 | 0x004AF180 | `sub_4AF180` | 有正文 | `src/ui/WindowRuntime.cpp:29240 — Sub_4AF180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1760 | 0x004AF250 | `sub_4AF250` | 有正文 | `src/ui/WindowRuntime.cpp:29263 — Sub_4AF250` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1761 | 0x004AF650 | `sub_4AF650` | 有正文 | `src/ui/WindowRuntime.cpp:29345 — Sub_4AF650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1762 | 0x004AF960 | `sub_4AF960` | 有正文 | `src/ui/WindowRuntime.cpp:29613 — Sub_4AF960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1763 | 0x004AFA00 | `sub_4AFA00` | 有正文 | `src/ui/WindowRuntime.cpp:29389 — Sub_4AFA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1764 | 0x004AFA60 | `sub_4AFA60` | 有正文 | `src/ui/WindowRuntime.cpp:29425 — Sub_4AFA60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1765 | 0x004AFAE0 | `sub_4AFAE0` | 有正文 | `src/ui/WindowRuntime.cpp:29415 — Sub_4AFAE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1766 | 0x004AFB10 | `sub_4AFB10` | 有正文 | `src/ui/WindowRuntime.cpp:29657 — Sub_4AFB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1767 | 0x004AFC00 | `sub_4AFC00` | 有正文 | `src/ui/WindowRuntime.cpp:29683 — Sub_4AFC00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1768 | 0x004AFCF0 | `sub_4AFCF0` | 有正文 | `src/ui/WindowRuntime.cpp:29709 — Sub_4AFCF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1769 | 0x004AFE20 | `sub_4AFE20` | 有正文 | `src/ui/WindowRuntime.cpp:29632 — Sub_4AFE20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1770 | 0x004AFEA0 | `sub_4AFEA0` | 有正文 | `src/ui/WindowRuntime.cpp:29645 — Sub_4AFEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1771 | 0x004AFED0 | `sub_4AFED0` | 有正文 | `src/ui/WindowRuntime.cpp:29401 — Sub_4AFED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1772 | 0x004AFF30 | `sub_4AFF30` | 有正文 | `src/ui/WindowRuntime.cpp:5469 — Sub_4AFF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1773 | 0x004AFF70 | `sub_4AFF70` | 有正文 | `src/ui/WindowRuntime.cpp:5843 — Sub_4AFF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1774 | 0x004AFFB0 | `sub_4AFFB0` | 有正文 | `src/ui/WindowRuntime.cpp:5483 — Sub_4AFFB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1775 | 0x004AFFE0 | `sub_4AFFE0` | 有正文 | `src/ui/WindowRuntime.cpp:12739 — Sub_4AFFE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1776 | 0x004B0000 | `sub_4B0000` | 有正文 | `src/ui/WindowRuntime.cpp:1903 — Sub_4B0000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1777 | 0x004B0060 | `sub_4B0060` | 有正文 | `src/ui/WindowRuntime.cpp:5492 — Sub_4B0060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1778 | 0x004B0090 | `sub_4B0090` | 有正文 | `src/ui/WindowRuntime.cpp:5858 — Sub_4B0090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1779 | 0x004B00C0 | `sub_4B00C0` | 有正文 | `src/ui/WindowRuntime.cpp:5503 — Sub_4B00C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1780 | 0x004B00F0 | `sub_4B00F0` | 有正文 | `src/ui/WindowRuntime.cpp:12746 — Sub_4B00F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1781 | 0x004B0110 | `sub_4B0110` | 有正文 | `src/ui/WindowRuntime.cpp:5427 — Sub_4B0110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1782 | 0x004B0130 | `sub_4B0130` | 有正文 | `src/ui/WindowRuntime.cpp:5436 — Sub_4B0130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1783 | 0x004B0180 | `sub_4B0180` | 有正文 | `src/ui/WindowRuntime.cpp:5450 — Sub_4B0180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1784 | 0x004B01B0 | `sub_4B01B0` | 有正文 | `src/ui/WindowRuntime.cpp:5461 — Sub_4B01B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1785 | 0x004B0200 | `sub_4B0200` | 有正文 | `src/ui/WindowRuntime.cpp:5870 — Sub_4B0200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1786 | 0x004B0290 | `sub_4B0290` | 有正文 | `src/ui/WindowRuntime.cpp:5891 — Sub_4B0290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1787 | 0x004B08D0 | `sub_4B08D0` | 有正文 | `src/ui/WindowRuntime.cpp:6067 — Sub_4B08D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1788 | 0x004B09E0 | `sub_4B09E0` | 有正文 | `src/ui/WindowRuntime.cpp:7864 — Sub_4B09E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1789 | 0x004B0A50 | `sub_4B0A50` | 有正文 | `src/ui/WindowRuntime.cpp:12753 — Sub_4B0A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1790 | 0x004B0CF0 | `sub_4B0CF0` | 有正文 | `src/ui/WindowRuntime.cpp:12818 — Sub_4B0CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1791 | 0x004B0DB0 | `sub_4B0DB0` | 有正文 | `src/ui/WindowRuntime.cpp:12848 — Sub_4B0DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1792 | 0x004B0F10 | `NULL_FUNC` | 有正文 | `src/ui/WindowRuntime.cpp:1834 — Sub_4B0F10` | 原入口的 RET／固定返回已用当前 IDA 核实；不是业务占位；完整接线未验 |
| 1793 | 0x004B0F20 | `sub_4B0F20` | 有正文 | `src/ui/WindowRuntime.cpp:6296 — Sub_4B0F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1794 | 0x004B1260 | `sub_4B1260` | 有正文 | `src/ui/WindowRuntime.cpp:6348 — Sub_4B1260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1795 | 0x004B1900 | `sub_4B1900` | 有正文 | `src/ui/WindowRuntime.cpp:7698 — Sub_4B1900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1796 | 0x004B1940 | `sub_4B1940` | 有正文 | `src/ui/WindowRuntime.cpp:6681 — Sub_4B1940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1797 | 0x004B2060 | `sub_4B2060` | 有正文 | `src/ui/WindowRuntime.cpp:6865 — Sub_4B2060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1798 | 0x004B2480 | `sub_4B2480` | 有正文 | `src/ui/WindowRuntime.cpp:6997 — Sub_4B2480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1799 | 0x004B2500 | `sub_4B2500` | 有正文 | `src/ui/WindowRuntime.cpp:7013 — Sub_4B2500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1800 | 0x004B25D0 | `sub_4B25D0` | 有正文 | `src/ui/WindowRuntime.cpp:7053 — Sub_4B25D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1801 | 0x004B2710 | `sub_4B2710` | 有正文 | `src/ui/WindowRuntime.cpp:7135 — Sub_4B2710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1802 | 0x004B2730 | `sub_4B2730` | 有正文 | `src/ui/WindowRuntime.cpp:7147 — Sub_4B2730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1803 | 0x004B2E00 | `sub_4B2E00` | 有正文 | `src/ui/WindowRuntime.cpp:7092 — Sub_4B2E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1804 | 0x004B2F50 | `sub_4B2F50` | 有正文 | `src/ui/WindowRuntime.cpp:7141 — Sub_4B2F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1805 | 0x004B2F70 | `sub_4B2F70` | 有正文 | `src/ui/WindowRuntime.cpp:7325 — Sub_4B2F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1806 | 0x004B3430 | `sub_4B3430` | 有正文 | `src/ui/WindowRuntime.cpp:7464 — Sub_4B3430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1807 | 0x004B34C0 | `sub_4B34C0` | 有正文 | `src/ui/WindowRuntime.cpp:7483 — Sub_4B34C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1808 | 0x004B3580 | `sub_4B3580` | 有正文 | `src/ui/WindowRuntime.cpp:7515 — Sub_4B3580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1809 | 0x004B3610 | `sub_4B3610` | 有正文 | `src/ui/WindowRuntime.cpp:7534 — Sub_4B3610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1810 | 0x004B3B00 | `sub_4B3B00` | 有正文 | `src/ui/WindowRuntime.cpp:12623 — Sub_4B3B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1811 | 0x004B3BF0 | `sub_4B3BF0` | 有正文 | `src/ui/WindowRuntime.cpp:12653 — Sub_4B3BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1812 | 0x004B4070 | `sub_4B4070` | 有正文 | `src/ui/WindowRuntime.cpp:12386 — Sub_4B4070` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1813 | 0x004B4140 | `sub_4B4140` | 有正文 | `src/ui/WindowRuntime.cpp:12413 — Sub_4B4140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1814 | 0x004B45A0 | `sub_4B45A0` | 有正文 | `src/ui/WindowRuntime.cpp:12506 — Sub_4B45A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1815 | 0x004B4680 | `sub_4B4680` | 有正文 | `src/ui/WindowRuntime.cpp:12536 — Sub_4B4680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1816 | 0x004B4B00 | `sub_4B4B00` | 有正文 | `src/ui/WindowRuntime.cpp:28399 — Sub_4B4B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1817 | 0x004B4B70 | `sub_4B4B70` | 有正文 | `src/ui/WindowRuntime.cpp:28419 — Sub_4B4B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1818 | 0x004B4B90 | `sub_4B4B90` | 有正文 | `src/ui/WindowRuntime.cpp:28428 — Sub_4B4B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1819 | 0x004B4BB0 | `sub_4B4BB0` | 有正文 | `src/ui/WindowRuntime.cpp:28437 — Sub_4B4BB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1820 | 0x004B4CA0 | `sub_4B4CA0` | 有正文 | `src/ui/WindowRuntime.cpp:28464 — Sub_4B4CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1821 | 0x004B4DB0 | `sub_4B4DB0` | 有正文 | `src/ui/WindowRuntime.cpp:28504 — Sub_4B4DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1822 | 0x004B4DF0 | `sub_4B4DF0` | 有正文 | `src/ui/WindowRuntime.cpp:28520 — Sub_4B4DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1823 | 0x004B4E20 | `sub_4B4E20` | 有正文 | `src/ui/WindowRuntime.cpp:28531 — Sub_4B4E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1824 | 0x004B5210 | `sub_4B5210` | 有正文 | `src/ui/WindowRuntime.cpp:28567 — Sub_4B5210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1825 | 0x004B53C0 | `sub_4B53C0` | 有正文 | `src/ui/WindowRuntime.cpp:28611 — Sub_4B53C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1826 | 0x004B5550 | `sub_4B5550` | 有正文 | `src/ui/WindowRuntime.cpp:28658 — Sub_4B5550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1827 | 0x004B5630 | `sub_4B5630` | 有正文 | `src/ui/WindowRuntime.cpp:28702 — Sub_4B5630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1828 | 0x004B5660 | `sub_4B5660` | 有正文 | `src/ui/WindowRuntime.cpp:28713 — Sub_4B5660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1829 | 0x004B5760 | `sub_4B5760` | 有正文 | `src/ui/WindowRuntime.cpp:28754 — Sub_4B5760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1830 | 0x004B57B0 | `sub_4B57B0` | 有正文 | `src/ui/WindowRuntime.cpp:28765 — Sub_4B57B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1831 | 0x004B5A40 | `sub_4B5A40` | 有正文 | `src/ui/WindowRuntime.cpp:28687 — Sub_4B5A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1832 | 0x004B5A90 | `sub_4B5A90` | 有正文 | `src/ui/WindowRuntime.cpp:18370 — Sub_4B5A90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1833 | 0x004B5AD0 | `sub_4B5AD0` | 有正文 | `src/ui/WindowRuntime.cpp:18385 — Sub_4B5AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1834 | 0x004B5B50 | `sub_4B5B50` | 有正文 | `src/ui/WindowRuntime.cpp:18404 — Sub_4B5B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1835 | 0x004B5B70 | `sub_4B5B70` | 有正文 | `src/ui/WindowRuntime.cpp:18412 — Sub_4B5B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1836 | 0x004B5C30 | `sub_4B5C30` | 有正文 | `src/ui/WindowRuntime.cpp:18432 — Sub_4B5C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1837 | 0x004B62A0 | `sub_4B62A0` | 有正文 | `src/ui/WindowRuntime.cpp:18554 — Sub_4B62A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1838 | 0x004B6970 | `sub_4B6970` | 有正文 | `src/ui/WindowRuntime.cpp:18609 — Sub_4B6970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1839 | 0x004B6C20 | `sub_4B6C20` | 有正文 | `src/ui/WindowRuntime.cpp:18600 — Sub_4B6C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1840 | 0x004B6C50 | `sub_4B6C50` | 有正文 | `src/ui/WindowRuntime.cpp:18671 — Sub_4B6C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1841 | 0x004B6DA0 | `sub_4B6DA0` | 有正文 | `src/ui/WindowRuntime.cpp:18716 — Sub_4B6DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1842 | 0x004B6DC0 | `sub_4B6DC0` | 有正文 | `src/ui/WindowRuntime.cpp:18758 — Sub_4B6DC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1843 | 0x004B6E00 | `sub_4B6E00` | 有正文 | `src/ui/WindowRuntime.cpp:18772 — Sub_4B6E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1844 | 0x004B70D0 | `sub_4B70D0` | 有正文 | `src/ui/WindowRuntime.cpp:18823 — Sub_4B70D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1845 | 0x004B74E0 | `sub_4B74E0` | 有正文 | `src/ui/WindowRuntime.cpp:18913 — Sub_4B74E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1846 | 0x004B77E0 | `sub_4B77E0` | 有正文 | `src/ui/WindowRuntime.cpp:18974 — Sub_4B77E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1847 | 0x004B7AB0 | `sub_4B7AB0` | 有正文 | `src/ui/WindowRuntime.cpp:19120 — Sub_4B7AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1848 | 0x004B7EE0 | `sub_4B7EE0` | 有正文 | `src/ui/WindowRuntime.cpp:19127 — Sub_4B7EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1849 | 0x004B8310 | `sub_4B8310` | 有正文 | `src/ui/WindowRuntime.cpp:19134 — Sub_4B8310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1850 | 0x004B8670 | `sub_4B8670` | 有正文 | `src/ui/WindowRuntime.cpp:19205 — Sub_4B8670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1851 | 0x004B86C0 | `sub_4B86C0` | 有正文 | `src/ui/WindowRuntime.cpp:19221 — Sub_4B86C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1852 | 0x004B8730 | `sub_4B8730` | 有正文 | `src/ui/WindowRuntime.cpp:19244 — Sub_4B8730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1853 | 0x004B8780 | `sub_4B8780` | 有正文 | `src/ui/WindowRuntime.cpp:19265 — Sub_4B8780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1854 | 0x004B8B10 | `sub_4B8B10` | 有正文 | `src/ui/WindowRuntime.cpp:19343 — Sub_4B8B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1855 | 0x004B8BE0 | `sub_4B8BE0` | 有正文 | `src/ui/WindowRuntime.cpp:19370 — Sub_4B8BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1856 | 0x004B8C50 | `sub_4B8C50` | 有正文 | `src/ui/WindowRuntime.cpp:19391 — Sub_4B8C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1857 | 0x004B8C90 | `sub_4B8C90` | 有正文 | `src/ui/WindowRuntime.cpp:19408 — Sub_4B8C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1858 | 0x004B8FF0 | `sub_4B8FF0` | 有正文 | `src/ui/WindowRuntime.cpp:19481 — Sub_4B8FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1859 | 0x004B9090 | `sub_4B9090` | 有正文 | `src/ui/WindowRuntime.cpp:19500 — Sub_4B9090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1860 | 0x004B90E0 | `sub_4B90E0` | 有正文 | `src/ui/WindowRuntime.cpp:19519 — Sub_4B90E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1861 | 0x004B9110 | `sub_4B9110` | 有正文 | `src/ui/WindowRuntime.cpp:19535 — Sub_4B9110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1862 | 0x004B9270 | `sub_4B9270` | 有正文 | `src/ui/WindowRuntime.cpp:19556 — Sub_4B9270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1863 | 0x004B92C0 | `sub_4B92C0` | 有正文 | `src/ui/WindowRuntime.cpp:19566 — Sub_4B92C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1864 | 0x004B9310 | `sub_4B9310` | 有正文 | `src/ui/WindowRuntime.cpp:19586 — Sub_4B9310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1865 | 0x004B99D0 | `sub_4B99D0` | 有正文 | `src/ui/WindowRuntime.cpp:25715 — Sub_4B99D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1866 | 0x004B9B10 | `sub_4B9B10` | 有正文 | `src/ui/WindowRuntime.cpp:19771 — Sub_4B9B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1867 | 0x004B9F40 | `sub_4B9F40` | 有正文 | `src/ui/WindowRuntime.cpp:19830 — Sub_4B9F40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1868 | 0x004BA950 | `sub_4BA950` | 有正文 | `src/ui/WindowRuntime.cpp:20102 — Sub_4BA950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1869 | 0x004BAA50 | `sub_4BAA50` | 有正文 | `src/ui/WindowRuntime.cpp:20137 — Sub_4BAA50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1870 | 0x004BAD10 | `sub_4BAD10` | 有正文 | `src/ui/WindowRuntime.cpp:20348 — Sub_4BAD10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1871 | 0x004BAE10 | `sub_4BAE10` | 有正文 | `src/ui/WindowRuntime.cpp:20394 — Sub_4BAE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1872 | 0x004BAF10 | `sub_4BAF10` | 有正文 | `src/ui/WindowRuntime.cpp:20440 — Sub_4BAF10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1873 | 0x004BB010 | `sub_4BB010` | 有正文 | `src/ui/WindowRuntime.cpp:20484 — Sub_4BB010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1874 | 0x004BB1D0 | `sub_4BB1D0` | 有正文 | `src/ui/WindowRuntime.cpp:20565 — Sub_4BB1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1875 | 0x004BB6E0 | `sub_4BB6E0` | 有正文 | `src/ui/WindowRuntime.cpp:20798 — Sub_4BB6E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1876 | 0x004BBED0 | `sub_4BBED0` | 有正文 | `src/ui/WindowRuntime.cpp:21149 — Sub_4BBED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1877 | 0x004BC060 | `sub_4BC060` | 有正文 | `src/ui/WindowRuntime.cpp:21205 — Sub_4BC060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1878 | 0x004BC150 | `sub_4BC150` | 有正文 | `src/ui/WindowRuntime.cpp:18722 — Sub_4BC150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1879 | 0x004BC230 | `sub_4BC230` | 有正文 | `src/ui/WindowRuntime.cpp:21240 — Sub_4BC230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1880 | 0x004BC660 | `sub_4BC660` | 有正文 | `src/ui/WindowRuntime.cpp:21373 — Sub_4BC660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1881 | 0x004BCEE0 | `sub_4BCEE0` | 有正文 | `src/ui/WindowRuntime.cpp:20262 — Sub_4BCEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1882 | 0x004BD100 | `sub_4BD100` | 有正文 | `src/ui/WindowRuntime.cpp:21623 — Sub_4BD100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1883 | 0x004BD500 | `sub_4BD500` | 有正文 | `src/ui/WindowRuntime.cpp:21722 — Sub_4BD500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1884 | 0x004BD540 | `sub_4BD540` | 有正文 | `src/ui/WindowRuntime.cpp:21732 — Sub_4BD540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1885 | 0x004BD680 | `sub_4BD680` | 有正文 | `src/ui/WindowRuntime.cpp:21764 — Sub_4BD680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1886 | 0x004BD810 | `sub_4BD810` | 有正文 | `src/ui/WindowRuntime.cpp:21864 — Sub_4BD810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1887 | 0x004BDAD0 | `sub_4BDAD0` | 有正文 | `src/ui/WindowRuntime.cpp:21870 — Sub_4BDAD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1888 | 0x004BDD90 | `sub_4BDD90` | 有正文 | `src/ui/WindowRuntime.cpp:21876 — Sub_4BDD90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1889 | 0x004BDF70 | `sub_4BDF70` | 有正文 | `src/ui/WindowRuntime.cpp:21921 — Sub_4BDF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1890 | 0x004BE200 | `sub_4BE200` | 有正文 | `src/ui/WindowRuntime.cpp:21971 — Sub_4BE200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1891 | 0x004BE3F0 | `sub_4BE3F0` | 有正文 | `src/ui/WindowRuntime.cpp:22019 — Sub_4BE3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1892 | 0x004BE600 | `sub_4BE600` | 有正文 | `src/ui/WindowRuntime.cpp:22077 — Sub_4BE600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1893 | 0x004BE930 | `sub_4BE930` | 有正文 | `src/ui/WindowRuntime.cpp:22162 — Sub_4BE930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1894 | 0x004BEC20 | `sub_4BEC20` | 有正文 | `src/ui/WindowRuntime.cpp:22218 — Sub_4BEC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1895 | 0x004BEF30 | `sub_4BEF30` | 有正文 | `src/ui/WindowRuntime.cpp:22291 — Sub_4BEF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1896 | 0x004BF160 | `sub_4BF160` | 有正文 | `src/ui/WindowRuntime.cpp:22342 — Sub_4BF160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1897 | 0x004BF2E0 | `sub_4BF2E0` | 有正文 | `src/ui/WindowRuntime.cpp:22375 — Sub_4BF2E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1898 | 0x004BF460 | `sub_4BF460` | 有正文 | `src/ui/WindowRuntime.cpp:22407 — Sub_4BF460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1899 | 0x004BF620 | `sub_4BF620` | 有正文 | `src/ui/WindowRuntime.cpp:22446 — Sub_4BF620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1900 | 0x004BF7C0 | `sub_4BF7C0` | 有正文 | `src/ui/WindowRuntime.cpp:22480 — Sub_4BF7C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1901 | 0x004BF950 | `sub_4BF950` | 有正文 | `src/ui/WindowRuntime.cpp:22525 — Sub_4BF950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1902 | 0x004BFBC0 | `sub_4BFBC0` | 有正文 | `src/ui/WindowRuntime.cpp:22611 — Sub_4BFBC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1903 | 0x004BFCB0 | `sub_4BFCB0` | 有正文 | `src/ui/WindowRuntime.cpp:22647 — Sub_4BFCB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1904 | 0x004BFF60 | `sub_4BFF60` | 有正文 | `src/ui/WindowRuntime.cpp:22761 — Sub_4BFF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1905 | 0x004C0160 | `sub_4C0160` | 有正文 | `src/ui/WindowRuntime.cpp:22839 — Sub_4C0160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1906 | 0x004C0380 | `sub_4C0380` | 有正文 | `src/ui/WindowRuntime.cpp:22921 — Sub_4C0380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1907 | 0x004C0590 | `sub_4C0590` | 有正文 | `src/ui/WindowRuntime.cpp:22993 — Sub_4C0590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1908 | 0x004C0800 | `sub_4C0800` | 有正文 | `src/ui/WindowRuntime.cpp:23089 — Sub_4C0800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1909 | 0x004C0910 | `sub_4C0910` | 有正文 | `src/ui/WindowRuntime.cpp:25387 — Sub_4C0910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1910 | 0x004C0A50 | `sub_4C0A50` | 有正文 | `src/ui/WindowRuntime.cpp:25429 — Sub_4C0A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1911 | 0x004C0BF0 | `sub_4C0BF0` | 有正文 | `src/ui/WindowRuntime.cpp:25499 — Sub_4C0BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1912 | 0x004C0CB0 | `sub_4C0CB0` | 有正文 | `src/ui/WindowRuntime.cpp:25529 — Sub_4C0CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1913 | 0x004C0EA0 | `sub_4C0EA0` | 有正文 | `src/ui/WindowRuntime.cpp:25594 — Sub_4C0EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1914 | 0x004C0F20 | `sub_4C0F20` | 有正文 | `src/ui/WindowRuntime.cpp:25612 — Sub_4C0F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1915 | 0x004C1390 | `sub_4C1390` | 有正文 | `src/ui/WindowRuntime.cpp:23133 — Sub_4C1390` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1916 | 0x004C13B0 | `sub_4C13B0` | 有正文 | `src/ui/WindowRuntime.cpp:23142 — Sub_4C13B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1917 | 0x004C1700 | `sub_4C1700` | 有正文 | `src/ui/WindowRuntime.cpp:23210 — Sub_4C1700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1918 | 0x004C17B0 | `sub_4C17B0` | 有正文 | `src/ui/WindowRuntime.cpp:23227 — Sub_4C17B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1919 | 0x004C1AA0 | `sub_4C1AA0` | 有正文 | `src/ui/WindowRuntime.cpp:23281 — Sub_4C1AA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1920 | 0x004C1B00 | `sub_4C1B00` | 有正文 | `src/ui/WindowRuntime.cpp:23295 — Sub_4C1B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1921 | 0x004C1B70 | `sub_4C1B70` | 有正文 | `src/ui/WindowRuntime.cpp:23314 — Sub_4C1B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1922 | 0x004C1BC0 | `sub_4C1BC0` | 有正文 | `src/ui/WindowRuntime.cpp:23332 — Sub_4C1BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1923 | 0x004C1C50 | `sub_4C1C50` | 有正文 | `src/ui/WindowRuntime.cpp:23359 — Sub_4C1C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1924 | 0x004C1EC0 | `sub_4C1EC0` | 有正文 | `src/ui/WindowRuntime.cpp:23422 — Sub_4C1EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1925 | 0x004C1FB0 | `sub_4C1FB0` | 有正文 | `src/ui/WindowRuntime.cpp:7882 — Sub_4C1FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1926 | 0x004C2060 | `sub_4C2060` | 有正文 | `src/ui/WindowRuntime.cpp:23456 — Sub_4C2060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1927 | 0x004C21B0 | `sub_4C21B0` | 有正文 | `src/ui/WindowRuntime.cpp:23493 — Sub_4C21B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1928 | 0x004C2460 | `sub_4C2460` | 有正文 | `src/ui/WindowRuntime.cpp:23586 — Sub_4C2460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1929 | 0x004C2530 | `sub_4C2530` | 有正文 | `src/ui/WindowRuntime.cpp:23619 — Sub_4C2530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1930 | 0x004C25B0 | `sub_4C25B0` | 有正文 | `src/ui/WindowRuntime.cpp:23644 — Sub_4C25B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1931 | 0x004C2630 | `sub_4C2630` | 有正文 | `src/ui/WindowRuntime.cpp:23670 — Sub_4C2630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1932 | 0x004C2634 | `sub_4C2634` | 有承接 | `src/ui/WindowRuntime.cpp:23670 — Sub_4C2630` | 原表指向含索引前缀的 4C2630；入口片段并入实际父正文；完整业务／运行未验 |
| 1933 | 0x004C26B0 | `sub_4C26B0` | 有正文 | `src/ui/WindowRuntime.cpp:23694 — Sub_4C26B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1934 | 0x004C2760 | `sub_4C2760` | 有正文 | `src/ui/WindowRuntime.cpp:23725 — Sub_4C2760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1935 | 0x004C27B0 | `sub_4C27B0` | 有正文 | `src/ui/WindowRuntime.cpp:23739 — Sub_4C27B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1936 | 0x004C28C0 | `sub_4C28C0` | 有正文 | `src/ui/WindowRuntime.cpp:23774 — Sub_4C28C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1937 | 0x004C29E0 | `sub_4C29E0` | 有正文 | `src/ui/WindowRuntime.cpp:23818 — Sub_4C29E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1938 | 0x004C2B20 | `sub_4C2B20` | 有正文 | `src/ui/WindowRuntime.cpp:23865 — Sub_4C2B20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1939 | 0x004C2FA0 | `sub_4C2FA0` | 有正文 | `src/ui/WindowRuntime.cpp:23977 — Sub_4C2FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1940 | 0x004C33B0 | `sub_4C33B0` | 有正文 | `src/ui/WindowRuntime.cpp:24076 — Sub_4C33B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1941 | 0x004C3530 | `sub_4C3530` | 有正文 | `src/ui/WindowRuntime.cpp:24112 — Sub_4C3530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1942 | 0x004C3550 | `sub_4C3550` | 有正文 | `src/ui/WindowRuntime.cpp:24121 — Sub_4C3550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1943 | 0x004C39A0 | `sub_4C39A0` | 有正文 | `src/ui/WindowRuntime.cpp:24183 — Sub_4C39A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1944 | 0x004C3A50 | `sub_4C3A50` | 有正文 | `src/ui/WindowRuntime.cpp:24200 — Sub_4C3A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1945 | 0x004C3D40 | `sub_4C3D40` | 有正文 | `src/ui/WindowRuntime.cpp:24254 — Sub_4C3D40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1946 | 0x004C3DA0 | `sub_4C3DA0` | 有正文 | `src/ui/WindowRuntime.cpp:24268 — Sub_4C3DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1947 | 0x004C3E20 | `sub_4C3E20` | 有正文 | `src/ui/WindowRuntime.cpp:24287 — Sub_4C3E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1948 | 0x004C3E70 | `sub_4C3E70` | 有正文 | `src/ui/WindowRuntime.cpp:24305 — Sub_4C3E70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1949 | 0x004C3F00 | `sub_4C3F00` | 有正文 | `src/ui/WindowRuntime.cpp:24332 — Sub_4C3F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1950 | 0x004C4180 | `sub_4C4180` | 有正文 | `src/ui/WindowRuntime.cpp:24396 — Sub_4C4180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1951 | 0x004C4270 | `sub_4C4270` | 有正文 | `src/ui/WindowRuntime.cpp:24430 — Sub_4C4270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1952 | 0x004C43C0 | `sub_4C43C0` | 有正文 | `src/ui/WindowRuntime.cpp:24467 — Sub_4C43C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1953 | 0x004C4680 | `sub_4C4680` | 有正文 | `src/ui/WindowRuntime.cpp:24560 — Sub_4C4680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1954 | 0x004C46C0 | `sub_4C46C0` | 有正文 | `src/ui/WindowRuntime.cpp:24574 — Sub_4C46C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1955 | 0x004C4740 | `sub_4C4740` | 有正文 | `src/ui/WindowRuntime.cpp:24599 — Sub_4C4740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1956 | 0x004C47C0 | `sub_4C47C0` | 有正文 | `src/ui/WindowRuntime.cpp:24625 — Sub_4C47C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1957 | 0x004C4840 | `sub_4C4840` | 有正文 | `src/ui/WindowRuntime.cpp:24649 — Sub_4C4840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1958 | 0x004C48EB | `sub_4C48EB` | 不适用 | `当前 IDA：0x004C48EB..0x004C48F0 仅 5 条 NOP` | 元数据登记的填充片段，无业务正文；保留原行，不扩大到其他入口 |
| 1959 | 0x004C48F0 | `sub_4C48F0` | 有正文 | `src/ui/WindowRuntime.cpp:24680 — Sub_4C48F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1960 | 0x004C4940 | `sub_4C4940` | 有正文 | `src/ui/WindowRuntime.cpp:24694 — Sub_4C4940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1961 | 0x004C4A50 | `sub_4C4A50` | 有正文 | `src/ui/WindowRuntime.cpp:24729 — Sub_4C4A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1962 | 0x004C4B70 | `sub_4C4B70` | 有正文 | `src/ui/WindowRuntime.cpp:24773 — Sub_4C4B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1963 | 0x004C4CB0 | `sub_4C4CB0` | 有正文 | `src/ui/WindowRuntime.cpp:24820 — Sub_4C4CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1964 | 0x004C50C0 | `sub_4C50C0` | 有正文 | `src/ui/WindowRuntime.cpp:24932 — Sub_4C50C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1965 | 0x004C54D0 | `sub_4C54D0` | 有正文 | `src/ui/WindowRuntime.cpp:25031 — Sub_4C54D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1966 | 0x004C5650 | `sub_4C5650` | 有正文 | `src/CharacterSelectPreview.cpp:456 — Sub_4C5650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1967 | 0x004C56E0 | `sub_4C56E0` | 有正文 | `src/CharacterSelectPreview.cpp:425 — Sub_4C56E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1968 | 0x004C5790 | `sub_4C5790` | 有正文 | `src/ui/WindowRuntime.cpp:25067 — Sub_4C5790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1969 | 0x004C5850 | `sub_4C5850` | 有正文 | `src/ui/WindowRuntime.cpp:25088 — Sub_4C5850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1970 | 0x004C5B70 | `sub_4C5B70` | 有正文 | `src/ui/WindowRuntime.cpp:25160 — Sub_4C5B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1971 | 0x004C5C00 | `sub_4C5C00` | 有正文 | `src/ui/WindowRuntime.cpp:25178 — Sub_4C5C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1972 | 0x004C5C30 | `sub_4C5C30` | 有正文 | `src/ui/WindowRuntime.cpp:25187 — Sub_4C5C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1973 | 0x004C5D00 | `sub_4C5D00` | 有正文 | `src/ui/WindowRuntime.cpp:25223 — Sub_4C5D00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1974 | 0x004C5DE0 | `sub_4C5DE0` | 有正文 | `src/ui/WindowRuntime.cpp:25258 — Sub_4C5DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1975 | 0x004C5EB0 | `sub_4C5EB0` | 有正文 | `src/ui/WindowRuntime.cpp:25292 — Sub_4C5EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1976 | 0x004C5F80 | `sub_4C5F80` | 有正文 | `src/ui/WindowRuntime.cpp:25326 — Sub_4C5F80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1977 | 0x004C6060 | `sub_4C6060` | 有正文 | `src/ui/WindowRuntime.cpp:25357 — Sub_4C6060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1978 | 0x004C61B0 | `sub_4C61B0` | 有正文 | `src/ui/WindowRuntime.cpp:8077 — Sub_4C61B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1979 | 0x004C63C0 | `sub_4C63C0` | 有正文 | `src/ui/WindowRuntime.cpp:8176 — Sub_4C63C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1980 | 0x004C6400 | `sub_4C6400` | 有正文 | `src/ui/WindowRuntime.cpp:8194 — Sub_4C6400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1981 | 0x004C6440 | `sub_4C6440` | 有正文 | `src/ui/WindowRuntime.cpp:8212 — Sub_4C6440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1982 | 0x004C6470 | `sub_4C6470` | 有正文 | `src/ui/WindowRuntime.cpp:8224 — Sub_4C6470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1983 | 0x004C6500 | `sub_4C6500` | 有正文 | `src/ui/WindowRuntime.cpp:8246 — Sub_4C6500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1984 | 0x004C6550 | `sub_4C6550` | 有正文 | `src/ui/WindowRuntime.cpp:8264 — Sub_4C6550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1985 | 0x004C6790 | `sub_4C6790` | 有正文 | `src/ui/WindowRuntime.cpp:8351 — Sub_4C6790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1986 | 0x004C6990 | `sub_4C6990` | 有正文 | `src/ui/WindowRuntime.cpp:8439 — Sub_4C6990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1987 | 0x004C69B0 | `sub_4C69B0` | 有正文 | `src/ui/WindowRuntime.cpp:8448 — Sub_4C69B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1988 | 0x004C6AC0 | `sub_4C6AC0` | 有正文 | `src/ui/WindowRuntime.cpp:8481 — Sub_4C6AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1989 | 0x004C6B10 | `sub_4C6B10` | 有正文 | `src/ui/WindowRuntime.cpp:8494 — Sub_4C6B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1990 | 0x004C6D90 | `sub_4C6D90` | 有正文 | `src/ui/WindowRuntime.cpp:8572 — Sub_4C6D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1991 | 0x004C6EC0 | `sub_4C6EC0` | 有正文 | `src/ui/WindowRuntime.cpp:8116 — Sub_4C6EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1992 | 0x004C71B0 | `sub_4C71B0` | 有正文 | `src/ui/WindowRuntime.cpp:8109 — Sub_4C71B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1993 | 0x004C71C0 | `sub_4C71C0` | 有正文 | `src/ui/WindowRuntime.cpp:8599 — Sub_4C71C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1994 | 0x004C7240 | `sub_4C7240` | 有正文 | `src/ui/WindowRuntime.cpp:8810 — Sub_4C7240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1995 | 0x004C7280 | `sub_4C7280` | 有正文 | `src/ui/WindowRuntime.cpp:8826 — Sub_4C7280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1996 | 0x004C72C0 | `sub_4C72C0` | 有正文 | `src/ui/WindowRuntime.cpp:8842 — Sub_4C72C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1997 | 0x004C7340 | `sub_4C7340` | 有正文 | `src/ui/WindowRuntime.cpp:8626 — Sub_4C7340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1998 | 0x004C7380 | `sub_4C7380` | 有正文 | `src/ui/WindowRuntime.cpp:8644 — Sub_4C7380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 1999 | 0x004C75A0 | `sub_4C75A0` | 有正文 | `src/ui/WindowRuntime.cpp:11938 — Sub_4C75A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2000 | 0x004C78A0 | `sub_4C78A0` | 有正文 | `src/ui/WindowRuntime.cpp:8721 — Sub_4C78A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2001 | 0x004C7910 | `sub_4C7910` | 有正文 | `src/ui/WindowRuntime.cpp:8735 — Sub_4C7910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2002 | 0x004C7A10 | `sub_4C7A10` | 有正文 | `src/ui/WindowRuntime.cpp:8774 — Sub_4C7A10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2003 | 0x004C7A90 | `sub_4C7A90` | 有正文 | `src/ui/WindowRuntime.cpp:8797 — Sub_4C7A90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2004 | 0x004C7AD0 | `sub_4C7AD0` | 有正文 | `src/ui/WindowRuntime.cpp:15963 — Sub_4C7AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2005 | 0x004C7AE0 | `sub_4C7AE0` | 有正文 | `src/ui/WindowRuntime.cpp:16069 — ProcessWindowShortcuts_4C7AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2006 | 0x004C9060 | `sub_4C9060` | 有正文 | `src/ui/WindowRuntime.cpp:15754 — Sub_4C9060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2007 | 0x004C9130 | `sub_4C9130` | 有正文 | `src/ui/WindowRuntime.cpp:15832 — Sub_4C9130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2008 | 0x004C9170 | `sub_4C9170` | 有正文 | `src/ui/WindowRuntime.cpp:15719 — Sub_4C9170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2009 | 0x004C9240 | `sub_4C9240` | 有正文 | `src/ui/WindowRuntime.cpp:15658 — Sub_4C9240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2010 | 0x004C92B0 | `sub_4C92B0` | 有正文 | `src/ui/WindowRuntime.cpp:15789 — Sub_4C92B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2011 | 0x004C92D0 | `sub_4C92D0` | 有正文 | `src/ui/WindowRuntime.cpp:15796 — Sub_4C92D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2012 | 0x004C93A0 | `sub_4C93A0` | 有正文 | `src/ui/WindowRuntime.cpp:15679 — Sub_4C93A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2013 | 0x004C93E0 | `sub_4C93E0` | 有正文 | `src/ui/WindowRuntime.cpp:15691 — Sub_4C93E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2014 | 0x004C9490 | `sub_4C9490` | 有正文 | `src/world/MailState.cpp:697 — Sub_4C9490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2015 | 0x004C94B0 | `sub_4C94B0` | 有正文 | `src/ui/WindowRuntime.cpp:4450 — Sub_4C94B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2016 | 0x004C94D6 | `?classic@locale@std@@SAABV12@XZ_20` | 有承接 | `src/ui/WindowRuntime.cpp:4450 — Sub_4C94B0` | 原调用 4C9C70／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2017 | 0x004C94F0 | `sub_4C94F0` | 有正文 | `src/ui/WindowRuntime.cpp:4467 — Sub_4C94F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2018 | 0x004C96F0 | `sub_4C96F0` | 有正文 | `src/ui/WindowRuntime.cpp:4514 — Sub_4C96F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2019 | 0x004C9BE0 | `sub_4C9BE0` | 有正文 | `src/ui/WindowRuntime.cpp:4669 — Sub_4C9BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2020 | 0x004C9C70 | `sub_4C9C70` | 有正文 | `src/ui/WindowRuntime.cpp:4426 — Sub_4C9C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2021 | 0x004C9E40 | `sub_4C9E40` | 有正文 | `src/world/MailState.cpp:1105 — Sub_4C9E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2022 | 0x004C9EE0 | `sub_4C9EE0` | 有正文 | `src/ui/WindowRuntime.cpp:4713 — Sub_4C9EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2023 | 0x004C9F08 | `?classic@locale@std@@SAABV12@XZ_22` | 有承接 | `src/ui/WindowRuntime.cpp:4713 — Sub_4C9EE0` | 原调用空回调 509080／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2024 | 0x004C9F20 | `sub_4C9F20` | 有正文 | `src/ui/WindowRuntime.cpp:4687 — Sub_4C9F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2025 | 0x004CA1A0 | `sub_4CA1A0` | 有正文 | `src/ui/WindowRuntime.cpp:4763 — Sub_4CA1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2026 | 0x004CA300 | `sub_4CA300` | 有正文 | `src/ui/WindowRuntime.cpp:4725 — Sub_4CA300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2027 | 0x004CA330 | `sub_4CA330` | 有正文 | `src/world/MailState.cpp:1140 — Sub_4CA330` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2028 | 0x004CA410 | `sub_4CA410` | 有正文 | `src/ui/WindowRuntime.cpp:4851 — Sub_4CA410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2029 | 0x004CA438 | `?classic@locale@std@@SAABV12@XZ_23` | 有承接 | `src/ui/WindowRuntime.cpp:4851 — Sub_4CA410` | 原调用 4CA450／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2030 | 0x004CA450 | `sub_4CA450` | 有正文 | `src/ui/WindowRuntime.cpp:4829 — Sub_4CA450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2031 | 0x004CA500 | `sub_4CA500` | 有正文 | `src/ui/WindowRuntime.cpp:4794 — Sub_4CA500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2032 | 0x004CA6F0 | `sub_4CA6F0` | 有正文 | `src/ui/WindowRuntime.cpp:4897 — Sub_4CA6F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2033 | 0x004CA9C0 | `sub_4CA9C0` | 有正文 | `src/ui/WindowRuntime.cpp:4940 — Sub_4CA9C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2034 | 0x004CAB60 | `sub_4CAB60` | 有正文 | `src/ui/WindowRuntime.cpp:4864 — Sub_4CAB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2035 | 0x004CAB90 | `sub_4CAB90` | 有正文 | `src/ui/WindowRuntime.cpp:4872 — Sub_4CAB90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2036 | 0x004CAC60 | `sub_4CAC60` | 有正文 | `src/world/MailState.cpp:1168 — Sub_4CAC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2037 | 0x004CAF60 | `sub_4CAF60` | 有正文 | `src/ui/WindowRuntime.cpp:5029 — Sub_4CAF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2038 | 0x004CAF88 | `?classic@locale@std@@SAABV12@XZ_24` | 有承接 | `src/ui/WindowRuntime.cpp:5029 — Sub_4CAF60` | 原调用 4CAFA0／返回 1 共享尾；并入实际父正文；完整业务／运行未验 |
| 2039 | 0x004CAFA0 | `sub_4CAFA0` | 有正文 | `src/ui/WindowRuntime.cpp:4979 — Sub_4CAFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2040 | 0x004CB2F0 | `sub_4CB2F0` | 有正文 | `src/ui/WindowRuntime.cpp:5056 — Sub_4CB2F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2041 | 0x004CB430 | `sub_4CB430` | 有正文 | `src/ui/WindowRuntime.cpp:4734 — Sub_4CB430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2042 | 0x004CB4A0 | `sub_4CB4A0` | 有正文 | `src/ui/WindowRuntime.cpp:4746 — Sub_4CB4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2043 | 0x004CB530 | `sub_4CB530` | 有正文 | `src/ui/WindowRuntime.cpp:5049 — Sub_4CB530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2044 | 0x004CB550 | `sub_4CB550` | 有正文 | `src/ui/WindowRuntime.cpp:5109 — Sub_4CB550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2045 | 0x004CB5D0 | `sub_4CB5D0` | 有正文 | `src/ui/WindowRuntime.cpp:5089 — Sub_4CB5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2046 | 0x004CB6A0 | `sub_4CB6A0` | 有正文 | `src/ui/WindowRuntime.cpp:5152 — Sub_4CB6A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2047 | 0x004CB920 | `sub_4CB920` | 有正文 | `src/ui/WindowRuntime.cpp:5129 — Sub_4CB920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2048 | 0x004CB9B0 | `sub_4CB9B0` | 有正文 | `src/ui/WindowRuntime.cpp:5039 — Sub_4CB9B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2049 | 0x004CB9E0 | `?classic@locale@std@@SAABV12@XZ_25` | 有正文 | `src/ui/WindowRuntime.cpp:5273 — Sub_4CB9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2050 | 0x004CB9F0 | `sub_4CB9F0` | 有正文 | `src/ui/WindowRuntime.cpp:5224 — Sub_4CB9F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2051 | 0x004CBD30 | `sub_4CBD30` | 有正文 | `src/ui/WindowRuntime.cpp:30102 — Sub_4CBD30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2052 | 0x004CBD90 | `sub_4CBD90` | 有正文 | `src/ui/WindowRuntime.cpp:30232 — Sub_4CBD90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2053 | 0x004CBDB0 | `sub_4CBDB0` | 有正文 | `src/ui/WindowRuntime.cpp:30242 — Sub_4CBDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2054 | 0x004CC340 | `sub_4CC340` | 有正文 | `src/ui/WindowRuntime.cpp:30171 — Sub_4CC340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2055 | 0x004CC540 | `sub_4CC540` | 有正文 | `src/ui/WindowRuntime.cpp:30126 — Sub_4CC540` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2056 | 0x004CC6A0 | `sub_4CC6A0` | 有正文 | `src/ui/WindowRuntime.cpp:29830 — Sub_4CC6A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2057 | 0x004CC6C0 | `sub_4CC6C0` | 有正文 | `src/ui/WindowRuntime.cpp:29889 — Sub_4CC6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2058 | 0x004CC6E0 | `sub_4CC6E0` | 有正文 | `src/ui/WindowRuntime.cpp:29899 — Sub_4CC6E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2059 | 0x004CC9B0 | `sub_4CC9B0` | 有正文 | `src/ui/WindowRuntime.cpp:29957 — Sub_4CC9B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2060 | 0x004CCED0 | `sub_4CCED0` | 有正文 | `src/ui/WindowRuntime.cpp:29839 — Sub_4CCED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2061 | 0x004CD150 | `sub_4CD150` | 有正文 | `src/ui/WindowRuntime.cpp:30065 — Sub_4CD150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2062 | 0x004CD1E0 | `sub_4CD1E0` | 有正文 | `src/ui/WindowRuntime.cpp:30083 — Sub_4CD1E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2063 | 0x004CD220 | `sub_4CD220` | 有正文 | `src/ui/WindowRuntime.cpp:30093 — Sub_4CD220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2064 | 0x004CD240 | `sub_4CD240` | 有正文 | `src/ui/WindowRuntime.cpp:46061 — InitializeLegacyStallBrowsePetValues_4CD240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2065 | 0x004CD250 | `sub_4CD250` | 有承接 | `src/ui/WindowRuntime.cpp:46067 — ConstructStallBrowsePetValues` | 固定成员构造／逆序销毁由实际数组 helper 承接；未记完整展开验收；完整业务／运行未验 |
| 2066 | 0x004CD2D0 | `sub_4CD2D0` | 有正文 | `src/world/SceneState.cpp:676 — DestroyLegacyPetElement_4CD2D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2067 | 0x004CD360 | `sub_4CD360` | 有正文 | `src/world/SceneState.cpp:657 — ConstructLegacyPetElement_4CD360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2068 | 0x004CD3E0 | `sub_4CD3E0` | 有正文 | `src/ui/WindowRuntime.cpp:46120 — Sub_4CD3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2069 | 0x004CD400 | `sub_4CD400` | 有正文 | `src/ui/WindowRuntime.cpp:46130 — Sub_4CD400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2070 | 0x004CD420 | `sub_4CD420` | 有正文 | `src/ui/WindowRuntime.cpp:4244 — Sub_4CD420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2071 | 0x004CD4F0 | `sub_4CD4F0` | 有正文 | `src/ui/WindowRuntime.cpp:3690 — Sub_4CD4F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2072 | 0x004CDB10 | `sub_4CDB10` | 有正文 | `src/ui/WindowRuntime.cpp:3896 — Sub_4CDB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2073 | 0x004CDE00 | `sub_4CDE00` | 有正文 | `src/ui/WindowRuntime.cpp:4269 — Sub_4CDE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2074 | 0x004CDE40 | `sub_4CDE40` | 有正文 | `src/ui/WindowRuntime.cpp:2137 — Sub_4CDE40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2075 | 0x004CDFA0 | `sub_4CDFA0` | 有正文 | `src/ui/WindowRuntime.cpp:2180 — Sub_4CDFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2076 | 0x004CE1F0 | `sub_4CE1F0` | 有正文 | `src/ui/WindowRuntime.cpp:2249 — Sub_4CE1F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2077 | 0x004CE2B0 | `sub_4CE2B0` | 有正文 | `src/ui/WindowRuntime.cpp:2279 — Sub_4CE2B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2078 | 0x004CE3F0 | `sub_4CE3F0` | 有正文 | `src/ui/WindowRuntime.cpp:2325 — Sub_4CE3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2079 | 0x004CE700 | `sub_4CE700` | 有正文 | `src/ui/WindowRuntime.cpp:4278 — Sub_4CE700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2080 | 0x004CE8A0 | `sub_4CE8A0` | 有正文 | `src/ui/WindowRuntime.cpp:2414 — Sub_4CE8A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2081 | 0x004CE8B0 | `sub_4CE8B0` | 有正文 | `src/ui/WindowRuntime.cpp:4324 — Sub_4CE8B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2082 | 0x004CE930 | `sub_4CE930` | 有正文 | `src/ui/WindowRuntime.cpp:4345 — Sub_4CE930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2083 | 0x004CECB0 | `sub_4CECB0` | 有正文 | `src/ui/WindowRuntime.cpp:4217 — Sub_4CECB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2084 | 0x004CED50 | `sub_4CED50` | 有正文 | `src/ui/WindowRuntime.cpp:2420 — Sub_4CED50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2085 | 0x004CED60 | `sub_4CED60` | 有正文 | `src/ui/WindowRuntime.cpp:4007 — Sub_4CED60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2086 | 0x004CF8A0 | `sub_4CF8A0` | 有正文 | `src/ui/WindowRuntime.cpp:4198 — Sub_4CF8A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2087 | 0x004CF940 | `sub_4CF940` | 有正文 | `src/ui/WindowRuntime.cpp:2509 — Sub_4CF940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2088 | 0x004CFC40 | `sub_4CFC40` | 有正文 | `src/ui/WindowRuntime.cpp:2428 — Sub_4CFC40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2089 | 0x004CFCA0 | `sub_4CFCA0` | 有正文 | `src/ui/WindowRuntime.cpp:2442 — Sub_4CFCA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2090 | 0x004CFD70 | `sub_4CFD70` | 有正文 | `src/ui/WindowRuntime.cpp:2466 — Sub_4CFD70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2091 | 0x004CFE70 | `sub_4CFE70` | 有正文 | `src/ui/WindowRuntime.cpp:2650 — Sub_4CFE70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2092 | 0x004D0230 | `sub_4D0230` | 有正文 | `src/ui/WindowRuntime.cpp:2738 — Sub_4D0230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2093 | 0x004D04A0 | `sub_4D04A0` | 有正文 | `src/ui/WindowRuntime.cpp:3178 — Sub_4D04A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2094 | 0x004D0990 | `sub_4D0990` | 有正文 | `src/ui/WindowRuntime.cpp:2779 — Sub_4D0990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2095 | 0x004D0A80 | `sub_4D0A80` | 有正文 | `src/ui/WindowRuntime.cpp:2805 — Sub_4D0A80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2096 | 0x004D0EB0 | `sub_4D0EB0` | 有正文 | `src/ui/WindowRuntime.cpp:2877 — Sub_4D0EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2097 | 0x004D16F0 | `sub_4D16F0` | 有正文 | `src/ui/WindowRuntime.cpp:3060 — Sub_4D16F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2098 | 0x004D1C00 | `sub_4D1C00` | 有正文 | `src/ui/WindowRuntime.cpp:3030 — Sub_4D1C00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2099 | 0x004D1D50 | `sub_4D1D50` | 有正文 | `src/ui/WindowRuntime.cpp:3327 — Sub_4D1D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2100 | 0x004D2260 | `sub_4D2260` | 有正文 | `src/ui/WindowRuntime.cpp:3415 — Sub_4D2260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2101 | 0x004D27C0 | `sub_4D27C0` | 有正文 | `src/ui/WindowRuntime.cpp:2387 — Sub_4D27C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2102 | 0x004D2840 | `sub_4D2840` | 有正文 | `src/ui/WindowRuntime.cpp:3531 — Sub_4D2840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2103 | 0x004D3020 | `sub_4D3020` | 有正文 | `src/ui/WindowRuntime.cpp:5386 — Sub_4D3020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2104 | 0x004D3040 | `sub_4D3040` | 有正文 | `src/ui/WindowRuntime.cpp:5394 — Sub_4D3040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2105 | 0x004D3110 | `sub_4D3110` | 有正文 | `src/YobiRuntime.cpp:36092 — Sub_4D3110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2106 | 0x004D3170 | `sub_4D3170` | 有正文 | `src/world/SceneState.cpp:8643 — Sub_4D3170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2107 | 0x004D3180 | `sub_4D3180` | 有正文 | `src/LegacyUiRecord.cpp:2349 — Sub_4D3180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2108 | 0x004D31C0 | `sub_4D31C0` | 有正文 | `src/LegacyUiRecord.cpp:2361 — Sub_4D31C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2109 | 0x004D3210 | `sub_4D3210` | 有正文 | `src/LegacyUiRecord.cpp:2374 — Sub_4D3210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2110 | 0x004D3410 | `sub_4D3410` | 有正文 | `src/LegacyUiRecord.cpp:2433 — Sub_4D3410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2111 | 0x004D36B0 | `sub_4D36B0` | 有正文 | `src/LegacyUiRecord.cpp:5292 — Sub_4D36B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2112 | 0x004D36F0 | `sub_4D36F0` | 有正文 | `src/LegacyUiRecord.cpp:5303 — Sub_4D36F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2113 | 0x004D4960 | `sub_4D4960` | 有正文 | `src/LegacyUiRecord.cpp:5568 — Sub_4D4960` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2114 | 0x004D4A50 | `sub_4D4A50` | 有正文 | `src/LegacyUiRecord.cpp:5591 — Sub_4D4A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2115 | 0x004D4ED0 | `sub_4D4ED0` | 有正文 | `src/LegacyUiRecord.cpp:5688 — Sub_4D4ED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2116 | 0x004D4F00 | `sub_4D4F00` | 有正文 | `src/LegacyUiRecord.cpp:5699 — Sub_4D4F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2117 | 0x004D5220 | `sub_4D5220` | 有正文 | `src/LegacyUiRecord.cpp:5764 — Sub_4D5220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2118 | 0x004D5250 | `sub_4D5250` | 有正文 | `src/LegacyUiRecord.cpp:5775 — Sub_4D5250` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2119 | 0x004D5580 | `sub_4D5580` | 有正文 | `src/LegacyUiRecord.cpp:5839 — Sub_4D5580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2120 | 0x004D55C0 | `sub_4D55C0` | 有正文 | `src/LegacyUiRecord.cpp:5850 — Sub_4D55C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2121 | 0x004D61E0 | `sub_4D61E0` | 有正文 | `src/LegacyUiRecord.cpp:6083 — Sub_4D61E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2122 | 0x004D62A0 | `sub_4D62A0` | 有正文 | `src/LegacyUiRecord.cpp:6100 — Sub_4D62A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2123 | 0x004D6640 | `sub_4D6640` | 有正文 | `src/YobiRuntime.cpp:36309 — Sub_4D6640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2124 | 0x004D6650 | `nullsub_3` | 有承接 | `src/YobiRuntime.cpp:36309 — Sub_4D6640` | 原父分配失败落入 RET；当前父实际 return nullptr 承接；完整业务／运行未验 |
| 2125 | 0x004D6690 | `sub_4D6690` | 有正文 | `src/YobiRuntime.cpp:36374 — Sub_4D6690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2126 | 0x004D66E0 | `sub_4D66E0` | 有正文 | `src/LegacyUiRecord.cpp:2510 — Sub_4D66E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2127 | 0x004D6710 | `sub_4D6710` | 有正文 | `src/LegacyUiRecord.cpp:2521 — Sub_4D6710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2128 | 0x004D6A30 | `sub_4D6A30` | 有正文 | `src/LegacyUiRecord.cpp:2602 — Sub_4D6A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2129 | 0x004D6A60 | `sub_4D6A60` | 有正文 | `src/LegacyUiRecord.cpp:2613 — Sub_4D6A60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2130 | 0x004D6E70 | `sub_4D6E70` | 有正文 | `src/LegacyUiRecord.cpp:2719 — Sub_4D6E70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2131 | 0x004D6EA0 | `sub_4D6EA0` | 有正文 | `src/LegacyUiRecord.cpp:2730 — Sub_4D6EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2132 | 0x004D7440 | `sub_4D7440` | 有正文 | `src/LegacyCatalogWindows.cpp:192 — Sub_4D7440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2133 | 0x004D7470 | `sub_4D7470` | 部分实现 | `src/LegacyCatalogWindows.cpp:204 — Sub_4D7470` | 正文已有；CA0478 数组仅有 extern 声明，当前未见定义／生产；C8AF84 页数仅初值与读取，容量和原生产待证；不以数组初值代运行期数据；本轮原IDA限定检索仅见首条已证记录及相关计数／图号的读取、取地址引用，未定位生产者；不外推间接写入不存在；本轮已加载IDA各段的十个已证W32地址字面量检索只命中同30处消费指令，未发现未建xref的地址存储槽；仍不排除动态地址／间接写入 |
| 2134 | 0x004D7BF0 | `sub_4D7BF0` | 有正文 | `src/LegacyCatalogWindows.cpp:355 — Sub_4D7BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2135 | 0x004D7C20 | `sub_4D7C20` | 部分实现 | `src/LegacyCatalogWindows.cpp:366 — Sub_4D7C20` | 正文已有；消费 CA0478 目录，但当前数组仅有 extern 声明，未见定义／生产；旧目录容量与生产待证；不以数组初值代运行期数据；本轮原IDA限定检索仅见首条已证记录及相关计数／图号的读取、取地址引用，未定位生产者；不外推间接写入不存在；本轮已加载IDA各段的十个已证W32地址字面量检索只命中同30处消费指令，未发现未建xref的地址存储槽；仍不排除动态地址／间接写入 |
| 2136 | 0x004D8200 | `sub_4D8200` | 有正文 | `src/LegacyCatalogWindows.cpp:497 — Sub_4D8200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2137 | 0x004D8230 | `sub_4D8230` | 部分实现 | `src/LegacyCatalogWindows.cpp:508 — Sub_4D8230` | 正文已有；消费 CA0478 目录，但当前数组仅有 extern 声明，未见定义／生产；旧目录容量与生产待证；不以数组初值代运行期数据；本轮原IDA限定检索仅见首条已证记录及相关计数／图号的读取、取地址引用，未定位生产者；不外推间接写入不存在；本轮已加载IDA各段的十个已证W32地址字面量检索只命中同30处消费指令，未发现未建xref的地址存储槽；仍不排除动态地址／间接写入 |
| 2138 | 0x004D88C0 | `sub_4D88C0` | 部分实现 | `src/LegacyCatalogWindows.cpp:648 — Sub_4D88C0` | 正文已有；消费 CA0478 目录，但当前数组仅有 extern 声明，未见定义／生产；旧目录容量与生产待证；不以数组初值代运行期数据；本轮原IDA限定检索仅见首条已证记录及相关计数／图号的读取、取地址引用，未定位生产者；不外推间接写入不存在；本轮已加载IDA各段的十个已证W32地址字面量检索只命中同30处消费指令，未发现未建xref的地址存储槽；仍不排除动态地址／间接写入 |
| 2139 | 0x004D8D70 | `sub_4D8D70` | 有正文 | `src/LegacyUiRecord.cpp:3320 — Sub_4D8D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2140 | 0x004D8DA0 | `sub_4D8DA0` | 有正文 | `src/LegacyUiRecord.cpp:3331 — Sub_4D8DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2141 | 0x004D91E0 | `sub_4D91E0` | 有正文 | `src/LegacyUiRecord.cpp:3450 — Sub_4D91E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2142 | 0x004D9220 | `sub_4D9220` | 有正文 | `src/LegacyUiRecord.cpp:3461 — Sub_4D9220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2143 | 0x004DA400 | `sub_4DA400` | 有正文 | `src/LegacyUiRecord.cpp:3709 — Sub_4DA400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2144 | 0x004DA580 | `sub_4DA580` | 有正文 | `src/LegacyUiRecord.cpp:3743 — Sub_4DA580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2145 | 0x004DAA00 | `sub_4DAA00` | 有正文 | `src/LegacyUiRecord.cpp:3841 — Sub_4DAA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2146 | 0x004DAA40 | `sub_4DAA40` | 有正文 | `src/LegacyUiRecord.cpp:3852 — Sub_4DAA40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2147 | 0x004DBD30 | `sub_4DBD30` | 有正文 | `src/LegacyUiRecord.cpp:4114 — Sub_4DBD30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2148 | 0x004DC290 | `sub_4DC290` | 有正文 | `src/LegacyUiRecord.cpp:4228 — Sub_4DC290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2149 | 0x004DC7A0 | `sub_4DC7A0` | 有正文 | `src/LegacyUiRecord.cpp:4335 — Sub_4DC7A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2150 | 0x004DC7E0 | `sub_4DC7E0` | 有正文 | `src/LegacyUiRecord.cpp:4347 — Sub_4DC7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2151 | 0x004DDA30 | `sub_4DDA30` | 有正文 | `src/LegacyUiRecord.cpp:4595 — Sub_4DDA30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2152 | 0x004DDB20 | `sub_4DDB20` | 有正文 | `src/LegacyUiRecord.cpp:4618 — Sub_4DDB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2153 | 0x004DDF80 | `sub_4DDF80` | 有正文 | `src/LegacyUiRecord.cpp:4721 — Sub_4DDF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2154 | 0x004DDFC0 | `sub_4DDFC0` | 有正文 | `src/LegacyUiRecord.cpp:4733 — Sub_4DDFC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2155 | 0x004DF210 | `sub_4DF210` | 有正文 | `src/LegacyUiRecord.cpp:4982 — Sub_4DF210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2156 | 0x004DF300 | `sub_4DF300` | 有正文 | `src/LegacyUiRecord.cpp:5005 — Sub_4DF300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2157 | 0x004DF730 | `sub_4DF730` | 有正文 | `src/LegacyUiRecord.cpp:5108 — Sub_4DF730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2158 | 0x004E0100 | `sub_4E0100` | 有正文 | `src/ui/WindowRuntime.cpp:12965 — Sub_4E0100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2159 | 0x004E0270 | `sub_4E0270` | 有正文 | `src/LegacyUiRecord.cpp:6373 — Sub_4E0270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2160 | 0x004E02B0 | `sub_4E02B0` | 有正文 | `src/ui/WindowRuntime.cpp:15336 — Sub_4E02B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2161 | 0x004E0380 | `sub_4E0380` | 有正文 | `src/LegacyUiRecord.cpp:6183 — Sub_4E0380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2162 | 0x004E0BC0 | `sub_4E0BC0` | 有正文 | `src/LegacyUiRecord.cpp:6388 — Sub_4E0BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2163 | 0x004E0C70 | `sub_4E0C70` | 有正文 | `src/LegacyUiRecord.cpp:6410 — Sub_4E0C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2164 | 0x004E0D10 | `sub_4E0D10` | 有正文 | `src/LegacyUiRecord.cpp:6425 — Sub_4E0D10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2165 | 0x004E0FF0 | `sub_4E0FF0` | 有正文 | `src/LegacyUiRecord.cpp:6490 — Sub_4E0FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2166 | 0x004E10F0 | `sub_4E10F0` | 有正文 | `src/LegacyUiRecord.cpp:6513 — Sub_4E10F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2167 | 0x004E1840 | `sub_4E1840` | 有正文 | `src/ui/WindowRuntime.cpp:1861 — Sub_4E1840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2168 | 0x004E18F0 | `sub_4E18F0` | 有正文 | `src/LegacyUiRecord.cpp:6607 — Sub_4E18F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2169 | 0x004E1980 | `sub_4E1980` | 有正文 | `src/LegacyUiRecord.cpp:6624 — Sub_4E1980` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2170 | 0x004E19E0 | `sub_4E19E0` | 有正文 | `src/LegacyUiRecord.cpp:6636 — Sub_4E19E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2171 | 0x004E1B10 | `sub_4E1B10` | 有正文 | `src/LegacyUiRecord.cpp:6664 — Sub_4E1B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2172 | 0x004E1EE0 | `sub_4E1EE0` | 有正文 | `src/LegacyUiRecord.cpp:6721 — Sub_4E1EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2173 | 0x004E2030 | `sub_4E2030` | 有正文 | `src/LegacyUiRecord.cpp:6748 — Sub_4E2030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2174 | 0x004E28F0 | `sub_4E28F0` | 有正文 | `src/LegacyUiRecord.cpp:6992 — Sub_4E28F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2175 | 0x004E2D90 | `sub_4E2D90` | 有正文 | `src/LegacyUiRecord.cpp:7125 — Sub_4E2D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2176 | 0x004E3750 | `sub_4E3750` | 有正文 | `src/LegacyUiRecord.cpp:7283 — Sub_4E3750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2177 | 0x004E4020 | `sub_4E4020` | 有正文 | `src/ui/WindowRuntime.cpp:39814 — Sub_4E4020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2178 | 0x004E4460 | `sub_4E4460` | 有正文 | `src/LegacyUiRecord.cpp:7400 — Sub_4E4460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2179 | 0x004E4860 | `sub_4E4860` | 有正文 | `src/ui/WindowRuntime.cpp:39699 — Sub_4E4860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2180 | 0x004E4A50 | `sub_4E4A50` | 有正文 | `src/LegacyUiRecord.cpp:3158 — Sub_4E4A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2181 | 0x004E4E60 | `sub_4E4E60` | 有正文 | `src/ui/WindowRuntime.cpp:12919 — Sub_4E4E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2182 | 0x004E4E80 | `sub_4E4E80` | 有正文 | `src/ui/WindowRuntime.cpp:12959 — Sub_4E4E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2183 | 0x004E4EA0 | `sub_4E4EA0` | 部分实现 | `src/LegacyCatalogWindows.cpp:102 — Sub_4E4EA0` | 正文已有；CB1D1C 名单仅 extern，当前未见定义／生产；数量 D30134 与页数 CF46D0 仅初值与读取，容量和原生产待证；不以数组初值代运行期数据；本轮原IDA限定检索仅见首条已证记录及相关计数／图号的读取、取地址引用，未定位生产者；不外推间接写入不存在；本轮已加载IDA各段的十个已证W32地址字面量检索只命中同30处消费指令，未发现未建xref的地址存储槽；仍不排除动态地址／间接写入 |
| 2184 | 0x004E52B0 | `sub_4E52B0` | 有正文 | `src/ui/WindowRuntime.cpp:12999 — Sub_4E52B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2185 | 0x004E54E0 | `sub_4E54E0` | 有正文 | `src/ui/WindowRuntime.cpp:2098 — Sub_4E54E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2186 | 0x004E5600 | `sub_4E5600` | 有正文 | `src/ui/WindowRuntime.cpp:2129 — Sub_4E5600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2187 | 0x004E5620 | `sub_4E5620` | 有正文 | `src/ui/WindowRuntime.cpp:12893 — Sub_4E5620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2188 | 0x004E5710 | `sub_4E5710` | 有正文 | `src/ui/WindowRuntime.cpp:15486 — Sub_4E5710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2189 | 0x004E5770 | `sub_4E5770` | 有正文 | `src/ui/WindowRuntime.cpp:15499 — Sub_4E5770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2190 | 0x004E5830 | `sub_4E5830` | 有正文 | `src/ui/WindowRuntime.cpp:15521 — Sub_4E5830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2191 | 0x004E5A40 | `sub_4E5A40` | 有正文 | `src/ui/WindowRuntime.cpp:15562 — Sub_4E5A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2192 | 0x004E5CD0 | `sub_4E5CD0` | 有正文 | `src/ui/WindowRuntime.cpp:15600 — Sub_4E5CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2193 | 0x004E5DA0 | `sub_4E5DA0` | 有正文 | `src/ui/WindowRuntime.cpp:1846 — Sub_4E5DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2194 | 0x004E5E40 | `sub_4E5E40` | 有正文 | `src/LegacyUiRecord.cpp:7490 — Sub_4E5E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2195 | 0x004E6CC0 | `sub_4E6CC0` | 有正文 | `src/LegacyUiRecord.cpp:7774 — Sub_4E6CC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2196 | 0x004E6D70 | `sub_4E6D70` | 有正文 | `src/LegacyUiRecord.cpp:7796 — Sub_4E6D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2197 | 0x004E6E10 | `sub_4E6E10` | 有正文 | `src/LegacyUiRecord.cpp:7811 — Sub_4E6E10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2198 | 0x004E70B0 | `sub_4E70B0` | 有正文 | `src/LegacyUiRecord.cpp:7870 — Sub_4E70B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2199 | 0x004E7140 | `sub_4E7140` | 有正文 | `src/LegacyUiRecord.cpp:7887 — Sub_4E7140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2200 | 0x004E71A0 | `sub_4E71A0` | 有正文 | `src/LegacyUiRecord.cpp:7899 — Sub_4E71A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2201 | 0x004E7290 | `sub_4E7290` | 有正文 | `src/LegacyUiRecord.cpp:7922 — Sub_4E7290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2202 | 0x004E7380 | `sub_4E7380` | 有正文 | `src/LegacyUiRecord.cpp:7945 — Sub_4E7380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2203 | 0x004E7740 | `sub_4E7740` | 有正文 | `src/LegacyUiRecord.cpp:8002 — Sub_4E7740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2204 | 0x004E7870 | `sub_4E7870` | 有正文 | `src/LegacyUiRecord.cpp:8028 — Sub_4E7870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2205 | 0x004E7FC0 | `sub_4E7FC0` | 有正文 | `src/LegacyUiRecord.cpp:8122 — Sub_4E7FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2206 | 0x004E81B0 | `sub_4E81B0` | 有正文 | `src/LegacyUiRecord.cpp:8167 — Sub_4E81B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2207 | 0x004E82F0 | `sub_4E82F0` | 有正文 | `src/LegacyUiRecord.cpp:8203 — Sub_4E82F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2208 | 0x004E83D0 | `sub_4E83D0` | 有正文 | `src/LegacyUiRecord.cpp:8222 — Sub_4E83D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2209 | 0x004E8920 | `sub_4E8920` | 有正文 | `src/LegacyUiRecord.cpp:8362 — Sub_4E8920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2210 | 0x004E8AD0 | `sub_4E8AD0` | 有正文 | `src/LegacyUiRecord.cpp:8415 — Sub_4E8AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2211 | 0x004E9280 | `sub_4E9280` | 有正文 | `src/LegacyUiRecord.cpp:8510 — Sub_4E9280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2212 | 0x004E92A0 | `sub_4E92A0` | 有正文 | `src/LegacyUiRecord.cpp:8517 — Sub_4E92A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2213 | 0x004E97C0 | `sub_4E97C0` | 有正文 | `src/LegacyUiRecord.cpp:8640 — Sub_4E97C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2214 | 0x004E97E0 | `sub_4E97E0` | 有正文 | `src/LegacyUiRecord.cpp:8647 — Sub_4E97E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2215 | 0x004E9C60 | `sub_4E9C60` | 有正文 | `src/LegacyUiRecord.cpp:8773 — Sub_4E9C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2216 | 0x004E9C80 | `sub_4E9C80` | 有正文 | `src/LegacyUiRecord.cpp:8780 — Sub_4E9C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2217 | 0x004EA130 | `sub_4EA130` | 有正文 | `src/LegacyUiRecord.cpp:8899 — Sub_4EA130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2218 | 0x004EA1B0 | `sub_4EA1B0` | 有正文 | `src/LegacyUiRecord.cpp:8911 — Sub_4EA1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2219 | 0x004EA670 | `sub_4EA670` | 有正文 | `src/ui/WindowRuntime.cpp:6212 — Sub_4EA670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2220 | 0x004EA690 | `sub_4EA690` | 有正文 | `src/LegacyUiRecord.cpp:9024 — Sub_4EA690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2221 | 0x004EAA50 | `sub_4EAA50` | 有正文 | `src/LegacyUiRecord.cpp:9417 — Sub_4EAA50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2222 | 0x004EAAA0 | `sub_4EAAA0` | 有正文 | `src/LegacyUiRecord.cpp:9436 — Sub_4EAAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2223 | 0x004EB120 | `sub_4EB120` | 有正文 | `src/LegacyUiRecord.cpp:9556 — Sub_4EB120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2224 | 0x004EB1C0 | `sub_4EB1C0` | 有正文 | `src/LegacyUiRecord.cpp:9572 — Sub_4EB1C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2225 | 0x004EB890 | `sub_4EB890` | 有正文 | `src/LegacyUiRecord.cpp:9324 — Sub_4EB890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2226 | 0x004EBAA0 | `sub_4EBAA0` | 有正文 | `src/LegacyUiRecord.cpp:9364 — Sub_4EBAA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2227 | 0x004EBD20 | `sub_4EBD20` | 有正文 | `src/LegacyUiRecord.cpp:9722 — Sub_4EBD20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2228 | 0x004EBF70 | `sub_4EBF70` | 有正文 | `src/YobiRuntime.cpp:36346 — Sub_4EBF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2229 | 0x004EBFD0 | `sub_4EBFD0` | 有正文 | `src/YobiRuntime.cpp:36365 — Sub_4EBFD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2230 | 0x004EC030 | `sub_4EC030` | 有正文 | `src/world/MailState.cpp:171 — CompareContactIndices_4EC030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2231 | 0x004EC1A0 | `sub_4EC1A0` | 有正文 | `src/world/MailState.cpp:290 — Sub_4EC1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2232 | 0x004EC1D0 | `sub_4EC1D0` | 有正文 | `src/LegacyUiRecord.cpp:9100 — Sub_4EC1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2233 | 0x004EC240 | `sub_4EC240` | 有正文 | `src/LegacyUiRecord.cpp:9123 — Sub_4EC240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2234 | 0x004ECDF0 | `sub_4ECDF0` | 有正文 | `src/LegacyUiRecord.cpp:1740 — Sub_4ECDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2235 | 0x004ECE00 | `sub_4ECE00` | 有正文 | `src/LegacyUiRecord.cpp:11324 — Sub_4ECE00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2236 | 0x004ED010 | `sub_4ED010` | 有正文 | `src/LegacyUiRecord.cpp:1748 — Sub_4ED010` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2237 | 0x004ED020 | `sub_4ED020` | 有正文 | `src/LegacyUiRecord.cpp:11379 — Sub_4ED020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2238 | 0x004ED670 | `sub_4ED670` | 有正文 | `src/world/SceneState.cpp:6256 — Sub_4ED670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2239 | 0x004ED6B0 | `sub_4ED6B0` | 有正文 | `src/ui/WindowRuntime.cpp:34843 — Sub_4ED6B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2240 | 0x004EDB20 | `sub_4EDB20` | 有正文 | `src/LegacyUiRecord.cpp:11509 — Sub_4EDB20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2241 | 0x004EE130 | `sub_4EE130` | 有正文 | `src/YobiRuntime.cpp:36131 — Sub_4EE130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2242 | 0x004EE380 | `sub_4EE380` | 有正文 | `src/YobiRuntime.cpp:36489 — Sub_4EE380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2243 | 0x004EE670 | `sub_4EE670` | 有正文 | `src/YobiRuntime.cpp:36191 — Sub_4EE670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2244 | 0x004EE9E0 | `sub_4EE9E0` | 有正文 | `src/YobiRuntime.cpp:36112 — Sub_4EE9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2245 | 0x004EEA40 | `sub_4EEA40` | 有正文 | `src/ui/WindowRuntime.cpp:35491 — Sub_4EEA40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2246 | 0x004EF0F0 | `sub_4EF0F0` | 有正文 | `src/LegacyUiRecord.cpp:11657 — Sub_4EF0F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2247 | 0x004EF5F0 | `sub_4EF5F0` | 有正文 | `src/YobiRuntime.cpp:36468 — Sub_4EF5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2248 | 0x004EF710 | `sub_4EF710` | 有正文 | `src/ui/WindowRuntime.cpp:35895 — Sub_4EF710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2249 | 0x004EF750 | `sub_4EF750` | 有正文 | `src/ui/WindowRuntime.cpp:35794 — Sub_4EF750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2250 | 0x004EFB70 | `sub_4EFB70` | 有正文 | `src/LegacyUiRecord.cpp:2856 — Sub_4EFB70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2251 | 0x004F0200 | `sub_4F0200` | 有正文 | `src/ui/WindowRuntime.cpp:39570 — Sub_4F0200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2252 | 0x004F0460 | `sub_4F0460` | 有正文 | `src/LegacyUiRecord.cpp:2995 — Sub_4F0460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2253 | 0x004F0B40 | `sub_4F0B40` | 有正文 | `src/LegacyUiRecord.cpp:11773 — Sub_4F0B40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2254 | 0x004F0CD0 | `sub_4F0CD0` | 有正文 | `src/LegacyUiRecord.cpp:3254 — Sub_4F0CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2255 | 0x004F0E20 | `sub_4F0E20` | 有正文 | `src/LegacyUiRecord.cpp:3287 — Sub_4F0E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2256 | 0x004F0F70 | `sub_4F0F70` | 有正文 | `src/LegacyUiRecord.cpp:11814 — Sub_4F0F70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2257 | 0x004F1800 | `sub_4F1800` | 有正文 | `src/ui/WindowRuntime.cpp:34467 — Sub_4F1800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2258 | 0x004F1C40 | `sub_4F1C40` | 有正文 | `src/LegacyUiRecord.cpp:11940 — Sub_4F1C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2259 | 0x004F2130 | `sub_4F2130` | 有正文 | `src/LegacyUiRecord.cpp:12162 — Sub_4F2130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2260 | 0x004F24A0 | `sub_4F24A0` | 有正文 | `src/LegacyUiRecord.cpp:12236 — Sub_4F24A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2261 | 0x004F25C0 | `sub_4F25C0` | 有正文 | `src/LegacyUiRecord.cpp:12262 — Sub_4F25C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2262 | 0x004F2650 | `sub_4F2650` | 有正文 | `src/LegacyUiRecord.cpp:12282 — Sub_4F2650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2263 | 0x004F2720 | `sub_4F2720` | 有正文 | `src/LegacyUiRecord.cpp:12308 — Sub_4F2720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2264 | 0x004F2A30 | `sub_4F2A30` | 有正文 | `src/LegacyUiRecord.cpp:12370 — Sub_4F2A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2265 | 0x004F39F0 | `sub_4F39F0` | 有正文 | `src/ui/WindowRuntime.cpp:34231 — Sub_4F39F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2266 | 0x004F3ED0 | `sub_4F3ED0` | 有正文 | `src/LegacyUiRecord.cpp:12047 — Sub_4F3ED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2267 | 0x004F4430 | `sub_4F4430` | 有正文 | `src/ui/WindowRuntime.cpp:32266 — Sub_4F4430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2268 | 0x004F4470 | `sub_4F4470` | 有正文 | `src/LegacyUiRecord.cpp:12736 — Sub_4F4470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2269 | 0x004F4630 | `sub_4F4630` | 有正文 | `src/ui/WindowRuntime.cpp:1876 — Sub_4F4630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2270 | 0x004F4690 | `sub_4F4690` | 有正文 | `src/LegacyUiRecord.cpp:12816 — Sub_4F4690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2271 | 0x004F4720 | `sub_4F4720` | 有正文 | `src/LegacyUiRecord.cpp:12841 — Sub_4F4720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2272 | 0x004F4740 | `sub_4F4740` | 有正文 | `src/LegacyUiRecord.cpp:12853 — Sub_4F4740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2273 | 0x004F4760 | `sub_4F4760` | 有正文 | `src/LegacyUiRecord.cpp:12860 — Sub_4F4760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2274 | 0x004F4790 | `sub_4F4790` | 有正文 | `src/LegacyUiRecord.cpp:12872 — Sub_4F4790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2275 | 0x004F50D0 | `sub_4F50D0` | 有正文 | `src/LegacyUiRecord.cpp:13092 — Sub_4F50D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2276 | 0x004F5170 | `sub_4F5170` | 有正文 | `src/LegacyUiRecord.cpp:13113 — Sub_4F5170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2277 | 0x004F51A0 | `sub_4F51A0` | 有正文 | `src/LegacyUiRecord.cpp:13122 — Sub_4F51A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2278 | 0x004F51D0 | `sub_4F51D0` | 有正文 | `src/LegacyUiRecord.cpp:13132 — Sub_4F51D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2279 | 0x004F5A60 | `sub_4F5A60` | 有正文 | `src/LegacyUiRecord.cpp:13769 — Sub_4F5A60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2280 | 0x004F61C0 | `sub_4F61C0` | 有正文 | `src/LegacyUiRecord.cpp:13910 — Sub_4F61C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2281 | 0x004F6220 | `sub_4F6220` | 有正文 | `src/LegacyUiRecord.cpp:13927 — Sub_4F6220` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2282 | 0x004F6BF0 | `sub_4F6BF0` | 有正文 | `src/LegacyUiRecord.cpp:14092 — Sub_4F6BF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2283 | 0x004F6C60 | `sub_4F6C60` | 有正文 | `src/LegacyUiRecord.cpp:14106 — Sub_4F6C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2284 | 0x004F6CA0 | `sub_4F6CA0` | 有正文 | `src/LegacyUiRecord.cpp:14118 — Sub_4F6CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2285 | 0x004F7140 | `sub_4F7140` | 有正文 | `src/LegacyUiRecord.cpp:14227 — Sub_4F7140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2286 | 0x004F7150 | `sub_4F7150` | 有正文 | `src/LegacyUiRecord.cpp:14236 — Sub_4F7150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2287 | 0x004F7870 | `sub_4F7870` | 有正文 | `src/LegacyUiRecord.cpp:14368 — Sub_4F7870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2288 | 0x004F8340 | `sub_4F8340` | 有正文 | `src/LegacyUiRecord.cpp:14546 — Sub_4F8340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2289 | 0x004F83B0 | `sub_4F83B0` | 有正文 | `src/LegacyUiRecord.cpp:14565 — Sub_4F83B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2290 | 0x004F8550 | `sub_4F8550` | 有正文 | `src/LegacyUiRecord.cpp:14621 — Sub_4F8550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2291 | 0x004F8E40 | `sub_4F8E40` | 有正文 | `src/LegacyUiRecord.cpp:14734 — Sub_4F8E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2292 | 0x004F9B10 | `sub_4F9B10` | 有正文 | `src/LegacyUiRecord.cpp:14853 — Sub_4F9B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2293 | 0x004F9FF0 | `sub_4F9FF0` | 有正文 | `src/YobiRuntime.cpp:36327 — Sub_4F9FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2294 | 0x004FA050 | `sub_4FA050` | 有正文 | `src/LegacyUiRecord.cpp:15334 — Sub_4FA050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2295 | 0x004FA350 | `sub_4FA350` | 有正文 | `src/LegacyUiRecord.cpp:15294 — Sub_4FA350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2296 | 0x004FA8E0 | `sub_4FA8E0` | 有正文 | `src/world/SceneState.cpp:9509 — Sub_4FA8E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2297 | 0x004FACE0 | `sub_4FACE0` | 有正文 | `src/YobiRuntime.cpp:36427 — Sub_4FACE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2298 | 0x004FAED0 | `sub_4FAED0` | 有正文 | `src/LegacyUiRecord.cpp:15394 — Sub_4FAED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2299 | 0x004FAEF0 | `sub_4FAEF0` | 有正文 | `src/LegacyUiRecord.cpp:15403 — Sub_4FAEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2300 | 0x004FB8F0 | `MouseEvent::resetDrag` | 有正文 | `src/ui/WindowRuntime.cpp:2076 — Sub_4FB8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2301 | 0x004FB920 | `sub_4FB920` | 有正文 | `src/ui/WindowRuntime.cpp:2088 — Sub_4FB920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2302 | 0x004FB940 | `sub_4FB940` | 有正文 | `src/LegacyUiRecord.cpp:12694 — Sub_4FB940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2303 | 0x004FBA30 | `sub_4FBA30` | 有正文 | `src/ui/WindowRuntime.cpp:7908 — Sub_4FBA30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2304 | 0x004FBB30 | `sub_4FBB30` | 有正文 | `src/LegacyUiRecord.cpp:15666 — Sub_4FBB30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2305 | 0x004FBB40 | `sub_4FBB40` | 有正文 | `src/LegacyUiRecord.cpp:15672 — Sub_4FBB40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2306 | 0x004FC420 | `sub_4FC420` | 有正文 | `src/LegacyUiRecord.cpp:16693 — Sub_4FC420` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2307 | 0x004FC430 | `sub_4FC430` | 有正文 | `src/LegacyUiRecord.cpp:16699 — Sub_4FC430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2308 | 0x004FC7F0 | `sub_4FC7F0` | 有正文 | `src/LegacyUiRecord.cpp:1636 — Sub_4FC7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2309 | 0x004FC810 | `sub_4FC810` | 有正文 | `src/LegacyUiRecord.cpp:1644 — Sub_4FC810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2310 | 0x004FC830 | `sub_4FC830` | 有正文 | `src/LegacyUiRecord.cpp:1652 — Sub_4FC830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2311 | 0x004FC850 | `sub_4FC850` | 有正文 | `src/LegacyUiRecord.cpp:1660 — Sub_4FC850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2312 | 0x004FC870 | `sub_4FC870` | 有正文 | `src/LegacyUiRecord.cpp:1668 — Sub_4FC870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2313 | 0x004FC890 | `sub_4FC890` | 有正文 | `src/LegacyUiRecord.cpp:1676 — Sub_4FC890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2314 | 0x004FC8B0 | `sub_4FC8B0` | 有正文 | `src/LegacyUiRecord.cpp:1684 — Sub_4FC8B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2315 | 0x004FC8D0 | `sub_4FC8D0` | 有正文 | `src/LegacyUiRecord.cpp:1692 — Sub_4FC8D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2316 | 0x004FC8F0 | `sub_4FC8F0` | 有正文 | `src/LegacyUiRecord.cpp:16831 — Sub_4FC8F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2317 | 0x004FCB90 | `sub_4FCB90` | 有正文 | `src/LegacyUiRecord.cpp:16891 — Sub_4FCB90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2318 | 0x004FCBA0 | `sub_4FCBA0` | 有正文 | `src/LegacyUiRecord.cpp:16897 — Sub_4FCBA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2319 | 0x004FD1A0 | `sub_4FD1A0` | 有正文 | `src/LegacyUiRecord.cpp:17123 — Sub_4FD1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2320 | 0x004FD4C0 | `sub_4FD4C0` | 有正文 | `src/DisplaySettings.cpp:15 — Sub_4FD4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2321 | 0x004FD510 | `sub_4FD510` | 有正文 | `src/DisplaySettings.cpp:27 — Sub_4FD510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2322 | 0x004FD550 | `sub_4FD550` | 有正文 | `src/DisplaySettings.cpp:39 — Sub_4FD550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2323 | 0x004FD5A0 | `sub_4FD5A0` | 有正文 | `src/DisplaySettings.cpp:53 — Sub_4FD5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2324 | 0x004FD5C0 | `sub_4FD5C0` | 有正文 | `src/DisplaySettings.cpp:63 — Sub_4FD5C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2325 | 0x004FD5F0 | `sub_4FD5F0` | 有正文 | `src/DisplaySettings.cpp:72 — Sub_4FD5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2326 | 0x004FD640 | `sub_4FD640` | 有正文 | `src/DisplaySettings.cpp:85 — Sub_4FD640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2327 | 0x004FD690 | `sub_4FD690` | 有正文 | `src/LegacyUiRecord.cpp:15205 — Sub_4FD690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2328 | 0x004FD6A0 | `sub_4FD6A0` | 有正文 | `src/LegacyUiRecord.cpp:15213 — Sub_4FD6A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2329 | 0x004FD9F0 | `sub_4FD9F0` | 有正文 | `src/LegacyUiRecord.cpp:17182 — Sub_4FD9F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2330 | 0x004FDA00 | `sub_4FDA00` | 有正文 | `src/LegacyUiRecord.cpp:17188 — Sub_4FDA00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2331 | 0x004FDBD0 | `sub_4FDBD0` | 有正文 | `src/LegacyUiRecord.cpp:17258 — Sub_4FDBD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2332 | 0x004FDD70 | `sub_4FDD70` | 有正文 | `src/DisplaySettings.cpp:105 — Sub_4FDD70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2333 | 0x004FDDC0 | `sub_4FDDC0` | 有正文 | `src/DisplaySettings.cpp:119 — Sub_4FDDC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2334 | 0x004FDE10 | `sub_4FDE10` | 有正文 | `src/LegacyUiRecord.cpp:17302 — Sub_4FDE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2335 | 0x004FDE20 | `sub_4FDE20` | 有正文 | `src/LegacyUiRecord.cpp:17308 — Sub_4FDE20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2336 | 0x004FE0E0 | `sub_4FE0E0` | 有正文 | `src/LegacyUiRecord.cpp:17414 — Sub_4FE0E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2337 | 0x004FE320 | `sub_4FE320` | 有正文 | `src/DisplaySettings.cpp:133 — Sub_4FE320` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2338 | 0x004FE370 | `sub_4FE370` | 有正文 | `src/DisplaySettings.cpp:146 — Sub_4FE370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2339 | 0x004FE3B0 | `sub_4FE3B0` | 有正文 | `src/DisplaySettings.cpp:159 — Sub_4FE3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2340 | 0x004FE3E0 | `sub_4FE3E0` | 有正文 | `src/DisplaySettings.cpp:98 — Sub_4FE3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2341 | 0x004FE3F0 | `sub_4FE3F0` | 有正文 | `src/DisplaySettings.cpp:168 — Sub_4FE3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2342 | 0x004FE440 | `sub_4FE440` | 有正文 | `src/DisplaySettings.cpp:181 — Sub_4FE440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2343 | 0x004FE490 | `sub_4FE490` | 有正文 | `src/LegacyUiRecord.cpp:17469 — Sub_4FE490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2344 | 0x004FE4A0 | `sub_4FE4A0` | 有正文 | `src/LegacyUiRecord.cpp:17475 — Sub_4FE4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2345 | 0x004FE860 | `sub_4FE860` | 有正文 | `src/LegacyUiRecord.cpp:17617 — Sub_4FE860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2346 | 0x004FEAC0 | `sub_4FEAC0` | 有正文 | `src/DisplaySettings.cpp:194 — Sub_4FEAC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2347 | 0x004FEB00 | `sub_4FEB00` | 有正文 | `src/LegacyUiRecord.cpp:17676 — Sub_4FEB00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2348 | 0x004FEB10 | `sub_4FEB10` | 有正文 | `src/LegacyUiRecord.cpp:17682 — Sub_4FEB10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2349 | 0x004FEF30 | `sub_4FEF30` | 有正文 | `src/LegacyUiRecord.cpp:17848 — Sub_4FEF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2350 | 0x004FEFF0 | `sub_4FEFF0` | 有正文 | `src/DisplaySettings.cpp:204 — Sub_4FEFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2351 | 0x004FF020 | `sub_4FF020` | 有正文 | `src/LegacyUiRecord.cpp:17875 — Sub_4FF020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2352 | 0x004FF030 | `sub_4FF030` | 有正文 | `src/LegacyUiRecord.cpp:17881 — Sub_4FF030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2353 | 0x004FF1A0 | `sub_4FF1A0` | 有正文 | `src/LegacyUiRecord.cpp:17938 — Sub_4FF1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2354 | 0x004FF310 | `sub_4FF310` | 有正文 | `src/DisplaySettings.cpp:212 — Sub_4FF310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2355 | 0x004FF340 | `sub_4FF340` | 有正文 | `src/LegacyUiRecord.cpp:17979 — Sub_4FF340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2356 | 0x004FF350 | `sub_4FF350` | 有正文 | `src/LegacyUiRecord.cpp:17985 — Sub_4FF350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2357 | 0x004FF4C0 | `sub_4FF4C0` | 有正文 | `src/LegacyUiRecord.cpp:18042 — Sub_4FF4C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2358 | 0x004FF620 | `sub_4FF620` | 有正文 | `src/DisplaySettings.cpp:221 — Sub_4FF620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2359 | 0x004FF650 | `sub_4FF650` | 有正文 | `src/LegacyUiRecord.cpp:15796 — Sub_4FF650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2360 | 0x00500370 | `sub_500370` | 有正文 | `src/LegacyUiRecord.cpp:16092 — Sub_500370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2361 | 0x005006F0 | `sub_5006F0` | 有正文 | `src/LegacyUiRecord.cpp:16186 — Sub_5006F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2362 | 0x005008C0 | `sub_5008C0` | 有正文 | `src/LegacyUiRecord.cpp:16254 — Sub_5008C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2363 | 0x005014A0 | `sub_5014A0` | 有正文 | `src/LegacyUiRecord.cpp:16407 — Sub_5014A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2364 | 0x00501F60 | `sub_501F60` | 有正文 | `src/LegacyUiRecord.cpp:1756 — Sub_501F60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2365 | 0x00501FD0 | `sub_501FD0` | 有正文 | `src/LegacyUiRecord.cpp:16513 — Sub_501FD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2366 | 0x00502930 | `sub_502930` | 有正文 | `src/LegacyUiRecord.cpp:18083 — Sub_502930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2367 | 0x00502940 | `sub_502940` | 有正文 | `src/LegacyUiRecord.cpp:18089 — Sub_502940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2368 | 0x005029B0 | `sub_5029B0` | 有正文 | `src/LegacyUiRecord.cpp:18113 — Sub_5029B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2369 | 0x00502B80 | `sub_502B80` | 有正文 | `src/LegacyUiRecord.cpp:18162 — Sub_502B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2370 | 0x00502C30 | `sub_502C30` | 有正文 | `src/LegacyUiRecord.cpp:18200 — Sub_502C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2371 | 0x00502C40 | `sub_502C40` | 有正文 | `src/LegacyUiRecord.cpp:18206 — Sub_502C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2372 | 0x00502C60 | `sub_502C60` | 有正文 | `src/LegacyUiRecord.cpp:18215 — Sub_502C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2373 | 0x00502C70 | `sub_502C70` | 有正文 | `src/LegacyUiRecord.cpp:18221 — Sub_502C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2374 | 0x00502C80 | `sub_502C80` | 有正文 | `src/LegacyUiRecord.cpp:18229 — Sub_502C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2375 | 0x00502C90 | `sub_502C90` | 有正文 | `src/LegacyUiRecord.cpp:18235 — Sub_502C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2376 | 0x00502CA0 | `sub_502CA0` | 有正文 | `src/LegacyUiRecord.cpp:18243 — Sub_502CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2377 | 0x00502CB0 | `sub_502CB0` | 有正文 | `src/LegacyUiRecord.cpp:18249 — Sub_502CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2378 | 0x00502D20 | `sub_502D20` | 有正文 | `src/LegacyUiRecord.cpp:18269 — Sub_502D20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2379 | 0x00502D30 | `sub_502D30` | 有正文 | `src/LegacyUiRecord.cpp:18275 — Sub_502D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2380 | 0x00502DA0 | `sub_502DA0` | 有正文 | `src/LegacyUiRecord.cpp:18281 — Sub_502DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2381 | 0x00502DB0 | `sub_502DB0` | 有正文 | `src/LegacyUiRecord.cpp:18287 — Sub_502DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2382 | 0x00502E20 | `sub_502E20` | 有正文 | `src/LegacyUiRecord.cpp:18293 — Sub_502E20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2383 | 0x00502E30 | `sub_502E30` | 有正文 | `src/LegacyUiRecord.cpp:18299 — Sub_502E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2384 | 0x00502EA0 | `sub_502EA0` | 有正文 | `src/LegacyUiRecord.cpp:18305 — Sub_502EA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2385 | 0x00502EB0 | `sub_502EB0` | 有正文 | `src/LegacyUiRecord.cpp:18311 — Sub_502EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2386 | 0x00502F20 | `sub_502F20` | 有正文 | `src/LegacyUiRecord.cpp:18317 — Sub_502F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2387 | 0x00502F30 | `sub_502F30` | 有正文 | `src/LegacyUiRecord.cpp:18323 — Sub_502F30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2388 | 0x00502FA0 | `sub_502FA0` | 有正文 | `src/LegacyUiRecord.cpp:18329 — Sub_502FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2389 | 0x00502FB0 | `sub_502FB0` | 有正文 | `src/LegacyUiRecord.cpp:18335 — Sub_502FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2390 | 0x00503020 | `sub_503020` | 有正文 | `src/LegacyUiRecord.cpp:18341 — Sub_503020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2391 | 0x00503030 | `sub_503030` | 有正文 | `src/LegacyUiRecord.cpp:18347 — Sub_503030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2392 | 0x005030A0 | `sub_5030A0` | 有正文 | `src/LegacyUiRecord.cpp:18353 — Sub_5030A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2393 | 0x00503120 | `sub_503120` | 有正文 | `src/LegacyUiRecord.cpp:18375 — Sub_503120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2394 | 0x00503160 | `sub_503160` | 有正文 | `src/LegacyUiRecord.cpp:18386 — Sub_503160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2395 | 0x00503440 | `sub_503440` | 有正文 | `src/LegacyUiRecord.cpp:18496 — Sub_503440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2396 | 0x00503590 | `sub_503590` | 有正文 | `src/LegacyUiRecord.cpp:18550 — Sub_503590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2397 | 0x005035E0 | `sub_5035E0` | 有正文 | `src/LegacyUiRecord.cpp:18565 — Sub_5035E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2398 | 0x005036F0 | `sub_5036F0` | 有正文 | `src/LoginInput.cpp:168 — QueryLoginControlState_5036F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2399 | 0x00503780 | `sub_503780` | 有正文 | `src/LoginInput.cpp:222 — DrawLoginControl_503780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2400 | 0x005038B0 | `sub_5038B0` | 有正文 | `src/LegacyUiRecord.cpp:18604 — Sub_5038B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2401 | 0x00503990 | `sub_503990` | 有正文 | `src/CharacterModalButton.cpp:17 — Sub_503990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2402 | 0x00503A20 | `sub_503A20` | 有正文 | `src/CharacterModalButton.cpp:72 — Sub_503A20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2403 | 0x00503AB0 | `sub_503AB0` | 有正文 | `src/Input.cpp:836 — ExtractLegacyLine_503AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2404 | 0x00503B70 | `sub_503B70` | 有正文 | `src/LegacyUiRecord.cpp:15041 — Sub_503B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2405 | 0x00503BC0 | `sub_503BC0` | 有正文 | `src/LegacyUiRecord.cpp:15051 — Sub_503BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2406 | 0x00503BE0 | `sub_503BE0` | 有正文 | `src/LegacyUiRecord.cpp:15058 — Sub_503BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2407 | 0x00503C30 | `sub_503C30` | 有正文 | `src/LegacyUiRecord.cpp:15072 — Sub_503C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2408 | 0x00503C40 | `sub_503C40` | 有正文 | `src/LegacyUiRecord.cpp:15078 — Sub_503C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2409 | 0x00503C50 | `sub_503C50` | 有正文 | `src/LegacyUiRecord.cpp:15084 — Sub_503C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2410 | 0x00503C60 | `sub_503C60` | 有正文 | `src/LegacyUiRecord.cpp:15090 — Sub_503C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2411 | 0x00503C70 | `sub_503C70` | 有正文 | `src/LegacyUiRecord.cpp:15096 — Sub_503C70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2412 | 0x00503C80 | `sub_503C80` | 有正文 | `src/render/SdlPresenter.cpp:542 — TakeScreenshotThrottled_503C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2413 | 0x00503CB0 | `sub_503CB0` | 有正文 | `src/ui/WindowRuntime.cpp:15632 — Sub_503CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2414 | 0x00503CD0 | `sub_503CD0` | 有正文 | `src/ui/WindowRuntime.cpp:15644 — Sub_503CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2415 | 0x00503E40 | `sub_503E40` | 有正文 | `src/Input.cpp:1086 — IsCurrentContext_503E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2416 | 0x00503E60 | `sub_503E60` | 有正文 | `src/Input.cpp:1092 — IsChatContext_503E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2417 | 0x00503E80 | `sub_503E80` | 有正文 | `src/ui/WindowRuntime.cpp:46285 — ProcessSceneWindows_503E80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2418 | 0x00504100 | `sub_504100` | 有正文 | `src/world/SceneState.cpp:10717 — Sub_504100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2419 | 0x00504120 | `Window::Render` | 有正文 | `src/ui/WindowRuntime.cpp:45506 — RenderWindows_504120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2420 | 0x00504180 | `sub_504180` | 有正文 | `src/LegacyAudio.cpp:796 — Sub_504180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2421 | 0x005042D0 | `sub_5042D0` | 有正文 | `src/LegacyAudio.cpp:872 — Sub_5042D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2422 | 0x00504380 | `sub_504380` | 有正文 | `src/LegacyAudio.cpp:912 — Sub_504380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2423 | 0x005043D0 | `sub_5043D0` | 有正文 | `src/LegacyAudio.cpp:930 — Sub_5043D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2424 | 0x00504410 | `sub_504410` | 有正文 | `src/LegacyAudio.cpp:777 — Sub_504410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2425 | 0x00504460 | `sub_504460` | 有正文 | `src/LegacyAudio.cpp:761 — Sub_504460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2426 | 0x005044C0 | `sub_5044C0` | 有正文 | `src/LegacyAudio.cpp:740 — Sub_5044C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2427 | 0x005044E0 | `sub_5044E0` | 有正文 | `src/YobiRuntime.cpp:35161 — Sub_5044E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2428 | 0x005046D0 | `sub_5046D0` | 有正文 | `src/YobiRuntime.cpp:1322 — UpdateModalAction_5046D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2429 | 0x005049A0 | `sub_5049A0` | 有正文 | `src/LegacyHitTest.cpp:12 — Sub_5049A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2430 | 0x00504A30 | `sub_504A30` | 有正文 | `src/world/SceneState.cpp:10698 — Sub_504A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2431 | 0x00504A50 | `sub_504A50` | 有正文 | `src/world/SceneState.cpp:6273 — Sub_504A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2432 | 0x00504AF0 | `sub_504AF0` | 有正文 | `src/LegacyAudio.cpp:860 — Sub_504AF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2433 | 0x00504B10 | `sub_504B10` | 有正文 | `src/world/SceneState.cpp:8754 — Sub_504B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2434 | 0x00504B60 | `sub_504B60` | 有正文 | `src/world/SceneState.cpp:8770 — Sub_504B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2435 | 0x00504BA0 | `sub_504BA0` | 有正文 | `src/LegacyUiRecord.cpp:12716 — Sub_504BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2436 | 0x00504C50 | `sub_504C50` | 有正文 | `src/YobiRuntime.cpp:36271 — Sub_504C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2437 | 0x00504EE0 | `sub_504EE0` | 有正文 | `src/LegacyUiRecord.cpp:13307 — Sub_504EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2438 | 0x00504F10 | `sub_504F10` | 有正文 | `src/world/SceneState.cpp:3442 — Sub_504F10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2439 | 0x00504FA0 | `sub_504FA0` | 有正文 | `src/LegacyUiRecord.cpp:13317 — Sub_504FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2440 | 0x00505040 | `sub_505040` | 有正文 | `src/world/SceneState.cpp:3471 — Sub_505040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2441 | 0x005050B0 | `sub_5050B0` | 有正文 | `src/world/SceneState.cpp:3488 — Sub_5050B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2442 | 0x00505110 | `sub_505110` | 有正文 | `src/world/SceneState.cpp:3504 — Sub_505110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2443 | 0x00505170 | `sub_505170` | 有正文 | `src/LegacyUiRecord.cpp:13345 — Sub_505170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2444 | 0x00505BC0 | `sub_505BC0` | 有正文 | `src/LegacyUiRecord.cpp:13579 — Sub_505BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2445 | 0x00505C20 | `sub_505C20` | 有正文 | `src/LegacyUiRecord.cpp:13618 — Sub_505C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2446 | 0x005062A0 | `sub_5062A0` | 有正文 | `src/LegacyUiRecord.cpp:13597 — Sub_5062A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2447 | 0x00506340 | `sub_506340` | 有正文 | `src/platform/LegacyMouse.cpp:516 — InitializeLegacyMouseRuntime_506340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2448 | 0x00506350 | `unknown_libname_47` | 有正文 | `src/platform/LegacyMouse.cpp:473 — ConstructLegacyMouseRuntime_506350` | 正文已有，MUP01补已完成十成员前缀的C++异常清理；X／Y／八按钮原序及成功后登记保持，完整业务／SEH／二次异常／owner／SDK／运行未验 |
| 2449 | 0x00506530 | `sub_506530` | 有承接 | `src/platform/LegacyMouse.cpp:486 — ConstructLegacyMouseRuntime_506350局部MouseConstructionPrefix析构；src/platform/LegacyCryptInt.cpp:76 — Destroy_401CF0` | 原506350八个展开跳转引用本两成员逆序叶；现局部完成计数在每次构造返回后推进，异常时逆序清理完成前缀，成功取消局部清理后才正常atexit登记；MUP01实际接线，不以正常退出替代；原SEH／二次异常／完整owner／SDK／运行未验 |
| 2450 | 0x00506580 | `sub_506580` | 有正文 | `src/platform/LegacyMouseTracker.cpp:165 — ResetLegacyMouseTracker_506580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2451 | 0x005066D0 | `sub_5066D0` | 有正文 | `src/platform/LegacyMouseTracker.cpp:236 — ResetLegacyMouseTrackerDefaults_5066D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2452 | 0x005066F0 | `sub_5066F0` | 有正文 | `src/platform/LegacyMouseTracker.cpp:43 — InsertAfter_5066F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2453 | 0x00506720 | `sub_506720` | 有正文 | `src/platform/LegacyMouseTracker.cpp:62 — Detach_506720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2454 | 0x00506760 | `sub_506760` | 有正文 | `src/platform/LegacyMouseTracker.cpp:86 — TakeFreeHead_506760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2455 | 0x00506780 | `sub_506780` | 有正文 | `src/platform/LegacyMouseTracker.cpp:97 — EvictActiveTail_506780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2456 | 0x005067A0 | `sub_5067A0` | 有正文 | `src/platform/LegacyMouseTracker.cpp:108 — AllocateAndActivate_5067A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2457 | 0x005067F0 | `sub_5067F0` | 有正文 | `src/platform/LegacyMouseTracker.cpp:255 — TrackLegacyMouseDown_5067F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2458 | 0x00506870 | `sub_506870` | 有正文 | `src/platform/LegacyMouse.cpp:523 — ResetLegacyMouseState_506870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2459 | 0x00506890 | `MouseEvent::Reset_0` | 有正文 | `src/platform/LegacyMouse.cpp:532 — ResetLegacyMousePosition_506890` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2460 | 0x00506900 | `MouseEvent::UpdateLDown` | 有正文 | `src/platform/LegacyMouse.cpp:555 — UpdateLegacyMouseLeftDown_506900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2461 | 0x005069E0 | `MouseEvent::UpdateLUp` | 有正文 | `src/platform/LegacyMouse.cpp:598 — UpdateLegacyMouseLeftUp_5069E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2462 | 0x00506A90 | `MouseEvent::UpdateLDbClick` | 有正文 | `src/platform/LegacyMouse.cpp:634 — UpdateLegacyMouseLeftDoubleClick_506A90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2463 | 0x00506B50 | `MouseEvent::UpdateRDown` | 有正文 | `src/platform/LegacyMouse.cpp:671 — UpdateLegacyMouseRightDown_506B50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2464 | 0x00506C40 | `sub_506C40` | 有正文 | `src/platform/LegacyMouse.cpp:714 — UpdateLegacyMouseRightUp_506C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2465 | 0x00506CF0 | `sub_506CF0` | 有正文 | `src/platform/LegacyMouse.cpp:750 — UpdateLegacyMouseRightDoubleClick_506CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2466 | 0x00506DB0 | `sub_506DB0` | 有正文 | `src/platform/LegacyMouse.cpp:787 — AddLegacyMouseWheelDelta_506DB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2467 | 0x00506DD0 | `sub_506DD0` | 有正文 | `src/platform/LegacyCursor.cpp:189 — ResetLegacyMouseTail_506DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2468 | 0x00506DE0 | `sub_506DE0` | 有正文 | `src/platform/LegacyCursor.cpp:170 — RequestCursorState_506DE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2469 | 0x00506E00 | `World::CursorUpdate` | 有正文 | `src/platform/LegacyCursor.cpp:195 — UpdateCursor_506E00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2470 | 0x00507090 | `sub_507090` | 有正文 | `src/render/GraphicPendingQueue.cpp:132 — DrawMatchingGraphicHover_507090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2471 | 0x00507140 | `sub_507140` | 有正文 | `src/render/GraphicHitTest.cpp:211 — Sub_507140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2472 | 0x00507C80 | `sub_507C80` | 有正文 | `src/platform/CommandOptions.cpp:253 — ParseIpOptions_507C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2473 | 0x00507D90 | `GetServerAddressInfo` | 有正文 | `src/LoginScene.cpp:460 — GetServerAddressInfo_507D90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2474 | 0x00507DF0 | `NetworkLoop` | 有正文 | `src/network/NetworkRuntime.cpp:4621 — NetworkLoop_507DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2475 | 0x00508140 | `sub_508140` | 有正文 | `src/network/NetworkRuntime.cpp:4596 — InitializeNetworkRuntime_508140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2476 | 0x005081A0 | `sub_5081A0` | 有正文 | `src/network/NetworkRuntime.cpp:4869 — ShutdownNetworkRuntime_5081A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2477 | 0x005081E0 | `sub_5081E0` | 有正文 | `src/network/NetworkRuntime.cpp:4777 — AppendReceiveQueue_5081E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2478 | 0x00508230 | `sub_508230` | 有正文 | `src/network/NetworkRuntime.cpp:4801 — ConsumeReceiveQueue_508230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2479 | 0x00508280 | `sub_508280` | 有正文 | `src/network/NetworkRuntime.cpp:4820 — ConsumeSendQueue_508280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2480 | 0x005082D0 | `sub_5082D0` | 有正文 | `src/network/NetworkRuntime.cpp:723 — Sub_5082D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2481 | 0x00508310 | `sub_508310` | 有正文 | `src/network/NetworkRuntime.cpp:733 — Sub_508310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2482 | 0x00508350 | `sub_508350` | 有正文 | `src/LoginConnection.cpp:9 — ResetServerConnectionState_508350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2483 | 0x00508360 | `sub_508360` | 有正文 | `src/LoginState.cpp:130 — Sub_508360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2484 | 0x00508800 | `sub_508800` | 有正文 | `src/network/NetworkRuntime.cpp:1077 — HandleResponse_508800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2485 | 0x00508910 | `sub_508910` | 有正文 | `src/CharacterListRequest.cpp:150 — StartCharacterListRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2486 | 0x00508950 | `sub_508950` | 有正文 | `src/CharacterListRequest.cpp:165 — PollCharacterListRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2487 | 0x005089B0 | `sub_5089B0` | 有正文 | `src/CharacterListRequest.cpp:191 — HandleCharacterListResponse_5089B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2488 | 0x00508AF0 | `sub_508AF0` | 有正文 | `src/LoginRequest.cpp:101 — StartLoginRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2489 | 0x00508B50 | `sub_508B50` | 有正文 | `src/LoginRequest.cpp:126 — PollLoginRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2490 | 0x00508BC0 | `sub_508BC0` | 有正文 | `src/LoginRequest.cpp:154 — HandleLoginResponse_508BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2491 | 0x00508C40 | `sub_508C40` | 有正文 | `src/LoginRequest.cpp:191 — Sub_508C40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2492 | 0x00508C90 | `sub_508C90` | 有正文 | `src/LoginRequest.cpp:205 — Sub_508C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2493 | 0x00508CF0 | `sub_508CF0` | 有正文 | `src/LoginRequest.cpp:229 — Sub_508CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2494 | 0x00508E00 | `sub_508E00` | 有正文 | `src/CharacterCreationRequest.cpp:36 — StartCharacterCreationRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2495 | 0x00508E90 | `sub_508E90` | 有正文 | `src/CharacterCreationRequest.cpp:63 — PollCharacterCreationRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2496 | 0x00508EE0 | `sub_508EE0` | 有正文 | `src/CharacterCreationRequest.cpp:84 — HandleCharacterCreationResponse_508EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2497 | 0x00508F70 | `sub_508F70` | 有正文 | `src/CharacterDeletionRequest.cpp:31 — StartCharacterDeletionRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2498 | 0x00508FC0 | `sub_508FC0` | 有正文 | `src/CharacterDeletionRequest.cpp:52 — PollCharacterDeletionRequest` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2499 | 0x00509020 | `sub_509020` | 有正文 | `src/CharacterDeletionRequest.cpp:75 — HandleCharacterDeletionResponse_509020` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2500 | 0x00509080 | `nullsub_1` | 有正文 | `src/Input.cpp:27 — NullSub_509080` | 原入口的 RET／固定返回已用当前 IDA 核实；不是业务占位；完整接线未验 |
| 2501 | 0x00509090 | `sub_509090` | 有正文 | `src/world/SceneState.cpp:6322 — Sub_509090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2502 | 0x00509100 | `sub_509100` | 有正文 | `src/world/SceneState.cpp:6345 — Sub_509100` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2503 | 0x005093C0 | `sub_5093C0` | 有正文 | `src/world/SceneState.cpp:5938 — Sub_5093C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2504 | 0x005093F0 | `Map::FlushNew` | 有正文 | `src/world/SceneState.cpp:6451 — Sub_5093F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2505 | 0x00509550 | `sub_509550` | 有正文 | `src/world/SceneState.cpp:6509 — Sub_509550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2506 | 0x00509650 | `sub_509650` | 有正文 | `src/world/SceneState.cpp:3242 — Sub_509650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2507 | 0x00509BA0 | `sub_509BA0` | 有正文 | `src/world/SceneState.cpp:3342 — Sub_509BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2508 | 0x00509E30 | `sub_509E30` | 有正文 | `src/world/SceneState.cpp:3412 — Sub_509E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2509 | 0x00509EB0 | `sub_509EB0` | 有正文 | `src/world/SceneState.cpp:3518 — Sub_509EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2510 | 0x0050A120 | `sub_50A120` | 有正文 | `src/world/SceneState.cpp:3585 — Sub_50A120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2511 | 0x0050A300 | `sub_50A300` | 有正文 | `src/world/SceneState.cpp:4383 — Sub_50A300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2512 | 0x0050A3E0 | `sub_50A3E0` | 有正文 | `src/world/SceneState.cpp:2508 — Sub_50A3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2513 | 0x0050A6B0 | `sub_50A6B0` | 有正文 | `src/world/SceneState.cpp:4096 — Sub_50A6B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2514 | 0x0050AC20 | `sub_50AC20` | 有正文 | `src/world/SceneState.cpp:4200 — Sub_50AC20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2515 | 0x0050AEF0 | `sub_50AEF0` | 有正文 | `src/YobiRuntime.cpp:36636 — Sub_50AEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2516 | 0x0050B0D0 | `sub_50B0D0` | 有正文 | `src/world/SceneState.cpp:4241 — Sub_50B0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2517 | 0x0050B380 | `sub_50B380` | 有正文 | `src/world/SceneState.cpp:2612 — Sub_50B380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2518 | 0x0050BCD0 | `sub_50BCD0` | 有正文 | `src/world/SceneState.cpp:2803 — Sub_50BCD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2519 | 0x0050BF20 | `sub_50BF20` | 有正文 | `src/world/SceneState.cpp:3769 — Sub_50BF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2520 | 0x0050BFA0 | `sub_50BFA0` | 有正文 | `src/world/SceneState.cpp:2924 — Sub_50BFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2521 | 0x0050BFC0 | `sub_50BFC0` | 有正文 | `src/world/SceneState.cpp:2937 — Sub_50BFC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2522 | 0x0050BFD0 | `sub_50BFD0` | 有正文 | `src/world/SceneState.cpp:3819 — Sub_50BFD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2523 | 0x0050C260 | `sub_50C260` | 有正文 | `src/world/SceneState.cpp:3845 — Sub_50C260` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2524 | 0x0050C580 | `sub_50C580` | 有正文 | `src/world/SceneState.cpp:3632 — Sub_50C580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2525 | 0x0050C7A0 | `sub_50C7A0` | 有正文 | `src/world/SceneState.cpp:2955 — Sub_50C7A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2526 | 0x0050C7F0 | `sub_50C7F0` | 有正文 | `src/ui/WindowRuntime.cpp:45803 — Sub_50C7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2527 | 0x0050C870 | `sub_50C870` | 有正文 | `src/world/SceneState.cpp:7142 — Sub_50C870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2528 | 0x0050C8D0 | `sub_50C8D0` | 有正文 | `src/world/SceneState.cpp:8633 — Sub_50C8D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2529 | 0x0050C900 | `sub_50C900` | 有正文 | `src/world/SceneState.cpp:3914 — Sub_50C900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2530 | 0x0050C940 | `sub_50C940` | 有正文 | `src/world/SceneState.cpp:3056 — Sub_50C940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2531 | 0x0050CAE0 | `sub_50CAE0` | 有正文 | `src/world/SceneState.cpp:3112 — Sub_50CAE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2532 | 0x0050CE10 | `sub_50CE10` | 有正文 | `src/world/SceneState.cpp:2974 — Sub_50CE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2533 | 0x0050CF20 | `sub_50CF20` | 有正文 | `src/network/NetworkRuntime.cpp:762 — Sub_50CF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2534 | 0x0050CF60 | `sub_50CF60` | 有正文 | `src/network/NetworkRuntime.cpp:753 — Sub_50CF60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2535 | 0x0050CF70 | `sub_50CF70` | 有正文 | `src/world/SceneState.cpp:2946 — Sub_50CF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2536 | 0x0050CF90 | `sub_50CF90` | 有正文 | `src/world/SceneState.cpp:6299 — Sub_50CF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2537 | 0x0050D000 | `sub_50D000` | 有正文 | `src/world/SceneState.cpp:3217 — Sub_50D000` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2538 | 0x0050D080 | `sub_50D080` | 有正文 | `src/world/SceneState.cpp:5949 — Sub_50D080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2539 | 0x0050D120 | `sub_50D120` | 有正文 | `src/world/SceneState.cpp:6007 — Sub_50D120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2540 | 0x0050D3B0 | `sub_50D3B0` | 有正文 | `src/network/NetworkRuntime.cpp:782 — Sub_50D3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2541 | 0x0050D3C0 | `sub_50D3C0` | 有正文 | `src/world/SceneState.cpp:6122 — Sub_50D3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2542 | 0x0050D500 | `sub_50D500` | 有正文 | `src/world/SceneState.cpp:6201 — Sub_50D500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2543 | 0x0050D700 | `sub_50D700` | 有正文 | `src/network/NetworkRuntime.cpp:820 — Sub_50D700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2544 | 0x0050D740 | `sub_50D740` | 有正文 | `src/world/MailState.cpp:302 — Sub_50D740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2545 | 0x0050D8C0 | `sub_50D8C0` | 有正文 | `src/world/MailState.cpp:341 — Sub_50D8C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2546 | 0x0050D9E0 | `sub_50D9E0` | 有正文 | `src/world/MailState.cpp:369 — Sub_50D9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2547 | 0x0050DC70 | `sub_50DC70` | 有正文 | `src/world/MailState.cpp:645 — Sub_50DC70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2548 | 0x0050DD80 | `sub_50DD80` | 有正文 | `src/network/NetworkRuntime.cpp:840 — Sub_50DD80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2549 | 0x0050E110 | `sub_50E110` | 有正文 | `src/ui/WindowRuntime.cpp:45742 — Sub_50E110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2550 | 0x0050E1A0 | `sub_50E1A0` | 有正文 | `src/world/SceneState.cpp:2997 — Sub_50E1A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2551 | 0x0050E210 | `sub_50E210` | 有正文 | `src/world/SceneState.cpp:3031 — Sub_50E210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2552 | 0x0050E310 | `sub_50E310` | 有正文 | `src/network/NetworkRuntime.cpp:792 — Sub_50E310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2553 | 0x0050E340 | `sub_50E340` | 有正文 | `src/ui/WindowRuntime.cpp:45829 — Sub_50E340` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2554 | 0x0050E360 | `sub_50E360` | 有正文 | `src/ui/WindowRuntime.cpp:45838 — Sub_50E360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2555 | 0x0050E380 | `sub_50E380` | 有正文 | `src/ui/WindowRuntime.cpp:45847 — Sub_50E380` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2556 | 0x0050E3A0 | `sub_50E3A0` | 有正文 | `src/ui/WindowRuntime.cpp:45856 — Sub_50E3A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2557 | 0x0050E3C0 | `sub_50E3C0` | 有正文 | `src/ui/WindowRuntime.cpp:45865 — Sub_50E3C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2558 | 0x0050E3F0 | `sub_50E3F0` | 有正文 | `src/ui/WindowRuntime.cpp:45877 — Sub_50E3F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2559 | 0x0050E440 | `sub_50E440` | 有正文 | `src/ui/WindowRuntime.cpp:45894 — Sub_50E440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2560 | 0x0050E470 | `sub_50E470` | 有正文 | `src/ui/WindowRuntime.cpp:45909 — Sub_50E470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2561 | 0x0050E4A0 | `sub_50E4A0` | 有正文 | `src/ui/WindowRuntime.cpp:45924 — Sub_50E4A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2562 | 0x0050E4D0 | `sub_50E4D0` | 有正文 | `src/ui/WindowRuntime.cpp:45938 — Sub_50E4D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2563 | 0x0050E500 | `sub_50E500` | 有正文 | `src/ui/WindowRuntime.cpp:45953 — Sub_50E500` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2564 | 0x0050E530 | `sub_50E530` | 有正文 | `src/ui/WindowRuntime.cpp:45968 — Sub_50E530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2565 | 0x0050E570 | `sub_50E570` | 有正文 | `src/ui/WindowRuntime.cpp:45988 — Sub_50E570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2566 | 0x0050E5B0 | `sub_50E5B0` | 有正文 | `src/world/SceneState.cpp:1387 — Sub_50E5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2567 | 0x0050E5D0 | `sub_50E5D0` | 有正文 | `src/world/SceneState.cpp:1394 — Sub_50E5D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2568 | 0x0050E630 | `sub_50E630` | 有正文 | `src/network/NetworkRuntime.cpp:808 — Sub_50E630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2569 | 0x0050E650 | `sub_50E650` | 有正文 | `src/world/SceneState.cpp:3022 — Sub_50E650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2570 | 0x0050E680 | `sub_50E680` | 有正文 | `src/LegacyAudio.cpp:1548 — Sub_50E680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2571 | 0x0050E6C0 | `sub_50E6C0` | 有正文 | `src/network/NetworkRuntime.cpp:743 — Sub_50E6C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2572 | 0x0050E6E0 | `sub_50E6E0` | 有正文 | `src/network/NetworkRuntime.cpp:938 — Sub_50E6E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2573 | 0x0050E770 | `sub_50E770` | 有正文 | `src/CharacterSelectScene.cpp:201 — Sub_50E770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2574 | 0x0050E780 | `sub_50E780` | 有正文 | `src/network/NetworkRuntime.cpp:962 — Sub_50E780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2575 | 0x0050E7B0 | `sub_50E7B0` | 有正文 | `src/platform/LegacyMouseTracker.cpp:244 — ConfigureLegacyMouseTracker_50E7B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2576 | 0x0050E7D0 | `sub_50E7D0` | 有正文 | `src/ui/WindowRuntime.cpp:45776 — Sub_50E7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2577 | 0x0050E7E0 | `sub_50E7E0` | 有正文 | `src/ui/WindowRuntime.cpp:45782 — Sub_50E7E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2578 | 0x0050E7F0 | `sub_50E7F0` | 有正文 | `src/ui/WindowRuntime.cpp:45788 — Sub_50E7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2579 | 0x0050E820 | `sub_50E820` | 有正文 | `src/ui/WindowRuntime.cpp:45797 — Sub_50E820` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2580 | 0x0050E830 | `sub_50E830` | 有正文 | `src/network/NetworkRuntime.cpp:976 — Sub_50E830` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2581 | 0x0050E850 | `sub_50E850` | 有正文 | `src/LegacyRecordOrder.cpp:147 — Sub_50E850` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2582 | 0x0050EB40 | `sub_50EB40` | 有正文 | `src/LegacyRecordOrder.cpp:217 — Sub_50EB40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2583 | 0x0050EC50 | `sub_50EC50` | 有正文 | `src/LegacyRecordOrder.cpp:420 — UpdateOrderedRecordBatch_50EC50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2584 | 0x0050EEB0 | `sub_50EEB0` | 有正文 | `src/LegacyRecordOrder.cpp:389 — UpdateOrderedRecords_50EEB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2585 | 0x0050EF20 | `sub_50EF20` | 有正文 | `src/LegacyRecordOrder.cpp:328 — ResetOrderedRecord_50EF20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2586 | 0x0050EF50 | `sub_50EF50` | 有正文 | `src/LegacyRecordOrder.cpp:316 — CountRecordsValue08EqualOne_50EF50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2587 | 0x0050EF70 | `sub_50EF70` | 有正文 | `src/LegacyRecordOrder.cpp:346 — ParseOrderedRecord_50EF70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2588 | 0x0050F130 | `sub_50F130` | 有正文 | `src/network/NetworkRuntime.cpp:988 — Sub_50F130` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2589 | 0x0050F190 | `sub_50F190` | 有正文 | `src/network/NetworkRuntime.cpp:1008 — Sub_50F190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2590 | 0x0050F280 | `sub_50F280` | 有正文 | `src/LegacyRecordOrder.cpp:251 — Sub_50F280` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2591 | 0x0050F2A0 | `sub_50F2A0` | 有正文 | `src/world/MailState.cpp:1027 — Sub_50F2A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2592 | 0x0050F3A0 | `sub_50F3A0` | 有正文 | `src/world/MailState.cpp:1059 — Sub_50F3A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2593 | 0x0050F430 | `sub_50F430` | 有正文 | `src/world/MailState.cpp:767 — Sub_50F430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2594 | 0x0050F450 | `sub_50F450` | 有正文 | `src/world/MailState.cpp:1225 — Sub_50F450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2595 | 0x0050F7F0 | `sub_50F7F0` | 有正文 | `src/world/MailState.cpp:775 — Sub_50F7F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2596 | 0x0050F8E0 | `sub_50F8E0` | 有正文 | `src/world/MailState.cpp:1079 — Sub_50F8E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2597 | 0x0050F990 | `sub_50F990` | 有正文 | `src/ui/WindowRuntime.cpp:45759 — Sub_50F990` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2598 | 0x0050F9E0 | `sub_50F9E0` | 有正文 | `src/world/MailState.cpp:805 — Sub_50F9E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2599 | 0x0050FC30 | `sub_50FC30` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2600 | 0x0050FE30 | `sub_50FE30` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2601 | 0x0050FF10 | `sub_50FF10` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2602 | 0x0050FF70 | `sub_50FF70` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2603 | 0x00510120 | `sub_510120` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 2604 | 0x00510140 | `sub_510140` | 有正文 | `src/LoginRequest.cpp:95 — ReadRequestOutcomeFlag` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2605 | 0x00510150 | `sub_510150` | 有正文 | `src/world/SceneState.cpp:4033 — Sub_510150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2606 | 0x00510460 | `sub_510460` | 有正文 | `src/network/NetworkRuntime.cpp:1040 — Sub_510460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2607 | 0x00510490 | `sub_510490` | 有正文 | `src/network/NetworkRuntime.cpp:1052 — Sub_510490` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2608 | 0x00510570 | `sub_510570` | 有正文 | `src/network/NetworkRuntime.cpp:2036 — Sub_510570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2609 | 0x00510630 | `sub_510630` | 有正文 | `src/network/NetworkRuntime.cpp:2248 — Sub_510630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2610 | 0x00510710 | `sub_510710` | 有正文 | `src/network/NetworkRuntime.cpp:2305 — Sub_510710` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2611 | 0x00510810 | `sub_510810` | 有正文 | `src/network/NetworkRuntime.cpp:2448 — Sub_510810` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2612 | 0x00510880 | `sub_510880` | 有正文 | `src/network/NetworkRuntime.cpp:2618 — Sub_510880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2613 | 0x005108F0 | `sub_5108F0` | 有正文 | `src/network/NetworkRuntime.cpp:4285 — Sub_5108F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2614 | 0x00510940 | `sub_510940` | 有正文 | `src/network/NetworkRuntime.cpp:2648 — Sub_510940` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2615 | 0x005109B0 | `SendBattlePacket` | 有正文 | `src/network/NetworkRuntime.cpp:4184 — SendBattlePacket_5109B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2616 | 0x00510A00 | `sub_510A00` | 有正文 | `src/network/NetworkRuntime.cpp:4088 — Sub_510A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2617 | 0x00510AC0 | `sub_510AC0` | 有正文 | `src/network/NetworkRuntime.cpp:2371 — Sub_510AC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2618 | 0x00510B80 | `sub_510B80` | 有正文 | `src/network/NetworkRuntime.cpp:2392 — Sub_510B80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2619 | 0x00510CE0 | `sub_510CE0` | 有正文 | `src/network/NetworkRuntime.cpp:4048 — Sub_510CE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2620 | 0x00510DA0 | `sub_510DA0` | 有正文 | `src/network/NetworkRuntime.cpp:4200 — Sub_510DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2621 | 0x00510E30 | `sub_510E30` | 有正文 | `src/network/NetworkRuntime.cpp:3883 — Sub_510E30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2622 | 0x00510EC0 | `SendItemMove` | 有正文 | `src/network/NetworkRuntime.cpp:4233 — Sub_510EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2623 | 0x00510F50 | `sub_510F50` | 有正文 | `src/network/NetworkRuntime.cpp:4266 — Sub_510F50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2624 | 0x00510FA0 | `sub_510FA0` | 有正文 | `src/network/NetworkRuntime.cpp:2759 — Sub_510FA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2625 | 0x00511030 | `sub_511030` | 有正文 | `src/network/NetworkRuntime.cpp:2478 — Sub_511030` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2626 | 0x005110B0 | `sub_5110B0` | 有正文 | `src/network/NetworkRuntime.cpp:2824 — Sub_5110B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2627 | 0x00511190 | `sub_511190` | 有正文 | `src/network/NetworkRuntime.cpp:2511 — Sub_511190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2628 | 0x005111E0 | `sub_5111E0` | 有正文 | `src/network/NetworkRuntime.cpp:2678 — Sub_5111E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2629 | 0x00511250 | `sub_511250` | 有正文 | `src/network/NetworkRuntime.cpp:2527 — Sub_511250` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2630 | 0x005112C0 | `sub_5112C0` | 有正文 | `src/network/NetworkRuntime.cpp:2549 — Sub_5112C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2631 | 0x00511350 | `sub_511350` | 有正文 | `src/network/NetworkRuntime.cpp:2588 — Sub_511350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2632 | 0x005113C0 | `sub_5113C0` | 有正文 | `src/network/NetworkRuntime.cpp:2708 — Sub_5113C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2633 | 0x00511430 | `sub_511430` | 有正文 | `src/network/NetworkRuntime.cpp:2738 — Sub_511430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2634 | 0x00511480 | `sub_511480` | 有正文 | `src/network/NetworkRuntime.cpp:2785 — Sub_511480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2635 | 0x00511510 | `sub_511510` | 有正文 | `src/network/NetworkRuntime.cpp:2860 — Sub_511510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2636 | 0x005115F0 | `sub_5115F0` | 有正文 | `src/network/NetworkRuntime.cpp:2917 — Sub_5115F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2637 | 0x00511640 | `sub_511640` | 有正文 | `src/network/NetworkRuntime.cpp:2938 — Sub_511640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2638 | 0x00511760 | `sub_511760` | 有正文 | `src/network/NetworkRuntime.cpp:3013 — Sub_511760` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2639 | 0x005117D0 | `sub_5117D0` | 有正文 | `src/network/NetworkRuntime.cpp:3043 — Sub_5117D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2640 | 0x00511840 | `sub_511840` | 有正文 | `src/network/NetworkRuntime.cpp:3073 — Sub_511840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2641 | 0x005118B0 | `sub_5118B0` | 有正文 | `src/network/NetworkRuntime.cpp:3103 — Sub_5118B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2642 | 0x00511900 | `sub_511900` | 有正文 | `src/network/NetworkRuntime.cpp:3125 — Sub_511900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2643 | 0x00511A00 | `sub_511A00` | 有正文 | `src/network/NetworkRuntime.cpp:3191 — Sub_511A00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2644 | 0x00511A50 | `sub_511A50` | 有正文 | `src/network/NetworkRuntime.cpp:3212 — Sub_511A50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2645 | 0x00511AE0 | `sub_511AE0` | 有正文 | `src/network/NetworkRuntime.cpp:3251 — Sub_511AE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2646 | 0x00511BC0 | `sub_511BC0` | 有正文 | `src/network/NetworkRuntime.cpp:3308 — Sub_511BC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2647 | 0x00511C30 | `sub_511C30` | 有正文 | `src/network/NetworkRuntime.cpp:3338 — Sub_511C30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2648 | 0x00511C80 | `sub_511C80` | 有正文 | `src/network/NetworkRuntime.cpp:3359 — Sub_511C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2649 | 0x00511CD0 | `sub_511CD0` | 有正文 | `src/network/NetworkRuntime.cpp:1885 — Sub_511CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2650 | 0x00511D10 | `sub_511D10` | 有正文 | `src/network/NetworkRuntime.cpp:3380 — Sub_511D10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2651 | 0x00511D60 | `sub_511D60` | 有正文 | `src/network/NetworkRuntime.cpp:3401 — Sub_511D60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2652 | 0x00511DD0 | `sub_511DD0` | 有正文 | `src/network/NetworkRuntime.cpp:3431 — Sub_511DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2653 | 0x00511E60 | `sub_511E60` | 有正文 | `src/network/NetworkRuntime.cpp:3470 — Sub_511E60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2654 | 0x00511EF0 | `sub_511EF0` | 有正文 | `src/network/NetworkRuntime.cpp:3509 — Sub_511EF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2655 | 0x00511FB0 | `sub_511FB0` | 有正文 | `src/network/NetworkRuntime.cpp:1901 — Sub_511FB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2656 | 0x00511FF0 | `sub_511FF0` | 有正文 | `src/network/NetworkRuntime.cpp:3557 — Sub_511FF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2657 | 0x00512040 | `sub_512040` | 有正文 | `src/network/NetworkRuntime.cpp:3573 — Sub_512040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2658 | 0x005120D0 | `sub_5120D0` | 有正文 | `src/network/NetworkRuntime.cpp:3612 — Sub_5120D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2659 | 0x00512160 | `sub_512160` | 有正文 | `src/network/NetworkRuntime.cpp:3651 — Sub_512160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2660 | 0x00512190 | `sub_512190` | 有正文 | `src/network/NetworkRuntime.cpp:3661 — Sub_512190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2661 | 0x005121E0 | `sub_5121E0` | 有正文 | `src/network/NetworkRuntime.cpp:3682 — Sub_5121E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2662 | 0x00512230 | `sub_512230` | 有正文 | `src/network/NetworkRuntime.cpp:1917 — Sub_512230` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2663 | 0x00512270 | `sub_512270` | 有正文 | `src/network/NetworkRuntime.cpp:3718 — Sub_512270` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2664 | 0x005122C0 | `sub_5122C0` | 有正文 | `src/network/NetworkRuntime.cpp:3739 — Sub_5122C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2665 | 0x00512310 | `sub_512310` | 有正文 | `src/network/NetworkRuntime.cpp:3760 — Sub_512310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2666 | 0x00512360 | `sub_512360` | 有正文 | `src/network/NetworkRuntime.cpp:3781 — Sub_512360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2667 | 0x005123B0 | `sub_5123B0` | 有正文 | `src/network/NetworkRuntime.cpp:3802 — Sub_5123B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2668 | 0x00512400 | `sub_512400` | 有正文 | `src/network/NetworkRuntime.cpp:3823 — Sub_512400` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2669 | 0x00512470 | `sub_512470` | 有正文 | `src/network/NetworkRuntime.cpp:3853 — Sub_512470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2670 | 0x005124E0 | `sub_5124E0` | 有正文 | `src/network/NetworkRuntime.cpp:3911 — Sub_5124E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2671 | 0x00512570 | `sub_512570` | 有正文 | `src/network/NetworkRuntime.cpp:1933 — Sub_512570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2672 | 0x005125B0 | `sub_5125B0` | 有正文 | `src/network/NetworkRuntime.cpp:3950 — Sub_5125B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2673 | 0x00512620 | `sub_512620` | 有正文 | `src/network/NetworkRuntime.cpp:3981 — Sub_512620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2674 | 0x00512720 | `sub_512720` | 有正文 | `src/network/NetworkRuntime.cpp:4301 — Sub_512720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2675 | 0x005127B0 | `sub_5127B0` | 有正文 | `src/network/NetworkRuntime.cpp:3703 — Sub_5127B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2676 | 0x00512800 | `sub_512800` | 有正文 | `src/network/NetworkRuntime.cpp:2142 — Sub_512800` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2677 | 0x005128C0 | `sub_5128C0` | 有正文 | `src/network/NetworkRuntime.cpp:2193 — Sub_5128C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2678 | 0x00512AB0 | `sub_512AB0` | 有正文 | `src/network/NetworkRuntime.cpp:2123 — Sub_512AB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2679 | 0x00512B00 | `sub_512B00` | 有正文 | `src/network/NetworkRuntime.cpp:2084 — Sub_512B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2680 | 0x00512B90 | `sub_512B90` | 有正文 | `src/network/NetworkRuntime.cpp:1837 — Sub_512B90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2681 | 0x00512BD0 | `sub_512BD0` | 有正文 | `src/network/NetworkRuntime.cpp:1853 — Sub_512BD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2682 | 0x00512C10 | `sub_512C10` | 有正文 | `src/network/NetworkRuntime.cpp:1818 — Sub_512C10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2683 | 0x00512C60 | `sub_512C60` | 有正文 | `src/network/NetworkRuntime.cpp:1869 — Sub_512C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2684 | 0x00512CA0 | `sub_512CA0` | 有正文 | `src/network/NetworkRuntime.cpp:1800 — Report_512CA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2685 | 0x00512CF0 | `sub_512CF0` | 有正文 | `src/network/NetworkRuntime.cpp:1949 — Sub_512CF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2686 | 0x00512D30 | `sub_512D30` | 有正文 | `src/network/NetworkRuntime.cpp:1985 — Sub_512D30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2687 | 0x00512DA0 | `sub_512DA0` | 有正文 | `src/network/NetworkRuntime.cpp:2015 — Sub_512DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2688 | 0x00512DF0 | `sub_512DF0` | 有正文 | `src/network/NetworkRuntime.cpp:1965 — Sub_512DF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2689 | 0x00512E40 | `network_dispatch` | 有正文 | `src/network/NetworkRuntime.cpp:4429 — NetworkDispatch_512E40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2690 | 0x00516B60 | `sub_516B60` | 有正文 | `src/network/NetworkRuntime.cpp:4562 — AllocateNetworkWorkspace_516B60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2691 | 0x00516C90 | `sub_516C90` | 有正文 | `src/network/NetworkRuntime.cpp:4128 — Sub_516C90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2692 | 0x00516D70 | `sub_516D70` | 有正文 | `src/network/NetworkRuntime.cpp:4175 — SendIuEPacket_516D70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2693 | 0x00516DA0 | `sub_516DA0` | 有正文 | `src/network/NetworkRuntime.cpp:1177 — AllocateProtocolBuffers_516DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2694 | 0x00517040 | `sub_517040` | 有正文 | `src/network/NetworkRuntime.cpp:1330 — Sub_517040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2695 | 0x00517080 | `sub_517080` | 有正文 | `src/network/NetworkRuntime.cpp:1311 — CopyProtocolBytes_517080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2696 | 0x005170D0 | `sub_5170D0` | 有正文 | `src/network/NetworkRuntime.cpp:1364 — AppendProtocolText_5170D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2697 | 0x00517120 | `sub_517120` | 有正文 | `src/network/NetworkRuntime.cpp:1398 — FormatProtocolIntegerToken_517120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2698 | 0x00517160 | `sub_517160` | 有正文 | `src/network/NetworkRuntime.cpp:1422 — FormatProtocolTextToken_517160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2699 | 0x00517190 | `sub_517190` | 有正文 | `src/network/NetworkRuntime.cpp:1435 — ParseProtocolInteger_517190` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2700 | 0x005171B0 | `sub_5171B0` | 有正文 | `src/network/NetworkRuntime.cpp:1749 — Sub_5171B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2701 | 0x005171F0 | `sub_5171F0` | 有正文 | `src/network/NetworkRuntime.cpp:1613 — EscapeProtocolText_5171F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2702 | 0x005172D0 | `sub_5172D0` | 有正文 | `src/network/NetworkRuntime.cpp:1679 — UnescapeProtocolText_5172D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2703 | 0x005173B0 | `receivedBufferSplit` | 有正文 | `src/network/NetworkRuntime.cpp:569 — SplitReceivedBuffer_5173B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2704 | 0x00517480 | `sub_517480` | 有正文 | `src/network/NetworkRuntime.cpp:1765 — SendProtocolBuffer_517480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2705 | 0x00517510 | `sub_517510` | 有正文 | `src/network/NetworkRuntime.cpp:1128 — BuildProtocolCommand_517510` | 当前本机服务端明文头采用原命令／空格；旧命令变换／密文状态尾按获批明文合同退休，完整发送未验 |
| 2706 | 0x00517670 | `sub_517670` | 有正文 | `src/network/NetworkRuntime.cpp:1346 — FormatProtocolInteger_517670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2707 | 0x00517690 | `sub_517690` | 有正文 | `src/network/NetworkRuntime.cpp:1354 — CopyProtocolBytesAndReturn_517690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2708 | 0x005176B0 | `sub_5176B0` | 有正文 | `src/network/NetworkRuntime.cpp:612 — Sub_5176B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2709 | 0x00517730 | `sub_517730` | 有正文 | `src/network/NetworkRuntime.cpp:656 — Sub_517730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2710 | 0x00517880 | `Yobi::FrameMove` | 有正文 | `src/YobiRuntime.cpp:35307 — Sub_517880` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2711 | 0x00517DA0 | `Yobi::PlayFrame` | 有正文 | `src/YobiRuntime.cpp:35423 — PlayFrame_517DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2712 | 0x005182B0 | `sub_5182B0` | 有正文 | `src/YobiRuntime.cpp:5851 — Sub_5182B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2713 | 0x00518300 | `sub_518300` | 有正文 | `src/YobiRuntime.cpp:5872 — Sub_518300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2714 | 0x005183B0 | `sub_5183B0` | 有正文 | `src/YobiRuntime.cpp:37321 — Sub_5183B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2715 | 0x005183D0 | `sub_5183D0` | 有正文 | `src/world/SceneState.cpp:614 — InitializeLegacyPlayerRuntime_5183D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2716 | 0x005183E0 | `sub_5183E0` | 有正文 | `src/world/SceneState.cpp:598 — ConstructLegacyPlayerRuntime_5183E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2717 | 0x00518520 | `sub_518520` | 有正文 | `src/world/SceneState.cpp:622 — InitializeLegacyPetRuntime_518520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2718 | 0x00518530 | `sub_518530` | 有正文 | `src/world/SceneState.cpp:711 — ConstructPetRuntimeValues` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2719 | 0x00518560 | `sub_518560` | 有正文 | `src/world/SceneState.cpp:719 — DestroyPetRuntimeValues` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2720 | 0x00518580 | `sub_518580` | 有正文 | `src/world/SceneState.cpp:4360 — Sub_518580` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2721 | 0x005185C0 | `sub_5185C0` | 有正文 | `src/world/SceneState.cpp:4300 — Sub_5185C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2722 | 0x00518600 | `sub_518600` | 有正文 | `src/world/SceneState.cpp:3700 — Sub_518600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2723 | 0x00518660 | `sub_518660` | 有正文 | `src/world/SceneState.cpp:3685 — Sub_518660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2724 | 0x005186A0 | `sub_5186A0` | 有正文 | `src/world/SceneState.cpp:1204 — Sub_5186A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2725 | 0x00518780 | `sub_518780` | 有正文 | `src/world/SceneState.cpp:1236 — Sub_518780` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2726 | 0x00518870 | `sub_518870` | 有正文 | `src/world/SceneState.cpp:1286 — Sub_518870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2727 | 0x00518A40 | `sub_518A40` | 有正文 | `src/world/SceneState.cpp:1333 — Sub_518A40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2728 | 0x00518BE0 | `sub_518BE0` | 有正文 | `src/world/SceneState.cpp:1274 — Sub_518BE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2729 | 0x00518C20 | `sub_518C20` | 有正文 | `src/world/SceneState.cpp:1173 — InitializeAlbum_518C20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2730 | 0x00518C60 | `sub_518C60` | 有正文 | `src/world/SceneState.cpp:8696 — Sub_518C60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2731 | 0x00518C80 | `sub_518C80` | 有正文 | `src/world/MailState.cpp:859 — Sub_518C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2732 | 0x00518CB0 | `sub_518CB0` | 有正文 | `src/world/MailState.cpp:1383 — Sub_518CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2733 | 0x00518DA0 | `sub_518DA0` | 有正文 | `src/world/MailState.cpp:441 — Sub_518DA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2734 | 0x00518F00 | `sub_518F00` | 有正文 | `src/world/MailState.cpp:508 — Sub_518F00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2735 | 0x00519090 | `sub_519090` | 有正文 | `src/world/MailState.cpp:578 — Sub_519090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2736 | 0x005190F0 | `sub_5190F0` | 有正文 | `src/world/MailState.cpp:599 — Sub_5190F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2737 | 0x00519170 | `sub_519170` | 有正文 | `src/world/MailState.cpp:626 — Sub_519170` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2738 | 0x005191C0 | `sub_5191C0` | 有正文 | `src/world/MailState.cpp:418 — Sub_5191C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2739 | 0x00519290 | `sub_519290` | 有正文 | `src/world/MailState.cpp:1444 — Sub_519290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2740 | 0x005192C0 | `sub_5192C0` | 有正文 | `src/world/MailState.cpp:1462 — Sub_5192C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2741 | 0x00519300 | `sub_519300` | 有正文 | `src/world/MailState.cpp:1410 — Sub_519300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2742 | 0x00519370 | `sub_519370` | 有正文 | `src/world/MailState.cpp:1516 — Sub_519370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2743 | 0x005194C0 | `sub_5194C0` | 有正文 | `src/world/MailState.cpp:1452 — Sub_5194C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2744 | 0x005194F0 | `sub_5194F0` | 有正文 | `src/world/MailState.cpp:1497 — Sub_5194F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2745 | 0x00519550 | `sub_519550` | 有正文 | `src/world/MailState.cpp:1427 — Sub_519550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2746 | 0x005195C0 | `sub_5195C0` | 有正文 | `src/LegacyRecordOrder.cpp:481 — FindActiveOrderedRecordIndex_5195C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2747 | 0x005195F0 | `sub_5195F0` | 有正文 | `src/world/MailState.cpp:1474 — Sub_5195F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2748 | 0x00519640 | `sub_519640` | 有正文 | `src/LegacyUiRecord.cpp:13759 — Sub_519640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2749 | 0x00519680 | `sub_519680` | 有正文 | `src/ui/WindowRuntime.cpp:1322 — Sub_519680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2750 | 0x005196B0 | `sub_5196B0` | 有正文 | `src/world/SceneState.cpp:3792 — Sub_5196B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2751 | 0x00519770 | `sub_519770` | 有正文 | `src/world/SceneState.cpp:4013 — Sub_519770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2752 | 0x00519840 | `sub_519840` | 有正文 | `src/world/SceneState.cpp:4412 — Sub_519840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2753 | 0x0051985F | `?underflow@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/world/SceneState.cpp:4412 — Sub_519840` | 原共享 -1 返回尾；当前父实际失败返回承接；完整业务／运行未验 |
| 2754 | 0x00519870 | `sub_519870` | 有正文 | `src/world/SceneState.cpp:4426 — Sub_519870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2755 | 0x005198A0 | `sub_5198A0` | 有正文 | `src/world/SceneState.cpp:4438 — Sub_5198A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2756 | 0x005198D0 | `sub_5198D0` | 有正文 | `src/world/SceneState.cpp:4456 — Sub_5198D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2757 | 0x005198F0 | `sub_5198F0` | 有正文 | `src/world/SceneState.cpp:4466 — Sub_5198F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2758 | 0x00519920 | `sub_519920` | 有正文 | `src/world/SceneState.cpp:4346 — Sub_519920` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2759 | 0x00519950 | `sub_519950` | 有正文 | `src/world/SceneState.cpp:3672 — Sub_519950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2760 | 0x005199A0 | `sub_5199A0` | 有正文 | `src/world/SceneState.cpp:10534 — Sub_5199A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2761 | 0x00519A80 | `sub_519A80` | 有正文 | `src/world/SceneState.cpp:9994 — Sub_519A80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2762 | 0x00519CD0 | `sub_519CD0` | 有正文 | `src/world/SceneState.cpp:10105 — Sub_519CD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2763 | 0x00519D50 | `sub_519D50` | 有正文 | `src/world/SceneState.cpp:10351 — Sub_519D50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2764 | 0x00519E50 | `sub_519E50` | 有正文 | `src/world/SceneState.cpp:10130 — Sub_519E50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2765 | 0x00519EB0 | `sub_519EB0` | 有正文 | `src/world/SceneState.cpp:7135 — Sub_519EB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2766 | 0x00519EC0 | `sub_519EC0` | 有正文 | `src/world/SceneState.cpp:10149 — Sub_519EC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2767 | 0x00519EE0 | `sub_519EE0` | 有正文 | `src/YobiRuntime.cpp:38130 — Sub_519EE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2768 | 0x0051A040 | `sub_51A040` | 有正文 | `src/world/SceneState.cpp:10155 — Sub_51A040` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2769 | 0x0051A060 | `sub_51A060` | 有正文 | `src/world/SceneState.cpp:10166 — Sub_51A060` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2770 | 0x0051A090 | `sub_51A090` | 有正文 | `src/world/SceneState.cpp:10044 — Sub_51A090` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2771 | 0x0051A4B0 | `sub_51A4B0` | 有正文 | `src/world/SceneState.cpp:10097 — Sub_51A4B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2772 | 0x0051A5A0 | `sub_51A5A0` | 有正文 | `src/world/SceneState.cpp:10367 — Sub_51A5A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2773 | 0x0051A5B0 | `sub_51A5B0` | 有正文 | `src/world/SceneState.cpp:10462 — Sub_51A5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2774 | 0x0051A5E0 | `sub_51A5E0` | 有正文 | `src/world/SceneState.cpp:10375 — Sub_51A5E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2775 | 0x0051A5F0 | `sub_51A5F0` | 有正文 | `src/world/SceneState.cpp:10382 — Sub_51A5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2776 | 0x0051A600 | `sub_51A600` | 有正文 | `src/world/SceneState.cpp:10388 — Sub_51A600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2777 | 0x0051A610 | `sub_51A610` | 有正文 | `src/world/SceneState.cpp:10394 — Sub_51A610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2778 | 0x0051A620 | `sub_51A620` | 有正文 | `src/world/SceneState.cpp:10400 — Sub_51A620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2779 | 0x0051A630 | `sub_51A630` | 有正文 | `src/world/SceneState.cpp:10406 — Sub_51A630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2780 | 0x0051A640 | `sub_51A640` | 有正文 | `src/YobiRuntime.cpp:38103 — Sub_51A640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2781 | 0x0051A660 | `sub_51A660` | 有正文 | `src/YobiRuntime.cpp:38113 — Sub_51A660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2782 | 0x0051A680 | `sub_51A680` | 有正文 | `src/YobiRuntime.cpp:38123 — Sub_51A680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2783 | 0x0051A6A0 | `sub_51A6A0` | 有正文 | `src/YobiRuntime.cpp:36748 — Sub_51A6A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2784 | 0x0051A840 | `sub_51A840` | 有正文 | `src/world/SceneState.cpp:10176 — Sub_51A840` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2785 | 0x0051B080 | `sub_51B080` | 有正文 | `src/world/SceneState.cpp:9072 — Sub_51B080` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2786 | 0x0051B0F0 | `sub_51B0F0` | 有正文 | `src/world/SceneState.cpp:9064 — Sub_51B0F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2787 | 0x0051B110 | `sub_51B110` | 有正文 | `src/LoginRequest.cpp:64 — Sub_51B110` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2788 | 0x0051B140 | `sub_51B140` | 有正文 | `src/LoginRequest.cpp:73 — CharacterNameExists_51B140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2789 | 0x0051B1B0 | `sub_51B1B0` | 有正文 | `src/CharacterListRequest.cpp:56 — WriteCharacterRecord_51B1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2790 | 0x0051B3B0 | `sub_51B3B0` | 有正文 | `src/LoginRequest.cpp:54 — Sub_51B3B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2791 | 0x0051B3E0 | `sub_51B3E0` | 有正文 | `src/world/SceneState.cpp:9516 — Sub_51B3E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2792 | 0x0051B530 | `sub_51B530` | 有正文 | `src/world/SceneState.cpp:3877 — Sub_51B530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2793 | 0x0051B5F0 | `sub_51B5F0` | 有正文 | `src/world/SceneState.cpp:3898 — Sub_51B5F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2794 | 0x0051B630 | `sub_51B630` | 有正文 | `src/world/SceneState.cpp:3928 — Sub_51B630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2795 | 0x0051B740 | `sub_51B740` | 有正文 | `src/CharacterSelectPreview.cpp:293 — Sub_51B740` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2796 | 0x0051B7D0 | `sub_51B7D0` | 有正文 | `src/CharacterSelectPreview.cpp:329 — Sub_51B7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2797 | 0x0051B910 | `sub_51B910` | 有正文 | `src/CharacterSelectPreview.cpp:383 — Sub_51B910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2798 | 0x0051BA10 | `sub_51BA10` | 有正文 | `src/CharacterSelectPreview.cpp:102 — Sub_51BA10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2799 | 0x0051BAC0 | `sub_51BAC0` | 有正文 | `src/CharacterSelectPreview.cpp:157 — Sub_51BAC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2800 | 0x0051BB80 | `sub_51BB80` | 有正文 | `src/CharacterSelectPreview.cpp:130 — Sub_51BB80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2801 | 0x0051BC50 | `sub_51BC50` | 有正文 | `src/CharacterSelectPreview.cpp:261 — Sub_51BC50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2802 | 0x0051BD00 | `sub_51BD00` | 有正文 | `src/CharacterSelectPreview.cpp:185 — Sub_51BD00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2803 | 0x0051BDB0 | `sub_51BDB0` | 有正文 | `src/world/SceneState.cpp:10412 — Sub_51BDB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2804 | 0x0051BDC0 | `sub_51BDC0` | 有正文 | `src/world/SceneState.cpp:10418 — Sub_51BDC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2805 | 0x0051BDD0 | `sub_51BDD0` | 有正文 | `src/world/SceneState.cpp:10473 — Sub_51BDD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2806 | 0x0051BDF0 | `sub_51BDF0` | 有正文 | `src/world/SceneState.cpp:10481 — Sub_51BDF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2807 | 0x0051BE10 | `sub_51BE10` | 有正文 | `src/world/SceneState.cpp:10428 — Sub_51BE10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2808 | 0x0051BE20 | `sub_51BE20` | 有正文 | `src/world/SceneState.cpp:10434 — Sub_51BE20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2809 | 0x0051BE30 | `sub_51BE30` | 有正文 | `src/world/SceneState.cpp:10440 — Sub_51BE30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2810 | 0x0051BE40 | `sub_51BE40` | 有正文 | `src/world/SceneState.cpp:10450 — Sub_51BE40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2811 | 0x0051BE50 | `sub_51BE50` | 有正文 | `src/world/SceneState.cpp:10501 — Sub_51BE50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2812 | 0x0051BEC0 | `sub_51BEC0` | 有正文 | `src/world/SceneState.cpp:10492 — Sub_51BEC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2813 | 0x0051BEE0 | `sub_51BEE0` | 有正文 | `src/world/SceneState.cpp:10456 — Sub_51BEE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2814 | 0x0051BEF0 | `sub_51BEF0` | 有正文 | `src/world/SceneState.cpp:573 — Sub_51BEF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2815 | 0x0051BF00 | `sub_51BF00` | 有正文 | `src/CharacterSelectPreview.cpp:218 — Sub_51BF00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2816 | 0x0051BFF0 | `sub_51BFF0` | 有正文 | `src/CharacterSelectPreview.cpp:240 — Sub_51BFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2817 | 0x0051C0D0 | `mappingCharaImageNo` | 有正文 | `src/world/SceneState.cpp:10305 — Sub_51C0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2818 | 0x0051C410 | `sub_51C410` | 有正文 | `src/world/SceneState.cpp:649 — IsValidPetIndex_51C410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2819 | 0x0051C430 | `World::Update` | 有正文 | `src/world/WorldUpdate.cpp:27 — Update` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2820 | 0x0051C770 | `sub_51C770` | 有正文 | `src/world/SceneState.cpp:1106 — SetCurrentSceneType` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2821 | 0x0051C790 | `sub_51C790` | 有正文 | `src/world/SceneState.cpp:1114 — SetCurrentSceneTypeAndState` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2822 | 0x0051C7B0 | `sub_51C7B0` | 有正文 | `src/world/SceneState.cpp:1122 — SetPendingSceneType` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2823 | 0x0051C7D0 | `World::DrawNormalScene` | 有正文 | `src/world/SceneState.cpp:6545 — DrawNormalScene_51C7D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2824 | 0x0051D5B0 | `sub_51D5B0` | 有正文 | `src/world/SceneState.cpp:7598 — Sub_51D5B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2825 | 0x0051D640 | `sub_51D640` | 有正文 | `src/world/SceneState.cpp:7041 — Sub_51D640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2826 | 0x0051D6F0 | `sub_51D6F0` | 有正文 | `src/world/SceneState.cpp:7079 — Sub_51D6F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2827 | 0x0051D750 | `sub_51D750` | 有正文 | `src/world/SceneState.cpp:7066 — Sub_51D750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2828 | 0x0051D790 | `sub_51D790` | 有正文 | `src/world/SceneState.cpp:10751 — Sub_51D790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2829 | 0x0051E0D0 | `sub_51E0D0` | 有正文 | `src/world/SceneState.cpp:11051 — Sub_51E0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2830 | 0x0051EC10 | `sub_51EC10` | 有正文 | `src/world/SceneState.cpp:11501 — Sub_51EC10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2831 | 0x0051EF80 | `sub_51EF80` | 有正文 | `src/world/SceneState.cpp:11666 — Sub_51EF80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2832 | 0x0051F610 | `sub_51F610` | 有正文 | `src/world/SceneState.cpp:11850 — Sub_51F610` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2833 | 0x0051F860 | `sub_51F860` | 有正文 | `src/world/SceneState.cpp:12005 — Sub_51F860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2834 | 0x0051FA70 | `sub_51FA70` | 有正文 | `src/world/SceneState.cpp:12011 — Sub_51FA70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2835 | 0x0051FC90 | `sub_51FC90` | 有正文 | `src/world/SceneState.cpp:12017 — Sub_51FC90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2836 | 0x0051FDE0 | `sub_51FDE0` | 有正文 | `src/world/WorldUpdate.cpp:364 — Sub_51FDE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2837 | 0x00520160 | `sub_520160` | 有正文 | `src/LoginSavedState.cpp:1003 — Sub_520160` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2838 | 0x005201B0 | `sub_5201B0` | 有正文 | `src/LoginSavedState.cpp:803 — RestoreCurrentSavedState_5201B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2839 | 0x00520200 | `sub_520200` | 有正文 | `src/LoginSavedState.cpp:1026 — Sub_520200` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2840 | 0x005202E0 | `sub_5202E0` | 有正文 | `src/platform/LegacyModuleDigest.cpp:252 — ComputeExecutableDigest_5202E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2841 | 0x005203B0 | `sub_5203B0` | 有正文 | `src/LoginInput.cpp:130 — WriteInputValueFF494F_5203B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2842 | 0x005203E0 | `sub_5203E0` | 有正文 | `src/LoginInput.cpp:138 — ReadInputValueFF494F_5203E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2843 | 0x00520410 | `sub_520410` | 有正文 | `src/LoginInput.cpp:149 — WriteInputValueFF4982_520410` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2844 | 0x00520440 | `sub_520440` | 有正文 | `src/LoginInput.cpp:157 — ReadInputValueFF4982_520440` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2845 | 0x00520470 | `sub_520470` | 有正文 | `src/LoginSavedState.cpp:907 — Sub_520470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2846 | 0x005204C0 | `sub_5204C0` | 有正文 | `src/LoginSavedState.cpp:705 — RestoreAudioSettings_5204C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2847 | 0x00520510 | `sub_520510` | 有正文 | `src/LoginSavedState.cpp:848 — Sub_520510` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2848 | 0x00520550 | `sub_520550` | 有正文 | `src/LoginSavedState.cpp:869 — Sub_520550` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2849 | 0x00520590 | `sub_520590` | 有正文 | `src/LoginSavedState.cpp:939 — Sub_520590` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2850 | 0x005205E0 | `sub_5205E0` | 有正文 | `src/LoginSavedState.cpp:724 — RestoreSavedScalarSettings_5205E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2851 | 0x00520640 | `sub_520640` | 有正文 | `src/LoginSavedState.cpp:965 — Sub_520640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2852 | 0x00520690 | `sub_520690` | 有正文 | `src/LoginSavedState.cpp:743 — RestoreSavedInputContexts_520690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2853 | 0x005206F0 | `sub_5206F0` | 有正文 | `src/LoginSavedState.cpp:981 — Sub_5206F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2854 | 0x00520700 | `sub_520700` | 有正文 | `src/LoginSavedState.cpp:777 — RestoreSavedByteState_520700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2855 | 0x00520720 | `sub_520720` | 有正文 | `src/LoginSavedState.cpp:992 — Sub_520720` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2856 | 0x00520730 | `sub_520730` | 有正文 | `src/LoginSavedState.cpp:787 — RestoreCurrentSavedValue_520730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2857 | 0x00520750 | `sub_520750` | 有正文 | `src/LoginSavedState.cpp:885 — Sub_520750` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2858 | 0x00520770 | `sub_520770` | 有正文 | `src/LoginSavedState.cpp:896 — Sub_520770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2859 | 0x00520790 | `Renderer::SortTexture` | 有正文 | `src/render/GraphicPendingQueue.cpp:395 — SortTextures_520790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2860 | 0x005208F0 | `sub_5208F0` | 有正文 | `src/render/GraphicPendingQueue.cpp:198 — BackupValues_5208F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2861 | 0x00520910 | `sub_520910` | 有正文 | `src/render/GraphicPendingQueue.cpp:208 — Sub_520910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2862 | 0x00520930 | `sub_520930` | 有正文 | `src/render/GraphicPendingQueue.cpp:214 — Sub_520930` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2863 | 0x00520970 | `Renderer::DrawGraphic3` | 有正文 | `src/DrawGraphic.cpp:50 — DrawGraphic3_520970` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2864 | 0x00520AD0 | `Renderer::FillDirtyArea` | 有正文 | `src/render/GraphicPendingQueue.cpp:286 — FillDirtyArea_520AD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2865 | 0x00520B10 | `Graphic::LoadTexture` | 有正文 | `src/render/GraphicDxCache.cpp:534 — LoadGraphicTexture_520B10` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2866 | 0x00520DD0 | `Renderer::DrawAllQueued` | 有正文 | `src/render/DrawAllQueued.cpp:346 — DrawAllQueued_520DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2867 | 0x00521870 | `sub_521870` | 有正文 | `src/render/GraphicPendingQueue.cpp:315 — Sub_521870` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2868 | 0x005219C0 | `sub_5219C0` | 有正文 | `src/render/GraphicPendingQueue.cpp:49 — CalculateRippleScreenDelta` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2869 | 0x00521A30 | `Renderer::QueueText` | 有正文 | `src/render/TextPendingQueue.cpp:80 — QueueText_521A30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2870 | 0x00521AA0 | `sub_521AA0` | 有正文 | `src/render/GraphicPendingQueue.cpp:229 — QueueMap_521AA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2871 | 0x00521B10 | `Renderer::DrawGraphic` | 有正文 | `src/DrawGraphic.cpp:275 — DrawGraphic` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2872 | 0x00521C80 | `sub_521C80` | 有正文 | `src/DrawGraphic.cpp:166 — Sub_521C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2873 | 0x00521DD0 | `Renderer::DrawGraphic2` | 有正文 | `src/DrawGraphic.cpp:218 — DrawGraphic2_521DD0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2874 | 0x00521F20 | `World::RenderChara` | 有正文 | `src/YobiRuntime.cpp:38165 — RenderChara_521F20` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2875 | 0x00522180 | `Renderer::DrawGraphic_Hover` | 有正文 | `src/DrawGraphicHover.cpp:9 — DrawGraphicHover` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2876 | 0x00522210 | `sub_522210` | 有正文 | `src/render/GraphicPendingQueue.cpp:259 — Sub_522210` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2877 | 0x005222A0 | `sub_5222A0` | 有正文 | `src/render/GraphicPendingQueue.cpp:30 — CompareGraphicCache3_5222A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2878 | 0x005222D0 | `Renderer::TakeGraphicDxCache` | 有正文 | `src/render/GraphicDxCache.cpp:436 — TakeGraphicDxCache_5222D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2879 | 0x00522570 | `Graphic::LoadGraphic` | 有正文 | `src/render/GraphicDxCache.cpp:365 — LoadGraphic_522570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2880 | 0x00522620 | `sub_522620` | 有正文 | `src/GraphicRead.cpp:370 — InitializeIndependentPaletteQueue_522620` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2881 | 0x00522630 | `sub_522630` | 有正文 | `src/GraphicRead.cpp:197 — ConstructIndependentPaletteQueue_522630` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2882 | 0x00522660 | `sub_522660` | 有正文 | `src/GraphicRead.cpp:204 — DestroyIndependentPaletteQueue_522660` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2883 | 0x00522690 | `sub_522690` | 有正文 | `src/GraphicRead.cpp:377 — InitializePaletteDbfState_522690` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2884 | 0x005226A0 | `unknown_libname_48` | 有正文 | `src/GraphicRead.cpp:210 — ConstructPaletteDbfState_5226A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2885 | 0x00522900 | `sub_522900` | 有正文 | `src/GraphicRead.cpp:227 — LoadIndependentPaletteIndex_522900` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2886 | 0x00522B30 | `sub_522B30` | 有正文 | `src/GraphicRead.cpp:483 — InitializeIndependentPalettes_522B30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2887 | 0x00522C50 | `sub_522C50` | 有正文 | `src/GraphicRead.cpp:527 — ReleaseIndependentPalettes_522C50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2888 | 0x00522CB0 | `sub_522CB0` | 有正文 | `src/GraphicRead.cpp:293 — LoadIndependentPalette32_522CB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2889 | 0x00523140 | `Graphic::LoadPatelet` | 有正文 | `src/GraphicRead.cpp:539 — RefreshIndependentPalette32_523140` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2890 | 0x005231A0 | `Renderer::BuildSurfaceCache` | 有正文 | `src/render/GraphicDxCache.cpp:684 — BuildSurfaceCache_5231A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2891 | 0x00523310 | `sub_523310` | 有正文 | `src/render/GraphicDxCache.cpp:321 — ResetTextureNodes_523310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2892 | 0x00523350 | `sub_523350` | 有正文 | `src/render/GraphicDxCache.cpp:337 — InvalidateGraphicRefs_523350` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2893 | 0x00523390 | `sub_523390` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2894 | 0x00523450 | `sub_523450` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2895 | 0x005234B0 | `nullsub_2` | 有承接 | `src/GraphicRead.cpp:172 — decodedBlocks.clear();` | 原为 RET 8 的非释放叶；指针载荷不归该叶释放，实际 vector 由宿主构析；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2896 | 0x005234C0 | `sub_5234C0` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2897 | 0x005235B0 | `sub_5235B0` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | 原迭代器字段操作；当前三个实际 std::map 的迭代器由 STL 承接；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2898 | 0x005235C0 | `sub_5235C0` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2899 | 0x00523690 | `sub_523690` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2900 | 0x005236B0 | `Vector::void::insert` | 有承接 | `src/GraphicRead.cpp:261 — decodedBlocks.push_back(decoded);` | 原 vector<void*> 插入；当前实际 vector push_back，另有图号／请求集合消费；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2901 | 0x005238C0 | `sub_5238C0` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2902 | 0x00523C90 | `sub_523C90` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2903 | 0x00523CD0 | `sub_523CD0` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2904 | 0x00523D20 | `sub_523D20` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2905 | 0x00523F90 | `sub_523F90` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2906 | 0x00523FB0 | `sub_523FB0` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2907 | 0x00523FD0 | `sub_523FD0` | 有承接 | `src/GraphicRead.cpp:153 — struct PaletteDbfState` | PaletteDbfState::records 的实际 std::map 构析、clear、try_emplace 与迭代器操作；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
| 2908 | 0x00524030 | `sub_524030` | 有承接 | `src/GraphicRead.cpp:159 — std::vector<std::uint8_t*> decodedBlocks;` | 原指针区间拷贝叶；当前实际 vector 维护元素序列；原库内／内联容器叶不另造包装；完整 owner／运行未验 |
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
| 2933 | 0x0052CD30 | `sub_52CD30` | 有正文 | `src/platform/MainLoopSupport.cpp:568 — Sub_52CD30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2934 | 0x0052CD40 | `sub_52CD40` | 有正文 | `src/LegacyAudio.cpp:963 — Sub_52CD40` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2935 | 0x0052CD60 | `sub_52CD60` | 有正文 | `src/LegacyAudio.cpp:1331 — Sub_52CD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2936 | 0x0052CDE0 | `sub_52CDE0` | 有正文 | `src/LegacyAudio.cpp:995 — Sub_52CDE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2937 | 0x0052CEA0 | `sub_52CEA0` | 有正文 | `src/network/NetworkRuntime.cpp:1444 — FindNextDelimitedField_52CEA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2938 | 0x0052CED0 | `sub_52CED0` | 有正文 | `src/network/NetworkRuntime.cpp:1512 — ExtractDelimitedField_52CED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2939 | 0x0052CF30 | `sub_52CF30` | 有正文 | `src/network/NetworkRuntime.cpp:1471 — CopyDelimitedField_52CF30` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2940 | 0x0052CFA0 | `sub_52CFA0` | 有正文 | `src/network/NetworkRuntime.cpp:1532 — ParseDelimitedInteger_52CFA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2941 | 0x0052CFF0 | `sub_52CFF0` | 有正文 | `src/network/NetworkRuntime.cpp:1547 — ParseDelimitedIntegerStatus_52CFF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2942 | 0x0052D050 | `base62Decode` | 有正文 | `src/network/NetworkRuntime.cpp:1564 — Base62Decode_52D050` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2943 | 0x0052D0D0 | `sub_52D0D0` | 有正文 | `src/network/NetworkRuntime.cpp:1598 — ParseDelimitedBase62_52D0D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2944 | 0x0052D120 | `sub_52D120` | 有正文 | `src/Input.cpp:784 — DecodeEscapeLetter_52D120` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2945 | 0x0052D150 | `Unescape` | 有正文 | `src/Input.cpp:799 — UnescapeLegacyText_52D150` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2946 | 0x0052D1B0 | `sub_52D1B0` | 有正文 | `src/Input.cpp:694 — EscapeLegacyText_52D1B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2947 | 0x0052D2D0 | `sub_52D2D0` | 有正文 | `src/Input.cpp:772 — EscapeInputText_52D2D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2948 | 0x0052D2F0 | `Unescape2` | 有正文 | `src/Input.cpp:828 — UnescapeLegacyTextAndNotify_52D2F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2949 | 0x0052D310 | `sub_52D310` | 有正文 | `src/LoginSavedState.cpp:87 — EncodeLegacyBytes_52D310` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2950 | 0x0052D460 | `sub_52D460` | 有正文 | `src/LoginSavedState.cpp:169 — DecodeLegacyBytes_52D460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2951 | 0x0052D570 | `sub_52D570` | 有正文 | `src/Input.cpp:902 — RemoveLegacyBytes_52D570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2952 | 0x0052D5C0 | `Graphic::readGraphicData` | 有正文 | `src/GraphicDecode.cpp:6 — ReadGraphicData` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2953 | 0x0052D790 | `sub_52D790` | 有正文 | `src/LegacyUiRecord.cpp:9778 — Sub_52D790` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2954 | 0x0052D7B0 | `sub_52D7B0` | 有正文 | `src/LegacyUiRecord.cpp:9785 — Sub_52D7B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2955 | 0x0052DC60 | `sub_52DC60` | 有正文 | `src/LegacyUiRecord.cpp:9904 — Sub_52DC60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2956 | 0x0052DCE0 | `sub_52DCE0` | 有正文 | `src/LegacyUiRecord.cpp:9916 — Sub_52DCE0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2957 | 0x0052E180 | `sub_52E180` | 有正文 | `src/world/SceneState.cpp:3980 — Sub_52E180` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2958 | 0x0052E1D0 | `sub_52E1D0` | 有正文 | `src/world/SceneState.cpp:3998 — Sub_52E1D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2959 | 0x0052E240 | `sub_52E240` | 有正文 | `src/LegacyUiRecord.cpp:10028 — Sub_52E240` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2960 | 0x0052E600 | `sub_52E600` | 有正文 | `src/LegacyUiRecord.cpp:10104 — Sub_52E600` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2961 | 0x0052E650 | `sub_52E650` | 有正文 | `src/LegacyUiRecord.cpp:10123 — Sub_52E650` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2962 | 0x0052ECB0 | `sub_52ECB0` | 有正文 | `src/LegacyUiRecord.cpp:10246 — Sub_52ECB0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2963 | 0x0052ED50 | `sub_52ED50` | 有正文 | `src/LegacyUiRecord.cpp:10262 — Sub_52ED50` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2964 | 0x0052F460 | `sub_52F460` | 有正文 | `src/LegacyUiRecord.cpp:10414 — Sub_52F460` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2965 | 0x0052F670 | `sub_52F670` | 有正文 | `src/LegacyUiRecord.cpp:10454 — Sub_52F670` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2966 | 0x0052F910 | `sub_52F910` | 有正文 | `src/LegacyUiRecord.cpp:10515 — Sub_52F910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2967 | 0x0052FB60 | `sub_52FB60` | 有正文 | `src/LegacyRecordOrder.cpp:270 — CompareOrderedRecords_52FB60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2968 | 0x0052FCC0 | `sub_52FCC0` | 有正文 | `src/LegacyRecordOrder.cpp:304 — BuildOrderedRecordIndices_52FCC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2969 | 0x0052FCF0 | `sub_52FCF0` | 有正文 | `src/LegacyUiRecord.cpp:10571 — Sub_52FCF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2970 | 0x0052FD60 | `sub_52FD60` | 有正文 | `src/LegacyUiRecord.cpp:10594 — Sub_52FD60` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2971 | 0x00530B70 | `sub_530B70` | 有正文 | `src/LegacyUiRecord.cpp:10807 — Sub_530B70` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2972 | 0x00530BA0 | `sub_530BA0` | 有正文 | `src/LegacyUiRecord.cpp:10817 — Sub_530BA0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2973 | 0x00532430 | `sub_532430` | 有正文 | `src/LegacyUiRecord.cpp:11161 — Sub_532430` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2974 | 0x00532450 | `sub_532450` | 有正文 | `src/LegacyUiRecord.cpp:11170 — Sub_532450` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2975 | 0x00532D80 | `sub_532D80` | 有正文 | `src/LegacyRecordOrder.cpp:263 — Sub_532D80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2976 | 0x00532E70 | `sub_532E70` | 有正文 | `src/platform/StartupCommon.cpp:26 — CreateSaveDirectories` | 当前重命名定义与原入口身份已定位；完整业务／接线／运行未验 |
| 2977 | 0x00532ED0 | `sub_532ED0` | 有正文 | `src/LoginSavedState.cpp:231 — LoadSavedPhysicalRecord_532ED0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2978 | 0x00532FC0 | `sub_532FC0` | 有正文 | `src/LoginSavedState.cpp:276 — WriteDefaultSavedFile_532FC0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2979 | 0x005331A0 | `sub_5331A0` | 有正文 | `src/LoginSavedState.cpp:390 — InitializeSavedPhysicalRecord_5331A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2980 | 0x00533300 | `sub_533300` | 有正文 | `src/LoginSavedState.cpp:820 — InitializeSavedState_533300` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2981 | 0x00533360 | `sub_533360` | 有正文 | `src/LoginSavedState.cpp:466 — ResetSavedState_533360` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2982 | 0x005333D0 | `sub_5333D0` | 有正文 | `src/LoginSavedState.cpp:489 — ProbeSavedStateLocation_5333D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2983 | 0x00533470 | `sub_533470` | 有正文 | `src/LoginSavedState.cpp:528 — RebuildSavedSlotMap_533470` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2984 | 0x00533530 | `sub_533530` | 有正文 | `src/LoginSavedState.cpp:578 — MigrateLegacySavedFiles_533530` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2985 | 0x00533570 | `sub_533570` | 有正文 | `src/LoginSavedState.cpp:1111 — Sub_533570` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2986 | 0x005335B0 | `sub_5335B0` | 有正文 | `src/LoginSavedState.cpp:1131 — Sub_5335B0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2987 | 0x005335F0 | `sub_5335F0` | 有正文 | `src/LoginSavedState.cpp:1171 — Sub_5335F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2988 | 0x00533640 | `sub_533640` | 有正文 | `src/LoginSavedState.cpp:1151 — Sub_533640` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2989 | 0x00533680 | `sub_533680` | 有正文 | `src/LoginSavedState.cpp:1198 — Sub_533680` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2990 | 0x005336D0 | `sub_5336D0` | 有正文 | `src/LoginSavedState.cpp:1066 — Sub_5336D0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2991 | 0x00533700 | `sub_533700` | 有正文 | `src/LoginSavedState.cpp:608 — StoreCurrentRecordToSelectedPhysical_533700` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2992 | 0x00533730 | `sub_533730` | 有正文 | `src/LoginSavedState.cpp:1225 — Sub_533730` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2993 | 0x00533860 | `sub_533860` | 有正文 | `src/LoginSavedState.cpp:1072 — Sub_533860` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2994 | 0x005338F0 | `sub_5338F0` | 有正文 | `src/LoginSavedState.cpp:517 — FindFreeSavedPhysicalSlot_5338F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2995 | 0x00533910 | `sub_533910` | 有正文 | `src/world/MailState.cpp:868 — Sub_533910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2996 | 0x00533950 | `sub_533950` | 有正文 | `src/world/MailState.cpp:879 — Sub_533950` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2997 | 0x00533B00 | `sub_533B00` | 有正文 | `src/world/MailState.cpp:905 — Sub_533B00` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2998 | 0x00533C80 | `sub_533C80` | 有正文 | `src/world/MailState.cpp:934 — Sub_533C80` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 2999 | 0x00534290 | `sub_534290` | 有正文 | `src/world/MailState.cpp:1017 — Sub_534290` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3000 | 0x005342E0 | `sub_5342E0` | 有正文 | `src/ui/WindowRuntime.cpp:45523 — Sub_5342E0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3001 | 0x005342F0 | `sub_5342F0` | 有正文 | `src/world/MailState.cpp:1123 — Sub_5342F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3002 | 0x00534370 | `sub_534370` | 有正文 | `src/world/MailState.cpp:674 — Sub_534370` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3003 | 0x005343A0 | `sub_5343A0` | 有正文 | `src/world/MailState.cpp:686 — Sub_5343A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3004 | 0x005343C0 | `sub_5343C0` | 有正文 | `src/world/MailState.cpp:729 — Sub_5343C0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3005 | 0x00534480 | `sub_534480` | 有正文 | `src/world/MailState.cpp:750 — Sub_534480` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3006 | 0x00534520 | `sub_534520` | 有正文 | `src/world/MailState.cpp:704 — Sub_534520` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3007 | 0x005345A0 | `sub_5345A0` | 有正文 | `src/world/MailState.cpp:1316 — Sub_5345A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3008 | 0x00534770 | `sub_534770` | 有正文 | `src/world/MailState.cpp:1345 — Sub_534770` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3009 | 0x005347F0 | `sub_5347F0` | 有正文 | `src/world/MailState.cpp:1362 — Sub_5347F0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3010 | 0x005348A0 | `sub_5348A0` | 有正文 | `src/world/MailState.cpp:1282 — Sub_5348A0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3011 | 0x00534910 | `sub_534910` | 有正文 | `src/world/MailState.cpp:1299 — Sub_534910` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3012 | 0x00534980 | `DirectDrawCreateEx` | 有承接 | `src/render/SdlPresenter.cpp:376 — CreateRendererPresenter` | 原 DirectDrawCreateEx import 跳板；当前渲染创建由实际 SDL presenter 承接；完整上层／运行未验 |
| 3013 | 0x00534986 | `ImmGetConversionStatus` | 有承接 | `src/platform/ImeLifecycle.cpp:37 — ImmGetConversionStatus` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3014 | 0x0053498C | `ImmAssociateContext` | 有承接 | `src/platform/ImeLifecycle.cpp:31 — ImmAssociateContext` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3015 | 0x00534992 | `ImmCreateContext` | 有承接 | `src/platform/ImeLifecycle.cpp:28 — ImmCreateContext` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3016 | 0x00534998 | `ImmReleaseContext` | 有承接 | `src/platform/ImeLifecycle.cpp:50 — ImmReleaseContext` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3017 | 0x0053499E | `ImmIsIME` | 有承接 | `src/platform/ImeLifecycle.cpp:65 — ImmIsIME` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3018 | 0x005349A4 | `ImmSetOpenStatus` | 有承接 | `src/platform/ImeLifecycle.cpp:79 — ImmSetOpenStatus` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3019 | 0x005349AA | `ImmGetDescriptionA` | 有承接 | `src/world/RenderInput.cpp:251 — ImmGetDescriptionA` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3020 | 0x005349B0 | `ImmGetCompositionStringA` | 有承接 | `src/platform/ImeComposition.cpp:30 — ImmGetCompositionStringA` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3021 | 0x005349B6 | `ImmGetCandidateListA` | 有承接 | `src/platform/WindowProcedure.cpp:427 — ImmGetCandidateListA` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3022 | 0x00534A30 | `send` | 有承接 | `src/network/NetworkRuntime.cpp:103 — send` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3023 | 0x00534A36 | `closesocket` | 有承接 | `src/network/NetworkRuntime.cpp:4684 — closesocket` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3024 | 0x00534A3C | `recv` | 有承接 | `src/network/NetworkRuntime.cpp:4666 — recv` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3025 | 0x00534A42 | `__WSAFDIsSet` | 有承接 | `src/network/NetworkRuntime.cpp:4662 — FD_ISSET` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3026 | 0x00534A48 | `select` | 有承接 | `src/network/NetworkRuntime.cpp:854 — select` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3027 | 0x00534A4E | `WSAStartup` | 有承接 | `src/network/NetworkRuntime.cpp:4599 — WSAStartup` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3028 | 0x00534A54 | `WSACleanup` | 有承接 | `src/network/NetworkRuntime.cpp:4881 — WSACleanup` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3029 | 0x00534A5A | `WSAGetLastError` | 有承接 | `src/network/NetworkRuntime.cpp:725 — WSAGetLastError` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3030 | 0x00534A60 | `connect` | 有承接 | `src/LoginState.cpp:151 — connect` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3031 | 0x00534A66 | `gethostbyname` | 有承接 | `src/LoginState.cpp:181 — gethostbyname` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3032 | 0x00534A6C | `inet_addr` | 有承接 | `src/LoginState.cpp:179 — inet_addr` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3033 | 0x00534A72 | `htons` | 有承接 | `src/LoginState.cpp:178 — htons` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3034 | 0x00534A78 | `setsockopt` | 有承接 | `src/LoginState.cpp:166 — setsockopt` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3035 | 0x00534A7E | `ioctlsocket` | 有承接 | `src/LoginState.cpp:162 — ioctlsocket` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3036 | 0x00534A84 | `socket` | 有承接 | `src/LoginState.cpp:151 — socket` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3037 | 0x00534A8A | `Module32Next` | 有承接 | `src/platform/MainLoopSupport.cpp:169 — Module32Next` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3038 | 0x00534A90 | `Module32First` | 有承接 | `src/platform/MainLoopSupport.cpp:145 — Module32First` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3039 | 0x00534A96 | `CreateToolhelp32Snapshot` | 有承接 | `src/platform/MainLoopSupport.cpp:138 — CreateToolhelp32Snapshot` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3040 | 0x00534A9C | `Process32Next` | 有承接 | `src/platform/MainLoopSupport.cpp:394 — Process32Next` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3041 | 0x00534AA2 | `Process32First` | 有承接 | `src/platform/MainLoopSupport.cpp:388 — Process32First` | 原入口为对应 Windows／Winsock import 跳板；当前真实 SDK 调用存在，完整调用方／运行未验 |
| 3042 | 0x00534AA8 | `DirectSoundCreate` | 有承接 | `src/platform/SdlAudioBackend.cpp:278 — SdlAudioBackend::Open` | 原 DirectSoundCreate 为 import 跳板；当前 Sub_524230 实际调用 SDL 后端 Open；不造旧 SDK 包装，完整上层／运行未验 |
| 3043 | 0x00534AB0 | `sub_534AB0` | 已排除 | `src/platform/LegacyExternalKeyboard.cpp:170 — Sub_534AB0` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3044 | 0x00534B80 | `DialogFunc` | 已排除 | `src/platform/LegacyExternalKeyboard.cpp:44 — DialogFunc_534B80` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3045 | 0x00534EF0 | `sub_534EF0` | 有正文 | `src/platform/LegacyNetworkAdapter.cpp:16 — Sub_534EF0` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3046 | 0x00535090 | `sub_535090` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3047 | 0x00535290 | `sub_535290` | 已排除 | `src/platform/LegacyExternalKeyboardObject.cpp:19 — DeleteExternalKeyboardVirtual_535290` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3048 | 0x00535310 | `sub_535310` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3049 | 0x005354C0 | `sub_5354C0` | 已排除 | `src/platform/LegacyExternalKeyboardObject.cpp:41 — GetExternalKeyboardResult_5354C0` | 原排除正文仍在，仅作历史，不计当前实现；用户已具名排除该入口；不计实现或验收 |
| 3050 | 0x005354D0 | `sub_5354D0` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3051 | 0x00535540 | `sub_535540` | 已排除 | `无当前实现；原入口保留` | 用户已具名排除该入口；不计实现或验收 |
| 3052 | 0x00536960 | `nullsub_4` | 有正文 | `src/platform/LegacyMouse.cpp:126 — EmptyLegacyExit_536960` | 原入口的 RET／固定返回已用当前 IDA 核实；不是业务占位；完整接线未验 |
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
| 3127 | 0x0053A200 | `sub_53A200` | 有承接 | `src/GraphicRead.cpp:274 — records.try_emplace(readWord(record), nullptr)；src/GraphicInfo.cpp:236 — BorrowGraphicFirstRecords().emplace(firstMapNo, firstRecord)` | 原 9 条指令为非空目标的两 W32 pair 复制，直接原引用含 45AF60／523D20；当前对应 map 插入构造实际 pair，键宽保持、指针原生拓宽；完整 owner／运行未验 |
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
| 3143 | 0x0053AF90 | `sub_53AF90` | 有正文 | `src/GraphicRead.cpp:384 — DecompressDbf_53AF90` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3144 | 0x0053B1D0 | `_fclose` | 有承接 | `src/platform/LegacyStdio.cpp:165 — CloseLegacyStream_53B1D0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3145 | 0x0053B226 | `_fgets` | 有承接 | `src/platform/LegacyStdio.cpp:32 — ReadLegacyStreamLine_53B226` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3146 | 0x0053B27D | `_rewind` | 有承接 | `src/platform/LegacyStdio.cpp:82 — RewindLegacyStream_53B27D` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3147 | 0x0053B2D1 | `__fsopen` | 有承接 | `src/platform/LegacyStdio.cpp:19 — OpenSharedLegacyStream_53B2D1` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3148 | 0x0053B2F1 | `_fopen` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3149 | 0x0053B304 | `_sprintf` | 有承接 | `src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3150 | 0x0053B356 | `_fputs` | 有承接 | `src/platform/LegacyStdio.cpp:62 — WriteLegacyStreamString_53B356` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3151 | 0x0053B399 | `__onexit` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3152 | 0x0053B406 | `_atexit` | 有承接 | `src/platform/LegacyCryptInt.cpp:33 — InitializeLegacyStartupValues_401C10；src/platform/MainLoopSupport.cpp:245 — InitializeLegacyProtectionChecksum1_404120` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3153 | 0x0053B418 | `___onexitinit` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3154 | 0x0053B447 | `sub_53B447` | 有承接 | `CMakeLists.txt:166 — 宿主CRT启动；src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyFormat.cpp:22 — std::vfprintf；src/platform/LegacyScan.cpp:8 — std::vfscanf` | 原53C8CD在C／C++初始化前从58F864间接调用；旧六回调／CPU探测／x87默认精度随MC01-C2退休，当前MSVC整体启动／格式／扫描服务；不新增旧回调槽或声明硬件状态等价 |
| 3155 | 0x0053B45E | `nullsub_6` | 待确认 | `—` | 原入口为单RET；仅58F868／58F86C两数据槽引用，本轮两槽无已建消费xref，不能称已证启动表；具体原职责／当前承接仍待证，不猜不可达／不适用或造空回调 |
| 3156 | 0x0053B45F | `__cfltcvt_init_7` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3157 | 0x0053B498 | `__ftol` | 有承接 | `src/Palette.cpp:138 — TruncatedLowByte；src/render/LegacyTextureDrawing.cpp:132 — TruncateTextureCoordinate` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3158 | 0x0053B4BF | `sub_53B4BF` | 有正文 | `src/platform/MainLoopSupport.cpp:660 — SeedRandom_53B4BF` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3159 | 0x0053B4C9 | `rand` | 有正文 | `src/platform/MainLoopSupport.cpp:667 — NextRandom_53B4C9` | 实际非占位正文已定位；完整业务／接线／运行未验 |
| 3160 | 0x0053B4E7 | `delete` | 有承接 | `src/platform/LegacyMemory.cpp:60 — FreeLegacyByteBuffer` | 当前 IDA 的原 delete 叶调用 free；本线实际通用释放使用 std::free，与已批准 malloc 分配族成组承接；完整 owner／运行未验 |
| 3161 | 0x0053B4F2 | `?_JumpToContinuation@@YGXPAXPAUEHRegistrationNode@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3162 | 0x0053B526 | `sub_53B526` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3163 | 0x0053B52D | `sub_53B52D` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3164 | 0x0053B534 | `?_UnwindNestedFrames@@YGXPAUEHRegistrationNode@@PAUEHExceptionRecord@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3165 | 0x0053B583 | `___CxxFrameHandler` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3166 | 0x0053B5B9 | `?_CallCatchBlock2@@YAPAXPAUEHRegistrationNode@@PBU_s_FuncInfo@@PAXHK@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3167 | 0x0053B60D | `?CatchGuardHandler@@YA?AW4_EXCEPTION_DISPOSITION@@PAUEHExceptionRecord@@PAUCatchGuardRN@@PAX2@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3168 | 0x0053B632 | `?_CallSETranslator@@YAHPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAX2PBU_s_FuncInfo@@H1@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3169 | 0x0053B6E6 | `?TranslatorGuardHandler@@YA?AW4_EXCEPTION_DISPOSITION@@PAUEHExceptionRecord@@PAUTranslatorGuardRN@@PAX2@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3170 | 0x0053B75B | `?_GetRangeOfTrysToCheck@@YAPBU_s_TryBlockMapEntry@@PBU_s_FuncInfo@@HHPAI1@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3171 | 0x0053B7D8 | `__global_unwind2` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3172 | 0x0053B7F8 | `__unwind_handler` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3173 | 0x0053B81A | `__local_unwind2` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3174 | 0x0053B882 | `__abnormal_termination` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3175 | 0x0053B8A5 | `__NLG_Notify1` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3176 | 0x0053B8AE | `__NLG_Notify` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3177 | 0x0053B8C6 | `_localtime` | 有承接 | `src/platform/LegacyTime.cpp:175 — LocalTime_53B8C6` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3178 | 0x0053BA26 | `_time` | 有承接 | `src/platform/LegacyTime.cpp:149 — ReadLegacyTime32_53BA26` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3179 | 0x0053BB02 | `_fflush` | 有承接 | `src/platform/LegacyStdio.cpp:159 — FlushLegacyStream_53BB02` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3180 | 0x0053BB3D | `__flush` | 有承接 | `src/platform/LegacyStdio.cpp:159 — FlushLegacyStream_53BB02` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3181 | 0x0053BB99 | `sub_53BB99` | 有承接 | `src/platform/LegacyStdio.cpp:171 — FlushAllLegacyStreams_53BB99` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3182 | 0x0053BBA2 | `_flsall` | 有承接 | `src/platform/LegacyStdio.cpp:171 — FlushAllLegacyStreams_53BB99` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3183 | 0x0053BC0F | `sub_53BC0F` | 有承接 | `src/platform/LegacyFormat.cpp:50 — FormatLegacyStream_53BC0F` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3184 | 0x0053BC41 | `_fseek` | 有承接 | `src/platform/LegacyStdio.cpp:75 — SeekLegacyStream_53BC41` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3185 | 0x0053BCCD | `_fread` | 有承接 | `src/platform/LegacyStdio.cpp:39 — ReadLegacyStream_53BCCD` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3186 | 0x0053BDC0 | `__alloca_probe` | 有承接 | `src/network/NetworkRuntime.cpp:217 — Sub_403C50；src/network/NetworkRuntime.cpp:302 — Sub_403E50` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3187 | 0x0053BDEF | `_isspace` | 有承接 | `src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3188 | 0x0053BE17 | `_iscntrl` | 有承接 | `src/platform/LegacyStringCompare.cpp:16 — ClassifyLegacyControl_53BE17` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3189 | 0x0053BE40 | `_strstr` | 有承接 | `src/platform/LegacyTime.cpp:131 — StrStr_53BE40` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3190 | 0x0053BEC0 | `_strncpy` | 有承接 | `src/platform/LegacyTime.cpp:119 — StrNCpy_53BEC0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3191 | 0x0053BFC0 | `_strncmp` | 有承接 | `src/platform/LegacyTime.cpp:82 — StrNCmp_53BFC0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3192 | 0x0053BFF8 | `cAlloc` | 有承接 | `src/platform/LegacyMemory.cpp:19 — CallocateLegacyMemory_53BFF8` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3193 | 0x0053C0A9 | `free` | 有承接 | `src/platform/LegacyTime.cpp:64 — Free_53C0A9` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3194 | 0x0053C112 | `_qsort` | 有承接 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420；src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3195 | 0x0053C266 | `_shortsort` | 有承接 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420；src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3196 | 0x0053C2B4 | `_swap` | 有承接 | `src/world/MapDrawCache.cpp:173 — SortMapPrimary_46C420；src/world/MapDrawCache.cpp:149 — CompareMapPrimary_46C440` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3197 | 0x0053C2F0 | `_strchr` | 有承接 | `src/platform/LegacyTime.cpp:125 — StrChr_53C2F0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3198 | 0x0053C3AC | `_malloc` | 有承接 | `src/platform/LegacyTime.cpp:58 — Malloc_53C3AC` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3199 | 0x0053C3BE | `__nh_malloc` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3200 | 0x0053C3EA | `sub_53C3EA` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)` | 原仅__nh_malloc调用并转旧两池／HeapAlloc；实际std::malloc整体承接分配及已批库内策略，不复刻模式，业务／owner／运行未验 |
| 3201 | 0x0053C45E | `??_L@YGXPAXIHP6EX0@Z1@Z` | 有承接 | `src/platform/LegacyRuntimeExit.cpp:44 — ConstructLegacyArray_53C45E` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3202 | 0x0053C4E0 | `??_M@YGXPAXIHP6EX0@Z@Z` | 有承接 | `src/platform/LegacyRuntimeExit.cpp:67 — DestroyLegacyArray_53C4E0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3203 | 0x0053C560 | `?__ArrayUnwind@@YGXPAXIHP6EX0@Z@Z` | 有承接 | `src/platform/LegacyRuntimeExit.cpp:23 — UnwindLegacyArray_53C560` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3204 | 0x0053C5BE | `_atol` | 有承接 | `src/platform/LegacyTime.cpp:39 — Atol_53C5BE` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3205 | 0x0053C649 | `_atoi` | 有承接 | `src/platform/LegacyTime.cpp:52 — Atoi_53C649` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3206 | 0x0053C654 | `__mkdir` | 有承接 | `src/platform/LegacyTime.cpp:137 — CreateLegacyDirectory_53C654` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3207 | 0x0053C680 | `_fwrite` | 有承接 | `src/platform/LegacyStdio.cpp:47 — WriteLegacyStream_53C680` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3208 | 0x0053C790 | `_strncat` | 有承接 | `src/platform/LegacyTime.cpp:113 — StrNCat_53C790` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3209 | 0x0053C8B3 | `_fgetc` | 有承接 | `src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3210 | 0x0053C8CD | `__cinit` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyMouse.cpp:67 — gSurfaceTransformCacheStartup` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3211 | 0x0053C8FA | `_exit` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3212 | 0x0053C90B | `__exit` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3213 | 0x0053C91C | `_doexit` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3214 | 0x0053C9B5 | `__initterm` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3215 | 0x0053C9CF | `??2@YAPAXI@Z` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer；include/cg/platform/LegacyMemory.hpp:31 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3216 | 0x0053C9DD | `_mbstowcs` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3217 | 0x0053CADC | `_sscanf` | 有承接 | `src/platform/LegacyScan.cpp:15 — ScanLegacyString_53CADC` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3218 | 0x0053CB10 | `__aulldiv` | 有承接 | `src/platform/LegacyFormat.cpp:60 — DivideLegacyUnsigned64_53CB10` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3219 | 0x0053CB78 | `_ftell` | 有承接 | `src/platform/LegacyStdio.cpp:69 — TellLegacyStream_53CB78` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3220 | 0x0053CCD0 | `__fstat` | 有承接 | `src/platform/LegacyTime.cpp:196 — ReadLegacyFileCreationTime32；include/cg/platform/LegacyTime.hpp:77 — ReadLegacyFileCreationTime32` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3221 | 0x0053CF50 | `__strcmpi` | 有承接 | `src/platform/LegacyStringCompare.cpp:22 — CompareInsensitive_53CF50` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3222 | 0x0053CFDC | `_fputc` | 有承接 | `src/platform/LegacyStdio.cpp:55 — WriteLegacyStreamByte_53CFDC` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3223 | 0x0053D000 | `__CIpow` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3224 | 0x0053D019 | `_pow` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3225 | 0x0053D022 | `sub_53D022` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3226 | 0x0053D1F5 | `sub_53D1F5` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3227 | 0x0053D21D | `__snprintf` | 有承接 | `src/platform/LegacyFormat.cpp:39 — FormatLegacyBoundedString_53D21D` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3228 | 0x0053D26E | `_strtok` | 有承接 | `src/platform/LegacyTime.cpp:76 — StrTok_53D26E` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3229 | 0x0053D30A | `__close` | 有承接 | `src/platform/LegacyLowIo.cpp:22 — CloseLegacyDescriptor_53D30A` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3230 | 0x0053D3BD | `__filelength` | 有承接 | `src/platform/LegacyLowIo.cpp:16 — GetLegacyDescriptorLength_53D3BD` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3231 | 0x0053D435 | `__open` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3232 | 0x0053D44C | `__sopen` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3233 | 0x0053D710 | `_memcpy` | 有承接 | `src/network/NetworkRuntime.cpp:4801 — ConsumeReceiveQueue_508230；src/network/NetworkRuntime.cpp:4820 — ConsumeSendQueue_508280` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3234 | 0x0053DA45 | `_strtol` | 有承接 | `src/platform/LegacyTime.cpp:45 — Strtol_53DA45` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3235 | 0x0053DA5C | `_strtoxl` | 有承接 | `src/platform/LegacyTime.cpp:45 — Strtol_53DA45` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3236 | 0x0053DC64 | `_tolower` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3237 | 0x0053DD2F | `__chdir` | 有承接 | `src/platform/LegacyTime.cpp:143 — ChangeLegacyDirectory_53DD2F` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3238 | 0x0053DDB5 | `start` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3239 | 0x0053DEAB | `__amsg_exit` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3240 | 0x0053DED0 | `_fast_error_exit` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3241 | 0x0053DEF3 | `nullsub_7` | 待确认 | `—` | 原入口单RET且当前无已建入引用；无依据对应格式输出，移除该候选；具体原职责／当前关系仍待证，不猜原不可达／不适用或造空回调 |
| 3242 | 0x0053DEF4 | `__input` | 有承接 | `src/platform/LegacyScan.cpp:8 — ScanLegacyInput_53DEF4` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3243 | 0x0053E919 | `__hextodec` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3244 | 0x0053E950 | `_fgetc_0` | 有承接 | `src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3245 | 0x0053E96A | `__un_inc` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3246 | 0x0053E981 | `__whiteout` | 有承接 | `src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3247 | 0x0053E9A5 | `___initstdio` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3248 | 0x0053EA4A | `___endstdio` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3249 | 0x0053EA5E | `__stbuf` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3250 | 0x0053EAEB | `__ftbuf` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3251 | 0x0053EB28 | `sub_53EB28` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3252 | 0x0053F2C6 | `_write_char` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3253 | 0x0053F2FB | `_write_multi_char` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3254 | 0x0053F32C | `_write_string` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3255 | 0x0053F364 | `_get_int_arg` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3256 | 0x0053F371 | `_get_int64_arg` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3257 | 0x0053F381 | `_get_short_arg` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3258 | 0x0053F38F | `__freebuf` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3259 | 0x0053F3BA | `__filbuf` | 有承接 | `src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3260 | 0x0053F493 | `__lseek` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3261 | 0x0053F52D | `__ioinit` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3262 | 0x0053F6D8 | `__openfile` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3263 | 0x0053F848 | `__getstream` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3264 | 0x0053F8C0 | `__flsbuf` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3265 | 0x0053F9E0 | `_strlen` | 有承接 | `src/platform/LegacyTime.cpp:95 — StrLen_53F9E0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3266 | 0x0053FA5B | `sub_53FA5B` | 有承接 | `src/platform/LegacyMemory.cpp:27 — ReallocateLegacyMemory_53FA5B` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3267 | 0x0053FCFB | `sub_53FCFB` | 有承接 | `src/platform/LegacyMemory.cpp:12 — GetLegacyAllocationSize_53FCFB` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3268 | 0x0053FD5C | `__setdefaultprecision` | 有承接 | `src/YobiRuntime.cpp:38165 — RenderChara_521F20；src/world/SceneState.cpp:10751 — Sub_51D790` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3269 | 0x0053FD6E | `__ms_p5_test_fdiv` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3270 | 0x0053FDAC | `__ms_p5_mp_test_fdiv` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3271 | 0x0053FDD5 | `__forcdecpt` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3272 | 0x0053FE2F | `__cropzeros` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3273 | 0x0053FE7D | `__positive` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3274 | 0x0053FE95 | `__fassign` | 有承接 | `src/platform/LegacyScan.cpp:8 — ScanLegacyInput_53DEF4的std::vfscanf；src/platform/LegacyScan.cpp:15 — ScanLegacyString_53CADC的std::vsscanf` | 原输入扫描的浮点赋值回调，登记58FB58并由53E324调用，实际std::vfscanf／std::vsscanf承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3275 | 0x0053FED3 | `__cftoe` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3276 | 0x0053FFD7 | `__cftof` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3277 | 0x005400B5 | `__cftog` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3278 | 0x00540150 | `__cftoe_g` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3279 | 0x00540177 | `__cftof_g` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3280 | 0x0054019A | `__cfltcvt` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3281 | 0x005401EB | `__shift` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3282 | 0x00540210 | `___InternalCxxFrameHandler` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3283 | 0x005402AB | `?FindHandler@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PAXPBU_s_FuncInfo@@EH1@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3284 | 0x00540452 | `?FindHandlerForForeignException@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PAXPBU_s_FuncInfo@@HH1@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3285 | 0x005404FA | `?TypeMatch@@YAHPBU_s_HandlerType@@PBU_s_CatchableType@@PBU_s_ThrowInfo@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3286 | 0x00540557 | `___FrameUnwindToState` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3287 | 0x005405F5 | `unknown_libname_49` | 有承接 | `src/platform/LegacyRuntimeExit.cpp:12 — FilterLegacyArrayUnwindException_5405F5` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3288 | 0x0054060B | `?CatchIt@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PAXPBU_s_FuncInfo@@PBU_s_HandlerType@@PBU_s_CatchableType@@PBU_s_TryBlockMapEntry@@H1E@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3289 | 0x00540686 | `?CallCatchBlock@@YAPAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PAU_CONTEXT@@PBU_s_FuncInfo@@PAXHK@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3290 | 0x00540792 | `?ExFilterRethrow@@YAHPAU_EXCEPTION_POINTERS@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3291 | 0x005407BC | `?BuildCatchObject@@YAXPAUEHExceptionRecord@@PAUEHRegistrationNode@@PBU_s_HandlerType@@PBU_s_CatchableType@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3292 | 0x00540980 | `unknown_libname_50` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3293 | 0x005409E7 | `?AdjustPointer@@YAPAXPAXABUPMD@@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3294 | 0x00540A10 | `__CallSettingFrame@12` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3295 | 0x00540A5C | `sub_540A5C` | 有承接 | `src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3296 | 0x00540AB2 | `sub_540AB2` | 有承接 | `src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C；CMakeLists.txt:166 — 宿主VCRuntime` | 原八处引用均在C++ handler／展开链；读58FB74=540A5C、SEH保护后交终止；当前VCRuntime／实际std::terminate承接，不把标准流构造当本叶直接实现 |
| 3297 | 0x00540B08 | `___tzset` | 有承接 | `src/platform/LegacyTime.cpp:178 — ::_localtime32(&timestamp)；src/platform/LegacyTime.cpp:171 — ::_mktime32(&calendar)` | 原时区初始化包装仅由53B8C6／541160调用并转540B1D；实际::_localtime32／::_mktime32整体承接，不保原私有函数划分；MSVC库内策略沿获批边界，业务／寿命／运行未验 |
| 3298 | 0x00540B1D | `__tzset` | 有承接 | `src/platform/LegacyTime.cpp:178 — ::_localtime32(&timestamp)；src/platform/LegacyTime.cpp:171 — ::_mktime32(&calendar)` | 原时区刷新仅由540B08调用，原getenv／Win32时区及转换职责归同两实际MSVC日历服务；不模拟旧TZ缓存／私有全局或声称当前DLL逐叶调用，业务／运行未验 |
| 3299 | 0x00540D7B | `__isindst` | 有承接 | `src/platform/LegacyTime.cpp:178 — ::_localtime32(&timestamp)；src/platform/LegacyTime.cpp:171 — ::_mktime32(&calendar)` | 原夏令时判断仅在旧localtime／本地转时间戳链内消费，四次转540F27；由实际::_localtime32／::_mktime32整体承接，旧私有算法退休，完整日期／失败／运行未验 |
| 3300 | 0x00540F27 | `_cvtdate` | 有承接 | `src/platform/LegacyTime.cpp:178 — ::_localtime32(&timestamp)；src/platform/LegacyTime.cpp:171 — ::_mktime32(&calendar)` | 原夏令时转换仅由540D7B内四处调用；同两MSVC公共日历服务整体承接，不复刻旧辅助边界／存储，不宣原体或机器ABI等价，业务／运行未验 |
| 3301 | 0x00541067 | `_gmtime` | 有承接 | `src/platform/LegacyTime.cpp:178 — ::_localtime32(&timestamp)` | 原UTC分解仅在旧localtime内三处调用；当前实际::_localtime32整体承接该日历转换链，不宣称直接调用::_gmtime32或新增UTC接口，九S32借用字段及完整寿命／运行未验 |
| 3302 | 0x00541160 | `___loctotime_t` | 有承接 | `src/platform/LegacyTime.cpp:159 — ConvertLocalTime_541160` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3303 | 0x00541222 | `__commit` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3304 | 0x00541279 | `__write` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3305 | 0x00541426 | `__read` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3306 | 0x00541620 | `_memcpy_0` | 有承接 | `src/platform/LegacyMemory.cpp:33 — CopyLegacyMemory_541620` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3307 | 0x00541955 | `__isctype` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3308 | 0x005419D0 | `_memset` | 有承接 | `src/platform/LegacyMemory.cpp:41 — FillLegacyMemory_5419D0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3309 | 0x00541A28 | `__callnewh` | 有承接 | `src/platform/LegacyMemory.cpp:55 — AllocateLegacyByteBuffer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3310 | 0x00541A43 | `sub_541A43` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)；CMakeLists.txt:166 — 宿主Windows CRT进入WinMain` | 原仅堆选择器541A70使用的模块MZ／PE两字节读取；旧选择机制随MSVC启动／堆策略承接，不新增模块头API，非原机制等价／运行通过 |
| 3311 | 0x00541A70 | `sub_541A70` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)；CMakeLists.txt:166 — 宿主Windows CRT进入WinMain` | 原仅541BB8调用的OS／环境／模块堆选择；当前MSVC启动与实际标准分配库整体承接，不复刻旧环境选择器，业务／启动／运行未验 |
| 3312 | 0x00541BB8 | `sub_541BB8` | 有承接 | `src/platform/LegacyMemory.cpp:57 — std::malloc(byteCount)；CMakeLists.txt:166 — 宿主Windows CRT进入WinMain` | 原仅旧start调用的HeapCreate／池初始化／失败HeapDestroy；当前MSVC宿主启动及实际标准分配族承接，不造旧heap或失败28分支，启动／SDK／运行未验 |
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
| 3328 | 0x00542E34 | `__except_handler3` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3329 | 0x00542EF1 | `__seh_longjmp_unwind@4` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3330 | 0x00542F10 | `__allmul` | 有承接 | `src/platform/LegacyScan.cpp:8 — ScanLegacyInput_53DEF4` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3331 | 0x00542F44 | `sub_542F44` | 有承接 | `src/platform/LegacyTime.cpp:20 — errno；src/platform/LegacyTime.cpp:25 — _doserrno；src/platform/LegacyTime.cpp:139 — ::_mkdir(path)；src/platform/LegacyLowIo.cpp:24 — ::_close(descriptor)` | 原九个旧文件／目录父的系统错误→DOS错误／errno转换，当前实际MSVC文件／目录服务及同线程错误访问整体承接，不手写旧表／影子槽，业务错误消费／owner／运行未验 |
| 3332 | 0x00543097 | `zerotoxdone` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3333 | 0x005430E2 | `_expbigret` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3334 | 0x0054310E | `__ffexpm1` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3335 | 0x00543151 | `_isintTOS` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3336 | 0x00543176 | `_isintTOSret` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3337 | 0x00543177 | `notanint` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3338 | 0x0054317E | `evenint` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3339 | 0x00543185 | `_usepowhlp` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3340 | 0x00543390 | `__startTwoArgErrorHandling` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3341 | 0x005433A7 | `__startOneArgErrorHandling` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3342 | 0x005433F0 | `__twoToTOS` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3343 | 0x00543405 | `__load_CW` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3344 | 0x00543435 | `__fload_withFB` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor；src/platform/MapEffectMath.hpp:35 — Cg2dMapEffectDivisor` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3345 | 0x00543489 | `special_exp` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3346 | 0x005434C5 | `__check_overflow_exit` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3347 | 0x005434D9 | `__check_range_exit` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3348 | 0x0054357C | `__powhlp` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3349 | 0x005436AC | `__d_inttype` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3350 | 0x00543711 | `__alloc_osfhnd` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3351 | 0x005437A6 | `__set_osfhnd` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3352 | 0x0054381D | `__free_osfhnd` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3353 | 0x00543897 | `__get_osfhandle` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3354 | 0x005438D4 | `__chsize` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3355 | 0x00543A1A | `_toupper` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3356 | 0x00543AE8 | `___crtLCMapStringA` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3357 | 0x00543D0C | `_strncnt` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3358 | 0x00543D37 | `__mbctoupper` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3359 | 0x00543DB2 | `__XcptFilter` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3360 | 0x00543EF3 | `_xcptlookup` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3361 | 0x00543F36 | `__wincmdln` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3362 | 0x00543F8E | `__setenvp` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3363 | 0x00544047 | `__setargv` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3364 | 0x005440E0 | `_parse_cmdline` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3365 | 0x00544294 | `___crtGetEnvironmentStringsA` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3366 | 0x005443C6 | `__FF_MSGBANNER` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3367 | 0x005443FF | `sub_5443FF` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — std::terminate；CMakeLists.txt:166 — 宿主CRT启动／终止诊断` | 原五处引用来自旧致命错误／消息banner／abort，按错误码输出CRT弹窗或标准错误；当前MSVC库内诊断整体承接，旧R600x／钩子／缓存沿MC01-C3退休 |
| 3368 | 0x00544552 | `_mbtowc` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3369 | 0x00544620 | `__allshl` | 有承接 | `src/platform/LegacyScan.cpp:8 — ScanLegacyInput_53DEF4` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3370 | 0x0054463F | `_ungetc` | 有承接 | `src/platform/LegacyStdio.cpp:95 — UnreadLegacyStreamByte_54463F` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3371 | 0x005446AD | `__fcloseall` | 有承接 | `src/platform/LegacyStdio.cpp:177 — CloseAllLegacyStreams_5446AD` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3372 | 0x00544705 | `__isatty` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3373 | 0x0054472B | `_wctomb` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3374 | 0x005447A0 | `__aullrem` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3375 | 0x00544815 | `__getbuf` | 有承接 | `src/platform/LegacyStdio.cpp:13 — OpenLegacyStream_53B2F1` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3376 | 0x00544859 | `__control87` | 有承接 | `src/YobiRuntime.cpp:38165 — RenderChara_521F20；src/world/SceneState.cpp:10751 — Sub_51D790` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3377 | 0x0054488E | `__controlfp` | 有承接 | `src/YobiRuntime.cpp:38165 — RenderChara_521F20；src/world/SceneState.cpp:10751 — Sub_51D790` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3378 | 0x005448A4 | `__abstract_cw` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3379 | 0x00544936 | `__hw_cw` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3380 | 0x005449BF | `__ZeroTail` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3381 | 0x00544A08 | `__IncMan` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3382 | 0x00544A5E | `__RoundMan` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3383 | 0x00544AEA | `__CopyMan` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3384 | 0x00544B05 | `__FillZeroMan` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3385 | 0x00544B11 | `__IsZeroMan` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3386 | 0x00544B2C | `__ShrMan` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3387 | 0x00544BB9 | `__ld12cvt` | 有承接 | `src/platform/LegacyScan.cpp:8 — ScanLegacyInput_53DEF4的std::vfscanf；src/platform/LegacyScan.cpp:15 — ScanLegacyString_53CADC的std::vsscanf` | 原544D25／544D3B两输入转换叶的参数表转换支撑，实际std::vfscanf／std::vsscanf整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3388 | 0x00544D25 | `sub_544D25` | 有承接 | `src/platform/LegacyScan.cpp:8 — ScanLegacyInput_53DEF4的std::vfscanf；src/platform/LegacyScan.cpp:15 — ScanLegacyString_53CADC的std::vsscanf` | 原12B中间值按5921C0的六W32表转换为双W32目标，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3389 | 0x00544D3B | `sub_544D3B` | 有承接 | `src/platform/LegacyScan.cpp:8 — ScanLegacyInput_53DEF4的std::vfscanf；src/platform/LegacyScan.cpp:15 — ScanLegacyString_53CADC的std::vsscanf` | 原12B中间值按5921D8的六W32表转换为单W32目标，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3390 | 0x00544D51 | `sub_544D51` | 有承接 | `src/platform/LegacyScan.cpp:8 — ScanLegacyInput_53DEF4的std::vfscanf；src/platform/LegacyScan.cpp:15 — ScanLegacyString_53CADC的std::vsscanf` | 原strgtold12→544D25链，仅53FE95双W32赋值分支使用，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3391 | 0x00544D7E | `sub_544D7E` | 有承接 | `src/platform/LegacyScan.cpp:8 — ScanLegacyInput_53DEF4的std::vfscanf；src/platform/LegacyScan.cpp:15 — ScanLegacyString_53CADC的std::vsscanf` | 原strgtold12→544D3B链，仅53FE95单W32赋值分支使用，由实际浮点输入扫描服务整体承接；原53E324经58FB58调用53FE95，保业务目标宽度／消费，旧96位私有算法及中间布局沿MSVC批准边界退休，不记原ABI／运行通过 |
| 3392 | 0x00544DB0 | `_strcpy` | 有承接 | `src/platform/LegacyTime.cpp:101 — StrCpy_544DB0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3393 | 0x00544DC0 | `_strcat` | 有承接 | `src/platform/LegacyTime.cpp:107 — StrCat_544DC0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3394 | 0x00544EA0 | `__fptostr` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3395 | 0x00544F17 | `__fltout` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3396 | 0x00544F7B | `___dtold` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3397 | 0x00545031 | `__fptrap` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3398 | 0x0054503A | `?__CxxUnhandledExceptionFilter@@YGJPAU_EXCEPTION_POINTERS@@@Z` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3399 | 0x00545080 | `sub_545080` | 有承接 | `CMakeLists.txt:166 — 宿主MSVC exe启动；src/platform/ClientEntry.cpp:242 — WinMain业务边界` | 原55317C C初始化槽安装54503A并保存旧filter；当前MSVC exe pre_cpp初始化持有默认C++异常filter，不再次安装旧入口；完整业务／运行未验 |
| 3400 | 0x00545091 | `sub_545091` | 有承接 | `CMakeLists.txt:166 — 宿主MSVC生命周期；src/platform/ClientEntry.cpp:242 — WinMain业务边界` | 原553198终止槽恢复保存filter；当前宿主拥有完整filter寿命，旧手工保存／恢复沿MC01-C3退休；既有WinMain报告临时层不冒充本叶 |
| 3401 | 0x0054509E | `?_ValidateRead@@YAHPBXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3402 | 0x005450BA | `?_ValidateWrite@@YAHPAXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3403 | 0x005450D6 | `?_ValidateExecute@@YAHP6GHXZ@Z` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3404 | 0x005450F0 | `_strcmp` | 有承接 | `src/platform/LegacyTime.cpp:89 — StrCmp_5450F0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3405 | 0x00545174 | `_abort` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3406 | 0x0054518B | `_getenv` | 有承接 | `src/platform/LegacyTime.cpp:33 — GetEnv_54518B` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3407 | 0x00545208 | `___crtGetStringTypeA` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3408 | 0x00545360 | `unknown_libname_51` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3409 | 0x00545477 | `unknown_libname_52` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3410 | 0x00545916 | `unknown_libname_53` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3411 | 0x00545929 | `unknown_libname_54` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3412 | 0x00545A65 | `sub_545A65` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3413 | 0x00545B3C | `unknown_libname_63` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3414 | 0x00545B51 | `unknown_libname_64` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3415 | 0x00545B66 | `unknown_libname_65` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3416 | 0x00545D6C | `unknown_libname_66` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3417 | 0x00545E1E | `unknown_libname_67` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3418 | 0x00546024 | `unknown_libname_68` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3419 | 0x005460EB | `__87except` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3420 | 0x005461BB | `__set_exp` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3421 | 0x005461E4 | `__sptype` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3422 | 0x0054623E | `__decomp` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3423 | 0x005462FF | `__frnd` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3424 | 0x00546311 | `__fpclass` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3425 | 0x005463A3 | `__setmode` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3426 | 0x00546419 | `___strgtold12` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3427 | 0x005468EA | `__setmbcp` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3428 | 0x00546A83 | `_getSystemCP` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3429 | 0x00546ACD | `_CPtoLCID` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3430 | 0x00546B00 | `_setSBCS` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3431 | 0x00546B29 | `_setSBUpLow` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3432 | 0x00546CAE | `___initmbctable` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3433 | 0x00546CCA | `__ismbblead` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3434 | 0x00546CDB | `_x_ismbbtype` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3435 | 0x00546D0C | `___crtMessageBoxA` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3436 | 0x00546D95 | `___addl` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3437 | 0x00546DB6 | `___add_12` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3438 | 0x00546E14 | `___shl_12` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3439 | 0x00546E42 | `___shr_12` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3440 | 0x00546E6F | `___mtold12` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3441 | 0x00546F36 | `_$I10_OUTPUT` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3442 | 0x005471C9 | `_raise` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3443 | 0x005472F6 | `_siglookup` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyRuntimeError.cpp:9 — TerminateLegacyRuntime_540A5C` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3444 | 0x0054733B | `__mbsnbicoll` | 有承接 | `src/platform/LegacyTime.cpp:33 — GetEnv_54518B` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3445 | 0x0054737A | `___wtomb_environ` | 有承接 | `src/platform/LegacyTime.cpp:33 — GetEnv_54518B` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3446 | 0x005473E8 | `__raise_exc` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3447 | 0x0054769B | `__handle_exc` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3448 | 0x005478B2 | `__set_errno` | 有承接 | `src/platform/LegacyTime.cpp:18 — LegacyErrno_1085FE0；include/cg/platform/LegacyTime.hpp:11 — LegacyErrno_1085FE0` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3449 | 0x005478D8 | `sub_5478D8` | 有承接 | `src/platform/MapEffectMath.cpp:88 — Cg2dMapEffectDivisor的std::pow；CMakeLists.txt:166 — 宿主默认数学错误服务` | 原唯一5461A4在87except传错误记录，零返回后更新errno；当前实际标准数学服务及宿主默认_matherr承接，不保旧记录／硬件状态或新增零返回包装 |
| 3450 | 0x005478DB | `__statfp` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3451 | 0x005478E9 | `__clrfp` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3452 | 0x005478F8 | `__ctrlfp` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3453 | 0x0054791B | `__set_statfp` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3454 | 0x00547971 | `___ld12mul` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3455 | 0x00547B91 | `___multtenpow12` | 有承接 | `src/platform/LegacyFormat.cpp:22 — FormatLegacyOutput_53EB28；src/platform/LegacyFormat.cpp:29 — FormatLegacyString_53B304` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3456 | 0x00547C0D | `___crtCompareStringA` | 有承接 | `src/platform/LegacyTime.cpp:33 — GetEnv_54518B` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3457 | 0x00547E8A | `___crtsetenv` | 有承接 | `src/platform/LegacyTime.cpp:33 — GetEnv_54518B` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3458 | 0x00548011 | `_findenv` | 有承接 | `src/platform/LegacyTime.cpp:33 — GetEnv_54518B` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3459 | 0x00548069 | `_copy_environ` | 有承接 | `src/platform/LegacyTime.cpp:33 — GetEnv_54518B` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3460 | 0x005480D0 | `__mbschr` | 有承接 | `src/platform/LegacyTime.cpp:33 — GetEnv_54518B` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3461 | 0x00548143 | `__strdup` | 有承接 | `src/platform/LegacyTime.cpp:33 — GetEnv_54518B` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3462 | 0x00548170 | `sub_548170` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3463 | 0x0054817A | `sub_54817A` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3464 | 0x00548193 | `sub_548193` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3465 | 0x0054819D | `sub_54819D` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3466 | 0x005481A7 | `sub_5481A7` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3467 | 0x005481C0 | `sub_5481C0` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3468 | 0x005481CA | `sub_5481CA` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3469 | 0x005481D4 | `sub_5481D4` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3470 | 0x005481EF | `sub_5481EF` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3471 | 0x005481F9 | `sub_5481F9` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3472 | 0x0054820E | `sub_54820E` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3473 | 0x00548218 | `sub_548218` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3474 | 0x00548233 | `sub_548233` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3475 | 0x0054823D | `sub_54823D` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3476 | 0x00548252 | `sub_548252` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:83 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3477 | 0x0054825C | `sub_54825C` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:83 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3478 | 0x00548275 | `sub_548275` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:83 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3479 | 0x0054827F | `sub_54827F` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:83 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3480 | 0x00548289 | `sub_548289` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:83 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3481 | 0x005482A4 | `sub_5482A4` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:83 — std::cerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3482 | 0x005482AE | `sub_5482AE` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:86 — std::clog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3483 | 0x005482B8 | `sub_5482B8` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:86 — std::clog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3484 | 0x005482D3 | `sub_5482D3` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:86 — std::clog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3485 | 0x005482DD | `??0Init@ios_base@std@@QAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3486 | 0x0054843B | `nullsub_5` | 有正文 | `src/platform/LegacyMouse.cpp:131 — EmptyLegacyExit_54843B` | 原入口的 RET／固定返回已用当前 IDA 核实；不是业务占位；完整接线未验 |
| 3487 | 0x00548463 | `sub_548463` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3488 | 0x005484CD | `sub_5484CD` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3489 | 0x0054852C | `sub_54852C` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3490 | 0x00548537 | `sub_548537` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3491 | 0x0054859F | `sub_54859F` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3492 | 0x005485FE | `sub_5485FE` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3493 | 0x0054865E | `??0locale@std@@QAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3494 | 0x0054867D | `??1locale@std@@QAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3495 | 0x00548695 | `?_Decref@facet@locale@std@@QAEPAV123@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3496 | 0x005486B1 | `sub_5486B1` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3497 | 0x005486BF | `sub_5486BF` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3498 | 0x005486C5 | `sub_5486C5` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3499 | 0x005486C9 | `?uflow@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3500 | 0x005486E6 | `?xsgetn@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHPADH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3501 | 0x0054875A | `?xsputn@?$basic_streambuf@DU?$char_traits@D@std@@@std@@MAEHPBDH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3502 | 0x005487D0 | `nullsub_8` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3503 | 0x005487D3 | `sub_5487D3` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3504 | 0x005487DE | `sub_5487DE` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3505 | 0x00548843 | `?overflow@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3506 | 0x00548979 | `?pbackfail@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3507 | 0x00548A26 | `?underflow@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3508 | 0x00548A53 | `?uflow@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEHXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3509 | 0x00548B68 | `unknown_libname_73` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3510 | 0x00548BEA | `unknown_libname_74` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3511 | 0x00548CAC | `?setbuf@?$basic_filebuf@DU?$char_traits@D@std@@@std@@MAEPAV?$basic_streambuf@DU?$char_traits@D@std@@@2@PADH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3512 | 0x00548CD7 | `sub_548CD7` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3513 | 0x00548CF6 | `sub_548CF6` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3514 | 0x00548D12 | `sub_548D12` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3515 | 0x00548D31 | `sub_548D31` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3516 | 0x00548D4D | `sub_548D4D` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3517 | 0x00548D69 | `?append@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ID@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3518 | 0x00548DC2 | `?erase@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@II@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3519 | 0x00548E29 | `?_Tidy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEX_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3520 | 0x00548E65 | `?init@?$basic_ios@DU?$char_traits@D@std@@@std@@IAEXPAV?$basic_streambuf@DU?$char_traits@D@std@@@2@_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3521 | 0x00548EA5 | `?_Gndec@?$basic_streambuf@DU?$char_traits@D@std@@@std@@IAEPADXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3522 | 0x00548EB5 | `?_Gninc@?$basic_streambuf@DU?$char_traits@D@std@@@std@@IAEPADXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3523 | 0x00548EC5 | `unknown_libname_75` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3524 | 0x00548ED5 | `?close@?$basic_filebuf@DU?$char_traits@D@std@@@std@@QAEPAV12@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3525 | 0x00548EFC | `?_Init@?$basic_filebuf@DU?$char_traits@D@std@@@std@@IAEXPAU_iobuf@@W4_Initfl@12@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3526 | 0x00548F74 | `?_Freeze@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3527 | 0x00548FA0 | `?_Grow@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAE_NI_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3528 | 0x00549025 | `?_Split@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3529 | 0x00549055 | `?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@PBDI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3530 | 0x0054908A | `?_Copy@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@AAEXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3531 | 0x00549147 | `?_Ungetc@std@@YA_NABDPAU_iobuf@@I@Z` | 有承接 | `src/platform/LegacyStdio.cpp:95 — UnreadLegacyStreamByte_54463F；src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3532 | 0x00549194 | `unknown_libname_76` | 有承接 | `src/platform/LegacyMouse.cpp:151 — RegisterLegacyEmptyExitPair；src/platform/LegacyMouse.cpp:439 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3533 | 0x005491BE | `sub_5491BE` | 有承接 | `src/platform/LegacyMouse.cpp:136 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3534 | 0x005491D6 | `sub_5491D6` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3535 | 0x005491E0 | `sub_5491E0` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3536 | 0x005491F6 | `sub_5491F6` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:82 — std::cin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3537 | 0x00549200 | `sub_549200` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3538 | 0x0054920A | `sub_54920A` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3539 | 0x00549220 | `sub_549220` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3540 | 0x0054922A | `sub_54922A` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3541 | 0x00549234 | `sub_549234` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3542 | 0x0054924D | `sub_54924D` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3543 | 0x00549257 | `sub_549257` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3544 | 0x00549261 | `sub_549261` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3545 | 0x0054927A | `sub_54927A` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3546 | 0x00549284 | `sub_549284` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:104 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3547 | 0x0054928E | `sub_54928E` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:104 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3548 | 0x005492A7 | `sub_5492A7` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:104 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3549 | 0x005492B1 | `sub_5492B1` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3550 | 0x005492BB | `sub_5492BB` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3551 | 0x005492D6 | `sub_5492D6` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcin` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3552 | 0x005492E0 | `sub_5492E0` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3553 | 0x005492F5 | `sub_5492F5` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3554 | 0x005492FF | `sub_5492FF` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3555 | 0x0054931A | `sub_54931A` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:103 — std::wcout` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3556 | 0x00549324 | `sub_549324` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3557 | 0x00549339 | `sub_549339` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:104 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3558 | 0x00549343 | `sub_549343` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:104 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3559 | 0x0054935E | `sub_54935E` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:104 — std::wcerr` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3560 | 0x00549368 | `sub_549368` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:107 — std::wclog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3561 | 0x00549372 | `sub_549372` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:107 — std::wclog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3562 | 0x0054938D | `sub_54938D` | 有承接 | `src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:107 — std::wclog` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3563 | 0x00549397 | `??0_Winit@std@@QAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3564 | 0x00549525 | `sub_549525` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3565 | 0x0054958F | `sub_54958F` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3566 | 0x005495EE | `unknown_libname_77` | 有承接 | `src/platform/LegacyMouse.cpp:71 — FlushLegacyStandardStream` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3567 | 0x00549638 | `sub_549638` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3568 | 0x00549643 | `sub_549643` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3569 | 0x005496AB | `sub_5496AB` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3570 | 0x0054970A | `sub_54970A` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3571 | 0x0054976A | `sub_54976A` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3572 | 0x00549778 | `sub_549778` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3573 | 0x0054977F | `sub_54977F` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3574 | 0x00549782 | `sub_549782` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3575 | 0x00549787 | `?uflow@?$basic_streambuf@_WU?$char_traits@_W@std@@@std@@MAEGXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3576 | 0x005497A8 | `?xsgetn@?$basic_streambuf@GU?$char_traits@G@std@@@std@@MAEHPAGH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3577 | 0x0054982F | `?xsputn@?$basic_streambuf@GU?$char_traits@G@std@@@std@@MAEHPBGH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3578 | 0x005498B7 | `unknown_libname_78` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3579 | 0x005498EE | `unknown_libname_79` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3580 | 0x00549925 | `sub_549925` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3581 | 0x0054992A | `sub_54992A` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3582 | 0x00549935 | `sub_549935` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3583 | 0x0054999A | `?overflow@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGG@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3584 | 0x00549AE2 | `?pbackfail@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGG@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3585 | 0x00549B99 | `?underflow@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3586 | 0x00549BC7 | `?uflow@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEGXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3587 | 0x00549CE5 | `?setbuf@?$basic_filebuf@GU?$char_traits@G@std@@@std@@MAEPAV?$basic_streambuf@GU?$char_traits@G@std@@@2@PAGH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3588 | 0x00549D13 | `unknown_libname_80` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3589 | 0x00549D2C | `sub_549D2C` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3590 | 0x00549D4B | `sub_549D4B` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3591 | 0x00549D67 | `sub_549D67` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3592 | 0x00549D86 | `sub_549D86` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3593 | 0x00549DA2 | `sub_549DA2` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3594 | 0x00549DBE | `?init@?$basic_ios@GU?$char_traits@G@std@@@std@@IAEXPAV?$basic_streambuf@GU?$char_traits@G@std@@@2@_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3595 | 0x00549E08 | `?gbump@?$basic_streambuf@GU?$char_traits@G@std@@@std@@IAEXH@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3596 | 0x00549E1C | `?_Gndec@?$basic_streambuf@GU?$char_traits@G@std@@@std@@IAEPAGXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3597 | 0x00549E2D | `?_Gninc@?$basic_streambuf@GU?$char_traits@G@std@@@std@@IAEPAGXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3598 | 0x00549E3D | `unknown_libname_81` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3599 | 0x00549E51 | `unknown_libname_82` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3600 | 0x00549E61 | `?close@?$basic_filebuf@GU?$char_traits@G@std@@@std@@QAEPAV12@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3601 | 0x00549E88 | `?_Init@?$basic_filebuf@GU?$char_traits@G@std@@@std@@IAEXPAU_iobuf@@W4_Initfl@12@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3602 | 0x00549EDE | `unknown_libname_83` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3603 | 0x00549F20 | `unknown_libname_84` | 有承接 | `src/platform/LegacyMouse.cpp:151 — RegisterLegacyEmptyExitPair；src/platform/LegacyMouse.cpp:439 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3604 | 0x00549F4A | `sub_549F4A` | 有承接 | `src/platform/LegacyMouse.cpp:136 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3605 | 0x00549F62 | `?clear@ios_base@std@@QAEXH_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3606 | 0x00549FFE | `sub_549FFE` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3607 | 0x0054A062 | `sub_54A062` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3608 | 0x0054A06F | `sub_54A06F` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3609 | 0x0054A08C | `unknown_libname_85` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3610 | 0x0054A0C9 | `sub_54A0C9` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3611 | 0x0054A0E5 | `sub_54A0E5` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3612 | 0x0054A142 | `unknown_libname_86` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3613 | 0x0054A17F | `sub_54A17F` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3614 | 0x0054A19C | `sub_54A19C` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3615 | 0x0054A1B8 | `sub_54A1B8` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3616 | 0x0054A1D0 | `??1ios_base@std@@UAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3617 | 0x0054A220 | `??_Gios_base@std@@UAEPAXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3618 | 0x0054A23C | `?_Callfns@ios_base@std@@AAEXW4event@12@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3619 | 0x0054A25E | `?_Addstd@ios_base@std@@IAEXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3620 | 0x0054A293 | `?_Init@ios_base@std@@IAEXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3621 | 0x0054A2CB | `?_Tidy@ios_base@std@@AAEXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3622 | 0x0054A315 | `?assign@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QAEAAV12@ABV12@II@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3623 | 0x0054A3E3 | `sub_54A3E3` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3624 | 0x0054A3ED | `nullsub_9` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3625 | 0x0054A3FA | `sub_54A3FA` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3626 | 0x0054A404 | `?_Init@locale@std@@CAPAV_Locimp@12@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3627 | 0x0054A4BD | `sub_54A4BD` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3628 | 0x0054A505 | `??0_Locimp@locale@std@@AAE@_N@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3629 | 0x0054A57E | `unknown_libname_87` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3630 | 0x0054A59B | `??_G_Locimp@locale@std@@UAEPAXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3631 | 0x0054A5B7 | `??1_Locimp@locale@std@@UAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3632 | 0x0054A638 | `unknown_libname_88` | 有承接 | `src/platform/LegacyMouse.cpp:151 — RegisterLegacyEmptyExitPair；src/platform/LegacyMouse.cpp:439 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3633 | 0x0054A662 | `sub_54A662` | 有承接 | `src/platform/LegacyMouse.cpp:136 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3634 | 0x0054A67A | `?_Xlen@std@@YAXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3635 | 0x0054A6D4 | `sub_54A6D4` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3636 | 0x0054A738 | `sub_54A738` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3637 | 0x0054A755 | `unknown_libname_89` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3638 | 0x0054A792 | `sub_54A792` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3639 | 0x0054A7AE | `sub_54A7AE` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3640 | 0x0054A80B | `unknown_libname_90` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3641 | 0x0054A848 | `sub_54A848` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3642 | 0x0054A865 | `sub_54A865` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3643 | 0x0054A881 | `sub_54A881` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3644 | 0x0054A899 | `?_Xran@std@@YAXXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3645 | 0x0054A8F3 | `unknown_libname_91` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3646 | 0x0054A930 | `sub_54A930` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3647 | 0x0054A94D | `sub_54A94D` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3648 | 0x0054A969 | `sub_54A969` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3649 | 0x0054A981 | `unknown_libname_92` | 有承接 | `src/platform/LegacyMouse.cpp:151 — RegisterLegacyEmptyExitPair；src/platform/LegacyMouse.cpp:439 — LegacyPairedEmptyExitStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3650 | 0x0054A9AB | `sub_54A9AB` | 有承接 | `src/platform/LegacyMouse.cpp:136 — RegisterLegacyPrimaryEmptyExit` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3651 | 0x0054A9C3 | `__Mbrtowc` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3652 | 0x0054AAD5 | `_btowc` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain；src/platform/LegacyStringCompare.cpp:10 — ClassifyLegacySpace_53BDEF` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3653 | 0x0054AB16 | `GetAdaptersAddresses` | 有承接 | `src/platform/LegacyNetworkAdapter.cpp:20 — GetAdaptersAddresses；src/platform/LegacyNetworkAdapter.cpp:16 — Sub_534EF0` | 原 GetAdaptersAddresses import 跳板；当前实际 Windows IP Helper API 调用存在，保留原适配器消费边界；完整网络／运行未验 |
| 3654 | 0x0054AB1C | `RtlUnwind` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3655 | 0x0054AB30 | `__strnicmp` | 有承接 | `src/platform/LegacyStringCompare.cpp:28 — CompareInsensitiveN_54AB30` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3656 | 0x0054ABDD | `__itoa` | 有承接 | `src/platform/LegacyTime.cpp:70 — Itoa_54ABDD` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3657 | 0x0054AC0A | `_xtoa` | 有承接 | `src/platform/LegacyTime.cpp:70 — Itoa_54ABDD` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3658 | 0x0054AC66 | `??1type_info@@UAE@XZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3659 | 0x0054AC7B | `??_Gtype_info@@UAEPAXI@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3660 | 0x0054AC98 | `__EH_prolog` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3661 | 0x0054ACB7 | `_fgetpos` | 有承接 | `src/platform/LegacyStdio.cpp:115 — GetLegacyStreamPosition_54ACB7` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3662 | 0x0054ACD9 | `_fsetpos` | 有承接 | `src/platform/LegacyStdio.cpp:129 — SetLegacyStreamPosition_54ACD9` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3663 | 0x0054ACF1 | `_setvbuf` | 有承接 | `src/platform/LegacyStdio.cpp:88 — SetLegacyStreamBuffer_54ACF1` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3664 | 0x0054AD92 | `_fputwc` | 有承接 | `src/platform/LegacyStdio.cpp:139 — WriteLegacyStreamWide_54AD92` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3665 | 0x0054AE87 | `_ungetwc` | 有承接 | `src/platform/LegacyStdio.cpp:152 — UnreadLegacyStreamWide_54AE87` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3666 | 0x0054AF70 | `_fgetwc` | 有承接 | `src/platform/LegacyStdio.cpp:146 — ReadLegacyStreamWide_54AF70` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3667 | 0x0054B05D | `sub_54B05D` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3668 | 0x0054B079 | `??0exception@@QAE@ABQBD@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3669 | 0x0054B0B6 | `??0exception@@QAE@ABV0@@Z` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3670 | 0x0054B100 | `sub_54B100` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3671 | 0x0054B116 | `?what@exception@@UBEPBDXZ` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3672 | 0x0054B123 | `__CxxThrowException@8` | 有承接 | `src/platform/LegacyMouse.cpp:80 — NarrowStandardStreamsStaticInitializer；src/platform/LegacyMouse.cpp:101 — WideStandardStreamsStaticInitializer` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3673 | 0x0054B160 | `_memcmp` | 有承接 | `src/platform/LegacyMemory.cpp:49 — CompareLegacyMemory_54B160` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3674 | 0x0054B1AC | `retnull_0` | 有承接 | `src/platform/LegacyMemory.cpp:49 — CompareLegacyMemory_54B160；include/cg/platform/LegacyMemory.hpp:26 — CompareLegacyMemory_54B160` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3675 | 0x0054B20C | `___termcon` | 有承接 | `src/platform/ClientEntry.cpp:242 — WinMain` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3676 | 0x0054B239 | `__ftelli64` | 有承接 | `src/platform/LegacyStdio.cpp:102 — TellLegacyStream64_54B239` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3677 | 0x0054B3C0 | `__fseeki64` | 有承接 | `src/platform/LegacyStdio.cpp:108 — SeekLegacyStream64_54B3C0` | 实际包装委托 MSVC／标准库；按已批准 CRT 宿主边界承接，完整调用方验收未验 |
| 3678 | 0x0054B450 | `__flswbuf` | 有承接 | `src/platform/LegacyStdio.cpp:139 — WriteLegacyStreamWide_54AD92` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3679 | 0x0054B574 | `__filwbuf` | 有承接 | `src/platform/LegacyStdio.cpp:26 — ReadLegacyStreamByte_53C8B3` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
| 3680 | 0x0054B658 | `__lseeki64` | 有承接 | `src/platform/LegacyLowIo.cpp:9 — OpenLegacyDescriptor_53D435` | 原库内／编译器支撑入口；当前实际 MSVC 服务或标准流／locale 使用边界承接，无旧私有包装；不记机器 ABI 或运行已验 |
