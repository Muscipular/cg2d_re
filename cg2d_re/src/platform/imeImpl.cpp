#include "cg/session/ClientSession.hpp"
#include "cg/session/ClientHost.hpp"
#include "imeImpl.hpp"

#include "StartupNative.hpp"

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

// 原73EDDC改为窗口唯一借用基线，后台会话不保存另一会话的上下文。
static HIMC gWindowDefaultImeContext = nullptr;
static bool gWindowDefaultImeContextCaptured = false;

static HIMC ReadWindowImeContext() noexcept
{
    const HWND window = reinterpret_cast<HWND>(gLegacyWindow);
    const HIMC context = ImmGetContext(window);
    if (context != nullptr) {
        ImmReleaseContext(window, context);
    }
    return context;
}

static void ReleaseOwnedImeContext(std::uintptr_t& slot) noexcept
{
    const HIMC context = reinterpret_cast<HIMC>(slot);
    if (context == nullptr) return;
    if (ReadWindowImeContext() == context) {
        ImmAssociateContext(reinterpret_cast<HWND>(gLegacyWindow), gWindowDefaultImeContext);
        if (ReadWindowImeContext() == context) return;
    }
    if (ImmDestroyContext(context) != FALSE) {
        slot = 0;
    }
}

ImeRuntimeState::~ImeRuntimeState()
{
    ReleaseOwnedImeContext(context);
    std::free(candidateList);
}

static bool CancelSessionIme(session::ClientSession& session) noexcept
{
    auto& ime = session.UiFrame().ime;
    const HIMC context = reinterpret_cast<HIMC>(ime.context);
    if (context == nullptr) return false;
    const bool hasComposition = !ime.composition.empty()
        || ImmGetCompositionStringW(context, GCS_COMPSTR, nullptr, 0) > 0;
    if (hasComposition
        && ImmNotifyIME(context, NI_COMPOSITIONSTR, CPS_CANCEL, 0) == FALSE) {
        return false;
    }
    // 取消可能同步派发关闭通知；重读所属候选指针，不重复释放旧快照。
    if (ime.candidateList != nullptr
        && ImmNotifyIME(context, NI_CLOSECANDIDATE, 0, 0) == FALSE) {
        return false;
    }
    ime.composition.clear();
    ime.attributes.clear();
    ime.cursorPosition = 0;
    ime.state73EFF0 = 0;
    std::free(ime.candidateList);
    ime.candidateList = nullptr;
    return true;
}

bool AssociateSessionIme(session::ClientSession& session) noexcept
{
    const HIMC context = reinterpret_cast<HIMC>(session.UiFrame().ime.context);
    if (context == nullptr) return false;
    if (ReadWindowImeContext() == context) return true;
    if (auto* const previousSession = FindWindowImeSession()) {
        if (!CancelSessionIme(*previousSession)) return false;
    }
    const HIMC previous = ImmAssociateContext(reinterpret_cast<HWND>(gLegacyWindow), context);
    if (ReadWindowImeContext() != context) return false;
    if (!gWindowDefaultImeContextCaptured) {
        gWindowDefaultImeContext = previous;
        gWindowDefaultImeContextCaptured = true;
    }
    return true;
}

session::ClientSession* FindWindowImeSession() noexcept
{
    const HIMC context = ReadWindowImeContext();
    if (context == nullptr) return nullptr;
    auto& host = session::GetClientHost();
    for (const auto& entry : host.Sessions()) {
        if (reinterpret_cast<HIMC>(entry.session.UiFrame().ime.context) == context) {
            return host.FindSession(entry.id);
        }
    }
    return nullptr;
}

