#pragma once

#include "base.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

// 状态模式／类型宏见 doc/cg2d_yobi_value_naming_spec.md；保留有符号整数常量和完整S32字段。
// YVN01：statusEffectMode13D8的解释域；RESET仅表示清理／基态，不保证类型值为空。
#define CG_BATTLE_STATUS_MODE_RESET 0 // 清理／基态。
#define CG_BATTLE_STATUS_MODE_PARAMETER 1 // 能力变化。
#define CG_BATTLE_STATUS_MODE_DEBUFF 2 // 普通减益输入。
#define CG_BATTLE_STATUS_MODE_EXTENDED 3 // 扩展状态。

// ANR清理请求的临时类型值；动作142随后可回写现有减益类型。
#define CG_BATTLE_STATUS_TYPE_CLEAR_REQUEST (-1)

// 普通减益类型：动作141及NewBattleDebuffEffect共用；与FDC位掩码不同。
#define CG_BATTLE_DEBUFF_NONE 0 // 无普通减益类型。
#define CG_BATTLE_DEBUFF_POISON 1 // 中毒。
#define CG_BATTLE_DEBUFF_SLEEP 2 // 睡眠。
#define CG_BATTLE_DEBUFF_STONE 3 // 石化。
#define CG_BATTLE_DEBUFF_INEBRIETY 4 // 醉酒。
#define CG_BATTLE_DEBUFF_CONFUSION 5 // 混乱。
#define CG_BATTLE_DEBUFF_FORGET 6 // 遗忘。

// mode=1的能力变化类型；NONE不代表其他模式的0。
#define CG_BATTLE_PARAMETER_EFFECT_NONE 0 // 不创建新的能力状态图。
#define CG_BATTLE_PARAMETER_EFFECT_DEFENSE_UP 1 // 防御上升。
#define CG_BATTLE_PARAMETER_EFFECT_DEFENSE_DOWN 2 // 防御下降。
#define CG_BATTLE_PARAMETER_EFFECT_ATTACK_UP 3 // 攻击上升。
#define CG_BATTLE_PARAMETER_EFFECT_ATTACK_DOWN 4 // 攻击下降。
#define CG_BATTLE_PARAMETER_EFFECT_AGILITY_UP 5 // 敏捷上升。
#define CG_BATTLE_PARAMETER_EFFECT_AGILITY_DOWN 6 // 敏捷下降。
#define CG_BATTLE_PARAMETER_EFFECT_RECOVERY_UP 7 // 回复能力上升。

// mode=3的扩展类型：宏值是状态类型而非技能号；保留来源编号，相同图号不合并技能。
#define CG_BATTLE_EXT_EFFECT_SKILL_2006 0 // 集中（用户确认名称）；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2010 4 // 元素增幅（用户确认名称）；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2012 5 // 防御结界（用户确认名称）；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2019 7 // 独舞（用户确认名称）；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2015 8 // 伺机而动（用户确认名称）；当前生产不安排动作143常驻图。
#define CG_BATTLE_EXT_EFFECT_SKILL_2014 9 // 战斗号令（用户确认名称）；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2027 10 // 肾上腺素（用户确认名称）；当前生产不安排动作143常驻图。
#define CG_BATTLE_EXT_EFFECT_SKILL_2058_2059 11 // 技能2058／2059；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2060_2061 12 // 技能2060／2061；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2062_2063 13 // 技能2062／2063；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2064_2065 14 // 技能2064／2065；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2066_2067 15 // 技能2066／2067；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2092_2093 16 // 技能2092／2093；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2094_2095 17 // 技能2094／2095；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2091 18 // 技能2091；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2089 19 // 技能2089；动作141特殊图号。
#define CG_BATTLE_EXT_EFFECT_SKILL_2090 20 // 技能2090；动作141特殊图号。
#define CG_BATTLE_EXT_EFFECT_SKILL_2050 30 // 技能2050；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2051 31 // 技能2051；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_2052 32 // 技能2052；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3002 33 // 技能3002；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3003 34 // 技能3003；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3004 35 // 技能3004；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3005 36 // 技能3005；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3006 37 // 技能3006；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3007 38 // 技能3007；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3008 39 // 技能3008；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3009 40 // 技能3009；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3010 41 // 技能3010；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3011 42 // 技能3011；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3012 43 // 技能3012；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3013 44 // 技能3013；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3014 45 // 技能3014；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3015 46 // 技能3015；动作143的扩展选择值。
#define CG_BATTLE_EXT_EFFECT_SKILL_3016 47 // 技能3016；动作143的扩展选择值。

#define CG_BATTLE_DEBUFF_TYPE_COUNT 6 // 技能32～49循环使用的六种普通减益。
#define CG_BATTLE_EXT_SKILL_2019_IMAGE_COUNT 3 // 13E0的防御／敏捷／攻击图像变体数。

