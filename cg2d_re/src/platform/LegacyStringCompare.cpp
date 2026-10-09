#include "cg/platform/LegacyStringCompare.hpp"

#include <cctype>
#include <cstring>

namespace cg::platform {

// MC01-C5：只转交MSVC分类，Input的ASCII输入及非零消费保持。
std::int32_t ClassifyLegacySpace_53BDEF(const std::int32_t character)
{
    return std::isspace(character);
}

std::int32_t ClassifyLegacyControl_53BE17(const std::int32_t character)
{
    return std::iscntrl(character);
}

std::int32_t CompareInsensitive_53CF50(const char* left, const char* right)
{
    return ::_stricmp(left, right);
}

std::int32_t CompareInsensitiveN_54AB30(
    const char* left, const char* right, std::uint32_t count)
{
    return ::_strnicmp(left, right, count);
}

} // namespace cg::platform
