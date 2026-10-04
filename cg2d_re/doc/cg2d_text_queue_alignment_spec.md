# 文本队列生产复制阶段与原门禁对齐 SPEC

> TQP01；2026-10-05依工程委托采用。对应T01/T02，沿[文本队列合同](cg2d_text_pending_queue.md)、[ITD01](cg2d_input_text_alignment_spec.md)及RPLAN.24。只改TextPendingQueue.cpp生产端，不改变token/池/字体层、公开接口或消费owner。

<a id="text-queue-producer-baseline"></a>
## 1. 原证据与差异

[只读证据](cg2d_text_queue_alignment_evidence.json)保存四完整原体：DrawText3 79、DrawText2 80、DrawText 81、QueueText 22，共262唯一指令。四入口854直接call点分别15/19/816/4、父数14/12/188/4；这些是原引用库存，不是全部调用方源码/寿命验收。DrawText的816引用分五页完整读取，不使用首次被工具截断的JSON。

| 部分 | 原事实 | 修正 |
|---|---|---|
| 入口失败 | 三绘制入口只在signed W32文本计数>=4096返回−2；QueueText只在signed W16图形计数>=4096返回−2 | 撤销新增负计数、模式、空/长文本、尾token/续尾重读索引及headToken拒绝，保原满队列失败 |
| 字段与复制 | 先写x/y/mode/color/font/value7/marker，再扫描源串；低U32含NUL，逐W32前移后余W8 | 撤销提前限长扫描/逐字节替代，保源身份、读写宽度/次序和实际NUL，不清旧尾或field108 |
| 两signed模式入口 | W32和W8复制完才读尾槽，随后清新next | DrawText2按原恢复；DrawText3保既有真实core复用font0，继承同一原次序，无新业务或影子状态 |
| unsigned模式DrawText | 456821在W32复制后、456827余W8复制前读尾槽；45682B才清next | 两复制阶段之间取尾，不与signed入口合并这一取样 |
| 空链 | 45660E/456721/456837先取字体层，随后写尾槽再调用QueueText | 先保存实际layer再更新尾槽/调用，不把layer读取移到写后 |
| 续链与返回 | 先连接旧尾，再重新取文本计数，用该值更新尾/加一/返回旧值；空链QueueText返回弃用，也重取文本计数 | 保既有真实重读/返回和容量独立性，不增检查或回滚 |
| QueueText | 原W16计数用于初始索引，先写layer后重新取W16写cacheIndex；imageNo−1/value4=0/batch4/文本引用，末再取W16、加一并signed扩展返回 | 去额外拒绝，原字段/写序/重读/非写字段保持 |

DrawText3原独立79条与DrawText2的正常业务差异只有font0；现有包装完整复用真实DrawText2。本步保其实现及公开签名，以参数/字段/复制/分支/返回功能对照登记，不宣称native分解、原栈/机器call轨迹或故障等价。若后续发现可见差异须以原证据另具名处理，不能从单包装缺少原call文本推出缺体或计全父已验。

## 2. 实际数据与生产消费

- 原73ECB8文本W32计数、62ECA0六尾槽、62ECB8文本池、558F50字体层、1020EB8图形W16计数及FF4EB8/101CEB8记录均按operand确认；原字体层/池初始化、现有GraphicCacheItem32 x64承载和一基token保持。
- QueueText原四直接入边是三绘制原点45661C/45672F/456845和ITD01的456C1A。新端DrawText3经真实DrawText2复用；TextPendingQueue两直接调用加InputText一调用承接四原关系，不以词法数量判缺失。
- QueueText只写cache3.layer/cacheIndex及记录imageNo/value4/batchType/reference.textToken；cache3+3、记录坐标/描述符/尾字节不写。正常域无整项清零，文本next外的旧字节保持。
- 消费连接原521723取记录+0C后521727调用456D70，另存14条局部窗口；原间接虚调用operand18H只记未解析，不当绝对callee。当前DrawAllQueued.cpp已有真实case4→RenderTextToSurface，旧“仅声明/未接”已过时；本组不审整个520DD0或字形/画面寿命。
- 当前GraphicHitTest.cpp已有507140与悬停文本索引生产，不凭旧文档把它当缺体；本组不改/验该生产者或45D7A0。
- 全部854上游原点保存用于继续追踪；完整上游/间接alias、排序/图形队列、文本消费/GDI/SDL与初始化退出寿命另行验收。

