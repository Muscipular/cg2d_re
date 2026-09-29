#pragma once

#include <cstdint>

namespace cg::network::detail {

// 512E40尚缺的真实回复目标；仅声明已证调用侧合同，不用空体填充。
void Sub_50E680(
    std::uintptr_t socket, std::int32_t first, std::int32_t second, std::int32_t third);

} // namespace cg::network::detail
