#include "cg/resource/GraphicInfo.hpp"

#include "cg/resource/GraphicRead.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/text/Encoding.hpp"

#include <algorithm>
#include <array>
#include <format>
#include <string>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1、0x0053BC41、0x0053BCCD、0x0053CB78；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <new>
#include <vector>

namespace cg::resource {

std::int32_t gMaxGraphicCount = 0; // 原全局：0x0103CFA0。
GraphicInfo* gGraphicInfo = nullptr; // 原全局：0x009A2970。
std::int32_t* gGraphicMapNoIndex = nullptr; // 原全局：0x009A2974。
std::int32_t gGraphicStartIndices[BinLoaderMaxGroups] {}; // 原全局：0x0073F100。

// 原 9A2960/73F128：去重值集合与首见记录引用，生命周期按两相邻CRT项配对。
using GraphicMapNumbers = std::vector<std::int32_t>;
using GraphicFirstRecords = std::map<std::int32_t, GraphicInfo*>;

static GraphicMapNumbers* gGraphicMapNumbers = nullptr; // 原全局：0x9A2960；仅指向原入口的唯一静态对象。
static GraphicFirstRecords* gGraphicFirstRecords = nullptr; // 原全局：0x73F128；仅指向原入口的唯一静态对象。

// 原函数：0x004597E0。
void InitializeGraphicMapNumbers_4597E0()
{
    // 原0x004597F0／0x00459820：同一类型按原入口创建，退出由MSVC原生析构承接。
    static GraphicMapNumbers storage{};
    gGraphicMapNumbers = &storage;
}

// 原函数：0x00459850。
void InitializeGraphicFirstRecords_459850()
{
    // 原0x00459860／0x00459890：同一类型按原入口创建，退出由MSVC原生析构承接。
    static GraphicFirstRecords storage{};
    gGraphicFirstRecords = &storage;
}

// 原函数：0x00459960。
// BLM01：按配置组加载完整记录；分配容量、MapNo索引和查询上界分别承接。
std::int32_t GraphicInit_459960(
    const wchar_t* const* graphicPaths, const wchar_t* const* graphicInfoPaths)
{
    CloseGraphicFiles();
    std::array<std::FILE*, BinLoaderMaxGroups> infoFiles {};
    const auto closeInfoFiles = [&infoFiles] {
        for (std::FILE*& file : infoFiles) {
            if (file != nullptr) {
                std::fclose(file);
                file = nullptr;
            }
        }
    };
    const auto fail = [&closeInfoFiles](const std::int32_t error) {
        closeInfoFiles();
        CloseGraphicFiles();
        return error;
    };

    std::uint64_t totalRecords = 0;
    for (std::size_t slot = 0; slot < gBinLoaderGroupCount; ++slot) {
        if (graphicPaths[slot][0] == '\0' || graphicInfoPaths[slot][0] == '\0') {
            continue;
        }
        infoFiles[slot] = _wfopen(graphicInfoPaths[slot], L"rb");
        if (infoFiles[slot] == nullptr) {
            platform::ShowWideMessage(
                L"图形索引文件开启失败：" + text::FromLegacyDisplayWide(graphicInfoPaths[slot]),
                L"资源加载", 0);
            continue;
        }
        gGraphicFiles[slot] = _wfopen(graphicPaths[slot], L"rb");
        if (gGraphicFiles[slot] == nullptr) {
            platform::ShowWideMessage(
                L"图形文件开启失败：" + text::FromLegacyDisplayWide(graphicPaths[slot]),
                L"资源加载", 0);
            std::fclose(infoFiles[slot]);
            infoFiles[slot] = nullptr;
            continue;
        }
        if (::_fseeki64(infoFiles[slot], 0, SEEK_END) != 0) {
            return fail(3);
        }
        const std::int64_t length = ::_ftelli64(infoFiles[slot]);
        if (length < 0 || ::_fseeki64(infoFiles[slot], 0, SEEK_SET) != 0) {
            return fail(3);
        }
        totalRecords += static_cast<std::uint64_t>(length) / sizeof(GraphicInfoRecord);
    }
    // 参考固定+10预留不足以承接32组间隙；每个配置槽均保留原组间+1。
    const std::uint64_t requiredCount = totalRecords
        + std::max<std::size_t>(10, gBinLoaderGroupCount);
    if (requiredCount > static_cast<std::uint64_t>(gBinLoaderGraphicCapacity)) {
        platform::ShowWideMessage(
            std::format(L"MAX_GRAPHIC={},Need={}", gBinLoaderGraphicCapacity, requiredCount),
            L"图形容量不足", 0);
        return fail(3);
    }
    const std::size_t indexBytes = sizeof(std::int32_t)
        * static_cast<std::size_t>(gBinLoaderGraphicCapacity) + 0x28;
    gGraphicMapNoIndex = static_cast<std::int32_t*>(std::malloc(indexBytes));
    if (gGraphicMapNoIndex == nullptr) {
        return fail(-1);
    }
    std::memset(gGraphicMapNoIndex, 0, indexBytes);
    std::memset(gGraphicStartIndices, 0, sizeof(gGraphicStartIndices));
    gGraphicMapNumbers->clear();
    gGraphicFirstRecords->clear();

    std::uint32_t base = 0;
    GraphicInfoRecord record;
    for (std::size_t slot = 0; slot < gBinLoaderGroupCount; ++slot) {
        gGraphicStartIndices[slot] = static_cast<std::int32_t>(base);
        std::uint32_t count = 0;
        if (infoFiles[slot] != nullptr) {
            while (std::fread(&record, sizeof(record), 1, infoFiles[slot]) == 1) {
                const std::uint32_t index = base + record.seqNo;
                GraphicInfo& target = gGraphicInfo[index];
                target.seqNo = index;
                target.offset = static_cast<std::int32_t>(record.offset);
                target.length = record.length;
                target.x = static_cast<std::int16_t>(record.x);
                target.y = static_cast<std::int16_t>(record.y);
                target.w = static_cast<std::int16_t>(record.w);
                target.h = static_cast<std::int16_t>(record.h);
                target.sx = record.sx;
                target.sy = record.sy;
                target.flag = record.flag;
                target.mapNo = record.mapNo;
                target.fileIndex = static_cast<std::int16_t>(slot);
                ++count;
                if (record.mapNo <= 0) {
                    gGraphicMapNoIndex[0] = 0;
                    continue;
                }
                if (record.mapNo < gBinLoaderGraphicCapacity) {
                    gGraphicMapNoIndex[record.mapNo] = static_cast<std::int32_t>(index);
                }
                if (record.mapNo >= static_cast<std::int32_t>(AutoMapColorCapacity)
                    && slot != 0 && slot != 1 && slot != 6 && slot != 8) {
                    target.mapNo = 0;
                }
                if (std::find(gGraphicMapNumbers->begin(), gGraphicMapNumbers->end(),
                        record.mapNo) == gGraphicMapNumbers->end()) {
                    gGraphicMapNumbers->push_back(record.mapNo);
                    gGraphicFirstRecords->emplace(record.mapNo, &target);
                }
            }
        }
        base += count + 1;
    }
    std::sort(gGraphicMapNumbers->begin(), gGraphicMapNumbers->end());
    closeInfoFiles();
    gMaxGraphicCount = static_cast<std::int32_t>(requiredCount);
    return 0;
}

// 原函数：0x0045A710。
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

// 原函数：0x0045A760。
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

// 原函数：0x0045A7B0。
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

// 原函数：0x0045A810。
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

// 原函数：0x0045A860。
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

// 原函数：0x0045A8B0。
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

// 原函数：0x0045A8D0。
std::int32_t GetGraphicMapNo(std::int32_t index)
{
    if (index < 0 || index >= gMaxGraphicCount) {
        return -1;
    }

    return gGraphicInfo[index].mapNo;
}

} // namespace cg::resource
