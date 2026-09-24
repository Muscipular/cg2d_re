#pragma once

#include <cstdint>

namespace cg::login::request_detail {

// 原 sub_512B00@0x512B00；返回值在 sub_508AF0 中被忽略。
std::int32_t Sub_512B00(
    std::uintptr_t socket,
    std::int32_t field4C,
    std::int32_t field50,
    std::int32_t field18);

} // namespace cg::login::request_detail

namespace cg::login::detail {

// 原 0xDA3B2E；与登录回复码 0xDA3B2C 分离，后续 508CF0 复用。
extern std::uint16_t gReplyCodeDA3B2E;

} // namespace cg::login::detail
