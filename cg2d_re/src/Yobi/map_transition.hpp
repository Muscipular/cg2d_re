#pragma once

#include "base.hpp"

namespace cg::yobi {

namespace detail {

// 地图实体与过渡模式共用的移动结束判定，保持原坐标读取顺序。
// 各调用点独立采样，按原FCOMP的C3判定且保持X到Y短路读取。
bool TransitionMovementIdle(const YobiAction* const action);

} // namespace detail

} // namespace cg::yobi
