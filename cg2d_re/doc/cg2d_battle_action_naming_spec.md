# 战斗动作命名（BAN01）

> 2026-10-25 补充范围：结合当前 cg2d.exe 会话、cgmsv_server 源码及用户指定 gmsv/data，继续核对剩余114项，覆盖角色phase的0～256全部257项。用户明确允许未证业务名的项按已证行为命名，例如PlayAnimation13AndResetFrames；动画号、工厂type及匿名标志位直接保留在名称或注释中，不据图号猜技能名称。下文旧“未命名”清单为本次补充前的历史状态。

本次保持BattleActionId及字段、参数的有符号整数承载；仅扩充枚举、替换已确认属于角色动作域的数字引用及更新说明。角色动画、技能号、效果工厂type、场景phase、骑手／特效自身phase和位掩码各自保留。原端数据库、服务端与游戏数据只读。不编译、调试、运行或新增测试；完成后通过数值逆替换核对源码词法一致性并精确提交。

日期：2026-10-21。用户要求将141、142等动作号按作用命名，随后要求继续尝试其他动作；沿用中文注释、英文标识符规则。

## 范围与依据

在既有battle_base.hpp的cg::yobi::detail定义有符号BattleActionId枚举，覆盖逐项读取实现确认的143个动作。替换AppendBattleCast_4392A0的动作参数（包括三元分支值）、battle_actor.cpp的phase分派case、已核实角色动作处理函数内的直接phase跳转／比较及castListCA4动作值；相同数值的音效号、字段偏移、角色动画动作号、其他对象phase及条件中的位掩码不替换。

| 动作号 | 标识符 | 当前实现作用 |
| --- | --- | --- |
| 141 | BattleAction_ApplyDebuff | UpdateBattleAction141_410E90按模式、类型及跳舞等技能特例选图，调用NewBattleDebuffEffect_41F660；常驻槽为effect1054，普通异常会设置相应FDC位，扩展2089／2090也使用此动作。 |
| 142 | BattleAction_ClearDebuff | UpdateBattleAction142_410F70标死effect1054、清槽及异常位；播放并推进后，重新取载荷，将debuffType13D0回写至statusEffectType13DC并重置模式。 |
| 143 | BattleAction_ApplyStatusEffect | UpdateBattleAction143_410FC0为能力变化或扩展状态选择图号并创建effect1068；包含能力下降，不命名为ApplyBuff。 |
| 144 | BattleAction_ClearStatusEffect | UpdateBattleAction144_411240标死effect1068、清槽及能力变化位，然后播放并推进；不解释为清除全部角色状态。 |

依据为battle_cast.cpp上述四个处理函数、battle_actor.cpp动作分派，以及ANR／PUE和技能生产入口的调用。原函数名称及地址保留，枚举仅表达动作队列中的整数编号。

## 保持的行为

- 队列、phase、函数参数及结构字段保留原整数类型；不改签名、布局、回调或执行顺序。
- 保留141／143的条件创建、睡眠／石化播放门禁，以及142的推进后状态回写；命名不表示无条件创建成功。
- 不改变ANR／PUE的模式解释、技能成功判定及状态类型宏；其他动作只按已读实现命名，不猜测未确认的角色动画或渲染字段含义。
- 保留并排除工作区既有battle_effect.cpp与battle_skill_status.cpp的减益模式宏改动；共享文件仅提交本次动作号替换。

## 其他已确认动作

用户要求继续命名后，逐项读取下列函数实现，将行为概括为枚举名；保留函数原名及地址，不重构调用链。等待动作的Ticks表示执行计数，不换算为毫秒。

| 编号 | 标识符 | 作用及实现来源 |
| --- | --- | --- |
| 0 | BattleAction_Idle | 待机；按死亡／睡眠／石化状态选择姿态，保留特殊队列166及技能72／113门禁。来源：battle_actor.cpp的UpdateBattleAction0_406650。 |
| 1 | BattleAction_WaitIdle | 保持待机姿态并播放；不自行推进动作队列。来源：battle_actor.cpp的UpdateBattleAction1_406720。 |
| 2 | BattleAction_EnterFormation | 移入阵型坐标；到达后转待机并设置入场完成位。来源：battle_actor.cpp的UpdateBattleAction2_406760。 |
| 6 | BattleAction_MoveToTargetPosition | 移动至moveTargetXFE8／YFEC；到达后推进动作。来源：battle_actor.cpp的UpdateBattleAction6_406CD0。 |
| 25 | BattleAction_WaitTargetReady | 等待当前效果目标待机并保存其坐标；目标缺失或为自身时按原条件推进。来源：battle_attack.cpp的UpdateBattleAction25_406BA0。 |
| 36 | BattleAction_Wait30Ticks | valueFF0递增至30后清零并推进；保留睡眠／石化播放门禁。来源：battle_attack.cpp的UpdateBattleAction36_40BAD0。 |
| 37 | BattleAction_Wait60Ticks | valueFF0递增至60后清零并推进；不是60毫秒。来源：battle_attack.cpp的UpdateBattleAction37_40BB20。 |
| 38 | BattleAction_Wait120Ticks | valueFF0递增至120后清零并推进；不是120毫秒。来源：battle_attack.cpp的UpdateBattleAction38_40BB70。 |
| 40 | BattleAction_FaceFormation | 按阵型位置恢复朝向后推进。来源：battle_attack.cpp的UpdateBattleAction40_40BD40。 |
| 47 | BattleAction_MarkDead | 设置角色FDC倒地／死亡位后推进，不销毁对象。来源：battle_cast.cpp的UpdateBattleAction47_40B3F0。 |
| 51 | BattleAction_RemoveFromBattle | 清空战斗角色全局槽并MarkDeath；实际释放沿调度器。来源：battle_cast.cpp的UpdateBattleAction51_40BE70。 |
| 55 | BattleAction_ConsumeFp | 播放一帧、扣除fpUsed1008指定消耗，再推进。来源：battle_cast.cpp的UpdateBattleAction55_40C0C0。 |
| 64 | BattleAction_SetIdlePose | 将角色动画动作设为0并推进；不同于持续待机动作0。来源：battle_cast.cpp的UpdateBattleAction64_40CBB0。 |
| 66 | BattleAction_PlayConfiguredImage | 切换为image1014及角色动作5，播放结束后推进并清帧事件。来源：battle_cast.cpp的UpdateBattleAction66_40C480。 |
| 73 | BattleAction_AdvanceLinkedCast | 先推进当前效果目标的动作，再推进本角色动作。来源：battle_base.cpp的AdvanceBattleLinkedCast_40CAC0。 |
| 74 | BattleAction_AdvanceEffectAndCast | 先推进当前效果游标，再推进本角色动作。来源：battle_base.cpp的AdvanceBattleEffectAndCast_40CAF0。 |
| 75 | BattleAction_EnableFlash | 将flash1004设为1后推进。来源：battle_cast.cpp的UpdateBattleAction75_40CB10。 |
| 76 | BattleAction_DisableFlash | 将flash1004清零后推进。来源：battle_cast.cpp的UpdateBattleAction76_40CB30。 |
| 79 | BattleAction_ResetDeathState | 清理中心／再生附属效果，恢复待机姿态并清除死亡位及stateFE0的2位。来源：battle_cast.cpp的UpdateBattleAction79_40CBD0。 |
| 80 | BattleAction_PlaySound1018 | 播放soundId1018，保留先读取X再读取音效号的顺序。来源：battle_cast.cpp的UpdateBattleAction80_40CC80。 |
| 81 | BattleAction_PlaySound101C | 播放soundId101C；与1018／1020是独立音效槽。来源：battle_cast.cpp的UpdateBattleAction81_40CCB0。 |
| 82 | BattleAction_PlaySound1020 | 播放soundId1020；不合并其他音效槽。来源：battle_cast.cpp的UpdateBattleAction82_40CCE0。 |
| 88 | BattleAction_StartScreenShake | 设最大振幅6并开启振幅上升，然后推进。来源：battle_cast.cpp的UpdateBattleAction88_40C8D0。 |
| 89 | BattleAction_EndScreenShakeRise | 结束振幅上升；后续主循环逐步减振幅，不立即归零。来源：battle_cast.cpp的UpdateBattleAction89_40C8F0。 |
| 96 | BattleAction_ShowEffectTarget | 将当前效果目标hidden置0后推进本角色。来源：battle_cast.cpp的UpdateBattleAction96_40D170。 |
| 97 | BattleAction_HideEffectTarget | 将当前效果目标hidden置1后推进本角色。来源：battle_cast.cpp的UpdateBattleAction97_40D1A0。 |
| 99 | BattleAction_SetAnimationRate25 | 播放速率设为25，推进后清frameIndex／frameRemain；单位不外推。来源：battle_cast.cpp的UpdateBattleAction99_40D390。 |
| 107 | BattleAction_MarkForRemoval | 直接MarkDeath当前对象；不清战斗全局槽，区别于51。来源：battle_cast.cpp的UpdateBattleAction107_40DF70。 |
| 145 | BattleAction_CreatePrayerField | 按四属性祈祷技能创建场地效果，随后推进。来源：battle_cast.cpp的UpdateBattleAction145_411290。 |
| 147 | BattleAction_ToggleAttributeReversal | 按ReverseType位创建或清除反转效果105C，同时切换该状态位。来源：battle_cast.cpp的UpdateBattleAction147_411320。 |
| 159 | BattleAction_ApplyResourceChange | 按numberDisplayType1034显示数值并更新HP／FP，随后推进动作队列。来源：battle_status.cpp的UpdateBattleAction159_4107C0。 |
| 160 | BattleAction_ApplyResourceChangeAndWait | 同159处理HP／FP，但只转动作37等待，不立即推进队列。来源：battle_status.cpp的UpdateBattleAction160_410B00。 |
| 162 | BattleAction_WaitEffectComplete | 等待effectPlaybackState1024为2，再清标记、显示角色并推进。来源：battle_status.cpp的UpdateBattleAction162_410E30。 |
| 171 | BattleAction_FadeIn | 递增角色及附属对象透明度至255后推进，保留设备绘制位处理。来源：battle_status.cpp的UpdateBattleAction171_412020。 |
| 174 | BattleAction_SelectPalette | 用当前效果的position／position3切换调色板，推进效果及动作。来源：battle_status.cpp的UpdateBattleAction174_4121D0。 |
| 175 | BattleAction_StartBackgroundFade | 创建或复用effect1060背景渐变对象并将其phase设为0，然后推进。来源：battle_status.cpp的UpdateBattleAction175_412210。 |
| 176 | BattleAction_EndBackgroundFade | 将effect1060的phase设为2并清角色槽；不是直接MarkDeath。来源：battle_status.cpp的UpdateBattleAction176_412260。 |
| 177 | BattleAction_PlayBackgroundFade | 创建不保存到effect1060的背景渐变，工厂末参1，然后推进。来源：battle_status.cpp的UpdateBattleAction177_412290。 |
| 178 | BattleAction_AdvanceAndIdle | 先推进队列游标，再将角色phase设为待机0。来源：battle_status.cpp的UpdateBattleAction178_4122C0。 |
| 206 | BattleAction_FaceEffectTarget | 朝当前效果记录的目标角色转向后推进。来源：battle_status.cpp的UpdateBattleAction206_4127F0。 |
| 207 | BattleAction_MoveToEffectPosition | 从效果position／flags读取目的坐标，推进效果后切入移动动作6。来源：battle_status.cpp的UpdateBattleAction207_412830。 |
| 253 | BattleAction_CreateCastingEffect | 本动作且播放标记为0时创建type43施法效果并保存battleCastEffect10，然后推进。来源：battle_cast.cpp的UpdateBattleAction253_40FE50。 |
| 254 | BattleAction_ClearCastingEffect | 标死battleCastEffect10并清槽／播放标记，然后推进。来源：battle_cast.cpp的UpdateBattleAction254_40FEB0。 |
| 256 | BattleAction_RequestBattleExit | 清事件分派忙标志并请求退出战斗；不在这里销毁角色。来源：battle_attack.cpp的UpdateBattleAction256_418B50。 |

