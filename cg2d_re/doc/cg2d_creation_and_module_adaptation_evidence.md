# 创建门禁与模块路径适配的静态收口证据

> 2026-10-04；依据[局部确定化SPEC](cg2d_local_determinism_spec.md)的P08/P18。只关闭对应源码差异/接线工作项，完整创建/登录、链接与运行仍未验收。
> 本轮未新增IDA取证、测试或摘要计算，未执行模块摘要函数。原端事实沿既有[创建合同](cg2d_character_select_scene.md#creation-gate-exit-evidence)与[模块合同](cg2d_login_module_digest.md)。

## P08 创建入口显式gate

- 原入口：4626A0，唯一主状态调用位于WorldUpdate场景4；原输入处理先于创建本体。
- 新端[WorldUpdate.cpp](../src/world/WorldUpdate.cpp)直接包含既有CharacterSelectSceneDependencies.hpp，并在ProcessKeyInput_4596D0之后调用ProcessCreationScene_4626A0(1)。
- 已有[CharacterSelectScene.cpp](../src/CharacterSelectScene.cpp)本体未修改；S32显式参数和本体内gate后续赋值保持，不增默认参数，不改变状态10分支、请求/清理或同帧连续处理。
- 调用后仍依次执行YobiUpdateAll、RenderChara、RenderInput、输入光标更新；原场景4的状态、输入和绘制位置保持。
- [依赖头](../src/world/WorldUpdateDependencies.hpp)已删除无参Sub_4626A0声明；全本线源码/头中无该旧符号。
- 当前只有一个显式调用、一份带参声明和一份已有本体。局部接线工作项关闭，不等于版本正文生产、创建交互/网络或整个Character功能验收。

## P18 模块路径预清

- 原入口：5202E0；原路径256B未预写，GetModuleFileNameA传0xFF且忽略返回。
- [LegacyModuleDigest.cpp](../src/platform/LegacyModuleDigest.cpp)的非空output路径只有一次char path[256]{}；API仍为(nullptr,path,0xFFu)，没有尾补NUL、返回门禁、截断或宽字符改造。
- output为空仍直接返回nullptr；文件打开失败仍复制既有LegacyText593850的完整NUL文本并返回nullptr；成功编码/33B复制和返回原output的既有实现保持。
- 本轮不修改摘要实现；只核指定path对象、唯一初始化及现有caller。LoginScene已有真实调用，其他路径缓冲不套用本项适配。
- 路径预清是D01具名批准的差异，不能称原失败路径等价。此工作项静态核对关闭，API/文件失败、摘要/登录与运行仍未验证。

## 工作项与功能门禁

P08/P18的完成条件分别是唯一真实接线和获批初始化边界静态核对，以上源码证据满足这两个工作项。
验收登记簿仅将两个任务标closed；FUNC-CHARACTER/FUNC-LOGIN的B/I/W/S/R门禁仍未整体通过。
原函数成员只补确证的4626A0与5202E0，成员枚举继续开放，不以两个入口代替整个功能闭包。
