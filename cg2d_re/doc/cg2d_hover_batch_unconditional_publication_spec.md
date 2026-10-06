# BOV01 描边批次无条件发布 SPEC

> 2026-10-06；按工程委托先记原证据/具体偏差及取舍，由本线程裁决实施。关联[DX x64合同](cg2d_graphic_dx_cache_x64_spec.md)、[原DX合同](cg2d_graphic_dx_cache.md)、[BOS01普通图形入队](cg2d_graphic_enqueue_descriptor_production_spec.md)、[追踪验收](cg2d_traceability_acceptance_spec.md)和[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。不全面复核已通过的文字生产、排序或总绘制。

<a id="hover-batch-publication-baseline"></a>
## 原端和实际偏差

原F:00122180 / [522180,52220B)，完整41条139B/1页、1call/6内跳/1ret，单chunk无switch；195普通调用点/115已知父仅纳盘，不记195父或195个函数。原唯一call5221F7到521B10；5221FC按返回EAX计算五倍，522202无条件把原40B槽+24的batch写U8=2，未判索引范围。

现有DrawGraphicHover完整包装已做四低W16坐标打包、step0/1/2图号标记和六实参调用，正常颜色/坐标值本步不改。随后真实调用唯一私有批次helper；helper为index==-2保原prefix+5C的原失败写，却额外用index<0或index>=4096拒绝其它写入，且用size_t索引。旧语义249的文字合同已写“不新增入队失败保护或跳过写入”，与这个helper实际额外拒绝不一致，本步直接解决。

旧249只有一来源/两文档，未登记分类或RENDER成员；历史快照无直接定位，保持0，不用当前文件存在刷新旧冻结行。原证据保存完整原函数/全部普通入向、旧包装/旧helper及整份原249映射档案，不覆盖历史。

<a id="hover-batch-publication-decision"></a>
## 裁决和实施

原四个低W16坐标打包两个W32；step S32依0/1/2分别OR图号W32 A0000000/90000000/C0000000，其余图号不变。六实参call521B10(x/y S32位型,layer U8,图号S32位型,value4=0,descriptor=null)，之后无条件写原返回索引对应batch U8=2，不加索引范围拒绝或跳过。helper按native实际record地址承载；原满队列返回-2对应prefix+5C写沿已证合同保，不套48B负索引偏移。原W32图号/低W16坐标/W8batch/S32索引与唯一owner保持，正常/原失败写序不改；异常域不专项模拟或计故障等价。描边包装正确正文/原六实参已在，只helper一正文实修；既有void适配保，不声明原EAX×5残值/机器ABI或195点115父完整运行已验。

只替换src/render/GraphicPendingQueue.cpp既有SetPendingGraphicBatchTypeAtLegacyIndex一个完整正文。保留index==-2专门的prefix+0x5C写及return，这是已证原满队列失败之后的写位置，不新建异常补偿或安全分支；撤掉额外通用索引拒绝，余路径取records.data()+S32 index的native非拥有实际record，再写U8 batch。不截地址到W32，不扩大非地址index/type或重排48B项。

只有DrawGraphicHover一个源码调用点使用本helper；公开声明、DrawGraphicHover包装正文、BOS/BOT/BOU入队根及描述符helper、排序/比较、总绘制、队列owner和其它字段保持。沿实际DrawGraphic→batch2→总绘制case2/Sub_522210链，不新增入队函数、API、独立模块或原函数ID。

不称原异常输入安全或故障等价，不构造负索引/越界/溢出样例；RPLAN.24禁止异常专项策略及新增保护，原容量门槛和原普通失败写保持。原x86 EAX最后是五倍索引残值；当前void适配及其公开调用合同保，不以此局部静态声明机器ABI、全部195点/115父和正常别名/资源寿命/像素运行已验。网络原格式只明文、内存CryptInt保、音频暂停。

<a id="hover-batch-publication-acceptance"></a>
## 可度量局部验收

| 项目 | 条件 |
| --- | --- |
| 原覆盖 | 完整41条139B/一页1call6内跳1ret、195普通点/115父仅纳盘 |
| 开发 | 一个helper正文；额外索引拒绝/size_t索引退休，-2前缀原写保，整CPP其余保持 |
| 历史与身份 | 旧249续记/完整旧record档案保，源码冻结定位0/旧两文档事实保；不新增语义ID |
| 计量 | 语义1409/有效ID1404、关联1522/3680（41.36%）均保；分类528→529 |
| 功能/证据 | RENDER40→41枚举open；两独立局部static I/S，E663→665；RESOURCE41保持/runtime=false |
| 完整门禁 | 七open仅进展引用；队列175/R7区间67/3061、60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false及其它门禁保 |

只必要定向文本静态、两JSON完整增量、measure/-Check，不构建/调试/查产物、测试、运行客户端或算法样例、hash。局部两证据不是两功能或完整运行验收。

<a id="hover-batch-publication-source-state"></a>
## 当前状态和后续

本步落实描边批次原无条件发布与旧249具体源码范围，命名快照 `cg2d-re-hover-batch-unconditional-publication-20261006`。沿真实生产、case2填充/画面及共享owner继续推进整体SPEC，不缩运行门禁；无独立复核或subAgent。
