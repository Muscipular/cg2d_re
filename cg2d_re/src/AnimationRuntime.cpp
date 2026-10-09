#include "cg/AnimationRuntime.hpp"

#include "AnimationRuntimeState.hpp"
#include "platform/LegacyFormat.hpp"
#include "platform/LegacyStdio.hpp"

#include "cg/GraphicAlloc.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/LegacyStringCompare.hpp"

#include <cstdlib>
#include <cstring>

namespace cg::resource::animation::detail {

std::array<std::uint32_t, 9> gFileCloseFlags9A29D0 {};
std::array<MainSlot, 256> gMainSlots9A29F8 {};
MainSlot gLeftSentinel9A3A00 {};
MainSlot gRightSentinel9A3A10 {};
std::array<platform::LegacyFile*, 9> gAnimateFiles9A3A20 {};
std::array<platform::LegacyFile*, 9> gCoordinateFiles9A2988 {};
std::array<CacheSlot, 256> gCacheSlots9A3A48 {};
std::uint32_t gActiveCount9A4A68 = 0;
std::uint32_t gActiveBytes9A4A6C = 0;
void* gCleanupAllocation9A4A54 = nullptr;

} // namespace cg::resource::animation::detail

namespace cg::resource::animation {

AnimateRecord* gAnimateRecords9A4A58 = nullptr;
AnimeCacheRecord* gAnimateCache9A4A5C = nullptr;
std::int32_t gAnimateCount9A4A60 = 0;
std::int32_t gMaxAnimateCount9A4A4C = 0;

namespace {

// 原45B380独立W32复制目标，不是9A3A48缓存数组的一过尾记录。
std::uint32_t gValue9A4A48 = 0;

// 原 AnimeInit 的磁盘读取槽与 10000 项局部配对表；均无指针，不随 x64 扩宽。
struct AnimeInfoDiskRecord {
    std::uint32_t animeNo;
    std::uint32_t offset;
    std::uint16_t count;
    std::uint16_t flag;
};
static_assert(sizeof(AnimeInfoDiskRecord) == 0x0C);
static_assert(offsetof(AnimeInfoDiskRecord, count) == 0x08);
static_assert(offsetof(AnimeInfoDiskRecord, flag) == 0x0A);

struct AnimeInfoPair {
    std::uint32_t animeNo;
    std::uint32_t offset;
    std::uint16_t count;
    std::uint16_t flag;
    std::int16_t fileIndex;
    std::uint16_t remaining;
};
static_assert(sizeof(AnimeInfoPair) == 0x10);
static_assert(offsetof(AnimeInfoPair, count) == 0x08);
static_assert(offsetof(AnimeInfoPair, flag) == 0x0A);
static_assert(offsetof(AnimeInfoPair, fileIndex) == 0x0C);

constexpr char kAnimeSizeErrorCaption[] =
    "\x83\x41\x83\x6A\x3F\x81\x5B\x83\x56\x3F\x3F\x83\x66\x81\x5B\x83\x5E"
    "\x82\xAA\x94\x6A\x91\xB9\x82\xB5\x82\xC4\x82\xA2\x82\xDC\x82\xB7\x81\x42";

} // namespace

std::uint32_t Sub_45B370()
{
    return Sub_45B380();
}

std::uint32_t Sub_45B380()
{
    const std::uint32_t value = gValue9A4A50;
    gValue9A4A48 = value;
    return value;
}

std::int32_t AnimeInit_45BB20(
    char* const* const dataPaths, char* const* const infoPaths)
{
    std::int32_t uniqueCount = 0;
    Sub_45B5D0();
    char localInfoPaths[9][256];
    char localDataPaths[9][256];
    for (std::size_t slot = 0; slot < 9; ++slot) {
        std::strcpy(localInfoPaths[slot], infoPaths[slot]);
        std::strcpy(localDataPaths[slot], dataPaths[slot]);
    }
    std::array<AnimeInfoPair, 10000> pairs {};
    std::array<std::int32_t, 20500> indices;
    indices.fill(-1);
    std::array<std::uint32_t, 9> filePresent {};
    AnimeInfoDiskRecord info;

    const auto cleanupFailure = [](const std::int32_t error) {
        if (detail::gCleanupAllocation9A4A54 != nullptr) {
            std::free(detail::gCleanupAllocation9A4A54);
        }
        for (platform::LegacyFile*& file : detail::gAnimateFiles9A3A20) {
            if (file != nullptr) {
                platform::CloseLegacyStream_53B1D0(file);
                file = nullptr;
            }
        }
        return error;
    };

    for (std::int32_t slot = 0; slot < 9; ++slot) {
        platform::LegacyFile* const file = platform::OpenLegacyStream_53B2F1(localInfoPaths[slot], "rb");
        if (file == nullptr) {
            if (slot == 0 || slot == 8) {
                return 2;
            }
            continue;
        }
        for (;;) {
            // 原容量检查在 fread 之后、EOF 之前；读取槽跨轮复用且不补短读。
            platform::ReadLegacyStream_53BCCD(&info, 0x0Cu, 1, file);
            if (uniqueCount >= gMaxAnimateCount9A4A4C) {
                platform::CloseLegacyStream_53B1D0(file);
                return 3;
            }
            if (std::feof(file) != 0) {
                filePresent[slot] = 1;
                platform::CloseLegacyStream_53B1D0(file);
                break;
            }
            const std::uint32_t key = info.animeNo - 100000u;
            std::int32_t index = indices[key];
            if (index == -1) {
                index = uniqueCount++;
            } else if (key == 0) {
                index = indices[0];
            }
            pairs[index].animeNo = key;
            pairs[index].offset = info.offset;
            const std::uint16_t count = info.count;
            pairs[index].count = count;
            pairs[index].flag = info.flag;
            if (info.flag > 1) {
                pairs[index].flag = 0;
            }
            pairs[index].fileIndex = static_cast<std::int16_t>(slot);
            indices[key] = index;
            if (count > 10000u) {
                char message[256];
                platform::FormatLegacyString_53B304(message, "i=%d,animsize=%d", slot, static_cast<int>(count));
                platform::ShowLegacyMessage(message, kAnimeSizeErrorCaption, 0);
                platform::CloseLegacyStream_53B1D0(file);
                return cleanupFailure(5);
            }
        }
    }

    // 原 45BD26 到此才清槽，不预先关闭旧句柄，也不在 Info 失败早退时清理。
    detail::gAnimateFiles9A3A20.fill(nullptr);
    for (std::size_t slot = 0; slot < 9; ++slot) {
        if (filePresent[slot] == 1) {
            detail::gAnimateFiles9A3A20[slot] = platform::OpenLegacyStream_53B2F1(localDataPaths[slot], "rb");
            if (detail::gAnimateFiles9A3A20[slot] == nullptr && (slot == 0 || slot == 8)) {
                return cleanupFailure(2);
            }
        }
    }
    for (std::int32_t index = 0; index < uniqueCount; ++index) {
        gAnimateRecords9A4A58[index].offset = pairs[index].offset;
        gAnimateRecords9A4A58[index].fileIndex = pairs[index].fileIndex;
        gAnimateRecords9A4A58[index].animeNo = pairs[index].animeNo;
        gAnimateRecords9A4A58[index].count = pairs[index].count;
        gAnimateRecords9A4A58[index].flag = pairs[index].flag;
        gAnimateRecords9A4A58[index].coordinateOffset = 0xFFFFFFFFu;
        gAnimateRecords9A4A58[index].coordinateCount = 0;
        gAnimateRecords9A4A58[index].actionList = nullptr;
        gAnimateRecords9A4A58[index].listNode = nullptr;
    }
    gAnimateCount9A4A60 = uniqueCount;
    std::qsort(gAnimateRecords9A4A58, static_cast<std::uint32_t>(uniqueCount),
        sizeof(AnimateRecord), Compare_45B420);
    return 0;
}

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

std::int32_t __cdecl GetIndex_45B400(const std::uint32_t animeNo)
{
    const std::int32_t result =
        BinarySearch_45B390(animeNo, 0, gAnimateCount9A4A60);
    return result < 0 ? -1 : result;
}

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
    platform::LegacyFile*& file = detail::gAnimateFiles9A3A20[fileIndex];
    platform::SeekLegacyStream_53BC41(file,
        static_cast<std::int32_t>(gAnimateRecords9A4A58[index].offset), 0);
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
        platform::ReadLegacyStream_53BCCD(buffer, gAnimateRecords9A4A58[index].flag >= 1 ? 20 : 12, 1, file);
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
        platform::ReadLegacyStream_53BCCD(buffer, 10u * action->frameCount, 1, file);
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

std::int32_t CoordInit_45C150(const std::int32_t index)
{
    const auto fileIndex = static_cast<std::uint16_t>(gAnimateRecords9A4A58[index].fileIndex);
    platform::LegacyFile*& file = detail::gCoordinateFiles9A2988[fileIndex];
    if (file == nullptr || gAnimateRecords9A4A58[index].actionList == nullptr
        || gAnimateRecords9A4A58[index].coordinates != nullptr
        || gAnimateRecords9A4A58[index].coordinateCount == 0) {
        return 1;
    }
    if (gAnimateRecords9A4A58[index].coordinateCount > 1000) {
        char message[256];
        platform::FormatLegacyString_53B304(message, "%d / 1000", gAnimateRecords9A4A58[index].coordinateCount);
        platform::ShowLegacyMessage(message,
            "\x8D\xC0\x95\x57\x83\x66\x81\x5B\x83\x5E\x97\xCA"
            "\x82\xAA\x88\xD9\x8F\xED\x82\xC5\x82\xB7\x81\x42", 0);
        return 0;
    }
    const std::int32_t graphicBase = gGraphicStartIndices[fileIndex];
    platform::SeekLegacyStream_53BC41(file,
        static_cast<std::int32_t>(gAnimateRecords9A4A58[index].coordinateOffset), 0);
    gAnimateRecords9A4A58[index].coordinates = static_cast<detail::CoordinateEntry*>(
        std::malloc(sizeof(detail::CoordinateEntry) * gAnimateRecords9A4A58[index].coordinateCount));
    if (gAnimateRecords9A4A58[index].coordinates == nullptr) {
        return 0;
    }
    detail::CoordinateEntry buffer[1000];
    platform::ReadLegacyStream_53BCCD(buffer, 8, gAnimateRecords9A4A58[index].coordinateCount, file);
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

void CoordLoadInit_45C040(char* const* const dataPaths, char* const* const infoPaths)
{
    detail::gCoordinateFiles9A2988.fill(nullptr);
    for (std::size_t slot = 0; slot < 9; ++slot) {
        if (detail::gCoordinateFiles9A2988[slot] != nullptr) {
            continue;
        }
        CoordLoad_45BF60(&detail::gCoordinateFiles9A2988[slot],
            dataPaths[slot], infoPaths[slot]);
        for (std::size_t later = slot + 1; later < 9; ++later) {
            if (platform::CompareInsensitive_53CF50(dataPaths[later], dataPaths[slot]) == 0) {
                std::strcpy(dataPaths[later], dataPaths[slot]);
                std::strcpy(infoPaths[later], infoPaths[slot]);
                detail::gCoordinateFiles9A2988[later] = detail::gCoordinateFiles9A2988[slot];
            }
        }
    }
}

std::int32_t CoordLoad_45BF60(
    platform::LegacyFile** const outputFile, const char* const dataPath, const char* const infoPath)
{
    *outputFile = platform::OpenLegacyStream_53B2F1(dataPath, "rb");
    if (*outputFile == nullptr) {
        return 0;
    }
    platform::LegacyFile* const infoFile = platform::OpenLegacyStream_53B2F1(infoPath, "rb");
    if (infoFile == nullptr) {
        return 0;
    }

    // 原 fread 覆盖路径参数低字节；未读到字节时保留实际地址低位，不补零。
    auto version = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(dataPath));
    platform::ReadLegacyStream_53BCCD(&version, 1, 1, infoFile);
    if (version != 1) {
        return 0;
    }

    std::uint32_t record[2];
    std::uint16_t count;
    while ((static_cast<std::uint8_t>(infoFile->flags) & 0x10u) == 0) {
        platform::ReadLegacyStream_53BCCD(record, 4, 2, infoFile);
        platform::ReadLegacyStream_53BCCD(&count, 2, 1, infoFile);
        const std::int32_t index = GetIndex_45B400(record[0] - 100000u);
        if (index >= 0) {
            gAnimateRecords9A4A58[index].coordinateOffset = record[1];
            gAnimateRecords9A4A58[index].coordinateCount = count;
        }
    }
    platform::CloseLegacyStream_53B1D0(infoFile);
    return 1;
}

} // namespace cg::resource::animation
