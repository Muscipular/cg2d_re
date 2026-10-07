# 四块转场与源矩形复制的完整基线 SPEC

编号BVB01；2026-10-08。沿Back/Temp实际复制到4535B0及其四块转场父51EC10，先固定已有正确实现的保留裁决。源码改动0，无新I，不为纳管造补丁。

<a id="quadrant-transition-blit-baseline"></a>
## 原基线

[完整结构化证据](cg2d_quadrant_transition_blit_baseline_evidence.json)保存原全部指令和入向。

| 原身份 / 新语义 | 完整本体 | 实际源 |
|---|---|---|
| F:000535B0 / 1751 | 92条278B/两页；0 direct/1 indirect、9跳转/2返回 | SurfaceState.cpp::BlitSurfaceToBack_4535B0 |
| F:0011EC10 / 1752 | 216条869B/四页；7 direct/0 indirect、25跳转/2返回 | SceneState.cpp::Sub_51EC10 |

合308条1147B/六页、7 direct/1 indirect/34跳转/4返回。复制叶十三原引用来自51D790、51E0D0、51EC10、51EF80四父；四块父两引用来自51FDE0；合十五引用五父。十五局部窗累计210条697B，窗口含邻接/重叠指令，不冒充十五独立无重叠块或完整五父。

4536A4以SI、4536AA以保存坐标的低word分别movsx后提交原BltFast，证明最终坐标也是低S16；不能只看前半部保存DWORD就扩成S32接口。现十三caller的低S16转换/借用和丢返回保持。
51FDE0原case18/19分别传mode0/1及value，额外压一个零实参但51EC10完整根不消费它；当前WorldUpdate透传两实参/返回保持，不添占位API。

两尾4536C6–4536D0（10B）、51EF75–51EF80（11B）合21B，逐字节NN_NOP/90、已装载有值、无入向、后真入口，仅认对齐用途。

<a id="quadrant-transition-blit-decision"></a>
## 正确实现保留决定

1. 复制根先按top/right/left/bottom保存源RECT，坐标仅低S16；四个初始交集门禁和负位移/后续逻辑尺寸读点、原右/底式回写矩形均已正确。完全离屏返回0不改矩形，实际复制走既有native无色键边界。原完整HRESULT机制沿已批SDK替换，新端0/-1与十三原caller丢返回边界保，不称所有间接ABI相同。
2. 四块根四RECT共64B、输入步长低S8零时4、共享触发/phase及两方向初值/取样/写序、原非对称边界写0均正确。尤其mode1的x1<320等分支原确写0，不按视觉对称性修成320/240。
3. 原mode1各坐标持样/发布和后续fresh阈值判断、mode0取样/发布、清屏后四次逐块fresh Temp→Y低S16→X低S16→同源RECT复制保持。四块Rect受callee原位裁剪，与原栈四RECT同消费，不冻成单份共享矩形。
4. 完成状态独立重读phase；CryptInt依2→3或3→4，构造后mask/encoded/count三字段取齐/同序发布，析构后phase=-1，原0/1返回保持。内部CryptInt不是网络加解密，token/reserved和网络明文不改。
5. 五已有相关源和两完整正文全文保持；现[波纹原条带裁决](cg2d_ripple_full_strip_production_spec.md)已明确保留native原矩形失败边界，不把既有资源失败处理误删为新增策略，也不扩大BlitTempToBack的接受范围。Back/Temp仍同owner/同步借用。
6. 两已有F/adapted与两X用途、两个域各限定S/三个开放任务和唯一进度补齐。仅静态正确性，不计新代码或新I，不提升全功能门禁。网络原格式明文/既有排除、音频暂停及RPLAN.24保持。
7. 不构建/调试/测试/运行、检查产物或计算摘要；异常不专项处理/模拟或记等价。完整像素/寿命/ABI及R8仍须按总SPEC验收。

<a id="quadrant-transition-blit-acceptance"></a>
## 度量与验收

| 项目 | 可复核要求 / 预期 | 边界 |
|---|---|---|
| 原证 | 两完整根308条1147B/六页，十五局部累计210条697B、两21B用途 | 不把窗口累计当全父覆盖 |
| 源与连接 | 五相关源与HEAD全文相同、两正确完整正文/现S16声明/case18-19透传保持 | 源码0改动，无新I/完整W |
| 原新纳管 | 1751/1752两adapted与两X alignment_nop | 不新增原分母/入口或排除 |
| 功能任务 | LOOP164→166、RENDER65→67，两个限定S各25来源；G02/R04/V04仍open | 完整叶/域门禁不提升 |
| 看板 | 语义1753/有效1746、合同1864（50.65%）、F894/E1056、R7已审456余2605；measure/Check退出0 | 分类120/mixed3/全入口排除113不变 |

<a id="quadrant-transition-blit-source-state"></a>
## 收口

本SPEC先固定保留裁决，随后原新/用途/对应文档/三任务与唯一进度，集中静态后派生看板，精确提交并固定快照cg2d-re-quadrant-transition-blit-baseline-20261008。完整四个复制父及控制父、其他callee/owner-alias寿命、机器ABI/链接/像素/正常失败/运行与R8继续；pending18/未知ECX、closed11/原差队列和全部闭包保，整体目标active。

### BVB01 当前基线限定静态结果

集中静态通过：两完整根308条1147B/六页、十五局部累计210条697B含重叠邻指令、两21B实际NN_NOP；五相关源与HEAD全文相同，两完整正确正文/低S16声明/case18-19接线及原非对称写0/三字段发布保持。源码0改动/无新I，完整机器与派生快照只有具名差异；十四文件UTF8无BOM/LF、四锚点/两限定S各25真实来源通过。

文本汇总与Check均退出0；实际语义1753/有效1746、合同1864（50.65%）、F894/E1056、R7已审456余2605、LOOP166/RENDER67。原分类120/mixed3/完整入口排除113、pending18/未知ECX、closed11/差队列/所有旧记录与完整闭包保持；完整五父/其他callee、owner-alias寿命、机器ABI/链接/像素/正常失败/运行及R8未验，整体目标继续。
