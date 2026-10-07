#pragma once

#include <cstdint>

namespace cg::login::detail {

// 原 0xDA3B2E；与登录回复码 0xDA3B2C 分离，508CF0复用。
extern std::uint16_t gReplyCodeDA3B2E;

// 原508CF0仅读result；S32比较/成功/末次调用返回保留，网络分派弃其返回。
std::int32_t Sub_508CF0(
    std::uintptr_t unusedSocket, const char* result, const char* unusedText);

} // namespace cg::login::detail
