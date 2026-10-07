# 创建场景入口门禁适配 SPEC

> 2026-10-02提案；2026-10-04压缩更新。**已按本轮用户决策委托采用固定1方案**；后续P08接线现已完成，本体保持，见文末实施结果。原寄存器未知仍是事实，不因决议改称已知常量；见[局部确定化子SPEC](cg2d_local_determinism_spec.md)。
> 压缩前全文见Git命名快照 `cg2d-re-spec-history-20261004-before-closure-compression` 的本路径；对应[收尾P08](../plan/cg2d_spec_closure_checklist.md)。

## 事实与缺口

[原证据](cg2d_character_select_scene.md#creation-gate-exit-evidence)：WorldUpdate场景4先4596D0再4626A0；后者保存入口ECX，部分路径在状态10与1比较。输入链退出ECX随路径变化，DirectSound外部接口失败可留外部寄存器残值，当前cg2d证据不能确定；不能用新端SDL/编译器寄存器替代。

CharacterSelectScene已有ProcessCreationScene_4626A0(int32_t entryGate)完整显式本体，无默认参数；WorldUpdate已按P08显式传1并清理旧无参声明。正常选择完成/错误恢复会在同次本体内置gate=1，但尚未证明所有初始状态10不可达。

## 已决定的明确规则

实施时场景4保留输入处理，再显式调用 `character_select::detail::ProcessCreationScene_4626A0(1)`。

固定1使原依赖入口ECX的门禁不再依赖按键/音效/外部残留，初始状态10会执行463100初始化并进状态12；这是适配选择，不能称原端事实。选择/编辑/请求/错误恢复对局部门禁的后续赋值、状态10条件、同次连续处理及请求/清理/恢复顺序均保留，不改成每帧一次switch。

不选默认0（会跳过上述初始化），不模拟某按键ECX、不增随机/伪状态。

## 实施改动范围

| 文件 | 精确改动 |
|---|---|
| WorldUpdate.cpp | 引入既有CharacterSelectSceneDependencies.hpp，替换场景4无参调用；输入、Yobi/绘制位置不变 |
| WorldUpdateDependencies.hpp | 移除不再使用的无参Sub_4626A0声明 |
| CharacterSelectSceneDependencies.hpp | 注明本体接受显式gate、WorldUpdate采用已批固定1；不加默认参数 |
| 局部合同/唯一进度 | 记录原行为与适配差异，核唯一调用/实参/顺序/遗留声明；不重写本体 |

## 验收与等待范围

决议及P08接线已完成；显式本体、固定1和原调用序保持，完整controller原新由[BVH01](cg2d_creation_controller_baseline_spec.md#creation-controller-source-state)承接，完整功能仍待验。其他七项适配各按D01独立边界，网络与音频不由gate决定。

本决议只允许上述接线及新增差异静态核对，不解除不构建/测试/运行/计算摘要的限制。真实创建交互、请求往返与完整启动仍须后续验收。

## 2026-10-04实施结果

P08已按本SPEC接线：WorldUpdate场景4在输入处理后显式调用既有完整本体并传1，
删除旧无参声明；本体、后续局部门禁赋值与Yobi/绘制顺序保持。
详见[静态收口证据](cg2d_creation_and_module_adaptation_evidence.md#p08-创建入口显式gate)。
仅源码接线工作项关闭，创建交互、version生产、网络及运行仍未验。

## BVH01当前接线与完整静态本体纳管（2026-10-08）

[完整基线](cg2d_creation_controller_baseline_spec.md#creation-controller-baseline)、[纳管决定](cg2d_creation_controller_baseline_spec.md#creation-controller-decision)及[原证](cg2d_creation_controller_baseline_evidence.json)：创建controller完整130条464B/两页，18直接/16目标、30跳转/一RET、唯一World场景4十条41B；批准D01固定1/原ECX未知分开保留，现同帧连续状态推进及后半回边正确。本页两处现时无参待接句已更正，D01/P08固定1及唯一带参调用不变。完整controller已有正确实现，本步新增原新与静态S；版本零正文唯一对象已由BRT落实，完整功能仍待，未确定原ECX，也未改变适配行为。五源/完整本体与声明全文保持，代码0改动/无新I；新1762/F:000626A0 adapted与CHARACTER一限定S各26真实来源，pending7/18仍保完整功能待验、成员23不新增、U11/V03/R04仍open。所有callee/完整创建交互与请求回复、owner-alias寿命/机器ABI链接运行未验，全部闭包保持。

### BVH01 当前创建controller原新限定静态结果

集中静态通过：完整原根130条464B/两页、18直接/16目标/零间接/30跳转/一RET、唯一World场景4十条41B，原入口ECX保存及两后半回边有实际指令；五源/完整本体及私有S32声明、唯一D01 gate1调度与BRT const[1]正文/共享消费全文保持，代码0改动无新I。入口SPEC两现时无参待接句已更正、pending7当前说明承接本体原新和已完成正文事实；pending7/18与原policy保。完整机器/派生仅一原新/F、pending7当前事实、一域S/三任务与三指标差集；十四文件UTF8无BOM/LF、四锚点/一限定S的26真实来源通过。

文本汇总及Check均退出0；实际语义1763/有效1756、合同1873（50.90%）、F904/E1079，R7已审465余2596/CHARACTER23、原分类120/mixed3/完整入口排除113、closed11（P08/U03原closed）/差队列及完整闭包保持。完整controller只在D01批准固定门禁范围静态通过，原ECX仍未知，不称原寄存器等价；完整创建交互/请求回复、16个callee及正常owner-alias寿命、机器ABI链接/运行和R8未验，整体目标继续。
