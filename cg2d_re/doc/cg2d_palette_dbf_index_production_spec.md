# BOQ01 独立调色板DBF索引生产 SPEC

2026-10-06：沿BOM01独立色表consumer与PGM01初始化的真实DBF依赖补实现缺口；依工程委托先记裁决再实施。本步不重验未变的auto.dat/旧FILE provider，不改变DBF格式。

<a id="palette-dbf-index-production-baseline"></a>
## 原基线与差异

原522900..522B29，既有语义724/F:00122900，单chunk完整170条553B/三页、16call/11内跳/1ret，无switch；唯一普通入向522B8E/父522B30，父弃返回。五段数据31B：58D360四Byte比较字、CGLoadSpr.dbf14B、格式8B、rb3B、反斜杠2B。比较字在.data，初值44424600，唯一直接数据引用5229C5为W32读取；不能据无直接写xref将可写数据角色改成constexpr。

[完整证据](cg2d_palette_dbf_index_production_evidence.json)保三页、连续指令字节/chunk、入向/常量及其直接引用、724完整旧FILE记录/历史直接定义、完整改前正文及三个源码补丁。原5236B0容器插入/523690配对/5234C0键索引/523CD0迭代按既有私有容器宿主承接；本根完整取证不代这些原机制或53AF90解码本体验收。

源码缺口：魔数比较固化为字面量，没有原共享W32读取角色；逐记录复合表达式未显式表达先stride后fresh payload及S32中间偏移。原正常值保持，不能将后者报成已观察到的DBF值错误。

<a id="palette-dbf-index-production-decision"></a>
## 本线程裁决与三个补丁

两个256B局部清0/GetCurrentDirectoryA(256)/固定directory[255]比较反斜杠、必要strcat反斜杠，%s%s\%s拼目录/gDataPath/CGLoadSpr.dbf，rb失败退出。对同共享32B头fread(size1,count32)忽略返回/保短读后缀，先读当前header.magic W32再读原58D360可写W32比较字，初值00464244、不升constexpr；不符原直接退出且不关闭FILE。按fresh compressedLength/decodedLength各裸字节分配，第二次fread及解码各重新读compressedLength，53AF90输出直接更新同decodedLength，返回不处理；delete压缩块后载荷集合持有实际decoded，payload=decoded+8。S32 index与每次decoded[0] count比较，逐项先从同decoded+4读S32 stride，再fresh共享payload native Byte*，先S32 stride*index再地址相加得到同record；原键record0沿已批准有序唯一键宿主表、重复键覆盖为当前实际record，旧块/旧键保持不清。结束fresh count发布后fresh stride发布、FILE关闭，再共享current=records.begin，每项保存current并前进shared iterator，旧current==end或实际record空则结束，否则读同record+4 S32值入同独立palette队列；队列/vector/map机制沿已有批准，不新查数或去重。全部原170条553B、16call/11内跳/1ret，唯一522B30 caller弃返回沿现有void角色；本根不代private容器内部/所有正常别名/解码算法/分配SDK或真实DBF运行。

新增文件内私有U32 gPaletteDbfMagic58D360、初值00464244；只有原共享比较字角色，不新增业务状态或公开接口。headerMagic和expectedMagic分步取样；W32非地址不扩宽，不将未知间接引用当已闭合。两个Body补丁仅共享比较字取样与stride/payload/偏移阶段，另一个全局承载补丁；原两清零缓冲、第255Byte目录检查、两读忽略返回、magic不符不关闭、旧块/键保、重复键覆盖、current与入队先后保。

保现有三长度样本、32B共享头/输出decodedLength、count/stride后pub/close再有序入队；不新加格式拒绝、分配/解码检查、重试、回滚或释放策略。原失败路径是兼容合同，不擅自改成“统一关闭”。不拓展为异常等价或整个private容器/SDK生命周期已验。

<a id="palette-dbf-index-production-acceptance"></a>
## 可度量局部验收

| 项目 | 条件 / 本步 |
| --- | --- |
| 原覆盖 | 完整170条553B/三页、16call/11内跳/1ret、唯一入向，五段31B原数据与可写比较字直接引用 |
| 开发 | 整GraphicRead.cpp仅全局W32承载1＋正文取样2补丁，其余根/FILE/容器/公开头保持 |
| 映射历史 | 724已有ID续记，完整旧记录深拷贝；sources仍单源单根、历史定义1/文档2保，不新增原函数 |
| 计量 | 语义1404/有效ID1399及1517关联41.22%不变，分类521→522；RESOURCE40→41枚举open |
| 局部I/S | 本根RESOURCE独立I/S共2条static E647→649；runtime=false |
| 完整门禁 | 七open只进展/引用，60源码closed10/完整验收0、member_inventory_closed=false/gates空/global_accepted=false/accepted_leaf_features=0保持 |

仅必要静态原全根/数据/完整源码补丁重放、W32-S32/native地址和台账完整增量，运行文字汇总measure/-Check；不构建/调试/查产物、不新增/执行测试、不运行客户端/算法，不计算hash。RPLAN.24不扩异常/越界/故障策略；网络原格式明文、内存CryptInt保、音频暂停。

<a id="palette-dbf-index-production-source-state"></a>
## 当前进度与后续

本步落实DBF比较字共享读取与记录地址生产取样，724旧FILE合同保、完整原根续记；命名快照 `cg2d-re-palette-dbf-index-production-20261006`。继续沿初始化hidden queue、实际DBF/解码块与键表-队列共享owner消费推进；全部父/正常alias/真实资源内容运行仍按七任务，局部I/S不代整体SPEC完成。