未命名项仍保留数字，并在枚举旁记录原因：攻击／受击／复制体等组合流程尚未逐项归纳；77／78及167～170只有绘制位或匿名字段证据，不能猜具体视觉含义；56～62、106、179、205等尚未确认角色动画的业务名称。

## 83～87补充分析

用户明确83为DamageUp，复核battle_cast.cpp五个处理函数及入队调用，确认其与84成对，而85～87是另一组表现／结算动作。

| 动作号 | 名称 | 行为与来源 |
| --- | --- | --- |
| 83 | BattleAction_ApplyDamageUp | 每次将scaleX／Y增加0.005，使用增加后的未舍入X与1.3F比较；达到阈值后先推进，再重新取载荷设置DamageUp位。ProcessBattleMSkill98Mode6安排此动作。这里没有计算伤害倍率。 |
| 84 | BattleAction_ClearDamageUp | 每次缩小0.01，达到1.0时将两轴固定为1.0；先推进，再清DamageUp位。PDN消息安排此解除动作，保留睡眠／石化播放门禁。 |
| 85 | BattleAction_SpinAndEnlarge | 每次放大0.01并旋转一个方向，达到2.0阈值时创建特效再推进；技能2002按角色名称表选择特效，否则使用110154。未设置DamageUp位，不能按83的含义命名。 |
| 86 | BattleAction_ApplyDamageToTargets | 遍历已有目标效果列表：显示并转向目标、消费命中标志，处理死亡／解除睡眠／免疫，飘字并扣HP，推进目标动作；末尾启动震屏并推进自身。伤害数值来自已有记录，不重新判定成功率。 |
| 87 | BattleAction_PlayEarthquakeEffects | ProcessBattleMSkill97（大地之怒）安排的视觉阶段：在对方十个阵型点随机创建110006／110007图像及音效263，计数达到270后推进；没有扣血操作，随后安排86结算伤害。 |

85由技能82、跳舞mode12及技能等级索引90的复制体分支共用，名称只概括已证的旋转放大行为，不推测统一技能业务。86由技能82／97及跳舞mode12共用，不限定为大地之怒专属。原放大阈值、计数、浮点计算顺序、随机调用及目标重复取样保持不变。

## 其余动作继续核对（2026-10-22）

本批新增59项行为命名；名称只描述已核实的动作合同，不把队列动作等同于技能、角色动画或特效工厂type。差异包括：46／48／49的移除链与213／214／215的继续队列链；172每次淡出3与173每次淡出2；241逐个延迟发射与242一次发射全部目标。

