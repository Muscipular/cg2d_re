# 战斗动作命名（BAN01）

日期：2026-10-21。用户要求将141、142等动作号按作用命名，随后要求继续尝试其他动作；沿用中文注释、英文标识符规则。

## 范围与依据

在既有battle_base.hpp的cg::yobi::detail定义有符号BattleActionId枚举，覆盖逐项读取实现确认的53个动作。替换AppendBattleCast_4392A0的动作参数（包括三元分支值）、battle_actor.cpp的phase分派case、已核实角色动作处理函数内的直接phase跳转／比较及castListCA4首项待机初始化；相同数值的音效号、字段偏移、角色动画动作号、其他对象phase及条件中的位掩码不替换。

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

## 验证

首批十个CPP替换627处引用，48个成员均有消费者及中文注释。按头文件实际值还原引用后，十个CPP完整文本与首批修改前一致（包含当时用户既有修改）；头文件去掉新增枚举及注释后原代码词法序列一致。

83～87补充五个成员及四个CPP的14处引用，总计53项／641处引用。按实际枚举值还原本批引用后，排除新增注释的完整代码词法序列与本批修改前一致；头文件除新增五项及注释外原代码一致。成员无重复名称／值且均有消费者，技能83～87及其他同值编号未混改。显式UTF-8并保持原换行，仅做静态差异及精确暂存范围检查，不编译、运行或新增测试。
