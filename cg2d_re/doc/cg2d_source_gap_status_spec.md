# 源码缺口标记校正 SPEC（HSC01）

2026-10-05，解决已接线/已有正文仍被头文件标成缺口的问题。
上位：[追踪体系](cg2d_traceability_acceptance_spec.md)、[收尾任务](../plan/cg2d_spec_closure_checklist.md)。
固定快照：`cg2d-re-source-gap-status-20261005`。

<a id="source-gap-status-baseline"></a>
## 当前事实

- LegacyRecordOrder.hpp仍称50EC50网络派发待接；NetworkRuntime.cpp已有yuD真实调用，原直接入向为513F93/512E40。
- YobiRuntime.hpp仍称43F520/440070回调、442F70尾未实现；当前YobiRuntime.cpp已有三者定义，前两者也已绑定到43F330/43FEB0，实体尾有实际442F70调用。
- WorldUpdateDependencies.hpp仍称4631D0的FDA6F8写者待接；当前创建确认已写同源S32字段，5199A0已有消费。
  原463B91为W32写0，519A3C为W32读；四条指向FDA6F8的比较是地址末端界限，不当作该值的读写。

[原引用与当前源码依据](cg2d_source_gap_status_evidence.json)限定以上四条注释，不把已有正文等同全业务/生命周期或运行通过。

<a id="source-gap-status-decision"></a>
## 实施裁决

纠正三个头文件共四条注释：列已存在的源码/调用，并保留完整父/数据/alias和运行验收边界。
不改C++声明、布局、函数体、分派、数据或原宽；不重写已存在的生产/回调，不新增虚假功能。
既有4631D0语义记录追加当前FDA6F8生产/消费事实和P08显式gate已落位的现行依据，保留历史证据文字。
R04/U09/U10/U11/V03只追加当前依据并保持open；既有函数/语义数量、处置、叶/任务及软件门禁保持。
网络仍按原yuD格式明文，音频暂停；未决原数据/旧表/全部上游与SDK/ABI/运行不由这次标记校正关闭。

<a id="source-gap-status-acceptance"></a>
## 限定静态验收

四注释精确重放/其余头文本保持，网络调用、两回调绑定/三定义/实体尾调用、FDA6F8声明/写入/消费与原W32一致。
一既有语义记录和开放引用精确增量、追加文档/任务同步，检查新增引用/编码及生成/Check。
不构建、测试、运行或计算摘要，不全面复检原dispatcher、创建页、Yobi正文或历史合同；无功能完成率提升。

<a id="source-gap-status-source-state"></a>
## 本组状态

四个过时缺口标记已按当前接线/正文纠正，FDA6F8的已证W32写者/消费者及当前P08依据可追。
本步只修源码与台账状态不一致，没有新增业务实现或通过完整功能/SDK/运行验收。
