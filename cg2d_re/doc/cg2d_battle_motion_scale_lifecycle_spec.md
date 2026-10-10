# 战斗运动与缩放效果生命周期 SPEC（BPS01）

2026-10-07；沿动作114已落位基线推进相邻动作113/154/155/156。原事实来自已打开IDA只读会话，工程委托有效，先记方案再实施。

<a id="battle-motion-scale-baseline"></a>
## 完整原根与具体源差

| 动作/原根 | 完整原指令 | 实际出向 | 唯一入向 |
|---|---|---|---|
| 113 / 40EFE0..40F093 | 53条179B/一页 | 3call/1普通jump/一ret | 4052CE |
| 154 / 40F790..40F7FB | 38条107B/一页 | 4call/3普通jump/一ret | 4052E4 |
| 155 / 40F800..40FB2E | 225条814B/三页 | 19call/28普通jump/一ret | 4052EF |
| 156 / 40FB30..40FB76 | 23条70B/一页 | 3call/2普通jump/一ret | 4052FA |

合339条1170B/六页/29call/34普通内部jump/四ret、四普通点同一404DB0父，只15条参数/弃返邻窗。邻窗含既有114行用于说明控制流，非重复验收其完整本体。[完整证据](cg2d_battle_motion_scale_lifecycle_evidence.json)保全部原指令、三F32常数和源前后态。原54D4A0/54D4B8/54D4BC分别为0ad7233c/00002041/cdcccc3d；原1.3为指令立即数3FA66666。

两具体差异：155的40FAC4取+10指针后判断并以同一EAX传MarkDeath；156的40FB4E取effect1060后读phase并以同一EAX传MarkDeath。源码各重复访问指针字段，改同阶段一次读取/非拥有局部借用。正常无介入值相同，不声称已有数值或并发故障，也不把源阶段修正当生成机器差异证明。

原154/155/156播放旗位都是MOV W32后TEST AL，不是直接TEST BYTE；原155两末次图号判定位也先MOV W32后TEST AH。因此正确W32源码保持，不能套用前轮低W8修改。

<a id="battle-motion-scale-decision"></a>
## 实施裁决与正常合同

动作113：先worldY发布offset44→405EA0移动→同一+10借用opponent X/扣graphicOffsetY的Y→worldY回存；原duration为+294的S32，除2/差平方/乘worldX减S32 field58加offset44后F32落位。fresh valueFF0的前值与duration比较、先增1，达到时用FE8/FEC的S32定位→439340(action)→fresh+10 MarkDeath→valueFF0清0。保原正常S32算术/浮点存储，未增加异常回绕/保护。

动作154：flagsFDC原MOV W32后TEST AL60，现W32读取保持；清60才PlayFrame(action,0)且弃返。effect1060空才41CD00(action,0,原零联合参数,24,0,0)→写借用槽→PlaySe212；后fresh读取effect1060，phase W32==1才439340(action)。不能缓存创建前槽跨播放/创建/音效。

动作155：flagsFDC先MOV W32/TEST AL60，清才PlayFrame(action,1)直到非0。value1024==0时41CD00模式22/末参1→先取soundX再写+10效果→PlaySe213→fresh位置和effect索引种子→RAND(1,9)乘原F32十分之一→再fresh种子→RAND(5,9)写attackMode→character8→device查询后fresh W32 flags设100000或8000→counter0。按target与scaleX比较分别减/加原F32 step，原FST保未落位中间量比较而各轴写F32；端点双轴对齐后fresh计数/总数，原比较/写入及末次分支保：缩小value1030==1设0不响、其它fresh payload flags1000选1.3/1并响；放大value1030==1设0并响、其它选1.3/1不响。其它缩小RAND(ftol(target*10),9)、放大RAND(1,ftol(target*10))，返回乘原F32十分之一发布后响；__ftol只已有CRT宿主正常S32转换承接。尾fresh总数+1与计数比较，达到时同阶段取一次+10效果→非空MarkDeath同一借用→清+10并清value1024；无论空否effect1060.phase=2/counter0，device后fresh flags清相应位，取soundX再发布flags→PlaySe212→character0→439340。真实各分支声调用不合并，音频暂停。

