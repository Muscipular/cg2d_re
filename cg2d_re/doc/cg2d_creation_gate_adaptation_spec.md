# 创建场景入口门禁适配 SPEC

> 2026-10-02提案；2026-10-04压缩更新。**已按本轮用户决策委托采用固定1方案**；后续P08接线现已完成，本体保持，见文末实施结果。原寄存器未知仍是事实，不因决议改称已知常量；见[局部确定化子SPEC](cg2d_local_determinism_spec.md)。
> 压缩前全文见Git命名快照 `cg2d-re-spec-history-20261004-before-closure-compression` 的本路径；对应[收尾P08](../plan/cg2d_spec_closure_checklist.md)。

## 事实与缺口

[原证据](cg2d_character_select_scene.md#creation-gate-exit-evidence)：WorldUpdate场景4先4596D0再4626A0；后者保存入口ECX，部分路径在状态10与1比较。输入链退出ECX随路径变化，DirectSound外部接口失败可留外部寄存器残值，当前cg2d证据不能确定；不能用新端SDL/编译器寄存器替代。

CharacterSelectScene已有ProcessCreationScene_4626A0(int32_t entryGate)完整显式本体，无默认参数；WorldUpdate仍调用无参缺口。正常选择完成/错误恢复会在同次本体内置gate=1，但尚未证明所有初始状态10不可达。

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

决议已解除等待批准；当前仍保留显式本体及可见无参缺口，后续按上表接线。其他七项适配各按D01独立边界，网络与音频不由gate决定。

本决议只允许上述接线及新增差异静态核对，不解除不构建/测试/运行/计算摘要的限制。真实创建交互、请求往返与完整启动仍须后续验收。

## 2026-10-04实施结果

P08已按本SPEC接线：WorldUpdate场景4在输入处理后显式调用既有完整本体并传1，
删除旧无参声明；本体、后续局部门禁赋值与Yobi/绘制顺序保持。
详见[静态收口证据](cg2d_creation_and_module_adaptation_evidence.md#p08-创建入口显式gate)。
仅源码接线工作项关闭，创建交互、version生产、网络及运行仍未验。
