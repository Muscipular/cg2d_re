# BOK01 图形记录生产与直接查询 SPEC

状态：2026-10-06，本线程按已授权的“未决工程先落 SPEC 后裁决实施”推进；只补数字创建真实图号/坐标依赖的缺口。实现与局部静态证据独立记录，完整功能验收保持未通过。

<a id="graphic-record-production-lookup-baseline"></a>
## 原端基线与范围

| 原入口 / 语义记录 | 角色 | 指令 / 字节 / 页 | call / 内部分支 / ret | 普通入向 / 不同父 |
| --- | --- | --- | --- | --- |
| 459960 / 736 / F:00059960 | GraphicInit_459960：九资源流、40B记录到32B表、图号索引/去重/首见引用 | 634 / 1996 / 8 | 21 / 96 / 7 | 1 / 1，父4576B0 |
| 45A710 / 160 / F:0005A710 | GetGraphicXY：按原容量读两个S16坐标 | 21 / 80 / 1 | 0 / 1 / 2 | 20 / 18 |
| 45A8B0 / 165 / F:0005A8B0 | GetGraphicMapNoIndex：直接S32图号索引 | 7 / 25 / 1 | 0 / 0 / 1 | 29 / 21 |
| 合计 | 三个已有原ID续记 | 662 / 2101 / 10 | 21 / 97 / 10 | 50个call引用；父按根分列 |

三根都是单chunk，无switch。完整指令分页、原chunk边界、入向与改前源码见[证据](cg2d_graphic_record_production_lookup_evidence.json)。三字符串包含NUL：554920的rb为3B，5598CC容量格式24B，5598B0原GBK提示28B，共55B。入向引用不代全部父/全生命周期接线验收。

保留已有[stdio合同](cg2d_stdio_contract.md)、[图形信息基线](cg2d_graphic_info.md)及[CRT宿主迁移决议](cg2d_crt_host_migration_spec.md)。736的旧FILE/格式事实完整续记；160/165的已有语义不新建重复ID。4597E0/459850构析、4574E0主表分配、图像解码/消费及SDK接线不扩成本步全根复验。

<a id="graphic-record-production-lookup-decision"></a>
## 已裁决的正常合同与具体改动

### 生产、容量与原失败路径

九数据FILE槽先清为原native指针，复制各255B路径并rb打开。必需槽0/3/6/7/8失败走共用关闭非空数据槽并清槽的出口，依槽返回1/2/7；其它槽允许缺失。九Info预扫按rb、SEEK_END=2、tell原S32位模式累计U32；每个已开Info关闭但不清共享Info槽。必需Info失败直接返回1/2/7，保数据流，不新增清理。

required按U32字节数/40+10落S32并发布，再以原有符号容量比较；不足显示原格式/GBK提示，关闭数据流返回3。图号索引分配4*gMax+0x28，先发布gGraphicMapNoIndex，空返回-1且不关闭数据流；仅清gMax个W32项，额外40B不清。既有vector/map对象clear；不新增旧表释放、回滚、容量修正或异常策略。

每槽先发布startIndices=base，再打开Info；可缺槽跳过并保base。40B栈记录不预清，低U8 FILEflags&0x10决定原读取循环；Read(0x28,1)返回忽略，短读或本次到EOF仍消费该记录并计数。每个有效Info尾按S32升序排序去重键vector，关闭当前Info且不清共享槽，关闭后base=processed。因此base按实际消费次数累计，不按最大seqNo或文件容量计算。

### 13处源码取样恢复

1. 读取后先取record.seqNo，再取当前主表指针，再processed+1、seqNo+base、发布W32 seqNo；不把计数先递增当原顺序。
2. offset/length/x/y/w/h/sx/sy/flag/mapNo/fileIndex共11字段，每项先重取gGraphicInfo再读字段/写目标。保持原W32/W16/W8、32B运行布局与原字段顺序；reserved+0x16两字节不写。主表指针不缓存跨全部字段。
3. 初mapNo=0只写gMap[0]=0并到读取循环尾。非零且U32<gMax时写索引，再在该分支重读record.mapNo；超范围沿初次样本，仍参与集合/树，不新增拒绝。去重区间先end后start取样，按S32整型相等查找，未见才追加同一查找key。追加后先重取主表计算实际记录非拥有T*，再重读record.mapNo为首见树key，然后emplace。

重复图号的索引后见覆盖；首见树保第一真实表位置指针，而非第一记录副本。该位置后续内容变动仍能由借用指针读到；不延长主表寿命或让树析构释放主表。

