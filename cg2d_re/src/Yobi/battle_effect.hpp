#pragma once

#include "base.hpp"

#include "battle_base.hpp"

#include <array>
#include <cstdint>

namespace cg::yobi {

namespace detail {

// 41CD00第三槽：创建类型0..47使用S32 Y槽；其他类型消费已有对象T*，由caller显式选择成员。
union BattleEffectThirdArgument {
    std::int32_t offsetY;
    YobiAction* existingAction;
};


// 共享原状态，只在本组实现中定义；原全局：0x0059E9D8。
extern std::array<std::int32_t, 1025> gBattleEffectPositions59E9D8;

// 原函数：0x0041E5C0。
void UpdateBattleAfterimage_41E5C0(YobiAction* action); // 递增FF0，新计数达到12才标死。
// 原函数：0x0041E5F0。
YobiAction* NewBattleAfterimage_41E5F0(YobiAction* source); // 原克隆路径无失败门禁，复用完整浅复制。
// 原函数：0x00419FA0。
void UpdateBattleOneShotEffect_419FA0(YobiAction* action); // PlayFrame(1)严格返回1才标死。
// 原函数：0x00418D80。
void UpdateBattleProjection_418D80(YobiAction* action); // 父FD0门禁仅影响移动，标死后仍播放/投影。
// 原函数：0x00418DF0。
void NewBattleProjection_418DF0(
    YobiAction* parent, float x, float y, float targetX, float targetY,
    std::int32_t imageNo); // 目标先截S32再转向；原三caller不消费返回，指定图号播放后额外移动两次（BPR01）。
// 原函数：0x00418F10。
void UpdateBattleArcProjection_418F10(YobiAction* action); // 固定父引用，标死后仍残影/屏幕投影，保留两次角度读取。
// 原函数：0x00418FF0。
YobiAction* NewBattleArcProjection_418FF0(
    YobiAction* parent, std::int32_t offsetX, std::int32_t offsetY,
    YobiAction* distanceSource, YobiAction* target, std::int32_t imageNo); // 原rand角度，距离分两次有符号除法。
// 原函数：0x0041E4D0。
void UpdateBattleScatterEffect_41E4D0(YobiAction* action); // BSEX01：移动/终帧标死仍投影，Y取样先于X发布。
// 原函数：0x0041E510。
YobiAction* NewBattleScatterEffect_41E510(YobiAction* parent, std::int32_t imageNo); // BSEX01：成功才三RNG；X发布后Y取样/W16零归，再发布Y。
// 原函数：0x0041E640。
void UpdateBattlePositionEffect_41E640(YobiAction* action); // PlayFrame(1)严格完成才标死，所有路径投影X/Y。
// 原函数：0x0041E680。
YobiAction* NewBattlePositionEffect_41E680(YobiAction* parent, std::int32_t imageNo);
// 原函数：0x0041E720。
YobiAction* NewBattleCoordinateEffect_41E720(float x, float y, std::int32_t imageNo); // 无父引用，不提前播放。
// 原函数：0x0041E7C0。
YobiAction* NewBattleTrailEffect_41E7C0(
    float x, float y, std::int32_t imageNo, std::int32_t mode,
    std::uint8_t priority); // 失败不消耗随机；成功绑定41E640，设备分支决定混合标志。
// 原函数：0x0041A160。
void UpdateBattleCompletingEffect_41A160(YobiAction* action); // 播放完成后通知父1024，标死仍投影。
// 原函数：0x0041A1C0。
void UpdateBattleParentEffect_41A1C0(YobiAction* action); // 父严格死亡时标死自身，仍播放并投影。
// 原函数：0x0041A200。
void UpdateBattleRisingEffect_41A200(YobiAction* action); // 固定父引用，未舍入Y比较；标死后仍播放/投影。
// 原函数：0x0041A270。
void UpdateBattleScalingEffect_41A270(YobiAction* action); // phase0放大、phase2缩小，以未舍入X判定阶段。
// 原函数：0x0041A310。
void UpdateBattleRandomEmitter_41A310(YobiAction* action); // 50次随机生成；父死亡早退，完成数通知父1024。
// 原函数：0x0041A530。
void UpdateBattleFallingEffect_41A530(YobiAction* action); // 未舍入Y超过nameY才递增父58，仍播放/投影。
// 原函数：0x0041A590。
void UpdateBattleReverseEmitter_41A590(YobiAction* action); // 132B数值载荷，每槽四粒子，倒序坐标。
// 原函数：0x0041A7D0。
void UpdateBattleIndexedEmitter_41A7D0(YobiAction* action); // 32次顺序生成，失败也递增nameX。
// 原函数：0x00420D50。
YobiAction* NewBattleRisingEffect_420D50(YobiAction* parent); // 先随机位置再分配，成功后可再随机混合标志。
// 原函数：0x0041CD00。
YobiAction* NewBattleEffect_41CD00(
    YobiAction* source, std::int32_t offsetX, BattleEffectThirdArgument third,
    std::uint32_t type, std::int32_t blend, std::int32_t link); // 完整48类及已有对象公共尾。
// 原函数：0x0041E890。
std::int32_t DrawBattleTiledEffect_41E890(std::int32_t type); // BSTL01：共享相位先推进，X外Y内100格/每格fresh相位/末次返回。
// 原函数：0x0041E920。
void UpdateBattleTiledEffect_41E920(YobiAction* action); // 独立58相位/1D4反向，绘制后判寿命且仍自增。
// 原函数：0x0041E9D0。
YobiAction* NewBattleTiledEffect_41E9D0(std::int32_t type, std::int32_t lifetime); // maxHp/readiness依次临时构造赋值。
// 原函数：0x00419FC0。
YobiAction* NewBattleRandomEffect_419FC0(
    YobiAction* parent, std::int32_t imageNo, std::int32_t direction); // X/Y分别取随机并重读尺寸。
// 原函数：0x0041A0D0。
YobiAction* NewBattleScreenEffect_41A0D0(
    std::int32_t x, std::int32_t y, std::int32_t imageNo, std::int32_t direction); // 屏幕坐标直接创建，绑定完整419FA0，不提前播放。
// 原函数：0x004208F0。
void NewBattleRebirthEffects_4208F0(YobiAction* source, YobiAction* target); // BRAE01：1080低S8，首段后重取目标类型。
// 原函数：0x004209C0。
std::uint8_t NewBattleAttributeEffects_4209C0(
    YobiAction* source, YobiAction* target); // BRAE01：四W16非零匹配；U8掩码表示匹配并调用。
// 原函数：0x0041F660。
YobiAction* NewBattleDebuffEffect_41F660(YobiAction* parent, std::int32_t imageNo); // BAS01：六图号先改FDC/13D0/13CC，分配失败不回滚；播放后fresh父载荷写1054再绑定父。
// 原函数：0x0041F800。
void UpdateBattleDebuffEffect_41F800(YobiAction* action); // BAS01：1054/105C共用借父回调，Play后重借父跟随；标死仍投影，Y转换后取屏幕偏移。
// 原函数：0x0041F860。
YobiAction* NewBattleReverseEffect_41F860(YobiAction* parent, std::int32_t imageNo); // BAS01：105C已有返空，设备后持flags两次发布；播放后fresh父载荷写105C再绑定父。
// 原函数：0x0041F950。
void UpdateBattleCenteredEffect_41F950(YobiAction* action); // BDE01：父严格标死后仍共同投影尾。
// 原函数：0x00419CB0。
void UpdateBattleOffsetFollowEffect_419CB0(YobiAction* action); // 父标死仍按屏幕坐标/S32偏移跟随并播放。
// 原函数：0x00419BE0。
void UpdateBattleTransitionEffect_419BE0(YobiAction* action); // phase0播放严格完成才置phase1/隐藏，不自动标死。
// 原函数：0x00419C10。
YobiAction* NewBattleTransitionEffect_419C10(YobiAction* parent, std::int32_t imageNo);
// 原函数：0x00419CF0。
YobiAction* NewBattleOffsetFollowEffect_419CF0(
    YobiAction* parent, std::int32_t offsetX, std::int32_t offsetY, std::int32_t imageNo); // 播放后按浮点坐标加偏移投影。
// 原函数：0x0041FA00。
YobiAction* NewBattleCenteredEffect_41FA00(YobiAction* parent, std::int32_t imageNo); // BDE01：1064已有即空返，S16半宽高/播放后发布/投影。
// 原函数：0x0041FB20。
void UpdateBattleAttachedEffect_41FB20(YobiAction* action);
// 原函数：0x0041FBE0。
YobiAction* NewBattleAttachedEffect_41FBE0(YobiAction* parent, std::int32_t mode); // 已有槽只换图并返回空。
// 原函数：0x0041FCD0。
void UpdateBattleStatusEffect_41FCD0(YobiAction* action);
// 原函数：0x0041FE00。
YobiAction* NewBattleStatusEffect_41FE00(YobiAction* parent, std::int32_t imageNo, std::int32_t mode); // 1068发布后执行缩放/下降。
// 原函数：0x0041FF90。
void UpdateBattleFieldEffect_41FF90(YobiAction* action); // BFL01：当前场景death严格1标死自身，仍投影；Y转换后再取屏幕Y偏移。
// 原函数：0x0041FFE0。
YobiAction* NewBattleFieldEffect_41FFE0(std::int32_t imageNo); // BFL01：B0已有只换图返空；两组索引先齐读坐标，播放后fresh场景发布，原投影尾。
// 原函数：0x00421480。
void UpdateBattleBackgroundFade_421480(YobiAction* action); // 完整五态淡变，同源背景发布与延后标死。
// 原函数：0x00421570。
YobiAction* NewBattleBackgroundFade_421570(
    std::int32_t imageNo, std::uint8_t step, std::int32_t publishBackground);
// 原函数：0x00422370。
void RestoreBattleStateEffects_422370(YobiAction* parent); // 固定入口FDC快照，保留成对共用清位掩码。
// 原函数：0x00421610。
YobiAction* NewBattleScreenEffect_421610(std::int32_t x, std::int32_t y);
// 原函数：0x004216B0。
void UpdateBattleScreenEmitter_4216B0(YobiAction* action); // 原rand先Y后X，生成后重取阶段并递减。
// 原函数：0x00421700。
YobiAction* NewBattleScreenEmitter_421700(std::int32_t phase);
// 原函数：0x00418B70。
YobiAction* NewBattleSpiracleShot_418B70(YobiAction* parent, const BattleEntryEffectRecord* effect, std::int32_t imageNo);
// 原函数：0x00419180。
YobiAction* NewBattleBlastWave_419180(YobiAction* parent, const BattleEntryEffectRecord* effect, std::int32_t imageNo);
// 原函数：0x00419390。
YobiAction* NewBattleRangeHit_419390(YobiAction* parent, const BattleEntryEffectRecord* effect, std::int32_t imageNo);
// 原函数：0x004195F0。
YobiAction* NewBattleBoomerang_4195F0(YobiAction* parent); // B22C01：原取样/几何发布，同步总回调推进后返回原生T*。
// 原函数：0x00419DA0。
YobiAction* NewBattleArcShot_419DA0(YobiAction* parent, const BattleEntryEffectRecord* effect,
    std::int32_t imageNo, std::int32_t count, std::int32_t mode);
// 原函数：0x004205F0。
YobiAction* NewBattleCastCopy_4205F0(YobiAction* parent); // BCPY01：独立战斗载荷与父借用，三载荷重取/末坐标发布；非浅克隆。

} // namespace detail

} // namespace cg::yobi
