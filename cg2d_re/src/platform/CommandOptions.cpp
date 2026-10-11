#include "cg/platform/CommandOptions.hpp"
#include "cg/platform/LegacyTime.hpp"

#include "cg/LoginScene.hpp"
#include "cg/ResourceNames.hpp"
#include "cg/ResourcePaths.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "StartupNative.hpp"
#include "LegacyFormat.hpp"

#include <bit>
#include <cstddef>
#include <cstdio>
#include <cstring>

namespace cg::platform {

char gNewestText[128] = "/update/newestV2.txt";
char gMapPath[128] = "map";
std::int32_t gDeviceBitsPerPixel = 8;
std::uint16_t gScanlineSyncMode = 1;
std::int32_t gNoDelay = 1;
std::uint32_t gRendererFlags = 0;
std::int32_t gEnableSound = 1;
std::int32_t gEnableMusic = 1;
std::int32_t gUseStereo = 1;
std::int32_t gEffectVolume = 15;
std::int32_t gMusicVolume = 15;
// 原连续零填充区：前缀、未命名间隔和 W32 运行表不得拆为独立存储。
struct BgmSelectionStorage final {
    std::uint8_t prefix[16];
    std::uint8_t gap[8];
    std::uint32_t runtimeWords[200];
};
static_assert(offsetof(BgmSelectionStorage, runtimeWords) == 0x18);
static_assert(sizeof(BgmSelectionStorage) == 0x338);
static BgmSelectionStorage gBgmSelection1050840 {};
std::uint8_t (&gBgmFrequencySelectionPrefix)[16] = gBgmSelection1050840.prefix;

std::int8_t ReadBgmFrequencySelectionByte(const std::uint8_t id) noexcept
{
    const auto* bytes =
        reinterpret_cast<const std::uint8_t*>(&gBgmSelection1050840);
    return std::bit_cast<std::int8_t>(bytes[id]);
}

std::uint32_t* BgmRuntimeOrderWords() noexcept
{
    return gBgmSelection1050840.runtimeWords;
}

std::int32_t gValueFF0D40 = 0;
std::int32_t gRendererModeStorage = 0;
std::uint8_t& gRendererMode = reinterpret_cast<std::uint8_t*>(&gRendererModeStorage)[0];

static_assert(sizeof(int) == 4);
static_assert(std::endian::native == std::endian::little);

// 原 ParseCmdOpt@0x465F00；证据、原有输入限制及未决存储见 doc/cg2d_command_options.md。
int ParseCommandOptions()
{
    char savedata[128] {};
    if (char* match = StrStr_53BE40(gCommandLine, "newest:")) {
        ScanLegacyString_53CADC(match + std::strlen("newest:"), "%s", gNewestText);
    }
    if (char* match = StrStr_53BE40(gCommandLine, "savedata:")) {
        ScanLegacyString_53CADC(match + std::strlen("savedata:"), "%s", savedata);
    }
    if (char* match = StrStr_53BE40(gCommandLine, "datafolder:")) {
        ScanLegacyString_53CADC(match + std::strlen("datafolder:"), "%s", gDataPath);
    }
    if (char* match = StrStr_53BE40(gCommandLine, "binfolder:")) {
        ScanLegacyString_53CADC(match + std::strlen("binfolder:"), "%s", resource::gBinPath);
    }
    if (char* match = StrStr_53BE40(gCommandLine, "mapfolder:")) {
        ScanLegacyString_53CADC(match + std::strlen("mapfolder:"), "%s", gMapPath);
    }

    // 原 0x46605C-0x46606B 不使用 savedata 或 gDataPath，固定拼接该路径。
    FormatLegacyString_53B304(gTextFF4E30, "data\\0\\%s", "save.dat");

    const char* const graphicKeys[9] = {
        "graphicbin:", "graphicbinex:", "graphicbinv2:",
        "graphicbinv3:", "graphicbin_puk2:", "graphicbin_puk3:",
        "graphicbin_joy:", "graphicbin_joy_ex:", "graphicbin_joy_ch1:"
    };
    const char* const graphicOverrideKeys[9] = {
        "graphicbin:", "graphicbinex:", "graphicbiv2x:",
        "graphicbinv3:", "graphicbin_puk2:", "graphicbin_puk3:",
        "graphicbin_joy:", "graphicbin_joy_ex:", "graphicbin_joy_ch1:"
    };
    const char* const graphicInfoKeys[9] = {
        "graphicinfobin:", "graphicinfobinex:", "graphicinfobinv2:",
        "graphicinfobinv3:", "graphicinfobin_puk2:", "graphicinfobin_puk3:",
        "graphicinfobin_joy:", "graphicinfobin_joy_ex:", "graphicinfobin_joy_ch1:"
    };
    const char* const graphicFormats[9] = {
        "graphic_%d.bin", "graphicex_%d.bin", "graphicv2_%d.bin",
        "graphicv3_%d.bin", "PUK2\\graphic_puk2_%d.bin", "PUK3\\graphic_puk3_%d.bin",
        "graphic_joy_%d.bin", "graphic_joy_ex_%d.bin", "graphic_joy_ch1_%d.bin"
    };
    const char* const graphicInfoFormats[9] = {
        "graphicInfo_%d.bin", "graphicinfoex_%d.bin", "graphicinfov2_%d.bin",
        "graphicinfov3_%d.bin", "PUK2\\graphicinfo_puk2_%d.bin", "PUK3\\graphicinfo_puk3_%d.bin",
        "graphicinfo_joy_%d.bin", "graphicinfo_joy_ex_%d.bin", "graphicinfo_joy_ch1_%d.bin"
    };
    const char* const animeKeys[9] = {
        "animebin:", "animebinex:", "animebinv2:",
        "animebinv3:", "animebin_puk2:", "animebin_puk3:",
        "animebin_joy:", "animebin_joy_ex:", "animebin_joy_ch1:"
    };
    const char* const animeInfoKeys[9] = {
        "animeinfobin:", "animeinfobinex:", "animeinfobinv2:",
        "animeinfobinv3:", "animeinfobin_puk2:", "animeinfobin_puk3:",
        "animeinfobin_joy:", "animebin_joy_ex:", "animebin_joy_ch1:"
    };
    const char* const animeFormats[9] = {
        "anime_%d.bin", "animeex_%d.bin", "animev2_%d.bin",
        "animev3_%d.bin", "PUK2\\anime_puk2_%d.bin", "PUK3\\anime_puk3_%d.bin",
        "anime_joy_%d.bin", "anime_joy_ex_%d.bin", "anime_joy_ch1_%d.bin"
    };
    const char* const animeInfoFormats[9] = {
        "animeinfo_%d.bin", "animeinfoex_%d.bin", "animeinfov2_%d.bin",
        "animeinfov3_%d.bin", "PUK2\\animeinfo_puk2_%d.bin", "PUK3\\animeinfo_puk3_%d.bin",
        "animeinfo_joy_%d.bin", "animeinfo_joy_ex_%d.bin", "animeinfo_joy_ch1_%d.bin"
    };
    const char* const coordinateKeys[9] = {
        "coordinatebin:", "coordinatebinex:", "coordinatebinv2:",
        "coordinatebinv3:", "coordinatebin_puk2:", "coordinatebin_puk3:",
        nullptr, nullptr, nullptr
    };
    const char* const coordinateInfoKeys[9] = {
        "coordinateinfobin:", "coordinateinfobinex:", "coordinateinfobinv2:",
        "coordinateinfobinv3:", "coordinateinfobin_puk2:", "coordinateinfobin_puk3:",
        nullptr, nullptr, nullptr
    };
    const char* const coordinateFormats[9] = {
        "coordinate_%d.bin", "coordinateex_%d.bin", "coordinatev2_%d.bin",
        "coordinatev3_%d.bin", "PUK2\\coordinate_puk2_%d.bin", "PUK3\\coordinate_puk3_%d.bin",
        nullptr, nullptr, nullptr
    };
    const char* const coordinateInfoFormats[9] = {
        "coordinateinfo_%d.bin", "coordinateinfoex_%d.bin", "coordinateinfov2_%d.bin",
        "coordinateinfov3_%d.bin", "PUK2\\coordinateinfo_puk2_%d.bin", "PUK3\\coordinateinfo_puk3_%d.bin",
        nullptr, nullptr, nullptr
    };

    // 原 var_260 是跨表、跨轮复用的未初始化 int；原函数不检查 sscanf 返回值。
    int value;
    for (int slot = 0; slot < 9; ++slot) {
        if (char* match = StrStr_53BE40(gCommandLine, graphicKeys[slot])) {
            ScanLegacyString_53CADC(match + std::strlen(graphicKeys[slot]), "%d", &value);
            FormatLegacyString_53B304(gResourceNames[GraphicNames][slot], graphicFormats[slot], value);
        }
        if (char* match = StrStr_53BE40(gCommandLine, graphicOverrideKeys[slot])) {
            ScanLegacyString_53CADC(match + std::strlen(graphicOverrideKeys[slot]), "%d", &value);
            FormatLegacyString_53B304(gResourceNames[GraphicNames][slot], graphicFormats[slot], value);
        }
        if (char* match = StrStr_53BE40(gCommandLine, graphicInfoKeys[slot])) {
            ScanLegacyString_53CADC(match + std::strlen(graphicInfoKeys[slot]), "%d", &value);
            FormatLegacyString_53B304(gResourceNames[GraphicInfoNames][slot], graphicInfoFormats[slot], value);
        }
        if (char* match = StrStr_53BE40(gCommandLine, animeKeys[slot])) {
            ScanLegacyString_53CADC(match + std::strlen(animeKeys[slot]), "%d", &value);
            FormatLegacyString_53B304(gResourceNames[AnimeNames][slot], animeFormats[slot], value);
        }
        if (char* match = StrStr_53BE40(gCommandLine, animeInfoKeys[slot])) {
            ScanLegacyString_53CADC(match + std::strlen(animeInfoKeys[slot]), "%d", &value);
            FormatLegacyString_53B304(gResourceNames[AnimeInfoNames][slot], animeInfoFormats[slot], value);
        }
    }

    for (int slot = 0; slot < 9; ++slot) {
        if (!StrChr_53C2F0(gResourceNames[CoordinateNames][slot], '*')) {
            if (char* match = StrStr_53BE40(gCommandLine, coordinateKeys[slot])) {
                ScanLegacyString_53CADC(match + std::strlen(coordinateKeys[slot]), "%d", &value);
                FormatLegacyString_53B304(gResourceNames[CoordinateNames][slot], coordinateFormats[slot], value);
            }
        }
        if (!StrChr_53C2F0(gResourceNames[CoordinateInfoNames][slot], '*')) {
            if (char* match = StrStr_53BE40(gCommandLine, coordinateInfoKeys[slot])) {
                ScanLegacyString_53CADC(match + std::strlen(coordinateInfoKeys[slot]), "%d", &value);
                FormatLegacyString_53B304(gResourceNames[CoordinateInfoNames][slot], coordinateInfoFormats[slot], value);
            }
        }
    }

    // 原 0x466791-0x466841：单遍逐槽，先数据后 Info，不递归解析别名。
    for (int slot = 0; slot < 9; ++slot) {
        for (int table = CoordinateNames; table <= CoordinateInfoNames; ++table) {
            char* destination = gResourceNames[table][slot];
            if (StrChr_53C2F0(destination, '*')) {
                ScanLegacyString_53CADC(destination, "*%d", &value);
                const char* source = gResourceNames[table][value];
                const std::size_t length = std::strlen(source) + 1;

                // 对应先量长、再正向 rep movsd/movsb；临时字保留自引用时的原拷贝行为。
                std::size_t offset = 0;
                for (; offset + sizeof(std::uint32_t) <= length; offset += sizeof(std::uint32_t)) {
                    std::uint32_t word;
                    std::memcpy(&word, source + offset, sizeof(word));
                    std::memcpy(destination + offset, &word, sizeof(word));
                }
                for (; offset < length; ++offset) {
                    destination[offset] = source[offset];
                }
            }
        }
    }

    if (StrStr_53BE40(gCommandLine, "windowmode")) {
        gWindowMode = 1;
    }
    HDC deviceContext = GetDC(reinterpret_cast<HWND>(gLegacyWindow));
    gDeviceBitsPerPixel = GetDeviceCaps(deviceContext, BITSPIXEL);
    ReleaseDC(reinterpret_cast<HWND>(gLegacyWindow), deviceContext);

    if (StrStr_53BE40(gCommandLine, "nodelay")) {
        gNoDelay = 0;
    }
    if (char* match = StrStr_53BE40(gCommandLine, "winpos:")) {
        int x;
        int y;
        ScanLegacyString_53CADC(match + std::strlen("winpos:"), "%d,%d", &x, &y);
        if (x >= 0 && y >= 0) {
            gStartX = x;
            gStartY = y;
        }
    }

    gRendererFlags = 0;
    gRendererMode = 1;
    if (char* match = StrStr_53BE40(gCommandLine, "3Ddevice:")) {
        ScanLegacyString_53CADC(match + std::strlen("3Ddevice:"), "%d", &gRendererModeStorage);
    }
    if (StrStr_53BE40(gCommandLine, "soundoff")) {
        gEnableSound = 0;
    }
    if (StrStr_53BE40(gCommandLine, "musicoff")) {
        gEnableMusic = 0;
    }
    if (gRequestedRenderMode) {
        gRendererMode = gRequestedRenderMode;
    }
    if (StrStr_53BE40(gCommandLine, "onlyactive")) {
        gRendererFlags |= 0x10;
    }
    gValueFF0D40 = 1;
    return ParseIpOptions_507C80(gCommandLine);
}

// 原函数：0x00507C80。
int ParseIpOptions_507C80(const char* const commandLine)
{
    std::memset(
        login::gValueD69798.data(),
        0,
        sizeof(login::gValueD69798));
    login::gValueD69794 = 0;

    // 原 sscanf 的目标是入参栈槽；未匹配时保留其上一次 W32 内容。
    int index = std::bit_cast<int>(
        static_cast<std::uint32_t>(
            reinterpret_cast<std::uintptr_t>(commandLine)));
    const char* cursor = commandLine;
    while (const char* const match = StrStr_53BE40(cursor, "IP:")) {
        const char* const number = match + 3;
        const int parsed = ScanLegacyString_53CADC(number, "%d:", &index);
        if (static_cast<std::uint32_t>(index)
            >= login::gValueD69798.size()) {
            return parsed;
        }

        const char* address = number
            + (index < 10 ? 2 : index <= 99 ? 3 : 4);
        auto& record = login::gValueD69798[static_cast<std::size_t>(index)];
        std::size_t addressLength = 0;
        while (*address != '\0'
            && *address != ':'
            && addressLength < 0x7F) {
            record.opaque[addressLength++] =
                static_cast<std::uint8_t>(*address++);
        }
        record.opaque[addressLength] = 0;

        const char* port = address + 1;
        std::size_t portLength = 0;
        while (*port != '\0'
            && *port != ' '
            && *port != '\t'
            && portLength < 0x3F) {
            record.opaque[0x80 + portLength++] =
                static_cast<std::uint8_t>(*port++);
        }
        record.opaque[0x80 + portLength] = 0;
        record.available = 1;
        ++login::gValueD9F2D4[static_cast<std::size_t>(index / 20)];
        cursor = port;
    }
    return 0;
}

} // namespace cg::platform
