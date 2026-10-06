# BOU01 地图批次与着色图形入队生产 SPEC

> 2026-10-06；按用户工程委托先记原证据、取舍及方案，由本线程裁决实施。关联[DX x64合同](cg2d_graphic_dx_cache_x64_spec.md)、[BOS01描述符消费](cg2d_graphic_enqueue_descriptor_production_spec.md)、[BOT01其它入队](cg2d_graphic_secondary_enqueue_production_spec.md)、[追踪验收](cg2d_traceability_acceptance_spec.md)与[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。本步沿真实生产消费补缺，不重验已过的排序、总绘制及缓存叶。

<a id="map-colored-enqueue-production-baseline"></a>
## 原端基线与具体差异

| 原ID / 入口及半开区间 | 当前真实源码 | 完整原体 / 引用 |
| --- | --- | --- |
| F:00121AA0 / [521AA0,521B0F) | src/render/GraphicPendingQueue.cpp / QueueMap_521AA0 | 26条111B/1页，0call/1内跳/2ret；2普通调用点，1已知父和1无所属函数点 |
| F:0005F090 / [45F090,45F0FA) | src/DrawGraphic.cpp / Sub_45F090 | 32条106B/1页，1call/0内跳/1ret；53普通调用点/8已知父 |

共58条217B/两页、1call/1内跳/3ret；均单chunk，无switch。55调用点仅纳盘，不记55个父或55个函数。521AA0的495E47调用点在当前原库没有所属函数，保留parent=null，不猜补父或另造原函数ID；另一父5029B0。45F090仅一条原call到520970；对应真实源码八参数调用，不将53上游点当调用完整验收。

两根已有源码及历史定位各1，但前态机器语义/分类/成员/来源文档均空。地图入口使用size_t索引并直接写坐标，本步恢复原生实际目标与原x/y取样阶段。着色包装使用私有GraphicModeFields整块零值投影后bit_cast，最终16B正常值与原端一致；本步恢复原W32/W16/U8重复、重叠写入的栈生产合同，**不声称已有颜色数值错误**。原证据保存旧两个正文、私有投影、全部原指令、调用点及历史冻结定位。

<a id="map-colored-enqueue-production-decision"></a>
## 具名裁决与实施

### 地图批次521AA0

入口S16 count仅原>=4096返S32-2，正常域保存实际record/cache3原生目标；layer W8先pub，再live count低W16 pubcacheIndex；先x参数样本再y参数样本，随后x/y S32、图号-1、value4 W32=0、action native0、batch W8=5；其余descriptor/未知字段/特殊旗不写。末fresh U16 count返回旧S16→S32，低W16+1 pub，不加负拒绝或改容量门槛；正常数据生产直接进入既有排序及case5地图消费。

只在原容量允许的正常路径形成原生非拥有record/cache3目标，按新48B主记录及4B排序槽承载；异常域不专项处理，不恢复unsigned索引保护。描述符16B、记录未知尾和特殊旗原本不写，保留旧值。记录直接进入既有SortTextures与DrawAllQueued case5/Sub_46C760真实消费，不更改排序/地图业务。

### 着色包装45F090

私有16B栈descriptor不用整块清零/投影快照；零W32先取color S32、写+2/取实际地址/写+6，再layer W8样本/写+10；零W16 +14、+0/+2/+4/+6，clip U8 +12，重复W16 +14；value4 S32样本后pubcolor W32 +8；原图号/y/x S32样本、末mix U8 +13=4，借实际栈descriptor给520970八实参(x,y,layer,imageNo,value4,1,0,address)，透传S32返回，不修改图号门槛/Draw3已证helper或复用寿命。原最终16B与旧源码正常一致，只恢复具名原发布合同，不声称颜色错误。

使用当前GraphicDescriptor16的实际16B栈对象及memcpy避免未对齐类型解引用。依原序写入，不新增通用工具/独立源码或public类型。声明未整体值初始化；原所有16B在调用前均已写入，末mix=4之后直接同步借用给DrawGraphic3，后者已有逐W32消费，不存入悬空栈引用。八实参中isGraphicNo=1/batch=0保持，不新加入口图号/指针/结果门槛。退休仅服务此入口的私有GraphicModeFields及其两项未用标准头，不改原公开头、ABI合同或其它根。

共两个正文及一个私有投影块、两个未用include，分布两个既有CPP。整文件重放这五处补丁证明其它正文（BOS/BOT四入队根、队列排序/比较/owner）保持；公开头和消费入口保持。正常S32/W32、W16/U8及native地址分工保持，不新增负拒绝、保护、钳制、回绕/故障模型；网络原格式明文、内存CryptInt保留、音频暂停。

<a id="map-colored-enqueue-production-acceptance"></a>
## 可度量局部验收

| 项目 | 本步条件 |
| --- | --- |
| 原覆盖 | 完整58条217B/两页1call1内跳3ret；55普通点分根，原parent=null保 |
| 实现 | 两CPP精确五补丁；地图目标/坐标取样、着色栈重复及重叠发布/8实参-S32返回 |
| 身份 | 两机器映射索引1407/1408对应已有真实源码，历史定位各1不刷新 |
| 计量 | 语义1407→1409/有效ID1402→1404；关联1520→1522（41.30%→41.36%）；分类526→528 |
| 功能/局部证据 | RENDER38→40枚举open；两根独立I/S四条static，E659→663；RESOURCE41保持 |
| 完整门禁 | 七open只更新进展引用；60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false及其它门禁保 |

只做必要定向文本静态、两JSON完整增量和measure/-Check。未构建/调试/查产物、不新增执行测试、不运行客户端或算法样例、不计算hash；局部static不代SDK/机器ABI、全部caller/正常别名、地图绘制/像素/owner运行。

<a id="map-colored-enqueue-production-source-state"></a>
## 当前状态与后续

本步落实地图批次实际槽与着色栈生产、真实已实现消费者接线及两新机器身份。命名快照 `cg2d-re-map-colored-enqueue-production-20261006`。沿真实场景生产、资源生命周期及剩余差集继续开发；完整验收及R1/R7/R8保持未闭，不将未归属调用点猜补为已闭父。
