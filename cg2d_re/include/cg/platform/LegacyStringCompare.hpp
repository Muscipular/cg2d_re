#pragma once

#include <cstdint>

namespace cg::platform {

// 原53BDEF/53BE17：直接MSVC分类；Input只消费实际ASCII字节的非零结果。
std::int32_t ClassifyLegacySpace_53BDEF(std::int32_t character);
std::int32_t ClassifyLegacyControl_53BE17(std::int32_t character);

// 原54AB30：直接MSVC _strnicmp，数量保持U32，locale由MSVC持有。
std::int32_t CompareInsensitiveN_54AB30(
    const char* left, const char* right, std::uint32_t count);

// 原53CF50：直接MSVC _stricmp，返回实际比较结果。
std::int32_t CompareInsensitive_53CF50(const char* left, const char* right);

} // namespace cg::platform
