#include "cg/resource/GraphicRead.hpp"


#include "cg/resource/GraphicDecode.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/render/Palette.hpp"
#include "cg/platform/Startup.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include <array>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <new>
#include <type_traits>
#include <vector>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053BC41、0x0053BCCD；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::resource {

std::FILE* gGraphicFiles[9] {}; // 原全局：0x00927EF4。
std::uint8_t gGraphicReadBuffer[GraphicScratchCapacity] {}; // 原全局：0x00833CB0。
std::uint8_t gGraphicDecodeBuffer[GraphicScratchCapacity] {}; // 原全局：0x0073F138。
std::uint8_t* gGraphicDataPtr = nullptr; // 原全局：0x01020EC0。
std::uint32_t gCurrentGraphicHeight = 0; // 原全局：0x01020ED0。
std::uint32_t gCurrentGraphicWidth = 0; // 原全局：0x01020ED4。
std::uint32_t gCurrentGraphicPaletteSize = 0; // 原全局：0x01020ED8。
std::int32_t gGraphicDataBuffer = 0; // 原全局：0x01020EE0。

namespace {

// 原 [58B7E8,58D348)：1752 项 W32；60 段初值逐项核原表一致，展开后仍可写。
struct HiddenPaletteSegment {
    std::int32_t first;
    std::int32_t last;
    std::int32_t mapNo;
};
constexpr HiddenPaletteSegment kHiddenPaletteSegments[] {
    {0, 0, 0},
    {1, 74, 110300},
    {75, 90, 110380},
    {91, 105, 110400},
    {106, 154, 110420},
    {155, 156, 110500},
    {157, 192, 110600},
    {193, 228, 110650},
    {229, 244, 110700},
    {245, 261, 110750},
    {262, 1261, 113000},
    {1262, 1273, 110401},
    {1274, 1274, 110414},
    {1275, 1275, 110504},
    {1276, 1279, 110376},
    {1280, 1288, 110469},
    {1289, 1354, 110520},
    {1355, 1389, 114165},
    {1390, 1392, 110767},
    {1393, 1396, 110505},
    {1397, 1400, 110396},
    {1401, 1406, 110509},
    {1407, 1420, 110586},
    {1421, 1450, 110770},
    {1451, 1475, 114200},
    {1476, 1476, 110599},
    {1477, 1477, 110400},
    {1478, 1480, 104332},
    {1481, 1489, 104344},
    {1490, 1494, 104339},
    {1495, 1534, 104353},
    {1535, 1535, 104400},
    {1536, 1536, 104406},
    {1537, 1537, 104412},
    {1538, 1538, 104418},
    {1539, 1539, 104424},
    {1540, 1540, 104430},
    {1541, 1541, 104436},
    {1542, 1542, 104442},
    {1543, 1543, 104448},
    {1544, 1544, 104454},
    {1545, 1545, 104460},
    {1546, 1546, 104466},
    {1547, 1547, 104472},
    {1548, 1548, 104478},
    {1549, 1549, 104484},
    {1550, 1550, 104490},
    {1551, 1551, 104496},
    {1552, 1552, 104502},
    {1553, 1553, 104508},
    {1554, 1554, 104514},
    {1555, 1564, 104393},
    {1565, 1605, 104520},
    {1606, 1607, 104335},
    {1608, 1673, 104561},
    {1674, 1689, 104669},
    {1690, 1691, 104337},
    {1692, 1733, 104627},
    {1734, 1747, 104685},
    {1748, 1751, 104700},
};
std::array<std::int32_t, 1752> gHiddenPaletteBaseQueue = [] {
    std::array<std::int32_t, 1752> result {};
    for (const auto& segment : kHiddenPaletteSegments) {
        for (std::int32_t index = segment.first; index <= segment.last; ++index) {
            result[static_cast<std::size_t>(index)] =
                segment.mapNo + index - segment.first;
        }
    }
    return result;
}();

// 原 103CF0C 的 0xB04 无指针记录；两个版本、两张 W32 色表和原 BGR 区。
struct IndependentPaletteRecord {
    std::int16_t paletteChoice;
    std::int16_t paletteVersion;
    std::uint32_t originalColors[256];
    std::uint32_t tonedColors[256];
    std::uint8_t sourceBgr[0x300];
};
static_assert(std::is_trivially_copyable_v<IndependentPaletteRecord>);
static_assert(sizeof(IndependentPaletteRecord) == 0xB04);
static_assert(offsetof(IndependentPaletteRecord, originalColors) == 0x04);
static_assert(offsetof(IndependentPaletteRecord, tonedColors) == 0x404);
static_assert(offsetof(IndependentPaletteRecord, sourceBgr) == 0x804);

// 原 103CF1C 的 8B 索引项；仅首引用拓宽，原 +4 标志与后三字节独立保留。
struct IndependentPaletteIndex {
    IndependentPaletteRecord* record;
    std::uint8_t loaded;
    std::uint8_t unknown[3];
};
static_assert(std::is_trivially_copyable_v<IndependentPaletteIndex>);
static_assert(sizeof(IndependentPaletteIndex) == 16);
static_assert(offsetof(IndependentPaletteIndex, loaded) == 8);

// 原 103CF30..103CF4F；短读保留共享头的未覆盖后缀。
struct PaletteDbfHeader {
    std::uint32_t magic;
    std::uint32_t decodedLength;
    std::uint32_t compressedLength;
    std::uint8_t unknown[20];
};
static_assert(sizeof(PaletteDbfHeader) == 0x20);
static_assert(offsetof(PaletteDbfHeader, decodedLength) == 4);
static_assert(offsetof(PaletteDbfHeader, compressedLength) == 8);

// 原 58D360 的可写 W32 比较字；共享头读取后才取样，不并入文件头。
std::uint32_t gPaletteDbfMagic58D360 = 0x00464244u;

// 原 103CF58 有序唯一键表与 103CF68 载荷集合；522C50 不销毁它们。
struct PaletteDbfState {
    PaletteDbfHeader header {};
    std::int32_t count = 0;  // 原 103CF50，最后一次载荷的项数。
    std::int32_t stride = 0; // 原 103CF54，最后一次载荷的原字节步长。
    // 原522860先构造+28索引，再构造+38载荷集合；私有节点由宿主承接。
    std::map<std::int32_t, std::uint8_t*> records;
    std::vector<std::uint8_t*> decodedBlocks;
    std::uint8_t* payload = nullptr; // 原 103CF78，指向解压块 +8。
    std::map<std::int32_t, std::uint8_t*>::iterator current {}; // 原 103CF80。

