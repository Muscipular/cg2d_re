#pragma once

#include <cstdint>

namespace cg::platform {

// 原 [53CF50,53CFDC)：按原 locale 分流，保留非 C 分支的跨字节状态及完整返回。
std::int32_t CompareInsensitive_53CF50(const char* left, const char* right);

} // namespace cg::platform
