#include "cg/platform/MainLoop.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/network/NetworkRuntime.hpp"

#include <bit>
#include <cstring>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::platform {
namespace {

// 原0x559384、0x55937C、0x559370：当前按字节比较GBK标题前缀。
constexpr char kPrefix559384[] = "\xC4\xA7\xC1\xA6\x8C\x9A\xD8\x90";
constexpr char kPrefix55937C[] = "\x50\x43\xBE\xAB\xEC\x60";
constexpr char kPrefix559370[] = "\x20\xD0\xC5\xCF\xA2\xD9\x59\xC1\xCF\x20";

// 原 CALLBACK@0x458840；保持 EnumWindows 的 HWND/LPARAM/BOOL 原生调用约定。
BOOL CALLBACK EnumerateWindowsCallback_458840(HWND window, LPARAM)
{
    char title[128];
    title[0] = static_cast<char>(network::ReadProtocolByte593850());
    std::memset(title + 1, 0, sizeof(title) - 1);
    RECT rect; // 原 GetWindowRect 失败后仍读取，未提供默认矩形；风险见证据文档。
    ::GetWindowTextA(window, title, static_cast<int>(sizeof(title)));
    ::GetWindowRect(window, &rect);

    const std::uint32_t height = static_cast<std::uint32_t>(rect.bottom)
        - static_cast<std::uint32_t>(rect.top);
    const std::uint32_t width = static_cast<std::uint32_t>(rect.right)
        - static_cast<std::uint32_t>(rect.left);

    auto* lowFlagByte = reinterpret_cast<unsigned char*>(&gWindowReportFlags);
    if (StrNCmp_53BFC0(title, kPrefix559384, static_cast<std::uint32_t>(sizeof(kPrefix559384) - 1)) == 0) {
        *lowFlagByte |= 1u;
    }
    if (StrNCmp_53BFC0(title, kPrefix55937C, static_cast<std::uint32_t>(sizeof(kPrefix55937C) - 1)) == 0) {
        *lowFlagByte |= 2u;
    }

    std::uint16_t flags;
    std::memcpy(&flags, &gWindowReportFlags, sizeof(flags));
    if (StrNCmp_53BFC0(title, kPrefix559370, static_cast<std::uint32_t>(sizeof(kPrefix559370) - 1)) == 0) {
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
