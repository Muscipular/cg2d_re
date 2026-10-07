# 上下扫描线转场触发槽取样 SPEC

编号BVC01；2026-10-08。沿矩形复制/转场链，固定51F860、51FA70共享触发取样的实际差异，仅修既有ProcessScanlineTransition一个prefix。

<a id="scanline-trigger-baseline"></a>
## 原基线

[完整原证](cg2d_scanline_trigger_sampling_evidence.json)保存两完整根、七父调用窗、两对齐用途及源/机器前态。

| 原身份 / 新语义 | 完整根 | 现来源 |
|---|---|---|
| F:0011F860 / 1753 | 151条524B/三页、10 direct/1 indirect/19跳转/2返回 | SceneState.cpp::Sub_51F860→ProcessScanlineTransition(Top) |
| F:0011FA70 / 1754 | 155条538B/三页、10 direct/1 indirect/19跳转/2返回 | SceneState.cpp::Sub_51FA70→ProcessScanlineTransition(Bottom) |

合306条1062B/六页、20 direct/2 indirect/38跳转/4返回、七引用同控制父51FDE0；七局部累计98条251B含重叠邻指令，不当完整父。原51F863/51FA73先保存共享W32触发58B6D0到ECX，再在51F878–51F888/51FA88–51FA98生产四S32 RECT，后续仅比较持样。新端原先在RECT生产之后才读触发槽，读点有差异。

两尾51FA6C–51FA70（4B）、51FC8A–51FC90（6B）合10B，已装载有值、逐字节NN_NOP/90、无入向/后真入口，仅对齐用途。

<a id="scanline-trigger-decision"></a>
## 实施决定

1. 在既有共享助手开头先保存完整W32触发槽，再生产四S32源矩形；触发判定使用持样，保留原零写回及另一分支FF4910独立取样。两个根wrapper、私有声明、现有owner、native边界和调用方保持。
2. 原逐行numerator/scale有符号除法、Top/Bottom源Y及原480边界、直接BltFast全S32坐标、复制后fresh比例和循环条件、绘制后低S8步长零取8与方向更新/真实0或1已正确，全文保持。Bottom第一行原可失败，沿既有原SDK资源失败裁决，不额外裁剪或造成功。
3. 完成时CryptInt2→3或3→4、负方向mask/encoded/count与正方向encoded/mask/count各三字段齐采再同序发布、析构后初始化槽清零保持。内部CryptInt不属网络加解密。
4. 控制父case26/28为Top负方向、case27/33为Top正方向，Bottom case29/31为正、case30/32为负，value直传/返回直透；原额外零实参完整根不消费，新端不添参数。只核七窗口，不称全父闭合。
5. 一cpp一个prefix/零头、零新helper/owner/入口。两adapted与两X用途，LOOP/RENDER四限定I-S及三任务仍open；不以这次局部修复提升完整门禁。
6. 原范围/网络格式明文、两加解密叶排除、音频暂停及RPLAN.24保持。不构建/配置/调试/测试/运行、检查产物或计算摘要；异常不专项处理/模拟或记等价，不降总SPEC像素/ABI/寿命/运行验收标准。

<a id="scanline-trigger-acceptance"></a>
## 度量与验收

| 项目 | 可复核要求 / 预期 | 边界 |
|---|---|---|
| 原证 | 两完整根306条1062B/六页、七局部累计98条251B、两10B用途 | 不冒称全控制父 |
| 源码 | 整cpp仅既有助手一个prefix；两wrapper及其余四相关源全文保持 | 不新增helper/API/owner |
| 语义用途 | 1753/1754两adapted、两X alignment_nop | 原分母/排除不增 |
| 功能任务 | LOOP166→168、RENDER67→69、四限定I-S各25来源；G02/R04/V04仍open | 全叶/域门禁保 |
| 看板 | 语义1755/有效1748、合同1866（50.71%）、F896/E1060、R7已审458余2603；measure/Check退出0 | 分类120/mixed3/全入口排除113保持 |

<a id="scanline-trigger-source-state"></a>
## 收口

先行SPEC后最小源码、原新/用途/七对应文档/三任务和唯一进度；集中静态、文本派生后记录实际结果，精确提交并固定cg2d-re-scanline-trigger-sampling-20261008。完整控制父/其他callee、owner-alias寿命、机器ABI/链接/像素/正常失败/运行与R8继续；pending18/未知ECX、closed11/原差队列与全部闭包保，整体目标active。

### BVC01 当前修复限定静态结果

集中静态通过：两完整根306条1062B/六页、七局部累计98条251B含重叠邻指令、两10B实际NN_NOP；整cpp仅既有助手一个prefix，完整W32触发先持样后RECT/按持样判定，两wrapper与其余四相关源全文保持，原采样/方向三字段齐采发布/native正常失败保。两完整机器及完整派生快照只有具名差异，十五文件UTF8无BOM/LF、四锚点/四限定I-S各25真实来源通过。

文本汇总与Check均退出0；实际语义1755/有效1748、合同1866（50.71%）、F896/E1060、R7已审458余2603、LOOP168/RENDER69。原分类120/mixed3/完整入口排除113、pending18/未知ECX、closed11/差队列/所有旧记录和全部闭包保持；完整控制父/其他callee、owner-alias寿命、机器ABI/链接/像素/正常失败/运行及R8未验，整体目标继续。