动作156：flagsFDC原W32读取，清60才PlayFrame(action,0)弃返；其后fresh取一次effect1060，phase W32==3才MarkDeath该同一非拥有指针→清effect1060→439340。保持原无额外空保护的业务前提，不提前跨PlayFrame缓存、不改变owner或位宽。

仅155在达到尾部计数条件后取castEffect，156在PlayFrame之后取effect；判断/MarkDeath使用同一原生YobiAction*借用，清槽继续写原字段。无新全局槽、owner、辅助函数、指针句柄、空保护或等待模型。动作113/154整个正确本体保持，两个新局部不得跨真实调用提前缓存。

155的原浮点FST/比较阶段由现有double中间量和F32落位承载，正常源顺序可对照，实际x87/生成机器精度未验；两处__ftol按[已有CRT宿主裁决](cg2d_compiler_helper_spec.md#compiler-helper-decision)供正常S32转换，不新造CRT体或扩大故障覆盖。五个原声音call点仅列当前既有接线/暂停，完整音频不在本步推进。

四已有定义首次明确原身份/完整正常源，新增1431..1434四语义及四正常equivalent。旧冻结定义定位不刷新，已有全部字段与非拥有+10联合别名、effect1060承载保持。

<a id="battle-motion-scale-acceptance"></a>
## 限定验收

- 四原根全部339条1170B/六页、29实际call/10被调入口、34普通内部jump/四ret及四唯一父邻窗、实际三个F32常数可追；不新造原函数或修改数据容量。
- 源整个YobiRuntime仅155/156各一借用片段，113/154全文与所有其它源保持；PlayFrame后/尾门禁后取样、MarkDeath同一借用及原清槽/推进序对应。
- 语义1431→1435/有效1426→1430、文档1544→1548/3680（42.07%）、BATTLE108→112仍枚举open，分类556→560；E736→738只两个本组局部I/S，原门禁不提升。R7 259/3061/余2802、队列172/待归属43及其余原状态保持。
- U08/R01/R04/V04只当前/引用仍open，60任务原关闭条件保。四动作完整339条1170B/六页、29实际call/34普通内部jump/四ret、四唯一普通入向同一404DB0父的15条邻窗和三个原F32常数，只当前完整正常源分支/字段/数值与调用阶段静态纳管。两源借用指针阶段修复，正常无介入值一致、不称并发/画面或生成机器bug；其余两动作正确源保持。全父/全部子依赖/其它状态生产及间接alias-owner寿命、SDK/生成机器ABI与实际x87精度、RNG/时钟/画面/运行未新验。__ftol沿已有CRT宿主正常S32转换批准边界，不扩大特殊值/故障模型；音频暂停与RPLAN.24保持。

<a id="battle-motion-scale-source-state"></a>
## 当前交付

两效果回收阶段一次指针取样已恢复，四完整正常动作源/真实调用/原身份纳管；固定快照 `cg2d-re-battle-motion-scale-lifecycle-20261007`。未编译、调试、测试、运行或计算摘要，完整战斗、全生命周期与R8继续原验收标准。

<a id="battle-motion-scale-113-current-20261010"></a>
## 2026-10-10 当前113读取状态

[BKE01](cg2d_battle_knockdown_exit_spec.md#battle-knockdown-exit-source-state)另恢复113入口worldY先payload、Move后两轴分别先fresh+10父指针再读取/发布该轴数值，共两组；原294/FF0数学/比较及末Advance后fresh父MarkDeath保持。此前“113整个本体保持”和source_body_changed=false属于BPS01固定前态，旧完整evidence.json不改写；1431原身份/正常equivalent类别和battle_motion_scale历史结构保持，只追加当前来源/状态，不称新第五身份或正常无介入值运行故障。全部SDK/机器ABI/alias/owner及声音画面仍待，当前WAV-only和执行限制保持。
