# BMF01 战斗事件完成与场景退出取样

日期：2026-10-06；按既有工程委托先记录方案后实施。沿[BME01事件调度与初始化](cg2d_battle_m_event_dispatch_sampling_spec.md)的真实消费者，修复事件完成与完整十状态场景中的取样宽度、读取及发布时序；不另建协议、场景或资源所有者。

<a id="battle-event-finish-scene-lifecycle-sampling-baseline"></a>
## 原端完整基线

| 原入口及完整区间 | 当前符号 | 指令 / 字节 | 普通入向 / 父 | 出向 |
| --- | --- | ---: | --- | ---: |
| 43B6C0..43B7EB | FinishBattleEvent_43B6C0 | 109 / 299 | 94；4239D0(1)、424620(4)、426FC0(1)、427470(88) | 5 |
| 43CBB0..43D6F9 | DrawBattleScene_43CBB0 | 741 / 2889 | 51C71C；父51C430 | 134 |

两单chunk完整850条3188B、十二指令页、三引用页及95普通call/去重5父/139出向，均含[实际字节与引用全集](cg2d_battle_event_finish_scene_lifecycle_sampling_evidence.json)。引用不代五父正文验收。Finish旧语义803保留当时字符串接线范围，仅追加本次完成/退休合同；场景补一个缺失原身份。

原43CBF5十状态直接表43D6FC为40B；音乐43CCCF→43D724、43CCE9→43D73C各六槽24B，共三表22槽88B，记录原字节、目标及分组。十状态0..9和default公共尾保持。音乐表仅核父已有选择与调用，不解除音频暂停或把音频算完成。

## 完成、生产与退出合同

43B6C0固定入口action的payload，先S32消息数−1并发布，才判正数。正数分支以live packet64逐次S8读取至NUL/大写边界，即使NUL也按原53BEC0复制3B；复制后重读64算next+4，header[3]置NUL后发布next。25头原53BE40首命中才更新phase，失配保旧phase，随后58/68归零。两分支各22轮先取A8再形成bit，命中才InitBattleCastList_4390E0，调用后下一轮重新读A8。结束分支先eventSlot形成bit，再取活动mask，再取反/AND并发布；随后必须再读eventSlot清32事件表对应非拥有T*。22轮恢复后清busy，才MarkYobiActionDead，不在此free；正数分支最后清busy/waitAC。

43CBB0入口一次取state；state按U32在2..6且背景值改变时先43CB10，再重读state进入十状态。状态0清20项36B命名特效、64项28B标题找1101，属性W8零写/命中5；原地图W16条件、YobiClearUp→InitializeBattleScene、入口/退出引用、输入上下文、背景/聊天/音乐/计时/渲染/state递增保持。音乐特殊背景238607..238618与mapPuk、一般场景和floor1457..1480覆盖选择保持。

状态1保转场成功/失败渲染输入次序和公共尾重复输入；状态2保更新/排序/渲染/聊天/UI/随机推进及两次playerFlags字节读取。C队列四槽4096B复制含NUL：原W32长度+1/整字节组复制→next+1/比较4→readIndex发布→尾字节→按同条件回0；ParseC后fieldFlags原W8测试，退出直接state7，否则位置改变时按原20角色循环、每次调用后复读全局索引，保pet快照/29窗口/音效/scene3及后续Hint。状态3在MarkDeath后先读并AND玩家W32标志，再清notice槽，再发布标志；不得把原仅AL运算改成全局W8写。

状态4/5/6各自更新、排序、渲染次序保持，不能统一；状态5的M队列与C同原W32复制/next门禁次序，scanOffset=2、state6，空队列就绪Hint保持。状态7先activeMask=0后exit=1，再Update→43BAA0→43BED0，清22角色表并按原音频调用；状态9先exit=1后activeMask=0，调用链没有43BED0。两退出门禁逐次读取四个W16并保短路；首W16条件必须在mode=2发布前保存。原重复四W16零写/46E4C0保留，状态7按state读→首W16零→S32+1→其余三W16零→58B6D0=1→state发布，随后fallthrough8。

