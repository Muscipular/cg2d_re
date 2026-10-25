#pragma once

#include "base.hpp"

// 本组内部共享声明；跨模块接口见 cg/Yobi/YobiRuntime.hpp。

namespace cg::yobi::detail {

// 原4221F0/骑乘过渡生产、421FB0消费；只解释骑乘子对象的phase，非父角色动作号。
enum BattleRiderSyncMode : std::int32_t {
    BattleRiderSyncMode_FollowParent = 0, // 同步父动作、方向、位置及绘制属性。
    BattleRiderSyncMode_DrawAttributesOnly = 1, // 过渡独立控制动作／坐标，仍同步父绘制属性及缩放。
};

// 原422C90配置、422780消费；type3..5仅首次创建设置缩放，重用对象不补写。
enum BattleAuxGearType : std::int32_t {
    BattleAuxGearType_StandOrDown = 1, // 原default动作映射，站立或跟随父倒地；创建排序次键-1。
    BattleAuxGearType_StandLoop = 2, // 站立循环，创建排序次键3。
    BattleAuxGearType_DirectionalQuarterScale = 3, // 首次缩放1/4，按父动作及八方向偏移随行。
    BattleAuxGearType_DirectionalHalfScale = 4, // 首次缩放1/2，同源动作与方向映射。
    BattleAuxGearType_DirectionalThreeQuarterScale = 5, // 首次缩放3/4，同源动作与方向映射。
    BattleAuxGearType_ClearGraphicOnDown = 6, // 父倒地时先清graphicIndex再播帧；配置已有对象不改图号。
};

// 原函数：0x00421F90。
void UpdateBattleAttachment_421F90(YobiAction* action); // 父战斗槽身份变化时只标死附件。
// 原函数：0x00421FB0。
void SyncBattleRider_421FB0(YobiAction* action); // 非零phase仅同步绘制属性；零phase另映射动作并投影。
// 原函数：0x004225D0。
void SyncBattleGear_4225D0(YobiAction* action); // AF406：主挂件动作、独立Coordinate及隐藏条件。
void ResolveBattleGearLayer406(YobiAction* action); // AF406：最终排序之后解析主挂件层，辅助宠物不参与。
const YobiAction* GetBattleTitleAnchor(const YobiAction* action); // AT01：只识别主称号；骑乘取人物rider。
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
