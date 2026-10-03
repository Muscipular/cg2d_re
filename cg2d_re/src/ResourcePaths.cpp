#include "cg/ResourcePaths.hpp"

#include "platform/LegacyFormat.hpp"
#include "platform/LegacyStdio.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/GraphicRead.hpp"
#include "cg/Palette.hpp"
#include "cg/ResourceNames.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/Startup.hpp"

#include <bit>
#include <cstring>

namespace cg::resource {
namespace {

// 原 [927F48,9A2068)；newMapRes 的已证高下标写入也是本表，不另建影子表。
std::uint8_t gAutoMapColors927F48[500000] {};

// 45A290/45A420 的共同采样顺序；只有首文件缺失或查询失败才终止。
bool ReadAutoMapFileTimes(char* const* const paths, std::int32_t (&times)[3])
{
    constexpr std::int32_t kSlots[] {0, 1, 6};
    for (std::int32_t index = 0; index < 3; ++index) {
        platform::LegacyFile* const file = platform::OpenLegacyStream_53B2F1(paths[kSlots[index]], "rb");
        if (file == nullptr) {
            if (index == 0) {
                return false;
            }
            continue;
        }
        std::int32_t value;
        const std::int32_t result = platform::ReadLegacyFileCreationTime32(file, &value);
        if (result >= 0) {
            times[index] = value;
        }
        platform::CloseLegacyStream_53B1D0(file);
        if (result < 0 && index == 0) {
            return false;
        }
    }
    return true;
}

} // namespace
} // namespace cg::resource

namespace cg::resource::detail {

static_assert(std::endian::native == std::endian::little);

// 原 45A420：每项读取后才比较，正文直接写共享表，短读保留已写前缀。
std::int32_t Sub_45A420(const char* const fileName, char* const* const graphicInfoPaths)
{
    std::int32_t times[3] {};
    if (!ReadAutoMapFileTimes(graphicInfoPaths, times)) {
        return 0;
    }
    platform::LegacyFile* const file = platform::OpenLegacyStream_53B2F1(fileName, "rb");
    if (file == nullptr) {
        return 0;
    }
    std::uint16_t version;
    if (platform::ReadLegacyStream_53BCCD(&version, 2, 1, file) != 1 || version != 2) {
        platform::CloseLegacyStream_53B1D0(file);
        return 0;
    }
    for (const std::int32_t expected : times) {
        std::int32_t stored;
        if (platform::ReadLegacyStream_53BCCD(&stored, 4, 1, file) != 1 || stored != expected) {
            platform::CloseLegacyStream_53B1D0(file);
            return 0;
        }
    }
    std::uint32_t sentinel;
    if (platform::ReadLegacyStream_53BCCD(&sentinel, 4, 1, file) != 1 || sentinel != 0
        || platform::ReadLegacyStream_53BCCD(gAutoMapColors927F48, 0x7A120u, 1, file) != 1) {
        platform::CloseLegacyStream_53B1D0(file);
        return 0;
    }
    platform::CloseLegacyStream_53B1D0(file);
    return 1;
}

// 原 45A290：成功打开后忽略写入/关闭结果，不把原写回改为事务提交。
std::int32_t Sub_45A290(const char* const fileName, char* const* const graphicInfoPaths)
{
    std::int32_t times[3] {};
    if (!ReadAutoMapFileTimes(graphicInfoPaths, times)) {
        return 0;
    }
    platform::LegacyFile* const file = platform::OpenLegacyStream_53B2F1(fileName, "wb");
    if (file == nullptr) {
        return 0;
    }
    const std::uint16_t version = 2;
    const std::uint32_t sentinel = 0;
    platform::WriteLegacyStream_53C680(&version, 2, 1, file);
    for (const std::int32_t time : times) {
        platform::WriteLegacyStream_53C680(&time, 4, 1, file);
    }
    platform::WriteLegacyStream_53C680(&sentinel, 4, 1, file);
    platform::WriteLegacyStream_53C680(gAutoMapColors927F48, 0x7A120u, 1, file);
    platform::CloseLegacyStream_53B1D0(file);
    return 1;
}

// 原 45A190：取色调用后才重取图形表；调色板读取失败不清已有颜色。
void Sub_45A190()
{
    const std::int32_t count = gMaxGraphicCount;
    render::PaletteEntry palette[256] {};
    if (render::ReadPalette(0, palette) == 0) {
        return;
    }
    std::memset(gAutoMapColors927F48, 0, sizeof(gAutoMapColors927F48));
    if (count <= 0) {
        return;
    }
    const GraphicInfo* records = gGraphicInfo;
    for (std::int32_t index = 0; index < count; ++index) {
        const std::int16_t fileIndex = records[index].fileIndex;
        const std::int32_t mapNo = records[index].mapNo;
        if (fileIndex != 0 && fileIndex != 1 && fileIndex != 6 && fileIndex != 8) {
            gAutoMapColors927F48[mapNo] = 0;
        } else if (mapNo == 0) {
            gAutoMapColors927F48[0] = 0;
        } else if (mapNo >= 1000 && mapNo <= 19999) {
            gAutoMapColors927F48[mapNo] = static_cast<std::uint8_t>(
                GetGraphicAutoMapColor_451D80(
                    std::bit_cast<std::int32_t>(records[index].seqNo), palette));
            records = gGraphicInfo;
        } else if (mapNo >= 220000 && mapNo <= 265535) {
            gAutoMapColors927F48[mapNo - 200000] = static_cast<std::uint8_t>(
                GetGraphicAutoMapColor_451D80(
                    std::bit_cast<std::int32_t>(records[index].seqNo), palette));
            records = gGraphicInfo;
        }
    }
}

} // namespace cg::resource::detail

