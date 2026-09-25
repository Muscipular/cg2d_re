#include "cg/Palette.hpp"

#include "cg/ResourcePaths.hpp"
#include "cg/platform/LegacyWindow.hpp"

#include <bit>
#include <cmath>
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
PaletteEntry gPaletteTarget[256] {};

// 原 [0x556254,0x5562DB)：45 项、每项连续 R/G/B 三字节。
std::uint8_t gPaletteToneTargets[45][3] {
    {128, 128, 128}, {90, 120, 150}, {128, 86, 86}, {128, 120, 120},
    {128, 160, 128}, {160, 160, 128}, {160, 128, 128}, {160, 128, 160},
    {128, 128, 160}, {96, 144, 160}, {128, 160, 160}, {128, 160, 144},
    {112, 112, 112}, {96, 96, 96}, {128, 192, 255}, {255, 255, 255},
    {128, 192, 255}, {128, 128, 128}, {128, 128, 128}, {0, 0, 0},
    {255, 255, 255}, {128, 128, 128}, {128, 128, 128}, {128, 128, 128},
    {128, 128, 128}, {128, 128, 128}, {128, 128, 128}, {128, 128, 128},
    {128, 128, 128}, {128, 128, 128}, {128, 128, 128}, {128, 128, 128},
    {128, 128, 128}, {128, 128, 128}, {128, 128, 128}, {128, 128, 128},
    {128, 128, 128}, {128, 128, 128}, {128, 160, 160}, {160, 128, 128},
    {128, 128, 160}, {128, 160, 128}, {128, 192, 255}, {255, 255, 255},
    {128, 128, 128},
};

PaletteTone gPaletteTone {128, 128, 128, 0};
PaletteTone* gPaletteTonePointer = &gPaletteTone;
std::int32_t gLoadedPalette = -1;
std::int32_t gPaletteTransitionActive = 0;
std::int32_t gPaletteTransitionFrame = 0;
float gPaletteBlueCurrent[256] {};
float gPaletteGreenCurrent[256] {};
float gPaletteRedCurrent[256] {};
float gPaletteBlueStep[256] {};
float gPaletteGreenStep[256] {};
float gPaletteRedStep[256] {};
float gPaletteToneBlueCurrent = 0.0f;
float gPaletteToneGreenCurrent = 0.0f;
float gPaletteToneRedCurrent = 0.0f;
float gPaletteToneBlueStep = 0.0f;
float gPaletteToneGreenStep = 0.0f;
float gPaletteToneRedStep = 0.0f;
std::int32_t gPaletteToneTargetIndex = 0;
std::int32_t gPaletteFrameVersion = 0;
std::int32_t gPaletteVersionThrottle = 0;

namespace {

std::uint8_t redMap[256] {};   // 原 0x628FFC，gBGR_Data.r。
std::uint8_t greenMap[256] {}; // 原 0x6290FC，gBGR_Data.g。
std::uint8_t blueMap[256] {};  // 原 0x629200，gBGR_Data.b。

std::int32_t Add32(std::int32_t value, std::uint32_t increment) noexcept
{
    return std::bit_cast<std::int32_t>(std::bit_cast<std::uint32_t>(value) + increment);
}

// 原 __ftol@0x53B498 向零截断为 int64，再只写 AL；不可用 uint8 浮点直转。
std::uint8_t TruncatedLowByte(double value) noexcept
{
    if (!std::isfinite(value) || value < -9223372036854775808.0
        || value >= 9223372036854775808.0) {
        return 0; // x87 FISTP 非法转换的整数不定值低字节为零。
    }
    return static_cast<std::uint8_t>(static_cast<std::int64_t>(value));
}

// 原 0x451084..0x45117E：按红、绿、蓝指针顺序分别建立 256 字节表。
int RebuildToneMap(
    std::uint8_t* ColorChannelMaps::* map, std::uint8_t PaletteTone::* component) noexcept
{
    if ((gPaletteTonePointer->*component) == 0x80) {
        for (int i = 0; i < 256; ++i) {
            (gColorChannelMaps.*map)[i] = static_cast<std::uint8_t>(i);
        }
        return 256;
    }

    int result = 0;
    for (int i = 0; i < 256; ++i) {
        result = (i * (gPaletteTonePointer->*component)) >> 7;
        if (result > 255) {
            result = 255;
        }
        (gColorChannelMaps.*map)[i] = static_cast<std::uint8_t>(result);
    }
    return result;
}

} // namespace

