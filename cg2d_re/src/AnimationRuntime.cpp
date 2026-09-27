#include "cg/AnimationRuntime.hpp"

#include "AnimationRuntimeState.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/platform/LegacyWindow.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace cg::resource::animation::detail {

std::array<std::uint32_t, 9> gFileCloseFlags9A29D0 {};
std::array<MainSlot, 256> gMainSlots9A29F8 {};
MainSlot gLeftSentinel9A3A00 {};
MainSlot gRightSentinel9A3A10 {};
std::array<std::FILE*, 9> gAnimateFiles9A3A20 {};
std::array<std::FILE*, 9> gCoordinateFiles9A2988 {};
std::array<CacheSlot, 256> gCacheSlots9A3A48 {};
std::uint32_t gActiveCount9A4A68 = 0;
std::uint32_t gActiveBytes9A4A6C = 0;

} // namespace cg::resource::animation::detail

namespace cg::resource::animation {

AnimateRecord* gAnimateRecords9A4A58 = nullptr;
AnimeCacheRecord* gAnimateCache9A4A5C = nullptr;
std::int32_t gAnimateCount9A4A60 = 0;
std::int32_t gMaxAnimateCount9A4A4C = 0;

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
    std::FILE*& file = detail::gAnimateFiles9A3A20[fileIndex];
    std::fseek(file, static_cast<std::int32_t>(gAnimateRecords9A4A58[index].offset), SEEK_SET);
    const std::int32_t graphicBase = gGraphicStartIndices[fileIndex];
    if (gAnimateRecords9A4A58[index].count == 0) {
        return 0;
    }
    gAnimateRecords9A4A58[index].actionList = std::malloc(
        sizeof(detail::ActionEntry) * gAnimateRecords9A4A58[index].count);
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
        std::fread(buffer, gAnimateRecords9A4A58[index].flag >= 1 ? 20 : 12, 1, file);
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
        action->frames = std::malloc(sizeof(detail::FrameEntry) * action->frameCount);
        if (action->frames == nullptr) {
            Sub_45B440(&slot->index);
            return 0;
        }
        slot->value04 += 12u * action->frameCount;
        std::fread(buffer, 10u * action->frameCount, 1, file);
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
    std::FILE*& file = detail::gCoordinateFiles9A2988[fileIndex];
    if (file == nullptr || gAnimateRecords9A4A58[index].actionList == nullptr
        || gAnimateRecords9A4A58[index].coordinates != nullptr
        || gAnimateRecords9A4A58[index].coordinateCount == 0) {
        return 1;
    }
    if (gAnimateRecords9A4A58[index].coordinateCount > 1000) {
        char message[256];
        std::sprintf(message, "%d / 1000", gAnimateRecords9A4A58[index].coordinateCount);
        platform::ShowLegacyMessage(message,
            "\x8D\xC0\x95\x57\x83\x66\x81\x5B\x83\x5E\x97\xCA"
            "\x82\xAA\x88\xD9\x8F\xED\x82\xC5\x82\xB7\x81\x42", 0);
        return 0;
    }
    const std::int32_t graphicBase = gGraphicStartIndices[fileIndex];
    std::fseek(file,
        static_cast<std::int32_t>(gAnimateRecords9A4A58[index].coordinateOffset), SEEK_SET);
    gAnimateRecords9A4A58[index].coordinates = std::malloc(
        sizeof(detail::CoordinateEntry) * gAnimateRecords9A4A58[index].coordinateCount);
    if (gAnimateRecords9A4A58[index].coordinates == nullptr) {
        return 0;
    }
    detail::CoordinateEntry buffer[1000];
    std::fread(buffer, 8, gAnimateRecords9A4A58[index].coordinateCount, file);
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

std::int32_t CoordLoad_45BF60(
    std::FILE** const outputFile, const char* const dataPath, const char* const infoPath)
{
    *outputFile = std::fopen(dataPath, "rb");
    if (*outputFile == nullptr) {
        return 0;
    }
    std::FILE* const infoFile = std::fopen(infoPath, "rb");
    if (infoFile == nullptr) {
        return 0;
    }

    // 原 fread 覆盖路径参数低字节；未读到字节时保留实际地址低位，不补零。
    auto version = static_cast<std::uint8_t>(reinterpret_cast<std::uintptr_t>(dataPath));
    std::fread(&version, 1, 1, infoFile);
    if (version != 1) {
        return 0;
    }

    std::uint32_t record[2];
    std::uint16_t count;
    while (std::feof(infoFile) == 0) {
        std::fread(record, 4, 2, infoFile);
        std::fread(&count, 2, 1, infoFile);
        const std::int32_t index = GetIndex_45B400(record[0] - 100000u);
        if (index >= 0) {
            gAnimateRecords9A4A58[index].coordinateOffset = record[1];
            gAnimateRecords9A4A58[index].coordinateCount = count;
        }
    }
    std::fclose(infoFile);
    return 1;
}

} // namespace cg::resource::animation
