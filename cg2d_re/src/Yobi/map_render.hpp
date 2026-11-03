#pragma once

#include "base.hpp"

#include <cstdint>

namespace cg::yobi {

namespace detail {

// 原函数：0x00442F70。
void DrawMapActorOverlaysAndAttachments_442F70(YobiAction* action, std::int32_t primaryIndex);

// 原444260：无payload附件，仅借用可选的父引用槽，不改变通用释放逻辑。
YobiAction* CreateOneShotMapAnimation_444260(
    YobiAction* source, const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t characterAction,
    const std::int32_t direction, const std::uint8_t displayPriority,
    YobiAction** const completionSlot);

// 原函数：0x004417A0。
std::int32_t GetActorOverheadOffset_4417A0(std::int32_t imageNo); // 原40项角色高度表及-88回退。
// 原444000：无回调挂件创建，四次CryptInt赋值和世界/屏幕投影。
YobiAction* CreateMapAttachmentAction_444000(
    YobiAction* parent, std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY,
    std::int32_t characterAction, std::int32_t direction, std::uint8_t displayPriority);
// 原两轮kind调整；primaryIndex未使用，输出按原指针写序，不返回聚合值。
// 原函数：0x004412D0。
void ResolveGearPetAnimationState_4412D0(
    YobiAction* action, std::int32_t primaryIndex,
    std::int32_t* characterAction, std::int32_t* revert);
// 原函数：0x00441430。
void ResolveGearPetDrawPlacement_441430(
    YobiAction* action, std::int32_t primaryIndex,
    std::int32_t* x, std::int32_t* y, std::uint8_t* before);
// 原函数：0x00441570。
void DrawGear_441570(
    YobiAction* action, std::int32_t primaryIndex,
    std::int32_t offsetX, std::int32_t offsetY, std::uint8_t before);
// 原43DB60：独立读两条CryptInt数值，绘条后总是绘外框，返回最后入队结果。
std::int32_t ShowHpBar_43DB60(
    YobiAction* action, std::int16_t offsetX, std::int16_t offsetY, std::int32_t color);

} // namespace detail

} // namespace cg::yobi
