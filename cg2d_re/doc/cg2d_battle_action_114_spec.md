# 战斗动作114完整正常本体 SPEC（BPR01）

2026-10-07；沿[BPP字段修复与六槽原表](cg2d_unassigned_range_classification_spec.md#range-third-batch-source-state)完成该动作剩余本体。工程委托有效，原事实来自已打开IDA只读会话。

<a id="battle-action-114-baseline"></a>
## 完整原根与差异

40F0A0..40F771完整489条1745B/七页，34实际call/20被调入口、57普通内部jump、六槽switch和一ret；唯一入向4052D9属于404DB0，只采连续11条参数/弃返邻窗。[完整证据](cg2d_battle_action_114_evidence.json)保全部指令、分支、call、四原低字节点、六槽原表和54D4B4实际四字节常数。

现有全部正常分支、图号/随机消费、名称表、两轴缩放、内存CryptInt业务上下限和公共尾均可静态对照。剩余源差为四处原TEST BYTE：40F4D7的20H、40F62A/40F6E4的2H及40F71C的8H；现源码用W32字段掩码。两数显路径已共用显示尾，因此仅三个源片段恢复低W8对象表示读取。正常旧掩码判断值一致，不称数值、画面或并发故障；flags存储仍W32，所有写者保持。

<a id="battle-action-114-decision"></a>
## 实施裁决与阶段合同

| 阶段 | 原范围 | 保留合同 |
|---|---|---|
| 位置与图号 | 40F0AE..40F129 | 当前index→position>0回存共享位置/否则补原缓存→freshindex→target借用；initial.flags原MOV W32，bit4/400选110000/110098，否则仅一次RAND(0,2)+110001。 |
| 朝向与发布 | 40F12F..40F197 | level>=90以action否则entries[battlePosition]，406190(target,from,1,0)；freshflags W32发布effectFlagsFE4后406390，freshindex==0且level<90才创建名称效果。 |
| 名称与缩放 | 40F1B8..40F2A0 | 名称20×36B/原两字节比较只消费是否相等，12B三列名称图表按col0零哨兵/匹配取col1，默认110158；419FC0(action,image,0)返回必须消费，两轴分别fresh读取S32 level/10＋各F32scale＋原F32 54D4B4(cdcccc3d)，各自落F32，不共用quotient。 |
| 启用与类型 | 40F2A0..40F37E | 同一22×S32[2]共享pair按+04启用、+00类型；type1..6原六槽switch，其中type4共享分支，41F660(entry,type+110074)返回不消费；禁用则flagsFDC&FFFFFC0F后AppendCast(action,target,142)。BPP源修保持，不重建布局或换生产序。 |
| 命中与扣除 | 40F37E..40F4F6 | hitFlags原一次W32，8→phase31，100→flash1/PlaySe210/phase31；其它在E000清且amount非零时按bit2建110008，再image>=0创建图效果、4208F0、PlaySe267；freshrecord扣readiness，freshentry比较0，负时Construct0→freshentry CopyPayload→Destroy。随后独立freshrecord低W8 bit20才flagsFDC\|2/gate1/alive1。 |
| 恢复分支 | 40F4F6..40F640 | freshflags W32，8000/2000只推进target；4000先推进，freshindex的bit10000000建22459，否则freshamount加readiness→freshentry与maxHp比较/超出时freshentry CopyPayload→freshflags/targetY/amount/targetX、数字(type5,2)。保真实业务0/maxHp限制，非故障防护。 |
| 普通分支 | 40F645..40F6FE | 其它flags中100000先MarkDeath(effect1054)→读取W32flagsFDC并清bit20→清借用effect1054指针→写回W32；再推进target，freshflags的10000000建22459，否则Y/amount/X与数字(type6,2)。两个数显路径原各一次freshrecord低W8 bit2，源码共用显示尾一次读取建22454，随后4209C0(from,target)。 |
| 循环与公共尾 | 40F710..40F771 | freshindex→record低W8 bit8清且position3非零才W32 targetflags计算14*((flags>>1&1)+1)写valueFD0；439470(action)直到返回1循环结束，最后439340(action)一次。保短路与target借用，不提前amount或增加保护。 |
| 入向与暂停 | 4052D8..4052DE；40F3BE/40F431 | 404DB0 case114压action调用/不消费返回到公共尾；源码static_cast<void>包装与break保。52C450历史事实只消费sound/x两个参数，音频仍暂停，不开发音频或把未消费栈参数变接口。 |

只在上述三个既有消费阶段读unsigned byte对象表示，不增加辅助函数、影子状态、保护或异常模型。initialFlags、hitFlags、后段flags与displayFlags保原W32整读，不能因原随后测试AL/AH而改低W8。原两轴各重读level，不缓存共同商；原57E174表12B行和三列用途保，54D4B4原F32常数按既有double加法、F32落位承载，未把静态阶段核定外推为生成机器或实际x87运行精度通过。

BPP真实启用[1]/类型[0]修复保留；本步补完整正常源，不重复验证已过动作141/143或全部创建依赖。1428/F:0000F0A0旧全文先保存，再增加本步完整映射与一项equivalent处置，范围只正常源合同。

<a id="battle-action-114-acceptance"></a>
## 限定验收

- 七页连续489条/1745B、34call/57内部jump/六槽switch/一ret、唯一父邻窗、四原W8点和54D4B4原cdcccc3d可追，既有BPP表数据分列；不新增原F/X或修改物理容量。
- 三个源片段精确替换，整个YobiRuntime其余字节、原字段、实际父弃返包装与BPP已修pair保持；两显示原W8点映射一个现有共同尾，不重复读取。
- 既有1428全文前态保留，语义1431/有效1426、文档1544/3680（41.96%）、BATTLE108枚举open、R7 259/3061及队列172/待归属43保持；分类555→556、E734→736仅一正常equivalent与两局部I/S。
- U08/R01/R04/V04只当前/引用仍open，60任务原关闭条件和完整门禁保持。40F0A0完整489条1745B/七页、34实际call/57普通内部jump/六槽switch/一ret、唯一4052D9同父404DB0的参数及弃返邻窗；只当前动作完整正常源分支/字段/值与调用阶段静态对应。四原TEST W8点由三个源片段承载，正常旧掩码值相同；保BPP已修启用[1]/类型[0]与所有其它源。依赖本体、全状态生产/所有父与间接alias/owner寿命、SDK/生成机器ABI及x87实际运行精度、RNG/时钟/画面/运行未新验，RPLAN.24不专项异常/保护/模拟或故障等价。

<a id="battle-action-114-source-state"></a>
## 当前交付

完整动作正常本体/入向及三片段四W8是BPR01历史交付，固定快照 `cg2d-re-battle-action-114-full-source-20261007`。2026-10-10 [BCVI01](cg2d_battle_cache_initial_spec.md#battle-cache-initial-source-state)另修当前位置缓存模型静态零初值与一临时HP零复制读序；旧原证据JSON保持历史，不改成当前正文。原缓存容量/完整运行初始化与全部owner/SDK/机器门禁仍待；未编译/调试/测试/运行或计算摘要，音频按当前WAV-only、原格式明文继续。