| 动作号 | 名称 | 已核实作用／来源 |
| --- | --- | --- |
| 3 | BattleAction_ApproachEffectTarget | 朝当前效果目标移动，距离小于70后推进；速度8，角色动画1。来源：battle_actor.cpp的UpdateBattleAction3_406C30。 |
| 4 | BattleAction_CloseInOnEffectTarget | 朝当前效果目标贴近，距离小于8后推进；速度8，角色动画3。来源：battle_actor.cpp的UpdateBattleAction4_406D70。 |
| 5 | BattleAction_MoveForward16Ticks | 沿当前角度移动；旧计数小于16时移动，否则推进并清计数。来源：battle_actor.cpp的UpdateBattleAction5_40CFA0。 |
| 11 | BattleAction_ReturnToFormation | 返回阵型x00／y04；保留骑乘死亡短路、死亡动画及转入46的条件。来源：battle_actor.cpp的UpdateBattleAction11_4067F0。 |
| 17 | BattleAction_MoveOutOfBounds | 继续移动，超出原屏幕扩展边界后MarkDeath；不重命名为立即销毁。来源：battle_attack.cpp的UpdateBattleAction17_4093B0。 |
| 24 | BattleAction_Wait21Ticks | 递增计数至大于20后清零并推进；不播放帧。来源：battle_attack.cpp的UpdateBattleAction24_409380。 |
| 41 | BattleAction_FaceTargetFormationPoint | 面向targetPosition100C对应的阵型静态坐标后推进。来源：battle_cast.cpp的UpdateBattleAction41_40BD60。 |
| 42 | BattleAction_FaceSelectedTarget | 面向targetPosition100C对应角色的实时坐标后推进。来源：battle_cast.cpp的UpdateBattleAction42_40BDB0。 |
| 43 | BattleAction_TrackOpponentFacing | 持续面向battleOpponent10并播放待机；自身不推进队列。来源：battle_cast.cpp的UpdateBattleAction43_40BE30。 |
| 44 | BattleAction_TurnTargetTowardSelf | 令当前效果目标面向本角色，随后推进本角色动作。来源：battle_cast.cpp的UpdateBattleAction44_40BDF0。 |
| 45 | BattleAction_SetHpZero | 将本角色HP置0后推进；不在此设置死亡位。来源：battle_cast.cpp的UpdateBattleAction45_40B130。 |
| 46 | BattleAction_HandleDeath | 死亡动画完成后清理效果并分流；保留再生、骑乘、外观覆盖及玩家／宠物差异。来源：battle_cast.cpp的UpdateBattleAction46_40B200。 |
| 48 | BattleAction_FadeOutForRemoval | 按后端执行闪烁／渐隐退场，最后转49或51移除；与214的推进收尾不同。来源：battle_cast.cpp的UpdateBattleAction48_40B490。 |
| 49 | BattleAction_WaitRemovalEffect | 等待播放状态2，清标记后转51移除；区别于215推进队列。来源：battle_cast.cpp的UpdateBattleAction49_40B5B0。 |
| 50 | BattleAction_PlayDeathAnimation | 播放角色死亡动画10，终帧后推进；不执行46的清理分流。来源：battle_cast.cpp的UpdateBattleAction50_40B3C0。 |
| 52 | BattleAction_DismissPlayerPet | 玩家离场时给其宠物安排13／47／51并启动；最后推进玩家动作。来源：battle_cast.cpp的UpdateBattleAction52_40BE90。 |
| 53 | BattleAction_ShrinkOut | 每次缩小0.02并创建type22效果；缩至0后清施法效果并推进。来源：battle_cast.cpp的UpdateBattleAction53_40BF20。 |
| 54 | BattleAction_GrowIn | 每次放大0.02至1并创建type23效果；恢复阵型朝向和绘制位后推进。来源：battle_cast.cpp的UpdateBattleAction54_40BFE0。 |
| 67 | BattleAction_TriggerTargetsOnFrameEvent | 帧事件到达时启动目标动作并推进效果游标；自身动画结束后推进。来源：battle_cast.cpp的UpdateBattleAction67_4106B0。 |
| 68 | BattleAction_ShowReactionIcon | 用reactionImage1010创建浮动图标，条件播放后推进。来源：battle_cast.cpp的UpdateBattleAction68_40C0E0。 |
| 69 | BattleAction_ShowReactionHitEffect | 用reactionImage1010创建30帧命中特效，条件播放后推进。来源：battle_cast.cpp的UpdateBattleAction69_40C120。 |
| 70 | BattleAction_FireSpiracleShots | 帧事件触发时逐目标创建气功弹，消费效果列表并播放音效254。来源：battle_attack.cpp的UpdateBattleAction70_4070E0。 |
| 71 | BattleAction_StartEffectTargets | 无帧事件门禁，逐目标启动动作并推进效果游标，再推进自身。来源：battle_cast.cpp的UpdateBattleAction71_40C4D0。 |
| 93 | BattleAction_ReleaseUpdateGate | 按活动条目数决定清全局更新门禁，清aliveFF8后推进；不删除角色。来源：battle_cast.cpp的UpdateBattleAction93_40CF70。 |
| 94 | BattleAction_OffsetFromEffectTarget | 从当前目标坐标出发沿其反向角度移动50，再面向目标并推进。来源：battle_cast.cpp的UpdateBattleAction94_40D010。 |
| 95 | BattleAction_RushAndZeroTargetHp | 沿当前角度移动16次后推进，并将效果目标HP置0；保留原写入顺序。来源：battle_cast.cpp的UpdateBattleAction95_40D0B0。 |
| 101 | BattleAction_MoveToSideCenter | 按目标阵营移至固定中央坐标，抵达后调整朝向、清计数并推进。来源：battle_cast.cpp的UpdateBattleAction101_40D580。 |
| 104 | BattleAction_ShowInspectionResults | 向当前玩家显示目标生命／魔力或调查失败消息；保留原CryptInt字节消费。来源：battle_cast.cpp的UpdateBattleAction104_40DC90。 |
| 105 | BattleAction_StepAroundEffectTarget | 按计数分段切换目标上下左右位置；旧计数到120后推进自身及对应目标。来源：battle_cast.cpp的UpdateBattleAction105_40DE40。 |
| 113 | BattleAction_MoveAlongArc | 按二次曲线调整显示高度并同步附属对象；到期定位目标坐标、推进并标死附属对象。来源：battle_cast.cpp的UpdateBattleAction113_40EFE0。 |
| 166 | BattleAction_ConsumeQueuedTime | 当前玩家消费排队时间值，显示负时间数字并推进排队值及动作。来源：battle_status.cpp的UpdateBattleAction166_411CB0。 |
| 172 | BattleAction_FadeOutStep3 | 角色及附属对象透明度每次减3；归零后设置隐藏相关位并推进。来源：battle_status.cpp的UpdateBattleActions172_173_4120F0。 |
| 173 | BattleAction_FadeOutStep2 | 角色及附属对象透明度每次减2；与172保持不同速度。来源：battle_status.cpp的UpdateBattleActions172_173_4120F0。 |
| 208 | BattleAction_MoveBesideEffectTarget | 按目标屏幕分区设置偏移落点，再转动作6移动，不直接推进队列。来源：battle_cast.cpp的SetBattleAction208Target_412870。 |
| 210 | BattleAction_RushWithTrail | 以速度16接近效果目标并生成随机尾迹；距离小于70后恢复速度8及播放率。来源：battle_cast.cpp的UpdateBattleAction210_412F70。 |
| 211 | BattleAction_ApplySceneEffects | 按效果flags四个位写入两个场景效果全局的各16位分量，推进效果及动作。来源：battle_cast.cpp的ApplyBattleSceneEffects_413070。 |
| 212 | BattleAction_RepeatOffsetEffect | 反复创建指定图号／XY偏移效果；计数达到记录flags阈值后推进效果及动作。来源：battle_cast.cpp的UpdateBattleAction212_413110。 |
| 213 | BattleAction_HandleDeathAndContinue | 死亡动画后清理常驻效果，外观恢复或渐隐后继续队列；保留再生分流。来源：battle_cast.cpp的UpdateBattleAction213_4131B0。 |
| 214 | BattleAction_FadeOutAndContinue | 按后端闪烁／渐隐，结束后推进队列；不同于48进入角色移除。来源：battle_cast.cpp的UpdateBattleAction214_4132B0。 |
| 215 | BattleAction_WaitFadeEffectAndContinue | 等待播放状态2并清标记后推进队列；不转51移除角色。来源：battle_cast.cpp的UpdateBattleAction215_4133D0。 |
| 219 | BattleAction_RestoreAppearanceAfterDeath | 恢复基础外观后按玩家／宠物分类继续死亡流程，非玩家宠物转48。来源：battle_cast.cpp的UpdateBattleAction219_413750。 |
| 220 | BattleAction_RestoreAppearanceBeforeFade | 恢复基础外观完成后转214渐隐继续流程。来源：battle_cast.cpp的UpdateBattleAction220_413980。 |
| 223 | BattleAction_UpdatePetSnapshot | 按效果position设置宠物快照索引；-2时同步状态UI并清快照，再推进。来源：battle_cast.cpp的UpdateBattleAction223_413FE0。 |
| 228 | BattleAction_StartAnimationSlowdown | 清计数，转229并立即执行首次减速动画更新。来源：battle_ride.cpp的UpdateBattleAction228_416000。 |
| 229 | BattleAction_AnimateSlowdown | 角色动画3播放率按250-4*计数递减；计数60后清零并推进，不改变移动速度。来源：battle_ride.cpp的UpdateBattleAction229_416020。 |
| 230 | BattleAction_MoveAlongEffectPath | 按效果position／flags中的XY坐标转向移动；距离小于8后落点并推进效果及动作。来源：battle_ride.cpp的UpdateBattleAction230_4160A0。 |
| 231 | BattleAction_DecelerateWithTrail | 每次速度减0.45至0并产生交替尾迹；停下后推进并恢复速度6。来源：battle_ride.cpp的UpdateBattleAction231_416150。 |
| 235 | BattleAction_ApplyEffectFlags | 按本角色位置调用ApplyBattleEffectFlags，再推进动作。来源：battle_base.cpp的ApplyBattleCastEffects_416DB0。 |
| 236 | BattleAction_ResetBusyAndIdle | 清busyFD8并转待机，立即执行待机处理；不等同于只写phase=0。来源：battle_actor.cpp的UpdateBattleAction236_406630。 |
| 240 | BattleAction_PrepareArcShots | 创建施法效果并播放；帧事件到达后清效果、计数，转241发射。来源：battle_attack.cpp的UpdateBattleAction240_417AC0。 |
| 241 | BattleAction_FireArcShotsSequentially | 精神／混乱冲击波逐目标发射弧形弹道，目标间等待计数6，保留帧事件及音效时序。来源：battle_attack.cpp的UpdateBattleAction241_417B80。 |
| 242 | BattleAction_FireArcShotsTogether | 精神／混乱冲击波一次遍历全部目标发射弧形弹道，随后推进；无241的逐目标延迟。来源：battle_attack.cpp的UpdateBattleAction242_417CC0。 |
| 244 | BattleAction_MarkDeadAndAcquireGate | 设置死亡位、全局更新门禁和aliveFF8，再推进；不同于47仅置死亡位。来源：battle_attack.cpp的UpdateBattleAction244_418550。 |
| 245 | BattleAction_CreateTiledEffect | 按效果position／position3指定平铺类型和寿命，推进效果及动作。来源：battle_attack.cpp的UpdateBattleAction245_418580。 |
| 246 | BattleAction_SetSpeedFromEffect | 用效果position3乘0.01设置motionMagnitude54，再推进效果及动作。来源：battle_attack.cpp的UpdateBattleAction246_4185C0。 |
| 247 | BattleAction_SavePosition | 将当前浮点XY分别截断到载荷x10／y14，再推进。来源：battle_attack.cpp的UpdateBattleAction247_4185F0。 |
| 248 | BattleAction_ReturnToSavedPosition | 移回x10／y14保存坐标，距离小于5后定位并推进；不同于回阵型坐标。来源：battle_attack.cpp的UpdateBattleAction248_418620。 |
| 249 | BattleAction_FireBlastWaves | 帧事件触发时按技能／等级逐目标发射冲击波，播放音效268并清事件。来源：battle_attack.cpp的UpdateBattleAction249_4071C0。 |
| 251 | BattleAction_SetHpFpOne | 将本角色HP和FP同时置1后推进，不在此清除死亡状态。来源：battle_cast.cpp的UpdateBattleAction251_40B180。 |