    ~PaletteDbfState()
    {
        // 原522701..522838：释放载荷 -> 归空集合 -> 清树节点 ->
        // 成员逆序释放集合容量，再销毁已空树/表头，不重复释放载荷。
        std::uint32_t blockIndex = 0;
        while (decodedBlocks.data() != nullptr && blockIndex < decodedBlocks.size()) {
            std::free(decodedBlocks[blockIndex]);
            ++blockIndex;
        }
        decodedBlocks.clear();
        records.clear();
    }
};

using IndependentPaletteQueue = std::vector<std::int32_t>;

// 原103CF88队列与103CF30 DBF为独立owner；实际对象在各原启动入口创建，指针只供访问。
IndependentPaletteQueue* gIndependentPaletteQueue = nullptr; // 原全局：0x103CF88；仅指向原入口的唯一静态对象。
PaletteDbfState* gPaletteDbfState = nullptr; // 原全局：0x103CF30；仅指向原入口的唯一静态对象。

std::int32_t gIndependentPaletteCount = 0; // 原 103CF18，不随池释放清零。
IndependentPaletteIndex* gIndependentPaletteIndex = nullptr;
IndependentPaletteRecord* gIndependentPaletteRecords = nullptr;
std::uint32_t gIndependentPaletteLoadCount = 0; // 原 103CF08，仅 loaded 为零的读取分支递增。

// 原 522900；唯一 caller 弃返回，保留文件、共享头、跨次索引和入队副作用。
void LoadIndependentPaletteIndex_522900()
{
    char directory[256] {};
    char fileName[256] {};
    ::GetCurrentDirectoryA(256, directory);
    if (directory[255] != '\\') {
        std::strcat(directory, "\\");
    }
    std::sprintf(
        fileName, "%s%s\\%s", directory, platform::gDataPath, "CGLoadSpr.dbf");
    std::FILE* const file =
        std::fopen(fileName, "rb");
    if (file == nullptr) {
        return;
    }
    static_cast<std::uint32_t>(std::fread(
        &gPaletteDbfState->header, 1,
        static_cast<std::uint32_t>(sizeof(PaletteDbfHeader)), file));
    const std::uint32_t magic = gPaletteDbfState->header.magic;
    const std::uint32_t expectedMagic = gPaletteDbfMagic58D360;
    if (magic != expectedMagic) {
        return; // 原 magic 不匹配路径没有 fclose。
    }

    auto* compressed = static_cast<std::uint8_t*>(
        std::malloc(gPaletteDbfState->header.compressedLength));
    auto* decoded = static_cast<std::uint8_t*>(
        std::malloc(gPaletteDbfState->header.decodedLength));
    static_cast<std::uint32_t>(std::fread(
        compressed, 1, static_cast<std::uint32_t>(gPaletteDbfState->header.compressedLength), file));
    static_cast<void>(DecompressDbf_53AF90(
        compressed, static_cast<std::int32_t>(gPaletteDbfState->header.compressedLength),
        decoded, &gPaletteDbfState->header.decodedLength));
    std::free(compressed);
    gPaletteDbfState->decodedBlocks.push_back(decoded);
    gPaletteDbfState->payload = decoded + 8;

    const auto readWord = [](const std::uint8_t* const bytes) {
        std::int32_t value;
        std::memcpy(&value, bytes, sizeof(value));
        return value;
    };
    for (std::int32_t index = 0; index < readWord(decoded); ++index) {
        const std::int32_t stride = readWord(decoded + 4);
        std::uint8_t* const payload = gPaletteDbfState->payload;
        const std::int32_t offset = stride * index;
        std::uint8_t* const record = payload + offset;
        const auto inserted = gPaletteDbfState->records.try_emplace(readWord(record), nullptr);
        inserted.first->second = record;
    }
    gPaletteDbfState->count = readWord(decoded);
    gPaletteDbfState->stride = readWord(decoded + 4);
    std::fclose(file);

    gPaletteDbfState->current = gPaletteDbfState->records.begin();
    while (gPaletteDbfState->current != gPaletteDbfState->records.end()) {
        const auto current = gPaletteDbfState->current++;
        const std::uint8_t* const record = current->second;
        if (record == nullptr) {
            break;
        }
        gIndependentPaletteQueue->push_back(readWord(record + 4));
    }
}

// 原 522CB0 的 32 位分支；loaded 只读，不自行置位或增加空记录恢复。
void LoadIndependentPalette32_522CB0(const std::int32_t index)
{
    if (index < 0 || index >= gIndependentPaletteCount) {
        return;
    }
    if (gIndependentPaletteIndex[index].loaded == 0) {
        std::int32_t imageNo;
        GetGraphicMapNoIndex((*gIndependentPaletteQueue)[index], &imageNo);
        std::uint32_t paletteLength;
        if (ReadGraphicDataByIndex(
                imageNo, &gGraphicDataPtr, &gCurrentGraphicWidth,
                &gCurrentGraphicHeight, &paletteLength) == 0) {
            IndependentPaletteIndex* const recordIndex = gIndependentPaletteIndex;
            gGraphicDataBuffer = -1;
            std::memset(recordIndex[index].record->sourceBgr, 0, 0x300);
        } else {
            const std::uint32_t width = gCurrentGraphicWidth;
            std::uint8_t* const data = gGraphicDataPtr;
            const std::uint32_t pixelCount = width * gCurrentGraphicHeight;
            const std::uint32_t sourceLength = paletteLength;
            const std::uint8_t* const sourceBgr = data + pixelCount;
            IndependentPaletteIndex* const recordIndex = gIndependentPaletteIndex;
            gGraphicDataBuffer = imageNo;
            std::memcpy(recordIndex[index].record->sourceBgr, sourceBgr, sourceLength);
            if (paletteLength < 0x300) {
                // 原 522D89..522DBF：起点向下取整到 BGR 边界，长度仍是 768-L。
                std::memset(
                    gIndependentPaletteIndex[index].record->sourceBgr + (paletteLength / 3) * 3,
                    0, 0x300 - paletteLength);
            }
        }

        std::uint32_t* const originalColors = gIndependentPaletteIndex[index].record->originalColors;
        originalColors[0] = 0;
        for (std::int32_t colorIndex = 1; colorIndex < 256; ++colorIndex) {
            const IndependentPaletteRecord* const sourceRecord = gIndependentPaletteIndex[index].record;
            const std::uint32_t red = sourceRecord->sourceBgr[colorIndex * 3 + 2];
            const std::uint32_t blue = sourceRecord->sourceBgr[colorIndex * 3];
            const std::uint32_t green = sourceRecord->sourceBgr[colorIndex * 3 + 1];
            const std::uint32_t color = (red << 16) | (green << 8) | blue;
            originalColors[colorIndex] = color;
            if (color == 0) {
                originalColors[colorIndex] = 1;
            }
        }
        ++gIndependentPaletteLoadCount;
    }

    std::uint32_t* const tonedColors = gIndependentPaletteIndex[index].record->tonedColors;
    tonedColors[0] = 0;
    for (std::int32_t colorIndex = 1; colorIndex < 256; ++colorIndex) {
        IndependentPaletteIndex* const sourceIndex = gIndependentPaletteIndex;
        std::uint8_t* const redMap = render::gColorChannelMaps.red;
        const IndependentPaletteRecord* const sourceRecord = sourceIndex[index].record;
        const std::uint32_t red = redMap[sourceRecord->sourceBgr[colorIndex * 3 + 2]];
        std::uint8_t* const greenMap = render::gColorChannelMaps.green;
        const std::uint32_t green = greenMap[sourceRecord->sourceBgr[colorIndex * 3 + 1]];
        const std::uint8_t blueIndex = sourceRecord->sourceBgr[colorIndex * 3];
        std::uint8_t* const blueMap = render::gColorChannelMaps.blue;
        const std::uint32_t blue = blueMap[blueIndex];
        const std::uint32_t color = (red << 16) | (green << 8) | blue;
        tonedColors[colorIndex] = color;
        if (color == 0) {
            tonedColors[colorIndex] = 1;
        }
    }
    IndependentPaletteIndex* versionIndex = gIndependentPaletteIndex;
    const std::int16_t paletteChoice = static_cast<std::int16_t>(render::gActivePalette);
    versionIndex[index].record->paletteChoice = paletteChoice;
    versionIndex = gIndependentPaletteIndex;
    const std::int16_t paletteVersion = static_cast<std::int16_t>(render::gPaletteFrameVersion);
    versionIndex[index].record->paletteVersion = paletteVersion;
}

} // namespace

