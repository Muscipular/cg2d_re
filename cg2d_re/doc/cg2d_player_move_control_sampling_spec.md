# 玩家移动控制取样与队列分支 SPEC（BUB01）

日期：2026-10-08。承接BUA01跟随门禁的真实上游，按AGENTS.md第8条先记后实施。[完整原指令、局部调用窗口与源前态](cg2d_player_move_control_sampling_evidence.json)。

<a id="player-move-control-baseline"></a>
## 原端基线与具体差异

当前cg2d.exe两完整根：469C90 200条806B/三页，469FC0 98条456B/两页，共298条1262B/五页两块、48直接call/4间接GetTickCount/42jump/3RET。2入向：MainLoop 51C7D0@51D25A调用469C90后下一46BD40；469C90@469FAD调用469FC0后仅收栈/RET，不遍历未取得的MainLoop全CFG。

| 差异 | 原端具体取样 | 实施 |
| --- | --- | --- |
| 玩家状态 | 469CC0 TEST FDA88E BYTE 80；现flags18C完整W32判00800000 | 复用已有ReadPlayerActionFlagsByte，仅读同一BYTE |
| 连续模式 | elapsed W32>=500时发布C8A79C W16=1后直入469DBD，不再CMP该W16；不足500才CMP W16是否1 | 具名局部forceContinuousMove，短路进入分支，恢复原读次数 |
| 鼠标点击 | 469DBD TEST D68D9C BYTE1；现edgeButtons W32读取后窄U8 | 沿既有unsigned char别名方式直接读取该W32字段低BYTE |
| 目标临时量 | 连续X mask→encoded→count/Y encoded→mask→count；单次X encoded→mask→count/Y count→encoded→mask；每处首pub前原三值均已取齐 | 四片段各先const W32/W32/W8预取，再同序pub及Destroy |
| 队列分支 | 46A082和46A0AF比较C82202 W16非零后直至46A0DF复用比较结果；现最终条件再读一次 | 一局部无捕获lambda返回所比较S16；原回调后的fresh读取保持 |

原连续tick路径先第二次GetTickCount→采targetX→发布tick→Ctor，单次路径第二tick→发布tick→采X→Ctor。两路径各X构造/取样/pub/销毁完成后才fresh取targetY；四Ctor/四Destroy及RNG数保，四间接tick总数/正常250ms无符号比较保持。内部LegacyCryptInt为内部存储边界，与网络明文要求不冲突。

469FC0路径：C821FC非零且队列0才清标记/46B6F0；pathCount W16==0且队列0才46B4B0，之后fresh pathCount再写方向FFFF；pathCount>0且队列0才ABD0→fresh队列S16>0时AA00→UI4CD3E0→AC10；最后应从原被比较值或原46A0D7 fresh读取承接，不添一次读取。只恢复两处非零短路的原取样值，不能固定计数或删回调后必要读取。

队列正值后AF30→取方向S16→BCF0→速度W32→Y/X方向W16与原401E00承接→46B1B0→回读玩家Y-X→46A5A0。随后46A5F0→fresh停止W32→清C8222E W16→原返回或46A700/46A940→468200→世界X/Y W32先全取再投影pub，保持原调用、指针及读取序。

原FC0尾使用ECX/EDX复制投影，EAX沿468200；不是把worldX写入EAX或返回AL1。现U8/U16为局部返回观察适配：C90首门禁按已读Equal的AL承接、末两分发保局部低位；FC0早停沿原停止/AB60低W16、正常尾沿468200低W16。MainLoop局部窗口未消费返回，不能据此声称全上游弃返或完整机器ABI已验。该适配具名登记，不改既有签名/返回算法。

两尾区10/8共18B，原IDA显示align，实际insn_t.itype逐项为ida_allins.NN_nop/单BYTE90，无入向/后真入口，只用途，不新F或范围排除。

<a id="player-move-control-decision"></a>
## 具名裁决及实施

只改SceneState两既有正文八片段：C90两BYTE、模式短路及四临时预取七处；FC0一处局部无捕获lambda，复用两次队列W16比较结果并保所有回调后fresh取样。一局部lambda、无新全局helper/声明/owner/布局；既有两个reader/邻接断言、四个Yobi/Scene跟随移动下游与所有其它源保持。

新增1677-1678/adapted，两真实F；不把原端取证或源码存在算完整链验收，不为正确下游制造修改。网络原格式仅明文、内部LegacyCryptInt、音频暂停及RPLAN.24持续。

<a id="player-move-control-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 边界 |
| --- | --- | --- |
| 源码 | 一cpp、两既有正文八片段、一局部lambda | 完整源精准片段差异；BYTE/短路/四预取及两W16比较复用，原其余语句保 |
| 原端 | 298条1262B/五页、两局部入向窗口、18B用途 | 原字节/字段/宽度/分支/调用归档，不代MainLoop完整CFG或数值/机器ABI |
| 身份 | 两新1677-1678/adapted | 旧1677条/冻结定位/pending18与ECX pending7保 |
| 功能 | LOOP89→91/MAP86→88，各两局部I/S，共4E各25来源 | 完整叶/运行门禁保，不伪闭 |
| 派生 | 语义1679/有效1672、合同1790（48.64%）、处置820/E948、R7已审388/余2673 | 只由汇总器生成，静态后另记实值 |
| 任务 | U10/V03/R04仅current/引用，仍open | closed11、差队列与完整门禁保持 |

全部父/callee、正常owner-alias寿命、数值/x87舍入、机器ABI/链接、画面/明文互通及R8未验。只必要静态与纯文本汇总；不构建、不测试、不运行、不计算摘要，不降低标准。

<a id="player-move-control-source-state"></a>
## 当前实施状态

两完整根及原尾区/局部入向窗口已取证，八片段差异与修复方式已明确，先记本SPEC再实施并同步唯一进度/台账/任务。静态与汇总后记录实值。命名快照：cg2d-re-player-move-control-sampling-20261008。

BUB01集中静态已通过：完整Scene仅两既有正文八限定片段、两个BYTE/模式短路/四处三预取及两队列W16比较复用，一局部lambda保回调后fresh取样；五原页298条1262B/元数据/18B用途、两窗口、证据/台账/文档和三任务模型一致。汇总及 -Check 已通过，完整派生快照实值为语义1679/有效1672、合同1790（48.64%）、处置820/E948、R7已审388/余2673、LOOP91/MAP88。全部原对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11及完整门禁保持；仅局部静态，不代完整寿命/数值/机器ABI/编译/链接/运行。命名快照：cg2d-re-player-move-control-sampling-20261008。