## 特效动作继续补齐（2026-10-22）

本批新增31项。沿动作入口→NewBattleEffect_41CD00的type→技能生产方与完成回调核对，不根据图号单独推断技能业务。

- 工厂type7～12直接引用ReflectionPhysics／AbsorbPhysics／IneffectivePhysics及对应Magic技能，确认六类屏障触发特效；type5则是六类技能共用的施加特效，不能命名为某一种反射或吸收。
- battle_skill_status.cpp的状态技能生产方证实type4为异常受术、type13为持续回复受术、type25为异常恢复受术、type6为属性反转受术、type33为复活受术、type37为能力变化受术。成功位仍决定后续状态动作；播放受术特效不等于成功。
- type16由补血生产路径及工厂末尾SkillEffectKind::Heal替换分支双向确认；type13的替换分支为SkillEffectKind::Recovery。Heal与Recovery不合并。
- 普通工厂动作link=1并转162；效果结束通知播放状态2，162清通知／隐藏相关字段后推进。127～129使用link=0且立即推进，故采用Start前缀；phase-120的120是映射偏移，保留原值。
- 180～203中的对应type采用AndFadeIn后缀：转204后透明度每次加3；播放状态2时按结果免疫位显示图标，或按数字类型结算HP／FP，再进入60次计数；状态4且透明度255才推进。各分支是否实际修改数值取决于输入，不表示每种特效都会恢复HP。
- 对暂未查到独立业务生产方的同type入口，仅沿相同工厂分支与已证收尾命名，保留各自编号，不把它们合并为同一动作或技能。

| 编号 | 标识符 | 作用及边界 |
| --- | --- | --- |
| 119 | BattleAction_PlayDebuffSpellEffect | 播放异常魔法受术特效（工厂type4），转162等待；本动作不直接施加状态或结算数值。 |
| 120 | BattleAction_PlayBarrierSpellEffect | 播放物理／魔法反射、吸收、无效化共用受术特效（工厂type5），转162等待；本动作不直接施加状态或结算数值。 |
| 121 | BattleAction_PlayPhysicalReflectEffect | 播放物理反射特效（工厂type7），转162等待；本动作不直接施加状态或结算数值。 |
| 122 | BattleAction_PlayPhysicalAbsorbEffect | 播放物理吸收特效（工厂type8），转162等待；本动作不直接施加状态或结算数值。 |
| 123 | BattleAction_PlayPhysicalNullifyEffect | 播放物理无效化特效（工厂type9），转162等待；本动作不直接施加状态或结算数值。 |
| 124 | BattleAction_PlayMagicReflectEffect | 播放魔法反射特效（工厂type10），转162等待；本动作不直接施加状态或结算数值。 |
| 125 | BattleAction_PlayMagicAbsorbEffect | 播放魔法吸收特效（工厂type11），转162等待；本动作不直接施加状态或结算数值。 |
| 126 | BattleAction_PlayMagicNullifyEffect | 播放魔法无效化特效（工厂type12），转162等待；本动作不直接施加状态或结算数值。 |
| 127 | BattleAction_StartPhysicalReflectEffect | 创建物理反射特效（工厂type7、link=0）后立即推进，不等待效果结束。 |
| 128 | BattleAction_StartPhysicalAbsorbEffect | 创建物理吸收特效（工厂type8、link=0）后立即推进，不等待效果结束。 |
| 129 | BattleAction_StartPhysicalNullifyEffect | 创建物理无效化特效（工厂type9、link=0）后立即推进，不等待效果结束。 |
| 130 | BattleAction_PlayRecoveryEffect | 播放持续回复魔法受术特效（工厂type13），转162等待；本动作不直接施加状态或结算数值。 |
| 133 | BattleAction_PlayStatusRecoveryEffect | 播放异常恢复魔法受术特效（工厂type25），转162等待；本动作不直接施加状态或结算数值。 |
| 146 | BattleAction_PlayAttributeReversalEffect | 播放属性反转受术特效（工厂type6），转162等待；本动作不直接施加状态或结算数值。 |
| 148 | BattleAction_PlayReviveEffect | 播放复活受术特效（工厂type33）并转162等待；死亡位重置及HP恢复由后续动作处理。 |
| 149 | BattleAction_PlayHealEffect | 播放补血魔法受术特效（工厂type16），转162等待；本动作不直接施加状态或结算数值。 |
| 157 | BattleAction_PlayParameterChangeEffect | 播放能力变化魔法受术特效（工厂type37），转162等待；本动作不直接施加状态或结算数值。 |
| 180 | BattleAction_PlayRecoveryEffectAndFadeIn | 播放持续回复魔法受术特效（工厂type13），转204渐显、按结果结算HP／FP并等待；不同于动作130。 |
| 183 | BattleAction_PlayStatusRecoveryEffectAndFadeIn | 播放异常恢复魔法受术特效（工厂type25），转204渐显、按结果结算HP／FP并等待；不同于动作133。 |
| 184 | BattleAction_PlayHealEffectAndFadeIn | 播放补血魔法受术特效（工厂type16），转204渐显、按结果结算HP／FP并等待；不同于动作149。 |
| 189 | BattleAction_PlayDebuffSpellEffectAndFadeIn | 播放异常魔法受术特效（工厂type4），转204渐显、按结果结算HP／FP并等待；不同于动作119。 |
| 190 | BattleAction_PlayBarrierSpellEffectAndFadeIn | 播放物理／魔法反射、吸收、无效化共用受术特效（工厂type5），转204渐显、按结果结算HP／FP并等待；不同于动作120。 |
| 191 | BattleAction_PlayPhysicalReflectEffectAndFadeIn | 播放物理反射特效（工厂type7），转204渐显、按结果结算HP／FP并等待；不同于动作121。 |
| 192 | BattleAction_PlayPhysicalAbsorbEffectAndFadeIn | 播放物理吸收特效（工厂type8），转204渐显、按结果结算HP／FP并等待；不同于动作122。 |
| 193 | BattleAction_PlayPhysicalNullifyEffectAndFadeIn | 播放物理无效化特效（工厂type9），转204渐显、按结果结算HP／FP并等待；不同于动作123。 |
| 194 | BattleAction_PlayMagicReflectEffectAndFadeIn | 播放魔法反射特效（工厂type10），转204渐显、按结果结算HP／FP并等待；不同于动作124。 |
| 195 | BattleAction_PlayMagicAbsorbEffectAndFadeIn | 播放魔法吸收特效（工厂type11），转204渐显、按结果结算HP／FP并等待；不同于动作125。 |
| 196 | BattleAction_PlayMagicNullifyEffectAndFadeIn | 播放魔法无效化特效（工厂type12），转204渐显、按结果结算HP／FP并等待；不同于动作126。 |
| 201 | BattleAction_PlayAttributeReversalEffectAndFadeIn | 播放属性反转受术特效（工厂type6），转204渐显、按结果结算HP／FP并等待；不同于动作146。 |
| 202 | BattleAction_PlayParameterChangeEffectAndFadeIn | 播放能力变化魔法受术特效（工厂type37），转204渐显、按结果结算HP／FP并等待；不同于动作157。 |
| 204 | BattleAction_FadeInAndResolveEffect | 逐步渐显，特效完成后按结果显示免疫或结算HP／FP；再等待60次计数且完全不透明才推进。 |

### 仍保留编号的动作

原生分派共有257个动作入口，累计143项已命名；以下114个编号对应86个处理入口，保留编号并记录本轮能确认的边界。定位完成不等于业务已证；未完整核对的组合流程明确列为后续分析项，不宣称所有动作已完成业务还原。

