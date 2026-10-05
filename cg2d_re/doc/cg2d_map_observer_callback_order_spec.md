# MOC01 观察回调参数与排序读取序

<a id="map-observer-callback-order-baseline"></a>
## 真实回调与缺口

沿[MOS01](cg2d_map_observation_source_spec.md)和[MCP01](cg2d_map_chara_producer_spec.md)的源/生产者继续核440C50。现回调及真实场景接线已存在；本次修五处复合表达式的读取序，避免把C++未指定的参数/操作数求值顺序当成原指令顺序。算法、五槽、排序依据均不重建。

[原证据](cg2d_map_observer_callback_order_evidence.json)保存[440C50,440F4F)完整233条767B。唯一入口引用440B70@440B85为type=1的callback地址写入，**不是直接call**；原生产者/新callback赋值及既有YobiCallback调度可追，但不代所有间接调用/完整owner寿命。

1. 原440C6A取payload、440C79读frame、440C7C读方向，之后440CB8读共享时长。现frame/direction在同一函数调用参数中读取，顺序未显式固定。
2. 每个5182B0返回后才取父frameX/frameY和父X/Y，再依原读取相应表偏移；原X：440DCC取frameX、440DD2取父X、440DD5先加frameX、440DD7取局部offsetX、440DDB加父X、440DDF加offsetX。Y同序为440DF0/DF6/DF9/DFB/DFF/E05。现复合坐标表达式未固定这些读取。
3. 排序每次先取后一S8序号并判负，再复读当前S8序号；440E83/87先取当前/后一子指针，440E8B后读当前deltaY，440E8E再比较后一deltaY。现复合比较无法表达“两指针先取、当前深度后读”；交换后当前项继续用于后续比较，不能统一缓存整轮。
4. 子绘图原440ECE取子指针，440ED2先取描述符地址，440ED8读图形索引，440EDC/EE0依序读局部Y/X，再调用绘图。现函数实参内直接读坐标表/图形/描述符，顺序未固定。

<a id="map-observer-callback-order-decision"></a>
## 委托裁决

1. 仅440C50五限定片段，入口用S32 frame→direction局部取样后传现有轨迹叶；共享时长仍通过原指针在既有叶读取。PC53/MASM、原FST progress/两次S64截断及已批准纯数值拆分保持，所有数值叶/表位型不改。
2. 两轴各在原偏移查询返回后显式取父frame坐标、父像素坐标，先算图形偏移加frame，再取局部槽偏移，依原关联加父坐标/槽偏移。全为现有正常S32运算，不添范围限制/回绕模拟。
3. 内层先保存nextIndex并判负，再读取currentIndex、两子指针、当前deltaY、比较后一deltaY；小于才按next→current交换两S8值。不跨比较缓存索引/深度，不换排序算法或改drawY排序，不加子空值/死亡过滤。
4. 绘子项先取描述符地址→图形索引→Y→X，再用已取样值调用原绘图；layer10/两个0、非拥有描述符寿命和槽指针保持。五槽扫描/非空FrameMove及其后子指针复读、局部坐标/序号发布、-1填尾、绘主项、绘后frame递增均保持。
5. 唯一原ID补真实源码/原callback生产与adapted，MAP/LOOP共用身份。只存在私有本体/YobiCallback签名，不伪造公开440C50声明；既有440B70公开接口、调度/其它回调/源和头/汇编保持。
6. 不新增保护、补零、拒绝、钳制、异常回绕或故障模拟/称等价；原格式明文与音频暂停保持，无新增音频实现。

<a id="map-observer-callback-order-acceptance"></a>
## 限定验收

- 当前全文只提交前源码加五具名片段，入口/两轴读取与加法关联、排序索引/两指针/当前深度、子绘图描述符/图形/Y/X可核；既有轨迹叶/完整五槽和父绘制/frame递增保持。233条767B及库存和唯一type1生产引用可追，不记直接call。
- 语义1290→1291、有效ID1286→1287、处置383→384，MAP46→47/LOOP11→12成员，E229→233仅两域各局部I/S，共用一回调实修/一原体，不重复计函数。
- G01/G02/U11/R01/R04/V04仍open；233条其它语义/所有数值及绘图callee、完整父与全上游、所有间接别名及owner重进退出/借用失效、实际ABI/SDK/PC53/时钟RNG/画面运行未验。全部完整门禁、R1/R7/R8及整体验收不升级；不编译/调试/测试/运行/查产物或计算摘要。

<a id="map-observer-callback-order-source-state"></a>
## 当前交付

440C50入口、两轴、排序及子绘图的读取按原分段明确落码；原算法、轨迹数值叶、五槽/排序和原生载荷/描述符寿命保持。局部修正不代整个观察链与场景运行验收。

固定命名快照：`cg2d-re-map-observer-callback-order-20261005`。
