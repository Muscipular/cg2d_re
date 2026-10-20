#pragma once

namespace cg::network { class CgConnection; }

#include <cstdint>

namespace cg::network::detail {

// 512E40的murD真实回复目标；第三解析数值原本体不消费，实现在LegacyAudio.cpp。
// 原函数：0x0050E680。
void HandleServerSoundPlayback_50E680(cg::network::CgConnection& networkConnection, std::int32_t first, std::int32_t second, std::int32_t third);

} // namespace cg::network::detail
