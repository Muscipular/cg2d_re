#include "ImeComposition.hpp"

#include "StartupNative.hpp"
#include "WindowProcedureState.hpp"

#include "cg/input/Input.hpp"
#include "cg/text/Encoding.hpp"

#include <imm.h>

#include <bit>
#include <cstring>
#include <optional>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

namespace cg::platform {
namespace {

// W接口的字符串长度始终是字节；属性仍为BYTE，光标值单独映射。
std::optional<std::wstring> ReadImeString(const DWORD kind)
{
    const HIMC context = reinterpret_cast<HIMC>(gImeContext);
    const LONG bytes = ::ImmGetCompositionStringW(context, kind, nullptr, 0);
    if (bytes < 0) {
        return std::nullopt;
    }
    std::wstring result(static_cast<std::size_t>(bytes) / sizeof(wchar_t), L'\0');
    const LONG copied = ::ImmGetCompositionStringW(context, kind, result.data(), static_cast<DWORD>(bytes));
    if (copied < 0) {
        return std::nullopt;
    }
    result.resize(static_cast<std::size_t>(copied) / sizeof(wchar_t));
    return result;
}

} // namespace

// 原 sub_459580@[0x459580,0x4596C8)。四个标志分支彼此独立，顺序与旧端一致。
char Sub_459580(const std::int16_t flags)
{
    const std::uint16_t flagBits = std::bit_cast<std::uint16_t>(flags);
    std::uint8_t returnByte = static_cast<std::uint8_t>(flagBits);
    if ((flagBits & GCS_RESULTSTR) != 0u) {
        if (const auto result = ReadImeString(GCS_RESULTSTR)) {
            input::AppendText(*result);
        }
    }
    if ((flagBits & GCS_COMPSTR) != 0u) {
        if (const auto composition = ReadImeString(GCS_COMPSTR)) {
            gImeComposition = *composition;
        }
    }
    if ((flagBits & GCS_COMPATTR) != 0u) {
        const HIMC context = reinterpret_cast<HIMC>(gImeContext);
        const LONG bytes = ::ImmGetCompositionStringW(context, GCS_COMPATTR, nullptr, 0);
        if (bytes >= 0) {
            std::vector<std::uint8_t> attributes(static_cast<std::size_t>(bytes));
            const LONG copied = ::ImmGetCompositionStringW(context, GCS_COMPATTR, attributes.data(), static_cast<DWORD>(bytes));
            if (copied >= 0) {
                attributes.resize(static_cast<std::size_t>(copied));
                attributes.resize(gImeComposition.size(), 0);
                gImeAttributes = std::move(attributes);
            }
        }
    }
    if ((flagBits & GCS_CURSORPOS) != 0u) {
        const LONG cursor = ::ImmGetCompositionStringW(reinterpret_cast<HIMC>(gImeContext), GCS_CURSORPOS, nullptr, 0);
        if (cursor >= 0) {
            gImeCompositionCursorPosition = static_cast<std::int32_t>(text::Prefix(gImeComposition, static_cast<std::size_t>(cursor)));
            input::gValue626C0C = 20;
            returnByte = static_cast<std::uint8_t>(gImeCompositionCursorPosition);
        }
    }
    return std::bit_cast<char>(returnByte);
}

} // namespace cg::platform
