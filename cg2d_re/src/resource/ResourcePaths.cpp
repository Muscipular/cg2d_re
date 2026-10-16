#include "cg/resource/ResourcePaths.hpp"


#include "cg/resource/GraphicInfo.hpp"
#include "cg/resource/GraphicRead.hpp"
#include "cg/render/Palette.hpp"
#include "cg/resource/ResourceNames.hpp"
#include "cg/platform/Startup.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <bit>
#include <cstring>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053BCCD、0x0053C680；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::resource {

// 原 [927F48,9A2068)；newMapRes 的已证高下标写入也是本表，不另建影子表。
std::uint8_t gAutoMapColors927F48[500000] {}; // 原全局：0x00927F48。

namespace {

// 45A290/45A420 的共同采样顺序；只有首文件缺失或查询失败才终止。
bool ReadAutoMapFileTimes(char* const* const paths, std::int32_t (&times)[3])
{
    constexpr std::int32_t kSlots[] {0, 1, 6};
    for (std::int32_t index = 0; index < 3; ++index) {
        std::FILE* const file = std::fopen(paths[kSlots[index]], "rb");
        if (file == nullptr) {
            if (index == 0) {
                return false;
            }
            continue;
        }
        struct _stat32 status;
        const std::int32_t result = ::_fstat32(::_fileno(file), &status);
        if (result == 0) {
            times[index] = static_cast<std::int32_t>(status.st_ctime);
        }
        std::fclose(file);
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
    std::FILE* const file = std::fopen(fileName, "rb");
    if (file == nullptr) {
        return 0;
    }
    std::uint16_t version;
    if (static_cast<std::uint32_t>(std::fread(&version, 2, 1, file)) != 1 || version != 2) {
        std::fclose(file);
        return 0;
    }
    for (const std::int32_t expected : times) {
        std::int32_t stored;
        if (static_cast<std::uint32_t>(std::fread(&stored, 4, 1, file)) != 1 || stored != expected) {
            std::fclose(file);
            return 0;
        }
    }
    std::uint32_t sentinel;
    if (static_cast<std::uint32_t>(std::fread(&sentinel, 4, 1, file)) != 1 || sentinel != 0
        || static_cast<std::uint32_t>(std::fread(gAutoMapColors927F48, 0x7A120u, 1, file)) != 1) {
        std::fclose(file);
        return 0;
    }
    std::fclose(file);
    return 1;
}

// 原 45A290：成功打开后忽略写入/关闭结果，不把原写回改为事务提交。
std::int32_t Sub_45A290(const char* const fileName, char* const* const graphicInfoPaths)
{
    std::int32_t times[3] {};
    if (!ReadAutoMapFileTimes(graphicInfoPaths, times)) {
        return 0;
    }
    std::FILE* const file = std::fopen(fileName, "wb");
    if (file == nullptr) {
        return 0;
    }
    const std::uint16_t version = 2;
    const std::uint32_t sentinel = 0;
    static_cast<std::uint32_t>(std::fwrite(&version, 2, 1, file));
    for (const std::int32_t time : times) {
        static_cast<std::uint32_t>(std::fwrite(&time, 4, 1, file));
    }
    static_cast<std::uint32_t>(std::fwrite(&sentinel, 4, 1, file));
    static_cast<std::uint32_t>(std::fwrite(gAutoMapColors927F48, 0x7A120u, 1, file));
    std::fclose(file);
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

CursorPathStorage gCursorPathStorage { // 原全局：0x0055CE88。
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
char (&gBinPath)[128] = gCursorPathStorage.binPath; // 原全局：0x0055CF04。

char gGraphicPathStorage[9][256] {}; // 原全局：0x0062DA98。
char gGraphicInfoPathStorage[9][256] {}; // 原全局：0x0062D174。
char gAnimePathStorage[9][256] {}; // 原全局：0x0062B5E4。
char gAnimeInfoPathStorage[9][256] {}; // 原全局：0x0062C808。
char gCoordinatePathStorage[9][256] {}; // 原全局：0x0062E398。
char gCoordinateInfoPathStorage[9][256] {}; // 原全局：0x0062BF08。

char* gGraphicPaths[9] {}; // 原全局：0x0062D108。
char* gGraphicInfoPaths[9] {}; // 原全局：0x0062DA74。
char* gAnimePaths[9] {}; // 原全局：0x0062BEE4。
char* gAnimeInfoPaths[9] {}; // 原全局：0x0062D150。
char* gCoordinatePaths[9] {}; // 原全局：0x0062B5C0。
char* gCoordinateInfoPaths[9] {}; // 原全局：0x0062D12C。

// 原函数：0x00456050。
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
        std::sprintf(gGraphicPaths[slot], "%s\\%s", gBinPath, cg::resource::gResourceNames[cg::resource::GraphicNames][slot]);
        std::sprintf(gGraphicInfoPaths[slot], "%s\\%s", gBinPath, cg::resource::gResourceNames[cg::resource::GraphicInfoNames][slot]);
        std::sprintf(gAnimePaths[slot], "%s\\%s", gBinPath, cg::resource::gResourceNames[cg::resource::AnimeNames][slot]);
        std::sprintf(gAnimeInfoPaths[slot], "%s\\%s", gBinPath, cg::resource::gResourceNames[cg::resource::AnimeInfoNames][slot]);
        std::sprintf(gCoordinatePaths[slot], "%s\\%s", gBinPath, cg::resource::gResourceNames[cg::resource::CoordinateNames][slot]);
        result = std::sprintf(
            gCoordinateInfoPaths[slot], "%s\\%s", gBinPath, cg::resource::gResourceNames[cg::resource::CoordinateInfoNames][slot]);
    }
    return result;
}

// 原函数：0x0045A130。
std::int32_t LoadAutoMapColor_45A130(char* const* const graphicInfoPaths)
{
    char fileName[128];
    std::sprintf(fileName, "%s\\%s", platform::gDataPath, "auto.dat");

    const std::int32_t loaded =
        detail::Sub_45A420(fileName, graphicInfoPaths);
    if (loaded != 0) {
        return loaded;
    }

    detail::Sub_45A190();
    return detail::Sub_45A290(fileName, graphicInfoPaths);
}

} // namespace cg::resource
