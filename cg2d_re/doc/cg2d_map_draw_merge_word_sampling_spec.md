# 地图缓存合并逐 W32 复制 SPEC（BUM01）

日期：2026-10-08。承接地图父与第四平面，沿真实C2F0→C420/C4D0→C1C0依赖推进。按AGENTS.md第8条先记后实施。[四完整根/两父局部窗口/源前态](cg2d_map_draw_merge_word_sampling_evidence.json)。

<a id="map-draw-merge-word-baseline"></a>
## 完整原基线与本步差异

| 根 | 原完整范围 | 原端量 | 当前处理 |
| --- | --- | --- | --- |
| 46C1C0 | [46C1C0,46C2E6) | 122条294B/两页/六call/12jump/一RET | 正确提交体保持 |
| 46C2F0 | [46C2F0,46C2FA) | 两条10B/一页/一call/一尾jump | 正确排序→合并顺序保持 |
| 46C420 | [46C420,46C43B) | 八条27B/一页/一call/一RET | 专用原qsort承接保持 |
| 46C4D0 | [46C4D0,46C759) | 192条649B/三页/四call/25jump/一RET | 六复制段逐W32/地址复制实修 |

合324条980B/七页四块、12directcall/38jump/三RET、六入向/两父。原合并六rep movsd位于46C56D、46C5E4、46C643、46C6A0、46C6FE、46C747，单项13个W32、bulk按剩余项×13前向复制；当前五整记录赋值和一个std::copy_n未在源码表达原每W32读写粒度。

## 原52B/x64字段复制合同

沿[原记录身份](cg2d_map_record_identity_spec.md#map-record-identity-contract)及[缓存布局](cg2d_map_draw_cache.md)保留前44B：图号/源索引/未解释W32、F32位型、S16坐标、批次BYTE/未写三BYTE、原16B描述符；两个已证非拥有地址由原+2C/+30拓为native before/after，新+30/+38，总64B。本机+2C..2F对齐空隙不是原字段，不复制为原数据。

在本文件匿名namespace加唯一CopyMapDrawRecord：前44B按十一W32每次先读该字再写该字，随后read before→write before，再read after→write after。原地址在此按x64合同拓宽，不截W32、不复用另一owner、不新增原F身份、引用计数或释放。

六段分别使用同一helper；bulk前向逐原生记录，保持所有复制完成后原pub时点。只复制原字段及原生链接，不改比较器/排序或资源查询，不把复制聚合为共享业务append。

## 六段计数与正确依赖保持

| 复制段 | 原计数发布 | 本步处理 |
| --- | --- | --- |
| 首轮主项 | 先publish merged+1，再复制 | 原顺序保 |
| 首个对象 | 先publish merged+1，再复制 | held object及fresh objectLimit保 |
| 后续对象 | 先复制，再publish已递增merged | 两比较/退回objectIndex及fresh计数保 |
| 剩余对象bulk | 局部objectIndex/merged先增，复制完再pub | 原前向记录/字序恢复 |
| 剩余主项 | 先pub merged+1，再复制 | mapNo过滤及fresh主数保 |
| 特殊表 | 段内前向逐项，段后pub | 原3072容量和fresh特殊数门禁保 |

C1C0仍before链→主项→after链，各次按当前主项clip BYTE==1分流，各取当前记录image W32、Y/X S16和该记录描述符；挂件batch原movsx BYTE，而已纳管U8提交接口只消费低BYTE，现BYTE承接有效，保持接口。链的next在调用后fresh取，每主项后fresh mergedCount，返最后计数；不提前快照整链或用挂件clip替代主clip。

C2F0原call排序→tail jump合并，现void动作适配保持；C420原count W32/52B size/比较回调交53C112，现专用原算法和三处同址交换门禁已有证据，保持，不重新证明通用CRT。原C300/C440比较器及四生产体已纳管，全部源码保持。

父CF60 CUT/普通两个局部窗原C2F0→C1C0后PlayerX读取，现接线保持；这里只具名局部返回消费，不造fixed返回或称全机器ABI/所有父已验。原四1024源/3072合并数组、独立S32 mergedCount及四SceneState计数/链接寿命保持，无新平面/owner/业务。
四尾10+6+5+7=28B逐BYTE实际NN_NOP/90，无入向/后真入口，仅R7用途，不增F或排除功能。

<a id="map-draw-merge-word-decision"></a>
## 具名裁决与实施

只新增本cpp一个私有字段复制helper，合并一个既有正文六片段替换，另三正确根全文保持。源前44B十一W32再两native链接保证原字段和采样表达，bulk逐记录保持线性前向序，各自pub时点不合并。保持现有64B布局/本机padding边界、全部其它语句/声明/生产者/比较器/资源/队列，无外部依赖/RTTI或异常策略。

新1700/C1C0、1701/C2F0、1702/C420、1703/C4D0均adapted。此处源码取样合同不代编译后机器逐指令、正常生命周期或像素验收；RPLAN.24不专项异常/回绕/越界模拟。网络原格式明文、内部LegacyCryptInt与音频暂停持续。

<a id="map-draw-merge-word-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 限定边界 |
| --- | --- | --- |
| 源码 | 一cpp，一个私有helper插入/一个合并正文六片段 | 四root与完整文件模型；前44B十一W32/两native链接，各pub时点保 |
| 原端 | 324条980B/七页、两局部窗口/28B用途 | 全四原根/六rep采样与正常数据/返回，不代所有callee/CRT/父/运行 |
| 身份 | 四新adapted/四R7用途 | 旧1700条/冻结定位/pending18与ECX pending7保 |
| 功能 | LOOP112→116/MAP109→113，各局部I/S，共4E各25来源 | 完整成员/生产消费/寿命/像素与运行门禁不伪闭 |
| 派生 | 语义1704/有效1697、合同1815（49.32%）、处置845/E992、R7已审411/余2650 | 汇总器生成，必要静态后记实值 |
| 任务 | U11/G02/R04/V04仅current/引用，四任务仍open | closed11/差队列和完整门禁保持 |

仅必要静态和纯文本汇总，不构建、测试、运行或计算摘要。完整正常生产消费/owner-alias寿命、全父/callee/数值/x87/机器ABI/链接/全像素画面/明文互通与R8继续，不降低验收标准。

<a id="map-draw-merge-word-source-state"></a>
## 当前状态

四原根与现源已取得，六复制粒度差异确认，另三正确体保持。先记本SPEC，再实修并同步唯一进度/台账/四任务；静态与汇总后记录实值。命名快照：cg2d-re-map-draw-merge-word-sampling-20261008。

BUM01必要静态已通过：完整MapDrawCache仅一个私有CopyMapDrawRecord插入及合并正文六片段变化，前44B十一W32每对读写后before/after native非拥有地址，bulk前向逐记录；原各pub时点/正常分支、另三正确根/布局/owner/四生产/两比较/父及队列声明全保持。四原根324条980B/七页、六rep/两局部后窗/四尾28B用途与证据及机器/文档模型一致。汇总及 -Check 通过，派生实值：语义1704/有效1697、合同1815（49.32%）、处置845/E992、R7已审411/余2650、LOOP116/MAP113。旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11及完整门禁保持。未编译、测试或运行；完整正常生产消费/寿命/数值/x87/全父/callee/机器ABI/像素及R8继续。命名快照：cg2d-re-map-draw-merge-word-sampling-20261008。
