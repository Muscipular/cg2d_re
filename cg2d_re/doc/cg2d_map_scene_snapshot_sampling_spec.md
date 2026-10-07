# 地图绘制父尾部快照预取 SPEC（BUK01）

日期：2026-10-08。承接BUJ01地图动作创建/绘制，沿父46CF60推进。按AGENTS.md第8条先记后实施。[十一页完整原端、三父局部窗口及源前态](cg2d_map_scene_snapshot_sampling_evidence.json)。

<a id="map-scene-snapshot-baseline"></a>
## 原端与本步差异

原[46CF60,46D9B9)单块704条2649B，十一页70×10+4、49directcall/73jump/两RET，三直接父43CB10/466B80/469020。源由一个父正文与三既有内部绘制helper承接，不为helper造原F身份。完整原端归档不等于全部callee/父或运行已验。

| 尾部路径 | 原读取与发布顺序 | 源码差异 |
| --- | --- | --- |
| CUT X / Y | W8 count→W32 encoded→W32 mask，三读齐后同序发布 | 现逐字段取即pub，恢复三局部预取 |
| 普通 X / Y | W32 mask→W32 encoded→W8 count，三读齐后同序发布 | 现逐字段取即pub，恢复三局部预取 |

同一临时先X Player GetVal/Construct/pub/Destroy，再Y同序，成功路径各轴各一次读取、两次Construct/Destroy和唯一RNG消费保。四分支分别预取，不先合并读取序或复用另一模式顺序。原尾audio::52CD60调用保持，音频仍暂停。

## 保持的生产消费与接线

入口16B描述符由偏移2/6/10三W32与0/14两W16零写完整覆盖，现全零值初始化正确保。mode和counter入口持有，原55DA8C已加载四BYTE10270000=10000，W32递增/按原重载路径清零保，不新模拟异常或回绕。

原X/Y NotEqual短路及dirty W16触发加载；边界每组按YEnd/XEnd/YStart/XStart独立取样，加载<0失败退出；callback后再采边界交CUT/普通walkability，移动/方向/flags与计数/dirty清序保。初始投影、S32算式、对角逐格和原音频marker调用/请求保。

原同格tile→object沿同一已取index，不能凭猜测在绘制后重新取span/index；外层跨度每次条件fresh读取，现表达正确。CUT tile/静态对象16B描述符各有原发布序，动态对象复用注册表、created指针及offsetY fresh槽；普通图层特殊范围、视口clip、S16低字局部承接与marker分支保持。三个既有helper和BUJ01创建/绘制代码保持，不新增保护或改业务。

原两成功尾EAX沿52CD60，加载失败沿负S32；469020后POP ECX/RET透传，另两父局部后窗立即覆EAX后绘制背景。现既有void动作观察适配保持，不说固定成功、全父弃返或全ABI。只保源码表达式/具名局部差异，完整数据/正常寿命/全像素画面/数值/x87仍须验收。

原尾7B逐BYTE实际NN_NOP/90，无入向/后46D9C0，仅用途，不新F或排除范围。

<a id="map-scene-snapshot-decision"></a>
## 具名裁决与实施

只改MapSceneDrawing一个既有Sub_46CF60正文四片段，CUT与普通X/Y各自三字段先取后发布，按原W8/W32序保。现两Construct/Destroy/Player读取和RNG、所有模式/加载/失败/循环/描述符/创建/绘制/marker/音频调用/字段/owner/返回观察、三个helper及所有其它源码和声明保持。

新1697/F:0006CF60/adapted，无新helper/lambda/header/共享owner/依赖。原格式明文、内部LegacyCryptInt、音频暂停/RPLAN.24持续。

<a id="map-scene-snapshot-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 验收边界 |
| --- | --- | --- |
| 源码 | 一cpp、一个既有正文四片段 | 全文精确模型；四模式载荷预取/原宽度，三个helper与其余全语句保 |
| 原端 | 704条2649B/十一页、三局部窗口/4B初值/7B用途 | 原字节/分支/取样归档，不代全callee/父/机器ABI/画面 |
| 身份 | 新1697/adapted | 旧1697条/冻结定位/pending18与ECX pending7保持 |
| 功能 | LOOP109→110/MAP106→107，各两局部I/S，共4E各25来源 | 完整成员/正常生产消费/寿命/运行门禁不伪闭 |
| 派生 | 语义1698/有效1691、合同1809（49.16%）、处置839/E984、R7已审405/余2656 | 汇总器生成，静态后记录实值 |
| 任务 | U10/V03/R04仅current/引用，仍open | closed11/差队列和完整门禁保持 |

仅必要静态/纯文本汇总，不构建、测试、运行或计算摘要。完整正常生产消费/owner-alias寿命、全父/callee/数值/x87/机器ABI/链接/全像素画面/明文互通与R8继续，验收标准保持。

<a id="map-scene-snapshot-source-state"></a>
## 当前状态

完整原父/三窗口/初值/尾区和源前态已取得，四尾载荷取样差异确认。先记本SPEC后实修并同步唯一进度、台账和三任务；静态与汇总后记实值。命名快照：cg2d-re-map-scene-snapshot-sampling-20261008。

BUK01必要静态已通过：完整MapSceneDrawing仅一个既有父正文四片段变化，CUT与普通X/Y按各自W8/W32顺序三载荷先取齐后发布；同一临时两读取/Construct/Destroy、RNG次数和三个helper及其它全源保持。704条2649B/十一页、三局部窗口/4B初值/7B用途与证据/台账/文档模型一致。汇总及 -Check 通过，派生实值：语义1698/有效1691、合同1809（49.16%）、处置839/E984、R7已审405/余2656、LOOP110/MAP107。旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11和完整门禁保持。未编译、测试或运行；完整正常寿命/数值/x87/机器ABI/全像素画面及R8继续。命名快照：cg2d-re-map-scene-snapshot-sampling-20261008。
