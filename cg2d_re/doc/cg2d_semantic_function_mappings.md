# R1：原函数与现有公共实现的显式映射

## 用途与记录口径

[映射清单](cg2d_semantic_function_mappings.json)记录已通过当前源码和原端证据定位的语义合并项，引用[全函数元数据](cg2d_ida_inventory.json)的稳定函数ID，不复制全部元数据或另建覆盖分母。
源码没有某个原地址文本，只能作为定位候选；公共命名、合并叶函数和CRT适配可能已承接其语义。首批八条及后续增量用于消除这类重复编码线索，不宣称原函数已逐一运行验收。
每条包含原入口、元数据ID、源文件/符号、取证文档和适配边界。未列函数保持待分类，不能把既有清单的全部条目默认设为完成、排除或无关。没有任何原文件哈希字段、计算或比对门禁。

## 本轮范围

- 43D7C0/43D810：世界对象逐项构造/析构已合并进ConstructWorldObjects/DestroyWorldObjects，保持正序构造与逆序销毁、每项两次CryptInt调用。
- 4CD250/4CD2D0/4CD360：浏览宠物CRT、六成员构造/析构；后两叶也用于主宠物。两个存储owner保持独立，浏览组的失败前缀清理沿既有已记录实现。4CD250包含IDA合并的退出相关函数块，不能按反编译线性展开另写一套。
- 488420：CF3B10静态预览十槽析构，由InitializeItemSelectors在该项构造后独立登记的回调调用既有DestroyWindowSelectorPreview承接。
- 5219C0：CalculateRippleScreenDelta；当前IDA5218EE调用后直接fld/fsub，不消费EAX，因此现有内部void返回适配不丢caller所需结果。两输出与F32落点均由当前实现承接。
- 521B10：公共DrawGraphic入口已有本体和实际调用，不另造同名地址包装函数。

本次只核地址映射与相应入口/调用存在性，不重新运行既有功能测试或扩大历史验收结论。原指令细节仍以清单链接的局部文档为准，旧版本文档中的“待实现”需结合后续章节和当前源码判断。

## 静态检查与边界

逐条确认原入口存在于权威元数据、稳定ID对应、源文件和符号存在、证据文档可达、入口不重复、JSON可读及UTF-8/LF。此检查只证明本清单结构和定位有效，不证明完整语义、编译链接或运行通过。
本轮未构建、未查产物、未新增或执行测试、未运行、未做哈希校验。总PLAN的R1全量分类、旧表分发、其他明确待决及最终流程验收保持未完成。

## 未归属区间的首入口定位增量

[候选定位清单](cg2d_unassigned_code_candidates.json)记录本轮对元数据中起点小于535000、长度至少5B的2154段所作只读筛选。157段并非纯90/CC；其中16段的首个非90/CC地址显示为指令，均定位到当前源码已有本体。记录按原区间起止保存，不伪造IDA函数ID、不修改数据库。
该筛选不是完备反汇编：多字节NOP、首项为数据或align但后面有代码、长度不足5B，以及535000之后范围均不能据此排除；157也不是未实现函数数。每条只证明列出的首入口有源码定位，不把整段范围、所有回调或所有跳表标为完成。
本轮结果用于后续R1按区间继续分类；不重复生成已存在的现代窗口回调或以无直接xref证明不可达。旧F9的402B30另按完整17条原指令补齐，其日志父页待批边界仍独立保留。

## 玩家坐标至 Ripple 的 CRT 整组映射

2026-10-02，当前 cg2d.exe 会话串行只读核十三个表槽 `553064..553094`，逐一读取实际指针，查看每条启动入口及其构造叶、退出叶的全部函数块，共39个函数、142条指令。新记录沿既有元数据稳定ID增加，不将原IDA的“库函数”标签当作自动排除依据。

| CRT槽 | 启动/构造/析构入口 | 构造目标顺序 | 现有适配组 |
| --- | --- | --- | --- |
| 0x553064 | 0x467B90/0x467BA0/0x467BD0 | 0x9EE8A8→0x9EE8C0 | PlayerCoordinates |
| 0x553068 | 0x467BF0/0x467C00/0x467C20 | 0xC751C0 | WorldCoordinateStates |
| 0x55306C | 0x467C30/0x467C40/0x467C60 | 0xC751B0 | WorldCoordinateStates |
| 0x553070 | 0x467C70/0x467C80/0x467CA0 | 0x9EE8D0 | WorldCoordinateStates |
| 0x553074 | 0x467CB0/0x467CC0/0x467CE0 | 0x9EE8E0 | WorldCoordinateStates |
| 0x553078 | 0x467CF0/0x467D00/0x467D30 | 0xC27FF8→0xC27FE8 | WorldCoordinateStates |
| 0x55307C | 0x467D50/0x467D60/0x467D90 | 0x9A7F88→0x9A7F98 | WorldMoveStates |
| 0x553080 | 0x467DB0/0x467DC0/0x467DE0 | 0xC821E8 | WorldMoveStates |
| 0x553084 | 0x467DF0/0x467E00/0x467E20 | 0x9E03C0 | WorldMoveStates |
| 0x553088 | 0x467E30/0x467E40/0x467E60 | 0x9FB918 | WorldMoveStates |
| 0x55308C | 0x467E70/0x467E80/0x467EA0 | 0xAF0FE8 | WorldMoveStates |
| 0x553090 | 0x467EB0/0x467EC0/0x467EE0 | 0xC821D8 | WorldMoveStates |
| 0x553094 | 0x467EF0/0x467F00/0x467F20 | 0x9A7FA8 | RippleGate |

