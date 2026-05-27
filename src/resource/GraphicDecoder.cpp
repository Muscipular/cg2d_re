#include "resource/GraphicDecoder.hpp"

#include "resource/BinaryReader.hpp"

#include <cstddef>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

namespace cg::resource {
namespace {

constexpr std::size_t BaseHeaderSize = 16;
constexpr std::size_t CgexPaletteSizeFieldSize = 4;
constexpr std::size_t BytesPerColor = 3;

[[nodiscard]] GraphicDecodeResult MakeError(GraphicDecodeError error, std::string message)
{
    GraphicDecodeResult result;
    result.error = error;
    result.message = std::move(message);
    return result;
}

template <typename T>
[[nodiscard]] bool TryToU64(T value, std::uint64_t& output)
{
    if constexpr (std::is_signed_v<T>) {
        if (value < 0) {
            return false;
        }
    }

    output = static_cast<std::uint64_t>(value);
    return true;
}

[[nodiscard]] bool TryReadU32Le(
    std::span<const std::uint8_t> bytes,
    std::size_t offset,
    std::uint32_t& value)
{
    if (offset > bytes.size() || bytes.size() - offset < sizeof(std::uint32_t)) {
        return false;
    }

    value = static_cast<std::uint32_t>(bytes[offset])
        | (static_cast<std::uint32_t>(bytes[offset + 1]) << 8)
        | (static_cast<std::uint32_t>(bytes[offset + 2]) << 16)
        | (static_cast<std::uint32_t>(bytes[offset + 3]) << 24);
    return true;
}

[[nodiscard]] bool TryParseHeader(
    std::span<const std::uint8_t> block,
    GraphicBlockHeader& header,
    std::size_t& headerSize)
{
    if (block.size() < BaseHeaderSize) {
        return false;
    }
    if (block[0] != 'R' || block[1] != 'D') {
        return false;
    }

    std::uint32_t width = 0;
    std::uint32_t height = 0;
    if (!TryReadU32Le(block, 4, width)
        || !TryReadU32Le(block, 8, height)
        || !TryReadU32Le(block, 12, header.blockSize)) {
        return false;
    }

    if (width == 0 || height == 0
        || width > static_cast<std::uint32_t>(std::numeric_limits<int>::max())
        || height > static_cast<std::uint32_t>(std::numeric_limits<int>::max())) {
        return false;
    }

    header.version = block[2];
    header.padding = block[3];
    header.width = static_cast<int>(width);
    header.height = static_cast<int>(height);
    header.compressed = (header.version & 0x01U) != 0;
    header.hasEmbeddedPalette = (header.version & 0x02U) != 0;

    headerSize = BaseHeaderSize;
    if (header.hasEmbeddedPalette) {
        headerSize += CgexPaletteSizeFieldSize;
        if (block.size() < headerSize || !TryReadU32Le(block, BaseHeaderSize, header.paletteSize)) {
            return false;
        }
    }

    if (header.blockSize < headerSize || header.blockSize > block.size()) {
        return false;
    }

    return true;
}

[[nodiscard]] bool TryAppendBytes(
    std::span<const std::uint8_t> input,
    std::size_t& inputOffset,
    std::size_t length,
    std::vector<std::uint8_t>& output,
    std::size_t expectedOutputSize)
{
    if (inputOffset > input.size() || output.size() > expectedOutputSize
        || length > input.size() - inputOffset) {
        return false;
    }
    if (length > expectedOutputSize - output.size()) {
        return false;
    }

    output.insert(output.end(), input.begin() + static_cast<std::ptrdiff_t>(inputOffset),
        input.begin() + static_cast<std::ptrdiff_t>(inputOffset + length));
    inputOffset += length;
    return true;
}

[[nodiscard]] bool TryAppendFill(
    std::uint8_t value,
    std::size_t length,
    std::vector<std::uint8_t>& output,
    std::size_t expectedOutputSize)
{
    if (output.size() > expectedOutputSize || length > expectedOutputSize - output.size()) {
        return false;
    }

    output.insert(output.end(), length, value);
    return true;
}

[[nodiscard]] bool TryReadByte(
    std::span<const std::uint8_t> input,
    std::size_t& inputOffset,
    std::uint8_t& value)
{
    if (inputOffset >= input.size()) {
        return false;
    }

    value = input[inputOffset];
    ++inputOffset;
    return true;
}

[[nodiscard]] bool DecodeRle(
    std::span<const std::uint8_t> input,
    std::size_t expectedOutputSize,
    std::vector<std::uint8_t>& output)
{
    output.clear();
    output.reserve(expectedOutputSize);

    std::size_t inputOffset = 0;
    while (output.size() < expectedOutputSize) {
        std::uint8_t control = 0;
        if (!TryReadByte(input, inputOffset, control)) {
            return false;
        }

        const std::uint8_t op = control & 0xF0U;
        const std::size_t highLength = control & 0x0FU;

        switch (op) {
        case 0x00U:
            if (!TryAppendBytes(input, inputOffset, highLength, output, expectedOutputSize)) {
                return false;
            }
            break;
        case 0x10U: {
            std::uint8_t low = 0;
            if (!TryReadByte(input, inputOffset, low)) {
                return false;
            }
            const std::size_t length = highLength * 0x100U + low;
            if (!TryAppendBytes(input, inputOffset, length, output, expectedOutputSize)) {
                return false;
            }
            break;
        }
        case 0x20U: {
            std::uint8_t middle = 0;
            std::uint8_t low = 0;
            if (!TryReadByte(input, inputOffset, middle) || !TryReadByte(input, inputOffset, low)) {
                return false;
            }
            const std::size_t length = highLength * 0x10000U + middle * 0x100U + low;
            if (!TryAppendBytes(input, inputOffset, length, output, expectedOutputSize)) {
                return false;
            }
            break;
        }
        case 0x80U: {
            std::uint8_t value = 0;
            if (!TryReadByte(input, inputOffset, value)
                || !TryAppendFill(value, highLength, output, expectedOutputSize)) {
                return false;
            }
            break;
        }
        case 0x90U: {
            std::uint8_t value = 0;
            std::uint8_t low = 0;
            if (!TryReadByte(input, inputOffset, value) || !TryReadByte(input, inputOffset, low)) {
                return false;
            }
            const std::size_t length = highLength * 0x100U + low;
            if (!TryAppendFill(value, length, output, expectedOutputSize)) {
                return false;
            }
            break;
        }
        case 0xA0U: {
            std::uint8_t value = 0;
            std::uint8_t middle = 0;
            std::uint8_t low = 0;
            if (!TryReadByte(input, inputOffset, value)
                || !TryReadByte(input, inputOffset, middle)
                || !TryReadByte(input, inputOffset, low)) {
                return false;
            }
            const std::size_t length = highLength * 0x10000U + middle * 0x100U + low;
            if (!TryAppendFill(value, length, output, expectedOutputSize)) {
                return false;
            }
            break;
        }
        case 0xC0U:
            if (!TryAppendFill(0, highLength, output, expectedOutputSize)) {
                return false;
            }
            break;
        case 0xD0U: {
            std::uint8_t low = 0;
            if (!TryReadByte(input, inputOffset, low)) {
                return false;
            }
            const std::size_t length = highLength * 0x100U + low;
            if (!TryAppendFill(0, length, output, expectedOutputSize)) {
                return false;
            }
            break;
        }
        case 0xE0U: {
            std::uint8_t middle = 0;
            std::uint8_t low = 0;
            if (!TryReadByte(input, inputOffset, middle) || !TryReadByte(input, inputOffset, low)) {
                return false;
            }
            const std::size_t length = highLength * 0x10000U + middle * 0x100U + low;
            if (!TryAppendFill(0, length, output, expectedOutputSize)) {
                return false;
            }
            break;
        }
        default:
            return false;
        }
    }

    return true;
}

[[nodiscard]] bool BuildEmbeddedPalette(
    const Palette& defaultPalette,
    std::span<const std::uint8_t> paletteBytes,
    Palette& palette)
{
    if (paletteBytes.empty()) {
        palette = defaultPalette;
        return true;
    }
    if (paletteBytes.size() % BytesPerColor != 0
        || paletteBytes.size() / BytesPerColor > Palette::ColorCount) {
        return false;
    }

    auto colors = defaultPalette.Colors();
    const std::size_t colorCount = paletteBytes.size() / BytesPerColor;
    for (std::size_t i = 0; i < colorCount; ++i) {
        const std::size_t source = i * BytesPerColor;
        colors[i] = Rgba8 {
            paletteBytes[source + 2],
            paletteBytes[source + 1],
            paletteBytes[source],
            255,
        };
    }

    palette = Palette::FromColors(colors);
    return true;
}

[[nodiscard]] bool TryGetPixelCount(
    const GraphicBlockHeader& header,
    std::size_t& pixelCount)
{
    const std::uint64_t width = static_cast<std::uint64_t>(header.width);
    const std::uint64_t height = static_cast<std::uint64_t>(header.height);
    const std::uint64_t pixels = width * height;

    if (pixels > std::numeric_limits<std::size_t>::max()) {
        return false;
    }

    pixelCount = static_cast<std::size_t>(pixels);
    return true;
}

[[nodiscard]] GraphicDecodeResult ConvertIndexedToRgba(
    const GraphicInfo& info,
    const GraphicBlockHeader& header,
    std::span<const std::uint8_t> indexed,
    const Palette& palette)
{
    GraphicDecodeResult result;
    result.image.width = header.width;
    result.image.height = header.height;
    result.image.offsetX = info.offsetX;
    result.image.offsetY = info.offsetY;
    result.image.mapNo = info.mapNo;

    const bool zeroOpaque = (info.flag & 0x08) != 0;
    result.image.pixels.resize(indexed.size() * 4U);

    for (int y = 0; y < header.height; ++y) {
        const int sourceY = header.height - 1 - y;
        for (int x = 0; x < header.width; ++x) {
            const std::size_t source = static_cast<std::size_t>(sourceY * header.width + x);
            const std::uint8_t colorIndex = indexed[source];
            const Rgba8 color = palette[colorIndex];
            const std::size_t target = static_cast<std::size_t>(y * header.width + x) * 4U;

            result.image.pixels[target] = color.r;
            result.image.pixels[target + 1] = color.g;
            result.image.pixels[target + 2] = color.b;
            result.image.pixels[target + 3] = (colorIndex == 0 && !zeroOpaque) ? 0 : color.a;
        }
    }

    return result;
}

} // namespace

GraphicDecodeResult GraphicDecoder::DecodeFromFile(
    const GraphicInfo& info,
    const std::filesystem::path& graphicDataPath,
    const Palette& defaultPalette) const
{
    std::uint64_t offset = 0;
    std::uint64_t length = 0;
    if (!TryToU64(info.offset, offset) || !TryToU64(info.length, length) || length == 0) {
        auto result = MakeError(GraphicDecodeError::InvalidInfo, "GraphicInfo offset/length 无效");
        result.path = graphicDataPath;
        return result;
    }

    BinaryReader reader(graphicDataPath);
    if (!reader.IsOpen()) {
        auto result = MakeError(GraphicDecodeError::FileOpenFailed, "Graphic 数据文件打开失败");
        result.path = graphicDataPath;
        return result;
    }

    if (offset > reader.Size()
        || length > reader.Size() - offset
        || length > std::numeric_limits<std::size_t>::max()) {
        auto result = MakeError(GraphicDecodeError::UnexpectedEof, "GraphicInfo 指向范围越过文件结尾");
        result.path = graphicDataPath;
        return result;
    }

    std::vector<std::uint8_t> block;
    if (!reader.Seek(offset) || !reader.ReadBytes(block, static_cast<std::size_t>(length))) {
        auto result = MakeError(GraphicDecodeError::UnexpectedEof, "Graphic 数据读取不完整");
        result.path = graphicDataPath;
        return result;
    }

    auto result = DecodeBlock(info, block, defaultPalette);
    result.path = graphicDataPath;
    return result;
}

GraphicDecodeResult GraphicDecoder::DecodeBlock(
    const GraphicInfo& info,
    std::span<const std::uint8_t> block,
    const Palette& defaultPalette) const
{
    GraphicBlockHeader header;
    std::size_t headerSize = 0;
    if (!TryParseHeader(block, header, headerSize)) {
        return MakeError(GraphicDecodeError::InvalidHeader, "Graphic 块头无效");
    }

    std::size_t pixelCount = 0;
    if (!TryGetPixelCount(header, pixelCount)) {
        return MakeError(GraphicDecodeError::OutputOverflow, "Graphic 输出尺寸无效");
    }

    const std::span<const std::uint8_t> payload = block.subspan(
        headerSize,
        static_cast<std::size_t>(header.blockSize) - headerSize);
    if (header.paletteSize > payload.size()) {
        return MakeError(GraphicDecodeError::UnexpectedEof, "CGEX palette 范围越界");
    }

    std::span<const std::uint8_t> imagePayload = payload;
    std::span<const std::uint8_t> paletteBytes;
    if (header.hasEmbeddedPalette && header.paletteSize > 0) {
        imagePayload = payload.first(payload.size() - header.paletteSize);
        paletteBytes = payload.last(header.paletteSize);
    }

    std::vector<std::uint8_t> decoded;
    if (header.compressed) {
        if (!DecodeRle(imagePayload, pixelCount, decoded)) {
            return MakeError(GraphicDecodeError::InvalidRle, "Graphic RLE 数据越界或控制字节无效");
        }
    } else {
        if (imagePayload.size() < pixelCount) {
            return MakeError(GraphicDecodeError::UnexpectedEof, "Graphic indexed 数据不完整");
        }
        decoded.assign(imagePayload.begin(), imagePayload.begin() + static_cast<std::ptrdiff_t>(pixelCount));
    }

    Palette activePalette = defaultPalette;
    if (!paletteBytes.empty()) {
        if (!BuildEmbeddedPalette(defaultPalette, paletteBytes, activePalette)) {
            return MakeError(GraphicDecodeError::InvalidPalette, "CGEX 内嵌 palette 长度无效");
        }
    }

    return ConvertIndexedToRgba(
        info,
        header,
        std::span<const std::uint8_t>(decoded.data(), pixelCount),
        activePalette);
}

} // namespace cg::resource
