#include "cg/resource/AnimationRuntime.hpp"
#include "cg/text/Encoding.hpp"
#include <string>

#include "AnimationRuntimeState.hpp"

#include "cg/resource/GraphicAlloc.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/platform/LegacyWindow.hpp"

#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053BC41、0x0053BCCD；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstdlib>
#include <cstring> // 原CRT接口：0x0053CF50；原参数／返回合同由直接调用承接。
#include <cstdint>
#include <algorithm>
#include <format>
#include <memory>
#include <new>

namespace cg::resource::animation::detail {

std::array<std::uint32_t, BinLoaderMaxGroups> gFileCloseFlags9A29D0 {}; // 原全局：0x009A29D0。
std::array<MainSlot, 256> gMainSlots9A29F8 {}; // 原全局：0x009A29F8。
MainSlot gLeftSentinel9A3A00 {}; // 原全局：0x009A3A00。
MainSlot gRightSentinel9A3A10 {}; // 原全局：0x009A3A10。
std::array<std::FILE*, BinLoaderMaxGroups> gAnimateFiles9A3A20 {}; // 原全局：0x009A3A20。
std::array<std::FILE*, BinLoaderMaxGroups> gCoordinateFiles9A2988 {}; // 原全局：0x009A2988。
std::array<CacheSlot, 256> gCacheSlots9A3A48 {}; // 原全局：0x009A3A48。
std::uint32_t gActiveCount9A4A68 = 0; // 原全局：0x009A4A68。
std::uint32_t gActiveBytes9A4A6C = 0; // 原全局：0x009A4A6C。
void* gCleanupAllocation9A4A54 = nullptr; // 原全局：0x009A4A54。

} // namespace cg::resource::animation::detail

