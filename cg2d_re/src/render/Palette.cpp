#include "cg/render/Palette.hpp"

#include "../platform/LegacyFormat.hpp"
#include "../platform/LegacyStdio.hpp"

#include "cg/resource/ResourcePaths.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/render/RendererResources.hpp"

#include <bit>
#include <cmath>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053C8B3；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstring>

namespace cg::render {

// 原函数：0x00451E90。
std::int32_t FindNearestPaletteColor_451E90(
    const std::uint32_t color, const PaletteEntry* const palette, const std::int32_t count)
{
    const std::int32_t initialBlue = palette[0].blue;
    const std::int32_t initialGreen = palette[0].green;
    const std::int32_t red = static_cast<std::uint8_t>(color);
    const std::int32_t green = static_cast<std::uint8_t>(color >> 8);
    const std::int32_t initialRed = palette[0].red;
    const std::int32_t blue = static_cast<std::uint8_t>(color >> 16);
    const std::int32_t initialDeltaRed = initialRed - red;
    const std::int32_t initialDeltaBlue = initialBlue - blue;
    const std::int32_t initialRedSquared = initialDeltaRed * initialDeltaRed;
    const std::int32_t initialBlueSquared = initialDeltaBlue * initialDeltaBlue;
    const std::int32_t initialDeltaGreen = initialGreen - green;
    const std::int32_t initialRedBlue = initialRedSquared + initialBlueSquared;
    const std::int32_t initialDistance =
        initialRedBlue + initialDeltaGreen * initialDeltaGreen;
    const std::int32_t limit = count - 16;
    double closest = initialDistance;
    std::int32_t selected = 0;
    for (std::int32_t index = 16; index < limit; ++index) {
        const std::int32_t currentBlue = palette[index].blue;
        const std::int32_t currentGreen = palette[index].green;
        const std::int32_t currentRed = palette[index].red;
        const std::int32_t deltaGreen = currentGreen - green;
        const std::int32_t deltaBlue = currentBlue - blue;
        const std::int32_t deltaRed = currentRed - red;
        const std::int32_t redSquared = deltaRed * deltaRed;
        const std::int32_t greenSquared = deltaGreen * deltaGreen;
        const std::int32_t redGreen = redSquared + greenSquared;
        const std::int32_t distance = redGreen + deltaBlue * deltaBlue;
        const double current = distance;
        if (current < closest) {
            closest = current;
            selected = index;
        }
    }
    return selected;
}

PaletteFile gPaletteFiles[45] { // 原全局：0x00554B78。
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

PaletteEntry gPalette[256] {}; // 原全局：0x00629B20。
PaletteEntry gPaletteBackup[256] {}; // 原全局：0x00629F2C。
std::uint32_t gPaletteAutoMap[256] {}; // 原全局：0x00628BC0。
PaletteEntry gPaletteTarget[256] {}; // 原全局：0x00626C60。

// 原 [0x556254,0x5562DB)：45 项、每项连续 R/G/B 三字节。
std::uint8_t gPaletteToneTargets[45][3] { // 原全局：0x00556254。
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

PaletteTone gPaletteTone {128, 128, 128, 0}; // 原全局：0x005562E0。
PaletteTone* gPaletteTonePointer = &gPaletteTone; // 原全局：0x005562E4。
std::int32_t gLoadedPalette = -1; // 原全局：0x005562F4。
std::int32_t gPaletteTransitionActive = 0; // 原全局：0x0062B0A0。
std::int32_t gPaletteTransitionFrame = 0; // 原全局：0x0062B09C。
float gPaletteBlueCurrent[256] {}; // 原全局：0x0062A664。
float gPaletteGreenCurrent[256] {}; // 原全局：0x00629720。
float gPaletteRedCurrent[256] {}; // 原全局：0x00629318。
float gPaletteBlueStep[256] {}; // 原全局：0x006287B4。
float gPaletteGreenStep[256] {}; // 原全局：0x006283A8。
float gPaletteRedStep[256] {}; // 原全局：0x0062AA64。
float gPaletteToneBlueCurrent = 0.0f; // 原全局：0x00629F28。
float gPaletteToneGreenCurrent = 0.0f; // 原全局：0x00629F24。
float gPaletteToneRedCurrent = 0.0f; // 原全局：0x00629F20。
float gPaletteToneBlueStep = 0.0f; // 原全局：0x00628FF8。
float gPaletteToneGreenStep = 0.0f; // 原全局：0x00628FF4。
float gPaletteToneRedStep = 0.0f; // 原全局：0x00628FF0。
std::int32_t gPaletteToneTargetIndex = 0; // 原全局：0x00628BBC。
std::int32_t gPaletteFrameVersion = 0; // 原全局：0x00628FDC。
std::int32_t gPaletteVersionThrottle = 0; // 原全局：0x00629310。

namespace {

std::uint8_t redMap[256] {};   // 原 0x628FFC，gBGR_Data.r。
std::uint8_t greenMap[256] {}; // 原 0x6290FC，gBGR_Data.g。
std::uint8_t blueMap[256] {};  // 原 0x629200，gBGR_Data.b。

// 原 __ftol@0x53B498 向零截断为 int64，再只写 AL；不可用 uint8 浮点直转。
std::uint8_t TruncatedLowByte(double value) noexcept
{
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

// 原函数：0x00453DE0。
int CountMaskBits_453DE0(std::uint32_t mask) noexcept
{
    return std::popcount(mask);
}

ColorChannelMaps gColorChannelMaps {blueMap, greenMap, redMap}; // 原全局：0x005562E8。
std::int32_t gPaletteInitialized = 0; // 原全局：0x00628FD8。
std::int32_t gActivePalette = 0; // 原全局：0x00628FD0。
std::int32_t gPaletteTransitionSteps = 0; // 原全局：0x00628FD4。
std::int32_t gPaletteCapability = 0; // 原全局：0x009A7F64。
std::int32_t gPaletteChanging = 0; // 原全局：0x00628FEC。

// 原函数：0x00450470。
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
        std::sprintf(
            path, "%s\\%s", resource::gBinPath, gPaletteFiles[0].path);
        platform::LegacyFile* file =
            std::fopen(path, "rb");
        if (file == nullptr) {
            // 原 0x556530 字节，保持 MessageBoxA 的旧编码边界。
            platform::ShowWideMessage(L"調色盤檔案開啟失敗", L"Error Code = 13", 0x10);
            return 0;
        }
        for (int i = 16; i < 240; ++i) {
            gPalette[i].blue = static_cast<std::uint8_t>(
                std::fgetc(file));
            gPalette[i].green = static_cast<std::uint8_t>(
                std::fgetc(file));
            gPalette[i].red = static_cast<std::uint8_t>(
                std::fgetc(file));
            gPalette[i].flags = 5;
        }
        std::fclose(file);
        std::memcpy(gPaletteBackup, gPalette, sizeof(gPaletteBackup));
    }

    gPalette[168].blue = 0;
    gPalette[168].green = 0;
    gPalette[168].red = 0;

    if (!CreateRendererPaletteBinding(gPalette)) {
        platform::ShowWideMessage(L"建立調色盤失敗", L"Error Code = 15", 0x10);
        return 0;
    }

    // 原 32 位路径在创建独立调色板对象后建立 CPU 查色表。
    for (int i = 0; i < 256; ++i) {
        const std::uint32_t red = gPalette[i].red;
        const std::uint32_t green = gPalette[i].green;
        const std::uint32_t redGreen = (red << 8) + green;
        const std::uint32_t blue = gPalette[i].blue;
        const std::uint32_t color = (redGreen << 8) + blue;
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

// 原函数：0x00450A80。
int ReadPalette(std::int32_t index, PaletteEntry* output)
{
    char path[256];
    std::sprintf(
        path, "%s\\%s", resource::gBinPath, gPaletteFiles[index].path);
    platform::LegacyFile* file =
        std::fopen(path, "rb");
    if (file == nullptr) {
        platform::ShowWideMessage(L"無法打開色盤檔案。", L"Error", 0x10);
        return 0;
    }
    for (int i = 16; i < 240; ++i) {
        output[i].blue = static_cast<std::uint8_t>(
            std::fgetc(file));
        output[i].green = static_cast<std::uint8_t>(
            std::fgetc(file));
        output[i].red = static_cast<std::uint8_t>(
            std::fgetc(file));
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

// PCR01：原450B70完整16条；S16门槛/39上界，steps先局部判非正再发布，原返回保持。
std::int32_t SelectPalette(std::int32_t index, std::int32_t steps) noexcept
{
    if (index >= 39 || gPaletteFiles[index].threshold > gPaletteCapability) {
        return index;
    }
    gActivePalette = index;
    const bool nonPositiveSteps = steps <= 0;
    gPaletteTransitionSteps = steps;
    if (nonPositiveSteps) {
        gPaletteTransitionSteps = 1;
    }
    return steps;
}

// 原函数：0x00450BB0。
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
        std::sprintf(
            path, "%s\\%s", resource::gBinPath, gPaletteFiles[previousActivePalette].path);
        platform::LegacyFile* file =
            std::fopen(path, "rb");
        if (file == nullptr) {
            return 0; // 失败不更新已加载索引；下一帧继续尝试打开。
        }

        for (int i = 16; i < 240; ++i) {
            gPaletteTarget[i].blue = static_cast<std::uint8_t>(
                std::fgetc(file));
            gPaletteTarget[i].green = static_cast<std::uint8_t>(
                std::fgetc(file));
            gPaletteTarget[i].red = static_cast<std::uint8_t>(
                std::fgetc(file));
            if (i == 168) {
                gPaletteTarget[i].blue = 0;
                gPaletteTarget[i].green = 0;
                gPaletteTarget[i].red = 0;
            }

            const std::int32_t blueCurrent = gPalette[i].blue;
            const std::int32_t greenCurrent = gPalette[i].green;
            const std::int32_t redCurrent = gPalette[i].red;
            gPaletteBlueCurrent[i] = static_cast<float>(blueCurrent);
            const std::int32_t blueDifference =
                static_cast<std::int32_t>(gPaletteTarget[i].blue) - blueCurrent;
            gPaletteGreenCurrent[i] = static_cast<float>(greenCurrent);
            const std::int32_t greenDifference =
                static_cast<std::int32_t>(gPaletteTarget[i].green) - greenCurrent;
            gPaletteRedCurrent[i] = static_cast<float>(redCurrent);
            const double steps = gPaletteTransitionSteps;
            const std::int32_t redDifference =
                static_cast<std::int32_t>(gPaletteTarget[i].red) - redCurrent;
            gPaletteBlueStep[i] = static_cast<float>(blueDifference / steps);
            gPaletteGreenStep[i] = static_cast<float>(greenDifference / steps);
            gPaletteRedStep[i] = static_cast<float>(redDifference / steps);
        }
        std::fclose(file);

        const std::int32_t loadedActivePalette = gActivePalette;
        gPaletteTransitionActive = 1;
        gPaletteFrameVersion = 1;
        const bool useDefaultTone = gPaletteTonePointer == &gPaletteTone;
        gPaletteVersionThrottle = 0;
        gLoadedPalette = loadedActivePalette;
        if (useDefaultTone) {
            const std::int32_t blueCurrent = gPaletteTone.blue;
            const std::int32_t greenCurrent = gPaletteTone.green;
            const std::int32_t redCurrent = gPaletteTone.red;
            toneTargetIndex = loadedActivePalette;
            gPaletteToneBlueCurrent = static_cast<float>(blueCurrent);
            const std::int32_t blueDifference = gPaletteToneTargets[toneTargetIndex][2] - blueCurrent;
            gPaletteToneTargetIndex = toneTargetIndex;
            gPaletteToneGreenCurrent = static_cast<float>(greenCurrent);
            const std::int32_t greenDifference = gPaletteToneTargets[toneTargetIndex][1] - greenCurrent;
            gPaletteToneRedCurrent = static_cast<float>(redCurrent);
            const double steps = gPaletteTransitionSteps;
            const std::int32_t redDifference = gPaletteToneTargets[toneTargetIndex][0] - redCurrent;
            gPaletteToneBlueStep = static_cast<float>(blueDifference / steps);
            gPaletteToneGreenStep = static_cast<float>(greenDifference / steps);
            gPaletteToneRedStep = static_cast<float>(redDifference / steps);
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

    const std::int32_t transitionSteps = gPaletteTransitionSteps;
    const std::int32_t transitionFrame = previousFrame + 1;
    gPaletteTransitionFrame = transitionFrame;
    if (transitionFrame > transitionSteps) {
        for (int i = 16; i < 240; ++i) {
            gPalette[i].blue = gPaletteTarget[i].blue;
            gPalette[i].green = gPaletteTarget[i].green;
            gPalette[i].red = gPaletteTarget[i].red;
        }
        if (gPaletteTonePointer == &gPaletteTone) {
            const std::uint8_t blue = gPaletteToneTargets[toneTargetIndex][2];
            const std::uint8_t green = gPaletteToneTargets[toneTargetIndex][1];
            gPaletteTone.blue = blue;
            const std::uint8_t red = gPaletteToneTargets[toneTargetIndex][0];
            gPaletteTone.green = green;
            gPaletteTone.red = red;
        }
        const std::int32_t previousCompletionVersion = gPaletteFrameVersion;
        gPaletteTransitionActive = 0;
        const std::int32_t completionVersion = previousCompletionVersion + 10;
        gPaletteChanging = 0;
        gPaletteFrameVersion = completionVersion;
    } else {
        for (int i = 16; i < 240; ++i) {
            // 蓝通道的原 __ftol 使用尚未舍入到 float 的 x87 加法值；绿/红使用已存 float。
            const double blue = static_cast<double>(gPaletteBlueStep[i])
                + gPaletteBlueCurrent[i];
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
            const std::uint8_t blue = TruncatedLowByte(gPaletteToneBlueCurrent);
            const float green = gPaletteToneGreenCurrent;
            gPaletteTone.blue = blue;
            const std::uint8_t greenByte = TruncatedLowByte(green);
            const float red = gPaletteToneRedCurrent;
            gPaletteTone.green = greenByte;
            gPaletteTone.red = TruncatedLowByte(red);
        }
        gPaletteChanging = 1;
    }

    // 8/16/24 位渲染分支已按范围排除；保留已实现的 32 位 CPU 查色表路径。
    RebuildPalette32();
    const std::int32_t rebuiltActivePalette = gActivePalette;
    const std::int32_t throttle = gPaletteVersionThrottle + 1;
    const std::int32_t throttleSteps = gPaletteTransitionSteps;
    gPaletteVersionThrottle = throttle;
    if (throttleSteps <= 120) {
        if (throttle >= 2) {
            gPaletteVersionThrottle = 0;
        }
        gPaletteFrameVersion = gPaletteFrameVersion + 1;
    } else if (throttle >= throttleSteps / 20) {
        gPaletteVersionThrottle = 0;
        gPaletteFrameVersion = gPaletteFrameVersion + 1;
    }

    if (previousActivePalette == rebuiltActivePalette && previousVersion == gPaletteFrameVersion) {
        return previousVersion;
    }
    RebuildToneMap(&ColorChannelMaps::red, &PaletteTone::blue);
    RebuildToneMap(&ColorChannelMaps::green, &PaletteTone::green);
    return RebuildToneMap(&ColorChannelMaps::blue, &PaletteTone::red);
}

// 原函数：0x00451220。
int RebuildPalette32() noexcept
{
    for (int i = 0; i < 256; ++i) {
        const std::uint32_t red = gPalette[i].red;
        const std::uint32_t green = gPalette[i].green;
        const std::uint32_t redGreen = (red << 8) + green;
        const std::uint32_t blue = gPalette[i].blue;
        const std::uint32_t color = (redGreen << 8) + blue;
        gPaletteAutoMap[i] = color;
        if (color == 0) {
            gPaletteAutoMap[i] = 1;
        }
    }
    gPaletteAutoMap[0] = 0;
    return 0x400;
}

} // namespace cg::render
