#pragma once

#include "base.hpp"

#include <array>
#include <cstdint>

namespace cg::yobi {

namespace detail {

inline constexpr std::array<std::int32_t, 5> kBattleHitImages54D450 {111256, 111257, 111258, 111259, 111260}; // 原常量参考：0x0054D450。

// 原函数：0x0041A9E0。
void UpdateBattleImpactEffect_41A9E0(YobiAction* action); // 完成后按当前效果处理HP/动作，固定目标与独立重读并存。
// 原函数：0x0041AF00。
void UpdateBattleImpactSoundEffect_41AF00(YobiAction* action); // 默认分类末尾另播132，其余合同同41A9E0。
// 原函数：0x0041B440。
void UpdateBattleFrameEventEffect_41B440(YobiAction* action); // 先消费帧事件，再按旧58/FD0推进动画与nameY消退。
// 原函数：0x0041BB00。
void UpdateBattleMovingImpactEffect_41BB00(YobiAction* action); // 实时目标中心命中，保留提前标死后的运动/投影尾。
// 原函数：0x0041C3B0。
void UpdateBattleHomingImpactEffect_41C3B0(YobiAction* action); // 距离门禁/HP分类，命中后出界仍继续移动与尾迹。
// 原函数：0x004219D0。
void UpdateBattleCarryingEffect_4219D0(YobiAction* action); // 当帧carry独立快照，出界推进/标死后仍带动当前目标。
// 工厂初始化：仅借用现有对象与真实回调，空action/source原样返回，不分配/播放。
// 原函数：0x00422E50。
void InitBattleTargetEffect_422E50(
    YobiAction* action, YobiAction* source, void (*callback)(YobiAction*)); // 固定目标坐标与图号音效。
// 原函数：0x00423040。
void InitBattleOffsetEffect_423040(
    YobiAction* action, YobiAction* source, void (*callback)(YobiAction*)); // 使用DX的S16半高，复制103C/1044。
// 原函数：0x004231D0。
void InitBattleCenterEffect_4231D0(
    YobiAction* action, YobiAction* source, void (*callback)(YobiAction*)); // 中心坐标、随机速度/角度，转向时重取目标。
// 原函数：0x00423350。
void InitBattleLowerEffect_423350(
    YobiAction* action, YobiAction* source, void (*callback)(YobiAction*)); // 使用对象2CC的S32半偏移，不复制技能。
// 原函数：0x004234B0。
void InitBattleScreenYEffect_4234B0(
    YobiAction* action, YobiAction* source, void (*callback)(YobiAction*),
    std::int32_t* side); // 可空数值输出；写输出后才读取目标屏幕Y。
// 原函数：0x0041F570。
void UpdateBattleHitEffect_41F570(YobiAction* action); // BHE01：原计数/上限持样，标死后Y取样先于X发布。
// BHE01：原五参数/零载荷请求和字段发布，播放后Y取样先于X发布，返回原生T*。
// 原函数：0x0041F5B0。
YobiAction* NewBattleHitEffect_41F5B0(
    YobiAction* parent, std::int32_t offsetX, std::int32_t offsetY,
    std::int32_t lifetime, std::int32_t imageNo);

} // namespace detail

} // namespace cg::yobi