<a id="text-queue-producer-decision"></a>
## 3. 决定与实施

1. 在TextPendingQueue.cpp撤销仅供额外限制的kTextCapacity，保全局初值/存储、点击查询与DrawText3真实包装。新增同文件两个私有复制阶段helper，分别按低U32的Count>>2与Count&3逐W32/余W8处理；只复制真实原字节，不补保护、默认零、额外NUL或新工具文件。
2. DrawText2/DrawText保原满队列早退，先写原记录字段再strlen低U32+1。signed入口两阶段后取尾；unsigned入口W32后取尾再W8，不共用整段拷贝掩盖阶段。
3. 两实现均在空尾分支先读取实际layer、更新尾槽后真实QueueText；原返回弃用、续尾/计数重新读取与returned S32/写U32保持，字体不新增范围门禁。
4. QueueText只移除负计数及headToken检查，原signed W16容量比较/索引、layer后重读cacheIndex及四字段/末计数返回保持；现有token/owner与接口不改。
5. 四根尚无语义映射，本组各新增唯一映射，处置只四根限定适用路径；DrawText3源码不改而以真实core复用核对，不能制造新实现。I/W/S各一，完整TEXT/RENDER/T01/T02及闭包不提升。
6. 异常对象/索引/Count、越界与溢出依RPLAN.24不专项模拟，不称安全或故障等价；未构建/查产物/新增或执行测试/运行/摘要，单线程、音频暂停和网络原格式明文保持。

<a id="text-queue-producer-acceptance"></a>
## 4. 局部验收

| 门禁 | 标准 |
|---|---|
| I | 原满队列失败/字段先写、低U32含NUL与W32-W8复制、unsigned中间取尾/层先读、计数重读/返回及QueueText非写字段保持；额外拒绝撤销 |
| W | 原854引用仅库存归档；四QueueText原关系经新端三个词法点及真实包装承接，当前case4消费/唯一池/token/owner可追；完整上游与寿命不验 |
| S | 四完整262条、854实际原点、数据operand/消费14窗口及同文件限定差异/旧台账外部保持；机器ABI或异常域不提升 |
| R | 全部上游业务/间接alias、完整排序/消费与输入渲染寿命、SDK/字形和画面运行仍未验 |

T01/T02继续open；后续沿真实消费和寿命推进，不从旧“只有声明”或本组静态结果推总SPEC完成。

<a id="text-queue-producer-source-closure"></a>
## 5. 源码收口

TQP01已落实五同文件限定修改：标准库cstring包含、额外限制常量替换为两个真实复制阶段、QueueText额外拒绝撤销、DrawText2/DrawText原生产顺序恢复。字段先写再strlen含NUL低U32；signed入口复制完成取尾，unsigned入口W32后/W8前取尾。两实现空链layer先读取、尾槽后写/真实QueueText弃返回，原续尾/计数重新取样/returned S32与written U32保持；QueueText只保原S16满队列失败和原字段/W16重读返回。

五补丁外整份TextPendingQueue.cpp与前提交一致；全局池/字体层/hover初值、点击查询、DrawText3真实font0复用及其他源码/头/owner未改。包装只继承真实核心已校正的原正常业务，不称原栈/native分解/机器call轨迹、异常安全或故障等价。

集中静态通过：四完整79/80/81/22共262唯一连续指令、854原call点及各父数，三唯一原callee/四QueueText关系、11数据operand与14消费窗口可追溯。新端两个生产直接点/ITD01一点及DrawText3真实包装承接原四关系，case4已有真实消费；间接operand18H未解析且未当callee。四新语义外1190旧记录及顶层保持，1194条/1191有效ID、18pending/4分类引用不变；旧60证据/103处置、其他功能/任务、执行策略与闭包保持，仅四equivalent/三局部I/W/S使107/63。

固定Git命名快照cg2d-re-text-queue-producer-source-20261005。T01/T02仍open，10/60任务、0/182叶/完整门禁保持；854上游的完整业务/原新参数/间接alias、排序/完整文本消费与输入渲染初始化退出寿命、SDK/字形/画面运行未验。RPLAN.24异常域不专项模拟、不记安全/故障等价，未构建/查产物/新增或执行测试/运行/摘要。


本组13精确文件UTF-8无BOM/LF、623条本地Markdown链接/45条本组证据引用及限定diff --check通过；既有measure生成/-Check退出0仅证明台账结构/派生缓存一致。精确提交保留用户既有六项改动。
