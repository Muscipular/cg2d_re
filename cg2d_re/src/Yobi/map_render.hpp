#pragma once

#include "base.hpp"

#include <cstdint>

namespace cg::yobi {

namespace detail {

// 原函数：0x00442F70。
void Sub_442F70(YobiAction* action, std::int32_t primaryIndex);

// 原444260：无payload附件，仅借用可选的父引用槽，不改变通用释放逻辑。
YobiAction* Sub_444260(
    const std::int32_t imageNo, const std::int32_t mapX,
    const std::int32_t mapY, const std::int32_t characterAction,
    const std::int32_t direction, const std::uint8_t displayPriority,
    YobiAction** const completionSlot);

} // namespace detail

} // namespace cg::yobi
