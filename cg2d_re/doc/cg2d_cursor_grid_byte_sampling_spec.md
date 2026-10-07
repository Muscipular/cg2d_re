# 鼠标目标格生产六处 BYTE 取样 SPEC（BUE01）

日期：2026-10-08。承接BUD01当前目标格生产者，按AGENTS.md第8条先记后实施。[完整原指令/局部窗口/常量/源前态](cg2d_cursor_grid_byte_sampling_evidence.json)。

<a id="cursor-grid-byte-baseline"></a>
## 原端与六处差异

原46F680完整167条619B/三页一块、11直接call/2GetTickCount间接call/23jump/2RET，唯一51C7D0@51D24B入向后下一504A50。当前源码已存在；本步补真实身份和原基线，并实修六处W32读后截断偏差。

| 原点 | 原读取 | 当前修复 |
| --- | --- | --- |
| 46F76A | 绘制门禁D68DB4 BYTE unsigned<212，mouseY Less通过才取样 | valueD68DB4 W32字段的低BYTE直接视图 |
| 46F80C | 绘制后第二事件门禁再次取D68DB4 BYTE | 独立第二次读取，不沿用绘制门禁值 |
| 46F838 / 46F8A1 | 左/右click分别取D68D9C BYTE | edgeButtons两次低BYTE读取，原时点不合并 |
| 46F867 / 46F8BB | 左/右held分别TEST D68D98 BYTE | heldButtons两处低BYTE test，保各自短路 |

LegacyMouseState中三字段均为W32；保字段/布局/原所有者，仅在本入口用既有unsigned char别名方式读取原低BYTE。不开新getter/头文件，不提前读或缓存跨DrawGraphic/tick后的状态。

## 保持的生产消费与数值源码

原先独立mouseY→mouseX各Read/F32，再屏幕转世界；X biased F64 0.5/ftol→先计算Y biased→pub pixelX→Y ftol/pub pixelY、普通S32加32和除64向零截断、目标Y先于目标X、反投影/四事件W32清序保持源码。已加载54DD28八BYTE实际F64 0.5证明位值，不证明完整数值/x87/舍入或画面运行等价；RPLAN.24不增异常或回绕模拟。

局部GraphicDescriptor16原全零覆盖为W32偏移2/6/10、W16偏移0/14五写，覆盖[0,16)全部字节；随后+8 W32 FFFFFFFF、+13 BYTE4。现值初始化、颜色memcpy与标志字节布局保持，不误称原描述符存在未定义残字节。drawY→drawX的偏置/ftol及DrawGraphic保持。

左click通过C8A774 W32门禁后pub单次事件/清elapsed→tick/pub C8A794；左held先读该W32基线，再pub持有事件。基线非零时再次tick后原SUB还会fresh读C8A794，当前now-gValueC8A794保持，不能改用此前baseline。右click/held仍分别读BYTE、以C8A7A8 W32门禁控制其原事件写入；不统一左右条件或时间逻辑。

正常路径原AL=2，早退沿第二Less的AL观察；现U8保持该局部视图。完整EAX/上游机器ABI未验，唯一窗口后未消费返回，不外推所有上游弃返。尾5B实际NN_NOP/90、无入向/后46F8F0，只用途。

<a id="cursor-grid-byte-decision"></a>
## 具名裁决与实施

只改SceneState::Sub_46F680一个既有正文六片段，以原三W32字段的低BYTE直接读取替代整字段读取后cast。两门禁、左右click/held均保持独立取样/短路与原读顺序；其余完整源码保持。

新增1683/F:0006F680/adapted，无新lambda/helper/声明/owner/布局/业务。网络原格式仅明文、内部LegacyCryptInt、音频暂停/RPLAN.24持续。

<a id="cursor-grid-byte-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 验收边界 |
| --- | --- | --- |
| 源码 | 一cpp、一个既有正文六片段 | 完整源精确变化；原六W8时点，三W32所有者/其它语句保 |
| 原端 | 167条619B/三页、唯一局部窗口、8B实际F64/5B用途 | 原字节/宽度/正常分支/布局归档，不代数值/机器ABI/画面 |
| 身份 | 新1683/adapted | 旧1683条/冻结定位/pending18与ECX pending7保 |
| 功能 | LOOP95→96/MAP92→93，各两局部I/S，共4E各25来源 | 完整叶/运行门禁不伪闭 |
| 派生 | 语义1684/有效1677、合同1795（48.78%）、处置825/E960、R7已审393/余2668 | 由汇总器生成，静态后记实值 |
| 任务 | U10/V03/R04仅current/引用，仍open | closed11/差队列和完整门禁保 |

只必要静态/纯文本汇总，不构建、不测试、不运行、不计算摘要。完整正常生产消费/owner-alias寿命、数值/x87/上游callee/机器ABI/链接/像素画面/明文互通及R8未验，不降低标准。

<a id="cursor-grid-byte-source-state"></a>
## 当前证据与实现状态

原167条/窗口/描述符写布局/常量及尾区已取得，六处读取宽度差异确认；先记本SPEC后实修并同步唯一进度/台账/任务。静态与汇总后另记实值。命名快照：cg2d-re-cursor-grid-byte-sampling-20261008。

BUE01集中静态已通过：完整Scene仅一既有正文六BYTE片段变化，原独立门禁/取样、三W32所有者与目标/16B描述符/事件/tick后fresh基线保持；三原页167条619B/元数据/窗口、8B实际F64/5B用途、证据/台账/文档和三任务模型一致。汇总及 -Check 已通过，完整派生快照实值为语义1684/有效1677、合同1795（48.78%）、处置825/E960、R7已审393/余2668、LOOP96/MAP93。旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11及完整门禁保持；只局部静态，不代完整数值/x87/正常寿命/机器ABI/编译/链接/像素画面/运行。命名快照：cg2d-re-cursor-grid-byte-sampling-20261008。
