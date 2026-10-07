# 跟随门禁跨字段 W32 取样 SPEC（BUA01）

日期：2026-10-08。承接BTZ01玩家移动/五槽调度链，按AGENTS.md第8条工程委托先记后实施。[完整原指令/入向窗口/公共尾与前态](cg2d_follow_gate_word_sampling_evidence.json)。

<a id="follow-gate-word-baseline"></a>
## 原端基线与差异

当前cg2d.exe完整46A5A0 20条66B、46A940 22条71B、46A990 20条71B，共62条208B、三页三块、2call/11jump/3RET、3入向两父局部窗口。两门禁入口原先W16 gate，再从FDA88A一次MOV EAX取W32，TEST AH1；当前由flags18A W16和flags18C W32两次读取后拼接，虽然所判低位相同，读取次数/范围与采样边界不一致。

| 入口 | 原合同 | 当前措施 |
| --- | --- | --- |
| 46A5A0 | gate非零才46A5AA一次W32；flags100通过才第二项S16读/负值返回，再同借用action与原x/y调441F60 | 复用既有ReadPlayerFlags18A单跨4B取样，其他语句不改 |
| 46A940 | 同gate与46A94B一次W32；从第二项到第五项，首负S16停止，每项同借用action调442730 | 同单次W32接口修复，原循环/NULL交由既有callee处理，入口不加检查 |
| 46A990 | gate0不触表；非零时前W32全1→后W32全1→末W16全1；五槽按W16 flags非零后才读同slot action指针、按顺序写W16索引 | 当前正文正确，保持原样，补真实身份/基线与追踪 |

ReadPlayerFlags18A既有边界按offsetof从私有PlayerRuntimeSlots中复制4B，flags18A W16与flags18C低W16相邻已有静态断言，原[FDA88A,FDA88E)共享字节一致。只复用该实现，不再另造getter、owner、布局，也不窄为AH或分别读两个字段。

三现void保具名局部寄存器观察适配。两门禁入口入向后分别下一46A5F0或468200；构建入口469F89后gate/flags路径与46A190，以及469FAD公共尾CALL469FC0→ADD ESP,14h→RET，本尾没有AL/EAX写入。初稿的低AL=1概括已据原指令纠正；本步不猜469FC0返回合同或所有上游纯弃返，完整父/上游机器ABI未验。

三尾区14/9/9共32B逐地址90/NN_NOP、无入向/后真入口，只对齐用途，不新F或范围排除。五项顺序表保持10B；不改既有其它调度的末项前视裁决，不造第六槽或异常等价门禁。

<a id="follow-gate-word-decision"></a>
## 具名裁决及实施

只修SceneState两既有门禁正文各一片段，以const uint32 flags=ReadPlayerFlags18A()替代分别读取/拼接；第一点注释同步到既有边界。ReadPlayerFlags18A本体/邻接断言、46A990完整正文及全部其它源保持。gate、字段宽度、短路、第二项/逐项S16、非拥有指针与下游/RNG顺序不改。

新1674-1676三真实F/adapted；正确构建入口不造代码改动。无新lambda/helper/声明/owner/布局/业务；网络原格式明文、内部LegacyCryptInt、音频暂停和RPLAN.24持续。

<a id="follow-gate-word-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 验收边界 |
| --- | --- | --- |
| 源码 | 一cpp、两既有正文两片段；正确构建/reader/断言不改 | 完整源仅两片段变化，门禁后单次跨W32取样与其余语句保 |
| 原端 | 62条208B/三页、3局部调用窗口/公共尾、32B用途 | 原字段/字节/边界完整归档，不代父全CFG或机器ABI |
| 身份 | 新1674-1676，三F/adapted | 旧1674条/冻结定位/pending18与ECX pending7保持 |
| 功能 | LOOP86→89/MAP83→86，各两局部I/S，共4E，各25真实来源 | 完整叶/运行门禁保，不伪闭 |
| 派生 | 语义1677/有效1670、文档1788（48.59%）、处置818/E944、R7已审386/余2675 | 汇总器生成，静态后记录实值 |
| 任务 | U10/V03/R04仅current/引用更新，继续open | closed11、差队列与完整门禁保 |

全部正常owner-alias寿命/完整生产消费、完整两父与上游/callee、机器ABI、链接、画面/明文互通及运行R8未验。遵守不构建、不测试、不运行、不计算摘要，不降低标准。

<a id="follow-gate-word-source-state"></a>
## 当前实现与证据状态

完整三根/入向窗口/原公共尾已取得，两门禁拼接读取差异已确认；按本SPEC先记后修并同步唯一进度/台账/任务。静态/汇总实值另行追加。最终命名快照：cg2d-re-follow-gate-word-sampling-20261008。

BUA01集中静态已通过：完整源码仅两限定片段/三根原指令与入向窗口/真实公共尾、证据和台账/对应文档/三任务模型一致。纯文本汇总及 -Check 已通过，派生快照实值为语义1677/有效1670、合同关联1788（48.59%）、处置818/E944、R7已审386/余2675，LOOP89/MAP86。旧对象/冻结定位/pending18与ECX pending7、差队列172/171/43、closed11和完整门禁保；仅局部静态，不代编译、链接、正常寿命或运行。命名快照：cg2d-re-follow-gate-word-sampling-20261008。
