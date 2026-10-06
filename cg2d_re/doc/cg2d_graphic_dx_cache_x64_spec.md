# DX 图像缓存 x64 承载 SPEC

> 2026-10-04按用户要求压缩更新；已批准方案与未闭门禁分别保留。压缩前全文见Git命名快照 `cg2d-re-spec-history-20261004-before-closure-compression` 的本路径，历史提案不再作为当前待批清单。
> 关联：[总SPEC](../plan/cg2d_full_reconstruction_plan.md)、[唯一进度](../plan/cg2d_full_reconstruction_progress.md)、[收尾任务G01–G04/C09](../plan/cg2d_spec_closure_checklist.md)。当前只做静态核对，不构建/测试/运行；异常按RPLAN.24，不恢复故障模拟门禁。

## 当前状态速查

| 范围 | 已落位 | 当前剩余 |
|---|---|---|
| 四表/节点/资源 | A案32B图项、四表分配、4096节点/reset/CPU建面、调色板/DBF、恒空建链及520B10上传 | 全场景初始化/重建/清理、共享owner与正常生命周期 |
| 缓存/队列/绘制 | 原生地址双缓存、48B联合队列、RenderChara、纹理父绘制、DrawAllQueued、507140本体及指定caller | 完整场景生产/借用寿命和清理；两树全应用原序与逐面寿命；本组具名宿主构析已接 |
| 设备与文字 | RendererInit接口/设备、格式/尺寸生产及探测；case4原字节文字绘制 | 模式2/3/4、目标字体、像素与完整运行对照 |

源码存在或历史静态库通过不等于初始化、生命周期、链接或画面验收。不得按旧记录“消费者未实现”重复做已有本体。

## 1. 继承证据与覆盖边界

原证据集中在[DX合同](cg2d_graphic_dx_cache.md)、[507140](cg2d_sub_507140.md)、[MainLoop](cg2d_main_loop.md)、[纹理绘制](cg2d_texture_rendering.md)及[呈现器](cg2d_sdl_presenter.md)。G1a范围内已核103CFA4直接用途，无原项整体复制/持久化/地址数值编码；不外推为全程序无别名或无间接访问。

原未知+0C/+0D/+0F不是填充，新对齐空隙也不是原未知字段。原地址、字段和指令详证不复制到SPEC；已证内容无冲突不重验。

## 2. 首选 A：原生非拥有指针

A案已于2026-09-27依工程委托选用，当前不是待批建议。私有DxGraphicEntry自然8B对齐、32B；只拓宽head/palette，不打包、不重排标量。

| 字段 | 原偏移/宽度 | 新偏移/宽度 |
|---|---|---|
| head / TextureNode* | +00/4B | +00/8B |
| width / W16 | +04/2B | +08/2B |
| height / W16 | +06/2B | +0A/2B |
| palette / PaletteCacheItem* | +08/4B | +10/8B |
| 未知字节、未知字节、flag、未知字节 | +0C/+0D/+0E/+0F，各1B | +18/+19/+1A/+1B，各1B |

2200000项为70400000B（70.4MB），原35200000B；预算不含资源池。尺寸按消费者阶段解释S16/W16，不统一改成正数，步长用新sizeof。扩大内存压力/失败时机是已披露差异，不归DX驱动豁免。

历史B案“16B+U32池编码”未采用；不得恢复句柄表、代际或生命周期新语义。

## 3. 候选文件与所有权

原候选已落位：GraphicDxCache实现私有DX表、节点与缓存；GraphicRead持独立调色板/DBF；GraphicCacheState只承接其尾槽，SurfaceState持共享Back/Temp，SdlPresenter为最终像素边界，均不冒充DX全部owner。

gMaxGraphicCount唯一持有容量；DX条目借用node/palette，node.next非拥有。4096纹理槽、104动态调色板槽、4静态共享槽分离生命周期；动态项按原序释放，静态共享项不逐图释放。复用已有资源缓冲、帧计数及共享状态，不造影子尺寸/队列或小模块。

## 4. API 责任边界

