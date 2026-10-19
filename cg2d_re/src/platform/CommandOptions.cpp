#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/platform/CommandOptions.hpp"

#include "cg/scene/login/LoginScene.hpp"
#include "cg/scene/login/LoginInput.hpp"
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
#include <cstring> // 原CRT接口：0x0053BE40、0x0053C2F0；原参数／返回合同由直接调用承接。
#include <string>

namespace cg::platform {

// char gNewestText[128] = "/update/newestV2.txt"; // 原全局：0x0058ADA8。
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

std::int32_t gCurrentSceneTypeStorage_FF0D40 = 0; // 原全局：0x00FF0D40。
std::int32_t gRendererModeStorage = 0; // 原全局：0x0062A640。
std::uint8_t& gRendererMode = reinterpret_cast<std::uint8_t*>(&gRendererModeStorage)[0]; // 原全局：0x0062A640。

static_assert(sizeof(int) == 4);
static_assert(std::endian::native == std::endian::little);

// ALM01：启动参数生产一次选人标记；凭据仍由 login 的既有变量持有。
static bool gAutoLoginPending = false;
static std::int32_t gAutoLoginCharacterSlot = -1;

// A08：保留首次关键字、空白及引号边界，不改变原参数解析顺序。
static bool ParseCommandArgumentValue(
    const char* commandLine, const char* key, std::string& outValue)
{
    if (commandLine == nullptr || key == nullptr) {
        return false;
    }

    const char* p = std::strstr(commandLine, key);
    if (p == nullptr) {
        return false;
    }
    p += std::strlen(key);

    while (*p == ' ' || *p == '\t') {
        ++p;
    }
    if (*p == '\0') {
        return false;
    }

    const char* start = p;
    const char* end = nullptr;
    if (*p == '"') {
        start = p + 1;
        end = std::strchr(start, '"');
        if (end == nullptr) {
            end = start + std::strlen(start);
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
static void ApplyAutoLoginCommandLine(const char* commandLine)
{
    std::string account;
    std::string password;
    std::string serverText;
    std::string characterSlotText;
    std::int32_t serverIndex = -1;

    if (ParseCommandArgumentValue(commandLine, "/uu:", account)
        && ParseCommandArgumentValue(commandLine, "/pw:", password)
        && ParseCommandArgumentValue(commandLine, "/sv:", serverText)
        && ParseCommandArgumentValue(commandLine, "/cv:", characterSlotText)) {
        char* endPtr = nullptr;
        long value = std::strtol(serverText.c_str(), &endPtr, 10);
        serverIndex = (endPtr != nullptr && *endPtr == '\0')
            ? static_cast<std::int32_t>(value) : -1;
        endPtr = nullptr;
        value = std::strtol(characterSlotText.c_str(), &endPtr, 10);
        gAutoLoginCharacterSlot = (endPtr != nullptr && *endPtr == '\0')
            ? static_cast<std::int32_t>(value) : -1;
        if (gAutoLoginCharacterSlot > 1) {
            gAutoLoginCharacterSlot = -1;
        }
    }

    if (serverIndex >= 0 && !account.empty() && !password.empty()
        && account.size() < login::LoginInputValueCapacity
        && password.size() < login::LoginInputValueCapacity) {
        login::detail::CacheAutoLoginInputValues(text::FromGbkWide(account), text::FromGbkWide(password));
        world::gCurrentSceneState = 2;
        world::gCurrentSceneType = 2;
        login::gPassword = text::FromGbkWide(password);
        login::gAccount = text::FromGbkWide(account);
        login::gSelectedServerIndex = std::bit_cast<std::int16_t>(
            static_cast<std::uint16_t>(serverIndex));
        gAutoLoginPending = true;
    }
}

bool ConsumeAutoLoginCharacterSlot(const std::uint32_t slot) noexcept
{
    if (gAutoLoginPending && gAutoLoginCharacterSlot >= 0
        && slot == static_cast<std::uint32_t>(gAutoLoginCharacterSlot)) {
        gAutoLoginPending = false;
        return true;
    }
    return false;
}

// 原 ParseCmdOpt@0x465F00；原参数见 doc/cg2d_command_options.md，A08扩展见ALM01 SPEC。
int ParseCommandOptions()
{
    char savedata[128] {};
    // if (char* match = const_cast<char*>(std::strstr(gCommandLine, "newest:"))) {
    //     std::sscanf(match + std::strlen("newest:"), "%s", gNewestText);
    // }
    if (char* match = const_cast<char*>(std::strstr(gCommandLine, "savedata:"))) {
        std::sscanf(match + std::strlen("savedata:"), "%s", savedata);
    }
    if (char* match = const_cast<char*>(std::strstr(gCommandLine, "datafolder:"))) {
        char value[128]{}; // 旧命令行解析边界，存储转为宽路径。
        if (std::sscanf(match + std::strlen("datafolder:"), "%s", value) == 1) gDataPath = text::FromGbkWide(value);
    }
    if (char* match = const_cast<char*>(std::strstr(gCommandLine, "binfolder:"))) {
        char value[128]{};
        if (std::sscanf(match + std::strlen("binfolder:"), "%127s", value) == 1) resource::gBinPath = text::FromGbkWide(value);
    }
    if (char* match = const_cast<char*>(std::strstr(gCommandLine, "mapfolder:"))) {
        char value[128]{}; // 旧命令行解析边界，存储转为宽路径。
        if (std::sscanf(match + std::strlen("mapfolder:"), "%s", value) == 1) gMapPath = text::FromGbkWide(value);
    }

    // 原 0x46605C-0x46606B 不使用 savedata 或 gDataPath，固定拼接该路径。
    std::sprintf(cg::render::gLegacyPendingStorage.prefix, "data\\0\\%s", "save.dat");

    if (const_cast<char*>(std::strstr(gCommandLine, "windowmode"))) {
        gWindowMode = 1;
    }
    HDC deviceContext = GetDC(reinterpret_cast<HWND>(gLegacyWindow));
    gDeviceBitsPerPixel = GetDeviceCaps(deviceContext, BITSPIXEL);
    ReleaseDC(reinterpret_cast<HWND>(gLegacyWindow), deviceContext);

    if (const_cast<char*>(std::strstr(gCommandLine, "nodelay"))) {
        gNoDelay = 0;
    }
    if (char* match = const_cast<char*>(std::strstr(gCommandLine, "winpos:"))) {
        int x;
        int y;
        std::sscanf(match + std::strlen("winpos:"), "%d,%d", &x, &y);
        if (x >= 0 && y >= 0) {
            gStartX = x;
            gStartY = y;
        }
    }

    gRendererFlags = 0;
    gRendererMode = 1;
    if (char* match = const_cast<char*>(std::strstr(gCommandLine, "3Ddevice:"))) {
        std::sscanf(match + std::strlen("3Ddevice:"), "%d", &gRendererModeStorage);
    }
    if (const_cast<char*>(std::strstr(gCommandLine, "soundoff"))) {
        gEnableSound = 0;
    }
    if (const_cast<char*>(std::strstr(gCommandLine, "musicoff"))) {
        gEnableMusic = 0;
    }
    if (gRequestedRenderMode) {
        gRendererMode = gRequestedRenderMode;
    }
    if (const_cast<char*>(std::strstr(gCommandLine, "onlyactive"))) {
        gRendererFlags |= 0x10;
    }
    gCurrentSceneTypeStorage_FF0D40 = 1;
    const int result = ParseIpOptions_507C80(gCommandLine);
    ApplyAutoLoginCommandLine(::GetCommandLineA());
    return result;
}

// 原函数：0x00507C80。
int ParseIpOptions_507C80(const char* const commandLine)
{
    std::memset(
        login::gLoginServerRecords_D69798.data(),
        0,
        sizeof(login::gLoginServerRecords_D69798));

    // 原 sscanf 的目标是入参栈槽；未匹配时保留其上一次 W32 内容。
    int index = std::bit_cast<int>(
        static_cast<std::uint32_t>(
            reinterpret_cast<std::uintptr_t>(commandLine)));
    const char* cursor = commandLine;
    while (const char* const match = const_cast<char*>(std::strstr(cursor, "IP:"))) {
        const char* const number = match + 3;
        const int parsed = std::sscanf(number, "%d:", &index);
        if (static_cast<std::uint32_t>(index)
            >= login::gLoginServerRecords_D69798.size()) {
            return parsed;
        }

        const char* address = number
            + (index < 10 ? 2 : index <= 99 ? 3 : 4);
        auto& record = login::gLoginServerRecords_D69798[static_cast<std::size_t>(index)];
        std::size_t addressLength = 0;
        while (*address != '\0'
            && *address != ':'
            && addressLength < 0x7F) {
            record.address[addressLength++] = *address++;
        }
        record.address[addressLength] = 0;

        const char* port = address + 1;
        std::size_t portLength = 0;
        while (*port != '\0'
            && *port != ' '
            && *port != '\t'
            && portLength < 0x3F) {
            record.portText[portLength++] = *port++;
        }
        record.portText[portLength] = 0;
        record.available = 1;
        ++login::gLoginServerGroupCounts_D9F2D4[static_cast<std::size_t>(index / 20)];
        cursor = port;
    }
    return 0;
}

} // namespace cg::platform