// W接口的字符串长度始终是字节；属性仍为BYTE，光标值单独映射。
static std::optional<std::wstring> ReadImeString(session::ClientSession& session, const DWORD kind)
{
    const HIMC context = reinterpret_cast<HIMC>(session.UiFrame().ime.context);
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

// 原 sub_458FD0@[0x458FD0,0x459032)；NSM01只在初始化当前会话时关联窗口。
int InitializeIme_458FD0(session::ClientSession& session)
{
    const std::uintptr_t previousActiveContext = session.UiFrame().ime.context;
    session.UiFrame().ime.composition.clear();
    session.UiFrame().ime.attributes.assign(1, 0);
    session.UiFrame().ime.state73EFF0 = 0;
    if (previousActiveContext != 0) {
        ShutdownIme_459040(session);
        if (session.UiFrame().ime.context != 0) return -1;
    }

    const HIMC createdContext = ImmCreateContext();
    if (createdContext == nullptr) return -1;
    session.UiFrame().ime.context = reinterpret_cast<std::uintptr_t>(createdContext);
    if (&session == session::GetClientHost().CurrentSession()) {
        if (!AssociateSessionIme(session)) {
            ReleaseOwnedImeContext(session.UiFrame().ime.context);
            return -1;
        }
    }
    const HIMC currentContext = reinterpret_cast<HIMC>(session.UiFrame().ime.context);

    static_assert(sizeof(DWORD) == sizeof(session.UiFrame().ime.conversionMode));
    ImmGetConversionStatus(
        currentContext,
        reinterpret_cast<DWORD*>(&session.UiFrame().ime.conversionMode),
        reinterpret_cast<DWORD*>(&session.UiFrame().ime.sentenceMode));
    session.UiFrame().ime.description.clear();
    return 0;
}

// 原 sub_459040@[0x459040,0x459081)；NSM01在解除自身关联后销毁独占上下文。
void ShutdownIme_459040(session::ClientSession& session)
{
    ReleaseOwnedImeContext(session.UiFrame().ime.context);
    if (session.UiFrame().ime.candidateList != nullptr) {
        std::free(session.UiFrame().ime.candidateList);
        session.UiFrame().ime.candidateList = nullptr;
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
std::int32_t ResetImeOpenStatus_4590A0(session::ClientSession& session) noexcept
{
    const std::int32_t isIme = IsImeKeyboardLayout_459090();
    if (isIme != 1) {
        return isIme;
    }

    const std::uintptr_t closeContext = session.UiFrame().ime.context;
    static_cast<void>(ImmSetOpenStatus(
        reinterpret_cast<HIMC>(closeContext),
        FALSE));

    const std::uintptr_t openContext = session.UiFrame().ime.context;
    const BOOL result = ImmSetOpenStatus(
        reinterpret_cast<HIMC>(openContext),
        TRUE);
    static_assert(sizeof(result) == sizeof(std::int32_t));
    return std::bit_cast<std::int32_t>(result);
}

// HWT01：这是4731E0共用尾的窄平台helper；保IME查询、256B未用键表采样和描述借用顺序。
std::wstring_view QueryWindowImeDescription_4734F0(session::ClientSession& session) noexcept
{
    static_cast<void>(IsImeKeyboardLayout_459090());
    std::array<BYTE, 256> keyboardState;
    static_cast<void>(GetKeyboardState(keyboardState.data()));
    return session.UiFrame().ime.description; // 原0x004590D0：借用同一描述，不更新或复制。
}

// 原 sub_459580@[0x459580,0x4596C8)。四个标志分支彼此独立，顺序与旧端一致。
char HandleImeCompositionFlags_459580(session::ClientSession& session, const std::int16_t flags)
{
    const std::uint16_t flagBits = std::bit_cast<std::uint16_t>(flags);
    std::uint8_t returnByte = static_cast<std::uint8_t>(flagBits);
    if ((flagBits & GCS_RESULTSTR) != 0u) {
        if (const auto result = ReadImeString(session, GCS_RESULTSTR)) {
            input::AppendText(session, *result);
        }
    }
    if ((flagBits & GCS_COMPSTR) != 0u) {
        if (const auto composition = ReadImeString(session, GCS_COMPSTR)) {
            session.UiFrame().ime.composition = *composition;
        }
    }
    if ((flagBits & GCS_COMPATTR) != 0u) {
        const HIMC context = reinterpret_cast<HIMC>(session.UiFrame().ime.context);
        const LONG bytes = ::ImmGetCompositionStringW(context, GCS_COMPATTR, nullptr, 0);
        if (bytes >= 0) {
            std::vector<std::uint8_t> attributes(static_cast<std::size_t>(bytes));
            const LONG copied = ::ImmGetCompositionStringW(context, GCS_COMPATTR, attributes.data(), static_cast<DWORD>(bytes));
            if (copied >= 0) {
                attributes.resize(static_cast<std::size_t>(copied));
                attributes.resize(session.UiFrame().ime.composition.size(), 0);
                session.UiFrame().ime.attributes = std::move(attributes);
            }
        }
    }
    if ((flagBits & GCS_CURSORPOS) != 0u) {
        const LONG cursor = ::ImmGetCompositionStringW(reinterpret_cast<HIMC>(session.UiFrame().ime.context), GCS_CURSORPOS, nullptr, 0);
        if (cursor >= 0) {
            session.UiFrame().ime.cursorPosition = static_cast<std::int32_t>(text::Prefix(session.UiFrame().ime.composition, static_cast<std::size_t>(cursor)));
            session.UiFrame().inputState.caretBlinkPhase = 20;
            returnByte = static_cast<std::uint8_t>(session.UiFrame().ime.cursorPosition);
        }
    }
    return std::bit_cast<char>(returnByte);
}

} // namespace cg::platform
