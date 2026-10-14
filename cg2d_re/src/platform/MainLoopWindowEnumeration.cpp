#include "cg/platform/MainLoop.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/platform/LegacyTime.hpp"
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

// 原0x559384、0x55937C、0x559370：当前按 UTF-8 比较 Unicode 标题前缀。
constexpr wchar_t kPrefix559384[] = L"魔力寶貝";
const std::string kPrefix559384Utf8View = cg::text::FromUtf16(kPrefix559384); // 旧窄接口只读兼容视图。
constexpr wchar_t kPrefix55937C[] = L"PC精靈";
const std::string kPrefix55937CUtf8View = cg::text::FromUtf16(kPrefix55937C); // 旧窄接口只读兼容视图。
constexpr wchar_t kPrefix559370[] = L" 信息資料 ";
const std::string kPrefix559370Utf8View = cg::text::FromUtf16(kPrefix559370); // 旧窄接口只读兼容视图。

// 原 CALLBACK@0x458840；保持 EnumWindows 的 HWND/LPARAM/BOOL 原生调用约定。
BOOL CALLBACK EnumerateWindowsCallback_458840(HWND window, LPARAM)
{
    wchar_t wideTitle[128] {};
    char title[384];
    title[0] = static_cast<char>(network::ReadProtocolByte593850());
    std::memset(title + 1, 0, sizeof(title) - 1);
    RECT rect; // 原 GetWindowRect 失败后仍读取，未提供默认矩形；风险见证据文档。
    ::GetWindowTextW(window, wideTitle, static_cast<int>(std::size(wideTitle)));
    cg::text::Copy(title, sizeof(title), cg::text::FromUtf16(wideTitle));
    ::GetWindowRect(window, &rect);

    const std::uint32_t height = static_cast<std::uint32_t>(rect.bottom)
        - static_cast<std::uint32_t>(rect.top);
    const std::uint32_t width = static_cast<std::uint32_t>(rect.right)
        - static_cast<std::uint32_t>(rect.left);

    auto* lowFlagByte = reinterpret_cast<unsigned char*>(&gWindowReportFlags);
    if (std::strncmp(title, kPrefix559384Utf8View.c_str(), static_cast<std::uint32_t>(static_cast<std::uint32_t>((kPrefix559384Utf8View.size() + 1) - 1))) == 0) {
        *lowFlagByte |= 1u;
    }
    if (std::strncmp(title, kPrefix55937CUtf8View.c_str(), static_cast<std::uint32_t>(static_cast<std::uint32_t>((kPrefix55937CUtf8View.size() + 1) - 1))) == 0) {
        *lowFlagByte |= 2u;
    }

    std::uint16_t flags;
    std::memcpy(&flags, &gWindowReportFlags, sizeof(flags));
    if (std::strncmp(title, kPrefix559370Utf8View.c_str(), static_cast<std::uint32_t>(static_cast<std::uint32_t>((kPrefix559370Utf8View.size() + 1) - 1))) == 0) {
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
