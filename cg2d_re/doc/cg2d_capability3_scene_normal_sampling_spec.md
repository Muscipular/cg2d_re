# 能力3场景正常运算与透明度取样SPEC（BSF01）

本项沿能力3登录主流程，依工程委托先裁决再实施。保持[场景原合同](cg2d_capability3_scene.md)、[最新决议](cg2d_closure_decisions_spec.md)、[验收体系](cg2d_traceability_acceptance_spec.md)和原格式明文网络，不扩展场景业务。

<a id="capability3-scene-normal-baseline"></a>
## 原基线与实修缺口

45F190／F:0005F190原根494条1965B、八页只读取证。49原调用为48直接／19唯一目标和1个PostMessageA间接；43跳转（含switch）／一RET，唯一World::Update 51C430@51C4A1。原指令、字节、边界与引用见[机器证据](cg2d_capability3_scene_normal_sampling_evidence.json)。

现源私有AddW32／SubW32／ShiftLeftW32把普通S32加减与相位移位改成uint32／bit_cast专项模型；状态3复制透明度先读取左对象字节后才采样右指针。原45F554→45F55A→45F560→45F566为先采左／右owner，再读左+30F、写右+30F。正常数值已经一致，本步恢复源码运算与取样先后，不宣称观察到运行故障。

邻接X:0005F93D-0005F970共51B，必须按真实内容分开：
- 45F93D..45F940：3B `8D 49 00`，LEA ECX,[ECX+0]身份对齐指令，不称为三个NOP；
- 45F940..45F96C：44B／十一W32状态目标，45F1AE单一原父数据引用；
- 45F96C..45F970：4B逐BYTE canonical nop。

全区无代码入向，前RET后45F970真实入口。仅登记对齐／跳表用途，不新增原函数、排除或运行验收。

<a id="capability3-scene-normal-decision"></a>
## 工程裁决

只改src/LoginCapability3Scene.cpp四个精确片段：

1. AddW32恢复left+right、SubW32恢复left-right、ShiftLeftW32恢复value<<count；类型／noexcept和全部调用保持。
2. 状态3完成左对象高BYTE写入后，依次持有本次左／右YobiAction*，读左透明度W8再交给同一右对象访问器。两个指针仅调用期非拥有快照，不新增owner或延长寿命。

ComposeWhiteAlpha中的高BYTE拼接、32bpp灰度位型、unsigned状态分发和低位转换是实际格式，保留。两处透明度访问器原已有正文保持；它们只取高BYTE／更新高BYTE并保低24位，不调用RNG。

保持状态0..10／默认分发、公共状态与鼠标低位取样、初值／阈值／原正常除40和47、两次NewYobiAction及不同失败路径、原FrameMove／MarkDeath与观察者清理、焦点／持久槽／负数返回与提示原字节、尾部四独立alpha和最后返回。声音调用保持且音频继续暂停；既有固定32bpp／屏幕键盘排除与网络裁决不变。

按RPLAN.24不专项模拟数学溢出、回绕或异常，不添加钳制、补零、保护、容量限制或新业务。原已有255阈值与钳制照原保留，不属于新增保护。

<a id="capability3-scene-normal-acceptance"></a>
## 度量与验收

| 项目 | 本步通过条件 |
|---|---|
| 源码 | 单整源仅四授权替换，三个私有工具／一个原场景体变更；其它正文与真实位型保持 |
| 取样 | 状态3复制阶段左owner→右owner→左W8→右W8，非拥有T*与原高BYTE边界 |
| 原证据 | 494条1965B／八页完整连续，48直接和1间接调用及唯一父、原正常算式／复制点可追 |
| 区间 | 51B分三实际部分、十一目标对应状态0..10／状态6公共分发，不误作NOP或F |
| 追踪 | 单真实F与LOGIN成员、单X用途、两局部I-S、U10／R04current/引用及唯一进度同步 |
| 静态 | 集中源码／证据／完整机器／文档增量、measure及-Check、完整派生快照／UTF-8-LF及实际引用通过 |

局部I/S不替代全部父与依赖、正常alias-owner寿命、图像／输入消费、SDK-机器ABI／链接和运行。现限制下不构建、测试或运行，不降低完整验收标准。

<a id="capability3-scene-normal-source-state"></a>
## 当前追踪状态

新增映射1588／adapted F:0005F190；私有辅助符号仅来源，不造F。LOGIN5→6，语义1589／有效1582、文档1700／3680（46.20%）、处置722／E832，R7 294余2767。U10／R04只current／引用仍open；冻结定位／pending18／缺口队列、源码closed11（P05原closed）／完整叶0／globalfalse与所有完整门禁保持。

实施命名快照：`cg2d-re-capability3-scene-normal-sampling-20261007`。完成本步后追加实际静态结果。

本步静态结果：单整源四授权片段／三普通S32工具与两owner先取齐、高BYTE和真实颜色位型／访问器及其它正文保持，完整原根494条1965B／唯一父、51B三部分和十一目标用途、完整机器／文档增量均通过；measure及-Check完成，完整派生快照／12文件UTF-8-LF和两I-S各17实际来源通过。未构建、测试或运行，不提升完整父／依赖、寿命、机器ABI或整体SPEC验收。
