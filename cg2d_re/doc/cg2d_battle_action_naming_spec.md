# 战斗动作命名（BAN01）

日期：2026-10-21。用户要求将141、142等动作号按作用命名，随后要求继续尝试其他动作；沿用中文注释、英文标识符规则。

## 范围与依据

在既有battle_base.hpp的cg::yobi::detail定义有符号BattleActionId枚举，覆盖逐项读取实现确认的112个动作。替换AppendBattleCast_4392A0的动作参数（包括三元分支值）、battle_actor.cpp的phase分派case、已核实角色动作处理函数内的直接phase跳转／比较及castListCA4动作值；相同数值的音效号、字段偏移、角色动画动作号、其他对象phase及条件中的位掩码不替换。

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

### 仍保留编号的动作

原生分派共有257个动作入口，累计112项已命名；以下145个编号对应89个处理入口，保留编号并记录本轮能确认的边界。定位完成不等于业务已证；未完整核对的组合流程明确列为后续分析项，不宣称所有动作已完成业务还原。

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
| 116、117、118、119、120、121、122、123、124、125、126、130、131、132、133、134、135、136、137、146、149、150、151、152、153、157、158、161、217、250、252 | UpdateBattleFactoryAction_40FC10 | 共用工厂映射；118不创建效果，其余type的业务含义及等待合同尚未逐项核实。 |
| 127、128、129 | UpdateBattleActions127To129_40FF90 | 已确认type7～9且工厂末参0后立即推进；效果业务含义未确认，不套用其他同type动作名。 |
| 138 | UpdateBattleAction138_40FF40 | 已确认特效创建／等待／清理步骤，但对应工厂type的具体业务尚未完整确认。 |
| 139 | UpdateBattleAction139_410000 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 140 | UpdateBattleAction140_410730 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
| 148 | UpdateBattleAction148_40FEF0 | 已确认特效创建／等待／清理步骤，但对应工厂type的具体业务尚未完整确认。 |
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
| 180、181、182、183、184、185、186、187、188、189、190、191、192、193、194、195、196、197、198、199、200、201、202、203 | UpdateBattleActions180_203_412310 | 共用工厂映射，转204而非162收尾；188不创建效果，不能与普通工厂入口合并。 |
| 204 | UpdateBattleAction204_4124E0 | 已定位处理函数与既有注释；本轮尚未完整核对组合状态、目标取样和收尾条件，不据局部行为猜名。 |
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
