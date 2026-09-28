#pragma once

#include <cstdint>

namespace cg::login::detail {

// 原 0xDA3B2E；与登录回复码 0xDA3B2C 分离，508CF0复用。
extern std::uint16_t gReplyCodeDA3B2E;

// 原508CF0完整登出回复；仅读取result，成功异步分支写世界同源73ECD0。
void Sub_508CF0(
    std::uintptr_t unusedSocket, const char* result, const char* unusedText);

} // namespace cg::login::detail