// 原函数：0x00522620。
void InitializeIndependentPaletteQueue_522620()
{
    // 原0x00522630／0x00522660：同一类型按原入口创建，退出由MSVC原生析构承接。
    static IndependentPaletteQueue storage{};
    gIndependentPaletteQueue = &storage;
}

// 原函数：0x00522690。
void InitializePaletteDbfState_522690()
{
    // 原0x005226A0／0x005226C0：同一类型按原入口创建，退出由MSVC原生析构承接。
    static PaletteDbfState storage{};
    gPaletteDbfState = &storage;
}

// 原函数：0x0053AF90。
std::int32_t DecompressDbf_53AF90(
    const std::uint8_t* const source,
    const std::int32_t sourceLength,
    std::uint8_t* const destination,
    std::uint32_t* const decodedLength) noexcept
{
    *decodedLength = 0;
    const std::uint8_t* cursor = source;
    const std::uint8_t* const end = source + sourceLength;
    std::uint8_t* output = destination;

    const auto copyLiteral = [&](std::uint32_t count) noexcept {
        while (count != 0) {
            *output++ = *cursor++;
            --count;
        }
    };
    const auto copyMatch = [&](const std::uint32_t distance, std::uint32_t count) noexcept {
        std::uint8_t* match = output - distance;
        while (count != 0) {
            *output++ = *match++;
            --count;
        }
    };
    const auto extendedCount = [&](const std::uint32_t base) noexcept {
        std::uint32_t count = 0;
        while (*cursor == 0) {
            count += 255;
            ++cursor;
        }
        return count + *cursor++ + base;
    };
    const auto offset16 = [&]() noexcept {
        const std::uint32_t value =
            static_cast<std::uint32_t>(cursor[0])
            | (static_cast<std::uint32_t>(cursor[1]) << 8);
        cursor += 2;
        return value;
    };

    enum class Phase { LiteralRun, AfterLiteral, Match };
    std::uint32_t token = *cursor++;
    Phase phase = Phase::LiteralRun;
    if (token > 17) {
        const std::uint32_t count = token - 17;
        copyLiteral(count);
        token = *cursor++;
        phase = count >= 4 ? Phase::AfterLiteral : Phase::Match;
    }

    for (;;) {
        if (phase == Phase::LiteralRun && token < 16) {
            const std::uint32_t count =
                (token != 0 ? token : extendedCount(15)) + 3;
            copyLiteral(count);
            token = *cursor++;
            phase = Phase::AfterLiteral;
            continue;
        }

        std::uint32_t distance;
        std::uint32_t count;
        if (phase == Phase::AfterLiteral && token < 16) {
            distance = 2049 + (token >> 2) + 4 * *cursor++;
            count = 3;
        } else if (token >= 64) {
            distance = 1 + ((token >> 2) & 7) + 8 * *cursor++;
            count = (token >> 5) + 1;
        } else if (token >= 32) {
            const std::uint32_t length = token & 31;
            count = (length != 0 ? length : extendedCount(31)) + 2;
            distance = 1 + (offset16() >> 2);
        } else if (token >= 16) {
            const std::uint32_t length = token & 7;
            count = (length != 0 ? length : extendedCount(7)) + 2;
            const std::uint32_t offset =
                ((token & 8) << 11) + (offset16() >> 2);
            if (offset == 0) {
                *decodedLength = static_cast<std::uint32_t>(output - destination);
                if (cursor == end) {
                    return 0;
                }
                return cursor < end ? -8 : -4;
            }
            distance = 0x4000 + offset;
        } else {
            distance = 1 + (token >> 2) + 4 * *cursor++;
            count = 2;
        }

        copyMatch(distance, count);
        const std::uint32_t trailing = cursor[-2] & 3;
        copyLiteral(trailing);
        token = *cursor++;
        phase = trailing != 0 ? Phase::Match : Phase::LiteralRun;
    }
}