LoadGraphicDx_522570保留真实加载、门禁、返回与副作用；发布base不提前解引用/快照，宽高在原阶段读取。初始化结果发布、引用失效和最终释放分开，不自动清槽、整体reset、保护或伪造原签名。

SDL/CPU创建销毁留render边界，公开队列不暴露底层SDL/Win32对象；不新增依赖或RTTI。原生替换不豁免正常owner和上层时序。

## 5. 六阶段合同

### 5.1 初始化

BuildSurfaceCache→调色板池初始化→InitPalette→GraphicAlloc→引用重置→GraphicInit。四分配逐槽发布，全部成功才清表，不提前清零或事务式抹除失败残留。

### 5.2 单图尺寸生产

原门禁→资源→取宽低W16→写当前图号→写w→重读表基址→取高低W16→写h→建链。读取失败只清工作输出，不清旧条目尺寸；工作宽度对齐不反写条目宽度。

### 5.3 纹理引用

原时点保留旧image解绑、条目head清除、新head发布、next链接及state写入。实际节点数、U32比较、有符号除余/对齐不变。节点不足保留半链，不补尾next=0、不回滚、不改外层成功返回。

### 5.4 调色板引用

旧缓存image失效→清条目引用→取得并发布缓存项→上传→复用/老化。P状态与DX尾字段分开；绘制只清调色板缓存+0C状态，不清DX未知字节。

### 5.5 重建

Destroy→Init→BuildSurfaceCache→节点reset→两个引用置0。不替换/重新分配整表，不清w/h、flag或未知字节，不提前失效。

### 5.6 退出

动态调色板释放并清池槽→node.surface释放并清surface→关闭资源→释放DX表。DX槽与容量不清，失败回收也不自动清槽；残值合同不代表悬空引用可安全重复使用/释放。

## 6. 真正的后续前提

| 门禁 | 已证/已裁决 | 剩余边界 |
|---|---|---|
| G2 | 四分配/失败顺序、首轮无表、显式启动及四条应用清理返回链已有证据 | 回调/动态调用、清理callee、FreeLibrary/CRT退出及相关静态目标的重入/重复清理可达性未整体闭合 |
| G3 | 节点字节账本/快照写序、建面发布/统计与失败残留已证；现代节点及factory已有 | 尾字节运行写者/含义、错误回调、面/缓存/owner全生命周期未整体闭合；装载零初值不证明运行值 |
| G4 | 5222D0第二参数恒空，非空类型/生产者门禁按用户裁决撤出 | 空路径共享宽高、扫描/节点链及正常生命周期仍保留；不改写为全程序不可达证明 |

G3节点原+1A/+1B首末位置已证映像装载零值，字段归属/宽度/含义和全部运行写者仍不能猜。surface原U32树序的直接用途为查找/插入/顺序清理，未见直接依树序绘制不等于所有间接影响不存在。

## 7. 后续源码六小步

原拆分为表承载→纹理池→调色板池→单图生产/建链→生命周期→消费者。前四及指定消费者已落位，后续聚焦收尾表G01–G04/C09；不重新把六步全部列待实现。验收仍含真实生产、共享身份、发布/失效、半链/残留和消费者写入，不以尺寸接口或节点数量代替全链。

## 8. 待批清单与风险

旧A案、模块归属、六阶段责任和地址键/队列方案已由后续裁决覆盖，不再求同一批准。未决仍是具体生命周期/共享owner、双树全应用原序及逐面清理、未获批正常差异和运行验证。

RPLAN.24不专项处理越界/数学溢出/回绕，也不新增保护、令牌耗尽或默认空值路径。表预算/分配失败差异、未清槽和悬空使用分别说明；DX7原生替换不自动豁免。静态断言写入源码不等于已执行编译验证。

## 9. GraphicAlloc 与 SDL 承载门禁（2026-09-27）

GraphicAlloc@[4574E0,457667)不创建surface，SDL不能代它的路径/容量/四块身份：

1. 顺序用rb探测九GraphicInfo路径，成功立即关闭，失败仍继续。
2. 先写五W32标量，再P1/P2/P3/P4连续分配且逐槽发布（含空），无中途短路。
3. 联合失败只依P1→P2→P3释放非空块，不释P4、不清槽/回滚标量。
4. 全成功按P1→P2→P3→P4清零，各阶段重读容量；返回完整W32的1/0。Sub_457670同样只按P1→P2→P3释放且不清槽。

