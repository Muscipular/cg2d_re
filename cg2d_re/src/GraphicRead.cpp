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
