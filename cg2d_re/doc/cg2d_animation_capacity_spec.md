# ACB01 动画主表分配预算与查询身份

<a id="animation-capacity-baseline"></a>
## 原事实与本次缺口

[动画合同](cg2d_animation_runtime.md)和[DX承载SPEC](cg2d_graphic_dx_cache_x64_spec.md)早期批准P1额外一项零哨兵，源码仍分配20501项并额外清零。本步依当前RPLAN.24与工程委托重新裁决该适配，恢复原20500项预算；只拓宽已证指针，查询包含上界保持，不把越界域改成业务默认值。

[只读原证据](cg2d_animation_capacity_evidence.json)保存四完整体共186条548B、五直接call与一个比较回调地址引用：

| 原身份 | 原范围/条数/字节 | 本次 |
|---|---|---|
| F:000574E0 GraphicAlloc | 4574E0..457667；116/391B | 修P1分配/额外清零，既有语义补当前合同 |
| F:0005B390 BinarySearch | 45B390..45B3F7；48/103B | 既有正常查询登记，源码不改 |
| F:0005B400 GetIndex | 45B400..45B41D；11/29B | 既有count包含上界登记，源码不改 |
| F:0005B420 Compare | 45B420..45B439；11/25B | 既有unsigned比较登记，源码不改 |

45750F固定push738000，即20500×原36B；五共享标量先发布，四块连续分配与槽发布不因失败短路。P1成功清零在4575E5重读当前容量，P2/P3/P4再各按原阶段取容量/地址；失败仅释放非空P1/P2/P3、不释放P4、不清槽，返回0；全部成功返回1。

BinarySearch原signed min/max，跨度>=10用向零除2中点，按unsigned键分支；不足10包含max顺扫。GetIndex传(键,0,count)，负返回转−1。Compare首U32升序返回−1/0/1；45BEBD是回调地址引用，不记直接call或把父AnimeInit验收。

<a id="animation-capacity-decision"></a>
## 具名裁决与源码

ACB01明确撤销早期P1额外零哨兵适配，覆盖动画合同§3/§4及DX§10对应条目；这是本线程依工程委托作出的范围内设计更新，不追改历史验收。RPLAN.24不专项处理越界/溢出，不将满容量一过尾补零或故障模拟继续承载为本线要求。

1. GraphicAlloc的P1分配改固定20500U×sizeof(AnimateRecord)，对应原固定项数；x64项56B只源于三指针拓宽。撤额外一项memset，保P1原容量重读清零、其它三分配和全部发布/失败/清理/返回正文。
2. AnimationRuntime.hpp仅更新边界说明；主表类型/布局、原共享指针/计数/容量、查询实现、装载/排序/坐标/释放不改。
3. count<20500时同容量清零尾项仍在表内；count==20500时查询仍可能读一过尾。该异常域不专项保护/拒绝/补零/钳制/回绕或故障模拟，不称安全、确定值、异常修复或原故障等价。
4. P2原16B→32B、P4原36B→48B和P3无指针32B保持。现代分配字节/内存压力及失败时机不称与x86逐次相同；GraphicAlloc本次为adapted，三查询只限已证正常路径equivalent。
5. 分配入口已有ClientInitialization真实调用，GetIndex已有CoordLoad及Yobi FrameMove/PlayFrame两处消费；排序传真实Compare。只补四身份与局部连接，不改其完整父或扩大初始化/播放链完成声明。

<a id="animation-capacity-acceptance"></a>
## 限定验收

- 源仅P1分配片段、额外清零和头注释三处；精确重放后其余正文一致，动画查询/生产者/播放/排序/清理源码保持。
- 四原完整186条548B及库存范围/五call一回调身份可追；核原固定预算、发布/失败/四清零阶段和三个正常查询/回调类型，不代完整上游或实际malloc/GDI/SDK。
- 新增三查询语义，既有分配语义追加；四处置及明确成员纳管。语义1260→1263/有效ID1256→1259，处置350→354；RESOURCE25→26、ANIMATION0→4。E196→198只RESOURCE本次I/S局部，不提升完整门禁。
- G01/U11/R01/R04继续open；完整加载/播放/场景/退出、文件/对象/节点和所有alias/owner寿命、真实分配失败/字形/画面/ABI/SDK运行未验，R1/R7/R8及整体SPEC未闭。

<a id="animation-capacity-source-state"></a>
## 当前交付

P1预算恢复20500项且不额外清零一过尾记录，原正常清零和包含上界查询保持；三已有查询获得稳定原身份与源合同。只修此处额外适配，不改原链表实际左右哨兵、其它局部确定化或网络明文规则。

固定命名快照：`cg2d-re-animation-capacity-20261005`。
