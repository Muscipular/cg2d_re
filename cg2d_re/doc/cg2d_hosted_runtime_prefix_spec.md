# 宿主兼容堆与低层IO前置 SPEC（HPI01）

> 2026-10-09当前FILE边界：[MC01-B](cg2d_crt_host_migration_spec.md#msvc-crt-file-source-state-20261009)已将全部现FILE/描述符/流格式与状态/位置/关闭改真实MSVC服务，旧表/缓冲/私有入口及C表退出当前链；下文旧内部布局/发布机制只属固定历史。业务字段/字节、真实结果/owner与全部适用门禁保持，未编译/运行；完整C++余组继续。

2026-10-05，按用户“聚焦缺口”推进；未决工程事项沿既有委托裁决。
上位：[启动生命周期](cg2d_crt_startup_lifecycle_spec.md)、[模式3承接](cg2d_mode3_heap_facade_spec.md)。
固定快照：`cg2d-re-hosted-prefix-io-source-20261005`。

<a id="hosted-prefix-baseline"></a>
## 已发现缺口与原基线

当前宿主路径不启用旧RunLegacyProcessEntry_53DDB5。已有兼容堆初始化和描述符生产
只被该旧入口调用，宿主应用初始化前仍是空堆句柄、模式0和空描述符表。
这使旧分配路径缺少真实堆，描述符懒扩容也不能代替标准0/1/2及继承句柄生产。

[原证据](cg2d_hosted_runtime_prefix_evidence.json)归档四已有根：
原入口78条/246B、堆30条/93B、描述符143条/427B、stdio收尾5条/20B，
合256条/786B；仅作为本步接线基线与父背景，不重验四本体或其全部callee。
原53DE0F heap(0)，失败经53DE1B fast_error_exit(28)；
成功后53DE24 ioinit，返回不消费，53DE48 cinit后才进入WinMain。
原553188登记53EA4A：先全流刷新，再读callable字节，非零才关闭全部流。

<a id="hosted-prefix-decision"></a>
## 实施裁决

在LegacyMouse既有同TU首个LegacyMultibyteStaticInitializer构造中依次调用：

1. InitializeLegacyHeap_541BB8(0)；返回0时FastLegacyRuntimeErrorExit_53DED0(28)。
2. InitializeLegacyDescriptors_53F52D()；保持原返回不消费。
3. std::atexit登记本组普通退出适配FinishHostedLegacyStdio。
4. 原InitializeLegacyMultibyte_546CAE()，随后继续既有应用投影。

普通退出适配先发布既有callable槽=0，再调用原FinishLegacyStdio_53EA4A。
登记早于该TU应用/标准流适配的退出登记，使其后登记回调按逆序先执行，
stdio随后刷新。登记返回忽略，不因失败短路后续C表投影/应用项，不回滚。
这承接原固定C-term阶段的普通stdio职责；时点/登记失败机制为D02/D03适配，
不宣称原SEH、快速/callable退出或SDK失败行为等价。

直接复用已有provider及共享域；不硬置堆模式、不改选择器/ENV或模式1/2。
模式3沿现有SDK五操作合同，旧兼容元数据独立；模式稳定/全部alias仍待。
stdio沿现有FILE与低层表消费，不新建SDK FILE副本、额外CloseHandle、
描述符清表或HeapDestroy回调。兼容堆/描述符存储保进程寿命；
callable阶段不凭空销毁，最终OS资源释放不当作所有owner已验。

仅该TU的既有应用序前置，不宣称所有跨TU动态初始化已定序。
不启用旧自定义入口、整套cinit/90表或旧argv/envp，不新增SDK标准流构造。
原70源投影＋15宿主角色对应85项计数不变；本组不是新增C++表槽。
WinMain、四provider及后续应用/SDK配置与pubSync正文保持。

<a id="hosted-prefix-acceptance"></a>
## 限定验收与剩余

- 一CPP三片段：已有include段、普通退出适配与首构造前置；其它源码保持。
- 精确核heap(0)→失败28→io返回忽略→即时登记→原multibyte顺序及普通退出槽/刷新。
- 四已有原身份补接线记录；原本体不改，不新增函数处置或提升完整功能门禁。
- 台账/唯一进度及新引用静态检查，汇总器生成并独立Check。
- 未构建、测试、运行或计算摘要；完整成员、跨TU首次使用、SDK/继承ABI、
  模式稳定/同域owner与未知alias、console/filter及normal/quick/callable全序仍待。
- C02/C04/C08/C10/C11/V02保持open；R1/R7和最终验收仍待。

<a id="hosted-prefix-source-closure"></a>
## 源码小步收口

按以上裁决接通兼容堆与低层描述符生产，并登记普通退出流刷新。
四原根只补本步映射，原1226记录/1223有效原ID总量、195/3680函数处置及15/218导入不变；
限定E147→150。任务关闭10/60、已验收叶0/182及完整门禁不提升。
本节记录源码小步，不宣称完整启动退出验收通过。

限定检查通过：一CPP三片段与四provider/WinMain未变；四语义补充及登记簿精确转换，
11项UTF-8无BOM/LF、10个新增Markdown链接、33个本组E引用与Git差异格式均通过。
看板已生成，独立Check通过；不扫描历史Markdown，也不提升完整门禁。