构造叶均按表中顺序调用 `ConstructZero_401C70`；两对象组析构反序，其余析构同一对象。`SceneState.cpp` 的四组Initialize入口覆盖这些唯一owner；`LegacyMouse.cpp` 的四个静态适配对象按 PlayerCoordinates、WorldCoordinateStates、WorldMoveStates、RippleGate 顺序定义，只触发初始化，不再提供聚合析构。

**本轮发现并修复的差异：** 原十三个入口各自先完成本条构造、再调用 `atexit` 登记本条析构；先前新端将快照等五条和移动状态六条分别合并登记。现按下述SPEC改为十三次显式登记，构造和回调目标次序不变，丢弃每次登记返回后继续；不再依靠聚合析构覆盖登记失败。本轮补齐39个原函数的映射与本组接线，不把它们记作39个新写的函数本体。宿主CRT实际退出与其他初始化项仍未验收。

### 本组退出登记修复 SPEC

上述差异由原十三组完整指令及当前源码直接证明，在既有初始化位置闭合。按用户工程委托采用并实现以下方案：四组公开入口改为InitializePlayerCoordinates、InitializeWorldCoordinateStates、InitializeWorldMoveStates、InitializeRippleGate；各组按原表顺序构造，每完成一个原表项就以无捕获回调调用std::atexit，明确丢弃返回值，继续下一项。两对象组只在两对象均构造后登记一次，回调中先析构后者。前缀退出回调在对应原表项完成后即已登记，不再等待整个聚合组完成；不据此声称未运行的异常退出路径已经验收。

LegacyMouse.cpp仍按四组原有次序触发初始化，移除四个适配对象的聚合析构，避免再次登记或双重销毁。SceneState.cpp保持唯一存储和既有CryptInt构造/析构叶；不扩展状态、不重复构造、不增加随机调用或登记失败补救，不引入依赖。原八个纯构造/聚合析构接口只在这四个适配对象使用，现已由四个完整生命周期入口替代。

集中核对本组13次登记的位置、忽略失败的继续路径、三对双对象的逆序析构、四组启动次序和旧入口引用清理。此修复不证明宿主CRT完整运行、其他静态初始化项或客户端启动/退出验收完成；仍遵守不构建、不运行、不执行测试及不做哈希校验的限制。

## 图形索引与 DBF 调色板容器生命周期映射

2026-10-02，补齐十二个原入口的当前源码映射，查看相关函数全部415条指令；已有标准容器方案沿[GraphicInfo合同](cg2d_graphic_info.md)、[动画同期生命周期证据](cg2d_animation_runtime.md)及[DX缓存§5.15](cg2d_graphic_dx_cache.md)，不另构造旧STL或扩大其适配范围。

| 原入口 | 原职责/存储 | 当前源码承接 |
| --- | --- | --- |
| 4597E0/4597F0/459820 | CRT553058、9A2960图号vector构造与退出 | GraphicInfo.cpp：gGraphicMapNumbers |
| 459850/459860/459890、45A9E0 | CRT55305C、73F128首见记录树的构造与退出 | GraphicInfo.cpp：gGraphicFirstRecords |
| 522620/522630/522660 | CRT553104、103CF88独立调色板mapNo队列 | GraphicRead.cpp：gIndependentPaletteQueue |
| 522690/5226A0 | CRT553108、103CF30 DBF持久索引与载荷集合 | GraphicRead.cpp：gPaletteDbf/PaletteDbfState |

459890只释放树节点、表头及最后共享哨兵；节点值是GraphicInfo主表的非拥有引用，不能再释放该主表。两个vector的allocator局部字节不作为业务状态复制；目标集合保持空初始化，不提前追加图号。

522690本体仅调用5226A0，再跳5226B0登记5226C0退出；5226A0在IDA中同时包含5226B0登记块、5226C0/5226D0退出块、522860构造块及536EB0/54BC10起的异常清理块。不得按反编译只显示的构造正文遗漏退出，也不为这些非独立元数据入口生成新函数ID或重复分母。

