#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/platform/CommandOptions.hpp"
#include "cg/core/m_log.hpp"

#include "cg/scene/login/LoginScene.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "../scene/login/LoginSavedState.hpp"
#include "cg/resource/ResourceNames.hpp"
#include "cg/resource/ResourcePaths.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/world/SceneState.hpp"
#include "StartupNative.hpp"

#include "cg/text/Encoding.hpp"
#include <bit>
#include <cstddef>
#include <cstdlib>
#include <cstdio> // 原格式化／扫描接口：0x0053B304、0x0053CADC；直接沿MSVC CRT。
#include <cwchar>
#include <cstring> // 原CRT接口：0x0053BE40、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <string>
#include <string_view>

#include "cg/core/vm.h"

namespace cg::platform {

// std::wstring gNewestText = L"/update/newestV2.txt"; // 原全局：0x0058ADA8。
std::wstring gMapPath = L"map"; // 原全局：0x0055DA00。
std::int32_t gDeviceBitsPerPixel = 8; // 原全局：0x00554B70。
std::uint16_t gScanlineSyncMode = 1; // 原全局：0x00554B74。
std::int32_t gNoDelay = 1; // 原全局：0x0055CE68。
std::uint32_t gRendererFlags = 0; // 原全局：0x009A6EC0。
std::int32_t gEnableSound = 1; // 原全局：0x0058D3B8。
std::int32_t gEnableMusic = 1; // 原全局：0x0058D3BC。
std::int32_t gUseStereo = 1; // 原全局：0x0058D3C4。
std::int32_t gEffectVolume = 15; // 原全局：0x0058D3C8。
std::int32_t gMusicVolume = 15; // 原全局：0x0058D3CC。
// 原连续零填充区：前缀、未命名间隔和 W32 运行表不得拆为独立存储。
struct BgmSelectionStorage final {
    std::uint8_t prefix[16]; // 原全局：0x01050840。
    std::uint8_t gap[8]; // 原全局：0x01050850。
    std::uint32_t runtimeWords[200]; // 原全局：0x01050858。
};
static_assert(offsetof(BgmSelectionStorage, runtimeWords) == 0x18);
static_assert(sizeof(BgmSelectionStorage) == 0x338);
static BgmSelectionStorage gBgmSelection1050840 {}; // 原全局：0x01050840。
std::uint8_t (&gBgmFrequencySelectionPrefix)[16] = gBgmSelection1050840.prefix; // 原全局：0x01050840。

std::int8_t ReadBgmFrequencySelectionByte(const std::uint8_t id) noexcept
{
    const auto* bytes =
        reinterpret_cast<const std::uint8_t*>(&gBgmSelection1050840);
    return std::bit_cast<std::int8_t>(bytes[id]);
}

std::int32_t gRendererModeStorage = 0; // 原全局：0x0062A640。
std::uint8_t& gRendererMode = reinterpret_cast<std::uint8_t*>(&gRendererModeStorage)[0]; // 原全局：0x0062A640。

static_assert(sizeof(int) == 4);
static_assert(std::endian::native == std::endian::little);

// A08：保留首次关键字、空白及引号边界，不改变原参数解析顺序。
__declspec(noinline)
static bool ParseCommandArgumentValue(
    const wchar_t* commandLine, const wchar_t* key, std::wstring& outValue)
{
    if (commandLine == nullptr || key == nullptr) {
        return false;
    }

    const wchar_t* p = std::wcsstr(commandLine, key);
    if (p == nullptr) {
        return false;
    }
    p += std::wcslen(key);

    while (*p == ' ' || *p == '\t') {
        ++p;
    }
    if (*p == '\0') {
        return false;
    }

    const wchar_t* start = p;
    const wchar_t* end = nullptr;
    if (*p == '"') {
        start = p + 1;
        end = std::wcschr(start, '"');
        if (end == nullptr) {
            end = start + std::wcslen(start);
        }
    } else {
        end = start;
        while (*end != '\0' && *end != ' ' && *end != '\t') {
            ++end;
        }
    }

    if (end <= start) {
        return false;
    }
    outValue.assign(start, static_cast<std::size_t>(end - start));
    return !outValue.empty();
}

// A08：既有 IP 表解析完成后，向实际登录消费者发布参数及场景。
__declspec(noinline)
static void ApplyAutoLoginCommandLine(network::CgConnection& connection, const wchar_t* commandLine)
{
    std::wstring account;
    std::wstring password;
    std::wstring serverText;
    std::wstring characterSlotText;
    std::int32_t serverIndex = -1;
    auto& request = connection.LoginRequests();
    VM_LIGHT_START

    if (ParseCommandArgumentValue(commandLine, L"/uu:", account)
        && ParseCommandArgumentValue(commandLine, L"/pw:", password)
        && ParseCommandArgumentValue(commandLine, L"/sv:", serverText)
        && ParseCommandArgumentValue(commandLine, L"/cv:", characterSlotText)) {
        wchar_t* endPtr = nullptr;
        long value = std::wcstol(serverText.c_str(), &endPtr, 10);
        serverIndex = (endPtr != nullptr && *endPtr == '\0')
            ? static_cast<std::int32_t>(value) : -1;
        endPtr = nullptr;
        value = std::wcstol(characterSlotText.c_str(), &endPtr, 10);
        request.autoLoginCharacterSlot = (endPtr != nullptr && *endPtr == '\0')
            ? static_cast<std::int32_t>(value) : -1;
        if (request.autoLoginCharacterSlot > 1) {
            request.autoLoginCharacterSlot = -1;
        }
    }

    if (serverIndex >= 0 && !account.empty() && !password.empty()
        && text::ToGbk(account).size() < login::LoginInputValueCapacity
        && text::ToGbk(password).size() < login::LoginInputValueCapacity) {
        login::detail::CacheAutoLoginInputValues(connection.Owner(), account, password);
        connection.Owner().WorldScene().sceneState = 2;
        connection.Owner().WorldScene().sceneType = 2;
        request.password = password;
        request.account = account;
        request.selectedServerIndex = std::bit_cast<std::int16_t>(
            static_cast<std::uint16_t>(serverIndex));
        request.autoLoginPending = true;
    }
    VM_LIGHT_END
}

__declspec(noinline)
bool ConsumeAutoLoginCharacterSlot(network::CgConnection& connection, const std::uint32_t slot) noexcept
{
    VM_LIGHT_START
    bool ret = false;
    auto& request = connection.LoginRequests();
    if (request.autoLoginPending && request.autoLoginCharacterSlot >= 0
        && slot == static_cast<std::uint32_t>(request.autoLoginCharacterSlot))
    {
        request.autoLoginPending = false;
        ret = true;
    }
    VM_LIGHT_END
    return ret;
}

// 原 ParseCmdOpt@0x465F00；原参数见 doc/cg2d_command_options.md，A08扩展见ALM01 SPEC。
__declspec(noinline)
int ParseCommandOptions(network::CgConnection& connection)
{
    VM_LIGHT_START
    wchar_t savedata[128] = L"";
    // if (wchar_t* match = const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"newest:"))) {
    //     std::swscanf(match + std::wcslen(L"newest:"), L"%127ls", gNewestText);
    // }
    if (wchar_t* match = const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"savedata:"))) {
        std::swscanf(match + std::wcslen(L"savedata:"), L"%127ls", savedata);
    }
    if (wchar_t* match = const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"datafolder:"))) {
        wchar_t value[128]{}; // 旧命令行解析边界，存储转为宽路径。
        if (std::swscanf(match + std::wcslen(L"datafolder:"), L"%127ls", value) == 1) gDataPath = value;
    }
    if (wchar_t* match = const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"binfolder:"))) {
        wchar_t value[128]{};
        if (std::swscanf(match + std::wcslen(L"binfolder:"), L"%127ls", value) == 1) resource::gBinPath = value;
    }
    if (wchar_t* match = const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"mapfolder:"))) {
        wchar_t value[128]{}; // 旧命令行解析边界，存储转为宽路径。
        if (std::swscanf(match + std::wcslen(L"mapfolder:"), L"%127ls", value) == 1) gMapPath = value;
    }

    // 原 0x46605C-0x46606B 不使用 savedata 或 gDataPath，固定拼接该路径。
    cg::render::gGraphicQueue.pending.savedPath = L"data\\0\\save.dat";

    HDC deviceContext = GetDC(reinterpret_cast<HWND>(gLegacyWindow));
    gDeviceBitsPerPixel = GetDeviceCaps(deviceContext, BITSPIXEL);
    ReleaseDC(reinterpret_cast<HWND>(gLegacyWindow), deviceContext);

    if (const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"nodelay"))) {
        gNoDelay = 0;
    }
    if (wchar_t* match = const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"winpos:"))) {
        int x;
        int y;
        std::swscanf(match + std::wcslen(L"winpos:"), L"%d,%d", &x, &y);
        if (x >= 0 && y >= 0) {
            gStartX = x;
            gStartY = y;
        }
    }

    gRendererFlags = 0;
    gRendererMode = 1;
    if (wchar_t* match = const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"3Ddevice:"))) {
        std::swscanf(match + std::wcslen(L"3Ddevice:"), L"%d", &gRendererModeStorage);
    }
    if (const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"soundoff"))) {
        gEnableSound = 0;
    }
    if (const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"musicoff"))) {
        gEnableMusic = 0;
    }
    if (gRequestedRenderMode) {
        gRendererMode = gRequestedRenderMode;
    }
    if (const_cast<wchar_t*>(std::wcsstr(gCommandLine.c_str(), L"onlyactive"))) {
        gRendererFlags |= 0x10;
    }
    connection.Owner().WorldScene().sceneType = 1;
    const int result = ParseIpOptions_507C80(gCommandLine.c_str());
    ApplyAutoLoginCommandLine(connection, ::GetCommandLineW());
    VM_LIGHT_END
    return result;
}

