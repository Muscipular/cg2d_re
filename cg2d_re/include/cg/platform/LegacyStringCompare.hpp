#pragma once

#include <cstdint>

namespace cg::platform {

// 原541955的U8输入域；读取共享58FC30分类表，mask取原ctype位，不使用宿主locale。
std::uint16_t ClassifyLegacyByte(std::uint8_t value, std::uint16_t mask);

// 原54AB30：直接MSVC _strnicmp，数量保持U32，locale由MSVC持有。
std::int32_t CompareInsensitiveN_54AB30(
    const char* left, const char* right, std::uint32_t count);

// 原53CF50：直接MSVC _stricmp，返回实际比较结果。
std::int32_t CompareInsensitive_53CF50(const char* left, const char* right);

} // namespace cg::platform