| 编号 | 处理入口 | 保留原因 |
| --- | --- | --- |
| 7 | UpdateBattleAction7_409410 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 8 | UpdateBattleAction8_4094A0 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 9 | UpdateBattleAction9_409510 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 10 | UpdateBattleAction10_409610 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 12 | UpdateBattleAction12_406930 | 已确认x08／y0C目标与粒子、死亡分支，尚未核实该坐标在全部生产路径的业务角色。 |
| 13 | UpdateBattleAction13_406A80 | 已确认x08／y0C目标与粒子、死亡分支，尚未核实该坐标在全部生产路径的业务角色。 |
| 14 | UpdateBattleAction14_409680 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 15 | UpdateBattleAction15_406E10 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 16 | UpdateBattleAction16_4072E0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 18 | UpdateBattleAction18_408170 | 已读取行为，仍需联动核对命中／反弹／复制体或反应链，避免用单个副作用命名整个动作。 |
| 19 | UpdateBattleAction19_4082F0 | 已读取行为，仍需联动核对命中／反弹／复制体或反应链，避免用单个副作用命名整个动作。 |
| 20 | UpdateBattleAction20_408410 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 21 | UpdateBattleAction21_408750 | 已读取行为，仍需联动核对命中／反弹／复制体或反应链，避免用单个副作用命名整个动作。 |
| 22 | UpdateBattleAction22_4087C0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 23 | UpdateBattleAction23_4090F0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 26 | UpdateBattleAction26_40A9E0 | 已读取行为，仍需联动核对命中／反弹／复制体或反应链，避免用单个副作用命名整个动作。 |
| 27 | UpdateBattleAction27_40AA70 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 28 | UpdateBattleAction28_40ACF0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 29 | UpdateBattleAction29_40AE50 | 已读取行为，仍需联动核对命中／反弹／复制体或反应链，避免用单个副作用命名整个动作。 |
| 30 | UpdateBattleAction30_40AF30 | 已读取行为，仍需联动核对命中／反弹／复制体或反应链，避免用单个副作用命名整个动作。 |
| 31 | UpdateBattleAction31_40B020 | 已读取行为，仍需联动核对命中／反弹／复制体或反应链，避免用单个副作用命名整个动作。 |
| 32 | UpdateBattleAction32_40B410 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 33 | UpdateBattleAction33_40B450 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 34 | UpdateBattleAction34_40B5E0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 35 | UpdateBattleAction35_40B930 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 39 | UpdateBattleAction39_40BBC0 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 56 | UpdateBattleAction56_40C160 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 57 | UpdateBattleAction57_40C1A0 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 58 | UpdateBattleAction58_40C1E0 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 59 | UpdateBattleAction59_40C240 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 60 | UpdateBattleAction60_40C220 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 61 | UpdateBattleAction61_40C280 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 62 | UpdateBattleAction62_40C2B0 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 63 | UpdateBattleAction63_40C2E0 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 65 | UpdateBattleAction65_40C380 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 72 | UpdateBattleAction72_40CAA0 | 仅确认caster1070的x00递增，该字段在此路径的业务含义未证。 |
| 77 | UpdateBattleAction77_40CB50 | 仅确认绘制位或匿名渲染字段的写入；尚未追通实际显示消费，不能混同hidden或状态位。 |
| 78 | UpdateBattleAction78_40CB80 | 仅确认绘制位或匿名渲染字段的写入；尚未追通实际显示消费，不能混同hidden或状态位。 |
| 90 | UpdateBattleAction90_40C910 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 91 | UpdateBattleAction91_40CF30 | 仅确认绘制位或匿名渲染字段的写入；尚未追通实际显示消费，不能混同hidden或状态位。 |
| 92 | UpdateBattleAction92_40CF50 | 仅确认绘制位或匿名渲染字段的写入；尚未追通实际显示消费，不能混同hidden或状态位。 |
| 98 | UpdateBattleAction98_40D1D0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 100 | UpdateBattleAction100_40D3C0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 102 | UpdateBattleAction102_40D6C0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 103 | UpdateBattleAction103_40DBE0 | 已读取行为，仍需联动核对命中／反弹／复制体或反应链，避免用单个副作用命名整个动作。 |
| 106 | UpdateBattleAction106_40DF50 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 108 | UpdateBattleAction108_40DF80 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 109 | UpdateBattleAction109_40E4B0 | 已读取行为，仍需联动核对命中／反弹／复制体或反应链，避免用单个副作用命名整个动作。 |
| 110 | UpdateBattleAction110_40E560 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 111 | UpdateBattleAction111_40E970 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 112 | UpdateBattleAction112_40EB00 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 114 | UpdateBattleAction114_40F0A0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 115 | UpdateBattleAction115_40FB80 | 已确认特效创建／等待／清理步骤，但对应工厂type的具体业务尚未完整确认。 |
| 116、117、118、131、132、134、135、136、137、150、151、152、153、158、161、217、250、252 | UpdateBattleFactoryAction_40FC10 | 已核实转162等待；118没有创建分支。其余仅确认工厂type及图号／缩放等表现，对应完整业务尚缺证据，保留编号。 |
| 138 | UpdateBattleAction138_40FF40 | 已确认特效创建／等待／清理步骤，但对应工厂type的具体业务尚未完整确认。 |
| 139 | UpdateBattleAction139_410000 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 140 | UpdateBattleAction140_410730 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 154 | UpdateBattleAction154_40F790 | 已确认特效创建／等待／清理步骤，但对应工厂type的具体业务尚未完整确认。 |
| 155 | UpdateBattleAction155_40F800 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 156 | UpdateBattleAction156_40FB30 | 已确认特效创建／等待／清理步骤，但对应工厂type的具体业务尚未完整确认。 |
| 163 | UpdateBattleAction163_4113A0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 164 | UpdateBattleAction164_411A50 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 165 | UpdateBattleAction165_411D30 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 167 | UpdateBattleAction167_411FA0 | 仅确认绘制位或匿名渲染字段的写入；尚未追通实际显示消费，不能混同hidden或状态位。 |
| 168 | UpdateBattleAction168_411FC0 | 仅确认绘制位或匿名渲染字段的写入；尚未追通实际显示消费，不能混同hidden或状态位。 |
| 169 | UpdateBattleAction169_411FE0 | 仅确认绘制位或匿名渲染字段的写入；尚未追通实际显示消费，不能混同hidden或状态位。 |
| 170 | UpdateBattleAction170_412000 | 仅确认绘制位或匿名渲染字段的写入；尚未追通实际显示消费，不能混同hidden或状态位。 |
| 179 | UpdateBattleAction179_4122E0 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 181、182、185、186、187、188、197、198、199、200、203 | UpdateBattleActions180_203_412310 | 已核实转204渐显、结果结算及等待；188没有创建分支。其余特效业务尚缺证据，不能与普通工厂入口合并。 |
| 205 | UpdateBattleAction205_4127B0 | 已核对动画／帧结束或方向变化行为，尚未确认对应角色动画的业务语义，保留编号。 |
| 209 | UpdateBattleAction209_412900 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 216 | UpdateBattleAction216_413400 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 218 | UpdateBattleAction218_413550 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 221 | UpdateBattleAction221_413B40 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 222 | UpdateBattleAction222_413E90 | 已读取行为，仍需联动核对命中／反弹／复制体或反应链，避免用单个副作用命名整个动作。 |
| 224 | UpdateBattleAction224_414050 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 225 | UpdateBattleAction225_4150F0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 226 | UpdateBattleAction226_4154C0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 227 | UpdateBattleAction227_415940 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 232 | UpdateBattleAction232_416220 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 233 | UpdateBattleAction233_416860 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 234 | UpdateBattleAction234_416AB0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 237 | UpdateBattleAction237_416DD0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 238、239 | UpdateBattleActions238_239_417410 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 243 | UpdateBattleAction243_417D80 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 255 | UpdateBattleAction255_4186B0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |

## 验证

首批十个CPP替换627处引用，48个成员均有消费者及中文注释。按头文件实际值还原引用后，十个CPP完整文本与首批修改前一致（包含当时用户既有修改）；头文件去掉新增枚举及注释后原代码词法序列一致。

83～87补充五个成员及四个CPP的14处引用，总计53项／641处引用。按实际枚举值还原本批引用后，排除新增注释的完整代码词法序列与本批修改前一致；头文件除新增五项及注释外原代码一致。成员无重复名称／值且均有消费者，技能83～87及其他同值编号未混改。显式UTF-8并保持原换行，仅做静态差异及精确暂存范围检查，不编译、运行或新增测试。

2026-10-22继续批新增59项、替换11个CPP的318处引用（含补齐此前已命名动作的两处跳转），累计112项／959处引用。按头文件实际值分别归一化修改前后代码中的动作枚举引用，排除注释后完整词法序列一致；头文件除新增成员及说明外原代码一致。全部成员有消费者且无重名重值，原生257项分派覆盖不变。保留浮点、RNG、HP／FP数值、目标重新取样及推进时序，仅做显式编码和静态核对。

2026-10-22特效批新增31项，七个CPP替换157处引用，累计143项／1116处引用。包含普通／渐显工厂内部case、127～129范围边界及状态技能生产方的局部state分支；工厂type和换算偏移不替换。以实际枚举值归一化前后CPP，剔除注释后完整词法序列一致；头文件仅增加31项带中文注释成员。143项无重名重值且全部引用，原生257项分派集合保持不变；显式UTF-8与原换行核对，不编译、运行或新增测试。


## 全257项补齐（2026-10-25）

用户确认未证业务名的项按已证行为命名。本轮新增114项，原143项名称和值保持；BattleActionId按原编号覆盖0～256全部257项。这里只命名动作域，不修改角色phase、动作队列或函数参数的整数类型，不更改原函数名称／地址、队列终止符和执行顺序。

