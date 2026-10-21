#pragma once

#include "base.hpp"

// 本组内部共享声明；跨模块接口见 cg/Yobi/YobiRuntime.hpp。

namespace cg::yobi::detail {

// 三种地图角色生产者共用的 x64 payload 分配，不改变通用 404AE0 的尺寸合同。
YobiAction* NewMapCharaAction();
// 原43F330/43FEB0：观察链两种生产者，43F520/440070绑定及正文已有，完整寿命/运行未验。
YobiAction* CreateDepartingObservationGroup_43F330(
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY,
    std::int32_t direction, std::int32_t travelDuration);
// 原函数：0x0043FEB0。
YobiAction* CreateArrivingObservationGroup_43FEB0(
    std::int32_t imageNo, std::int32_t mapX, std::int32_t mapY,
    std::int32_t direction);
// 原4408C0：仅正编号写观察载荷五槽，入口无action空保护，不改子指针。
void SetObservationGroupMembers_4408C0(
    YobiAction* action, std::int32_t first, std::int32_t second,
    std::int32_t third, std::int32_t fourth, std::int32_t fifth);

// 原 5F9750 独立观察链；引用非拥有，清头不释放 action。
// 原函数：0x0043F180。
YobiAction* AppendObservationGroup_43F180(YobiAction* action);
// 原函数：0x0043F1B0。
void* UnlinkObservationGroup_43F1B0(YobiAction* action);

// 原 440B70：从 440F50 生产的五槽状态创建地图观察节点。
YobiAction* CreateObservationTravelFromSnapshot_440B70();
// 原 43FD90：按主图号、子槽号和方向累加原表偏移，返回 y。
std::int32_t* AddObservationMemberSlotOffset_43FD90(
    std::int32_t imageNo, std::int32_t slot, std::int32_t direction,
    std::int32_t* x, std::int32_t* y);

} // namespace cg::yobi::detail