// 原函数：0x00522B30。
std::int32_t InitializeIndependentPalettes_522B30()
{
    gIndependentPaletteQueue->clear();
    for (const std::int32_t mapNo : gHiddenPaletteBaseQueue) {
        gIndependentPaletteQueue->push_back(mapNo);
    }
    LoadIndependentPaletteIndex_522900();
    const std::int32_t queueCount =
        static_cast<std::int32_t>(gIndependentPaletteQueue->size());
    gIndependentPaletteCount = queueCount;

    auto* const allocatedIndex = static_cast<IndependentPaletteIndex*>(
        ::GlobalLock(::GlobalAlloc(
            GPTR, sizeof(IndependentPaletteIndex) * queueCount)));
    const std::int32_t clearCount = gIndependentPaletteCount;
    gIndependentPaletteIndex = allocatedIndex;
    std::memset(allocatedIndex, 0, sizeof(IndependentPaletteIndex) * clearCount);

    const std::int32_t recordCount = gIndependentPaletteCount;
    auto* const allocatedRecords = static_cast<IndependentPaletteRecord*>(
        ::GlobalLock(::GlobalAlloc(
            GPTR, sizeof(IndependentPaletteRecord) * recordCount)));
    const std::int32_t initialCount = gIndependentPaletteCount;
    gIndependentPaletteRecords = allocatedRecords;

    std::int32_t index = 0;
    if (initialCount > 0) {
        for (;;) {
            IndependentPaletteRecord* const records = gIndependentPaletteRecords;
            IndependentPaletteIndex* const indexTable = gIndependentPaletteIndex;
            IndependentPaletteRecord* const record = &records[index];
            indexTable[index].record = record;
            gIndependentPaletteIndex[index].loaded = 0;
            const std::int32_t nextCount = gIndependentPaletteCount;
            ++index;
            if (index >= nextCount) {
                break;
            }
        }
    }
    return index;
}

