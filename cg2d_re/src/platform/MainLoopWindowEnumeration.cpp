#include "cg/platform/MainLoop.hpp"
#include <string_view>
#include "cg/network/NetworkRuntime.hpp"

#include <bit>
#include <cstring> // 原CRT接口：0x0053BFC0；原参数／返回合同由直接调用承接。

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <cstdint>
#include <iterator>

namespace cg::platform {
namespace {

// 原 CALLBACK@0x458840；保持 EnumWindows 的 HWND/LPARAM/BOOL 原生调用约定。
BOOL CALLBACK EnumerateWindowsCallback_458840(HWND window, LPARAM)
{
    wchar_t wideTitle[128] {};
    static_cast<void>(network::ReadProtocolByte593850());
    RECT rect; // 原 GetWindowRect 失败后仍读取，未提供默认矩形；风险见证据文档。
    ::GetWindowTextW(window, wideTitle, static_cast<int>(std::size(wideTitle)));
    const std::wstring_view title(wideTitle);
    ::GetWindowRect(window, &rect);

    const std::uint32_t height = static_cast<std::uint32_t>(rect.bottom)
        - static_cast<std::uint32_t>(rect.top);
    const std::uint32_t width = static_cast<std::uint32_t>(rect.right)
        - static_cast<std::uint32_t>(rect.left);

    auto* lowFlagByte = reinterpret_cast<unsigned char*>(&gWindowReportFlags);
    if (title.starts_with(L"魔力宝贝" /* 原0x559384、0x55937C、0x559370：直接比较UTF-16标题前缀。 */)) {
        *lowFlagByte |= 1u;
    }
    if (title.starts_with(L"PC精灵" /* 原常量参考：0x0055937C。 */)) {
        *lowFlagByte |= 2u;
    }

    std::uint16_t flags;
    std::memcpy(&flags, &gWindowReportFlags, sizeof(flags));
    if (title.starts_with(L" 信息资料 " /* 原常量参考：0x00559370。 */)) {
        flags = static_cast<std::uint16_t>(flags | 4u);
        std::memcpy(&gWindowReportFlags, &flags, sizeof(flags));
    }
    if (width == 249u && height == 136u) {
        flags = static_cast<std::uint16_t>(flags | 8u);
        std::memcpy(&gWindowReportFlags, &flags, sizeof(flags));
    }

    switch (flags) {
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15: {
        // 原 0x458975..0x458988：先读低 16 位，再只清低 16 位，报告值零扩展。
        std::memcpy(&flags, &gWindowReportFlags, sizeof(flags));
        constexpr std::uint16_t zero = 0;
        std::memcpy(&gWindowReportFlags, &zero, sizeof(zero));
        gWindowReportValue = static_cast<std::int32_t>(flags);
        return FALSE;
    }
    default:
        return TRUE;
    }
}

} // namespace

// 原 [0x4587D0,0x458839)：未触发枚举时的 -1 和独立 binary64 截止均保留。
int EnumerateWindows_4587D0()
{
    if (!(gLastRenderDt > gWindowEnumerationDeadline)) {
        return -1;
    }

    int result = -1;
    if (gWindowReportRequested == 1 && gWindowReportAttempts < 3) {
        result = static_cast<int>(::EnumWindows(&EnumerateWindowsCallback_458840, static_cast<LPARAM>(0)));
        const std::uint32_t attempts = std::bit_cast<std::uint32_t>(gWindowReportAttempts);
        gWindowReportAttempts = std::bit_cast<std::int32_t>(attempts + 1u);
    }

    gWindowEnumerationDeadline = gLastRenderDt + 3000.0;
    if (result != 0) {
        return result;
    }
    return gWindowReportValue > 0 ? 1 : 0;
}

} // namespace cg::platform
