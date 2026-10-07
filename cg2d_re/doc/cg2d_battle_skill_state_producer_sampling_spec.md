# BQR02 技能引用计数、选择与动作状态生产SPEC

2026-10-07。沿[BQQ01关闭回调](cg2d_battle_window_release_range_sampling_spec.md#battle-window-release-range-sampling-source-state)和[BQP01回退/列表](cg2d_battle_skill_lists_rollback_sampling_spec.md#battle-skill-lists-rollback-sampling-source-state)真实依赖推进。按AGENTS.md第8条工程委托，先记[十一完整原根与源差异](cg2d_battle_skill_state_producer_sampling_evidence.json)裁决，再修源；原事实不猜。BQR01已用于队列就绪，本步编号BQR02。

<a id="battle-skill-state-producer-sampling-baseline"></a>
## 完整基线与具体差异

| 原入口 | 新源符号（cpp） | 指令/实际B/页 | call/jump/ret | 处理 |
| --- | --- | --- | --- | --- |
| 479640 | WindowRuntime#Sub_479640 | 2/11/1 | 0/0/1 | W32计数清0正确保 |
| 479650 | WindowRuntime#Sub_479650 | 2/7/1 | 0/0/1 | 同W32计数增1正确保 |
| 4796C0 | WindowRuntime#Sub_4796C0 | 2/8/1 | 0/0/1 | W8计数清0正确保 |
| 4796D0 | WindowRuntime#Sub_4796D0 | 2/7/1 | 0/0/1 | 同W8计数增1正确保 |
| 4796F0 | WindowRuntime#Sub_4796F0 | 2/8/1 | 0/0/1 | 另一W8引用清0正确保 |
| 47A140 | WindowRuntime#Sub_47A140 | 30/88/1 | 1/3/1 | 技能回复held真假及原fS参数正确保 |
| 47A540 | WindowRuntime#Sub_47A540 | 16/52/1 | 1/1/1 | mode5/callback/借窗口/创建返回正确保 |
| 47A580 | WindowRuntime#Sub_47A580 | 16/52/1 | 1/1/1 | 独立mode6入口正确保 |
| 51BEC0 | SceneState#Sub_51BEC0 | 7/20/1 | 0/0/1 | 入口W8读修复 |
| 51BE50 | SceneState#Sub_51BE50 | 31/102/1 | 4/5/1 | 入口W8读及发布W8/W16修复 |
| 4410E0 | SceneState#Sub_4410E0 | 2/6/1 | 0/0/1 | 原动作范围MOV EAX,11h正确保 |

十一原根112条361B/十一页、七直接CALL/五callee、十普通jump/十一RET。全部原函数确切边界与单chunk已证，不涉及新R7。71代码入向/25有IDA父只登记；4856CF/4856E5两无父CALL来自刚纳管的BQQ01关闭代码，不伪造F或留下未识别来源。

| 实际内存差异 | 原指令/字节 | 裁决 |
| --- | --- | --- |
| 状态读取 | 51BEC0与51BE50均A0 8EA8FD00，即FDA88E W8 | 从现flags18C字节+2取W8，保入口缓存/之后value190 W32及条件 |
| 发送后置位 | 51BE97 80 0D 8EA8FD00 02，W8 OR2 | 原同地址W8读改写，不再整W32 OR20000 |
| 动作0清位 | 51BEA5 66 81 25 8EA8FD00 FDFF，W16 ANDFFFD | fresh同地址W16取样/清位/写回，以2B memcpy避免宽指针别名 |

两体三源片段对应四原内存点。普通值同旧源；恢复原宽度与别名合同，不宣异常、并发、机器故障或数值错误。flags18C仍是承载[FDA88C,FDA890)的共享W32，value190仍独立S32，不拆影子状态，不修改其它同存储消费者或既有公开接口。

<a id="battle-skill-state-producer-sampling-decision"></a>
## 正常生产、消费与裁决

1. 五计数叶分别清/增D67AF8 W32、清/增D67AFC W8、清D67AFD W8；保现正常S32/W8储存，原RET没有显式EAX返回，不造固定成功。D67AF8四原引用只涉及这两叶和已过479660；D67AFC五引用还含已过4796E0及47B210；D67AFD35引用跨多个窗口。本步只认已列清增叶/实际引用，不将所有引用父计为完整生产/寿命验收。
2. 47A140：先借window10判非空，再fresh mode2；held skill index用于rebirth W32读取，之后enabled W8转真假。真假不同才held index加0或100→fresh socket→510F50，弃返。不能在发送参数阶段fresh enabled/index或改正负范围；同事实按原普通S32表达式保留。
3. 47A540/47A580：入口callback参数已held，先mode W32发布5/6，再CC4D14发布非拥有callback T*，随后fresh window10 T*。非空只将window.state W32清8000；原AND CH仅寄存器消费，内存不是W8。调用475AA0(10,1,0,nullptr)并透传原Yobi T*结果，空创建结果不变。通用动画工厂前批已过，本步不重验。
4. CC4D14实际两写47A54E/47A58E、四读47CA99/47CD6E/47CD94/47D074，三消费者原父47C950/47CC80/47CFE0。数据xref的iscode=false表示数据关系，来源指令均真实代码；47CD94是直接间接CALL [CC4D14]。现WindowValueCallback、三个现消费者借同slot；此处只登记slot生产和实际消费地址，不声称三个完整consumer/所有选项逻辑或owner寿命通过。
5. 51BEC0：flags W8入口缓存后value190 W32也取一次，再返回bit1设时同value，否则0。源保该先后，不把value取样放到分支内。
6. 51BE50：入口W8 flags缓存；bit1设且value190等requested去重，返入口flags低S8。否则原负值短路，不执行4410E0；非负值实际>=0x11归0。4410E0完整原体MOV EAX,11h/RET已证，当前常量来源正确。
7. 发送先held动作值，再Y对象Read401EC0、X对象Read401EC0各一次、fresh socket调用511E60(socket,x,y,value)。原GetVal401EA0真实尾别名401EC0已在BQP01通过，不换解码/计数RNG，不删除弃返读取。发送后fresh FDA88E W8 OR2；held value=0才再fresh同地址W16 ANDFFFD；最后value190 W32发布，保原网络返回低S8，不把回包结果/更新后的flags当入口去重返回。
8. 原510F55的fS含NUL为665300（58B238），511E65的klF含NUL为6B6C4600（58B2E0），只核十条原前缀/格式推送，不据前缀验收完整封包叶。现两封包体保持原一/三整数次序、各次capacity/buffer读取与明文传送；本步不改协议/加解密/网络源码。

<a id="battle-skill-state-producer-sampling-acceptance"></a>
## 可度量验收

- 十一原根逐指令连续实际字节/确切首尾与单chunk、112/361B/十一页、七CALL五callee/十分支/十一RET。
- 十一完整源体冻结；仅SceneState.cpp两体三片段，WindowRuntime.cpp八正确体及范围getter正确体保持。四W8/W16点、共享W32边界、发送前后时序、入口/网络低S8返回和原短路对应。
- 五计数清增、47A140 held参数/真假、两宠物selector mode/callback/借窗口与透传结果、六callback原读写点/typed声明和真实下游调用对照。原四/五/35计数引用、71入向只登记不当完整父。
- 原两个格式字节与源码封包保持；现网络/编码/解码体及SDK接口不改，不以本批扩排网络或验收全接线。
- 新十一正常语义/equivalent：1543..1553；语义1554/有效F1547、关联1665/3680（45.24%）、分类678/BATTLE230枚举open/E788；无F/有效区间各7、R7 264/3061余2797保持。
- 两局部I/S、四任务U08/R01/R04/V04只增当前与引用仍open；其它60任务/完整门禁不增。定向静态、measure/-Check、完整派生快照增量及编码/引用/diff通过后，精确12文件提交。

当前验收限正常源与原字节静态对应。其它consumer/计数生产者、完整数据容量/窗口/owner寿命、SDK/机器ABI/画面运行与R8继续逐项完成；音频暂停、不构建测试运行摘要及RPLAN.24限制保持，不降验收标准。

<a id="battle-skill-state-producer-sampling-source-state"></a>
## 当前交付

先落本SPEC与原证据，修两动作状态体三片段，纳管九正确体；追加十一原正常身份/处置、四任务及唯一进度后生成看板。固定快照`cg2d-re-battle-skill-state-producer-sampling-20261007`。不会据静态关联、BATTLE成员数或局部I/S声明完整战斗/制造/整体SPEC完成。
