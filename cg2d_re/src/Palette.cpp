#include "cg/Palette.hpp"

#include "cg/ResourcePaths.hpp"
#include "cg/platform/LegacyWindow.hpp"

#include <cstdio>
#include <cstring>

namespace cg::render {

PaletteFile gPaletteFiles[45] {
    {0, "pal\\palet_00.cgp"}, {0, "pal\\palet_01.cgp"},
    {0, "pal\\palet_02.cgp"}, {0, "pal\\palet_03.cgp"},
    {0, "pal\\palet_04.cgp"}, {0, "pal\\palet_05.cgp"},
    {0, "pal\\palet_06.cgp"}, {0, "pal\\palet_07.cgp"},
    {0, "pal\\palet_08.cgp"}, {0, "pal\\palet_09.cgp"},
    {0, "pal\\palet_10.cgp"}, {0, "pal\\palet_11.cgp"},
    {0, "pal\\palet_12.cgp"}, {0, "pal\\palet_13.cgp"},
    {0, "pal\\palet_14.cgp"}, {0, "pal\\palet_15.cgp"},
    {0, "pal\\palet_opA.cgp"}, {0, "pal\\palet_opB.cgp"},
    {0, "pal\\palet_opC.cgp"}, {0, "pal\\palet_black.cgp"},
    {0, "pal\\palet_white.cgp"}, {0, "pal\\palet_logo00.cgp"},
    {0, "pal\\palet_logo01.cgp"}, {0, "pal\\palet_logo02J.cgp"},
    {0, "pal\\palet_idpass.cgp"}, {0, "pal\\palet_opDJ.cgp"},
    {1, "pal\\palet_logo02.cgp"}, {1, "pal\\palet_opAex.cgp"},
    {1, "pal\\palet_opBex.cgp"}, {1, "pal\\palet_ld2.cgp"},
    {2, "pal\\palet_ssv2.cgp"}, {1, "pal\\palet_tiex.cgp"},
    {2, "pal\\palet_tiv2_1.cgp"}, {2, "pal\\palet_tiv2_2.cgp"},
    {0, "pal\\palet_e_s.cgp"}, {2, "pal\\palet_logo02cn.cgp"},
    {2, "pal\\palet_logo36cn.cgp"}, {2, "pal\\palet_logo37cn.cgp"},
    {0, "pal\\palet_10.cgp"}, {0, "pal\\palet_06.cgp"},
    {0, "pal\\palet_08.cgp"}, {0, "pal\\palet_04.cgp"},
    {0, "pal\\palet_14.cgp"}, {0, "pal\\palet_15.cgp"},
    {0, "pal\\palet_e_s.cgp"},
};

PaletteEntry gPalette[256] {};
PaletteEntry gPaletteBackup[256] {};
std::uint32_t gPaletteAutoMap[256] {};

namespace {

std::uint8_t blueMap[256] {};  // 原 0x628FFC。
std::uint8_t greenMap[256] {}; // 原 0x6290FC。
std::uint8_t redMap[256] {};   // 原 0x629200。

} // namespace

ColorChannelMaps gColorChannelMaps {blueMap, greenMap, redMap};
std::int32_t gPaletteInitialized = 0;
std::int32_t gActivePalette = 0;
std::int32_t gPaletteTransitionSteps = 0;
std::int32_t gPaletteCapability = 0;
std::int32_t gPaletteChanging = 0;

int InitPalette32()
{
    // 原 0x450479..0x4506DA 写入的 32 项局部颜色，不沿用 cg3a 的固定色表。
    const PaletteEntry fixed[32] {
        {0, 0, 0, 5},       {128, 0, 0, 5},     {0, 128, 0, 5},     {128, 128, 0, 5},
        {0, 0, 128, 5},     {128, 0, 128, 5},   {0, 128, 128, 5},   {192, 192, 192, 5},
        {192, 220, 192, 5}, {166, 202, 240, 5}, {222, 0, 0, 5},     {255, 95, 0, 5},
        {255, 255, 160, 5}, {0, 95, 210, 5},    {80, 210, 255, 5},  {40, 225, 40, 5},
        {245, 195, 150, 5}, {225, 160, 95, 5},  {195, 125, 70, 5},  {155, 85, 30, 5},
        {70, 65, 55, 5},    {40, 35, 30, 5},    {255, 251, 240, 5}, {160, 160, 164, 5},
        {128, 128, 128, 5}, {255, 0, 0, 5},     {0, 255, 0, 5},     {255, 255, 0, 5},
        {0, 0, 255, 5},     {255, 0, 255, 5},   {0, 255, 255, 5},   {255, 255, 255, 5},
    };

    for (int i = 0; i < 10; ++i) {
        gPalette[i].blue = fixed[i].blue;
        gPalette[i].green = fixed[i].green;
        gPalette[i].red = fixed[i].red;
        gPalette[i].flags = 2;
        gPalette[i + 246].blue = fixed[i + 22].blue;
        gPalette[i + 246].green = fixed[i + 22].green;
        gPalette[i + 246].red = fixed[i + 22].red;
        gPalette[i + 246].flags = 2;
    }
    for (int i = 0; i < 6; ++i) {
        gPalette[i + 10].blue = fixed[i + 10].blue;
        gPalette[i + 10].green = fixed[i + 10].green;
        gPalette[i + 10].red = fixed[i + 10].red;
        gPalette[i + 10].flags = 5;
        gPalette[i + 240].blue = fixed[i + 16].blue;
        gPalette[i + 240].green = fixed[i + 16].green;
        gPalette[i + 240].red = fixed[i + 16].red;
        gPalette[i + 240].flags = 5;
    }

    if (gPaletteInitialized == 0) {
        char path[512];
        std::sprintf(path, "%s\\%s", resource::gBinPath, gPaletteFiles[0].path);
        std::FILE* file = std::fopen(path, "rb");
        if (file == nullptr) {
            // 原 0x556530 字节，保持 MessageBoxA 的旧编码边界。
            platform::ShowLegacyMessage(
                "\xBD\xD5\xA6\xE2\xBD\x4C\xC0\xC9\xAE\xD7\xB6\x7D\xB1\xD2\xA5\xA2\xB1\xD1",
                "Error Code = 13", 0x10);
            return 0;
        }
        for (int i = 16; i < 240; ++i) {
            gPalette[i].blue = static_cast<std::uint8_t>(std::fgetc(file));
            gPalette[i].green = static_cast<std::uint8_t>(std::fgetc(file));
            gPalette[i].red = static_cast<std::uint8_t>(std::fgetc(file));
            gPalette[i].flags = 5;
        }
        std::fclose(file);
        std::memcpy(gPaletteBackup, gPalette, sizeof(gPaletteBackup));
    }

    gPalette[168].blue = 0;
    gPalette[168].green = 0;
    gPalette[168].red = 0;

    // SDL 的 32 位像素不创建 DX7 调色板对象；保留原内联颜色转换及更新次序。
    for (int i = 0; i < 256; ++i) {
        const std::uint32_t color = gPalette[i].blue
            + ((gPalette[i].green + (static_cast<std::uint32_t>(gPalette[i].red) << 8)) << 8);
        gPaletteAutoMap[i] = color;
        if (color == 0) {
            gPaletteAutoMap[i] = 1;
        }
    }
    gPaletteAutoMap[0] = 0;

    if (gPaletteInitialized == 0) {
        for (int i = 0; i < 256; ++i) {
            gColorChannelMaps.blue[i] = static_cast<std::uint8_t>(i);
            gColorChannelMaps.green[i] = static_cast<std::uint8_t>(i);
            gColorChannelMaps.red[i] = static_cast<std::uint8_t>(i);
        }
    }
    gPaletteInitialized = 1;
    gPaletteChanging = 0;
    return 1;
}

int ReadPalette(std::int32_t index, PaletteEntry* output)
{
    char path[256];
    std::sprintf(path, "%s\\%s", resource::gBinPath, gPaletteFiles[index].path);
    std::FILE* file = std::fopen(path, "rb");
    if (file == nullptr) {
        platform::ShowLegacyMessage(
            "\xB5\x4C\xAA\x6B\xA5\xB4\xB6\x7D\xA6\xE2\xBD\x4C\xC0\xC9\xAE\xD7\xA1\x43",
            "Error", 0x10);
        return 0;
    }
    for (int i = 16; i < 240; ++i) {
        output[i].blue = static_cast<std::uint8_t>(std::fgetc(file));
        output[i].green = static_cast<std::uint8_t>(std::fgetc(file));
        output[i].red = static_cast<std::uint8_t>(std::fgetc(file));
        output[i].flags = 5;
    }
    std::fclose(file);
    for (int i = 0; i < 16; ++i) {
        output[i] = gPalette[i];
    }
    for (int i = 240; i < 256; ++i) {
        output[i] = gPalette[i];
    }
    return 1;
}

std::int32_t SelectPalette(std::int32_t index, std::int32_t steps) noexcept
{
    if (index >= 39 || gPaletteFiles[index].threshold > gPaletteCapability) {
        return index;
    }
    gActivePalette = index;
    gPaletteTransitionSteps = steps;
    if (steps <= 0) {
        gPaletteTransitionSteps = 1;
    }
    return steps;
}

int RebuildPalette32() noexcept
{
    for (int i = 0; i < 256; ++i) {
        const std::uint32_t color = gPalette[i].blue
            + ((gPalette[i].green + (static_cast<std::uint32_t>(gPalette[i].red) << 8)) << 8);
        gPaletteAutoMap[i] = color;
        if (color == 0) {
            gPaletteAutoMap[i] = 1;
        }
    }
    gPaletteAutoMap[0] = 0;
    return 0x400;
}

} // namespace cg::render
