#pragma once

#include <cstdint>

namespace cg::network::detail {

// 512E40的murD真实回复目标；第三解析数值原本体不消费，实现在LegacyAudio.cpp。
void Sub_50E680(
    std::uintptr_t socket, std::int32_t first, std::int32_t second, std::int32_t third);

} // namespace cg::network::detail
