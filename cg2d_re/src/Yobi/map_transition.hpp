#pragma once

#include "base.hpp"

namespace cg::yobi {

// YMN03：448040参数及MapTransitionPayload.mode的解释域，原S16字段／接口保持。
// 与50DD80协议mode0～3、普通地图角色phase和战斗特效type分别独立。
enum MapTransitionMode : std::int16_t {
    MapTransitionMode_Wandering = 0, // 逐格游走并定时生产两路18902特效，446D80。
    MapTransitionMode_PairedEffects = 1, // 依次生产两路18902特效后结束，446FA0。
    MapTransitionMode_ExpandingOrbit = 2, // 三点圆周半径从0向外增加，447070。
    MapTransitionMode_ContractingOrbit = 3, // 三点圆周半径从800向内减少，447230。
    MapTransitionMode_AcceleratingBlink = 4, // 原地闪烁间隔缩短，不表示运动加速，4473B0。
    MapTransitionMode_DeceleratingBlink = 5, // 原地闪烁间隔增长，不表示运动减速，4474E0。
    MapTransitionMode_StraightBlink = 6, // 直线步进并闪烁，447610。
    MapTransitionMode_ApproachingBlink = 7, // 向玩家靠近并闪烁，447820。
    MapTransitionMode_TrailUsingYOrigin = 8, // 抛落／残影，绘制Y偏移取资源轴1，447A20。
    MapTransitionMode_TrailUsingXOrigin = 9, // 同类残影，但绘制Y偏移沿原端取轴0，447C40；业务名未证。
};

namespace detail {

// 地图实体与过渡模式共用的移动结束判定，保持原坐标读取顺序。
// 各调用点独立采样，按原FCOMP的C3判定且保持X到Y短路读取。
bool TransitionMovementIdle(const YobiAction* const action);

} // namespace detail

} // namespace cg::yobi