// 原函数：0x00507C80。
int ParseIpOptions_507C80(const wchar_t* const commandLine)
{
    login::gLoginServerRecords_D69798.fill({});

    // 原 sscanf 的目标是入参栈槽；未匹配时保留其上一次 W32 内容。
    int index = std::bit_cast<int>(
        static_cast<std::uint32_t>(
            reinterpret_cast<std::uintptr_t>(commandLine)));
    const wchar_t* cursor = commandLine;
    while (const wchar_t* const match = const_cast<wchar_t*>(std::wcsstr(cursor, L"IP:"))) {
        const wchar_t* const number = match + 3;
        const int parsed = std::swscanf(number, L"%d:", &index);
        if (static_cast<std::uint32_t>(index)
            >= login::gLoginServerRecords_D69798.size()) {
            return parsed;
        }

        const wchar_t* address = number
            + (index < 10 ? 2 : index <= 99 ? 3 : 4);
        auto& record = login::gLoginServerRecords_D69798[static_cast<std::size_t>(index)];
        // NTM01：IPv6必须使用[host%数字scope]:port；验证完成后才发布记录。
        if (*address == L'[') {
            const std::wstring_view token(address, std::wcscspn(address, L" \t"));
            const auto close = token.find(L']');
            if (close == token.npos || close <= 1 || close - 1 > 0x7F
                || close + 2 >= token.size() || token[close + 1] != L':'
                || token.substr(1, close - 1).find(L':') == token.npos
                || token.substr(close + 2).find_first_not_of(L"0123456789") != token.npos
                || token.size() - close - 2 > 0x3F) {
                m_log_iw(L"[network] IPv6参数错误，格式应为IP:编号:[地址%%scope]:端口\n");
                return -1;
            }
            std::uint32_t numericPort = 0;
            for (const auto digit : token.substr(close + 2)) {
                numericPort = numericPort * 10 + static_cast<std::uint32_t>(digit - L'0');
                if (numericPort > 65535) {
                    m_log_iw(L"[network] IPv6端口超出0..65535范围\n");
                    return -1;
                }
            }
            record.address.assign(token.substr(1, close - 1));
            record.portText.assign(token.substr(close + 2));
            record.available = 1;
            ++login::gLoginServerGroupCounts_D9F2D4[static_cast<std::size_t>(index / 20)];
            cursor = address + token.size();
            continue;
        }
        const std::wstring_view token(address, std::wcscspn(address, L" \t"));
        const auto separator = token.find(L':');
        if (separator != token.npos && token.find(L':', separator + 1) != token.npos) {
            m_log_iw(L"[network] IPv6地址必须用方括号包围\n");
            return -1;
        }
        record.address.clear();
        std::size_t addressLength = 0;
        while (*address != '\0'
            && *address != ':'
            && addressLength < 0x7F) {
            record.address.push_back(*address++);
            ++addressLength;
        }

        const wchar_t* port = *address != L'\0' ? address + 1 : address;
        record.portText.clear();
        std::size_t portLength = 0;
        while (*port != '\0'
            && *port != ' '
            && *port != '\t'
            && portLength < 0x3F) {
            record.portText.push_back(*port++);
            ++portLength;
        }
        record.available = 1;
        ++login::gLoginServerGroupCounts_D9F2D4[static_cast<std::size_t>(index / 20)];
        cursor = port;
    }
    return 0;
}

} // namespace cg::platform
