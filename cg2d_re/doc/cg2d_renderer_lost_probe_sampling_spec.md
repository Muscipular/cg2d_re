# BOX01 渲染器失效探测取样 SPEC

> 2026-10-06；按工程委托先记方案/证据与取舍，本线程裁决实施。关联[呈现器合同](cg2d_sdl_presenter.md)、[DX x64合同](cg2d_graphic_dx_cache_x64_spec.md)、[BOW01节点释放](cg2d_renderer_node_release_sampling_spec.md)、[追踪验收](cg2d_traceability_acceptance_spec.md)及[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。本步只失效根的节点循环，不重复工厂/队列/绘制。

<a id="renderer-lost-probe-baseline"></a>
## 原端与差异

原F:000536D0 / Renderer::IsLost@[4536D0,45375C)，完整57条140B/一页、4call10内跳1ret，单chunk无switch；唯一466E2D调用点/父466DC0仅纳盘。四调用分别在4536EE/453714/45372F/45374B，均为vtable+60的IsLost操作，60是偏移不是绝对callee地址。

原入口4536D0先读实际节点数S32，随后清lost累积；<=0跳节点段但仍按Temp→Back→front顺序探测。每节点4536E5单次取surface，非空才虚探测，状态非零只把lost置1、仍继续剩余节点；4536FA先取动态count，4536FF/453700后推进下标和节点，453703用同样本判尾。原没有AutoMap探测，不因某节点失效提前返回。

现有SDL/CPU IsLost已有逐面/Temp/Back/呈现器存活映射；旧for表达式把count读取放在下标递增之后并用size_t索引。改为入口count样本先于lost=0，正数按原生node指针遍历，每项后先读取count再推进，沿同样本判尾。正常无介入状态时结果相同，不称已有画面/失效结果错误；恢复原取样合同及实际对象身份。历史定位1、前态机器语义/分类/成员/来源文档为空，新增真实根映射，不新增原函数或隐藏owner。

<a id="renderer-lost-probe-decision"></a>
## 裁决与实施

原节点actualcount S32入口样本先于lost=0，非正仍走全部Temp/Back/front尾。正数以固定实际node起址遍历，单次surface样本/仅非空才探测，非零状态置lost=1仍继续其余节点；每项后fresh S32 count先取，后++index/++native node，同样本判尾。结束后Temp/Back/front各按原时点取对象/非空探测，末front非零返回1，否则返回累积lost；现行SDL/CPU已批准存活/呈现器映射及尾正文保，不恢复DX HRESULT/COM/vtable机制，不把CPU锁定当lost，不新增AutoMap探测或早停。只一正文循环阶段实修，S32计数/0或1累积及native非拥有地址、唯一节点/面owner保持；不写count、节点surface/image/next/reuse/尺寸/未知尾，不释放或重建。完整原57条140B/一页4 vtable+60调用10内跳1ret，唯一466E2D/父466DC0只纳盘，操作数60不当绝对callee；所有父/callee/正常别名/SDL模式/重建-逐面owner/机器ABI-像素运行未验，异常不专项处理或称故障等价。

只替换src/render/SdlPresenter.cpp一个完整IsLost正文，其改动限节点循环；Temp起的全部尾正文逐字保持，整CPP其余正文（包括BOW01保持的Destroy包装）不改。节点起址用gTextureNodes.data()，S32 count/index保持，实际node通过native指针推进，不截地址、不保留原1C stride。每轮surface只读取一次，不写/释放节点或调色板引用。

CPU面是否存活、Temp/Back和呈现器探测沿已批准SDL承接，不以锁定当lost；原HRESULT/COM机制按DX7批准豁免，不扩大到正常状态顺序和生命周期。不新增AutoMap探测、负计数/容量拒绝、补零、钳制、回滚/回绕或异常模型，不运行异常样例。实际重建/模式切换、正常别名、父/callee与像素仍需适用运行验收。

本次追踪也确认两类尺寸不混同：采样分母+B8/+BA是S16，factory描述符+20/+1C是W32；现有LegacyTextureState类型及工厂正文保持。这个取证结论不是工厂完整验收，本步不新增影子描述符存储、扩大字段或改初始化采样分母。

<a id="renderer-lost-probe-acceptance"></a>
## 可度量局部验收

| 项目 | 条件 |
| --- | --- |
| 原覆盖 | 全57条140B/一页4 vtable+60调用10内跳1ret、唯一点/父只纳盘 |
| 开发 | 单IsLost正文，count入口/节点尾先取再推进；累积lost不短路，全部既有尾及其它源码保持 |
| 身份/历史 | 新1410映射对已有真实根，冻结定位1/旧行号保，不新增helper原ID |
| 计量 | 语义1410→1411/有效ID1405→1406；关联1523→1524（41.39%→41.41%）；分类530→531 |
| 功能/证据 | RENDER42→43枚举open；两条根内局部static I/S E667→669，RESOURCE41/runtime=false保 |
| 完整门禁 | 八open包含C09仅进展引用；队列175/R7区间67/3061、60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false保 |

只必要定向文本静态、两JSON完整增量和measure/-Check。未构建/调试/查产物、测试、运行客户端或算法、hash；没有完整SDK/机器ABI、模式画面或整个失效—重建生命周期通过。网络原格式仅明文/内存CryptInt保/音频暂停，整体范围不缩。

<a id="renderer-lost-probe-source-state"></a>
## 当前状态与后续

本步落实原节点失效循环的count取样及实际node推进，并登记真根1410；命名快照 `cg2d-re-renderer-lost-probe-sampling-20261006`。继续沿唯一重建父及逐面owner、双树-C09/正常别名/场景和R1/R7/R8收口，运行门禁保持。
