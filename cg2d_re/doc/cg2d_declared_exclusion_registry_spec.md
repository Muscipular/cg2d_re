# 已批准范围排除的R1登记 SPEC（SER01）

2026-10-05，补齐已具名全入口排除项与R1处置账之间的缺失登记。
上位：[追踪体系](cg2d_traceability_acceptance_spec.md)、[具名决议](cg2d_closure_decisions_spec.md)。
固定快照：`cg2d-re-declared-exclusion-registry-20261005`。

<a id="declared-exclusion-baseline"></a>
## 当前差集

汇总器已依据三份分类JSON和newest五入口登记113个完整入口排除、2个混合入口。
R1处置账仅有6条excluded，另107条已批准排除却没有处置；其中97条也在定位差集，另10条已有定位关联。
只补这107条：屏幕键盘91、网络401960/401AB0两叶、低色深9、newest5。
原具名分类和授权来自[键盘范围](cg2d_external_keyboard_scope.json)、[网络两叶](cg2d_network_cipher_scope.json)、
[低色深范围](cg2d_render_depth_scope.json)及[总SPEC](../plan/cg2d_full_reconstruction_plan.md)的newest列表；不新增或改写原授权。
[逐ID登记依据](cg2d_declared_exclusion_registry_evidence.json)保留库存ID/入口、原分类记录位置和本次处置。

<a id="declared-exclusion-decision"></a>
## 登记裁决

只选择当前declared_excluded_ids内、非declared_mixed_ids且无既有处置的库存身份。
逐项复核原分类status以“用户范围排除”开头、ID/入口与库存对应；newest项必须在原具名五入口列表。
记录excluded、当前epoch及原分类/具名合同/本组SPEC依据，不覆盖任何原处置/源码/语义记录。
不按名称、地址区间或下游关系扩张集合，不把缺定位当未实现。

保留键盘共享509080/53A200、两个混合渲染入口、普通输入/IME、8位索引资源/调色板，
以及403C50/403E50网络取包/组包/队列/状态和原格式明文职责；音频继续暂停并属于总体范围。
这次107条只收口**排除身份登记**，不算107个实现/验收通过，也不消除定位登记差集。
原函数/导入/区间/叶/任务分母及全部软件门禁保持；R1全量归属、R7未知差集和R8仍open。

<a id="declared-exclusion-acceptance"></a>
## 限定静态验收

核107条身份/范围来源一一对应、91/2/9/5分组、原6条及其他处置完整保留，保护集合/混合入口未被排除。
登记簿只加107条excluded和R01/R04开放依据；源码、语义、原授权/范围/执行政策、软件证据与门禁保持。
生成/Check验证113排除及总处置320/3680；差集184/需核81、源级实现处置与E173不因排除登记提高。
检查新增引用/编码及精确差异，不读取源码正文或重做原IDA分类，不构建、测试、运行或计算摘要。

<a id="declared-exclusion-source-state"></a>
## 本组状态

已批准的113个全入口排除均有R1逐ID处置，原六条保留，本次补107条。
这不是全部3680函数分类，也不是完整功能/原格式互通或运行验收；其他未知及原验收条件继续保留。
