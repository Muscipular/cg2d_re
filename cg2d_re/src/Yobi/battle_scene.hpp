#pragma once

#include "base.hpp"

#include <cstdint>

namespace cg::yobi {

namespace detail {

// 共享原状态，只在本组实现中定义；原全局：0x005ABB24。
extern std::int32_t gBattlePendingGroupCount5ABB24;

} // namespace detail

} // namespace cg::yobi
