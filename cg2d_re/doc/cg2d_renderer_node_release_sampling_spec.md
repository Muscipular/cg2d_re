# BOW01 渲染器节点释放取样 SPEC

> 2026-10-06；按工程委托先记具体方案与实施边界，由本线程裁决。关联[DX x64合同](cg2d_graphic_dx_cache_x64_spec.md)、[原DX合同](cg2d_graphic_dx_cache.md)、[追踪验收](cg2d_traceability_acceptance_spec.md)、[唯一进度](../plan/cg2d_full_reconstruction_progress.md)和收尾C09。聚焦节点释放生产/消费及其真实根，不重复开发已存在的呈现器/资源池。

<a id="renderer-node-release-baseline"></a>
## 原端、身份和缺口

原F:0004C3C0 / Renderer::Destroy@[44C3C0,44C55C)，单chunk、完整136条412B/两页18call15内跳1ret，无switch；2普通调用点/2父：457A4F的457A40退出父与466B8C的466B80重建父，仅纳盘，不称完整父已验。

44C408是该根内部标签，经44C402流入及44C3D1/44C3DB条件跳入，**不是独立原函数**；内部节点段[44C408,44C436)20条46B、1虚调用/3内跳，18调用点按4 IAT、4已从EBP赋值确定的GlobalHandle寄存器调用及10 vtable+8 Release分列，原操作数5/8和IAT槽不记绝对callee地址。根已有真实SDL/CPU销毁包装及独立节点helper，历史源码定位各根1，前态机器语义/分类/成员/来源文档为空；只登记真正44C3C0原根。

原节点段44C41A只读一次surface，在非空分支44C423释放所保存对象、44C426清同一槽；44C428读动态实际节点数，然后44C42D递增下标/44C42E推进node，44C431用所取样本比较。旧helper对node.surface判空后又读取作为Release参数，计数则在++index后取；本步改成一次保存实际面、原生node推进及计数先取样。正常无介入数据时最终结果一致，不声称已证明像素或资源数量错误；恢复原字段取样与真实对象身份合同。

<a id="renderer-node-release-decision"></a>
## 裁决与实施

真正原根44C3C0，44C408只是内部20条46B节点释放段，不另造原函数。节点段入口S32 actualcount只<=0退出；正数固定起始node，每轮只读一次native surface，非空才同sourceOwner.Release(savedSurface)后清同槽，随后fresh S32 count先读、再++index及++native node，以同样本判是否继续。surface/节点仅已证地址拓宽，count/S32及其余字段保持；不清count、next/image/reuse/W16字段/未知尾、不释放被覆盖的旧面或失败前缀。已有32位SDL/CPU销毁包装正文及正常节点→Temp/AutoMap→设备/接口/调色板→Back/呈现器→renderer owner接线保，主线程边界与已获批DX7原生替换沿现行合同，不借局部恢复PALDATA16/COM等机制。只单节点helper实修、完整原136条412B/18call15内跳1ret及2普通调用点2父只纳盘；4 IAT/4 GlobalHandle寄存器/10 vtable+8调用按类型记，不将偏移当callee地址。所有父/callee/正常别名-owner/重建退出/SDK-机器ABI/像素及完整Destroy运行未验，原异常不专项模拟/保护/故障等价。

只替换GraphicDxCache.cpp既有ReleaseTextureNodeSurfaces_44C408一个完整正文：保存入口count，<=0原早退；正数使用gTextureNodes.data()原生非拥有node，保存当前node->surface，只在非空时释放这一对象并清同一surface槽；fresh currentCount先取，再推进index和node，使用同一count判尾。sizeof/natural布局带动native节点步长，不保留原1C步长或转换为size_t/截地址。

不新增API/helper文件/原函数ID，不把一个内部20条段当另一原根或R7独立区间。正确RendererDestroy包装、SDL主线程及后端承接、Temp/AutoMap、设备/接口/调色板、Back/呈现器/资源owner及其余函数保持；只核局部节点段与包装的实际调用顺序，**未完整验收整个Destroy**。DX7机制按已批准替换，PALDATA16继续不属于当前32位范围，不扩豁免正常节点和owner时序。

不清count/其它节点字节，不遍历原count<=0的失败前缀，不提前清槽或增加保护/回滚/故障回绕；重复/重入/正常别名与所有caller/callee运行仍按原适用门禁推进。网络原格式明文、内存CryptInt保、音频暂停。

<a id="renderer-node-release-acceptance"></a>
## 可度量局部验收

| 项目 | 条件 |
| --- | --- |
| 原覆盖 | 原完整136条412B/两页18call15内跳1ret/两普通点两父仅纳盘；节点20条46B内段属于同根 |
| 开发 | 单helper正文：一次surface样本/仅非空释放清槽、count先取再推进/同样本判尾；整CPP其余和包装保持 |
| 身份/历史 | 新机器映射1409对应既有44C3C0真实源码，冻结定位1/旧行号保；没有F:0004C408或helper独立原函数 |
| 计量 | 语义1409→1410/有效ID1404→1405；关联1522→1523（41.36%→41.39%）；分类529→530 |
| 功能/证据 | RENDER41→42枚举open；仅节点局部I/S两条static，E665→667；RESOURCE41保/runtime=false |
| 完整门禁 | 八open包含C09只进展引用；队列175/R7区间67/3061、60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保持 |

只必要定向文本静态、两JSON完整增量、measure/-Check；未构建/调试/查产物、测试、运行客户端或算法样例、hash。原完整取证不当完整实现验收，两局部static不当两个功能或完整生命周期/运行通过。

<a id="renderer-node-release-source-state"></a>
## 当前状态与后续

本步单节点释放叶实修并落真正Destroy根机器身份，命名快照 `cg2d-re-renderer-node-release-sampling-20261006`。继续按真实重建/退出父、逐面owner和双树生产/消费收口C09及整体SPEC；完整Destroy/应用清理、SDK和R1/R7/R8运行仍未闭。