P4的+18/+1C、子项+18的指针性来自读写/free指令，不信IDA int标签；原36B/子项28B不能直接承载拓宽指针。既有地址槽/标量保持真实共享身份，不能降成局部常量。父调用与动画生产已有后续实现，完整生命周期未验。

## 10. x64 表承载选择与分配落位

| 表 | 原与新承载 | 分配/边界 |
|---|---|---|
| P1 AnimateRecord | 原36B；+10/+18/+1C三引用拓宽到新+10/+20/+28，项56B | 56×20500；ACB01撤早期额外零哨兵，固定项数对应原45750F，包含上界查询保持 |
| P2 DxGraphicEntry | 原16B→32B，见§2 | 32×2200000 |
| P3 GraphicInfo | 已证无指针32B不变，复用唯一gGraphicInfo | 32×2200000 |
| P4 AnimeCacheRecord | 六W32后两个原生指针及尾4B，原36B→48B | 48×20500；子表生产/释放按动画合同 |

P1/P3复用原owner，P2/P4独立槽；现代尺寸不冒充原malloc长度，内存压力/失败时机不声称逐次等价。四块发布/失败/清零严格按§9。[动画合同](cg2d_animation_runtime.md)记录排序步长、引用与子表细节。历史“InitializeClient仅声明/动画未接”已被后续覆盖；当前剩余按页首，不以旧编译结论代新链接/运行。

## 11. 纹理节点池与 reset 的局部选择

TextureNode原28B→新40B：两个原W32引用改原生非拥有指针，图号/state仍W32，三W16快照与两未知尾字节独立。容量4096；实际count和扫描下标各为独立S32槽。

reset无条件先清下标，正count时只依次写image=-1、state=0、next=nullptr，不清surface或其他字段。surface排序不截断地址；布局采用不等于G3生命周期通过。

## 12. CPU 纹理建面与缓存构建的当前状态

CPU面替换原DX纹理factory；两种原caps对应两次独立分配尝试。BuildSurfaceCache入口count=0，surface先发布再判空，模式降级不恢复，两统计只记模式非零返回；失败不reset/invalidate/释放前缀，整轮成功才发布4096并走原尾调用。factory消费低S16宽高，node参数不是out-pointer。

