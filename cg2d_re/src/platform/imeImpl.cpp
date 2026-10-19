#include "imeImpl.hpp"

#include "StartupNative.hpp"
#include "WindowProcedureState.hpp"

#include "cg/input/Input.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/text/Encoding.hpp"

#include <imm.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

namespace cg::platform {

// W接口的字符串长度始终是字节；属性仍为BYTE，光标值单独映射。
static std::optional<std::wstring> ReadImeString(const DWORD kind)
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

// 原 sub_458FD0@[0x458FD0,0x459032)。调用顺序与旧端一致。
int InitializeIme_458FD0()
{
    const std::uintptr_t previousActiveContext = gImeContext;
    gImeComposition.clear();
    gImeAttributes.assign(1, 0);
    gImeState73EFF0 = 0;
    if (previousActiveContext != 0) {
        ShutdownIme_459040();
    }

    const HIMC createdContext = ImmCreateContext();
    gImeContext = reinterpret_cast<std::uintptr_t>(createdContext);
    const HWND window = reinterpret_cast<HWND>(gLegacyWindow);
    const HIMC previousContext = ImmAssociateContext(window, createdContext);
    // 原459007先重取当前context，459018再发布previous槽。
    const HIMC currentContext = reinterpret_cast<HIMC>(gImeContext);
    gPreviousImeContext = reinterpret_cast<std::uintptr_t>(previousContext);

    static_assert(sizeof(DWORD) == sizeof(gImeConversionMode));
    ImmGetConversionStatus(
        currentContext,
        reinterpret_cast<DWORD*>(&gImeConversionMode),
        reinterpret_cast<DWORD*>(&gImeSentenceMode));
    gImeDescription.clear();
    return 0;
}

// 原 sub_459040@[0x459040,0x459081)。原函数不销毁或清空 IME context。
void ShutdownIme_459040()
{
    const HIMC currentContext = reinterpret_cast<HIMC>(gImeContext);
    const HWND releaseWindow = reinterpret_cast<HWND>(gLegacyWindow);
    ImmReleaseContext(releaseWindow, currentContext);
    const HIMC previousContext = reinterpret_cast<HIMC>(gPreviousImeContext);
    const HWND associateWindow = reinterpret_cast<HWND>(gLegacyWindow);
    ImmAssociateContext(associateWindow, previousContext);
    if (gImeCandidateList != nullptr) {
        std::free(gImeCandidateList);
        gImeCandidateList = nullptr;
    }
}

// HWT01：当前线程布局先查询，再返回ImmIsIME完整Win32 BOOL，不转bool或补默认值。
// 原函数：0x00459090。
std::int32_t IsImeKeyboardLayout_459090() noexcept
{
    const HKL keyboardLayout = GetKeyboardLayout(0);
    const BOOL result = ImmIsIME(keyboardLayout);
    static_assert(sizeof(result) == sizeof(std::int32_t));
    return std::bit_cast<std::int32_t>(result);
}

// 原函数：0x004590A0。
std::int32_t ResetImeOpenStatus_4590A0() noexcept
{
    const std::int32_t isIme = IsImeKeyboardLayout_459090();
    if (isIme != 1) {
        return isIme;
    }

    const std::uintptr_t closeContext = gImeContext;
    static_cast<void>(ImmSetOpenStatus(
        reinterpret_cast<HIMC>(closeContext),
        FALSE));

    const std::uintptr_t openContext = gImeContext;
    const BOOL result = ImmSetOpenStatus(
        reinterpret_cast<HIMC>(openContext),
        TRUE);
    static_assert(sizeof(result) == sizeof(std::int32_t));
    return std::bit_cast<std::int32_t>(result);
}

// HWT01：这是4731E0共用尾的窄平台helper；保IME查询、256B未用键表采样和描述借用顺序。
std::wstring_view QueryWindowImeDescription_4734F0() noexcept
{
    static_cast<void>(IsImeKeyboardLayout_459090());
    std::array<BYTE, 256> keyboardState;
    static_cast<void>(GetKeyboardState(keyboardState.data()));
    return gImeDescription; // 原0x004590D0：借用同一描述，不更新或复制。
}

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
            input::gInputCaretBlinkPhase_626C0C = 20;
            returnByte = static_cast<std::uint8_t>(gImeCompositionCursorPosition);
        }
    }
    return std::bit_cast<char>(returnByte);
}

} // namespace cg::platform
