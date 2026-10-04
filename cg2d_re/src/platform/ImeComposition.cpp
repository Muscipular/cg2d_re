#include "ImeComposition.hpp"

#include "StartupNative.hpp"
#include "WindowProcedureState.hpp"

#include "cg/Input.hpp"

#include <imm.h>

#include <bit>
#include <cstring>

namespace cg::platform {
namespace {

constexpr LONG kImeBufferCapacity = 256;
constexpr LONG kImeMaximumByteCount = kImeBufferCapacity - 1;

} // namespace

// 原 sub_459580@[0x459580,0x4596C8)。四个标志分支彼此独立，顺序与旧端一致。
char Sub_459580(const std::int16_t flags)
{
    const std::uint16_t flagBits = std::bit_cast<std::uint16_t>(flags);
    std::uint8_t returnByte = static_cast<std::uint8_t>(flagBits);

    if ((flagBits & GCS_RESULTSTR) != 0u) {
        // 原局部未预清；仅第二次API后在查询长度处写NUL。
        char result[kImeBufferCapacity];
        LONG byteCount = ImmGetCompositionStringA(
            reinterpret_cast<HIMC>(gImeContext),
            GCS_RESULTSTR,
            nullptr,
            0);
        if (byteCount >= kImeBufferCapacity) {
            byteCount = kImeMaximumByteCount;
        }
        static_cast<void>(ImmGetCompositionStringA(
            reinterpret_cast<HIMC>(gImeContext),
            GCS_RESULTSTR,
            result,
            static_cast<DWORD>(byteCount)));
        result[byteCount] = '\0';

        std::int32_t resultLength = static_cast<std::int32_t>(std::strlen(result));
        if (resultLength >= kImeBufferCapacity) {
            resultLength = kImeMaximumByteCount;
        }
        for (std::int32_t index = 0; index < resultLength; ++index) {
            input::AppendChar_44ADB0(result[index]);
        }
    }

    if ((flagBits & GCS_COMPSTR) != 0u) {
        LONG byteCount = ImmGetCompositionStringA(
            reinterpret_cast<HIMC>(gImeContext),
            GCS_COMPSTR,
            nullptr,
            0);
        if (byteCount >= kImeBufferCapacity) {
            byteCount = kImeMaximumByteCount;
        }
        static_cast<void>(ImmGetCompositionStringA(
            reinterpret_cast<HIMC>(gImeContext),
            GCS_COMPSTR,
            gImeComposition,
            static_cast<DWORD>(byteCount)));
        gImeComposition[byteCount] = '\0';
    }

    if ((flagBits & GCS_COMPATTR) != 0u) {
        LONG byteCount = ImmGetCompositionStringA(
            reinterpret_cast<HIMC>(gImeContext),
            GCS_COMPATTR,
            nullptr,
            0);
        if (byteCount >= kImeBufferCapacity) {
            byteCount = kImeMaximumByteCount;
        }
        static_cast<void>(ImmGetCompositionStringA(
            reinterpret_cast<HIMC>(gImeContext),
            GCS_COMPATTR,
            gImeAttributes,
            static_cast<DWORD>(byteCount)));
        gImeAttributes[byteCount] = 0;
    }

    if ((flagBits & GCS_CURSORPOS) != 0u) {
        const LONG cursorPosition = ImmGetCompositionStringA(
            reinterpret_cast<HIMC>(gImeContext),
            GCS_CURSORPOS,
            nullptr,
            0);
        static_assert(sizeof(cursorPosition) == sizeof(gImeCompositionCursorPosition));
        gImeCompositionCursorPosition = cursorPosition;
        input::gValue626C0C = 20;
        returnByte = static_cast<std::uint8_t>(cursorPosition);
    }

    return std::bit_cast<char>(returnByte);
}

} // namespace cg::platform