namespace cg::yobi {

namespace detail {
// BAN01：战斗动作队列编号；用于AppendBattleCast_4392A0及角色phase分派，不是技能号或图号。
// 原函数编号／地址保留；257项覆盖0～256，逐项依据见doc/cg2d_battle_action_naming_spec.md。
enum BattleActionId : std::int32_t {
    BattleAction_Idle = 0, // 待机；按死亡／睡眠／石化状态选择姿态，保留特殊队列166及技能72／113门禁。
    BattleAction_WaitIdle = 1, // 保持待机姿态并播放；不自行推进动作队列。
    BattleAction_EnterFormation = 2, // 移入阵型坐标；到达后转待机并设置入场完成位。
    BattleAction_ApproachEffectTarget = 3, // 朝当前效果目标移动，距离小于70后推进；速度8，角色动画1。
    BattleAction_CloseInOnEffectTarget = 4, // 朝当前效果目标贴近，距离小于8后推进；速度8，角色动画3。
    BattleAction_MoveForward16Ticks = 5, // 沿当前角度移动；旧计数小于16时移动，否则推进并清计数。
    BattleAction_MoveToTargetPosition = 6, // 移动至moveTargetXFE8／YFEC；到达后推进动作。
    BattleAction_PlayAnimation2FacingEffectTarget = 7, // 播放动画2，面向当前效果目标；目标为-1时面向本角色阵型点，终帧后推进。
    BattleAction_PlayAnimation2FacingMoveTarget = 8, // 播放动画2并面向moveTargetXFE8／YFEC，终帧后推进。
    BattleAction_MoveTowardTargetOrFormation = 9, // 前两项动作为近战攻击14时返回阵型点，否则接近当前效果目标；到达阈值后推进。
    BattleAction_PlayAnimation4AndRestoreFacing = 10, // 播放动画4；终帧且前项为归位11、自身未死亡时恢复阵型朝向，再推进。
    BattleAction_ReturnToFormation = 11, // 返回阵型x00／y04；保留骑乘死亡短路、死亡动画及转入46的条件。
    BattleAction_PrepareEscape = 12, // 面向退场点播放逐步减速的动画与周期尾迹，计数到120后推进；死亡时直接准备退场。
    BattleAction_MoveToExitPoint = 13, // 移往x08／y0C退场点并产生尾迹，到达后定位并推进；也供玩家宠物离场使用。
    BattleAction_PerformMeleeAttack = 14, // 按帧事件执行近战命中、反射／吸收／无效、状态及HP／FP结果，等待收尾后推进。
    BattleAction_LaunchRangedAttack = 15, // 播放远程攻击动画；模式3创建回旋镖，其他模式按帧事件逐目标创建远程命中对象。
    BattleAction_ResolveProjectileHit = 16, // 投射物接近目标后结算命中、状态与数值，处理特殊命中效果及继续飞出／销毁。
    BattleAction_MoveOutOfBounds = 17, // 继续移动，超出原屏幕扩展边界后MarkDeath；不重命名为立即销毁。
    BattleAction_FireBowShots = 18, // 按帧事件逐目标发射图号22050的箭矢命中对象；保留乱射等技能的重复播放分支。
    BattleAction_FireThrowingShots = 19, // 探测动画7／5并按帧事件逐目标发射图号22090的投射物；乱射可清帧重播。
    BattleAction_ResolveCloseRangeHit = 20, // 移动至目标70距离内处理命中、伤害／免疫及目标推进，随后销毁或继续飞出。
    BattleAction_MoveBoomerangToTurnPoint = 21, // 回旋镖先移往载荷x00／y04转折点，距离小于20后转入回旋命中22。
    BattleAction_ResolveBoomerangHits = 22, // 回旋过程中逐目标结算命中和数值，消费效果记录并等待延迟，结束后转23返回。
    BattleAction_ReturnBoomerang = 23, // 回旋镖转向并返回原施放者，按距离及技能推进施放者动作，近距离标死自身。
    BattleAction_Wait21Ticks = 24, // 递增计数至大于20后清零并推进；不播放帧。
    BattleAction_WaitTargetReady = 25, // 等待当前效果目标待机并保存其坐标；目标缺失或为自身时按原条件推进。
    BattleAction_RecoilOnHit = 26, // 按受击／格挡结果选动画8／9并沿反向移动；自身不推进队列。
    BattleAction_ReactToHit = 27, // 处理受击后退、可选暴击旋转、减速和等待；按死亡及结果标志恢复速率后推进。
    BattleAction_ReactToHitWithSound = 28, // 播放音效132并按14／34／54计数执行后退、减速及等待后推进。
    BattleAction_RecoilAndWait44Ticks = 29, // 先后退，再等待至计数44；恢复播放率和移动速度后推进。
    BattleAction_RecoilAndWait14Or28Ticks = 30, // 先后退，按死亡位等待至计数14或28，恢复速率后推进。
    BattleAction_DodgeAndWaitAttacker = 31, // 后退闪避，可按招架位旋转；等待对手离开近战动作14后恢复姿态并推进。
    BattleAction_PlayHitAnimation = 32, // 播放受击动画8，终帧后推进；不移动或累计受击计数。
    BattleAction_PlayGuardAnimation = 33, // 播放格挡动画9，终帧后推进；与受击动画8保持独立。
    BattleAction_BounceOutAfterKnockout = 34, // 先受击后退，再散射、旋转并在屏幕边界反弹三次，飞出边界后推进。
    BattleAction_FlyOutAfterKnockout = 35, // 先受击后退，再散射并以速度15附带尾迹飞出边界，随后推进。
    BattleAction_Wait30Ticks = 36, // valueFF0递增至30后清零并推进；保留睡眠／石化播放门禁。
    BattleAction_Wait60Ticks = 37, // valueFF0递增至60后清零并推进；不是60毫秒。
    BattleAction_Wait120Ticks = 38, // valueFF0递增至120后清零并推进；不是120毫秒。
    BattleAction_AlternateHitAndGuardAnimations = 39, // 交替播放动画8／9及音效并逐步缩短间隔；十轮后恢复阵型朝向并推进。
    BattleAction_FaceFormation = 40, // 按阵型位置恢复朝向后推进。
    BattleAction_FaceTargetFormationPoint = 41, // 面向targetPosition100C对应的阵型静态坐标后推进。
    BattleAction_FaceSelectedTarget = 42, // 面向targetPosition100C对应角色的实时坐标后推进。
    BattleAction_TrackOpponentFacing = 43, // 持续面向battleOpponent10并播放待机；自身不推进队列。
    BattleAction_TurnTargetTowardSelf = 44, // 令当前效果目标面向本角色，随后推进本角色动作。
    BattleAction_SetHpZero = 45, // 将本角色HP置0后推进；不在此设置死亡位。
    BattleAction_HandleDeath = 46, // 死亡动画完成后清理效果并分流；保留再生、骑乘、外观覆盖及玩家／宠物差异。
    BattleAction_MarkDead = 47, // 设置角色FDC倒地／死亡位后推进，不销毁对象。
    BattleAction_FadeOutForRemoval = 48, // 按后端执行闪烁／渐隐退场，最后转49或51移除；与214的推进收尾不同。
    BattleAction_WaitRemovalEffect = 49, // 等待播放状态2，清标记后转51移除；区别于215推进队列。
    BattleAction_PlayDeathAnimation = 50, // 播放角色死亡动画10，终帧后推进；不执行46的清理分流。
    BattleAction_RemoveFromBattle = 51, // 清空战斗角色全局槽并MarkDeath；实际释放沿调度器。
    BattleAction_DismissPlayerPet = 52, // 玩家离场时给其宠物安排13／47／51并启动；最后推进玩家动作。
    BattleAction_ShrinkOut = 53, // 每次缩小0.02并创建type22效果；缩至0后清施法效果并推进。
    BattleAction_GrowIn = 54, // 每次放大0.02至1并创建type23效果；恢复阵型朝向和绘制位后推进。
    BattleAction_ConsumeFp = 55, // 播放一帧、扣除fpUsed1008指定消耗，再推进。
    BattleAction_PlayAnimation5AndResetFrames = 56, // 动画5终帧后依次清帧索引、剩余帧和帧事件，再推进。
    BattleAction_PlayAnimation13AndResetFrames = 57, // 动画13终帧后清帧索引和剩余帧再推进，保留帧事件。
    BattleAction_PlayAnimation14AndResetFrames = 58, // 动画14终帧后清帧索引和剩余帧再推进，保留帧事件。
    BattleAction_PlayAnimation15AndResetFrames = 59, // 动画15终帧后清帧索引和剩余帧再推进，保留帧事件。
    BattleAction_SetAnimation15 = 60, // 仅将角色动画设为15后推进，不播放或清帧。
    BattleAction_PlayAnimation12 = 61, // 播放动画12，终帧后推进，不清帧状态。
    BattleAction_PlayAnimation18 = 62, // 播放动画18，终帧后推进，不清帧状态。
    BattleAction_TurnLeftThenRight = 63, // 待机动画中在计数20／50分别改变方向-2／+4，旧计数到70后推进。
    BattleAction_SetIdlePose = 64, // 将角色动画动作设为0并推进；不同于持续待机动作0。
    BattleAction_OscillateFacing = 65, // 待机动画中按八次计数交替改变一个方向，旧计数到70后推进。
    BattleAction_PlayConfiguredImage = 66, // 切换为image1014及角色动作5，播放结束后推进并清帧事件。
    BattleAction_TriggerTargetsOnFrameEvent = 67, // 帧事件到达时启动目标动作并推进效果游标；自身动画结束后推进。
    BattleAction_ShowReactionIcon = 68, // 用reactionImage1010创建浮动图标，条件播放后推进。
    BattleAction_ShowReactionHitEffect = 69, // 用reactionImage1010创建30帧命中特效，条件播放后推进。
    BattleAction_FireSpiracleShots = 70, // 帧事件触发时逐目标创建气功弹，消费效果列表并播放音效254。
    BattleAction_StartEffectTargets = 71, // 无帧事件门禁，逐目标启动动作并推进效果游标，再推进自身。
    BattleAction_NotifyCasterEntryComplete = 72, // 递增caster1070载荷首个整数后推进；当前合击生产中该槽对应场景completedEntryCount00。
    BattleAction_AdvanceLinkedCast = 73, // 先推进当前效果目标的动作，再推进本角色动作。
    BattleAction_AdvanceEffectAndCast = 74, // 先推进当前效果游标，再推进本角色动作。
    BattleAction_EnableFlash = 75, // 将flash1004设为1后推进。
    BattleAction_DisableFlash = 76, // 将flash1004清零后推进。
    BattleAction_EnableBackendDrawFlags = 77, // 按原3D设备分支设置对象绘制位0x100000或0x8000，再推进。
    BattleAction_DisableBackendDrawFlags = 78, // 按原3D设备分支清除对象绘制位0x100000或0x8000，再推进。
    BattleAction_ResetDeathState = 79, // 清理中心／再生附属效果，恢复待机姿态并清除死亡位及stateFE0的2位。
    BattleAction_PlaySound1018 = 80, // 播放soundId1018，保留先读取X再读取音效号的顺序。
    BattleAction_PlaySound101C = 81, // 播放soundId101C；与1018／1020是独立音效槽。
    BattleAction_PlaySound1020 = 82, // 播放soundId1020；不合并其他音效槽。
    BattleAction_ApplyDamageUp = 83, // 逐步放大至1.3阈值，推进后设置DamageUp位；不在客户端计算伤害倍率。
    BattleAction_ClearDamageUp = 84, // 逐步缩回1.0并清除DamageUp位，保留睡眠／石化播放门禁。
    BattleAction_SpinAndEnlarge = 85, // 旋转并放大至2.0阈值后创建特效；技能2002可按名称选图，不设置DamageUp。
    BattleAction_ApplyDamageToTargets = 86, // 逐项消费效果列表伤害／免疫／死亡／解除睡眠等结果，推进目标并启动震屏。
    BattleAction_PlayEarthquakeEffects = 87, // 大地之怒：在对方十个阵型点随机播放特效和音效，计数至270后推进，不在此扣血。
    BattleAction_StartScreenShake = 88, // 设最大振幅6并开启振幅上升，然后推进。
    BattleAction_EndScreenShakeRise = 89, // 结束振幅上升；后续主循环逐步减振幅，不立即归零。
    BattleAction_SweepEffectTargetsAndExit = 90, // 移动并旋转，接近各效果目标时启动目标动作；消费目标后飞出边界并推进。
    BattleAction_SetObjectFlag2AndPlay = 91, // 设置YobiAction::flags的位2，推进后播放一帧；不写hidden字段。
    BattleAction_ClearObjectFlag2AndPlay = 92, // 清除YobiAction::flags的位2，推进后播放一帧；与动作91成对。
    BattleAction_ReleaseUpdateGate = 93, // 按活动条目数决定清全局更新门禁，清aliveFF8后推进；不删除角色。
    BattleAction_OffsetFromEffectTarget = 94, // 从当前目标坐标出发沿其反向角度移动50，再面向目标并推进。
    BattleAction_RushAndZeroTargetHp = 95, // 沿当前角度移动16次后推进，并将效果目标HP置0；保留原写入顺序。
    BattleAction_ShowEffectTarget = 96, // 将当前效果目标hidden置0后推进本角色。
    BattleAction_HideEffectTarget = 97, // 将当前效果目标hidden置1后推进本角色。
    BattleAction_CastWithTargetMarkers = 98, // 播放施法动画6，在帧事件及终帧创建目标标记，并按技能绘制平铺效果。
    BattleAction_SetAnimationRate25 = 99, // 播放速率设为25，推进后清frameIndex／frameRemain；单位不外推。
    BattleAction_PlayAttackAnimationSequence = 100, // 按技能等级选择动画序列表，逐段播放、触发目标反应和命中特效；遇序列负值后推进。
    BattleAction_MoveToSideCenter = 101, // 按目标阵营移至固定中央坐标，抵达后调整朝向、清计数并推进。
    BattleAction_RushThroughTargets = 102, // 先以速度32移动八次，再逐目标处理命中、伤害／吸收／反射及状态，恢复速率后推进。
    BattleAction_SpawnMeleeAttackCopies = 103, // 逐目标创建独立复制体，安排等待、绘制位、移动、近战攻击及销毁队列。
    BattleAction_ShowInspectionResults = 104, // 向当前玩家显示目标生命／魔力或调查失败消息；保留原CryptInt字节消费。
    BattleAction_StepAroundEffectTarget = 105, // 按计数分段切换目标上下左右位置；旧计数到120后推进自身及对应目标。
    BattleAction_SetAnimation13 = 106, // 仅将角色动画设为13后推进，不播放或清帧。
    BattleAction_MarkForRemoval = 107, // 直接MarkDeath当前对象；不清战斗全局槽，区别于51。
    BattleAction_AttackAllTargetsOnFrameEvent = 108, // 动画5的有效帧事件到达后逐目标结算攻击结果，清事件；终帧后推进。
    BattleAction_SpawnChargedStrikeCopy = 109, // 创建图号101031的独立复制体，复制当前效果并安排等待、蓄力攻击110及销毁。
    BattleAction_ChargeAndStrike = 110, // 先累计60次蓄力，再接近目标，按成功／反射结果结算双方HP及命中特效，延迟后推进。
    BattleAction_FlyOutFastAndShakeScreen = 111, // 短暂后退后散射，以速度32反向飞出；离开边界时启动震屏并推进。
    BattleAction_SpawnArcAttackCopies = 112, // 按帧事件为效果目标创建复制体，安排弧线移动及动作114命中，保留等级90特殊队列。
    BattleAction_MoveAlongArc = 113, // 按二次曲线调整显示高度并同步附属对象；到期定位目标坐标、推进并标死附属对象。
    BattleAction_ResolveArcCopyHits = 114, // 逐效果处理弧线复制体命中、反射／吸收、状态及数值；恢复速率并推进。
    BattleAction_PlayAttackClashEffect = 115, // 按反击标志创建工厂type0／3效果，隐藏自身与效果目标，转162等待。
    BattleAction_PlayEffectType1 = 116, // 创建工厂type1（固定图号110063）并转162等待；业务名称未证。
    BattleAction_PlayMagicAttackEffect = 117, // 创建工厂type2攻击魔法受术效果并转162等待；包含反射反馈路径。
    BattleAction_EnterEffectWait = 118, // 不创建新效果，直接转162等待现有效果完成；保留原缺少创建分支的行为。
    BattleAction_PlayDebuffSpellEffect = 119, // 播放异常魔法受术特效（工厂type4），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayBarrierSpellEffect = 120, // 播放物理／魔法反射、吸收、无效化共用受术特效（工厂type5），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayPhysicalReflectEffect = 121, // 播放物理反射特效（工厂type7），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayPhysicalAbsorbEffect = 122, // 播放物理吸收特效（工厂type8），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayPhysicalNullifyEffect = 123, // 播放物理无效化特效（工厂type9），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayMagicReflectEffect = 124, // 播放魔法反射特效（工厂type10），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayMagicAbsorbEffect = 125, // 播放魔法吸收特效（工厂type11），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayMagicNullifyEffect = 126, // 播放魔法无效化特效（工厂type12），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_StartPhysicalReflectEffect = 127, // 创建物理反射特效（工厂type7、link=0）后立即推进，不等待效果结束。
    BattleAction_StartPhysicalAbsorbEffect = 128, // 创建物理吸收特效（工厂type8、link=0）后立即推进，不等待效果结束。
    BattleAction_StartPhysicalNullifyEffect = 129, // 创建物理无效化特效（工厂type9、link=0）后立即推进，不等待效果结束。
    BattleAction_PlayRecoveryEffect = 130, // 播放持续回复魔法受术特效（工厂type13），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayEffectType14 = 131, // 创建工厂type14的等级分档效果并转162等待；可由状态报文mode1安排。
    BattleAction_PlayEffectType15 = 132, // 创建工厂type15的等级分档效果并转162等待；业务名称未证。
    BattleAction_PlayStatusRecoveryEffect = 133, // 播放异常恢复魔法受术特效（工厂type25），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayEffectType26 = 134, // 创建工厂type26（图号110156）并转162等待；不同技能可共用。
    BattleAction_PlayEffectType27 = 135, // 创建工厂type27（图号111162）并转162等待，不按技术号2053猜业务名。
    BattleAction_PlayEffectType28 = 136, // 创建工厂type28（图号111163）并转162等待，不按技术号2054猜业务名。
    BattleAction_PlayHealingCastEffect = 137, // 补血技能生产的施放侧效果：工厂type29，创建后转162等待。
    BattleAction_PlayItemUseEffect = 138, // 物品使用入口安排的工厂type20效果，创建后转162等待。
    BattleAction_CastSpellAndTriggerTargets = 139, // 播放施法动画及type34效果，按帧事件启动目标或创建魔法效果；终帧清理并推进。
    BattleAction_TriggerSpellEffectOrHealTarget = 140, // 攻击魔法创建type35效果，补血魔法推进当前目标；随后推进自身。
    BattleAction_ApplyDebuff = 141, // 按模式／类型创建异常效果1054；可设置异常位，含2089／2090扩展图。
    BattleAction_ClearDebuff = 142, // 清除1054及异常位；推进后回写当前减益类型并重置状态效果模式。
    BattleAction_ApplyStatusEffect = 143, // 创建能力变化／扩展状态效果1068；支持负向能力变化，并非仅增益。
    BattleAction_ClearStatusEffect = 144, // 清除1068及能力变化位；不等于清除角色全部状态。
    BattleAction_CreatePrayerField = 145, // 按四属性祈祷技能创建场地效果，随后推进。
    BattleAction_PlayAttributeReversalEffect = 146, // 播放属性反转受术特效（工厂type6），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_ToggleAttributeReversal = 147, // 按ReverseType位创建或清除反转效果105C，同时切换该状态位。
    BattleAction_PlayReviveEffect = 148, // 播放复活受术特效（工厂type33）并转162等待；死亡位重置及HP恢复由后续动作处理。
    BattleAction_PlayHealEffect = 149, // 播放补血魔法受术特效（工厂type16），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayAmountTieredRecoveryEffect = 150, // 按value1030的2000／3500阈值选恢复图像（工厂type18），转162等待。
    BattleAction_PlayLevelScaledEffect21 = 151, // 创建工厂type21等级分档效果，部分技能使用castLevel1040；转162等待。
    BattleAction_PlayParentBoundEffect22 = 152, // 创建工厂type22、图号110097的效果，父对象死亡时结束；转162等待。
    BattleAction_PlayParentBoundEffect23 = 153, // 创建工厂type23、图号110096的效果，父对象死亡时结束；转162等待。
    BattleAction_StartScalingEffect24 = 154, // 创建并保存工厂type24缩放效果，等待其phase为1后推进本角色。
    BattleAction_PulseScaleWithParentEffect = 155, // 创建type22父对象效果，按原计数往复缩放角色并恢复，结束时令保存效果进入phase2。
    BattleAction_WaitScalingEffect24End = 156, // 等待保存的type24效果phase为3，标死、清槽后推进角色。
    BattleAction_PlayParameterChangeEffect = 157, // 播放能力变化魔法受术特效（工厂type37），转162等待；本动作不直接施加状态或结算数值。
    BattleAction_PlayEffectType38 = 158, // 按技能等级选择工厂type38图像并转162等待；业务名称未证。
    BattleAction_ApplyResourceChange = 159, // 按numberDisplayType1034显示数值并更新HP／FP，随后推进动作队列。
    BattleAction_ApplyResourceChangeAndWait = 160, // 同159处理HP／FP，但只转动作37等待，不立即推进队列。
    BattleAction_PlayBombLevelEffect = 161, // 创建工厂type39，按gBattleBombLevelD67B2C选择图像，转162等待。
    BattleAction_WaitEffectComplete = 162, // 等待effectPlaybackState1024为2，再清标记、显示角色并推进。
    BattleAction_TransitionRebirthForm = 163, // 按结果标志进入或退出Rebirth形态，处理换图、透明度及形态附属效果，不是死亡复活。
    BattleAction_RestoreRebirthBaseForm = 164, // 恢复Rebirth前的基础图像、状态位和透明度，结束后返回保存的角色phase。
    BattleAction_ReactToCriticalHit = 165, // 受击后退后持续旋转、减速和等待；与27的可选旋转不同，最后按死亡标志推进。
    BattleAction_ConsumeQueuedTime = 166, // 当前玩家消费排队时间值，显示负时间数字并推进排队值及动作。
    BattleAction_EnableAlphaOverride = 167, // 将batchType设为8，只覆盖绘制颜色的Alpha通道，然后推进。
    BattleAction_ClearColorOverrides = 168, // 将batchType清零，停止覆盖绘制颜色通道，然后推进。
    BattleAction_OverrideAllMixFlags = 169, // 将混合标志覆盖掩码rebirthValue1设为255，使用既有rebirthValue2覆盖全部混合位。
    BattleAction_ClearMixFlagOverride = 170, // 将混合标志覆盖掩码rebirthValue1清零，然后推进。
    BattleAction_FadeIn = 171, // 递增角色及附属对象透明度至255后推进，保留设备绘制位处理。
    BattleAction_FadeOutStep3 = 172, // 角色及附属对象透明度每次减3；归零后设置隐藏相关位并推进。
    BattleAction_FadeOutStep2 = 173, // 角色及附属对象透明度每次减2；与172保持不同速度。
    BattleAction_SelectPalette = 174, // 用当前效果的position／position3切换调色板，推进效果及动作。
    BattleAction_StartBackgroundFade = 175, // 创建或复用effect1060背景渐变对象并将其phase设为0，然后推进。
    BattleAction_EndBackgroundFade = 176, // 将effect1060的phase设为2并清角色槽；不是直接MarkDeath。
    BattleAction_PlayBackgroundFade = 177, // 创建不保存到effect1060的背景渐变，工厂末参1，然后推进。
    BattleAction_AdvanceAndIdle = 178, // 先推进队列游标，再将角色phase设为待机0。
    BattleAction_PlayAnimation6 = 179, // 播放动画6，终帧后推进，不额外创建施法效果。
    BattleAction_PlayRecoveryEffectAndFadeIn = 180, // 播放持续回复魔法受术特效（工厂type13），转204渐显、按结果结算HP／FP并等待；不同于动作130。
    BattleAction_PlayEffectType1AndFadeIn = 181, // 创建工厂type1后转204渐显、处理结果及等待；业务名称未证。
    BattleAction_PlayMagicAttackEffectAndFadeIn = 182, // 创建type2攻击魔法受术效果后转204渐显、处理结果及等待。
    BattleAction_PlayStatusRecoveryEffectAndFadeIn = 183, // 播放异常恢复魔法受术特效（工厂type25），转204渐显、按结果结算HP／FP并等待；不同于动作133。
    BattleAction_PlayHealEffectAndFadeIn = 184, // 播放补血魔法受术特效（工厂type16），转204渐显、按结果结算HP／FP并等待；不同于动作149。
    BattleAction_PlayLevelScaledEffect21AndFadeIn = 185, // 创建工厂type21等级分档效果后转204渐显、处理结果及等待。
    BattleAction_PlayParentBoundEffect22AndFadeIn = 186, // 创建工厂type22父对象生命周期关联效果后转204渐显、处理结果及等待。
    BattleAction_PlayParentBoundEffect23AndFadeIn = 187, // 创建工厂type23父对象生命周期关联效果后转204渐显、处理结果及等待。
    BattleAction_EnterFadeInAndResolveEffect = 188, // 不创建新效果，直接转204渐显、处理结果及等待；保留缺少创建分支的行为。
    BattleAction_PlayDebuffSpellEffectAndFadeIn = 189, // 播放异常魔法受术特效（工厂type4），转204渐显、按结果结算HP／FP并等待；不同于动作119。
    BattleAction_PlayBarrierSpellEffectAndFadeIn = 190, // 播放物理／魔法反射、吸收、无效化共用受术特效（工厂type5），转204渐显、按结果结算HP／FP并等待；不同于动作120。
    BattleAction_PlayPhysicalReflectEffectAndFadeIn = 191, // 播放物理反射特效（工厂type7），转204渐显、按结果结算HP／FP并等待；不同于动作121。
    BattleAction_PlayPhysicalAbsorbEffectAndFadeIn = 192, // 播放物理吸收特效（工厂type8），转204渐显、按结果结算HP／FP并等待；不同于动作122。
    BattleAction_PlayPhysicalNullifyEffectAndFadeIn = 193, // 播放物理无效化特效（工厂type9），转204渐显、按结果结算HP／FP并等待；不同于动作123。
    BattleAction_PlayMagicReflectEffectAndFadeIn = 194, // 播放魔法反射特效（工厂type10），转204渐显、按结果结算HP／FP并等待；不同于动作124。
    BattleAction_PlayMagicAbsorbEffectAndFadeIn = 195, // 播放魔法吸收特效（工厂type11），转204渐显、按结果结算HP／FP并等待；不同于动作125。
    BattleAction_PlayMagicNullifyEffectAndFadeIn = 196, // 播放魔法无效化特效（工厂type12），转204渐显、按结果结算HP／FP并等待；不同于动作126。
    BattleAction_PlayEffectType26AndFadeIn = 197, // 创建工厂type26后转204渐显、处理结果及等待；业务名称未证。
    BattleAction_PlayHealingCastEffectAndFadeIn = 198, // 创建工厂type29补血施放侧效果后转204渐显、处理结果及等待。
    BattleAction_PlayEffectType14AndFadeIn = 199, // 创建工厂type14后转204渐显、处理结果及等待；业务名称未证。
    BattleAction_PlayEffectType15AndFadeIn = 200, // 创建工厂type15后转204渐显、处理结果及等待；业务名称未证。
    BattleAction_PlayAttributeReversalEffectAndFadeIn = 201, // 播放属性反转受术特效（工厂type6），转204渐显、按结果结算HP／FP并等待；不同于动作146。
    BattleAction_PlayParameterChangeEffectAndFadeIn = 202, // 播放能力变化魔法受术特效（工厂type37），转204渐显、按结果结算HP／FP并等待；不同于动作157。
    BattleAction_PlayBombLevelEffectAndFadeIn = 203, // 创建工厂type39的炸弹等级分档效果后转204渐显、处理结果及等待。
    BattleAction_FadeInAndResolveEffect = 204, // 逐步渐显，特效完成后按结果显示免疫或结算HP／FP；再等待60次计数且完全不透明才推进。
    BattleAction_PlayAnimation16AndResetFrames = 205, // 播放动画16，终帧后清帧索引和剩余帧再推进，保留帧事件。
    BattleAction_FaceEffectTarget = 206, // 朝当前效果记录的目标角色转向后推进。
    BattleAction_MoveToEffectPosition = 207, // 从效果position／flags读取目的坐标，推进效果后切入移动动作6。
    BattleAction_MoveBesideEffectTarget = 208, // 按目标屏幕分区设置偏移落点，再转动作6移动，不直接推进队列。
    BattleAction_ReactToHitAndStrikeLinkedTarget = 209, // 受击后退和减速期间，在指定计数对关联目标结算后续攻击，再按死亡标志收尾。
    BattleAction_RushWithTrail = 210, // 以速度16接近效果目标并生成随机尾迹；距离小于70后恢复速度8及播放率。
    BattleAction_ApplySceneEffects = 211, // 按效果flags四个位写入两个场景效果全局的各16位分量，推进效果及动作。
    BattleAction_RepeatOffsetEffect = 212, // 反复创建指定图号／XY偏移效果；计数达到记录flags阈值后推进效果及动作。
    BattleAction_HandleDeathAndContinue = 213, // 死亡动画后清理常驻效果，外观恢复或渐隐后继续队列；保留再生分流。
    BattleAction_FadeOutAndContinue = 214, // 按后端闪烁／渐隐，结束后推进队列；不同于48进入角色移除。
    BattleAction_WaitFadeEffectAndContinue = 215, // 等待播放状态2并清标记后推进队列；不转51移除角色。
    BattleAction_CastMonsterSpellAndTriggerTargets = 216, // 播放怪物法术的施法效果；帧事件时处理255哨兵目标列表，启动目标并按技能创建效果。
    BattleAction_PlayRandomOffsetEffect40 = 217, // 创建工厂type40，在角色图像范围内随机偏移效果坐标并设置后端绘制位，转162等待。
    BattleAction_ChangeAppearanceWithCrossFade = 218, // 创建换图副体并交叉渐变，按效果记录切换外观、保存基础图号和覆盖状态后推进。
    BattleAction_RestoreAppearanceAfterDeath = 219, // 恢复基础外观后按玩家／宠物分类继续死亡流程，非玩家宠物转48。
    BattleAction_RestoreAppearanceBeforeFade = 220, // 恢复基础外观完成后转214渐隐继续流程。
    BattleAction_ApplyFollowupHit = 221, // 先消费攻击者记录，再按下一条目标记录结算追加命中／反射／吸收与状态，推进动作及效果。
    BattleAction_MoveLinkedProjectileToTarget = 222, // 关联投射物接近目标后销毁；特定回旋技能则推进目标／效果并转22继续回旋命中。
    BattleAction_UpdatePetSnapshot = 223, // 按效果position设置宠物快照索引；-2时同步状态UI并清快照，再推进。
    BattleAction_TransitionRideState = 224, // 按效果flags进入或解除骑乘，处理骑手运动、角色槽转移、装备和状态效果恢复。
    BattleAction_FailToMountAndReturn = 225, // 骑乘失败入口创建骑手并播放抛物线运动，随后还原角色图像及坐标并清理临时对象。
    BattleAction_DismountAndRestoreActors = 226, // 骑乘退出时播放骑手受击／抛物线运动，恢复原角色槽、装备及状态并销毁合成对象。
    BattleAction_DismountWithActorAnimation = 227, // 解除骑乘时复制本角色动画并按结果选择骑手动画，落地后恢复原角色和挂件。
    BattleAction_StartAnimationSlowdown = 228, // 清计数，转229并立即执行首次减速动画更新。
    BattleAction_AnimateSlowdown = 229, // 角色动画3播放率按250-4*计数递减；计数60后清零并推进，不改变移动速度。
    BattleAction_MoveAlongEffectPath = 230, // 按效果position／flags中的XY坐标转向移动；距离小于8后落点并推进效果及动作。
    BattleAction_DecelerateWithTrail = 231, // 每次速度减0.45至0并产生交替尾迹；停下后推进并恢复速度6。
    BattleAction_RushIntoTargetWithTrail = 232, // 动画3冲向目标，近距离结算命中／反射／吸收及数值，逐帧产生随机尾迹。
    BattleAction_ReactToHitWithoutDisplacement = 233, // 保留受击计数、旋转、尾迹及死亡门禁；不调用移动函数，与27／165保持独立。
    BattleAction_BounceOutImmediately = 234, // 首计数即创建散射与音效，旋转并在边界反弹，飞出后推进；没有34的前段受击后退。
    BattleAction_ApplyEffectFlags = 235, // 按本角色位置调用ApplyBattleEffectFlags，再推进动作。
    BattleAction_ResetBusyAndIdle = 236, // 清busyFD8并转待机，立即执行待机处理；不等同于只写phase=0。
    BattleAction_RushAndResolveHitWithin16Ticks = 237, // 前16次移动并按距离触发命中，达到计数时也处理目标结果；之后继续移动及队列收尾。
    BattleAction_ResolveDeceleratingProjectileHit = 238, // 处理目标命中及等待；等待结束后每次减速0.5，低速时转239加速。
    BattleAction_ResolveAcceleratingProjectileHit = 239, // 与238共用命中逻辑，等待结束后每次加速0.5，命中后按原条件销毁或飞出。
    BattleAction_PrepareArcShots = 240, // 创建施法效果并播放；帧事件到达后清效果、计数，转241发射。
    BattleAction_FireArcShotsSequentially = 241, // 精神／混乱冲击波逐目标发射弧形弹道，目标间等待计数6，保留帧事件及音效时序。
    BattleAction_FireArcShotsTogether = 242, // 精神／混乱冲击波一次遍历全部目标发射弧形弹道，随后推进；无241的逐目标延迟。
    BattleAction_ResolveStraightProjectileHit = 243, // 直线投射物按距离及帧事件结算命中，等待后继续移动；无238／239的加减速切换。
    BattleAction_MarkDeadAndAcquireGate = 244, // 设置死亡位、全局更新门禁和aliveFF8，再推进；不同于47仅置死亡位。
    BattleAction_CreateTiledEffect = 245, // 按效果position／position3指定平铺类型和寿命，推进效果及动作。
    BattleAction_SetSpeedFromEffect = 246, // 用效果position3乘0.01设置motionMagnitude54，再推进效果及动作。
    BattleAction_SavePosition = 247, // 将当前浮点XY分别截断到载荷x10／y14，再推进。
    BattleAction_ReturnToSavedPosition = 248, // 移回x10／y14保存坐标，距离小于5后定位并推进；不同于回阵型坐标。
    BattleAction_FireBlastWaves = 249, // 帧事件触发时按技能／等级逐目标发射冲击波，播放音效268并清事件。
    BattleAction_PlayConfiguredEffect41 = 250, // 按image102C创建工厂type41效果并转162等待；保留与type42的独立编号。
    BattleAction_SetHpFpOne = 251, // 将本角色HP和FP同时置1后推进，不在此清除死亡状态。
    BattleAction_PlayConfiguredEffect42 = 252, // 按image102C创建工厂type42效果并转162等待；不与动作250合并。
    BattleAction_CreateCastingEffect = 253, // 本动作且播放标记为0时创建type43施法效果并保存battleCastEffect10，然后推进。
    BattleAction_ClearCastingEffect = 254, // 标死battleCastEffect10并清槽／播放标记，然后推进。
    BattleAction_ApplyAreaAttackOnFrameEvent = 255, // 动画5有效帧事件时逐目标结算范围攻击，首目标创建type47效果，终帧后推进。
    BattleAction_RequestBattleExit = 256, // 清事件分派忙标志并请求退出战斗；不在这里销毁角色。
};

// BAN01：未证业务名的动画／特效按已证动画号、工厂type或字段行为命名；不把同值的技能、图号和特效phase混用。

// BSI01：客户端分派技能号；用于gBattleMSkill5AAA78、castSkill103C及skill7C的已归一化消费。
// 保持各字段为有符号整数，允许未知／Lua技能；不用于原始BattleSkillMeta::skillId注册键。
// data技术号与所属技能号不一致时取techId / 100；不改变现有205000报文归一化阈值。
// Strong／Super仅对应data的强力／超强名称；每个成员的业务／用户来源见注释。
enum BattleSkillDispatchId : std::int32_t {
    BattleSkill_ContinuationAttack = 0, // 连击；TECH_ContinuationAttack。
    BattleSkill_Parameter_DoubleEdgedBlade = 1, // 诸刃；TECH_Parameter，名称译名区分共用处理函数。
    BattleSkill_Parameter_AllOrNothing = 3, // 乾坤一掷；TECH_Parameter，名称译名区分共用处理函数。
    BattleSkill_SpiracleShot = 4, // 气功弹；TECH_SpiracleShot。
    BattleSkill_GuardBreak = 5, // 崩击；TECH_GuardBreak／GuardBreakEx。
    BattleSkill_Forcecut = 6, // 战栗袭心；TECH_Forcecut。
    BattleSkill_SpecialGuard = 8, // 圣盾；TECH_SpecialGuard。
    BattleSkill_CrossCounter = 11, // 反击；TECH_CrossCounter。
    BattleSkill_Consentration = 12, // 明镜止水；保留TECH_Consentration来源拼写，与扩展集中技能不同。
    BattleSkill_AttackMagicEarth = 19, // 陨石／地魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWater = 20, // 冰冻／水魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicFire = 21, // 火焰／火魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWind = 22, // 风刃／风魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicEarthStrong = 23, // 强力陨石／地魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWaterStrong = 24, // 强力冰冻／水魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicFireStrong = 25, // 强力火焰／火魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWindStrong = 26, // 强力风刃／风魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicEarthSuper = 27, // 超强陨石／地魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWaterSuper = 28, // 超强冰冻／水魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicFireSuper = 29, // 超强火焰／火魔法；TECH_Attack_Magic。
    BattleSkill_AttackMagicWindSuper = 30, // 超强风刃／风魔法；TECH_Attack_Magic。
    BattleSkill_Dorain = 31, // 吸血魔法；保留TECH_Dorain来源拼写。
    BattleSkill_StatusChangePoison = 32, // 中毒魔法；TECH_StatusChange。
    BattleSkill_StatusChangeSleep = 33, // 昏睡魔法；TECH_StatusChange。
    BattleSkill_StatusChangeStone = 34, // 石化魔法；TECH_StatusChange。
    BattleSkill_StatusChangeInebriety = 35, // 酒醉魔法；TECH_StatusChange。
    BattleSkill_StatusChangeConfusion = 36, // 混乱魔法；TECH_StatusChange。
    BattleSkill_StatusChangeForget = 37, // 遗忘魔法；TECH_StatusChange。
    BattleSkill_StatusChangePoisonStrong = 38, // 强力中毒魔法；TECH_StatusChange。
    BattleSkill_StatusChangeSleepStrong = 39, // 强力昏睡魔法；TECH_StatusChange。
    BattleSkill_StatusChangeStoneStrong = 40, // 强力石化魔法；TECH_StatusChange。
    BattleSkill_StatusChangeInebrietyStrong = 41, // 强力酒醉魔法；TECH_StatusChange。
    BattleSkill_StatusChangeConfusionStrong = 42, // 强力混乱魔法；TECH_StatusChange。
    BattleSkill_StatusChangeForgetStrong = 43, // 强力遗忘魔法；TECH_StatusChange。
    BattleSkill_StatusChangePoisonSuper = 44, // 超强中毒魔法；TECH_StatusChange。
    BattleSkill_StatusChangeSleepSuper = 45, // 超强昏睡魔法；TECH_StatusChange。
    BattleSkill_StatusChangeStoneSuper = 46, // 超强石化魔法；TECH_StatusChange。
    BattleSkill_StatusChangeInebrietySuper = 47, // 超强酒醉魔法；TECH_StatusChange。
    BattleSkill_StatusChangeConfusionSuper = 48, // 超强混乱魔法；TECH_StatusChange。
    BattleSkill_StatusChangeForgetSuper = 49, // 超强遗忘魔法；TECH_StatusChange。
    BattleSkill_TreatTypeEarth = 50, // 大地的祈祷；TECH_Treat_Type。
    BattleSkill_TreatTypeWater = 51, // 海洋的祈祷；TECH_Treat_Type。
    BattleSkill_TreatTypeFire = 52, // 火焰的祈祷；TECH_Treat_Type。
    BattleSkill_TreatTypeWind = 53, // 云群的祈祷；TECH_Treat_Type。
    BattleSkill_ReverseType = 54, // 属性反转；TECH_Reverse_Type。
    BattleSkill_ReflectionPhysics = 55, // 攻击反弹；TECH_Reflection_Physics。
    BattleSkill_ReflectionMagic = 56, // 魔法反弹；TECH_Reflection_Magic。
    BattleSkill_AbsorbPhysics = 57, // 攻击吸收；TECH_Absorb_Physics。
    BattleSkill_AbsorbMagic = 58, // 魔法吸收；TECH_Absorb_Magic。
    BattleSkill_IneffectivePhysics = 59, // 攻击无效；TECH_Ineffective_Physics。
    BattleSkill_IneffectiveMagic = 60, // 魔法无效；TECH_Ineffective_Magic。
    BattleSkill_HealMagic = 61, // 补血魔法；TECH_Heal_Magic。
    BattleSkill_HealMagicStrong = 62, // 强力补血魔法；TECH_Heal_Magic。
    BattleSkill_HealMagicSuper = 63, // 超强补血魔法；TECH_Heal_Magic。
    BattleSkill_LpRecovery = 64, // 恢复魔法；TECH_LpRecovery。
    BattleSkill_LpRecoveryStrong = 65, // 强力恢复魔法；TECH_LpRecovery。
    BattleSkill_LpRecoverySuper = 66, // 超强恢复魔法；TECH_LpRecovery。
    BattleSkill_StatusRecover = 67, // 洁净魔法；TECH_StatusRecover。
    BattleSkill_Revive = 68, // 气绝回复；TECH_Revive。
    BattleSkill_NormalAttack = 73, // 攻击；TECH_NormalAttack。
    BattleSkill_StatusAttackPoison = 75, // 中毒攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_StatusAttackSleep = 76, // 昏睡攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_StatusAttackStone = 77, // 石化攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_StatusAttackInebriety = 78, // 酒醉攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_StatusAttackConfusion = 79, // 混乱攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_StatusAttackForget = 80, // 遗忘攻击；TECH_StatusAttack默认异常，可被MD:覆盖。
    BattleSkill_PanicAttack = 94, // 混乱攻击；TECH_PanicAttack，随机异常，与79固定混乱不同。
    BattleSkill_RandomShot = 95, // 乱射；TECH_RandomShot。
    BattleSkill_Assassin = 96, // 暗杀；TECH_Assassin。
    BattleSkill_EarthQuake = 97, // 大地之怒；TECH_EarthQuake。
    BattleSkill_Dance = 98, // 跳舞；TECH_Dance。
    BattleSkill_DefUp = 99, // 捷舞；TECH_DefUp，防御上升。
    BattleSkill_DefDown = 100, // 霹雳舞；TECH_DefDown，防御下降。
    BattleSkill_AtkUp = 101, // 黏巴达舞；TECH_AtkUp，攻击上升。
    BattleSkill_AtkDown = 102, // 梭舞／灵魂舞；TECH_AtkDown，攻击下降。
    BattleSkill_AglUp = 103, // 啪啦啪啦舞；TECH_AglUp，敏捷上升。
    BattleSkill_AglDown = 104, // 浩室舞；TECH_AglDown，敏捷下降。
    BattleSkill_Ranbu = 105, // 飞燕乱舞；TECH_Ranbu，用户指定105保留Ranbu；挑衅另用2008。
    BattleSkill_UltimateAttack = 112, // 狮子吼；TECH_UltimateAttack，所属107的技术11201按整数除100。
    BattleSkill_RecoveryUp = 114, // 回复能力上升；服务器RCVUP宏与客户端独立114路径，当前data缺记录。
    BattleSkill_DelayAttack257 = 257, // 戒骄戒躁；TECH_DelayAttack，技术25700／100；子类保留编号，所属技能1001。
    BattleSkill_DelayAttack258 = 258, // 一击必中；TECH_DelayAttack，技术25800／100；子类保留编号，所属技能1002。
    BattleSkill_StatusAttackPoisonStrike = 259, // 毒击；TECH_StatusAttack，技术25900／100；PUK2_SKILL下默认中毒，与75不合并。
    BattleSkill_Wattack = 260, // 一石二鸟；TECH_Wattack，技术26000／100，所属技能1004。
    BattleSkill_FirstAttack = 262, // 迅速果断；TECH_FirstAttack，技术26200／100，所属技能1006。
    BattleSkill_Copy = 263, // 羊头狗肉；TECH_Copy，技术26300／100，所属技能1007。
    BattleSkill_Madmax = 266, // 因果报应；TECH_Madmax，技术26600／100，所属技能1010。
    BattleSkill_MagicMissile = 267, // 精神冲击波；TECH_MagicMissile，技术26700／100，所属技能1011。
    BattleSkill_MonWind = 400, // 突风；TECH_MonWind，技术40030／100，所属技能257。
    BattleSkill_Acid = 401, // 酸性攻击；TECH_Acid，技术40130／100，data名称为アシッド。
    BattleSkill_Barrier = 402, // 屏障；TECH_Barrier，技术40230／100，data名称为バリア。
    BattleSkill_RiderBreakAttack = 404, // 人宠合击ATK；TECH_RiderBreak，技术40430／100。
    BattleSkill_LvSet = 405, // 等级控制；TECH_LvSet，技术40530／100。
    BattleSkill_RiderBreakMind = 406, // 人宠合击MND；TECH_RiderBreak，技术40630／100。
    BattleSkill_Concentration = 2006, // 集中；用户确认名称，英文为项目译名。
    BattleSkill_Recklessness = 2007, // 鲁莽；用户确认名称，英文为项目译名。
    BattleSkill_Taunt = 2008, // 挑衅；用户确认名称，英文为项目译名。用户指定挑衅按2008；105不命名为Taunt。
    BattleSkill_ElementalAmplification = 2010, // 元素增幅；用户确认名称，英文为项目译名。
    BattleSkill_Blessing = 2011, // 祝福；用户确认名称，英文为项目译名。
    BattleSkill_DefensiveBarrier = 2012, // 防御结界；用户确认名称，英文为项目译名。
    BattleSkill_VoodooCurse = 2013, // 巫毒诅咒；用户确认名称，英文为项目译名。
    BattleSkill_BattleCommand = 2014, // 战斗号令；用户确认名称，英文为项目译名。
    BattleSkill_BideTime = 2015, // 伺机而动；用户确认名称，英文为项目译名。
    BattleSkill_ShortStrike = 2016, // 短击；用户确认名称，英文为项目译名。
    BattleSkill_ShadowHand = 2017, // 暗影之手；用户确认名称，英文为项目译名。
    BattleSkill_SoloDance = 2019, // 独舞；用户确认名称，英文为项目译名。
    BattleSkill_MercyStrike = 2021, // 手下留情；用户确认名称，英文为项目译名。
    BattleSkill_KarmaCycle = 2022, // 因果轮回；用户确认名称，英文为项目译名。
    BattleSkill_Adrenaline = 2027, // 肾上腺素；用户确认名称，英文为项目译名。
    BattleSkill_EmergencyTransfusion = 2028, // 紧急输血；用户确认名称，英文为项目译名。
    BattleSkill_StatusAttackPoison2096 = 2096, // 独立中毒异常攻击；图号110075及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_StatusAttackSleep2097 = 2097, // 独立昏睡异常攻击；图号110076及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_StatusAttackStone2098 = 2098, // 独立石化异常攻击；图号110077及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_StatusAttackInebriety2099 = 2099, // 独立酒醉异常攻击；图号110078及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_StatusAttackConfusion2100 = 2100, // 独立混乱异常攻击；图号110079及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_StatusAttackForget2101 = 2101, // 独立遗忘异常攻击；图号110080及状态位已证，正式名称未确认，保留编号区分75～80。
    BattleSkill_ConfusionShockwave = 3029, // 混乱冲击波；用户确认名称，英文为项目译名。
    BattleSkill_SwallowSlash = 3100, // 燕斩；用户确认名称，英文为项目译名。
    BattleSkill_BladeSoul = 3101, // 锋上魂；用户确认名称，英文为项目译名。
    BattleSkill_WindRendingStrike = 4003, // 裂风击；用户确认名称，英文为项目译名。
};

// BSI01未命名项仍保留原数字：不能仅凭共享处理入口或图像推导同一业务。
// 72、81～88、93、106～111、113、117、2002、2005：尚未完成本编号域的名称对应核实。
// 2050～2052、2058～2067、2089～2095、3002～3016：仅确认独立技能／图号，未提供业务名。
// 2055～2057、2068～2079、3001、3019、3037、3038、4002：已有分支不足以确定名称。
// 4004仅作为现有比较的区间上界保留，不据此定义技能名称。
// 2009鹰眼为用户提供名称，本轮未找到独立消费分支，暂不定义无消费者的枚举项。

// BAF01：BC及相关角色记录的协议位；与BM命中标志、服务器CHAR_BATTLEFLG分别定义。
// 数值来源及客户端承接差异见 doc/cg2d_battle_actor_flags_spec.md。
enum BattleCharacterFlag : std::uint32_t {
    BattleCharacterFlag_Appear = 0x00000001u,       // 协议要求播放入场；FDC运行位反向承接。
    BattleCharacterFlag_Dead = 0x00000002u,         // 倒地／死亡。
    BattleCharacterFlag_Player = 0x00000004u,       // 玩家类别；部分骑乘宠物也按此类别编码。
    BattleCharacterFlag_Pet = 0x00000008u,          // 宠物类别，服务端BC_FLAG_MON。
    BattleCharacterFlag_Poison = 0x00000010u,       // 中毒。
    BattleCharacterFlag_Sleep = 0x00000020u,        // 睡眠。
    BattleCharacterFlag_Stone = 0x00000040u,        // 石化。
    BattleCharacterFlag_Inebriety = 0x00000080u,    // 醉酒，对应ModDrunk。
    BattleCharacterFlag_Confusion = 0x00000100u,    // 混乱。
    BattleCharacterFlag_Forget = 0x00000200u,       // 遗忘。
    BattleCharacterFlag_TwoActions = 0x00000400u,   // 双动。
    BattleCharacterFlag_ReverseType = 0x00000800u,  // 属性反转。
    BattleCharacterFlag_DamageUp = 0x00001000u,     // 伤害增强。
    BattleCharacterFlag_Morph = 0x00002000u,        // 变身。
    BattleCharacterFlag_DefenseUp = 0x00004000u,    // 防御上升。
    BattleCharacterFlag_DefenseDown = 0x00008000u,  // 防御下降。
    BattleCharacterFlag_AttackUp = 0x00010000u,     // 攻击上升。
    BattleCharacterFlag_AttackDown = 0x00020000u,   // 攻击下降。
    BattleCharacterFlag_AgilityUp = 0x00040000u,    // 敏捷上升。
    BattleCharacterFlag_AgilityDown = 0x00080000u,  // 敏捷下降。
    BattleCharacterFlag_RecoveryUp = 0x00100000u,   // 回复能力上升。
    BattleCharacterFlag_Earth = 0x01000000u,        // 地属性。
    BattleCharacterFlag_Water = 0x02000000u,        // 水属性。
    BattleCharacterFlag_Fire = 0x04000000u,         // 火属性。
    BattleCharacterFlag_Wind = 0x08000000u,         // 风属性。
    BattleCharacterFlag_Rebirth = 0x10000000u,     // REB形态，不是倒地后的复活动作。
};

// BattleActorPayload::flagsFDC的运行位；存储仍为uint32_t，保留组合位及未知位。
enum BattleActorFlag : std::uint32_t {
    BattleActorFlag_EntryComplete = 0x00000001u, // 入场／归位完成；不能直接当作协议Appear。
    BattleActorFlag_Dead = 0x00000002u,         // 倒地／死亡。
    BattleActorFlag_Player = 0x00000004u,       // 玩家类别。
    BattleActorFlag_Pet = 0x00000008u,          // 宠物类别。
    BattleActorFlag_Poison = 0x00000010u,       // 中毒，特效110075。
    BattleActorFlag_Sleep = 0x00000020u,        // 睡眠，特效110076。
    BattleActorFlag_Stone = 0x00000040u,        // 石化，特效110077。
    BattleActorFlag_Inebriety = 0x00000080u,    // 醉酒，特效110078。
    BattleActorFlag_Confusion = 0x00000100u,    // 混乱，特效110079。
    BattleActorFlag_Forget = 0x00000200u,       // 遗忘，特效110080。
    BattleActorFlag_TwoActions = 0x00000400u,   // 双动；BC路径仅更新附着效果，不补写此位。
    BattleActorFlag_ReverseType = 0x00000800u,  // 属性反转，特效110152。
    BattleActorFlag_DamageUp = 0x00001000u,     // 伤害增强；放大／缩小动作完成时更新。
    BattleActorFlag_Morph = 0x00002000u,        // 变身，用于绘制与挂件处理。
    BattleActorFlag_DefenseUp = 0x00004000u,    // 防御上升，特效103018。
    BattleActorFlag_DefenseDown = 0x00008000u,  // 防御下降，特效103019。
    BattleActorFlag_AttackUp = 0x00010000u,     // 攻击上升，特效103020。
    BattleActorFlag_AttackDown = 0x00020000u,   // 攻击下降，特效103021。
    BattleActorFlag_AgilityUp = 0x00040000u,    // 敏捷上升，特效103022。
    BattleActorFlag_AgilityDown = 0x00080000u,  // 敏捷下降，特效103023。
    BattleActorFlag_RecoveryUp = 0x00100000u,   // 回复能力上升，103033分支使用103018显示。
    BattleActorFlag_Earth = 0x01000000u,        // 地属性，fields288[0]。
    BattleActorFlag_Water = 0x02000000u,        // 水属性，fields288[1]。
    BattleActorFlag_Fire = 0x04000000u,         // 火属性，fields288[2]。
    BattleActorFlag_Wind = 0x08000000u,         // 风属性，fields288[3]。
    BattleActorFlag_Rebirth = 0x10000000u,     // REB形态及其持续特效。
};

// 仅替换既有组合掩码；睡眠／石化播放门禁不代表全部不可行动状态。
inline constexpr std::uint32_t BattleActorSleepOrStoneMask = BattleActorFlag_Sleep | BattleActorFlag_Stone;
inline constexpr std::uint32_t BattleActorPlayerOrPetMask = BattleActorFlag_Player | BattleActorFlag_Pet;
inline constexpr std::uint32_t BattleActorAbnormalMask = BattleActorFlag_Poison | BattleActorFlag_Sleep
    | BattleActorFlag_Stone | BattleActorFlag_Inebriety | BattleActorFlag_Confusion | BattleActorFlag_Forget;
// 原0xFFF03FFF只清六个能力升降位，不能扩大到RecoveryUp。
inline constexpr std::uint32_t BattleActorParameterChangeMask = BattleActorFlag_DefenseUp | BattleActorFlag_DefenseDown
    | BattleActorFlag_AttackUp | BattleActorFlag_AttackDown | BattleActorFlag_AgilityUp | BattleActorFlag_AgilityDown;

// 原1C起的16B数值记录；按动作解释字段，见BMSA06控制/附加记录合同，不含对象指针。
struct BattleEntryEffectRecord {
    std::int32_t position; // 原记录+00；命中目标、245特效类型或-1队列哨兵。
    std::uint32_t flags; // 原记录+04，保持完整W32标志。
    std::int32_t position3; // 原记录+08；数值/附加图号，245用作寿命输入。
    std::int32_t position4; // 原记录+0C；图号/附加值，三字段4393A0不写此字段。
};


// 4239D0/427470骑乘拆分原18h载荷；指针原生拓宽，末8B仅保留原分配范围，不解释业务。
struct BattleRideCarrierPayload {
    YobiAction* first00; // 原成员偏移：+0x00。 42958F/424179写首个438290结果，非拥有引用，226恢复角色槽。
    YobiAction* second04; // 原成员偏移：+0x04。 429923/42417B写第二个438290结果，非拥有引用，226恢复后清槽。
    std::uint32_t flags08; // 原成员偏移：+0x08。 42912D写M整数、414820恢复FDC；4239D0/RID不写此字段。
    std::int32_t gearImage0C; // 原成员偏移：+0x0C。 428FEC/42933E写51B740结果，恢复时严格>0才建主装备。
    std::array<std::byte, 8> bytes10; // 原成员偏移：+0x10。
};

static_assert(sizeof(BattleRideCarrierPayload) == 2 * sizeof(YobiAction*) + 16);

// E01：SKL 归一化前的技能标识与 E10 节点绑定，随战斗载荷复制／清理。
struct BattleSkillMeta {
    std::int32_t techId; // 未归一化的报文技术编号，YAML图号匹配键。
    std::int32_t skillId; // 未归一化的报文技能编号，与techId组成Lua注册键。
    std::int32_t runtimeGeneration; // E10运行代次，非地址。
    std::int32_t runtimeNodeId; // E10运行表节点编号，非拥有引用。
    std::int32_t flags; // 位0标识Lua技能绑定，沿参考保存。
    std::int32_t reserved; // 参考保留字段，初始化为零并随载荷复制。
};

// 战斗专用原生载荷；保留四段用途未定字节，后缀仅标原位置；完整字段/生产仍待证。
struct BattleActorPayload {
    std::int32_t x00; // 原成员偏移：+0x00。 原00坐标槽；40CAA0还会递增caster载荷同槽，不另建计数副本。
    std::int32_t y04; // 原成员偏移：+0x04。
    std::int32_t x08; // 原成员偏移：+0x08。
    std::int32_t y0C; // 原成员偏移：+0x0C。 43C9A0按(08-00,0C-04)计算朝向，不与对象坐标合并。
    std::int32_t x10; // 原成员偏移：+0x10。 416DD0恢复目标对象的F32坐标来源。
    std::int32_t y14; // 原成员偏移：+0x14。
    std::int32_t battlePosition18; // 4221F0/422A30/422B00复制到附件+1D4。
    std::array<BattleEntryEffectRecord, 200> effects1C; // 原成员偏移：+0x1C。 原16B效果记录，重置只写position。
    std::int32_t effectRunningC9C; // 原成员偏移：+0xC9C。
    std::int32_t effectCountCA0; // 原成员偏移：+0xCA0。
    std::array<std::int32_t, 200> castListCA4; // 原成员偏移：+0xCA4。 FC4前的200项动作队列。
    std::int32_t castPlayIndexFC4; // 原成员偏移：+0xFC4。
    std::int32_t castListIndexFC8; // 原成员偏移：+0xFC8。
    std::int32_t actionFrameCountFCC; // 原+FCC；动作回调帧计数，用于阶段等待和移动阈值。
    std::int32_t valueFD0; // 原+FD0；共用等待倒计时、一次性启动状态及碰撞边界编号，不能只叫计时器，保留原名。
    std::int32_t valueFD4; // 原+FD4；攻击等待帧与REB进入／退出阶段共用，单一含义不能覆盖，保留原名。
    std::int32_t busyFD8; // 原成员偏移：+0xFD8。
    std::uint32_t flagsFDC; // 原成员偏移：+0xFDC；BattleActorFlag位集合，保留未知位和原消息承接。
    std::uint32_t stateFE0; // 原成员偏移：+0xFE0。
    std::int32_t effectFlagsFE4; // 原成员偏移：+0xFE4。
    std::int32_t moveTargetXFE8; // 412870按目标坐标侧写入；与00/08/10坐标来源独立。
    std::int32_t moveTargetYFEC;
    std::int32_t valueFF0; // 原+FF0；共用周期／碰撞计数、延时和保存的phase，无法统一为计时器或阶段，保留原名。
    std::int32_t attackModeFF4; // 原成员偏移：+0xFF4。 426FC0写入原完整攻击模式。
    std::int32_t aliveFF8; // 原成员偏移：+0xFF8。
    std::array<std::byte, 4> bytesFFC; // 原成员偏移：+0xFFC。 原FFC..FFF：随整个载荷清零/浅复制，用途及地址属性未定。
    std::int32_t flashCounter1000; // 原成员偏移：+0x1000。 404DB0残影节拍；438290沿通用分配零化，不额外写入。
    std::int32_t flash1004; // 原成员偏移：+0x1004。 416DD0的bit100门禁，仅首次写1并播放音效。
    std::int32_t fpUsed1008; // 原成员偏移：+0x1008。
    std::int32_t targetPosition100C; // 原成员偏移：+0x100C。 4242B0三方序列写入原目标。
    std::int32_t reactionImage1010; // 原成员偏移：+0x1010。
    std::int32_t image1014; // 原成员偏移：+0x1014。 427470装备消息写51B740转换后的图号，40C480消费。
    std::int32_t soundId1018; // 原+1018；BM／技能链生产，动作80传给PlaySe的音效编号。
    std::int32_t soundId101C; // 原成员偏移：+0x101C。 40CCB0音效编号；427470技能98模式6已写入，424620已接，其他生产沿对应父合同。
    std::int32_t soundId1020; // 原成员偏移：+0x1020。 40CCE0音效编号；与1018/101C独立，不是地址。
    std::int32_t effectPlaybackState1024; // 原+1024；关联特效置1、完成置2，部分动作续用3／4；保持完整S32状态。
    std::int32_t image1028; // 原成员偏移：+0x1028。 427470技能链生产图号；41CD00的type34用作图号及音效/坐标重读源。
    std::int32_t image102C; // 原成员偏移：+0x102C。 426FC0按技术索引和目标阵营选表。
    std::int32_t value1030; // 原+1030；共用HP／FP数值、选图参数和动作模式，不能仅命名为伤害值，保留原名；非指针。
    std::int32_t numberDisplayType1034; // 原成员偏移：+0x1034。 427470状态/技能链写入，4107C0/410B00消费的显示及数值类型。
    std::int32_t value1038; // 原+1038；409680作强制命中门禁，40E560兼作动作阶段；统一业务含义未定，保留原名。
    std::int32_t castSkill103C; // 原+0x103C；分派技能号，已证常量见BattleSkillDispatchId，保留未知整数。
    std::int32_t castLevel1040; // 原成员偏移：+0x1040。 与1044的技术索引独立。
    std::int32_t castSkillLevel1044; // 原成员偏移：+0x1044。
    std::int32_t skill98Mode1048; // 原成员偏移：+0x1048。 42CFE5/42E90A写S32，141的技能98按图号偏移消费。
    std::array<std::byte, 4> bytes104C; // 原成员偏移：+0x104C。 原104C..104F：仅保留存储，不按邻接值猜图号或计数。
    std::int32_t attackDebuffType1050; // 原成员偏移：+0x1050。 424F64保存技能94的M整数；409680用于减益图号偏移。
    YobiAction* effect1054; // 原成员偏移：+0x1054。 41F660发布；416DD0/4072E0先标死再清槽，非拥有引用。
    YobiAction* attachedEffect1058; // 原成员偏移：+0x1058。 41FBE0创建并发布的非拥有附属特效引用。
    YobiAction* effect105C; // 原成员偏移：+0x105C。 41F860发布；4131B0/422370标死并清槽，非拥有引用。
    YobiAction* effect1060; // 原成员偏移：+0x1060。 非拥有克隆/骑乘/缩放特效；4067F0骑乘路径payload为BattleRideCarrierPayload。
    YobiAction* centeredEffect1064; // 原成员偏移：+0x1064。 41FA00发布的非拥有居中特效；41F950随父对象移动。
    YobiAction* effect1068; // 原成员偏移：+0x1068。 41FE00发布、41FCD0更新；4131B0标死并清槽，非拥有引用。
    YobiAction* rebirthEffect106C; // 原成员偏移：+0x106C。 4114B0/438CAF发布419CF0结果；497C80只检查空值，79标死后清槽。
    YobiAction* caster1070; // 原成员偏移：+0x1070。 4392A0记录动作发起者，非拥有引用。
    std::array<YobiAction*, 3> rebirthAuxiliaryEffects1074; // 原成员偏移：+0x1074。 原1074/1078/107C三槽，非拥有；79按原顺序清理。
    std::int8_t rebirthEffectType1080; // 原成员偏移：+0x1080。 4113A0按低U8加1发布/退出清零，4208F0按S8选图；其他生产仍缺。
    std::array<std::byte, 3> bytes1081; // 原成员偏移：+0x1081。 原1081..1083：用途未定字节，不以原生隐式对齐代替承载。
    std::array<std::int32_t, 200> queuedTargets1084; // 原成员偏移：+0x1084。 原[1084,13A4)，43B7F0追加值及哨兵。
    std::int32_t queuedTargetIndex13A4; // 原+13A4；queuedTargets1084读取游标，由43B830递增。
    std::int32_t queuedTargetCount13A8; // 原成员偏移：+0x13A8。
    std::int32_t baseImage13AC; // 原成员偏移：+0x13AC。 413550保存原图号，413750/413980恢复，独立于13E4。
    std::int32_t appearanceOverrideActive13B0; // 原成员偏移：+0x13B0。 413550写0/1，413750/413980恢复后清零，4131B0消费。
    YobiAction* gear13B4; // 原成员偏移：+0x13B4。 422C20主附件，非拥有引用。
    std::array<YobiAction*, 2> auxiliaryGear13B8; // 原成员偏移：+0x13B8。 原13B8/13BC，422C90的0/1槽。
    std::array<std::int32_t, 2> auxiliaryGearTypes13C0; // 原成员偏移：+0x13C0。 原13C0/13C4，清附件不清类型。
    YobiAction* rider13C8; // 原成员偏移：+0x13C8。 4222B0保存4221F0创建的非拥有引用，移除时MarkDeath再清槽。
    std::int32_t debuffState13CC; // 原成员偏移：+0x13CC。 41F660分配前写2，失败不回滚。
    std::int32_t debuffType13D0; // 原成员偏移：+0x13D0。 41F660六图号对应类型1至6。
    std::array<std::byte, 4> bytes13D4; // 原成员偏移：+0x13D4。 原13D4..13D7：仅保留存储，不解释为减益状态或指针。
    std::int32_t statusEffectMode13D8; // 原+13D8；CG_BATTLE_STATUS_MODE_*解释域，427470生产、41FE00消费完整W32。
    std::int32_t statusEffectType13DC; // 原+13DC；按模式使用CG_BATTLE_DEBUFF_*／PARAMETER_EFFECT_*／EXT_EFFECT_*，保留清理哨兵及未知值。
    std::int32_t statusEffectImageIndex13E0; // 原+13E0；扩展技能2019按S32查54D4C0，变体数见CG_BATTLE_EXT_SKILL_2019_IMAGE_COUNT。
    std::int32_t image13E4; // 原成员偏移：+0x13E4。 427470技能链生产图号；41CD00的type43从重取的源载荷读取。
    BattleSkillMeta skillMeta; // E01：本机具名扩展，分配及完整克隆统一按 sizeof(BattleActorPayload)。
};

static_assert(sizeof(BattleEntryEffectRecord) == 16);

static_assert(std::is_trivially_copyable_v<BattleActorPayload>);
static_assert(std::is_trivially_copy_assignable_v<BattleActorPayload>);

static_assert(offsetof(BattleEntryEffectRecord, position4) == 12);

// 原场景180B载荷；标量保留宽度，B0场地特效引用按原生T*拓宽。
struct BattleScenePayload {
    std::int32_t completedEntryCount00; // 原+00，41A9E0/41AF00完成指定分类且场景phase8时递增。
    std::int32_t entryCount04; // 原成员偏移：+0x04。 427470/CMB按S32读写的参与者数量。
    std::array<std::int32_t, 20> entryPositions08; // 原成员偏移：+0x08。 原08..57的位置列表，非对象地址。
    std::int32_t packetSubState58; // 原成员偏移：+0x58。
    std::int32_t remainingMessages5C; // 原成员偏移：+0x5C。
    std::int32_t eventSlot60; // 原成员偏移：+0x60。
    std::int32_t packetOffset64; // 原成员偏移：+0x64。
    std::int32_t groupBase68; // 原成员偏移：+0x68。
    std::int32_t casterPosition6C; // 原成员偏移：+0x6C。 427750恢复、437A29回存的同源执行快照，非新端字节偏移。
    std::int32_t value70; // 原+70；gBattleMValue5ABA7C快照，既存角色位置又作累计技能重复标记，不能统一叫施法者位置，保留原名。
    std::int32_t defenderPosition74; // 原成员偏移：+0x74。
    std::int32_t targetPosition78; // 原成员偏移：+0x78。
    std::int32_t skill7C; // 原+0x7C；当前场景技能号；原生分派见BattleSkillDispatchId，Lua路径仍承接原始ID。
    std::int32_t technique80; // 原成员偏移：+0x80。
    std::int32_t itemMode84; // 原+84；ITM消息模式，执行状态保存／恢复使用。
    std::int32_t skill98Mode88; // 原+88；技能98模式，执行状态保存／恢复使用。
    std::int32_t feedbackSourcePosition8C; // 原+8C；反射／累计回馈的来源角色位置快照。
    std::uint32_t resultFlags90; // 原成员偏移：+0x90。
    std::uint32_t flags94; // 原成员偏移：+0x94。
    std::int32_t feedbackEffectImage98; // 原+98；反射／累计回馈的首个附加图号快照。
    std::uint32_t flags9C; // 原成员偏移：+0x9C。 恢复为函数局部量，公共尾不回存。
    std::int32_t feedbackAlternateImageA0; // 原成员偏移：+0xA0。 同上，不与messageValueA4合并。
    std::int32_t messageValueA4; // 原成员偏移：+0xA4。
    std::uint32_t entryMaskA8; // 原成员偏移：+0xA8。 4394A0参与对象位图，含特殊槽20/21。
    std::int32_t waitCountAC; // 原成员偏移：+0xAC。 忙状态每次检查递增，超时逐对象恢复后清零。
    YobiAction* fieldEffectB0; // 原成员偏移：+0xB0。 41FFE0发布的非拥有场地特效；438550无场地标志时标死并重取载荷清槽。
};

static_assert(offsetof(BattleScenePayload, entryMaskA8) == 0xA8);

static_assert(offsetof(BattleScenePayload, fieldEffectB0) == 0xB0);

// 原函数：0x004237E0。
std::int32_t GetBattlePosition_4237E0(std::int32_t position); // 50/51转20/21，其余原值。
// 原函数：0x0043B7F0。
void AppendBattleQueuedTarget_43B7F0(YobiAction* action, std::int32_t value);
// 原函数：0x0043B830。
std::int32_t AdvanceBattleQueuedTarget_43B830(YobiAction* action); // 先推进13A4，再判断新槽是否为-1。
// 原函数：0x0043BF00。
std::int32_t GetBattleValueBand_43BF00(std::int32_t value); // B22H01：原1..15按五个S32阈值计档1..5，范围外-1。
// 原函数：0x00404D90。
std::int32_t SelectIntValue_404D90(const std::int32_t* values, std::int32_t count, std::int32_t index);
// 原函数：0x004393A0。
void AppendBattleEffectList_4393A0(
    YobiAction* action, std::int32_t position, std::uint32_t flags, std::int32_t position3);
// 原函数：0x00439400。
void AppendBattleEffect_439400(YobiAction* action, BattleEntryEffectRecord effect);
// 原函数：0x0043BC10。
void AppendBattleAuxEffects_43BC10(
    YobiAction* action, std::int32_t position, std::uint32_t flags,
    std::int32_t value3, std::int32_t value4, std::int32_t value5);
// 原439470：先推进C9C，返回新项position是否为-1；空对象返回0。
std::int32_t AdvanceBattleEffect_439470(YobiAction* action);
// 原43BC90：三次推进、四输出原序写入并分别重读游标，返回末次推进结果。
std::int32_t TakeBattleAuxEffects_43BC90(
    YobiAction* action, std::int32_t* position, std::int32_t* value3,
    std::int32_t* value4, std::int32_t* value5);
// 原406390：固定当前flags/position，五块独立执行；附加块消费四输出，逐调用重取对象。
void ApplyBattleEffectFlags_406390(std::int32_t sourcePosition, YobiAction* action);
// 原函数：0x00416DB0。
void ApplyBattleCastEffects_416DB0(YobiAction* action); // 动作235：先应用效果，再推进动作队列。

// 原函数：0x0043BD00。
std::uint8_t IsBattleAppendEffectSkill_43BD00(std::int32_t skill); // 原54项技能准入集合，返回0/1；不查询链，见追加效果链SPEC。
// 原函数：0x0043BE00。
void AddBattleAppendEffect_43BE00(
    std::int32_t skill, std::int32_t technique, std::int32_t position); // 匹配首节点替换，否则尾插。
// 原函数：0x0043BEA0。
std::int32_t FindBattleAppendEffectTechnique_43BEA0(
    std::int32_t skill, std::int32_t position); // 按位置/技能查首节点，未命中-1。
// 原函数：0x00405CB0。
void UpdateBattleDirectionFromAngle_405CB0(YobiAction* action); // 角度只校正一圈，按八个不等宽区间写方向。
// 原函数：0x00405E60。
void NormalizeBattleAngle_405E60(YobiAction* action); // BSTL01：上限首样/第二角度持样/负值重取，一次加减360。
// 原函数：0x00405EA0。
void MoveBattleByAngle_405EA0(YobiAction* action); // X存回F32后才计算Y，角度/速度按原序读取。
// 原函数：0x00405ED0。
std::int8_t GetBattleRelativeDirection_405ED0(
    float sourceX, float sourceY, float targetX, float targetY,
    float tolerance); // 第二组坐标作基准，原负基准两次相减及三闭区间保持独立。
// 原函数：0x00406060。
double GetBattleAngleDifference_406060(YobiAction* source, YobiAction* target); // 空引用返回0，未舍入角差只校正一圈。
// 原函数：0x004060D0。
void TurnBattleTowardPosition_4060D0(
    YobiAction* action, float x, float y, std::int32_t updateDirection,
    float step); // 无空指针门禁；角差转S32取绝对值后限步，严格1才更新方向。
// 原函数：0x00406190。
void TurnBattleToward_406190(
    YobiAction* action, YobiAction* target, std::int32_t updateDirection, float step);
// 原函数：0x004062A0。
std::int32_t GetBattleDistanceToPosition_4062A0(YobiAction* action, float x, float y); // BRCP01：先X后Y求差，平方和存F32/正常S32返回。
// 原函数：0x00406300。
std::int32_t GetBattleDistance_406300(YobiAction* first, YobiAction* second); // 平方和先舍入F32再开方。
// 原函数：0x00406360。
std::int32_t CountBattleActiveEntries_406360(); // BHRC01：22槽含20/21，FF8完整S32严格==1，无新payload保护。
// 原函数：0x004065A0。
std::int32_t SeedBattleRandom_4065A0(std::int32_t value); // 前20槽非空数乘活动M字节长度与输入，重播同源随机种子。
// 原函数：0x00406260。
void FaceBattleActorTowardFormationPosition_406260(YobiAction* action); // 载荷反向坐标差转角度，再更新方向。
// 原函数：0x004390E0。
void InitBattleCastList_4390E0(std::int32_t index); // 22槽索引，保留禁用/phase13门禁。
// 原函数：0x004391E0。
void ResetBattleCastList_4391E0(std::int32_t index); // 恢复重置，不清忙状态、技能或转向。
// 原函数：0x004392A0。
void AppendBattleCast_4392A0(YobiAction* caster, YobiAction* actor, std::int32_t state);
// 原函数：0x004392F0。
void AppendBattleCastAt_4392F0(
    YobiAction* caster, YobiAction* actor, std::int32_t state, std::int32_t index);
// 原函数：0x00439340。
void AdvanceBattleCastState_439340(YobiAction* action); // 先推进FC4，到200或-1哨兵写phase236。
// 原函数：0x0040CAC0。
void AdvanceBattleLinkedCast_40CAC0(YobiAction* action); // 动作73：当前效果目标先推进，再推进自身。
// 原函数：0x0040CAF0。
void AdvanceBattleEffectAndCast_40CAF0(YobiAction* action); // 动作74：推进效果后总是推进动作。
// 原函数：0x00439380。
void MoveBattleCastState_439380(YobiAction* action); // 播放索引对应项写phase，不推进索引。
// 原函数：0x004394A0。
std::int32_t CheckBattleTurnReady_4394A0(YobiAction* parent, std::int32_t enable); // 22槽就绪及超时恢复。

} // namespace detail

} // namespace cg::yobi
