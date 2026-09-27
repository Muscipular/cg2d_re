#include "cg/GraphicRead.hpp"

#include "cg/GraphicDecode.hpp"
#include "cg/GraphicInfo.hpp"

#include <cstdlib>
#include <cstring>

namespace cg::resource {

std::FILE* gGraphicFiles[9] {};
std::uint8_t gGraphicReadBuffer[GraphicScratchCapacity] {};
std::uint8_t gGraphicDecodeBuffer[GraphicScratchCapacity] {};
std::uint8_t* gGraphicDataPtr = nullptr;
std::uint32_t gCurrentGraphicHeight = 0;
std::uint32_t gCurrentGraphicWidth = 0;
std::uint32_t gCurrentGraphicPaletteSize = 0;
std::int32_t gGraphicDataBuffer = 0;

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
    std::memcpy(&info, &gGraphicInfo[index], sizeof(info));
    const std::int16_t fileIndex = gGraphicInfo[index].fileIndex;
    if (gGraphicFiles[fileIndex] == nullptr) {
        return 0;
    }

    // Windows x64 的 long 仍为 32 位，保留原 fseek 偏移宽度及忽略返回值的顺序。
    static_assert(sizeof(long) == 4);
    std::fseek(gGraphicFiles[fileIndex], info.offset, SEEK_SET);
    if (std::fread(gGraphicReadBuffer, info.length, 1, gGraphicFiles[fileIndex]) != 1) {
        return 0;
    }

    *destination = gGraphicDecodeBuffer;
    std::uint32_t length;
    std::uint32_t ignoredPaletteLength;
    if (paletteLength == nullptr) {
        paletteLength = &ignoredPaletteLength;
    }

    return ReadGraphicData(
        gGraphicReadBuffer, destination, width, height, &length, paletteLength) != nullptr ? 1 : 0;
}

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