沿既有std::vector<S32>/std::map<S32,GraphicInfo*>/std::sort承接已批准的宿主容器适配，保本处值、重复策略、引用与可观察顺序。不还原原STL节点颜色/树形、内部比较次数、排序过程、分配失败时机或x86 ABI。原local最大mapNo仅内部记数且无发布/业务消费，不引入影子全局。

### 两个直接查询保持正确正文

GetGraphicXY以U32 index与gMaxGraphicCount位模式比较：成功先读原主表S16 x并写*x，之后重取表读S16 y并写*y，返回1；失败按x再y各W16清0返回0。保输出/字段及两输出别名，不用loaded数量替代上界，不新增空指针门禁或缓存两字段。

GetGraphicMapNoIndex以S32 index直接读gGraphicMapNoIndex[index]，写S32*outSeqNo，返回1。原无范围/空指针/零值门禁；不把未登记图号改为失败或默认序号。两getter正文与现有公开头保持。

地址采用已证native非拥有T*/T**；非地址仍原宽。网络原格式明文、内存CryptInt保持、音频暂停。RPLAN.24约束适用，不新增溢出/越界/拒绝/钳制/补零/故障模型，也不宣异常等价。

<a id="graphic-record-production-lookup-acceptance"></a>
## 可度量的局部验收

| 项目 | 通过条件 | 本步材料 / 门禁 |
| --- | --- | --- |
| 原端覆盖 | 三个完整根662条2101B/10页，21call/97内部分支/10ret，无遗漏chunk或switch；50普通入向与55B字符串按根分列 | 原字节与指令分页、原chunk/入向/常量封套 |
| 源码恢复 | 仅GraphicInit的13处归档补丁，getter/头及其它CPP正文保持；字段发布与条件重取/首见引用匹配上述正常合同 | 整CPP定向文本差异与原指令取样对照 |
| 映射/分类 | 736/160/165续记，完整旧记录另存；三新增adapted分类，不复制原ID | 1399语义/1394有效ID保持，分类507→510 |
| 功能成员 | RESOURCE26→29登记三根；BATTLE103→105只登记实际数字消费的两个getter | 两域member_inventory_closed=false，完整gates仍空 |
| 独立局部I/S | RESOURCE三根各I/S、BATTLE两getter各I/S，均static，runtime_verified=false | 599→609条证据；10条跨域证据仍只有3个原函数 |
| 派生一致 | measure_cg2d.ps1生成和-Check一致，仅三函数/两域与七open的指定增量 | 关联1512/3680=41.09%保持；七open不转完成 |

三函数在BATTLE共享基线中可能成为candidate；candidate不等于original_inventory_ids成员，生产者Init仍归RESOURCE。getter因源码名无地址后缀，其历史direct_snapshot_definitions为空，不伪造新源码位置；sources明确指向现有符号。

本步不构建/调试/新增或执行测试/运行客户端/算法样例/计算hash。完整功能、所有callee/caller、主表所有者/回收重入、真实资源内容与画面、ABI/PC53及R1/R7/R8仍按各自门禁推进。60收尾任务源码closed10/完整验收0、global_accepted=false、accepted_leaf_features=0保持，局部静态通过不代完整验收。

<a id="graphic-record-production-lookup-source-state"></a>
## 源码状态与后续推进

实施固定命名快照：`cg2d-re-graphic-record-production-lookup-20261006`。原生产者634条完整正文补容器/分配/记录发布局部证据，13处恢复取样；两getter正确源码纳管。旧stdio证据不改成容器/内容验收，保旧事实后以本SPEC当前范围为准。

U08/U09/G01/G02/R01/R04/V04仅增加本步开发事实和引用。继续沿主表分配、GraphicRead实际资源内容与数字/渲染消费、真实owner推进；SDK未决和完整运行缺口仍须由真实证据与后续实现解决，不用本步静态结论掩盖。

## 2026-10-11 GAP01：主表分配依赖补齐

[GAP01](cg2d_graphic_allocation_release_spec.md#graphic-allocation-source-state)补齐4574E0分配及457670释放，两完整原根135条444B/三页、12E8四目标/十一跳三RET，分配原证复用ACB01。五组五定位/四注释恢复容量先取/分配返回持值、P1判空先于P4pub、失败和终止同指针free/清零count先取；原九路径失败继续、四块大小/正常清零、失败与终止仅释放前三块且不清槽保持。旧735追加/新free一F、旧处置350追加/新adapted一，RESOURCE45/RENDER122，两限定S零I八open；完整owner/所有主表借用/SDK与运行待。
