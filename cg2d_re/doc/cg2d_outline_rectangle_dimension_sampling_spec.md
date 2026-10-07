# 描边与填色逻辑尺寸取样 SPEC

编号：BVA01；2026-10-08。继续沿case2/522210真实矩形消费到451350，先SPEC后恢复实际差异；保持已正确的32位像素算法。

<a id="outline-rectangle-dimension-baseline"></a>
## 原端与当前缺口

[结构化证据](cg2d_outline_rectangle_dimension_sampling_evidence.json)保存完整451350–451D78共783条2600B/12页、零direct/两indirect call、84跳转/两返回。三原调用522253/52226D/522287同属522210，53条131B基线复用BOZ01，不再取完整父。

| 阶段 | 原地址 / 行为 | 修复 |
|---|---|---|
| 初始交集 | 451365读+94判left>=宽；right<0；45137F读+98判top>=高；bottom<0 | 现640/480常量改同源逻辑尺寸读点，保持短路顺序 |
| 原负left置0后 | 4513A8 fresh+94，right>=此宽则按保存S32回写right | 保存logicalWidth用于比较/回写，不与后续矩形width变量混用 |
| 原负top置0后 | 4513C6 fresh+98，bottom>=此高则按保存S32回写bottom | 保存logicalHeight用于比较/回写 |
| 宽高与模式拒绝 | right-left/bottom-top；mode非2要求两值>2，mode2仅width!=0 | 原条件已正确，保留，不新增/删除数学或空矩形保护 |
| 原32位起笔 | 45145B按native pitch/4，定位top*pitch+left+1 | 当前物理Back pitch与left+1正确保，不改逻辑尺寸替代物理pitch |
| 三32位模式 | 451531描边/边角；451A04整域填色；451C97单行 | 当前行/列/边角位置及原低W16颜色/AutoMap已正确，全文保 |

原556300三字节A2 E8 FF用于模式0颜色256的RGB扩展，额外索引不新增为新调色板表/地址身份；后跟原对齐项。源常量与原位序一致，保留，不扩表或加限长。8B尾451D78–451D80逐字节NN_NOP/90、已装载有值/无入向/后真入口，只认对齐用途。
两indirect是native Lock/Unlock机制，沿现已批SDL/CPU替换；不据此免除上层门禁、坐标、像素或寿命。当前Back可用门禁不改。

<a id="outline-rectangle-dimension-decision"></a>
## 工程决定

1. 限SurfaceState.cpp的Sub_451350一个prefix片段：初始交集分别取逻辑宽/高；left负值置0之后fresh保存宽用于right比较/回写；top负值置0之后fresh保存高用于bottom比较/回写。原RECT四S32原地更新、严格<0及>=边界保持。
2. 沿现GetRendererLogicalWidth/Height与唯一RendererResources owner，零头文件/新helper/owner/入口。新保存值命名logicalWidth/Height，避免与后续矩形width/height同名，原像素pitch和Back物理大小保。
3. 正确三个32位像素模式、模式拒绝、低W16参数/颜色256的3B、left+1起笔/所有边角/行列、native失败边界及void返回全文保持。正确522210整源和1411/BOZ限定证据不改，既有GraphicPendingQueue.cpp内私有声明保持，不向上层暴露SDK类型。
4. 按既有只复刻32位色裁决，将一个新F归mixed，在R1清单追加一记录，只排除本入口8/16/24位目标分支；共同裁剪/模式分流/32位像素和生命周期保留，不能整体排除。清单11→12、mixed2→3，语义总分类引用说明同步两个→三个；无新范围决定或排除函数。
5. 同步原新关系/对应文档、G02/R01/R04/V04与唯一进度，集中静态后派生看板/精确提交。数据格式网络仅明文、两叶排除/音频暂停不变；RPLAN.24异常不专项处理/模拟或记等价，不加额外夹限/补零/拒绝。
6. 不构建/调试/测试/运行、检查产物或任何摘要计算。原SDK机制替换不代替像素/正常失败/owner-alias寿命和R8验收。

<a id="outline-rectangle-dimension-acceptance"></a>
## 度量与验收

| 项目 | 证据 / 预期 | 限定范围 |
|---|---|---|
| 原证 | 全783条2600B/12页、BOZ53条131B复用、3B RGB与8B实际NN_NOP | 未重核全部父/间接/寿命 |
| 代码 | 整个cpp仅一具名prefix，四逻辑尺寸读点恢复，原像素/模式拒绝/调用者全文保 | 只有适用共同+32位路径静态 |
| 纳管/范围 | 1750/F:00051350 mixed，一X用途，R1清单12/mixed3；完整入口排除113不变、具名分类119→120 | 不计排除分支实现，不扣整个F |
| 功能任务 | LOOP163→164/RENDER64→65，四限定I/S各25来源；四任务仍open | 全域和182叶门禁保持 |
| 看板 | 语义1751/有效1744、合同1862（50.60%）、F892/E1054/R7已审454余2607；measure/Check退出0 | 文件关联率不是验收完成率 |

<a id="outline-rectangle-dimension-source-state"></a>
## 收口

本SPEC先固定，随后一cpp片段和R1增量、对应文档/机器/四任务/唯一进度，集中静态/派生成功后固定快照cg2d-re-outline-rectangle-dimension-sampling-20261008。所有原排除/混合记录及既有1411证据保持，不复核/重写已过像素流程。

上传可选源1020EC4另项只读查询仍不足以证明完整生产/间接别名；本步不猜值或类型、造producer、冻结当前零值或称不可达。该原有缺口保持独立追踪，继续可推进的真实裁剪开发。
完整父/callee、owner-alias寿命、正常失败、机器ABI/链接/像素/运行和适用R8继续；原pending18/未知ECX/closed11/差队列与全闭包保，整体目标active。

### BVA01 当前修复与混合范围限定静态结果

集中静态已通过：完整783条2600B/12页、BOZ53条131B准确复用、3B RGB与8B实际NN_NOP；整cpp只有一个prefix，原三32位像素/模式拒绝/physical pitch与正确caller整源/私有声明保持。三完整机器只有具名差异，R1原11记录和1411/旧证据保持；完整派生快照只新mixed身份、两域/候选、四任务/一X、R1分类来源和对应指标变化，八对应文档/十七文件UTF8无BOM/LF、四锚点/四限定I-S各25真实来源通过。

文本汇总与Check均退出0；实际语义1751/有效1744、合同1862（50.60%）、F892/E1054、R7已审454余2607、LOOP164/RENDER65。R1清单12/mixed3、具名分类120、原完整入口排除113；不扣整个混合F或计低色深为实现。pending18/未知ECX、closed11/原差队列和全闭包、旧四BUU错误证据fail保持；只有本步尺寸取样/适用32位局部静态通过，完整所有父/callee、owner-alias寿命、机器ABI/链接/像素/运行及R8未验，整体目标继续。
