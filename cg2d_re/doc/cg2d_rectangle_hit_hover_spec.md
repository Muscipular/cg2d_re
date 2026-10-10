# 矩形命中与可选描边 SPEC（RHL01）

2026-10-10。沿既有输入/渲染功能推进，依子AGENTS第8条先记录事实及工程裁决。同一cg2d_re分支、不打tag。

<a id="rectangle-hit-hover-baseline"></a>
## 原根与真实消费

5049A0–504A25（尾不含）完整55条133B/一页，五实际E8、五跳两RET；135处直接CALL/102个父仅核调用点字节和归属，不代完整父。实际比较依次为401DB0(mouseX,right)、401FC0(left,mouseX)、401DB0(mouseY,bottom)、401FC0(top,mouseY)，四次均TEST AL，失败立即返回S32 0。

401FC0–401FD1完整五条17B/一页：经实际401EA0单读右CryptInt，再有符号比较lhs<=rhs，以SETLE覆盖AL。其16处直接CALL/三父均仅消费AL，现bool实现适用这些消费者；原EAX高24位不记等价。现Read_401EC0承接401EA0，名称不代原字节地址。

旧Less_401DB0与DrawGraphicHover_522180已有完整合同，悬停父507140复用既有证据。本步不重采、登记或改动这些正确本体，不将已存799条父摘要当本轮完整验收。

<a id="rectangle-hit-hover-decision"></a>
## 工程裁决

只将LegacyHitTest.cpp现有四个getter比较接回既有Less_401DB0/IntLessEqualCrypt_401FC0，直接使用唯一gLegacyMouseState中的mouseX/Y，加两个既有项目头。四次短路、每次单读及CryptInt计数/RNG保持。现getter路径正常值和副作用相同，本步补实际接线，不宣称已存在坐标值错误。比较叶body正确，保持；两定义及两真实声明各补一合同注释。

命中条件为left<=mouseX<right、top<=mouseY<bottom；各比较独立读同一对象，不能缓存坐标或合并读取。layer为S32，负值仍返回1而不描边；非负才读取完整W32 hover颜色并调用既有绘框。原实参left/top/right/bottom/layer由既有边界消费低W16坐标和低U8层，step=0；绘框返回丢弃，随后返回S32 1。保留原无矩形规整或尺寸门禁的行为。

只四整源、一组四调用、两个已有头和四注释；无新API/helper、类型、字段、布局、状态副本、owner或分配释放。无新null/尺寸/边界/容量保护、默认值、补零、钳制、回滚或异常/溢出模型；网络/文本、MSVC CRT及WAV-only边界保持。

<a id="rectangle-hit-hover-acceptance"></a>
## 限定静态验收

核两原根连续60条150B、六实际E8/四目标、五跳三RET，135矩形CALL与16比较CALL字节及父、四根AL短路和16叶消费者AL。核四完整源精确差异，比较叶正确body、原描边/返回及其余全文保持；仅必要文本检查和静态汇总器。

两新F/两adapted处置，INPUT增加两成员、RENDER/UIRUNTIME各一成员，三限定S零I，十任务仍open。48有界代码及所有完整枚举/gates、六closure/总SPEC§10保持；文档关联率不当实现率或完成率。

不构建、测试、运行客户端/算法、检查产物、计算摘要或引依赖。完整135父/间接路由、全部坐标/hover生产消费、队列和alias-owner寿命、SDK/机器ABI及实际描边/像素与运行仍待；不以本步替代完整输入、渲染、窗口或生命周期验收。

<a id="rectangle-hit-hover-source-state"></a>
## 当前状态

前提交“[#] 补齐窗口拖动焦点并恢复取样宽度”；拟提交“[#] 补齐矩形命中与描边比较接线”。源码接线、登记、静态汇总/-Check终态退出0及派生范围核对通过。语义2269=2221F关联+48有界代码、处置1711/证据1513、INPUT88/RENDER86/UIRUNTIME125；文档关联2331/3680=63.34%。184叶验收0、60任务关闭11开放49、六closure和完整门禁保持，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_rectangle_hit_hover_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐矩形命中与描边比较接线」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x5049A0 | 0x504A25 | 55/133 |
| 0x401FC0 | 0x401FD1 | 5/17 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