### 原端与参考证据

| 来源 | 已核对事实及适用边界 |
| --- | --- |
| cg2d.exe，0x004392A0 | 0x004392C6将第三参数写入castList，0x004392D3写入-1终止符，0x004392DE设置忙标志。第三参数是角色动作号。 |
| cg2d.exe，0x00439340 | 0x0043936C将下一队列值写入角色phase；0x00439370在队列终止／结束时写236。命名为ResetBusyAndIdle，保持原分流。 |
| cg2d.exe，0x00404DD5 | 输入来自角色phase；实际跳转表0x0040573C，表项宽4字节，起始case为0，共257项。逐项读取原表目标并核对目标处call，257项均与当前UpdateNativeBattleActor处理函数一致。 |
| cg2d.exe，0x0040FC10、0x00412310 | 分别核对30条普通特效映射及23条渐显特效映射，工厂type、末参1及后继162／204与源码一致。118／188原本没有创建分支，分别命名EnterEffectWait／EnterFadeInAndResolveEffect。 |
| cg2d.exe，0x0040CAA0；battle_message.cpp合击生产 | 0x0040CAB1递增caster载荷首整数；本入口由场景入队，实际对应BattleScenePayload::completedEntryCount00。命名NotifyCasterEntryComplete，不把该写入解释为角色坐标移动。 |
| cgmsv_server/include/battle.h:572起、battle/battle_event.c:6097起 | BM_FLAG_CRITICAL、AVOIDANCE、MIKAWASHI、PARRY及AKO标志与角色受击、闪避和击飞生产对应；27／165分别保留可选旋转与必经旋转，31仍等待攻击者离开动作14。服务器宏不用于替换对象flags、角色flagsFDC或其他同值编号。 |
| cgmsv_server/skill/tech.c | TECH_LpRecovery、TECH_Steal、TECH_DefUp、TECH_Acid、TECH_MonWind等注册只用于核对技能生产入口，不推导客户端动作编号。 |
| H:\Games\cgmsv_21.2a\gmsv\data\skill.txt、tech.txt | 显式GBK严格读取。skill.txt中61／62／63为补血系列、106为分身、97为大地之怒、260为座骑；tech.txt中技术9900属于TECH_DefUp。据此核生产背景，多技能共用的动作按行为命名，不把技能号直接用作phase。 |
| src/world/WorldActorDrawing.cpp:572起、src/Yobi/base.cpp:591起 | 308对应batchType颜色通道掩码，值8只覆盖Alpha；309对应rebirthValue1混合标志覆盖掩码，255覆盖全部位。167～170分别据实际消费命名。77／78保留后端绘制位的行为名称；0x8000按帧奇偶跳过绘制，0x100000把Alpha设为128，不概括为同一种底层混合机制。 |

场景phase8与角色动作8分属不同域。近战处理中的caster来自场景，因此其phase==8保留原值；骑手、缩放特效等对象自身的phase0／1／2／3也保持。本轮不修改或保存IDA数据库，不修改服务端或游戏数据。

### 新增114项

表中函数均为当前源码处理入口，地址后缀与原257项跳转表逐项对应；共用工厂函数的具体type另见说明。动画／工厂type／匿名标志的业务名未证时，名称只表达已证行为。