原DBF退出先按装载序释放解压块，再清空指针集合、清树节点、释放指针数组、释放表头及最后共享哨兵。当前PaletteDbfState析构先释放解压块，再由成员析构管理map/vector；具体节点、哨兵、指针数组的分配释放次数和时机仍沿已记录标准容器适配边界，不冒称逐指令等价。重建/522C50只释放独立调色板双池，不销毁这一进程期owner；两个队列也不可混用。

本轮仅补齐这组映射，未改资源算法、存储或构造时序。其他容器支持函数的调用定位只作为后续分类线索，未在本轮清单中自动完成；跨模块构造/退出顺序、宿主容器异常处理和完整资源运行仍未验收。

## 资源容器支持链与有类型追加的映射

2026-10-02，继续沿上节真实调用补齐21个支持入口；依据当前IDA完整伪代码、关键比较/调用指令与已确认的父函数合同，记录为“调用链级容器适配”，不记作21个新写的业务函数，也不声称逐指令复刻。

45AA80：图形首见树范围删除；45AB50：图形首见树单节点删除及平衡；45AF20：图形树子树销毁；45AF60：图形树节点插入及平衡；45B1D0：图形树最左节点查找；45B1F0：图形树迭代前驱；45B250：图形树迭代后继；45B2A0：图号排序分区叶；523390：DBF索引树销毁；523450：DBF迭代后继并返回旧值；5234C0：DBF有符号唯一键插入；5235C0：DBF树范围删除；523690：DBF键值临时项构造；5236B0：四类vector共用插入；5238C0：DBF树单节点删除及平衡；523C90：DBF子树销毁；523CD0：DBF迭代后继；523D20：DBF树节点插入及平衡；523F90：DBF最左节点查找；523FD0：DBF迭代前驱；524030：DBF退出清集合的顺序复制叶。

### 两棵树的键与借用值

图形树的459E3D setl、459ED6 jge和插入叶45AFC2 jl均以S32键比较；DBF树的5234E9 setl、523571 jge、523D82 jl同样如此。旧反编译的DWORD标签不能把这些键改成无符号数。前者经过图号vector去重后只保留首次GraphicInfo引用；后者5234C0在已有键时返回原节点，522A8D仍无条件更新记录引用。当前emplace与try_emplace后赋second分别承接两种语义。

523690在522A75被调用：522A6D先将第二项临时值写零，随后构造键/空引用对，再执行插入与真实引用发布。新端只有引用拓宽，键仍为S32。树删除叶先保存后继、维护节点链接及颜色、释放节点并减少计数；后继/前驱/最左节点支持由对应std::map机制承接，不暴露原STL哨兵或把记录值作为owner释放。

### 共用vector叶的五个直接调用点

| 原调用点 | 原载荷 | 当前承接 |
| --- | --- | --- |
| 459E07 | 图号W32 | GraphicInfo.cpp的gGraphicMapNumbers.push_back |
| 518B37 | 请求编号W32 | SceneState.cpp中Sub_518A40的requests.push_back |
| 522A38 | 解压块地址 | GraphicRead.cpp的decodedBlocks.push_back，元素为原生指针 |
| 522AF1 | DBF记录+4的mapNo W32 | gIndependentPaletteQueue.push_back |
| 522B7E | 静态mapNo W32 | InitializeIndependentPalettes_522B30向同一队列追加 |

以上原调用都明确传入当前end和数量1；非尾部、多项插入仍存在于原通用5236B0正文，但未为本线已证调用引入新的公开通用接口。该函数没有作为数据回调的直接xref；这不等于证明全程序不存在间接地址生成。容量增长、分配次数和异常时机沿既有容器适配边界，不能用有类型追加映射冒充所有输入域验收。

### 排序叶与退出复制叶的特殊边界

45B2A0只对超过16元素的区间递归分区，父459960在45A071以后另作插入排序收尾。当前std::sort对应整个父排序阶段；不能独立替换该叶后把小区间“不排序”的返回合同改写为已等价。父两处调用45A049/45A057均弃返回。

524030通用正文按W32顺序复制；唯一直接callsite522734传入相同end作源起止，实际零次复制并返回目标start，随后将集合end写回start。当前owner退出由标准容器析构回收集合储存，不再复制已经释放的解压载荷。

本轮未改算法或新增标准容器依赖。节点布局/树形、内部迭代器地址、容量策略、分配失败与异常清理的逐次等价不在既有适配承诺内；已知caller的键顺序、唯一性、引用归属、追加顺序和数值/指针宽度保持本线验收要求。共享allocator、空析构及其他未列支持入口没有因此自动完成。

## AL比较、战斗状态内联与单跳CRT映射

2026-10-02增加七条内联/尾跳映射，未改现有算法：