MakeSurface已接节点/变换面、Temp回退的+2/幂次方/正方形/格式门禁；AutoMap与Temp首次保留原尺寸，快照按实际pitch无色键复制。RendererInit设备/接口/模式生产和探测已有，模式2/3/4与画面/生命周期未验。详见[DX §77](cg2d_graphic_dx_cache.md#77-buildsurfacecache-的-cpu-建面替换与发布边界)、[纹理合同](cg2d_texture_rendering.md)。

## 13. 独立调色板归属与恒空参数裁决

2026-09-27用户委托选择owner，明确第二参数恒空；当前有效合同：

- 103CF88队列、103CF18计数、103CF1C索引、103CF0C的0xB04记录与522900 DBF解压索引，统一归GraphicRead。索引首W32记录指针拓宽，+4的W8标志单列；记录仍两个W16、两组256个W32及0x300原BGR字节，不造副本。
- render 520B10/523140窄访问同一记录；DX.palette仍指PaletteCacheItem，不能混成独立记录或用AutoMap代非零索引。522B30/522C50保留分配发布、记录池先于索引池释放。
- 5222D0保留未使用void*第二参数，不读/判断/解引用、不加断言/错误；522570显式传nullptr。只走共享宽高、宽度对齐、扫描读取和建链空路径，不实现非空+4/+8读写分流，也不再要求非空生产者；原证据保留。
- 522900/522B30/522C50、522CB0/523140、恒空建链、520B10上传及53AF90本体已有，部分父/退出已接；全初始化/重建/场景清理/上传画面仍须按G01核，不能把局部实现当全链。

## 14. 已批准：surface 变换缓存键使用 64 位地址

2026-09-27批准地址缓存64位、T*优先：保存TextureSurface*，指针相等核身份；不同地址只在比较边界转uintptr_t做无符号排序，不用未关联指针内建小于、不截U32。图号S32、调色板低S16及四矩形S32不扩宽；保留场景10/其他场景双树选择。

两树留既有渲染实现，用有序容器承接业务缓存；不把旧节点/28B栈键当x64布局，不增句柄/递增ID/代际/引用计数/像素快照或寿命。保留清理时点与逐项释放，不新增按帧清空、隐式失效、地址复用保护/回滚。

新端地址顺序及同次清理不同surface的释放次序可能不同；这是已批准表示差异，不保证旧地址、STL树形、分配失败时机等价，也不豁免像素/其他键分量/正常控制流。44CD80/44D490及父绘制已有。

**2026-10-04后续裁决：按[启动与生命周期子SPEC](cg2d_crt_startup_lifecycle_spec.md)，宿主容器承接私有树节点/共享空节点，不再要求复造原节点布局；应用清理时点、逐surface释放与唯一owner仍须接线。业务有序容器存在不等于这些接线已完成。** 后续已证629300/62A630两树、62B0AC计数/62B0B0空节点及44CC30/44CD40构造、44CC60/44CD70析构共享关系；不能只凭std::map存在或业务clear就宣称应用生命周期闭合；原私有树机制按获批宿主替代分类，原证据保留。该债务见[启动合同](cg2d_startup_flow.md)和C09。


2026-10-04实施增量：[双树构析子SPEC](cg2d_graphic_transform_lifecycle_spec.md#transform-cache-lifecycle-source-closure)已将两map改为唯一被动存储和具名construct/destroy，各自即时真实宿主退出登记；当前模块适配仅按相邻两槽调用，三个场景选择点借同一对象。原节点退出不逐surface释放，44D490业务清理保持；宿主私有节点/空节点职责与原事实分列。整表接原应用相对序须同时移除模块适配，C09与全资源生命周期仍开放，未构建/运行。

## 15. 已裁决并实施：队列的类型化对象与文本联合字段

采用8B无所有权联合字段：`YobiAction* action / uint32_t textToken`。不新增tag，按原生产/消费路径区分，不按数值范围猜用途，不采用旧507140对象令牌表方案。

| 字段 | 原x86 | 新x64 |
|---|---|---|
| x/y/imageNo | +00/+04/+08，各4B | 不变 |
| 对象/文本引用 | +0C，4B | +10，8B；+0C..+0F自然对齐间隙 |
| value4 | +10，4B | +18，4B |
| descriptor | +14，16B | +1C，16B |
| batchType / 未写尾3B | +24/+25 | +2C/+2D |
| 主记录 | 40B | 48B，8B对齐 |

GraphicCache3仍4B；LegacyPendingStorage前缀0x88B、记录从+0x88，4096条后排序区+0x30088、整体0x34088B。gLegacyPendingStorage唯一持有，gTextFF4E30别名原前缀；索引-2写batchType必须显式写prefix[0x5C]，不得按新步长算负索引。源码断言表达布局，当前未编译执行。

普通图形/排序零写为action=nullptr，文本只写textToken并保留value4/batchType次序，不额外清另一用途尾部。RenderChara原5220F2发布同一action后重取flags；不经普通DrawGraphic回填。绘制仅在原门禁后读取+5C/+60缩放，命中先过value4和真实图加载，case4交文本token，不能因引用非零提前解对象。

action仍归runtime/list，队列非拥有；清计数/覆盖/MarkDeath不释放，不增登记/代际/按帧失效或改变payload/action摘链释放序。布局/RenderChara/DrawAllQueued/507140指定接线已有；完整场景、寿命、设备模式和画面未验。

## 16. RenderChara 的对象存储与数值承接

完整本体在YobiRuntime，复用私有action、首尾哨兵和唯一队列，World::Update六处原调用不变；不公开布局、不增遍历快照/回调层/临时表。证据见[Yobi合同](cg2d_yobi_runtime.md#renderchara521f20-完整对象入队)。

- 名称为原+64..+A3的64B（来自41EBBD–41EBCE复制，非IDA20B标注）；+A4/+A8两个S32坐标、+BC/+BD/+BE三个W8、+C0 W32。由既有calloc(sizeof(YobiAction))初始化，不造字段生产者、不截断/补NUL。
- 521F90测action+64内嵌地址，不测首字符；有效对象直接交DrawText2，不加非空字符串门禁。callee低S16坐标/低S8文本参数不变，名称后重取W16图形计数；628FC8复用gFrameParity628FC8。
- 原CRT链设置x87 PC53，正常有限乘法用double(S32 offset)×double(F32 scale)，向零转int64取低S32，再加原屏幕坐标；不先转F32或合并为浮点加法。不新增全局控制字，不承诺外部舍入/精度改动等价；非有限转换/溢出按RPLAN.24。
- 按原序名称→容量→图号/死亡/隐藏门禁→descriptor→颜色/混合掩码/alpha覆盖→发布action→重取flags。帧奇偶、特殊图标志/W16计数保持；实现完整函数不代其他生产者或全场景验收。

## 模式文件与缓存不足两真实格式板块 SPEC（2026-10-04）

两真实格式点已接53B304，其余已批SDL/CPU与正文不变：

| 原点 | 必须保留的合同 |
|---|---|
| 44C370（属44B630） | 正常初始化支持模式不等才WriteRendererModeFile(false)；CreateFile成功后采628FEA的S8→S32，以5531A4的%d写现已清11B缓冲，再strlen不含NUL→WriteFile→CloseHandle。原完整目的容量未证明；fallback true只写原'1'，helper不造独立原身份 |
| 52254A（属5222D0） | scan绕回scanStart才将当前S32索引按58D348原%d+LF写未清256B→调试输出→返回；领取/解绑和半链不清不回滚，不补尾next，不改reuseState/扫描/块大小/调色板/恒空参数 |

该小步只静态核两个调用邻窗/格式/采样/差异，未重验两个完整父，也未实际写模式文件或运行。容量全链、像素、后端/CRT与总SPEC门禁保持。


## PGM01 独立调色板两组全局内存收尾（2026-10-05）

[专项SPEC](cg2d_palette_global_memory_spec.md#palette-global-memory-source-state)及[原两根126条/377B](cg2d_palette_global_memory_evidence.json)补522B30/522C50的同域生产/释放基线。
原GPTR/GlobalLock分配、索引16B仅首T*拓宽/loaded低W8及记录0xB04保持；只拆既有组合释放接口。
解锁后重新取当前owner释放，清records之前保存index，index解锁后再取当前owner释放；原八SDK调用/弃返回/无条件清指针保持。
两个private owner、队列/DBF和其他缓存借用保持，完整父/模式重建/上游/间接alias、借用失效、SDK/ABI及运行仍未验收。

## ACB01 P1固定项数与早期额外记录撤销（2026-10-05）

[具名更新](cg2d_animation_capacity_spec.md#animation-capacity-source-state)覆盖§10旧20501预算：原45750F固定20500×36B，新端固定20500×56B仅拓宽已证三指针；撤一过尾额外清零，原成功容量重读/P2-P4预算和全部发布/失败/清理序保持。

count包含上界不改，满容量一过尾按RPLAN.24不专项保护/补零或模拟/称安全/故障等价；现代分配字节/失败时机不称原x86逐次相同。实际上下游/完整生命周期/分配失败及SDK/画面运行继续G01/U11，不恢复异常门禁或扩大其它适配。

## ATP01 动画载荷类型承接（2026-10-05）

[具名类型实施](cg2d_animation_pointer_spec.md#animation-pointer-decision)在既有两头/装载/清理源完成五已证指针与三分配结果类型承接，P4 actionList只承接CacheActionEntry，前24B原字节+未知帧引用，不借主表动作字段。未知帧/P4.buffer/9A4A54继续void*；不新增对象owner或表容量。

P1/P4表56/48B、主动作/P4子项32B和帧/坐标12/8B保持，分配/清理原控制与取样序不改；ACB01固定20500项预算保持。静态断言是源码要求，不称执行过编译或实际ABI/完整生命周期/画面验收，异常不专项保护或模拟。

## BOS01 两公共入队根与描述符发布补充（2026-10-06）

[BOS01 SPEC](cg2d_graphic_enqueue_descriptor_production_spec.md#graphic-enqueue-descriptor-production-decision)/[原证据](cg2d_graphic_enqueue_descriptor_production_evidence.json)。公共521B10与520970两入队根完整227条704B/四页、4call-24内跳/6ret，422普通引用仅纳盘、父按根181/30分列。两根撤额外负计数拒绝、保S16上限4096/原-2；普通坐标撤S32回绕helper按正常S32+S16。真实record目标在map/XY前保存，描述符每对W32读取/发布四次，空参4W16→颜色W32FFFFFFFF→两U8→末W16；Draw3先实际descriptor pub再X/Y逐W16修正。clip先batch pub、cacheIndex/live计数和队列尾原宽/时点恢复；同文件helper不增原函数ID。

保持既有native队列记录/排序槽布局，action已证地址拓宽；入队body仅按原字段与已证顺序操作，非地址字段W16/W32/U8不扩，不能拿原x86物理跨度直接访问新T*记录。

旧7完整历史续记、1404新机器映射，语义1404→1405/有效ID1399→1400、关联1517→1518（41.22%→41.25%）、分类522→524，RENDER34→36枚举open；独立局部I/S共4条E651→655。RESOURCE41保持，历史定义0/1不刷新；七open仅进展/引用、60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及全门禁保持。

四文件补丁仅两根正文、同文件描述符helper插入、原S32回绕helper退休；其它根/既有W16减法/公开头/map-XY provider和队列布局保持。完整caller/正常源目标别名及排序/绘制consumer-owner/真实画面运行继续推进；不执行异常样例或扩大拒绝/保护策略，未构建/测试/运行/hash，网络原格式明文、音频暂停。

## BOT01 其它两入队根的字段发布（2026-10-06）

[BOT01 SPEC](cg2d_graphic_secondary_enqueue_production_spec.md#graphic-secondary-enqueue-production-decision)/[原证据](cg2d_graphic_secondary_enqueue_production_evidence.json)。521C80/521DD0两完整入队根206条650B/四页、3call-22内跳/5ret，26普通调用点只纳盘、父分根1/18。两正文保存callee前的真实record目标与原字段样本，复用BOS01原四W32逐对消费/空参W16-W32-U8默认发布；C80 clip先batch、DD0 batch先clip各按原序，保各图号门槛/映射与仅XY的不同职责和special范围。

两既有源码根新增机器映射1405/1406，语义1405→1407/有效ID1400→1402、关联1518→1520（41.25%→41.30%）、分类524→526，RENDER36→38枚举open；四独立局部static I/S E655→659。历史定位各1不刷新，RESOURCE41保持、helper不增原函数；七open仅进展引用，60源码closed10/完整验收0/global_accepted=false/accepted_leaf_features=0及其它门禁保持。

本步只整DrawGraphic.cpp两个正文补丁；BOS helper/另外两根、其它正文及布局/头/owner/provider保持。C80低图号直传，不借其它根增加[-1,999]拒绝；DD0图形号直接XY、不加map。正常S32坐标与W16-U8字段/计数保，不新加负拒绝/保护/异常策略。全部caller/正常别名及排序-绘制/owner-像素运行继续推进；未构建/测试/运行/hash，网络原格式明文、音频暂停。

## BOU01 地图/着色入队原生目标与栈生产（2026-10-06）

[BOU01 SPEC](cg2d_map_colored_enqueue_production_spec.md#map-colored-enqueue-production-decision)/[原证据](cg2d_map_colored_enqueue_production_evidence.json)。地图521AA0/着色45F090完整58条217B/两页，1call-1内跳/3ret；55普通调用点只纳盘，已知父分根1/8，地图495E47保parent=null，不猜所属函数。两个正文恢复地图实际record/cache3与x/y取样，及着色栈W32/W16/U8重复-重叠发布/八实参-S32返回；原正常着色最终值正确，不记颜色修复或故障等价。

两既有源码根新增机器映射1407/1408；语义1407→1409/有效ID1402→1404、关联1520→1522（41.30%→41.36%）、分类526→528、RENDER38→40枚举open；四独立局部static I/S，E659→663。历史定位各1/旧行号保，RESOURCE41/队列175/R7区间67/3061及其余门禁保持；七open仅进展引用，60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保。

两CPP只五补丁（两个正文、退休一个私有投影及两未用include）；BOS/BOT四入队根、描述符消费helper/队列owner/排序/总绘制/provider及公开头保持。保正常S32/W32/W16/U8/native地址分工、地图batch5及不写字段；不加负拒绝、图号/结果门槛、保护或异常回绕模型。沿真实场景生产、地图/绘制consumer-owner与正常别名/像素继续推进；未构建/测试/运行/hash，网络原格式明文/音频暂停，整体SPEC范围不缩。

## BOV01 描边批次无条件写入（2026-10-06）

[BOV01 SPEC](cg2d_hover_batch_unconditional_publication_spec.md#hover-batch-publication-decision)/[原证据](cg2d_hover_batch_unconditional_publication_evidence.json)。描边522180全41条139B/一页、1call6内跳1ret；195普通调用点/115父仅纳盘。原call521B10后无条件U8 batch2写；真实helper额外索引范围拒绝与旧249文字合同不符，本步撤该拒绝/size_t，按native实际record写batch。已证-2前缀+5C写及正确描边包装/四低W16-两W32/step图号标记和六实参保持，不称颜色或坐标错误。

旧249续记并保存完整旧record；源码冻结定位0/旧两文档事实保，不新增语义或helper原ID。语义1409/有效ID1404、关联1522（41.36%）保持；分类528→529、RENDER40→41枚举open；两独立局部static I/S E663→665。RESOURCE41/队列175/R7区间67/3061及其余门禁保，七open仅进展引用；60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保持。

只GraphicPendingQueue.cpp一个helper正文；正确DrawGraphicHover/BOS/BOT/BOU入队和描述符helper、排序比较/队列owner/总绘制/公开头保持。原容量门槛和失败后前缀写保，不加负拒绝/保护/钳制或异常回绕模型，不运行异常输入。既有void接口保，不称原EAX五倍索引残值/机器ABI、全部195点/115父、正常别名/描边case2填充-owner/像素运行已验。未构建/测试/运行/hash，网络原格式明文/内存CryptInt保/音频暂停，整体SPEC继续。

## BOW01 节点释放取样与真正Destroy根（2026-10-06）

[BOW01 SPEC](cg2d_renderer_node_release_sampling_spec.md#renderer-node-release-decision)/[原证据](cg2d_renderer_node_release_sampling_evidence.json)。真正Destroy根44C3C0完整136条412B/两页18call15内跳1ret、2普通点2父只纳盘；44C408只是20条46B内部节点段，不登记独立原函数。18调用按4 IAT/4 GlobalHandle寄存器/10 vtable+8类型保，不把操作数当绝对callee地址。单节点helper保存一次surface、非空同owner释放/清同槽，fresh count先取再推进index/native node，沿同样本判尾；正确销毁包装及其它阶段保。

新增机器映射1409对应已有真正原根；语义1409→1410/有效ID1404→1405、关联1522→1523（41.36%→41.39%）、分类529→530、RENDER41→42枚举open；两独立节点局部static I/S E665→667。冻结定位1/旧行号保，RESOURCE41/队列175/R7区间67/3061及其它门禁保持；八open包含C09仅进展引用，60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保。

仅GraphicDxCache.cpp一个节点释放helper正文；根包装/主线程与32位SDL-DX7承接、Temp/AutoMap/设备/接口/调色板/Back/呈现器/renderer owner及其它源码保持。不清count、next/image/reuse/W16或未知尾、不扫原count<=0失败前缀，不新增拒绝/回滚/保护/异常策略。完整原取证不代整个Destroy实现验收；正常节点段/包装调用序局部static不代全部父-callee/正常别名/逐面owner-双树-C09/重建退出/SDK-机器ABI/像素运行。未构建/测试/运行/hash，网络原格式明文/内存CryptInt保/音频暂停，整体SPEC继续。