// 原函数：0x00522C50。
void ReleaseIndependentPalettes_522C50()
{
    ::GlobalUnlock(::GlobalHandle(gIndependentPaletteRecords));
    ::GlobalFree(::GlobalHandle(gIndependentPaletteRecords));
    IndependentPaletteIndex* const index = gIndependentPaletteIndex;
    gIndependentPaletteRecords = nullptr;
    ::GlobalUnlock(::GlobalHandle(index));
    ::GlobalFree(::GlobalHandle(gIndependentPaletteIndex));
    gIndependentPaletteIndex = nullptr;
}

// 原函数：0x00523140。
std::uint8_t RefreshIndependentPalette32_523140(std::int16_t& index)
{
    if (index <= 0 || index >= gIndependentPaletteCount) {
        index = 0;
        return 2;
    }
    const IndependentPaletteRecord* const record = gIndependentPaletteIndex[index].record;
    if (record != nullptr
        && record->paletteChoice == render::gActivePalette
        && record->paletteVersion == render::gPaletteFrameVersion) {
        return 0;
    }
    LoadIndependentPalette32_522CB0(index);
    return 1;
}

void GetIndependentPaletteColors32(
    const std::int16_t index,
    const std::uint32_t*& originalColors,
    const std::uint32_t*& tonedColors)
{
    const IndependentPaletteRecord* const record = gIndependentPaletteIndex[index].record;
    if (record == nullptr) {
        originalColors = nullptr;
        tonedColors = nullptr;
        return;
    }
    originalColors = record->originalColors;
    tonedColors = record->tonedColors;
}

