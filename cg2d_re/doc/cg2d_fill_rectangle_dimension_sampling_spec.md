# 矩形填充逻辑尺寸取样 SPEC

编号：BUZ01；日期：2026-10-08。BUY01正确变换/清理基线已提交，继续沿同一真实绘制链修复FillRect尺寸读点；先固定SPEC，再改已有代码，不新增绘制器。

<a id="fill-rectangle-dimension-baseline"></a>
## 原基线与实际差异

[结构化证据](cg2d_fill_rectangle_dimension_sampling_evidence.json)保留44C9A0完整162条625B/三页，两direct/六indirect call、15跳转/两返回，唯一原caller520AEF属于520AD0。后者19条57B原证复用既有BOZ01矩形消费证据，四次实际W16读取和完整返回等887601AE才写dirty已正确，不重新核整父。

| 取样阶段 | 原数字指令 | 当前缺口 / 修复目标 |
|---|---|---|
| 第一次接口查询成功后 | 44C9C4：+94 S32保存到EAX | 当前固定640；恢复保存逻辑宽供初始left与后续right比较 |
| right<=0已通过、top比较 | 44C9E9：+98 S32 | 当前固定480；只到此短路阶段读取逻辑高 |
| left负值裁剪后、right超界 | 44CA14以保存宽比较；44CA18 fresh读+94低W16写BX | 比较保初始width，只有超界才重新取宽发布S16 |
| top负值裁剪后、bottom比较 | 44CA26 fresh读+98 S32，44CA33把保存DX低W16写SI | 当前固定480；重新保存height供bottom比较与低W16发布 |
| 最终门禁 / 提交 | 44CA36第二次接口查询→+BC→四顶点/混合/解绑null/Draw | 已正确，全文保持 |

四坐标只消费低S16，color保完整W32、mix取低W8，返回保完整W32；不沿BUX01整数纹理入口误扩本入口参数。逻辑宽高是既有RendererResources同源+94/+98，初始化640/480不允许改成所有消费者常量。原新共享owner和GetRendererLogicalWidth/Height均已有，不新增影子状态或接口。

44CC11–44CC20尾15B逐字节NN_NOP/90、已装载有值、无入向且后接44CC20真实入口，只认对齐用途。

<a id="fill-rectangle-dimension-decision"></a>
## 实施决定

1. 第一次Has查询后保存width，初始left>=width/right<=0/top>=fresh高/bottom<=0保持短路原序。left负值置0保持；right>保存width才fresh取逻辑宽窄化S16写right。top负值置0后重新保存height，bottom>height才按此保存值窄化S16写bottom。
2. 四原S16参数和私有声明、两Has/+BC门禁、裁剪后四角/UV0/z0/rhw1/同色、原混合三路径、共享stage解绑null与真实Draw返回保持。SubmitTexturedRectangle及两纹理函数全文保持，CPU Back物理范围/native设备/raster/SDK承接不改。
3. 限一cpp一个具名prefix片段；零头文件、新helper/owner/入口。正确FillDirtyArea的四次W16/原描述符取样/busy消费和既有1412语义/F/E全文保，不扩写调用者或新增I。
4. 同步纠正现有FillRect文档的固定640/480描述，登记一新实际F与一X用途、两域限定I/S和三任务/唯一进度；新增纳管不算新函数实现。
5. 网络原格式仅明文、既有排除和音频暂停不变；RPLAN.24异常不专项处理、不模拟或记等价，不加空矩形/坐标域/钳制或保护。原正常left/top置零与right/bottom边界裁剪保留，不因异常裁决删除原行为。
6. 不构建/调试/测试/运行、检查产物或计算任何摘要；原SDK机制豁免不覆盖上层尺寸取样。完整像素/寿命/ABI/运行验收仍须满足整体SPEC。

<a id="fill-rectangle-dimension-acceptance"></a>
## 度量与验收

| 验收项 | 必须证据 / 预期 | 边界 |
|---|---|---|
| 完整原入口 | 162条625B/三页、2 direct/6 indirect/15跳转/2返回，一19条57B原caller复用与15B用途 | 不重验所有父/callee/SDK |
| 源修复 | 整个cpp仅一具名片段，宽保存/条件fresh低W16与高两阶段准确；正确caller/四其他函数保持 | 仅正常路径静态，无新异常策略 |
| 原新身份 | 1749/F:0004C9A0 adapted，一X alignment_nop，既有1412不改 | 不新增原分母/函数或排除 |
| 功能任务 | LOOP162→163、RENDER63→64，四限定I/S各25来源；G02/R04/V04只current/引用仍open | 全功能门禁不提升 |
| 看板 | 语义1750/有效1743、合同1861（50.57%）、F891/E1050、R7已审453余2608；measure/Check退出0 | 合同关联率不是验收完成率 |

<a id="fill-rectangle-dimension-source-state"></a>
## 源状态与收口

本SPEC先行固定，随后实施一cpp片段、同步原文/台账/任务与唯一进度，集中静态通过后派生看板并精确提交到cg2d-re-fill-rectangle-dimension-sampling-20261008。所有其他源码、既有正确1412记录/四BUU错误证据fail与原分母/闭包保持。

完整父/callee、owner/alias寿命、机器ABI/链接/像素/正常失败与运行、适用R8继续按整体目标推进；不得以本局部通过替代。整体目标active，pending18/未知ECX/closed11/原差队列保持。

### BUZ01 当前修复限定静态结果

集中静态已通过：完整162条625B/三页、BOZ01既有19条57B原caller准确复用和15B NN_NOP用途；整cpp仅一个具名片段，正确caller整源/四其他函数及原混合解绑/返回尾保持。完整语义/登记簿与派生快照只有具名模型差异，七对应文档/三任务与十五文件UTF8无BOM/LF、四锚点/四限定I-S各25真实来源通过。

文本汇总与Check均退出0；实际语义1750/有效1743、合同1861（50.57%）、F891/E1050、R7已审453余2608、LOOP163/RENDER64。既有1412/BOZ证据及四旧BUU错误证据fail保持，pending18/未知ECX、closed11/原差队列与全部闭包保持；只有本步尺寸取样修复/静态对照通过，完整父/callee、owner-alias寿命、机器ABI/链接/像素/运行及R8未验，整体目标继续。
