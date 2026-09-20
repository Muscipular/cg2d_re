#include "cg/GraphicInfo.hpp"

namespace cg::resource {

std::int32_t gMaxGraphicCount = 0;
GraphicInfo* gGraphicInfo = nullptr;
std::int32_t* gGraphicMapNoIndex = nullptr;

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

std::int32_t ProbePaletteCapability_45C970()
{
    constexpr std::int32_t kGraphicNos[5] {
        99657,
        245151,
        22311,
        22212,
        22002,
    };

    std::int32_t mappedGraphicNo = 0;
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
