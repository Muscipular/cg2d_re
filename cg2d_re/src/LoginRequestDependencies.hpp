#pragma once

#include <cstdint>

namespace cg::login::detail {

// 原 0xDA3B2E；与登录回复码 0xDA3B2C 分离，后续 508CF0 复用。
extern std::uint16_t gReplyCodeDA3B2E;

} // namespace cg::login::detail
