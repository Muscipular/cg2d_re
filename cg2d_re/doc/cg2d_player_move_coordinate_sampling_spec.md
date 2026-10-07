# 玩家移动坐标生产消费取样 SPEC（BTZ01）

日期：2026-10-08。原端取证与初稿完成于2026-10-07；本步实施及验收记录于2026-10-08。承接BTY01主角色与运动链，按AGENTS.md第8条工程委托先记后实施；[完整原指令、调用窗口与前态](cg2d_player_move_coordinate_sampling_evidence.json)。

<a id="player-move-coordinate-baseline"></a>
## 原端基线与差异

当前cg2d.exe完整46A410为99条395B，46A700为136条574B，共235条969B/四页两块、32call/15jump/3RET、3入向两父局部窗口。两根均有三临时字段在首pub前全部取齐的原序，但各轴/入口次序不同。

| 入口/轴 | 实际原预取与发布顺序 | 当前差异与措施 |
| --- | --- | --- |
| 46A410 snapshotX | 46A421/425/429先encoded W32→mask W32→readCount W8；46A42D起同序pub/Destroy | 当前边读边pub，先三const预取再同序发布 |
| 46A410 snapshotY | 46A455/459/45D先readCount W8→encoded W32→mask W32；46A461起同序pub/Destroy | 按Y独有顺序修，不套X顺序 |
| 46A700 arrivalX | 46A82E/832/836先mask W32→encoded W32→readCount W8；46A83A起同序pub/Destroy | 先三预取，保到格既有字段序 |
| 46A700 arrivalY | 46A86F/873/877同mask→encoded→readCount；46A87B起同序pub/Destroy | 先三预取，保X寿命结束后才fresh worldY求值 |

46A410各两构造两销毁、snapshot X完整发布/销毁后Y、Multiply64/世界坐标差/PC53方向数值叶/原浮点落点、按玩家是否存在发布步进、方向/动画/同步序保。64/0/4/0.01实际F32位值沿已有证据复用，不改变未验证数值表达式或新增回绕/故障模型。

46A700保先投影同步、步X零才步Y、idle时51A680==1才停动画/清标志、W16方向槽FFFF和尾同步。moving保targetX F32、Y求值时先worldY取样再Y落F32、两次动态倍率及X Double/worldX后X-F32/Y-F32落点、距离call前持有scaled步与世界坐标；平方和/未到时持有步配fresh worldX/Y求和发布不改。

到格先算gridX=targetX×实际F32 1/64，随后pub世界X/Y、清两移动槽、W16 C8222E=1，之后才ftol/Construct和本步三预取/pub；X销毁后从fresh已发布worldY求Y。value174普通S32只在>0才减，减到0且同玩家指针非空才phase2→payload counter48=0→snapshotY读取→snapshotX读取→socket→原MIS70请求。之后移动动画、重新取玩家指针/W16方向、投影/坐标同步保持；网络原格式与明文接口保持，不改请求值/字段/发包序。

两个现void维持具名局部寄存器观察适配：46A410唯一后下一442460 call，46A700两点后下一46A940或循环尾/468200；不外推全上游弃返或完整机器ABI。两尾区5/2共7B逐地址90/NN_NOP、无入向/后真入口，只用途。

调度46A190/46A5F0已有末项前视丢弃裁决见既有Yobi文档，当前两调度正文保持；不扩大五槽、不据正常未消费读省略宣称异常故障等价，也不将本步局部消费纳管当全部父通过。

<a id="player-move-coordinate-decision"></a>
## 具名裁决及实施

只修SceneState两既有正文四片段。按所属入口与轴顺序，以const W32/W32/W8或W8/W32/W32先全部取齐，再按原同序发布；原Construct/Destroy调用及剩余语句保持。无新lambda/helper/声明、owner、布局或业务，不改共用CopyPayload/AssignFromInt。

新1672-1673两真实F/adapted，保现void局部观察和未验边界。网络原格式明文、内部LegacyCryptInt、音频暂停与RPLAN.24持续，正确数值/到格/计数/MIS合同不据缺口修复扩改。

<a id="player-move-coordinate-acceptance"></a>
## 度量与验收

| 项目 | 本步目标 | 验收边界 |
| --- | --- | --- |
| 源码 | 一cpp、两既有正文、四片段；无新helper/声明 | 完整源只四片段变化，三预取/同序pub、各两构造两销毁及其它语句保持 |
| 原端 | 235条969B/四页、3局部窗口、两尾区7B | 全字段/字节/目标与边界归档，不代全部父或数值运行 |
| 语义 | 新1672-1673，两F/adapted | 旧1672条、冻结定位/pending18与ECX pending7保 |
| 功能 | LOOP84→86/MAP81→83；各两局部I/S共4E，各25来源 | 完整叶与运行门禁保 |
| 派生 | 语义1674/有效1667、文档1785（48.51%）、处置815/E940、R7已审383/余2678 | 汇总器生成，静态后追加实值 |
| 任务 | U10/V03/R04仅current/引用更新，仍open | closed11、差队列与完整门禁保 |

全部正常生产消费/owner-alias寿命、完整父/callee、数值、机器ABI、链接、画面、明文互通及运行R8未验。遵守不构建、不测试、不运行、不计算摘要，不降低标准。

<a id="player-move-coordinate-source-state"></a>
## 当前实现与证据状态

两完整根及三局部调用路径已取得，已确认目标快照/到格回写四处差异；按本SPEC先记后修，同步唯一进度/台账/任务。集中静态和纯文本汇总实值另行追加。最终命名快照：cg2d-re-player-move-coordinate-sampling-20261008。

### 本步验收实值（2026-10-08）

目标快照与到格回写四处原预取/发布次序已实际落实，原数值/字段/计数/MIS合同保持。

集中静态通过：完整SceneState仅两正文四预取/pub片段变化，snapshotX encoded-first/snapshotY count-first及到格两轴mask-first次序、各两构造/两销毁、RNG和X完整寿命后fresh Y保；全部其它数值/字段/方向/计数/原MIS70与明文接口语句保持。两根四归档段235条969B/三局部窗口/两实际尾区7B、证据全部字段与跨日记录、完整机器/五文档/三任务模型、十三文件UTF8无BOM/LF与四局部I-S各25真实引用通过。纯文本汇总与-Check沿同一次执行均终0，完整派生快照模型比对通过。实际语义1674/有效1667、文档1785（48.51%）、处置815/E940、R7已审383/余2678、LOOP86/MAP83；原pending18/ECX pending7、差队列172/缺定位171/待归属43、closed11/完整叶0/globalfalse保。未编译、测试、运行或计算摘要；完整父/callee/全部正常owner-alias寿命/数值/机器ABI/明文互通及运行R8继续。
