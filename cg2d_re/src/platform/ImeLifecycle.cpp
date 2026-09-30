#include "ImeLifecycle.hpp"

#include "StartupNative.hpp"
#include "WindowProcedureState.hpp"

#include "cg/platform/LegacyWindow.hpp"

#include <imm.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>

namespace cg::platform {

// 原 sub_458FD0@[0x458FD0,0x459032)。调用顺序与旧端一致。
int InitializeIme_458FD0()
{
    gImeComposition[0] = '\0';
    gImeAttributes[0] = 0;
    gImeState73EFF0 = 0;
    if (gImeContext != 0) {
        ShutdownIme_459040();
    }

    gImeContext = reinterpret_cast<std::uintptr_t>(ImmCreateContext());
    gPreviousImeContext = reinterpret_cast<std::uintptr_t>(ImmAssociateContext(
        reinterpret_cast<HWND>(gLegacyWindow),
        reinterpret_cast<HIMC>(gImeContext)));

    static_assert(sizeof(DWORD) == sizeof(gImeConversionMode));
    ImmGetConversionStatus(
        reinterpret_cast<HIMC>(gImeContext),
        reinterpret_cast<DWORD*>(&gImeConversionMode),
        reinterpret_cast<DWORD*>(&gImeSentenceMode));
    std::memset(gImeDescription, 0, sizeof(gImeDescription));
    return 0;
}

// 原 sub_459040@[0x459040,0x459081)。原函数不销毁或清空 IME context。
void ShutdownIme_459040()
{
    ImmReleaseContext(
        reinterpret_cast<HWND>(gLegacyWindow),
        reinterpret_cast<HIMC>(gImeContext));
    ImmAssociateContext(
        reinterpret_cast<HWND>(gLegacyWindow),
        reinterpret_cast<HIMC>(gPreviousImeContext));
    if (gImeCandidateList != nullptr) {
        std::free(gImeCandidateList);
        gImeCandidateList = nullptr;
    }
}

std::int32_t IsImeKeyboardLayout_459090() noexcept
{
    const HKL keyboardLayout = GetKeyboardLayout(0);
    const BOOL result = ImmIsIME(keyboardLayout);
    static_assert(sizeof(result) == sizeof(std::int32_t));
    return std::bit_cast<std::int32_t>(result);
}

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

char* BorrowImeDescription_4590D0() noexcept
{
    return gImeDescription;
}

const char* BorrowWindowImeDescription_4734F0() noexcept
{
    static_cast<void>(IsImeKeyboardLayout_459090());
    std::array<BYTE, 256> keyboardState;
    static_cast<void>(GetKeyboardState(keyboardState.data()));
    return BorrowImeDescription_4590D0();
}

} // namespace cg::platform