namespace cg::resource::animation {

AnimateRecord* gAnimateRecords9A4A58 = nullptr; // 原全局：0x009A4A58。
AnimeCacheRecord* gAnimateCache9A4A5C = nullptr; // 原全局：0x009A4A5C。
std::int32_t gAnimateCount9A4A60 = 0; // 原全局：0x009A4A60。
std::int32_t gMaxAnimateCount9A4A4C = 0; // 原全局：0x009A4A4C。

namespace {

// 原45B380独立W32复制目标，不是9A3A48缓存数组的一过尾记录。
std::uint32_t gValue9A4A48 = 0; // 原全局：0x009A4A48。

// 原AnimeInit磁盘记录和临时配对项；BLM01按配置容量分配，字段仍保持原宽度。
struct AnimeInfoDiskRecord {
    std::uint32_t animeNo; // 原成员偏移：+0x00。
    std::uint32_t offset; // 原成员偏移：+0x04。
    std::uint16_t count; // 原成员偏移：+0x08。
    std::uint16_t flag; // 原成员偏移：+0x0A。
};
static_assert(sizeof(AnimeInfoDiskRecord) == 0x0C);
static_assert(offsetof(AnimeInfoDiskRecord, count) == 0x08);
static_assert(offsetof(AnimeInfoDiskRecord, flag) == 0x0A);

struct AnimeInfoPair {
    std::uint32_t animeNo; // 原成员偏移：+0x00。
    std::uint32_t offset; // 原成员偏移：+0x04。
    std::uint16_t count; // 原成员偏移：+0x08。
    std::uint16_t flag; // 原成员偏移：+0x0A。
    std::int16_t fileIndex; // 原成员偏移：+0x0C。
    std::uint16_t remaining; // 原成员偏移：+0x0E。
};
static_assert(sizeof(AnimeInfoPair) == 0x10);
static_assert(offsetof(AnimeInfoPair, count) == 0x08);
static_assert(offsetof(AnimeInfoPair, flag) == 0x0A);
static_assert(offsetof(AnimeInfoPair, fileIndex) == 0x0C);

} // namespace

// 原函数：0x0045B370。
std::uint32_t Sub_45B370()
{
    return Sub_45B380();
}

// 原函数：0x0045B380。
std::uint32_t Sub_45B380()
{
    const std::uint32_t value = gValue9A4A50;
    gValue9A4A48 = value;
    return value;
}

// 原函数：0x0045BB20。
// BLM01：完整12B记录，后组覆盖同号动画；临时配对容量与主表一致。
std::int32_t AnimeInit_45BB20(
    char* const* const dataPaths, char* const* const infoPaths)
{
    Sub_45B5D0();
    for (std::FILE*& file : detail::gAnimateFiles9A3A20) {
        if (file != nullptr) {
            std::fclose(file);
            file = nullptr;
        }
    }
    detail::gFileCloseFlags9A29D0.fill(0);
    gAnimateCount9A4A60 = 0;
    const auto cleanupFailure = [](const std::int32_t error) {
        for (std::FILE*& file : detail::gAnimateFiles9A3A20) {
            if (file != nullptr) {
                std::fclose(file);
                file = nullptr;
            }
        }
        return error;
    };
    std::unique_ptr<AnimeInfoPair[]> pairs(
        new (std::nothrow) AnimeInfoPair[gMaxAnimateCount9A4A4C]);
    constexpr std::size_t kAnimeIndexCount = 0x10000u * 4u;
    std::unique_ptr<std::int32_t[]> indices(new (std::nothrow) std::int32_t[kAnimeIndexCount]);
    if (pairs == nullptr || indices == nullptr) {
        return 3;
    }
    std::fill_n(indices.get(), kAnimeIndexCount, -1);
    std::int32_t uniqueCount = 0;
    AnimeInfoDiskRecord info;
    for (std::size_t slot = 0; slot < gBinLoaderGroupCount; ++slot) {
        if (dataPaths[slot][0] == '\0' || infoPaths[slot][0] == '\0') {
            continue;
        }
        std::FILE* const infoFile = std::fopen(infoPaths[slot], "rb");
        if (infoFile == nullptr) {
            platform::ShowWideMessage(
                L"动画索引文件开启失败：" + text::FromLegacyDisplayWide(infoPaths[slot]),
                L"资源加载", 0);
            continue;
        }
        detail::gAnimateFiles9A3A20[slot] = std::fopen(dataPaths[slot], "rb");
        if (detail::gAnimateFiles9A3A20[slot] == nullptr) {
            platform::ShowWideMessage(
                L"动画文件开启失败：" + text::FromLegacyDisplayWide(dataPaths[slot]),
                L"资源加载", 0);
            std::fclose(infoFile);
            continue;
        }
        while (std::fread(&info, sizeof(info), 1, infoFile) == 1) {
            const std::uint32_t key = info.animeNo - 100000u;
            std::int32_t index = indices[key];
            if (index == -1) {
                if (uniqueCount >= gMaxAnimateCount9A4A4C) {
                    std::fclose(infoFile);
                    return cleanupFailure(3);
                }
                index = uniqueCount++;
                indices[key] = index;
            }
            pairs[index].animeNo = key;
            pairs[index].offset = info.offset;
            pairs[index].count = info.count;
            pairs[index].flag = info.flag > 1 ? 0 : info.flag;
            pairs[index].fileIndex = static_cast<std::int16_t>(slot);
            if (info.count > 10000u) {
                platform::ShowWideMessage(
                    std::format(L"i={},animsize={}", slot, info.count), L"动画资料损坏", 0);
                std::fclose(infoFile);
                return cleanupFailure(5);
            }
        }
        std::fclose(infoFile);
    }
    if (uniqueCount >= gMaxAnimateCount9A4A4C) {
        platform::ShowWideMessage(
            std::format(L"MAX_ANIME={},Need={}", gMaxAnimateCount9A4A4C, uniqueCount),
            L"动画容量不足", 0);
        return cleanupFailure(3);
    }
    for (std::int32_t index = 0; index < uniqueCount; ++index) {
        AnimateRecord& record = gAnimateRecords9A4A58[index];
        record.offset = pairs[index].offset;
        record.fileIndex = pairs[index].fileIndex;
        record.animeNo = pairs[index].animeNo;
        record.count = pairs[index].count;
        record.flag = pairs[index].flag;
        record.coordinateOffset = 0xFFFFFFFFu;
        record.coordinateCount = 0;
        record.actionList = nullptr;
        record.listNode = nullptr;
        record.coordinates = nullptr;
    }
    gAnimateCount9A4A60 = uniqueCount;
    std::qsort(gAnimateRecords9A4A58, static_cast<std::size_t>(uniqueCount),
        sizeof(AnimateRecord), Compare_45B420);
    return 0;
}

// 原函数：0x0045B420。
std::int32_t __cdecl Compare_45B420(
    const void* const left, const void* const right)
{
    const auto* const a = static_cast<const AnimateRecord*>(left);
    const auto* const b = static_cast<const AnimateRecord*>(right);
    if (a->animeNo < b->animeNo) {
        return -1;
    }
    return a->animeNo > b->animeNo ? 1 : 0;
}

// 原函数：0x0045B390。
std::int32_t __cdecl BinarySearch_45B390(
    const std::uint32_t animeNo, std::int32_t min, std::int32_t max)
{
    if (min > max) {
        return -1;
    }
    while (max - min >= 10) {
        const std::int32_t middle = min + (max - min) / 2;
        const std::uint32_t current =
            gAnimateRecords9A4A58[middle].animeNo;
        if (current == animeNo) {
            return middle;
        }
        if (current > animeNo) {
            max = middle - 1;
        } else {
            min = middle + 1;
        }
        if (min > max) {
            return -1;
        }
    }
    for (std::int32_t index = min; index <= max; ++index) {
        if (gAnimateRecords9A4A58[index].animeNo == animeNo) {
            return index;
        }
    }
    return -1;
}

// 原函数：0x0045B400。
std::int32_t __cdecl GetIndex_45B400(const std::uint32_t animeNo)
{
    const std::int32_t result =
        BinarySearch_45B390(animeNo, 0, gAnimateCount9A4A60);
    return result < 0 ? -1 : result;
}

// 原函数：0x0045B760。
std::int32_t AnimeLoadAction_45B760(const std::int32_t index)
{
    const auto fileIndex = static_cast<std::uint16_t>(gAnimateRecords9A4A58[index].fileIndex);
    if (gAnimateRecords9A4A58[index].actionList != nullptr) {
        Sub_45B5B0(static_cast<detail::MainSlot*>(gAnimateRecords9A4A58[index].listNode));
        return 1;
    }
    detail::MainSlot* const slot = Sub_45B530();
    if (slot == nullptr) {
        return 0;
    }
    std::FILE*& file = detail::gAnimateFiles9A3A20[fileIndex];
    ::_fseeki64(file,
        static_cast<std::int64_t>(gAnimateRecords9A4A58[index].offset), SEEK_SET);
    const std::int32_t graphicBase = gGraphicStartIndices[fileIndex];
    if (gAnimateRecords9A4A58[index].count == 0) {
        return 0;
    }
    gAnimateRecords9A4A58[index].actionList = static_cast<detail::ActionEntry*>(
        std::malloc(sizeof(detail::ActionEntry) * gAnimateRecords9A4A58[index].count));
    if (gAnimateRecords9A4A58[index].actionList == nullptr) {
        Sub_45B440(&slot->index);
        return 0;
    }
    slot->index = index;
    gAnimateRecords9A4A58[index].listNode = slot;
    Sub_45B590(slot);
    slot->value04 = 28u * gAnimateRecords9A4A58[index].count;
    std::memset(gAnimateRecords9A4A58[index].actionList, 0,
        sizeof(detail::ActionEntry) * gAnimateRecords9A4A58[index].count);
    if (gAnimateRecords9A4A58[index].count > 10000) {
        Sub_45B440(&slot->index);
        return 0;
    }

    // 原函数共用的 2000B 栈区，头/帧读取相互覆盖；不清零、不扩容或补短读。
    std::uint16_t buffer[1000];
    for (std::int32_t actionIndex = 0;
         actionIndex < gAnimateRecords9A4A58[index].count; ++actionIndex) {
        static_cast<std::uint32_t>(std::fread(buffer, static_cast<std::uint32_t>(gAnimateRecords9A4A58[index].flag >= 1 ? 20 : 12), 1, file));
        auto* const action = static_cast<detail::ActionEntry*>(
            gAnimateRecords9A4A58[index].actionList) + actionIndex;
        action->frames = nullptr;
        action->direction = static_cast<std::int16_t>(buffer[0]);
        action->action = static_cast<std::int16_t>(buffer[1]);
        action->duration = buffer[2];
        action->durationLowWord = buffer[2];
        action->duration |= static_cast<std::uint32_t>(buffer[3]) << 16;
        action->frameCount = buffer[4];
        action->frameCount |= static_cast<std::uint32_t>(buffer[5]) << 16;
        if (action->frameCount != 0) {
            action->duration /= 16u * action->frameCount;
            if (action->duration == 0) {
                action->duration = 1;
            }
        }
        if (gAnimateRecords9A4A58[index].flag >= 1) {
            action->paletteIndex = static_cast<std::int16_t>(buffer[6]);
            action->revert = buffer[7];
            action->value14 = buffer[8];
            action->value14 |= static_cast<std::uint32_t>(buffer[9]) << 16;
        } else {
            action->paletteIndex = 0;
            action->revert = 0;
            action->value14 = 0xFFFFFFFFu;
        }
        if (action->frameCount >= 10000) {
            Sub_45B440(&slot->index);
            return 0;
        }
        action->frames = static_cast<detail::FrameEntry*>(
            std::malloc(sizeof(detail::FrameEntry) * action->frameCount));
        if (action->frames == nullptr) {
            Sub_45B440(&slot->index);
            return 0;
        }
        slot->value04 += 12u * action->frameCount;
        static_cast<std::uint32_t>(std::fread(buffer, static_cast<std::uint32_t>(10u * action->frameCount), 1, file));
        for (std::uint32_t frameIndex = 0; frameIndex < action->frameCount; ++frameIndex) {
            const std::uint16_t* const source = buffer + frameIndex * 5;
            static_cast<detail::FrameEntry*>(action->frames)[frameIndex].graphicIndex = source[0];
            static_cast<detail::FrameEntry*>(action->frames)[frameIndex].graphicIndex |=
                static_cast<std::uint32_t>(source[1]) << 16;
            static_cast<detail::FrameEntry*>(action->frames)[frameIndex].graphicIndex +=
                static_cast<std::uint32_t>(graphicBase);
            static_cast<detail::FrameEntry*>(action->frames)[frameIndex].x =
                static_cast<std::int16_t>(source[2]);
            static_cast<detail::FrameEntry*>(action->frames)[frameIndex].y =
                static_cast<std::int16_t>(source[3]);
            static_cast<detail::FrameEntry*>(action->frames)[frameIndex].flag = source[4];
        }
    }
    if (CoordInit_45C150(index) == 0) {
        Sub_45B440(&slot->index);
        return 0;
    }
    slot->value04 += 8u * gAnimateRecords9A4A58[index].coordinateCount;
    ++detail::gActiveCount9A4A68;
    detail::gActiveBytes9A4A6C += slot->value04;
    return 1;
}

// 原函数：0x0045C150。
std::int32_t CoordInit_45C150(const std::int32_t index)
{
    const auto fileIndex = static_cast<std::uint16_t>(gAnimateRecords9A4A58[index].fileIndex);
    std::FILE*& file = detail::gCoordinateFiles9A2988[fileIndex];
    if (file == nullptr || gAnimateRecords9A4A58[index].actionList == nullptr
        || gAnimateRecords9A4A58[index].coordinates != nullptr
        || gAnimateRecords9A4A58[index].coordinateCount == 0) {
        return 1;
    }
    if (gAnimateRecords9A4A58[index].coordinateCount > 1000) {
        std::wstring message;
        cg::text::FormatTo(message, L"{} / 1000", gAnimateRecords9A4A58[index].coordinateCount);
        platform::ShowWideMessage(message, L"座标资料数量异常。", 0);
        return 0;
    }
    const std::int32_t graphicBase = gGraphicStartIndices[fileIndex];
    ::_fseeki64(file,
        static_cast<std::int64_t>(gAnimateRecords9A4A58[index].coordinateOffset), SEEK_SET);
    gAnimateRecords9A4A58[index].coordinates = static_cast<detail::CoordinateEntry*>(
        std::malloc(sizeof(detail::CoordinateEntry) * gAnimateRecords9A4A58[index].coordinateCount));
    if (gAnimateRecords9A4A58[index].coordinates == nullptr) {
        return 0;
    }
    detail::CoordinateEntry buffer[1000];
    static_cast<std::uint32_t>(std::fread(buffer, 8, static_cast<std::uint32_t>(gAnimateRecords9A4A58[index].coordinateCount), file));
    for (std::int32_t coordinate = 0;
         coordinate < gAnimateRecords9A4A58[index].coordinateCount; ++coordinate) {
        auto* const destination = static_cast<detail::CoordinateEntry*>(
            gAnimateRecords9A4A58[index].coordinates) + coordinate;
        destination->graphicIndex = buffer[coordinate].graphicIndex
            + static_cast<std::uint32_t>(graphicBase);
        destination->x = buffer[coordinate].x;
        destination->y = buffer[coordinate].y;
    }
    return 1;
}

// 原函数：0x0045C040。
// BLM01：成功的后组坐标文件覆盖所有组的共享来源，空路径继承前组。
void CoordLoadInit_45C040(char* const* const dataPaths, char* const* const infoPaths)
{
    CloseCoordinateFiles();
    for (std::size_t slot = 0; slot < gBinLoaderGroupCount; ++slot) {
        if (dataPaths[slot][0] == '\0' || infoPaths[slot][0] == '\0') {
            if (slot > 0) {
                detail::gCoordinateFiles9A2988[slot] = detail::gCoordinateFiles9A2988[slot - 1];
            }
            continue;
        }
        std::FILE* loadedFile = nullptr;
        if (CoordLoad_45BF60(&loadedFile, dataPaths[slot], infoPaths[slot]) == 0) {
            continue;
        }
        CloseCoordinateFiles();
        for (std::size_t group = 0; group < gBinLoaderGroupCount; ++group) {
            detail::gCoordinateFiles9A2988[group] = loadedFile;
        }
    }
}

// 原函数：0x0045BF60。
// BLM01：临时Info文件始终关闭，仅成功后向调用方交付数据FILE。
std::int32_t CoordLoad_45BF60(
    std::FILE** const outputFile, const char* const dataPath, const char* const infoPath)
{
    std::FILE* const infoFile = std::fopen(infoPath, "rb");
    if (infoFile == nullptr) {
        platform::ShowWideMessage(
            L"坐标索引文件开启失败：" + text::FromLegacyDisplayWide(infoPath), L"资源加载", 0);
        return 0;
    }
    std::uint8_t version;
    if (std::fread(&version, 1, 1, infoFile) != 1 || version != 1) {
        std::fclose(infoFile);
        return 0;
    }
    std::FILE* const dataFile = std::fopen(dataPath, "rb");
    if (dataFile == nullptr) {
        platform::ShowWideMessage(
            L"坐标文件开启失败：" + text::FromLegacyDisplayWide(dataPath), L"资源加载", 0);
        std::fclose(infoFile);
        return 0;
    }
    std::uint32_t record[2];
    std::uint16_t count;
    while (std::fread(record, sizeof(record), 1, infoFile) == 1
        && std::fread(&count, sizeof(count), 1, infoFile) == 1) {
        const std::int32_t index = GetIndex_45B400(record[0] - 100000u);
        if (index >= 0) {
            gAnimateRecords9A4A58[index].coordinateOffset = record[1];
            gAnimateRecords9A4A58[index].coordinateCount = count;
        }
    }
    std::fclose(infoFile);
    *outputFile = dataFile;
    return 1;
}

} // namespace cg::resource::animation