// 原函数：0x0045A900。
int ReadGraphicDataByIndex(
    std::int32_t index,
    std::uint8_t** destination,
    std::uint32_t* width,
    std::uint32_t* height,
    std::uint32_t* paletteLength)
{
    if (index < 0 || index >= gMaxGraphicCount) {
        return 0;
    }

    GraphicInfo info;
    const GraphicInfo* const record = &gGraphicInfo[index];
    std::memcpy(&info, record, sizeof(info));
    const std::int16_t fileIndex = record->fileIndex;
    std::FILE* const file = gGraphicFiles[fileIndex];
    if (file == nullptr) {
        return 0;
    }

    // 原45A94D保留S32偏移，忽略定位结果；读取重新取同一共享文件槽。
    std::fseek(file, static_cast<std::int32_t>(info.offset), 0);
    std::FILE* const readFile = gGraphicFiles[fileIndex];
    const std::uint32_t sourceLength = info.length;
    if (static_cast<std::uint32_t>(std::fread(gGraphicReadBuffer, static_cast<std::uint32_t>(sourceLength), 1, readFile)) != 1) {
        return 0;
    }

    const bool hasPaletteLength = paletteLength != nullptr;
    *destination = gGraphicDecodeBuffer;
    std::uint32_t length;
    std::uint32_t ignoredPaletteLength;
    if (!hasPaletteLength) {
        paletteLength = &ignoredPaletteLength;
    }

    return ReadGraphicData(
        gGraphicReadBuffer, destination, width, height, &length, paletteLength) != nullptr ? 1 : 0;
}

// 原函数：0x00451D80。
std::int32_t GetGraphicAutoMapColor_451D80(
    const std::int32_t index, const render::PaletteEntry* const palette)
{
    std::uint32_t red = 0;
    std::uint32_t green = 0;
    std::uint32_t blue = 0;
    std::uint32_t count = 0;
    const std::int32_t loaded = ReadGraphicDataByIndex(
        index, &gGraphicDataPtr, &gCurrentGraphicWidth, &gCurrentGraphicHeight, nullptr);
    if (loaded == 0) {
        gGraphicDataBuffer = -1;
        return loaded;
    }
    const std::int32_t height = std::bit_cast<std::int32_t>(gCurrentGraphicHeight);
    gGraphicDataBuffer = index;
    if (height <= 0) {
        return 0;
    }

    const std::uint8_t* row = gGraphicDataPtr;
    std::int32_t width = std::bit_cast<std::int32_t>(gCurrentGraphicWidth);
    for (std::int32_t rowIndex = 0; rowIndex < height; ++rowIndex) {
        std::int32_t column = 0;
        while (column < width) {
            const std::uint8_t colorIndex = row[column];
            if (colorIndex != 0) {
                red += palette[colorIndex].red;
                green += palette[colorIndex].green;
                const std::uint32_t blueContribution = palette[colorIndex].blue;
                const std::uint32_t accumulatedBlue = blue + blueContribution;
                ++count;
                blue = accumulatedBlue;
            }
            width = std::bit_cast<std::int32_t>(gCurrentGraphicWidth);
            ++column;
        }
        row += width;
    }
    if (count == 0) {
        return 0;
    }
    const std::uint32_t averageBlue = static_cast<std::uint8_t>(blue / count);
    std::uint32_t blueGreen = averageBlue << 8;
    const std::uint32_t averageGreen = static_cast<std::uint8_t>(green / count);
    blueGreen |= averageGreen;
    const std::uint32_t averageRed = static_cast<std::uint8_t>(red / count);
    const std::uint32_t color = (blueGreen << 8) | averageRed;
    return render::FindNearestPaletteColor_451E90(color, palette, 256);
}

// 原函数：0x0045A6C0。
void CloseGraphicFiles()
{
    if (gGraphicMapNoIndex != nullptr) {
        std::free(gGraphicMapNoIndex);
        gGraphicMapNoIndex = nullptr;
    }
    for (std::FILE*& file : gGraphicFiles) {
        if (file != nullptr) {
            std::fclose(file);
            file = nullptr;
        }
    }
}

} // namespace cg::resource