| 编号 | 名称 | 处理函数／原地址 | 已证行为 |
| --- | --- | --- | --- |
| 7 | BattleAction_PlayAnimation2FacingEffectTarget | UpdateBattleAction7_409410 | 播放动画2，面向当前效果目标；目标为-1时面向本角色阵型点，终帧后推进。 |
| 8 | BattleAction_PlayAnimation2FacingMoveTarget | UpdateBattleAction8_4094A0 | 播放动画2并面向moveTargetXFE8／YFEC，终帧后推进。 |
| 9 | BattleAction_MoveTowardTargetOrFormation | UpdateBattleAction9_409510 | 前两项动作为近战攻击14时返回阵型点，否则接近当前效果目标；到达阈值后推进。 |
| 10 | BattleAction_PlayAnimation4AndRestoreFacing | UpdateBattleAction10_409610 | 播放动画4；终帧且前项为归位11、自身未死亡时恢复阵型朝向，再推进。 |
| 12 | BattleAction_PrepareEscape | UpdateBattleAction12_406930 | 面向退场点播放逐步减速的动画与周期尾迹，计数到120后推进；死亡时直接准备退场。 |
| 13 | BattleAction_MoveToExitPoint | UpdateBattleAction13_406A80 | 移往x08／y0C退场点并产生尾迹，到达后定位并推进；也供玩家宠物离场使用。 |
| 14 | BattleAction_PerformMeleeAttack | UpdateBattleAction14_409680 | 按帧事件执行近战命中、反射／吸收／无效、状态及HP／FP结果，等待收尾后推进。 |
| 15 | BattleAction_LaunchRangedAttack | UpdateBattleAction15_406E10 | 播放远程攻击动画；模式3创建回旋镖，其他模式按帧事件逐目标创建远程命中对象。 |
| 16 | BattleAction_ResolveProjectileHit | UpdateBattleAction16_4072E0 | 投射物接近目标后结算命中、状态与数值，处理特殊命中效果及继续飞出／销毁。 |
| 18 | BattleAction_FireBowShots | UpdateBattleAction18_408170 | 按帧事件逐目标发射图号22050的箭矢命中对象；保留乱射等技能的重复播放分支。 |
| 19 | BattleAction_FireThrowingShots | UpdateBattleAction19_4082F0 | 探测动画7／5并按帧事件逐目标发射图号22090的投射物；乱射可清帧重播。 |
| 20 | BattleAction_ResolveCloseRangeHit | UpdateBattleAction20_408410 | 移动至目标70距离内处理命中、伤害／免疫及目标推进，随后销毁或继续飞出。 |
| 21 | BattleAction_MoveBoomerangToTurnPoint | UpdateBattleAction21_408750 | 回旋镖先移往载荷x00／y04转折点，距离小于20后转入回旋命中22。 |
| 22 | BattleAction_ResolveBoomerangHits | UpdateBattleAction22_4087C0 | 回旋过程中逐目标结算命中和数值，消费效果记录并等待延迟，结束后转23返回。 |
| 23 | BattleAction_ReturnBoomerang | UpdateBattleAction23_4090F0 | 回旋镖转向并返回原施放者，按距离及技能推进施放者动作，近距离标死自身。 |
| 26 | BattleAction_RecoilOnHit | UpdateBattleAction26_40A9E0 | 按受击／格挡结果选动画8／9并沿反向移动；自身不推进队列。 |
| 27 | BattleAction_ReactToHit | UpdateBattleAction27_40AA70 | 处理受击后退、可选暴击旋转、减速和等待；按死亡及结果标志恢复速率后推进。 |
| 28 | BattleAction_ReactToHitWithSound | UpdateBattleAction28_40ACF0 | 播放音效132并按14／34／54计数执行后退、减速及等待后推进。 |
| 29 | BattleAction_RecoilAndWait44Ticks | UpdateBattleAction29_40AE50 | 先后退，再等待至计数44；恢复播放率和移动速度后推进。 |
| 30 | BattleAction_RecoilAndWait14Or28Ticks | UpdateBattleAction30_40AF30 | 先后退，按死亡位等待至计数14或28，恢复速率后推进。 |
| 31 | BattleAction_DodgeAndWaitAttacker | UpdateBattleAction31_40B020 | 后退闪避，可按招架位旋转；等待对手离开近战动作14后恢复姿态并推进。 |
| 32 | BattleAction_PlayHitAnimation | UpdateBattleAction32_40B410 | 播放受击动画8，终帧后推进；不移动或累计受击计数。 |
| 33 | BattleAction_PlayGuardAnimation | UpdateBattleAction33_40B450 | 播放格挡动画9，终帧后推进；与受击动画8保持独立。 |
| 34 | BattleAction_BounceOutAfterKnockout | UpdateBattleAction34_40B5E0 | 先受击后退，再散射、旋转并在屏幕边界反弹三次，飞出边界后推进。 |
| 35 | BattleAction_FlyOutAfterKnockout | UpdateBattleAction35_40B930 | 先受击后退，再散射并以速度15附带尾迹飞出边界，随后推进。 |
| 39 | BattleAction_AlternateHitAndGuardAnimations | UpdateBattleAction39_40BBC0 | 交替播放动画8／9及音效并逐步缩短间隔；十轮后恢复阵型朝向并推进。 |
| 56 | BattleAction_PlayAnimation5AndResetFrames | UpdateBattleAction56_40C160 | 动画5终帧后依次清帧索引、剩余帧和帧事件，再推进。 |
| 57 | BattleAction_PlayAnimation13AndResetFrames | UpdateBattleAction57_40C1A0 | 动画13终帧后清帧索引和剩余帧再推进，保留帧事件。 |
| 58 | BattleAction_PlayAnimation14AndResetFrames | UpdateBattleAction58_40C1E0 | 动画14终帧后清帧索引和剩余帧再推进，保留帧事件。 |
| 59 | BattleAction_PlayAnimation15AndResetFrames | UpdateBattleAction59_40C240 | 动画15终帧后清帧索引和剩余帧再推进，保留帧事件。 |
| 60 | BattleAction_SetAnimation15 | UpdateBattleAction60_40C220 | 仅将角色动画设为15后推进，不播放或清帧。 |
| 61 | BattleAction_PlayAnimation12 | UpdateBattleAction61_40C280 | 播放动画12，终帧后推进，不清帧状态。 |
| 62 | BattleAction_PlayAnimation18 | UpdateBattleAction62_40C2B0 | 播放动画18，终帧后推进，不清帧状态。 |
| 63 | BattleAction_TurnLeftThenRight | UpdateBattleAction63_40C2E0 | 待机动画中在计数20／50分别改变方向-2／+4，旧计数到70后推进。 |
| 65 | BattleAction_OscillateFacing | UpdateBattleAction65_40C380 | 待机动画中按八次计数交替改变一个方向，旧计数到70后推进。 |
| 72 | BattleAction_NotifyCasterEntryComplete | UpdateBattleAction72_40CAA0 | 递增caster1070载荷首个整数后推进；当前合击生产中该槽对应场景completedEntryCount00。 |
| 77 | BattleAction_EnableBackendDrawFlags | UpdateBattleAction77_40CB50 | 按原3D设备分支设置对象绘制位0x100000或0x8000，再推进。 |
| 78 | BattleAction_DisableBackendDrawFlags | UpdateBattleAction78_40CB80 | 按原3D设备分支清除对象绘制位0x100000或0x8000，再推进。 |
| 90 | BattleAction_SweepEffectTargetsAndExit | UpdateBattleAction90_40C910 | 移动并旋转，接近各效果目标时启动目标动作；消费目标后飞出边界并推进。 |
| 91 | BattleAction_SetObjectFlag2AndPlay | UpdateBattleAction91_40CF30 | 设置YobiAction::flags的位2，推进后播放一帧；不写hidden字段。 |
| 92 | BattleAction_ClearObjectFlag2AndPlay | UpdateBattleAction92_40CF50 | 清除YobiAction::flags的位2，推进后播放一帧；与动作91成对。 |
| 98 | BattleAction_CastWithTargetMarkers | UpdateBattleAction98_40D1D0 | 播放施法动画6，在帧事件及终帧创建目标标记，并按技能绘制平铺效果。 |
| 100 | BattleAction_PlayAttackAnimationSequence | UpdateBattleAction100_40D3C0 | 按技能等级选择动画序列表，逐段播放、触发目标反应和命中特效；遇序列负值后推进。 |
| 102 | BattleAction_RushThroughTargets | UpdateBattleAction102_40D6C0 | 先以速度32移动八次，再逐目标处理命中、伤害／吸收／反射及状态，恢复速率后推进。 |
| 103 | BattleAction_SpawnMeleeAttackCopies | UpdateBattleAction103_40DBE0 | 逐目标创建独立复制体，安排等待、绘制位、移动、近战攻击及销毁队列。 |
| 106 | BattleAction_SetAnimation13 | UpdateBattleAction106_40DF50 | 仅将角色动画设为13后推进，不播放或清帧。 |
| 108 | BattleAction_AttackAllTargetsOnFrameEvent | UpdateBattleAction108_40DF80 | 动画5的有效帧事件到达后逐目标结算攻击结果，清事件；终帧后推进。 |
| 109 | BattleAction_SpawnChargedStrikeCopy | UpdateBattleAction109_40E4B0 | 创建图号101031的独立复制体，复制当前效果并安排等待、蓄力攻击110及销毁。 |
| 110 | BattleAction_ChargeAndStrike | UpdateBattleAction110_40E560 | 先累计60次蓄力，再接近目标，按成功／反射结果结算双方HP及命中特效，延迟后推进。 |
| 111 | BattleAction_FlyOutFastAndShakeScreen | UpdateBattleAction111_40E970 | 短暂后退后散射，以速度32反向飞出；离开边界时启动震屏并推进。 |
| 112 | BattleAction_SpawnArcAttackCopies | UpdateBattleAction112_40EB00 | 按帧事件为效果目标创建复制体，安排弧线移动及动作114命中，保留等级90特殊队列。 |
| 114 | BattleAction_ResolveArcCopyHits | UpdateBattleAction114_40F0A0 | 逐效果处理弧线复制体命中、反射／吸收、状态及数值；恢复速率并推进。 |
| 115 | BattleAction_PlayAttackClashEffect | UpdateBattleAction115_40FB80 | 按反击标志创建工厂type0／3效果，隐藏自身与效果目标，转162等待。 |
| 116 | BattleAction_PlayEffectType1 | UpdateBattleFactoryAction_40FC10 | 创建工厂type1（固定图号110063）并转162等待；业务名称未证。 |
| 117 | BattleAction_PlayMagicAttackEffect | UpdateBattleFactoryAction_40FC10 | 创建工厂type2攻击魔法受术效果并转162等待；包含反射反馈路径。 |
| 118 | BattleAction_EnterEffectWait | UpdateBattleFactoryAction_40FC10 | 不创建新效果，直接转162等待现有效果完成；保留原缺少创建分支的行为。 |
| 131 | BattleAction_PlayEffectType14 | UpdateBattleFactoryAction_40FC10 | 创建工厂type14的等级分档效果并转162等待；可由状态报文mode1安排。 |
| 132 | BattleAction_PlayEffectType15 | UpdateBattleFactoryAction_40FC10 | 创建工厂type15的等级分档效果并转162等待；业务名称未证。 |
| 134 | BattleAction_PlayEffectType26 | UpdateBattleFactoryAction_40FC10 | 创建工厂type26（图号110156）并转162等待；不同技能可共用。 |
| 135 | BattleAction_PlayEffectType27 | UpdateBattleFactoryAction_40FC10 | 创建工厂type27（图号111162）并转162等待，不按技术号2053猜业务名。 |
| 136 | BattleAction_PlayEffectType28 | UpdateBattleFactoryAction_40FC10 | 创建工厂type28（图号111163）并转162等待，不按技术号2054猜业务名。 |
| 137 | BattleAction_PlayHealingCastEffect | UpdateBattleFactoryAction_40FC10 | 补血技能生产的施放侧效果：工厂type29，创建后转162等待。 |
| 138 | BattleAction_PlayItemUseEffect | UpdateBattleAction138_40FF40 | 物品使用入口安排的工厂type20效果，创建后转162等待。 |
| 139 | BattleAction_CastSpellAndTriggerTargets | UpdateBattleAction139_410000 | 播放施法动画及type34效果，按帧事件启动目标或创建魔法效果；终帧清理并推进。 |
| 140 | BattleAction_TriggerSpellEffectOrHealTarget | UpdateBattleAction140_410730 | 攻击魔法创建type35效果，补血魔法推进当前目标；随后推进自身。 |
| 150 | BattleAction_PlayAmountTieredRecoveryEffect | UpdateBattleFactoryAction_40FC10 | 按value1030的2000／3500阈值选恢复图像（工厂type18），转162等待。 |
| 151 | BattleAction_PlayLevelScaledEffect21 | UpdateBattleFactoryAction_40FC10 | 创建工厂type21等级分档效果，部分技能使用castLevel1040；转162等待。 |
| 152 | BattleAction_PlayParentBoundEffect22 | UpdateBattleFactoryAction_40FC10 | 创建工厂type22、图号110097的效果，父对象死亡时结束；转162等待。 |
| 153 | BattleAction_PlayParentBoundEffect23 | UpdateBattleFactoryAction_40FC10 | 创建工厂type23、图号110096的效果，父对象死亡时结束；转162等待。 |
| 154 | BattleAction_StartScalingEffect24 | UpdateBattleAction154_40F790 | 创建并保存工厂type24缩放效果，等待其phase为1后推进本角色。 |
| 155 | BattleAction_PulseScaleWithParentEffect | UpdateBattleAction155_40F800 | 创建type22父对象效果，按原计数往复缩放角色并恢复，结束时令保存效果进入phase2。 |
| 156 | BattleAction_WaitScalingEffect24End | UpdateBattleAction156_40FB30 | 等待保存的type24效果phase为3，标死、清槽后推进角色。 |
| 158 | BattleAction_PlayEffectType38 | UpdateBattleFactoryAction_40FC10 | 按技能等级选择工厂type38图像并转162等待；业务名称未证。 |
| 161 | BattleAction_PlayBombLevelEffect | UpdateBattleFactoryAction_40FC10 | 创建工厂type39，按gBattleBombLevelD67B2C选择图像，转162等待。 |
| 163 | BattleAction_TransitionRebirthForm | UpdateBattleAction163_4113A0 | 按结果标志进入或退出Rebirth形态，处理换图、透明度及形态附属效果，不是死亡复活。 |
| 164 | BattleAction_RestoreRebirthBaseForm | UpdateBattleAction164_411A50 | 恢复Rebirth前的基础图像、状态位和透明度，结束后返回保存的角色phase。 |
| 165 | BattleAction_ReactToCriticalHit | UpdateBattleAction165_411D30 | 受击后退后持续旋转、减速和等待；与27的可选旋转不同，最后按死亡标志推进。 |
| 167 | BattleAction_EnableAlphaOverride | UpdateBattleAction167_411FA0 | 将batchType设为8，只覆盖绘制颜色的Alpha通道，然后推进。 |
| 168 | BattleAction_ClearColorOverrides | UpdateBattleAction168_411FC0 | 将batchType清零，停止覆盖绘制颜色通道，然后推进。 |
| 169 | BattleAction_OverrideAllMixFlags | UpdateBattleAction169_411FE0 | 将混合标志覆盖掩码rebirthValue1设为255，使用既有rebirthValue2覆盖全部混合位。 |
| 170 | BattleAction_ClearMixFlagOverride | UpdateBattleAction170_412000 | 将混合标志覆盖掩码rebirthValue1清零，然后推进。 |
| 179 | BattleAction_PlayAnimation6 | UpdateBattleAction179_4122E0 | 播放动画6，终帧后推进，不额外创建施法效果。 |
| 181 | BattleAction_PlayEffectType1AndFadeIn | UpdateBattleActions180_203_412310 | 创建工厂type1后转204渐显、处理结果及等待；业务名称未证。 |
| 182 | BattleAction_PlayMagicAttackEffectAndFadeIn | UpdateBattleActions180_203_412310 | 创建type2攻击魔法受术效果后转204渐显、处理结果及等待。 |
| 185 | BattleAction_PlayLevelScaledEffect21AndFadeIn | UpdateBattleActions180_203_412310 | 创建工厂type21等级分档效果后转204渐显、处理结果及等待。 |
| 186 | BattleAction_PlayParentBoundEffect22AndFadeIn | UpdateBattleActions180_203_412310 | 创建工厂type22父对象生命周期关联效果后转204渐显、处理结果及等待。 |
| 187 | BattleAction_PlayParentBoundEffect23AndFadeIn | UpdateBattleActions180_203_412310 | 创建工厂type23父对象生命周期关联效果后转204渐显、处理结果及等待。 |
| 188 | BattleAction_EnterFadeInAndResolveEffect | UpdateBattleActions180_203_412310 | 不创建新效果，直接转204渐显、处理结果及等待；保留缺少创建分支的行为。 |
| 197 | BattleAction_PlayEffectType26AndFadeIn | UpdateBattleActions180_203_412310 | 创建工厂type26后转204渐显、处理结果及等待；业务名称未证。 |
| 198 | BattleAction_PlayHealingCastEffectAndFadeIn | UpdateBattleActions180_203_412310 | 创建工厂type29补血施放侧效果后转204渐显、处理结果及等待。 |
| 199 | BattleAction_PlayEffectType14AndFadeIn | UpdateBattleActions180_203_412310 | 创建工厂type14后转204渐显、处理结果及等待；业务名称未证。 |
| 200 | BattleAction_PlayEffectType15AndFadeIn | UpdateBattleActions180_203_412310 | 创建工厂type15后转204渐显、处理结果及等待；业务名称未证。 |
| 203 | BattleAction_PlayBombLevelEffectAndFadeIn | UpdateBattleActions180_203_412310 | 创建工厂type39的炸弹等级分档效果后转204渐显、处理结果及等待。 |
| 205 | BattleAction_PlayAnimation16AndResetFrames | UpdateBattleAction205_4127B0 | 播放动画16，终帧后清帧索引和剩余帧再推进，保留帧事件。 |
| 209 | BattleAction_ReactToHitAndStrikeLinkedTarget | UpdateBattleAction209_412900 | 受击后退和减速期间，在指定计数对关联目标结算后续攻击，再按死亡标志收尾。 |
| 216 | BattleAction_CastMonsterSpellAndTriggerTargets | UpdateBattleAction216_413400 | 播放怪物法术的施法效果；帧事件时处理255哨兵目标列表，启动目标并按技能创建效果。 |
| 217 | BattleAction_PlayRandomOffsetEffect40 | UpdateBattleFactoryAction_40FC10 | 创建工厂type40，在角色图像范围内随机偏移效果坐标并设置后端绘制位，转162等待。 |
| 218 | BattleAction_ChangeAppearanceWithCrossFade | UpdateBattleAction218_413550 | 创建换图副体并交叉渐变，按效果记录切换外观、保存基础图号和覆盖状态后推进。 |
| 221 | BattleAction_ApplyFollowupHit | UpdateBattleAction221_413B40 | 先消费攻击者记录，再按下一条目标记录结算追加命中／反射／吸收与状态，推进动作及效果。 |
| 222 | BattleAction_MoveLinkedProjectileToTarget | UpdateBattleAction222_413E90 | 关联投射物接近目标后销毁；特定回旋技能则推进目标／效果并转22继续回旋命中。 |
| 224 | BattleAction_TransitionRideState | UpdateBattleAction224_414050 | 按效果flags进入或解除骑乘，处理骑手运动、角色槽转移、装备和状态效果恢复。 |
| 225 | BattleAction_FailToMountAndReturn | UpdateBattleAction225_4150F0 | 骑乘失败入口创建骑手并播放抛物线运动，随后还原角色图像及坐标并清理临时对象。 |
| 226 | BattleAction_DismountAndRestoreActors | UpdateBattleAction226_4154C0 | 骑乘退出时播放骑手受击／抛物线运动，恢复原角色槽、装备及状态并销毁合成对象。 |
| 227 | BattleAction_DismountWithActorAnimation | UpdateBattleAction227_415940 | 解除骑乘时复制本角色动画并按结果选择骑手动画，落地后恢复原角色和挂件。 |
| 232 | BattleAction_RushIntoTargetWithTrail | UpdateBattleAction232_416220 | 动画3冲向目标，近距离结算命中／反射／吸收及数值，逐帧产生随机尾迹。 |
| 233 | BattleAction_ReactToHitWithoutDisplacement | UpdateBattleAction233_416860 | 保留受击计数、旋转、尾迹及死亡门禁；不调用移动函数，与27／165保持独立。 |
| 234 | BattleAction_BounceOutImmediately | UpdateBattleAction234_416AB0 | 首计数即创建散射与音效，旋转并在边界反弹，飞出后推进；没有34的前段受击后退。 |
| 237 | BattleAction_RushAndResolveHitWithin16Ticks | UpdateBattleAction237_416DD0 | 前16次移动并按距离触发命中，达到计数时也处理目标结果；之后继续移动及队列收尾。 |
| 238 | BattleAction_ResolveDeceleratingProjectileHit | UpdateBattleActions238_239_417410 | 处理目标命中及等待；等待结束后每次减速0.5，低速时转239加速。 |
| 239 | BattleAction_ResolveAcceleratingProjectileHit | UpdateBattleActions238_239_417410 | 与238共用命中逻辑，等待结束后每次加速0.5，命中后按原条件销毁或飞出。 |
| 243 | BattleAction_ResolveStraightProjectileHit | UpdateBattleAction243_417D80 | 直线投射物按距离及帧事件结算命中，等待后继续移动；无238／239的加减速切换。 |
| 250 | BattleAction_PlayConfiguredEffect41 | UpdateBattleFactoryAction_40FC10 | 按image102C创建工厂type41效果并转162等待；保留与type42的独立编号。 |
| 252 | BattleAction_PlayConfiguredEffect42 | UpdateBattleFactoryAction_40FC10 | 按image102C创建工厂type42效果并转162等待；不与动作250合并。 |
| 255 | BattleAction_ApplyAreaAttackOnFrameEvent | UpdateBattleAction255_4186B0 | 动画5有效帧事件时逐目标结算范围攻击，首目标创建type47效果，终帧后推进。 |

### 本轮验证

- 257项名称和值均唯一，连续覆盖0～256；UpdateNativeBattleActor的257个case均使用本枚举，所有成员有实际消费者。
- 14份CPP共替换565处动作号引用，包含直接入队、三元结果、角色跳转／比较、复制体／投射物生产及队列终止回退。场景、骑手、特效自身phase，技能号、动画号、工厂type、图号和条件位掩码保持各自原值与解释域。
- 对15份源／头文件逐词法比较：先去掉新增114个枚举定义，再将枚举引用逆替换为实际整数，与本轮HEAD原代码一致；字符串字面值也参与比较，仅排除注释。原143项定义、调用、条件、取样、数值与写入顺序均保持。
- 两个工厂消费者的53条phase→type映射及后继162／204与本轮IDA伪代码逐项一致；118／188没有被补造创建分支。
- 显式UTF-8读取，工作区换行保持CRLF，Git差异检查通过。只做静态核对，未编译、调试、测试、运行客户端或计算文件摘要；行为／画面运行验收未执行。
