#pragma once

#include <cstdint>

namespace cg::login {

// 原 0xDA3B28，角色删除异步响应写入的 W16 回复码，映像初值为 0。
extern std::uint16_t gCharacterDeletionReplyCode;

// 原 sub_508F70@[0x508F70,0x508FB3)；发起当前所选角色的删除请求。
std::uint32_t StartCharacterDeletionRequest();

// 原 sub_508FC0@[0x508FC0,0x509011)；轮询回复或 600000 ms 超时。
std::int32_t PollCharacterDeletionRequest();

} // namespace cg::login