- 401D10/401F60分别承接CryptInt与整数、整数与CryptInt的不等比较；前者4处、后者3处直接callsite均在调用后test AL。现有单次Read_401EC0后比较保留读取副作用。
- 401FA0原cmp第一整数与CryptInt读值后setl，是S32整数小于读值；8处直接callsite按AL决定分支。Yobi与登录输入按同方向比较，保留矩形命中的短路及每次独立读取。
- 402000比较相等，10处直接callsite均判AL；相关43E490各退出均另行生产完整0/1，不直接返回比较留下的高位。其他已实现caller依既有局部返回合同，本记录不重新宣称完整父函数验收。
- 438230/438250各10指令，分别先检查模式1/3，再比较类型，已内联到BattleActorPayload的13D8/13DC条件。两W32保持短路，不新建共享状态或简单包装。
- 477160仅尾跳477170，CRT553098由WindowNumberSpacingStaticInitializer直接调用InitializeNumberSpacing_477170承接，保持在Ripple组之后。

前四叶原指令只写AL，高24位仍来自CryptInt读值。上述映射依据已核直接分支消费者，不能据此把所有类似原函数普遍归为零扩展bool；要求完整EAX的路径仍须单独保存其合同，例如既有4FA8E0的完整比较结果适配。本轮没有增加读次数或RNG调用。

另见[函数边界片段与填充分类](cg2d_function_boundary_classifications.md)：4C2634与4D6650并入真实外层本体，4C48EB是确认的NOP区间；这些分类不改变原元数据ID或分母。

## 启动前段、双变换缓存与标准流

2026-10-02补齐18个入口映射，覆盖原CRT553004..553024的九个表槽及其此前缺少映射的支持叶。当前会话逐项读取CRT指针并查看相应入口全部函数块；不延伸到仍暂缓的553028音频入口。

| 原CRT槽 | 原入口与支持叶 | 当前承接 |
| --- | --- | --- |
| 553004 | 401C10→401C20，登记401C50 | StartupCryptValuesStaticInitializer与启动双CryptInt owner |
| 553008/55300C/553010 | 404120/404160/4041A0及各自构造、析构 | 三个独立保护状态静态适配对象 |
| 553014 | 43D760→43D770，登记43D7A0 | WorldObjectsStaticInitializer；逐项叶已有其他映射 |
| 553018/55301C | 44CC20/44CD30、44CC30/44CD40、44CC60/44CD70、共用453FB0 | GraphicDxCache.cpp的两棵类型化变换缓存 |
| 553020 | 453E00/453E10/453E30 | NarrowStandardStreamsStaticInitializer |
| 553024 | 453E40/453E50/453E70 | WideStandardStreamsStaticInitializer |

保护和世界对象继续复用现有单项生命周期对象及唯一owner，保持构造/析构职责，不增加调用或随机消费。本轮仅建立映射，没有重写其本体或再次登记退出。

双缓存原树只销毁节点、表头和最后共享哨兵，未逐值释放缓存surface；现有std::map的指针值保持非拥有语义，实际surface释放由44D490业务入口处理。44CD70仅设置第二树地址后跳入44CC70公共段，不能按自动库名生成空函数。453FB0的旧48B节点与共享哨兵由既有类型化地址键容器适配承接，不仿造布局或allocator残值。