namespace cg::resource {

const std::uint8_t* BorrowAutoMapColors() noexcept
{
    return gAutoMapColors927F48;
}

CursorPathStorage gCursorPathStorage {
    {
        {0x6B, 0x0003BE9B, 0xFFE2FFEC},
        {0x6C, 0x0003BE9C, 0x00000000},
        {0x6D, 0x0003C3A8, 0xFFEAFFE9},
        {0x75, 0x0003C3A1, 0xFFE7FFF0},
        {0x6F, 0x0003C3A2, 0xFFEAFFF7},
        {0x70, 0x0003C3A3, 0xFFF0FFF9},
        {0x71, 0x0003C3A4, 0xFFF6FFF7},
        {0x72, 0x0003C3A5, 0xFFF9FFF0},
        {0x73, 0x0003C3A6, 0xFFF6FFE9},
        {0x74, 0x0003C3A7, 0xFFF0FFE7},
    },
    0x6E,
    "Bin",
};
char (&gBinPath)[128] = gCursorPathStorage.binPath;

char gGraphicPathStorage[9][256] {};
char gGraphicInfoPathStorage[9][256] {};
char gAnimePathStorage[9][256] {};
char gAnimeInfoPathStorage[9][256] {};
char gCoordinatePathStorage[9][256] {};
char gCoordinateInfoPathStorage[9][256] {};

char* gGraphicPaths[9] {};
char* gGraphicInfoPaths[9] {};
char* gAnimePaths[9] {};
char* gAnimeInfoPaths[9] {};
char* gCoordinatePaths[9] {};
char* gCoordinateInfoPaths[9] {};

int BuildResourcePaths()
{
    for (int slot = 0; slot < 9; ++slot) {
        gGraphicPaths[slot] = gGraphicPathStorage[slot];
        gGraphicInfoPaths[slot] = gGraphicInfoPathStorage[slot];
        gAnimePaths[slot] = gAnimePathStorage[slot];
        gAnimeInfoPaths[slot] = gAnimeInfoPathStorage[slot];
        gCoordinatePaths[slot] = gCoordinatePathStorage[slot];
        gCoordinateInfoPaths[slot] = gCoordinateInfoPathStorage[slot];
    }

    int result = 0;
    for (int slot = 0; slot < 9; ++slot) {
        platform::FormatLegacyString_53B304(gGraphicPaths[slot], "%s\\%s", gBinPath, gResourceNames[GraphicNames][slot]);
        platform::FormatLegacyString_53B304(gGraphicInfoPaths[slot], "%s\\%s", gBinPath, gResourceNames[GraphicInfoNames][slot]);
        platform::FormatLegacyString_53B304(gAnimePaths[slot], "%s\\%s", gBinPath, gResourceNames[AnimeNames][slot]);
        platform::FormatLegacyString_53B304(gAnimeInfoPaths[slot], "%s\\%s", gBinPath, gResourceNames[AnimeInfoNames][slot]);
        platform::FormatLegacyString_53B304(gCoordinatePaths[slot], "%s\\%s", gBinPath, gResourceNames[CoordinateNames][slot]);
        result = platform::FormatLegacyString_53B304(
            gCoordinateInfoPaths[slot], "%s\\%s", gBinPath, gResourceNames[CoordinateInfoNames][slot]);
    }
    return result;
}

std::int32_t LoadAutoMapColor_45A130(char* const* const graphicInfoPaths)
{
    char fileName[128];
    platform::FormatLegacyString_53B304(fileName, "%s\\%s", platform::gDataPath, "auto.dat");

    const std::int32_t loaded =
        detail::Sub_45A420(fileName, graphicInfoPaths);
    if (loaded != 0) {
        return loaded;
    }

    detail::Sub_45A190();
    return detail::Sub_45A290(fileName, graphicInfoPaths);
}

} // namespace cg::resource
