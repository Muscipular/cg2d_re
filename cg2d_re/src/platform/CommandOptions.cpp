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
#include <array>
#include <bit>
#include <cstddef>
#include <cstdlib>
#include <cstdio> // 原格式化／扫描接口：0x0053B304、0x0053CADC；直接沿MSVC CRT。
#include <cwchar>
#include <cstring> // 原CRT接口：0x0053BE40、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <string>
#include <string_view>
#include <limits>

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

struct AutoLoginArgument {
    std::uint64_t group;
    std::size_t field; // uu/pw/sv/cv分别对应0/1/2/3。
    std::size_t begin;
    std::size_t end;
    std::wstring_view value; // 借用本次扫描的命令行；消费移动正文后不得继续使用。
};

// NSM01：只识别独立登录参数；其他参数的引号正文不会被扫描成新会话。
static bool NextAutoLoginArgument(
    const std::wstring_view commandLine, std::size_t& cursor, AutoLoginArgument& argument) noexcept
{
    static constexpr std::array<std::wstring_view, 4> keys {L"/uu", L"/pw", L"/sv", L"/cv"};
    while (cursor < commandLine.size()) {
        const auto begin = commandLine.find_first_not_of(L" \t", cursor);
        if (begin == commandLine.npos) break;
        std::size_t field = 0;
        while (field < keys.size() && !commandLine.substr(begin).starts_with(keys[field])) ++field;
        std::size_t position = begin + 3;
        std::uint64_t group = 0;
        bool valid = field < keys.size();
        if (valid) {
            while (position < commandLine.size() && commandLine[position] >= L'0' && commandLine[position] <= L'9') {
                const auto digit = static_cast<std::uint64_t>(commandLine[position] - L'0');
                if (group > (std::numeric_limits<std::uint64_t>::max() - digit) / 10) {
                    valid = false;
                } else if (valid) {
                    group = group * 10 + digit;
                }
                ++position;
            }
            valid = valid && position < commandLine.size() && commandLine[position] == L':';
        }
        if (valid) {
            std::size_t valueBegin = position + 1;
            while (valueBegin < commandLine.size()
                && (commandLine[valueBegin] == L' ' || commandLine[valueBegin] == L'\t')) ++valueBegin;
            if (valueBegin > position + 1 && valueBegin < commandLine.size() && commandLine[valueBegin] == L'/') {
                // 缺值不吞掉下一项，尤其不能将其他编号组作为当前组的正文消费。
                cursor = position + 1;
                argument = {group, field, begin, cursor, {}};
                return true;
            }
            const bool quoted = valueBegin < commandLine.size() && commandLine[valueBegin] == L'"';
            if (quoted) ++valueBegin;
            auto valueEnd = quoted ? commandLine.find(L'"', valueBegin)
                : commandLine.find_first_of(L" \t", valueBegin);
            if (valueEnd == commandLine.npos) valueEnd = commandLine.size();
            cursor = valueEnd + (quoted && valueEnd < commandLine.size() ? 1 : 0);
            argument = {group, field, begin, cursor, commandLine.substr(valueBegin, valueEnd - valueBegin)};
            return true;
        }
        bool quoted = false;
        cursor = begin;
        while (cursor < commandLine.size()) {
            const wchar_t character = commandLine[cursor];
            if (!quoted && (character == L' ' || character == L'\t')) break;
            if (character == L'"') quoted = !quoted;
            ++cursor;
        }
    }
    cursor = commandLine.size();
    return false;
}

bool FindNextAutoLoginSessionIndex(std::uint64_t& groupIndex) noexcept
{
    bool found = false;
    std::size_t cursor = 0;
    AutoLoginArgument argument {};
    while (NextAutoLoginArgument(gCommandLine, cursor, argument)) {
        if (argument.group != 0 && (!found || argument.group < groupIndex)) {
            groupIndex = argument.group;
            found = true;
        }
    }
    return found;
}

// NSM01：各组独立沿A08发布；处理后即消费，不等待登录结果或向其他会话继承。
__declspec(noinline)
void ConsumeAutoLoginCommandLine(network::CgConnection& connection, const std::uint64_t groupIndex)
{
    std::array<std::wstring, 4> values;
    std::array<bool, 4> present {};
    std::size_t cursor = 0;
    AutoLoginArgument argument {};
    while (NextAutoLoginArgument(gCommandLine, cursor, argument)) {
        if (argument.group == groupIndex && !present[argument.field]) {
            values[argument.field].assign(argument.value);
            present[argument.field] = true;
        }
    }
    const auto& account = values[0];
    const auto& password = values[1];
    const auto& serverText = values[2];
    const auto& characterSlotText = values[3];
    std::int32_t serverIndex = -1;
    auto& request = connection.LoginRequests();
    VM_LIGHT_START

    if (!account.empty() && !password.empty() && !serverText.empty() && !characterSlotText.empty()) {
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
    for (auto& value : values) {
        SecureZeroMemory(value.data(), value.size() * sizeof(wchar_t));
    }
    cursor = 0;
    while (NextAutoLoginArgument(gCommandLine, cursor, argument)) {
        if (argument.group != groupIndex) continue;
        const std::size_t removed = argument.end - argument.begin;
        const std::size_t remaining = gCommandLine.size() - removed;
        // 先在原有效区内移动并清尾，再缩短string，避免向size外写入或留下旧密码尾部。
        std::char_traits<wchar_t>::move(gCommandLine.data() + argument.begin,
            gCommandLine.data() + argument.end, gCommandLine.size() - argument.end);
        SecureZeroMemory(gCommandLine.data() + remaining, removed * sizeof(wchar_t));
        gCommandLine.resize(remaining);
        cursor = argument.begin;
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
    ConsumeAutoLoginCommandLine(connection, 0);
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
