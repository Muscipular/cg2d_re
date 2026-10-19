#pragma once

#include "base.hpp"

// 本组内部共享声明；跨模块接口见 cg/Yobi/YobiRuntime.hpp。

namespace cg::yobi::detail {
// 原函数：0x00421F90。
void UpdateBattleAttachment_421F90(YobiAction* action); // 父战斗槽身份变化时只标死附件。
// 原函数：0x00421FB0。
void SyncBattleRider_421FB0(YobiAction* action); // 非零phase仅同步绘制属性；零phase另映射动作并投影。
// 原函数：0x004225D0。
void SyncBattleGear_4225D0(YobiAction* action); // AF406：主挂件动作、独立Coordinate及隐藏条件。
void ResolveBattleGearLayer406(YobiAction* action); // AF406：最终排序之后解析主挂件层，辅助宠物不参与。
// 原函数：0x00422780。
void SyncBattleAuxGear_422780(YobiAction* action, std::int32_t index); // type/动作/方向映射；身份检查前复制方向。
// 原函数：0x004221F0。
YobiAction* NewBattleRider_4221F0(YobiAction* parent, std::int32_t imageNo);
// 原函数：0x00422A30。
YobiAction* NewBattleGear_422A30(YobiAction* parent, std::int32_t imageNo);
// 原函数：0x00422B00。
YobiAction* NewBattleAuxGear_422B00(
    YobiAction* parent, std::int32_t imageNo, std::int16_t sortIndex);
// 原函数：0x004222B0。
std::int32_t SetBattleRider_4222B0(
    YobiAction* action, std::int32_t imageNo, std::int32_t ridingImageNo);
// 原函数：0x00422BC0。
void SetBattleGear_422BC0(YobiAction* action, std::int32_t imageNo); // 原无模式门槛，等价422C20的index0。
// 原函数：0x00422C20。
void SetBattleGear_422C20(YobiAction* action, std::int32_t imageNo, std::int32_t index);
// 原两caller的index均0/1；type6不更新已有图号，type3..5仅首次创建设置缩放。
// 原函数：0x00422C90。
void SetBattleAuxGear_422C90(
    YobiAction* action, std::int32_t imageNo, std::int32_t type, std::int32_t index);
// 原函数：0x00420F10。
void SyncBattleAttachmentAlpha_420F10(
    YobiAction* action, std::int32_t alpha, std::int8_t delta,
    std::uint32_t addMask, std::uint32_t removeMask); // 按原九槽顺序重取，不处理1060/1070。

} // namespace cg::yobi::detail
