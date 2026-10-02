#pragma once

#include <cstdint>

namespace cg::platform {

// 原541955的U8输入域；读取共享58FC30分类表，mask取原ctype位，不使用宿主locale。
std::uint16_t ClassifyLegacyByte(std::uint8_t value, std::uint16_t mask);

// 原 [53CF50,53CFDC)：按原 locale 分流，保留非 C 分支的跨字节状态及完整返回。
std::int32_t CompareInsensitive_53CF50(const char* left, const char* right);

} // namespace cg::platform
