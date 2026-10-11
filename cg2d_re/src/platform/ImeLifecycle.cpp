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
    const std::uintptr_t previousActiveContext = gImeContext;
    gImeComposition[0] = '\0';
    gImeAttributes[0] = 0;
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
    std::memset(gImeDescription, 0, sizeof(gImeDescription));
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

// HWT01：原4590D0借用唯一100B IME描述，保原可写char*地址和owner。
char* BorrowImeDescription_4590D0() noexcept
{
    return gImeDescription;
}

// HWT01：这是4731E0共用尾的窄平台helper；保IME查询、256B未用键表采样和描述借用顺序。
const char* BorrowWindowImeDescription_4734F0() noexcept
{
    static_cast<void>(IsImeKeyboardLayout_459090());
    std::array<BYTE, 256> keyboardState;
    static_cast<void>(GetKeyboardState(keyboardState.data()));
    return BorrowImeDescription_4590D0();
}

} // namespace cg::platform
