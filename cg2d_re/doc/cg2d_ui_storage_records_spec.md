# 窗口交易／存放记录结构体 SPEC（USR01）

日期：2026-10-15。按用户要求，将四块char大数组改为记录结构体和定长记录数组；沿此前“SPEC后直接执行”的授权推进。

## 模型与共享关系

- WindowTradeItem：0xC4C，used／selection、图片、数量、物品ID、29字节名称、32×96描述、页数、标志和17字节备注。
- WindowStorageItem：0xC44，独立于交易记录，没有物品ID字段，名称／描述／备注及页数偏移不同。
- WindowTradePet：0x5FC；WindowStoragePet：0x600。两者占用／种族／图片／等级／数值区位置不同，分别建模。共用已存在的world::detail::PetTechniqueRecord（0x8C）；保留10条技能、两份17字节名称。存放记录额外保留重量／上限。
- WindowStoragePetSummary：0x154，旧面板消费的独立摘要，含5项战斗数值、4项元素值、10×25技能名和17字节名称，不与完整宠物记录合并。
- WindowItemDetailGroup：20条WindowStorageItem + 5条WindowStoragePetSummary + S32金额；摘要起点0xF550，金额0xFBF4，总长0xFBF8。

具名字段仅依据已存在的生产／消费和原端指令；未消费或尚未命名的间隔保留显式reserved字节，不新增业务含义。四个全局对象名称与原地址注释保留，现代窗口和LegacyUiRecord继续使用同一份对象；不增加影子存储。类型放UI内部共享头，原全局声明仍由WindowRuntimeState提供。

## 证据与不变项

当前cg2d.exe：4E54E0写交易物品，4E0100写展示物品，4E52B0按8C复制宠物技能；4A84A0写存放回复，4E1B10／4E1EE0消费旧面板宠物摘要。IDA已有物品类型仅作辅助：其desc覆盖页数字段、C3C标作两个W16，与实际DWORD写入不一致；以004E0253的DWORD页数写入等真实指令和当前生产／消费为准。

保留GBK定长文本、字段位宽／位型、容量、原偏移、单一定义、初始化、清零范围和读写顺序。整记录复制／交换仍复制全部字节，跨函数借用不复制记录；不改变失败分支、字段解析顺序、字符串截取或异常路径。逐条记录与分组用sizeof／offsetof和标准布局／平凡可复制断言验证。不使用packed布局掩盖错误，不扩宽非地址字段；本次记录内不添加指针。

将记录寻址改为类型化索引，已知字段使用成员访问；旧通用绘制／字节操作仅在明确边界借用字符视图，不能把类型化指针再按旧字节步长推进。同步现行函数定位与进度，并用build.bat Debug配置／编译／链接验证；不运行客户端或测试。

共享技能值类型原文移至src/world/PetTechniqueRecord.hpp，由WorldUpdateDependencies和UI记录头共同包含，namespace和布局不变。这样复用同一类型，同时避免UI记录头依赖世界模块完整声明及其包含顺序。

## 本轮验证与剩余项

六类记录及四块全局总大小的编译期断言通过；五个读写实现文件已核原地址注释和旧字节步长残留，已知字段改为成员访问，整体复制仍保留全部字节。当前函数定位和UI改名映射同步，3680行原身份／状态保持。

与LWS01合并后执行build.bat Debug：所有源文件编译通过，链接退出1，仅剩LegacyCatalogWindows引用的gLegacyPetSkillCatalogCA0478和gLegacyTradeTargetNamesCB1D1C两项未定义符号。容量／生产仍待证，不提供猜测存储。用户已指定由“分析 sub_504180 中的结构体”会话继续补齐；本轮不宣称构建成功。既有C4701及音频C4477警告保留，未测试、运行或部署。

台账-Check仍仅报两份既有外部键盘历史附件缺失，未写派生文件；不计全量台账通过。UTF-8无BOM／LF、JSON、Git差异和3680行函数身份／状态核对通过。
