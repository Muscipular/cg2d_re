# Borrow接口与存储包装清理（BCL01）

日期：2026-10-15。依据：用户要求“消灭所有borrow”，沿既有工程委托实施。范围为当前src／include活动代码；历史原名、地址与证据保留，不能将历史符号全局抹去。

## 当前方案与边界

已核47个定义／45个名称。移除八个纯存储访问函数及六个玩家／战斗数组访问函数；剩余31个名称（33个定义，含三个邮件视图重载）按真实选择、查询、视图组装、编辑或配置行为改名。opaque类型的字段读取保留现有封装接口，不把私有Yobi／公会／邮件／平台对象布局扩散给消费者。窗口4C72C0实际选择物品列表，47D220实际配置两个预览，按真实行为命名并保原地址。

六个图形容器由各原Initialize入口中的同类型函数局部静态对象拥有；同模块具名非拥有T*只在构造成功后发布，业务直接解引用同一对象。删除原被动字节存储、六取存储函数和十二个纯构造／销毁叶，原Initialize入口改void，不虚造登记成功返回；各入口只有中央调用且结果原已弃用。顺序仍由LegacyMouse.cpp现有中央调用决定，类型和算法不动，退出登记／异常清理由MSVC原生对象寿命承接；具名覆盖手写atexit结果／失败接口，完整运行与异常未验。

摊位私有StallBrowseData在原构造入口内直接静态定义，通过同模块非拥有指针供原业务访问；原启动类只保构造，退出由该实际对象自动逆序析构，去optional／取存储函数与纯销毁叶。两个选择器预览保持独立：原ConstructWindowSelectorPreview固定索引0／1分别触发两个独立局部静态YobiAction构造，仍由原两中央拥有者调用；十值→窗口发布→另十值→窗口发布次序保持。去共享optional数组／访问函数及显式退出包装，由各实际静态对象在原组位置登记逆序退出。无新增业务索引检查、回退、复制或第二owner；不将两对象合成一次性提前构造的数组。

六个玩家／结果访问包装改为现有类型的内部直接对象／字段；四个结果POD仅将同一唯一零初值定义移到world::detail，不增加对象或动态构造。其余接口保原参数、返回、字段宽度、有效/无效分支及调用和发布顺序，仅改语义名称。全部源码调用、声明、当前函数表、定位和语义／验收引用同步；原身份、原地址及已证历史保留。

不引入依赖，不构建、测试、运行、查产物、计算摘要、修改IDA或委派。仅做必要静态差异／表达式与生命周期核对和台账文本汇总；每个可验证小步精确提交，保留范围外修改，完整验收门禁不变。

## 保留接口的语义命名

| 旧名称（历史） | 当前名称 |
|---|---|
| `BorrowIndependentPaletteColors32` | `GetIndependentPaletteColors32` |
| `BorrowGuildName10514BC` | `GetGuildName` |
| `BorrowGuildSecondText10514E5` | `GetGuildSecondText` |
| `BorrowGuildAuxText1057BF4` | `GetGuildAuxText` |
| `BorrowGuildRankText1051518` | `GetGuildRankText` |
| `BorrowTextRecord` | `FindTextPendingRecord` |
| `BorrowImeDescription_4590D0` | `GetImeDescription_4590D0` |
| `BorrowWindowImeDescription_4734F0` | `QueryWindowImeDescription_4734F0` |
| `BorrowGraphicDxCacheEntries` | `GetGraphicDxCacheEntries` |
| `BorrowCpuTempSurface` | `GetCpuTempSurface` |
| `BorrowBattleRebirthSlot106C` | `FindBattleRebirthSlot` |
| `BorrowBattleRiderSlot13C8` | `FindBattleRiderSlot` |
| `BorrowYobiActionDrawMode` | `GetYobiActionDrawMode` |
| `BorrowYobiNameC4` | `GetYobiName` |
| `BorrowYobiTitleE1` | `GetYobiTitle` |
| `WindowItemSelector_BorrowSelectedItem` | `WindowItemSelector_GetItemList_4C72C0` |
| `BorrowSelectedAlbumRecords` | `GetSelectedAlbumRecords` |
| `ClearAlbumNewFlagAndBorrowName` | `ClearAlbumNewFlagAndGetName` |
| `BorrowActiveMapEffectHead` | `GetActiveMapEffectHead` |
| `BorrowNextMapEffect` | `GetNextMapEffect` |
| `BorrowPlayerJobName` | `GetPlayerJobName` |
| `BorrowTitleText` | `GetTitleText` |
| `BorrowPlayerDisplayText` | `GetPlayerDisplayText` |
| `WindowCraft_BorrowSecondPreview` | `WindowCraft_ConfigurePreviews_47D220` |
| `WindowMail_BorrowContactView` | `WindowMail_MakeContactView` |
| `BorrowMailMessageBytes` | `MakeMailMessageView` |
| `BorrowMailMessageF7CA08` | `GetMailMessageF7CA08` |
| `BorrowGuildMailMessageDE0B30` | `GetGuildMailMessageDE0B30` |
| `BorrowMiniMailMessage10763A0` | `GetMiniMailMessage10763A0` |
| `BorrowCurrentProfile10738E8` | `EditCurrentProfile10738E8` |
| `BorrowAutoMapColors` | `GetAutoMapColors` |

