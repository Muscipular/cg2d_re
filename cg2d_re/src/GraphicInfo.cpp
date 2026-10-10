#include "cg/GraphicInfo.hpp"

#include "platform/LegacyFormat.hpp"
#include "platform/LegacyStdio.hpp"

#include "cg/GraphicRead.hpp"
#include "cg/platform/LegacyWindow.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <new>
#include <vector>

namespace cg::resource {

std::int32_t gMaxGraphicCount = 0;
GraphicInfo* gGraphicInfo = nullptr;
std::int32_t* gGraphicMapNoIndex = nullptr;
std::int32_t gGraphicStartIndices[9] {};

namespace {

// 原 9A2068/83339C；与 ResourcePaths 的 256B 路径槽不同，缓存步长仍是 255B。
char gGraphicCachedPaths[9][255] {};
char gGraphicInfoCachedPaths[9][255] {};
platform::LegacyFile* gGraphicInfoFile = nullptr; // 原 927EF0；fclose 后原端不清槽。
std::int32_t gGraphicRequiredCount = 0; // 原 833C9C，预扫总字节数/40+10。

// 原 9A2960/73F128：去重值集合与首见记录引用，生命周期按两相邻CRT项配对。
using GraphicMapNumbers = std::vector<std::int32_t>;
using GraphicFirstRecords = std::map<std::int32_t, GraphicInfo*>;

alignas(GraphicMapNumbers)
std::byte gGraphicMapNumbersStorage[sizeof(GraphicMapNumbers)] {};
alignas(GraphicFirstRecords)
std::byte gGraphicFirstRecordsStorage[sizeof(GraphicFirstRecords)] {};

GraphicMapNumbers& BorrowGraphicMapNumbers() noexcept
{
    return *std::launder(
        reinterpret_cast<GraphicMapNumbers*>(gGraphicMapNumbersStorage));
}

GraphicFirstRecords& BorrowGraphicFirstRecords() noexcept
{
    return *std::launder(
        reinterpret_cast<GraphicFirstRecords*>(gGraphicFirstRecordsStorage));
}

// 原4597F0的返回未消费；固定构造同一实际vector，不报告旧指针残值。
void ConstructGraphicMapNumbers_4597F0()
{
    static_cast<void>(std::construct_at(
        reinterpret_cast<GraphicMapNumbers*>(gGraphicMapNumbersStorage)));
}

void DestroyGraphicMapNumbers_459820()
{
    std::destroy_at(&BorrowGraphicMapNumbers());
}

GraphicFirstRecords* ConstructGraphicFirstRecords_459860()
{
    return std::construct_at(
        reinterpret_cast<GraphicFirstRecords*>(gGraphicFirstRecordsStorage));
}

// 原459890：节点/表头归宿主，GraphicInfo*仍借用，不释放主表。
void DestroyGraphicFirstRecords_459890()
{
    std::destroy_at(&BorrowGraphicFirstRecords());
}

} // namespace

std::int32_t InitializeGraphicMapNumbers_4597E0()
{
    ConstructGraphicMapNumbers_4597F0();
    return std::atexit(DestroyGraphicMapNumbers_459820);
}

std::int32_t InitializeGraphicFirstRecords_459850()
{
    static_cast<void>(ConstructGraphicFirstRecords_459860());
    return std::atexit(DestroyGraphicFirstRecords_459890);
}

std::int32_t GraphicInit_459960(
    const char* const* graphicPaths, const char* const* graphicInfoPaths)
{
    for (platform::LegacyFile*& file : gGraphicFiles) {
        file = nullptr;
    }

    // 原 459B5B 的共用错误出口只关闭数据文件，不释放旧 mapNo 索引或主表。
    const auto failAndCloseData = [](const std::int32_t slot) {
        for (platform::LegacyFile*& file : gGraphicFiles) {
            if (file != nullptr) {
                platform::CloseLegacyStream_53B1D0(file);
                file = nullptr;
            }
        }
        if (slot == 0) {
            return 1;
        }
        if (slot == 3) {
            return 2;
        }
        return slot == 6 || slot == 7 || slot == 8 ? 7 : 3;
    };

    for (std::int32_t slot = 0; slot < 9; ++slot) {
        std::strcpy(gGraphicCachedPaths[slot], graphicPaths[slot]);
        gGraphicFiles[slot] = platform::OpenLegacyStream_53B2F1(gGraphicCachedPaths[slot], "rb");
        if (gGraphicFiles[slot] == nullptr
            && (slot == 0 || slot == 3 || slot == 6 || slot == 7 || slot == 8)) {
            return failAndCloseData(slot);
        }
    }

    std::uint32_t totalInfoBytes = 0;
    for (std::int32_t slot = 0; slot < 9; ++slot) {
        std::strcpy(gGraphicInfoCachedPaths[slot], graphicInfoPaths[slot]);
        gGraphicInfoFile = platform::OpenLegacyStream_53B2F1(gGraphicInfoCachedPaths[slot], "rb");
        if (gGraphicInfoFile != nullptr) {
            platform::SeekLegacyStream_53BC41(gGraphicInfoFile, 0, 2);
            totalInfoBytes += static_cast<std::uint32_t>(platform::TellLegacyStream_53CB78(gGraphicInfoFile));
            platform::CloseLegacyStream_53B1D0(gGraphicInfoFile);
        } else {
            if (slot == 0) {
                return 1;
            }
            if (slot == 3) {
                return 2;
            }
            if (slot == 6 || slot == 7 || slot == 8) {
                return 7;
            }
        }
    }
    gGraphicRequiredCount = static_cast<std::int32_t>(totalInfoBytes / 40u + 10u);
    if (gGraphicRequiredCount >= gMaxGraphicCount) {
        char message[256];
        platform::FormatLegacyString_53B304(message, "MAX_GRAPHICS=%d,Need=%d",
            gMaxGraphicCount, gGraphicRequiredCount);
        platform::ShowLegacyMessage(
            message, "sprmgr: MAX_GRAPHICS \xB2\xBB\x89\xF2\xA1\xA3", 0);
        return failAndCloseData(9);
    }

    gGraphicMapNoIndex = static_cast<std::int32_t*>(std::malloc(
        sizeof(std::int32_t) * static_cast<std::size_t>(gMaxGraphicCount) + 0x28));
    if (gGraphicMapNoIndex == nullptr) {
        return -1;
    }
    std::memset(
        gGraphicMapNoIndex, 0,
        sizeof(std::int32_t) * static_cast<std::size_t>(gMaxGraphicCount));
    BorrowGraphicMapNumbers().clear();
    BorrowGraphicFirstRecords().clear();

    GraphicInfoRecord record; // 原 40B 栈槽不预清零；跨次 fread 保留未覆盖字节。
    std::int32_t processed = 0;
    std::int32_t base = 0;
    for (std::int32_t slot = 0; slot < 9; ++slot) {
        gGraphicStartIndices[slot] = base;
        gGraphicInfoFile = platform::OpenLegacyStream_53B2F1(gGraphicInfoCachedPaths[slot], "rb");
        if (gGraphicInfoFile == nullptr) {
            if (slot == 0) {
                return 1;
            }
            if (slot == 3) {
                return 2;
            }
            if (slot == 6 || slot == 7 || slot == 8) {
                return 7;
            }
            continue;
        }
        // 原459CCE/459F1D只检查flags低U8的EOF位，短读仍消费同一记录。
        while ((static_cast<std::uint8_t>(gGraphicInfoFile->flags) & 0x10u) == 0) {
            platform::ReadLegacyStream_53BCCD(&record, 0x28, 1, gGraphicInfoFile);
            const std::uint32_t sequenceNo = record.seqNo;
            GraphicInfo* recordTable = gGraphicInfo;
            ++processed;
            const std::uint32_t index = sequenceNo + static_cast<std::uint32_t>(base);
            recordTable[index].seqNo = index;
            recordTable = gGraphicInfo;
            recordTable[index].offset = static_cast<std::int32_t>(record.offset);
            recordTable = gGraphicInfo;
            recordTable[index].length = record.length;
            recordTable = gGraphicInfo;
            recordTable[index].x = static_cast<std::int16_t>(record.x);
            recordTable = gGraphicInfo;
            recordTable[index].y = static_cast<std::int16_t>(record.y);
            recordTable = gGraphicInfo;
            recordTable[index].w = static_cast<std::int16_t>(record.w);
            recordTable = gGraphicInfo;
            recordTable[index].h = static_cast<std::int16_t>(record.h);
            recordTable = gGraphicInfo;
            recordTable[index].sx = record.sx;
            recordTable = gGraphicInfo;
            recordTable[index].sy = record.sy;
            recordTable = gGraphicInfo;
            recordTable[index].flag = record.flag;
            recordTable = gGraphicInfo;
            recordTable[index].mapNo = record.mapNo;
            recordTable = gGraphicInfo;
            recordTable[index].fileIndex = static_cast<std::int16_t>(slot);
            std::int32_t lookupMapNo = record.mapNo;
            if (lookupMapNo == 0) {
                gGraphicMapNoIndex[0] = 0;
                continue;
            }
            if (static_cast<std::uint32_t>(lookupMapNo)
                < static_cast<std::uint32_t>(gMaxGraphicCount)) {
                gGraphicMapNoIndex[lookupMapNo] = static_cast<std::int32_t>(index);
                lookupMapNo = record.mapNo;
            }
            auto& mapNumbers = BorrowGraphicMapNumbers();
            const auto end = mapNumbers.end();
            const auto begin = mapNumbers.begin();
            if (std::find(begin, end, lookupMapNo) == end) {
                mapNumbers.push_back(lookupMapNo);
                GraphicInfo* const firstRecord = &gGraphicInfo[index];
                const std::int32_t firstMapNo = record.mapNo;
                BorrowGraphicFirstRecords().emplace(firstMapNo, firstRecord);
            }
        }
        std::sort(BorrowGraphicMapNumbers().begin(), BorrowGraphicMapNumbers().end());
        platform::CloseLegacyStream_53B1D0(gGraphicInfoFile);
        base = processed;
    }
    return 0;
}

std::int32_t GetGraphicXY(std::uint32_t index, std::int16_t* x, std::int16_t* y)
{
    if (index < static_cast<std::uint32_t>(gMaxGraphicCount)) {
        *x = gGraphicInfo[index].x;
        // 原 0x45A733：写入第一个输出后再从全局表读取第二个字段。
        *y = gGraphicInfo[index].y;
        return 1;
    }

    *x = 0;
    *y = 0;
    return 0;
}

std::int32_t GetGraphicWH(std::uint32_t index, std::int16_t* w, std::int16_t* h)
{
    if (index < static_cast<std::uint32_t>(gMaxGraphicCount)) {
        *w = gGraphicInfo[index].w;
        // 原 0x45A783：保留首个输出写入后的第二次读表。
        *h = gGraphicInfo[index].h;
        return 1;
    }

    *w = 0;
    *h = 0;
    return 0;
}

std::int32_t GetGraphicSxSy(std::uint32_t index, std::uint16_t* sx, std::uint16_t* sy)
{
    if (index < static_cast<std::uint32_t>(gMaxGraphicCount)) {
        *sx = static_cast<std::uint16_t>(gGraphicInfo[index].sx);
        // 原 0x45A7D4/0x45A7DE：首个输出写入后重新读表，并对 sy 零扩展。
        *sy = static_cast<std::uint16_t>(gGraphicInfo[index].sy);
        return 1;
    }

    *sx = 0;
    *sy = 0;
    return 0;
}

std::int32_t GetGraphicFlag1(std::uint32_t index, std::uint16_t* flag)
{
    if (index < static_cast<std::uint32_t>(gMaxGraphicCount)) {
        *flag = static_cast<std::uint16_t>(
            static_cast<std::uint32_t>(gGraphicInfo[index].flag) % 100u);
        return 1;
    }

    *flag = 0;
    return 0;
}

std::int32_t GetGraphicFlag2(std::uint32_t index, std::uint16_t* flag)
{
    if (index < static_cast<std::uint32_t>(gMaxGraphicCount)) {
        *flag = static_cast<std::uint16_t>(
            static_cast<std::uint32_t>(gGraphicInfo[index].flag) / 100u);
        return 1;
    }

    *flag = 0;
    return 0;
}

std::int32_t GetGraphicMapNoIndex(std::int32_t index, std::int32_t* outSeqNo)
{
    *outSeqNo = gGraphicMapNoIndex[index];
    return 1;
}

// PCR01：原45C970完整62条；出参原不预清，两轮同命中index重查，-1/4-index返回保持。
std::int32_t ProbePaletteCapability_45C970()
{
    constexpr std::int32_t kGraphicNos[5] {
        99657,
        245151,
        22311,
        22212,
        22002,
    };

    std::int32_t mappedGraphicNo;
    std::int32_t candidateIndex = 0;
    for (; candidateIndex < 5; ++candidateIndex) {
        GetGraphicMapNoIndex(kGraphicNos[candidateIndex], &mappedGraphicNo);
        if (mappedGraphicNo != 0) {
            break;
        }
    }

    if (candidateIndex >= 5) {
        return -1;
    }

    // 原 0x45C9D2：从首轮命中的同一索引重新读取，不能复用首轮输出。
    for (; candidateIndex < 5; ++candidateIndex) {
        GetGraphicMapNoIndex(kGraphicNos[candidateIndex], &mappedGraphicNo);
        if (mappedGraphicNo != 0) {
            return 4 - candidateIndex;
        }
    }

    return -1;
}

std::int32_t GetGraphicMapNo(std::int32_t index)
{
    if (index < 0 || index >= gMaxGraphicCount) {
        return -1;
    }

    return gGraphicInfo[index].mapNo;
}

} // namespace cg::resource
