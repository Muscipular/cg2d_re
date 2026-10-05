# MCL01 观察节点追加/摘链读取点与已有清头身份

<a id="map-chara-link-baseline"></a>
## 实际缺口及原范围

[观察链查找/清理MQC01](cg2d_map_chara_lookup_spec.md)已修next提前保存；其原生头/载荷的追加与摘链在[Yobi合同](cg2d_yobi_runtime.md)已有本体。当前43F180判断next后在前进时重读；原只取一次next。当前43F1B0命中后沿用比较缓存next取被摘载荷；原再次读取前驱payload.next。本步只修这两个局部读取点，不重做生产者/通用分配或头owner。

[原证据](cg2d_map_chara_link_evidence.json)保存三体39条110B、九直接call/七父：43F180 15条41B、43F1B0 22条58B、43F220 2条11B。引用分别为三生产者/两回调/World Update与Draw的四调用；保存引用不代七完整父或全producer。

- **追加**：43F180读头，空链发布实参action并返回旧零；非空每节点43F189取载荷，43F18C一次读next，非零沿该指针继续，零时43F19A写实参到当时尾载荷next、返回实参。不拒绝空实参，不预清其next。43F195的物理retn保存在原体，不据其不可达正常路径造新返回分支。
- **摘链**：头等于action时读取该action载荷/next、只改头并返回被摘载荷；否则每轮先取当前载荷及next，再比较next与action。命中后43F1DD再读前驱载荷next、43F1E0读取该节点载荷、43F1E3读被摘载荷next、43F1E6重接前驱next，返回被摘载荷。遍历不命中返回0；原不清被摘next或释放action/payload。
- **已有清头**：43F220仅清同一5F9750头、不遍历/标死/释放节点，当前本体不修改。空指针或链异常不加保护/修补/模拟。
- MapCharaPayload仍是既有五W32/五原生孩子指针/S32 frame/原生next及0x50承载；非拥有链指针和最终payload释放仍沿原Yobi runtime，不新建链/影子节点或公开载荷。

<a id="map-chara-link-decision"></a>
## 委托裁决与源码

1. 仅YobiRuntime.cpp两模式/两块：43F180取载荷后保存YobiAction* next，用同一次读取值判断空尾或前进；原空头/发布/尾写/参数返回保持。
2. 43F1B0在比较缓存next命中之后再次保存当前payload.next为removedAction，沿它读取被摘载荷和其next再接前驱。保持头命中分支/未命中0、比较点和返回载荷地址；不改成读取实参action的载荷或提前快照所有链接。
3. 43F220已有清头本体及43F1F0/43F230全部当前正文保持，既有Opaque payload返回接口不改，不据本次摘链结果扩展通用action类型。三已有原身份/真实来源及adapted纳管，其中两个实修、一个清头未改，分别关联MAP/LOOP传递依赖但同一ID只计一次。
4. 三生产者、两回调、世界父调度/生命周期、NewMapCharaAction与通用创建/最终释放、数组与非地址宽度、公共声明及其它源/头/汇编保持。全producer/七父、间接alias和地址逃逸/所有owner重进退出/borrow失效/实际ABI/SDK/时钟RNG/场景画面运行未闭。
5. 依RPLAN.24，不增NULL/head==action保护、修链、拒绝、钳制、补零、回绕或故障模拟/称异常安全等价。不为新增空或自循环输入调整原链行为，网络原格式明文和音频暂停保持。

<a id="map-chara-link-acceptance"></a>
## 限定验收

- 当前全文只HEAD加两模式/两块，追加一次next取样/条件尾写与摘链命中后第二取样/重接/返回可核；43F220及MQC01查询/清理全文不改，五值/五孩子/frame/原生next与头owner保持。
- 三原39条110B/库存/九call七父，原空头/尾字段/摘链双读/只清头点和三个实际本体/声明可追；不代七完整父/全producer或异常/机器ABI运行。
- 1282→1285语义/1278→1281有效ID、375→378处置；MAP38→41、LOOP3→6成员，E217→221仅两域各I/S，四记录共用两源实修/三原证据，不计清头新增或双份函数。
- G01/G02/U11/R01/R04/V04继续open；所有B/D/W/R/完整功能、R1/R7/R8/60项关闭与整体SPEC不提升。不编译、调试、测试、运行样例/客户端、检查产物或计算摘要。

<a id="map-chara-link-source-state"></a>
## 当前交付

节点追加和摘链恢复原next读取/复读取样，已有只清头入口补原身份；原生唯一链、发布/重接/返回与最终释放职责保持。局部代码/身份静态不代表整条观察链/场景或所有owner寿命完成。

固定命名快照：`cg2d-re-map-chara-link-20261005`。