## 直接访问替换

| 旧访问函数（历史） | 当前对象／表达式 |
|---|---|
| `BorrowGraphicMapNumbers` | `(*gGraphicMapNumbers)` |
| `BorrowGraphicFirstRecords` | `(*gGraphicFirstRecords)` |
| `BorrowIndependentPaletteQueue` | `(*gIndependentPaletteQueue)` |
| `BorrowPaletteDbfState` | `(*gPaletteDbfState)` |
| `BorrowSurfaceTransformCache629300` | `(*gSurfaceTransforms629300)` |
| `BorrowSurfaceTransformCache62A630` | `(*gSurfaceTransforms62A630)` |
| `BorrowPlayerItemsFDA8A4` | `gPlayerRuntimeSlots.items` |
| `BorrowPlayerStallName` | `gPlayerRuntimeSlots.text15BB8.data()` |
| `BorrowBattleResults` | `gBattleResultsFF0C14` |
| `BorrowBattleSkillResults` | `gBattleSkillResultsFF0C8C` |
| `BorrowBattleResultNames` | `gBattleResultNamesFF0C34` |
| `BorrowBattleResultGraphics` | `gBattleResultGraphicsFF0D0C` |
| BorrowWindowSelectorPreviewStorage | 两个原固定索引各自的局部静态预览 |
| WindowStall_BorrowBrowseStorage | gStallBrowseData非拥有指针，指向原入口唯一静态对象 |

## 实施与静态核对结果

BCL01已落实到60个源／头文件：14个纯访问包装删除，其余31个名称／33个定义按真实行为命名，活动src／include中大小写不敏感的borrow匹配为0。连同19个纯构析／退出函数，共删除33个函数。六个图形容器、两个独立预览和一个摊位浏览对象在原启动入口直接静态构造；中央初始化源码与CryptInt实现保持，两个预览20次及浏览30次的原RNG消费、字段发布与逆序退出保持。

精确变换与函数正文核对通过：190个函数仅作预定名称／访问投影，1842个保留函数全文不变。953项直接定位、917行函数表位置／承接说明和238条语义记录同步；原3680身份保持，13行从正文改为原生承接，26个过时连接／optional字段归历史，直接定义数量及验收门禁不变。UTF-8无BOM／LF、JSON与差异检查通过。台账-Check仍仅受两份既有键盘历史附件缺失阻碍，未写派生文件，不计全量台账通过；未编译、测试、运行或修改IDA数据库。并行字符串会话等待本轮源码提交，收尾文档采用精确上下文补丁，其范围外修改不纳入本次提交。

## BCL02：继续移除简单访问包装

用户要求清理GetAutoMapColors这类简单包装。本轮移除39个纯访问函数：地图颜色、DX缓存指针、Temp面、IME描述、活动地图效果链首和四个资料／搜索对象访问函数，共九个；以及30个只返回既有PlayerRuntimeSlots字段或执行一次CryptInt.value()的玩家访问函数。调用点直接使用同一对象／字段，原有需要的const视图、整数宽度、S32索引转换、单次读取及求值位置保持，不缓存额外快照、不复制owner，不改CryptInt实现与RNG次数。

颜色数组、Temp指针、地图链首及四个资料／搜索对象仅将原唯一存储移出匿名namespace，通过既有头文件声明；不新造对象或构析，不暴露SDL类型。DX指针和IME描述沿已有同一对象声明；玩家类型及对象已经公开在既有内部依赖头，无须搬动布局。保留含分支、跨字段观察、视图组装、数据转换，以及Yobi／公会／邮件等私有布局边界的接口，不为删一行访问函数扩散完整私有对象。

按现有工程委托实施；先记方案再更新源码、当前定位／语义／函数表和唯一进度。所有文件采用apply_patch精确上下文更新，与并行字符串修改逐段合并；仅静态核对，不编译、测试、运行或修改IDA，原身份与完整验收门禁保留，精确提交本轮差异。

BCL02完成：45个源／头文件删除39个函数，294处调用直接访问原对象／字段；126个保留函数只作本轮访问投影，1704个保留函数正文未改，无新增函数。六类CryptInt读取在原22个消费点各执行一次，原中央初始化及CryptInt源码不改。颜色表、Temp、活动链首及资料／搜索存储仍是原唯一对象，声明不新增owner或构析；消费点const绑定保持。

992项直接定位及990行函数表位置／承接说明同步；4590D0的一个库存定义和FDA880访问的一个非库存定义退出直接定义集合，原3680身份保持，仅4590D0由正文改为消费点承接。24条语义记录更新实际对象来源、表达式及旧包装历史；19639项原事实／历史字段和所有原验收门禁、处置类型保持。补丁曾在相似语义条目及验收引用中命中错误位置，已按唯一函数身份修正，并逐文件比较预期变换，确认无范围外差异。

UTF-8无BOM／LF、JSON、源码／元数据预期变换及差异静态核对通过；台账-Check仍仅两份既有键盘历史附件缺失，未写派生文件，不计全量台账通过。未编译、测试、运行或修改IDA。并行字符串整理的独立提交完整保留，后续WindowRuntimeInternal.hpp文字形式调整及Python缓存不纳入本轮提交。
