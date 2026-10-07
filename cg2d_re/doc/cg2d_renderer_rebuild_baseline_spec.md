# 渲染重建与场景快照基线 SPEC（BUS01）

日期：2026-10-08。沿BUR01重绘实际父与渲染重建继续，按AGENTS.md第8条工程委托记本步范围。[完整原根、局部消费、原消息及源码前态](cg2d_renderer_rebuild_baseline_evidence.json)为对照。四现有实现已核正确，保留代码，不为纳管造改动、不新增I成果。

<a id="renderer-rebuild-baseline"></a>
## 完整原基线

| 原根 | 完整范围 | 原端量 | 当前源码 |
| --- | --- | --- | --- |
| 51D640 | [51D640,51D6E3) | 40条163B/一页，七directcall/两indirectcall/一RET、两入向 | SceneState正确快照父保 |
| 456D30 | [456D30,456D41) | 7条17B/一页，一RET、18入向 | 六W32尾槽归零/返0保 |
| 4512C0 | [4512C0,451306) | 23条70B/一页，一SDK indirectcall/一RET、23入向 | 获批32位CPU整面清屏保 |
| 466B80 | [466B80,466D41) | 130条449B/两页70+60，16directcall/八indirectcall/十jump/一RET、七入向 | RendererRebuild完整正文保 |

合200条699B/五页四块、23directcall/11indirectcall/十jump/四RET，50原引用来自18父，只身份与九局部后窗，不代全部父/callee复核。四直接定位此前无语义、文档合同、处置和功能成员，本步解决这些追踪缺口，不称新增四个函数实现。

51D640投影更新→两计数W16/W32清零→CacheReset→真实地图重绘→排序→清屏；C82204 BYTE保存/清0、queueMode0、DrawAllQueued、恢复BYTE后Back→Temp整面拷贝，随后**固定queueMode1**，两计数再清→CacheReset→独立取U32 tick，零扩为double写截止并返回同一样本。两个真实入向466CE2/51D226已有源接线，后者属于已纳管正常场景1580；旧SDL文档“后者未实现/绘制队列缺”只为历史。

456D30原REP STOSD清六连续W32共24B，当前唯一std::array<uint32_t,6>及fill(0)/返0正确。4512C0原当前背景W32经DD颜色填充作用于Back整面；当前CPU 32位整面填色按获批DX替换保，不扩原栈DDBLTFX/COM布局/HRESULT机制复刻，不因此免除颜色、方向、像素或owner寿命验收。

<a id="renderer-rebuild-decision"></a>
## 重建完整控制流与裁决

466B80入口保存W32 queueMode，Destroy→Init。第一次仅低S8，1走原提示，2将windowMode置为旧值!=1后重试；重试仅低BYTE非零走提示，其他初始结果直接继续。消息正文559340实际23B含NUL，标题5536B8实际5B含NUL，与现原字节字面量一致；flags10h，消息之后fresh窗口Post WM_CLOSE。之后仍BuildSurfaceCache/InitPalette；后者为0单独Post(104h,0Dh,0)，不变成早退或WM_CLOSE。

仅一次SceneType取样：10两计数清/CacheReset/46BD40/46CF60(0)/当前背景图号→DrawGraphic(320,240,200,image,0,nullptr)/Sort/Clear/queueMode0/DrawQueue/Back→Temp；恢复**入口保存模式**，两计数再清/CacheReset/独立tick写截止。9调用51D640，其固定1保；其他跳过这两段。之后fresh renderer palette binding，非空且当前windowMode!=0才256色写入，再独立palette释放→初始化，清两完整鼠标W32，最后另取tick写截止，不复用场景分支tick或总鼠标reset。

原重建尾EAX沿最终tick，但七原局部点随后取renderer/wParam、调用WndInit或显式xor0，均不消费tick；既有void动作视图保，不造新增返回接口或称整个窗口回调/外部ABI等价。51D640原U32返回和现接口正确，仍原值返回。所有四正确正文/四cpp、调用声明、数据owner、字体/调色板与网络源码完整保持。

DX7原生替换已批准：原BltFast及Clear COM/vtable内部采样以现CPU owner/整面操作承接，不把SDK内部取样顺序强塞上层新接口。高层标记保存恢复、计数宽度、队列/状态/时钟与Back-Temp方向仍按原；不新增缓冲、缩放、混合或空实现。既有初始化/排序1197/绘制队列1201/地图1716与1718/46CF60等提供者不重做，未用本步基线关闭其运行门禁。

<a id="renderer-rebuild-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 边界 |
| --- | --- | --- |
| 代码 | 四正确正文与四cpp全文保，代码/声明改动0 | 不新增实现或I成果 |
| 原端 | 200条699B/五页、28B原消息、九局部后窗、四尾53B用途 | 50引用仅身份，不代全部18父/callee或运行 |
| 身份 | 新1719..1722四adapted、四R7用途 | 旧1719条/pending18与ECX pending7保持 |
| 功能 | LOOP131→135/RENDER53→57/MAP126→128/BATTLE262→263 | 仅四S各25来源；两个父地图职责/重建场景10战斗职责有据，完整门禁保持 |
| 派生 | 语义1723/有效1716、文档1834（49.84%）、处置864/E1018、R7已审429/余2632 | 汇总器生成记实值，NETWORK61保持 |
| 任务 | U10/G02/R04/V04仅current/引用，各open | closed11/缺口队列及完整门禁保持 |

四尾13+15+10+15=53B实际逐BYTE NN_NOP/90、无入向、后真入口，仅对齐用途，不造新F或范围排除。只必要静态/文本汇总，不编译、测试、运行或摘要。完整生产消费、owner/alias寿命、全父/callee、业务失败/机器ABI、链接、像素、文件/资源来源与明文互通及适用R8继续，验收不降。网络原格式仅明文/两加解密叶排除、内部LegacyCryptInt/音频暂停/RPLAN.24保持。

<a id="renderer-rebuild-source-state"></a>
## 当前状态

四完整原根/原消息/九局部后窗/尾区及源码前态已取得。先记本SPEC，再登记四基线/处置/对应任务与唯一进度；源码保持，必要静态与文本汇总后追加实值。命名快照cg2d-re-renderer-rebuild-baseline-20261008。

### BUS01 本步实际结果

必要静态通过：四完整原根200条699B/五页、九局部后窗/28B原消息/四尾53B，四正确正文与四cpp/声明/提供者完整保持，两机器台账及五文档/四任务完整模型核定；四锚点/25来源及UTF-8无BOM/LF通过。派生核对模型按汇总器既有成员排序规范对齐，成员集合和数量未改，完整快照其余对象保持。文本汇总与-Check通过：语义1723/有效1716、文档1834（49.84%）、处置864/E1018、R7已审429/余2632，LOOP135/MAP128/RENDER57/BATTLE263，NETWORK61保持。代码改动/新增实现0，四域只S、无I；四任务open、完整叶验收0、整体未验收。未编译、测试或运行，不代全父/callee、资源寿命/业务失败/机器ABI、链接/像素/文件/明文互通及R8。命名快照cg2d-re-renderer-rebuild-baseline-20261008，整体目标active。
