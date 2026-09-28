#pragma once

#include <cstdint>

namespace cg::login {

// 原 0xDA3B2A 的 W16 创建回复码，独立于相邻的删除和登录回复码。
extern std::uint16_t gCharacterCreationReplyCode;

// 原 sub_508E90@[0x508E90,0x508EE0)：完成回复或 600000 ms 超时。
std::int32_t PollCharacterCreationRequest();

// 原 sub_508EE0@[0x508EE0,0x508F61)：只在请求状态为 1 时处理创建回复。
void HandleCharacterCreationResponse_508EE0(
    std::uintptr_t socket,
    const char* result,
    std::int32_t registNumber,
    const char* unusedText); // 原dispatcher仍复制并传入第四参数，本体未读取。

} // namespace cg::login