状态8输入/转场选择14或12，成功后503E80→44D490→scene9/1→YobiClear→446610，构造0值载体，先取maskW32/encodedW32/readCountW8，再依次发布到ripple，析构后5108F0(socket,0)，立即返回。状态9清理后分别对ripple与transition各按同三取三发复制，再SetPendingSceneType(7)返回。16B内存数值载体不是网络加解密；token/reserved、构造析构和随机消费保持，不宣逐栈ABI同形。

公共尾mode=2先返回；按currentPosition分0..9、10..19或0..19，借用每槽同一actor，原动作/参数与batch308W8、flag24W8位门禁保持，调用后复读并递增全局index，再复读mode；随后46E4A0和退出请求转state7。exitAction空判及HP>0使用同一次取样的非拥有指针，HP既有ReadOnce保单次有符号读取，对应原Greater(0)；条件后仍重新读取全局action做hover，绘制前再读全局action。hint整字节组复制→点击W8取样/位条件→尾字节→同条件音效和原明文SendBattlePacket请求→state7，绘字坐标W16边界保持。

<a id="battle-event-finish-scene-lifecycle-sampling-decision"></a>
## 十八项差异裁决

| 类别 | 原端 / 实施 |
| --- | --- |
| 完成函数三项、四处 | 25头索引恢复S32；两22轮先A8取样再bit；退休先slot bit→活动mask读取→取反AND/pub，之后slot复读清表。保持消息数先减/pub后门禁。 |
| 场景字节与索引 | 标题S32及命中条件先于属性清零；玩家两W8、ParseC后字段W8、点击W8用unsigned char读取对象字节，不以读W32再截断替代。 |
| C/M复制 | 两处原计数W32，指针保持原生地址；next比较先于readIndex发布及尾copy，随后消费同保存条件。仅正常合同，不引溢出/回绕或容量保护。 |
| 两退出门禁 | 根内memcpy读取原W16，保各项短路和首条件先于mode发布，不引uint16_t指针别名。状态7首零写先于state增量，其余原零写及fallthrough保留。 |
| 数值载体三处 | 三成员先全部读取，再按mask/encoded/readCount发布；不换成全16B复制、不改token/reserved/SDK构造析构或随机消费。 |
| notice/退出指针/点击 | notice AND先于槽清；退出HP门禁同指针但条件后live复读不删除；点击位条件先于hint尾copy。 |

共两根18项逻辑修复/22处替换（Finish3/4、Scene15/18）；根内W16读取lambda仅现代承载，不造原F/X身份。共有声明头只更正已具名43B610工厂过期注释，声明、布局与公开头保持。原已正确分支、失败/返回、重复零写、旧helper和全部其它源码正文保持；不加guard/补零/钳制/拒绝或故障模型，不声称编译器访问指令已验证。

<a id="battle-event-finish-scene-lifecycle-sampling-acceptance"></a>
## 可度量验收

| 项 | 本步标准 |
| --- | --- |
| 原根 | 850条3188B完整区间、12指令页/3引用页连续完整；95call/5父/139出向，三直接表22槽88B可追。 |
| 实现 | 从HEAD精确重放18项22处；两根正常取样宽度/读取发布次序及完成/清理/重进/退出接线有限静态；共有头只有具名注释更正。 |
| 台账 | 旧803历史追加、新场景一F与两adapted；YOBI/BATTLE两根，LOOP仅场景；共六局部I/S，不重复语义或关联计数。 |
| 验证 | 必要静态与获准文本汇总/Check，不编译、测试、运行或计算摘要；所有完整运行门禁保留。 |

<a id="battle-event-finish-scene-lifecycle-sampling-source-state"></a>
## 当前状态与后续

两根差异落码，事件完成→Init/退休及十状态生产/消费、Yobi清理和退出明文请求接线纳管。完整427470事件分派、全部资源owner重进/退出和借用失效、五父完整消费、SDK/ABI/PC53/时钟RNG/画面仍需继续开发与相应验收。本步不恢复暂停音频；七任务及R1/R7/R8和整体目标保持open。

语义1349→1350/有效ID1344→1345、adapted454→456；YOBI53→55/BATTLE50→52、LOOP31→32，E337→343仅三域六局部I/S。文档关联1462→1463/3680=39.76%，是关联覆盖率，不是完成率。代码片段5、R7区间67/3061、队列175、60任务及已闭10和全部完整门禁保持。本组16文件，固定快照：`cg2d-re-battle-event-finish-scene-lifecycle-sampling-20261006`。