int CountMaskBits_453DE0(std::uint32_t mask) noexcept
{
    return std::popcount(mask);
}

ColorChannelMaps gColorChannelMaps {redMap, greenMap, blueMap};
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

int UpdatePalette_450BB0()
{
    const std::int32_t previousVersion = gPaletteFrameVersion;
    const std::int32_t previousActivePalette = gActivePalette;
    std::int32_t previousFrame = 0;
    std::int32_t toneTargetIndex;

    if (gLoadedPalette == previousActivePalette) {
        if (gPaletteTransitionActive == 0) {
            return 0; // 原 0x450BD6..0x450BDF 的 EAX。
        }
        previousFrame = gPaletteTransitionFrame;
        toneTargetIndex = gPaletteToneTargetIndex;
    } else {
        const std::int32_t paletteOffset = std::bit_cast<std::int32_t>(
            std::bit_cast<std::uint32_t>(previousActivePalette) * 130u);
        // 原端只比较门槛，不额外处理负索引。
        if (gPaletteFiles[previousActivePalette].threshold > gPaletteCapability) {
            return paletteOffset;
        }

        char path[512];
        std::sprintf(path, "%s\\%s", resource::gBinPath, gPaletteFiles[previousActivePalette].path);
        std::FILE* file = std::fopen(path, "rb");
        if (file == nullptr) {
            return 0; // 失败不更新已加载索引；下一帧继续尝试打开。
        }

        for (int i = 16; i < 240; ++i) {
            gPaletteTarget[i].blue = static_cast<std::uint8_t>(std::fgetc(file));
            gPaletteTarget[i].green = static_cast<std::uint8_t>(std::fgetc(file));
            gPaletteTarget[i].red = static_cast<std::uint8_t>(std::fgetc(file));
            if (i == 168) {
                gPaletteTarget[i].blue = 0;
                gPaletteTarget[i].green = 0;
                gPaletteTarget[i].red = 0;
            }

            gPaletteBlueCurrent[i] = static_cast<float>(gPalette[i].blue);
            gPaletteGreenCurrent[i] = static_cast<float>(gPalette[i].green);
            gPaletteRedCurrent[i] = static_cast<float>(gPalette[i].red);
            const std::int32_t blueDifference =
                static_cast<std::int32_t>(gPaletteTarget[i].blue) - gPalette[i].blue;
            const std::int32_t greenDifference =
                static_cast<std::int32_t>(gPaletteTarget[i].green) - gPalette[i].green;
            const std::int32_t redDifference =
                static_cast<std::int32_t>(gPaletteTarget[i].red) - gPalette[i].red;
            const double steps = static_cast<double>(gPaletteTransitionSteps);
            gPaletteBlueStep[i] = static_cast<float>(static_cast<double>(blueDifference) / steps);
            gPaletteGreenStep[i] = static_cast<float>(static_cast<double>(greenDifference) / steps);
            gPaletteRedStep[i] = static_cast<float>(static_cast<double>(redDifference) / steps);
        }
        std::fclose(file);

        const std::int32_t loadedActivePalette = gActivePalette;
        gPaletteTransitionActive = 1;
        gPaletteFrameVersion = 1;
        gPaletteVersionThrottle = 0;
        gLoadedPalette = loadedActivePalette;
        if (gPaletteTonePointer == &gPaletteTone) {
            toneTargetIndex = loadedActivePalette;
            gPaletteToneBlueCurrent = static_cast<float>(gPaletteTone.blue);
            gPaletteToneTargetIndex = toneTargetIndex;
            gPaletteToneGreenCurrent = static_cast<float>(gPaletteTone.green);
            gPaletteToneRedCurrent = static_cast<float>(gPaletteTone.red);
            const double steps = static_cast<double>(gPaletteTransitionSteps);
            gPaletteToneBlueStep = static_cast<float>(
                (static_cast<double>(gPaletteToneTargets[toneTargetIndex][2]) - gPaletteTone.blue)
                / steps);
            gPaletteToneGreenStep = static_cast<float>(
                (static_cast<double>(gPaletteToneTargets[toneTargetIndex][1]) - gPaletteTone.green)
                / steps);
            gPaletteToneRedStep = static_cast<float>(
                (static_cast<double>(gPaletteToneTargets[toneTargetIndex][0]) - gPaletteTone.red)
                / steps);
        } else {
            gPaletteToneRedCurrent = 0.0f;
            gPaletteToneGreenCurrent = 0.0f;
            gPaletteToneBlueCurrent = 0.0f;
            gPaletteToneRedStep = 0.0f;
            gPaletteToneGreenStep = 0.0f;
            gPaletteToneBlueStep = 0.0f;
            toneTargetIndex = gPaletteToneTargetIndex;
        }
    }

    gPaletteTransitionFrame = Add32(previousFrame, 1u);
    if (gPaletteTransitionFrame > gPaletteTransitionSteps) {
        for (int i = 16; i < 240; ++i) {
            gPalette[i].blue = gPaletteTarget[i].blue;
            gPalette[i].green = gPaletteTarget[i].green;
            gPalette[i].red = gPaletteTarget[i].red;
        }
        if (gPaletteTonePointer == &gPaletteTone) {
            gPaletteTone.blue = gPaletteToneTargets[toneTargetIndex][2];
            gPaletteTone.green = gPaletteToneTargets[toneTargetIndex][1];
            gPaletteTone.red = gPaletteToneTargets[toneTargetIndex][0];
        }
        gPaletteTransitionActive = 0;
        gPaletteChanging = 0;
        gPaletteFrameVersion = Add32(gPaletteFrameVersion, 10u);
    } else {
        for (int i = 16; i < 240; ++i) {
            // 蓝通道的原 __ftol 使用尚未舍入到 float 的 x87 加法值；绿/红使用已存 float。
            const double blue = static_cast<double>(gPaletteBlueStep[i])
                + static_cast<double>(gPaletteBlueCurrent[i]);
            gPaletteBlueCurrent[i] = static_cast<float>(blue);
            const float green = static_cast<float>(
                static_cast<double>(gPaletteGreenStep[i]) + gPaletteGreenCurrent[i]);
            gPaletteGreenCurrent[i] = green;
            const float red = static_cast<float>(
                static_cast<double>(gPaletteRedStep[i]) + gPaletteRedCurrent[i]);
            gPaletteRedCurrent[i] = red;
            gPalette[i].blue = TruncatedLowByte(blue);
            gPalette[i].green = TruncatedLowByte(green);
            gPalette[i].red = TruncatedLowByte(red);
        }
        if (gPaletteTonePointer == &gPaletteTone) {
            gPaletteToneBlueCurrent = static_cast<float>(
                static_cast<double>(gPaletteToneBlueStep) + gPaletteToneBlueCurrent);
            gPaletteToneGreenCurrent = static_cast<float>(
                static_cast<double>(gPaletteToneGreenStep) + gPaletteToneGreenCurrent);
            gPaletteToneRedCurrent = static_cast<float>(
                static_cast<double>(gPaletteToneRedStep) + gPaletteToneRedCurrent);
            gPaletteTone.blue = TruncatedLowByte(gPaletteToneBlueCurrent);
            gPaletteTone.green = TruncatedLowByte(gPaletteToneGreenCurrent);
            gPaletteTone.red = TruncatedLowByte(gPaletteToneRedCurrent);
        }
        gPaletteChanging = 1;
    }

    // 8/16/24 位渲染分支已按范围排除；保留已实现的 32 位 CPU 查色表路径。
    RebuildPalette32();
    gPaletteVersionThrottle = Add32(gPaletteVersionThrottle, 1u);
    if (gPaletteTransitionSteps <= 120) {
        if (gPaletteVersionThrottle >= 2) {
            gPaletteVersionThrottle = 0;
        }
        gPaletteFrameVersion = Add32(gPaletteFrameVersion, 1u);
    } else if (gPaletteVersionThrottle >= gPaletteTransitionSteps / 20) {
        gPaletteVersionThrottle = 0;
        gPaletteFrameVersion = Add32(gPaletteFrameVersion, 1u);
    }

    if (previousActivePalette == gActivePalette && previousVersion == gPaletteFrameVersion) {
        return previousVersion;
    }
    RebuildToneMap(&ColorChannelMaps::red, &PaletteTone::blue);
    RebuildToneMap(&ColorChannelMaps::green, &PaletteTone::green);
    return RebuildToneMap(&ColorChannelMaps::blue, &PaletteTone::red);
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