标准流沿[既有启动SPEC](cg2d_startup_flow.md#standard-stream-lifecycle)：公开标准流/FILE缓冲与内部控制器计数由宿主CRT管理；应用侧设置tie、unitbuf并按原三流顺序同步。453E30/453E70在IDA条目中包含54843C/5494FE共享退出尾块，不能只记最初两条跳转；也不能因此把宿主私有控制器次数当作逐指令复刻。

本轮新增记录只证明这些原入口已有可定位的应用侧或标准容器适配；跨模块构造/退出、宿主CRT异常与完整启动仍未运行验收。先前已完成的世界坐标逐项登记不在本轮复查或修改范围。

## 鼠标、玩家及宠物CRT后段映射

2026-10-02，补登记八个原函数。当前IDA实际CRT槽5530F8/5530FC/553100分别为506340/5183D0/518520；本轮查看八入口全部函数块共315条指令，与当前唯一存储及构析代码作定点映射。没有新增构造调用或把地址文本未命中判作缺失本体。

| 原入口 | 当前承接 | 证据与边界 |
|---|---|---|
| 506340、506350 | LegacyMouseStaticInitializer | 构造mouseX、mouseY、八个按钮坐标；正常退出完全反序。506350元数据含506360登记、506370/380退出、506460构造和两组异常块，不能只按反编译正文分类 |
| 506530 | LegacyMouseStaticInitializer逆序循环中的基本操作、Destroy_401CF0 | 原先析构this+10H再this；其引用来自鼠标异常清理块。现有正常循环的同顺序操作不是原异常接线已完成的证据，记录明确保留未完成边界 |
| 5183D0、5183E0 | PlayerRuntimeValuesStaticInitializer；ConstructPlayerRuntimeValues/DestroyPlayerRuntimeValues | 玩家六对象按原+18/+28/+38/+48/+58/+68构造、反序退出；5183E0同时归属登记/退出/构造及异常块，不额外生成5183F0/518400等函数ID |
| 518520、518530、518560 | PetRuntimeValuesStaticInitializer；ConstructPetRuntimeValues/DestroyPetRuntimeValues | 五条宠物各六对象；正序构造，记录和内部字段均逆序析构。原5110H步长只用于原端证据，不套到新端类型 |

原5530F8/FC/100每项完成整组构造后登记一次退出，新端三个静态适配对象按相同相对次序定义，没有此前世界坐标五/六表项被聚合登记的问题。正常运行期重置与CRT析构不是同一操作，不因记录了析构而调用重置或清其他状态。

**本组只确认正常构析和当前接线映射。** 原506350的54BA70/54BAD0清理块、506530的54BB30块、5183E0的54BB50/54BBA0块及宠物EH迭代器均已保留在原元数据范围内；新端普通循环不能单独证明原异常展开已承接。此处不补推测异常处理、不把CPU故障或越界/溢出纳入专项模拟，也不把原生C++退出登记的实际运行结果计为已验。完整生命周期与R1剩余分类继续。

## 两个物品选择器的独立CRT登记

2026-10-02，查看4883F0/488400/4998F0/499900/499920五个包装入口的全部18条指令、55309C/5530A0真实表项，并核对488420与499920均使用488430公共析构正文。新增这五条映射，同时将既有488420映射更新到新的实际登记入口。

原第一项在CF3B10构造后就登记488420；第二项独立构造D2F338后登记499920。新端先前以ItemSelectorsStaticInitializer的一个聚合析构统一登记两个对象，延迟了第一项登记，且未保持两次登记失败的独立性。按[局部修复SPEC](ui_windows/shared_items.md#两个物品选择器逐项退出登记修复-spec2026-10-02)，InitializeItemSelectors现在执行“第一项完整构造→登记第一项退出→第二项完整构造→登记第二项退出”，两次返回均显式弃用，登记失败不阻止后续原步骤。

中央静态适配仅触发InitializeItemSelectors，移除聚合析构；两个原公开Construct/Destroy接口无其他源码消费者并已移除。退出回调分别读取各自唯一owner的preview，调用既有DestroyWindowSelectorPreview；十槽逆序、静态预览所有权、选择和描述符内容不变，没有新增分配/释放或随机读取。此修复不承诺宿主CRT实际退出、构造异常展开或整体窗口生命周期已验收。

## 平台导入跳板的原生边界分类

2026-10-02，按当前cg2d.exe会话的实际指令与导入表，登记534980..534AAE区间内31个独立入口。每项只有一条6B的间接jmp，操作数直接指向已枚举IAT槽；分别为DDRAW 1项、IMM32 9项、WSOCK32 15项、KERNEL32 5项、DSOUND 1项。区间中其他非跳板函数不因此归类或排除。

本轮分类方案：以原元数据稳定ID登记原入口/末界、IAT位置、DLL、当前IDA导入名及序号、原引用位置、现有源码调用点和既有合同。对序号导入，DLL和ordinal才是原导入身份；__imp_标签不是映像有名导入的证明，API对应沿已记录的网络/音频取证。无序号的有名导入不改写成序号。不会为了这些6B跳板另写同名空包装，也不把新端源码存在系统API调用等同于最终链接、旧DLL内部实现或业务流程全部等价。

| 原入口 | 导入边界 | 当前源码承接与范围 |
|---|---|---|
| 534980 | DirectDrawCreateEx | RendererResources.cpp的RendererInit_44B630→SdlPresenter.cpp的CreateRendererPresenter；沿已批准SDL创建替代，保留上层失败处理，不恢复COM接口 |
| 534986、53498C、534992、534998 | ImmGetConversionStatus、ImmAssociateContext、ImmCreateContext、ImmReleaseContext | ImeLifecycle.cpp的初始化/关闭；转换状态另由WindowProcedure.cpp使用 |
| 53499E、5349A4 | ImmIsIME、ImmSetOpenStatus | ImeLifecycle.cpp的布局检查与关闭/重新打开顺序 |
| 5349AA | ImmGetDescriptionA | RenderInput.cpp的RenderInput_4590E0 |
| 5349B0 | ImmGetCompositionStringA | ImeComposition.cpp的Sub_459580 |
| 5349B6 | ImmGetCandidateListA | WindowProcedure.cpp通知分支；四处原引用与新端分支合并不得只按API出现次数推算完成 |
| 534A30、534A36、534A3C | send、closesocket、recv | NetworkRuntime.cpp；关闭另由LoginState.cpp连接状态使用 |
| 534A42、534A48 | __WSAFDIsSet、select | LoginState.cpp直接调用；NetworkRuntime.cpp使用原生FD_ISSET边界及select，不模拟旧fd_set字节布局 |
| 534A4E、534A54、534A5A | WSAStartup、WSACleanup、WSAGetLastError | NetworkRuntime.cpp初始化/退出/错误叶；错误查询另由LoginState.cpp使用 |
| 534A60、534A66、534A6C、534A72 | connect、gethostbyname、inet_addr、htons | LoginState.cpp的Sub_508360 |
| 534A78、534A7E、534A84 | setsockopt、ioctlsocket、socket | 同一连接入口的套接字创建与配置 |
| 534A8A、534A90、534A96 | Module32Next、Module32First、CreateToolhelp32Snapshot | MainLoopSupport.cpp的CheckProcessModules_4042D0；快照另由CheckProtection_4043B0使用 |
| 534A9C、534AA2 | Process32Next、Process32First | MainLoopSupport.cpp的CheckProtection_4043B0 |
| 534AA8 | DSOUND序号1 / DirectSoundCreate标签 | 仅记录52B007的原引用；音频仍暂缓，不计该导入替代已完成 |

IME继续沿现有IMM32 A接口旧字节合同；HIMC/HKL/HWND和原生返回在平台边界处理。Winsock使用现有本机SOCKET/fd_set，CMake声明ws2_32；原WSOCK32导入分类不承诺跨版本内部实现一致。newest排除链也引用同一组网络API，不能因为系统API已经使用就恢复这五个排除函数；加密最终接线、服务器wire合同及完整收发继续待决/未验。

快照/模块/进程枚举沿MainLoopSupport当前原生结构，将dwSize赋为sizeof整个宿主记录；没有运行这些API，也没有改为固定成功或复制原x86结构尺寸。

DirectDraw两处原创建尝试按[SDL呈现合同](cg2d_sdl_presenter.md)已有替代，不声称GUID、HRESULT或重试次数逐项相同。DirectSound仅登记暂缓状态，既有局部SDL音频并不使完整音频或murD/52C3D0自动完成。本轮不改任何业务、API调用、CMake依赖或源码文件，只补导入位置分类；全部父流程和真实运行仍以各自验收证据为准。

## 登录及角色请求的命名入口映射

2026-10-02，沿直接定义定位清单补九条不在名称中保留地址的现有本体映射。只核当前入口/调用位置、已保存合同及必要元数据，不无差别重做已完成的原算法验证；四组请求启动/轮询和共享结果读取均有真实唯一本体，不新增包装或假返回。

| 原入口 | 现有本体 | 实际直接调用侧 |
|---|---|---|
| 508910/508950 | CharacterListRequest.cpp：StartCharacterListRequest/PollCharacterListRequest | CharacterListProgress.cpp：UpdateCharacterListProgress_45DD60 |
| 508AF0/508B50 | LoginRequest.cpp：StartLoginRequest/PollLoginRequest | LoginState.cpp：UpdateLoginProgress |
| 508E00/508E90 | CharacterCreationRequest.cpp：StartCharacterCreationRequest/PollCharacterCreationRequest | CharacterSelectScene.cpp：Sub_45E7D0 |
| 508F70/508FC0 | CharacterDeletionRequest.cpp：StartCharacterDeletionRequest/PollCharacterDeletionRequest | CharacterDeletionProgress.cpp：UpdateCharacterDeletionProgress_45DC00 |
| 510140 | LoginRequest.cpp：ReadRequestOutcomeFlag | MainLoop.cpp五个静态调用点；另一个原引用403EBC仍随403E50发送接线待决 |

四组启动均在各自真实发送后发布共享请求状态和起始tick，但取样和清理顺序独立：列表先清两条记录；登录按+18/+50/+4C采样；创建沿共享属性/元素/图号/动作及名称；删除采对应记录+4C。原socket为地址边界，新端保持本机宽度；选择值仍按原S16解释，其他协议数值不扩宽。

轮询不能合并为一种通用成功判定：列表回复1/2/其他分别为1/-3/-2；登录先有轮询开关，回复恰好1才设置共享结果并成功；创建恰好1成功；删除任意非零回复成功。各自未完成时保留U32经过时间严格大于600000的超时条件和独立原字节文案。510140原样返回完整共享S32，主循环仍保留五处独立读取；它不属于newest五函数范围排除。

本轮未改请求业务或发送/回复生产者；内层请求调用已有不代表创建场景入口门禁、取消加密后的wire接线或完整收发已闭合。新增映射只消除命名造成的定位遗漏，不将源码、历史验证或函数数量转换为整体完成率。

## 图形查询、资源读取及调色板命名入口

2026-10-02，沿直接定位清单补十五条现有实现映射。原合同分别沿[图形元信息](cg2d_graphic_info.md)、[资源实现](cg2d_resource_implementation.md)、[解码核心](cg2d_graphic_decode.md)及[变换缓存](cg2d_graphic_dx_cache.md)；本轮核当前源符号、关键数据/返回边界及必要调用证据，不重复编译或全量复检原算法。

| 原入口 | 当前源码/符号 | 必须保留的边界 |
|---|---|---|
| 44D3F0 | GraphicDxCache.cpp：SurfaceTransformKeyLess | 非拥有T*只在比较时转完整uintptr_t；后续S32图号、S16调色板、矩形left/right/top/bottom有符号顺序 |
| 45A6C0 | GraphicRead.cpp：CloseGraphicFiles | 先释放/清mapNo索引，再依九槽关闭/清文件；不释放主表或共享缓冲 |
| 45A710/45A760 | GraphicInfo.cpp：GetGraphicXY/GetGraphicWH | 无符号索引比较，首个S16输出写完后重读第二字段；失败按序清零 |
| 45A7B0 | GraphicInfo.cpp：GetGraphicSxSy | 源U8零扩展到两个U16输出，保留写入与重读顺序 |
| 45A810/45A860 | GraphicInfo.cpp：GetGraphicFlag1/GetGraphicFlag2 | 零扩展后分别模100/除100，不是位掩码 |
| 45A8B0 | GraphicInfo.cpp：GetGraphicMapNoIndex | 原无索引检查，写查表结果并返回1 |
| 45A8D0 | GraphicInfo.cpp：GetGraphicMapNo | 独立S32负数/上界判断，失败-1，否则原mapNo位型 |
| 45A900 | GraphicRead.cpp：ReadGraphicDataByIndex | 图号/文件/短读失败不清输出；成功读后先发布共享目标，再解码 |
| 52D5C0 | GraphicDecode.cpp：ReadGraphicData | 六参数指针边界、资源头W32宽高/长度、原始及RLE分支，成功返回初始目标 |
| 450470 | Palette.cpp：InitPalette32 | 共享逻辑及32位目标已承接；低色深目标按既定排除，8位源索引/调色板继续保留 |
| 450A80 | Palette.cpp：ReadPalette | 先读中间224色并关闭，再复制当前首尾16色；EOF字节处理不改为新失败 |
| 450B70 | Palette.cpp：SelectPalette | 拒绝返回原index，接受返回原steps；仅发布的非正步数改为1 |
| 451220 | Palette.cpp：RebuildPalette32 | 00RRGGBB纯黑先改1，最后仅索引0清零；返回400H，flags不是alpha |

补核45A6C0完整21条指令与唯一直接caller457A6D：其后立即调用457670，后者首指令重新写EAX。现有void清理接口不发布残留寄存器值；清理范围仍限mapNo索引和九文件槽，不因此扩大至GraphicInfo主表、CPU表面或其他资源。

图形元信息的S16宽高不能替代资源头W32解码宽高；共享读/解码区也不能改为每调用私有副本。ReadGraphicDataByIndex的核心失败仍保留已发布的目标指针，省略paletteLength时沿原局部槽，不新增重试、预清输出或容量修复。变换比较沿既定x64地址适配，不承诺原地址、树形或逐项释放序相同。

本轮只登记已有本体，未修改业务、存储、图形格式、CMake或色深范围。十五条映射不等同于完整加载、像素、设备重建、资源生命周期或运行验收完成；其余R1条目继续独立处理。

## 启动与场景分发命名入口

2026-10-02，补齐十六个已有命名本体与原入口的对应关系。沿[入口合同](cg2d_startup_flow.md)、[窗口生命周期](cg2d_window_lifecycle.md)、[窗口过程](cg2d_window_procedure.md)、[主循环](cg2d_main_loop.md)及[场景状态](cg2d_scene_state.md)登记；这是已有源码的归属定位，不是新增十六个业务函数。

| 原入口 | 当前源码/符号 | 对应边界 |
|---|---|---|
| 465790 | ClientEntry.cpp：WinMain | 原命令行、互斥量、updated门禁、窗口类与初始化顺序；最终0 |
| 4659F0 | StartupCommon.cpp：ParseClientVersion | 三版本分量与sscanf完整返回 |
| 465A20 | StartupCommon.cpp：CaptureDesktopColor | COLOR_BACKGROUND的U32保存及返回 |
| 465A30 | WindowLifecycle.cpp：SetWindowedDesktopColor | 设置00A4A0A0；模式门禁在调用者 |
| 465A80 | SetupIni.cpp：ReadSetupIni | A接口容量/字节，末次sprintf返回；屏幕键盘沿已批排除 |
| 465F00 | CommandOptions.cpp：ParseCommandOptions | 既有原字段与字节解析；返回末次507C80结果 |
| 4669F0 | WindowLifecycle.cpp：InitializeWindow | 调用后重读模式；已有/新建窗口分支；公共尾ShowWindow及UpdateWindow返回 |
| 466D50 | WindowLifecycle.cpp：PumpWindowMessages | PM_REMOVE；WM_QUIT立即0，队列清空1 |
| 466DC0 | WindowProcedure.cpp：WindowProcedure | 真实回调本体由gWindowClass.lpfnWndProc接入 |
| 532E70 | StartupCommon.cpp：CreateSaveDirectories | data内0至9目录，忽略API结果，最终1；本轮未执行 |
| 456050 | ResourcePaths.cpp：BuildResourcePaths | 先九槽六组绑定再逐槽构造；最后coordinateInfo的sprintf结果 |
| 457020 | MainLoop.cpp：MainLoop | 真实主循环及清理本体，两个正常出口0 |
| 51C430 | WorldUpdate.cpp：Update | pending提交顺序、原场景分发；void |
| 51C770 | SceneState.cpp：SetCurrentSceneType | 清当前状态→写类型→返回S32类型 |
| 51C790 | SceneState.cpp：SetCurrentSceneTypeAndState | 写当前类型→写当前状态→返回S32类型 |
| 51C7B0 | SceneState.cpp：SetPendingSceneType | 清pending状态→写pending类型→返回S32类型 |

WinMain确有本体且仍编入启动静态库；当前没有完整客户端链接目标，原PE图标和11光标资源也尚未接入。不以创建演示入口替代这条真实启动链。三个场景setter只承接原字段写入，不额外清窗、释放资源或立即提交pending。

本轮同时纠正Startup.hpp接口总注释及启动文档中ParseCommandOptions、MainLoop、WindowProcedure的过时状态。WorldUpdate场景4的4626A0门禁适配仍待确认，取消加密后的网络接线仍待决，音频继续暂缓；本体及命名映射存在不等于上述依赖、最终链接或运行验收已完成。未改CMake、业务分支或待批行为。

## 两个字体测宽命名入口

2026-10-02，登记44B2A0→cg::login::detail::TextMeasure与
44B2E0→cg::render::Font::measure，均位于Font.cpp。
对应[字体合同](cg2d_font_measure.md#测宽入口额外返回分支对齐2026-10-02)；
本轮核24/33条完整原指令后移除旧源码额外的空指针和越界字体索引返回0分支，
没有把二者合并，也没有改变字体度量、调用方、正常文本或编码边界。

44B2A0直接读取首字节，NUL先退出；无预算，推进后先读下一字节再累加宽度。
44B2E0先检查有符号非正预算，满足时不读text；正预算按高位推进1/2字节，
尾部才检查已消费字节，剩余预算1仍加入完整双字节宽度。
两者均无尾字节解码/校验。异常沿RPLAN.24，不仿真原故障、不记异常验收通过；
映射与静态分支修正不等于完整字体绘制、输入生命周期或运行验收完成。

## 登录状态、消息、黑名单与游戏时间

2026-10-02，登记七个已有命名本体，并把已批准的newest五入口单列为范围排除。
[登录合同](cg2d_login_state_controller.md)、[黑名单合同](cg2d_blacklist_state.md)、
[游戏时间合同](cg2d_game_time.md)继续分别约束各模块；不以名称或数量推定全链完成。

| 原入口 | 当前对应 | 保留边界 |
|---|---|---|
| 45D990 | LoginState.cpp：UpdateServerConnection | 原连接阶段、非零结果的对象清理与黑名单加载；newest沿已批排除 |
| 45EA50 | LoginState.cpp：ResetLoginProgress | 仅清W16阶段，不清请求或计时状态 |
| 45EA60 | LoginState.cpp：UpdateLoginProgress | 创建后重读阶段，同帧启动；非零轮询结果先清对象再返回 |
| 45CE00 | LoginMessage.cpp：ResetLoginMessage | 仅清W16消息阶段，不清结果或共享正文 |
| 45CE10 | LoginMessage.cpp：UpdateLoginMessage | 原字节含NUL复制、仅LF分行、测宽与特殊文本分支 |
| 401030 | Blacklist.cpp：LoadBlacklistFile | 清路径/记录不清游标；feof循环；打开失败仍发布fileId并返回W8的1 |
| 458DE0 | world/GameTime.cpp：UpdateGameTime | U32时间分解；minutes先于sTick写，返回phase/192商 |

50FC30、50FE30、50FF10、50FF70、510120按2026-09-21用户裁决仅排除
newest HTTP/raw-socket请求链。JSON的sources为空，scope_control_sources指向
落实排除的UpdateServerConnection；该字段是范围控制位置，不是被排除函数的替代本体。
五条不能计为已实现，不能外推排除510140、常规登录请求、网络队列或取消加密后的帧处理。

本轮没有修改上述业务代码或恢复音频。窗口报告4589A0/4589B0/4589C0
本体各只有读取73ECF4、清73ECF4、读取73ED08，唯一caller均为待决403E50；
现有MainLoop共享owner已承载原W32状态。本轮未重复造无消费者包装，
也未把这三入口或网络父回调登记为已有实现。
