#pragma once

namespace cg::network { class CgConnection; }

#include <cstdint>

namespace cg::login {

// 原508E00：按原序取创建字段、发送aSEl，再发布请求状态和起始tick。
std::uint32_t StartCharacterCreationRequest();

// 原 sub_508E90@[0x508E90,0x508EE0)：完成回复或 600000 ms 超时。
std::int32_t PollCharacterCreationRequest();

// 原 sub_508EE0@[0x508EE0,0x508F61)：只在请求状态为 1 时处理创建回复。
void HandleCharacterCreationResponse_508EE0(cg::network::CgConnection& networkConnection,
    const char* result,
    std::int32_t registNumber,
    const char* unusedText); // 原dispatcher仍复制并传入第四参数，本体未读取。

} // namespace cg::login
