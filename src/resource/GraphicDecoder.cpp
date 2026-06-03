#include "resource/GraphicDecoder.hpp"

#include "resource/BinaryReader.hpp"

#include <bit>
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

[[nodiscard]] GraphicIndexedDecodeResult MakeIndexedError(GraphicDecodeError error, std::string message)
{
    GraphicIndexedDecodeResult result;
    result.error = error;
    result.message = std::move(message);
    return result;
}

[[nodiscard]] GraphicPaletteDecodeResult MakePaletteError(GraphicDecodeError error, std::string message)
{
    GraphicPaletteDecodeResult result;
    result.error = error;
    result.message = std::move(message);
    return result;
}

struct DecodedIndexedBlock {
    GraphicBlockHeader header;
    GraphicDataMetadata metadata;
    std::size_t pixelCount = 0;
    std::vector<std::uint8_t> decoded;
};

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

[[nodiscard]] bool TryReadI32Le(
    std::span<const std::uint8_t> bytes,
    std::size_t offset,
    std::int32_t& value)
{
    std::uint32_t unsignedValue = 0;
    if (!TryReadU32Le(bytes, offset, unsignedValue)) {
        return false;
    }

    value = std::bit_cast<std::int32_t>(unsignedValue);
    return true;
}

[[nodiscard]] bool TryParseHeader(
    std::span<const std::uint8_t> block,
    GraphicBlockHeader& header,
    GraphicDataMetadata& metadata)
{
    if (block.size() < BaseHeaderSize) {
        return false;
    }
    if (block[0] != 'R' || block[1] != 'D') {
        return false;
    }

    std::int32_t width = 0;
    std::int32_t height = 0;
    std::int32_t blockLength = 0;
    if (!TryReadI32Le(block, 4, width)
        || !TryReadI32Le(block, 8, height)
        || !TryReadI32Le(block, 12, blockLength)) {
        return false;
    }

    if (width <= 0 || height <= 0 || blockLength <= 0) {
        return false;
    }

    header.head[0] = static_cast<char>(block[0]);
    header.head[1] = static_cast<char>(block[1]);
    header.version = static_cast<CG_GraphicVersion>(block[2]);
    header.unk1 = static_cast<char>(block[3]);
    header.w = width;
    header.h = height;
    header.length = blockLength;

    const auto version = static_cast<std::uint8_t>(header.version);
    metadata = GraphicDataMetadata {};
    metadata.compressed = (version & 0x01U) != 0;
    metadata.hasEmbeddedPalette = (version & 0x02U) != 0;

    std::size_t headerSize = BaseHeaderSize;
    if (metadata.hasEmbeddedPalette) {
        headerSize += CgexPaletteSizeFieldSize;
        std::int32_t paletteSize = 0;
        if (block.size() < headerSize || !TryReadI32Le(block, BaseHeaderSize, paletteSize) || paletteSize < 0) {
            return false;
        }
        metadata.paletteSize = static_cast<std::uint32_t>(paletteSize);
    }
    metadata.headerSize = headerSize;

    if (static_cast<std::uint64_t>(header.length) < headerSize
        || static_cast<std::uint64_t>(header.length) > block.size()) {
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
    const std::uint64_t width = static_cast<std::uint64_t>(header.w);
    const std::uint64_t height = static_cast<std::uint64_t>(header.h);
    const std::uint64_t pixels = width * height;

    if (pixels > std::numeric_limits<std::size_t>::max()) {
        return false;
    }

    pixelCount = static_cast<std::size_t>(pixels);
    return true;
}

[[nodiscard]] GraphicDecodeError DecodeIndexedBlockData(
    std::span<const std::uint8_t> block,
    DecodedIndexedBlock& output,
    std::string& message)
{
    GraphicBlockHeader header;
    GraphicDataMetadata metadata;
    if (!TryParseHeader(block, header, metadata)) {
        message = "Graphic 块头无效";
        return GraphicDecodeError::InvalidHeader;
    }

    std::size_t pixelCount = 0;
    if (!TryGetPixelCount(header, pixelCount)) {
        message = "Graphic 输出尺寸无效";
        return GraphicDecodeError::OutputOverflow;
    }

    const std::span<const std::uint8_t> imagePayload = block.subspan(
        metadata.headerSize,
        static_cast<std::size_t>(header.length) - metadata.headerSize);
    std::size_t decodedSize = pixelCount;
    if (metadata.hasEmbeddedPalette) {
        if (metadata.paletteSize > std::numeric_limits<std::size_t>::max() - decodedSize) {
            message = "Graphic 输出尺寸无效";
            return GraphicDecodeError::OutputOverflow;
        }
        decodedSize += metadata.paletteSize;
    }

    std::vector<std::uint8_t> decoded;
    if (metadata.compressed) {
        if (!DecodeRle(imagePayload, decodedSize, decoded)) {
            message = "Graphic RLE 数据越界或控制字节无效";
            return GraphicDecodeError::InvalidRle;
        }
    } else {
        if (imagePayload.size() < decodedSize) {
            message = "Graphic indexed 数据不完整";
            return GraphicDecodeError::UnexpectedEof;
        }
        decoded.assign(imagePayload.begin(), imagePayload.begin() + static_cast<std::ptrdiff_t>(decodedSize));
    }

    // IDA Renderer::LoadDXPalette(0x44E290) 最终将 palette[0] 置 0；旧端以黑色 color key 表达透明。
    metadata.zeroTransparent = true;

    output.header = header;
    output.metadata = metadata;
    output.pixelCount = pixelCount;
    output.decoded = std::move(decoded);
    return GraphicDecodeError::None;
}

[[nodiscard]] std::span<const std::uint8_t> IndexedPixels(const DecodedIndexedBlock& block)
{
    return std::span<const std::uint8_t>(block.decoded.data(), block.pixelCount);
}

[[nodiscard]] std::span<const std::uint8_t> EmbeddedPaletteBytes(const DecodedIndexedBlock& block)
{
    if (!block.metadata.hasEmbeddedPalette || block.metadata.paletteSize == 0) {
        return {};
    }

    return std::span<const std::uint8_t>(
        block.decoded.data() + block.pixelCount,
        block.metadata.paletteSize);
}

[[nodiscard]] GraphicDecodeResult ConvertIndexedToRgba(
    const GraphicInfo& info,
    const GraphicBlockHeader& header,
    const GraphicDataMetadata& metadata,
    std::span<const std::uint8_t> indexed,
    const Palette& palette)
{
    GraphicDecodeResult result;
    result.image.width = header.w;
    result.image.height = header.h;
    result.image.offsetX = info.x;
    result.image.offsetY = info.y;
    result.image.mapNo = info.mapNo;
    result.image.dataHeader = header;
    result.image.dataMetadata = metadata;
    result.image.pixels.resize(indexed.size() * 4U);

    for (int y = 0; y < header.h; ++y) {
        const int sourceY = header.h - 1 - y;
        for (int x = 0; x < header.w; ++x) {
            const std::size_t source = static_cast<std::size_t>(sourceY * header.w + x);
            const std::uint8_t colorIndex = indexed[source];
            const Rgba8 color = palette[colorIndex];
            const std::size_t target = static_cast<std::size_t>(y * header.w + x) * 4U;

            result.image.pixels[target] = color.r;
            result.image.pixels[target + 1] = color.g;
            result.image.pixels[target + 2] = color.b;
            result.image.pixels[target + 3] = colorIndex == 0 ? 0 : color.a;
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
    DecodedIndexedBlock decodedBlock;
    std::string message;
    const GraphicDecodeError error = DecodeIndexedBlockData(block, decodedBlock, message);
    if (error != GraphicDecodeError::None) {
        return MakeError(error, std::move(message));
    }

    Palette activePalette = defaultPalette;
    const std::span<const std::uint8_t> paletteBytes = EmbeddedPaletteBytes(decodedBlock);
    if (!paletteBytes.empty()) {
        if (!BuildEmbeddedPalette(defaultPalette, paletteBytes, activePalette)) {
            return MakeError(GraphicDecodeError::InvalidPalette, "CGEX 内嵌 palette 长度无效");
        }
    }

    return ConvertIndexedToRgba(
        info,
        decodedBlock.header,
        decodedBlock.metadata,
        IndexedPixels(decodedBlock),
        activePalette);
}

GraphicIndexedDecodeResult GraphicDecoder::DecodeGraphicIndexed(
    const GraphicInfo& info,
    std::span<const std::uint8_t> block) const
{
    DecodedIndexedBlock decodedBlock;
    std::string message;
    const GraphicDecodeError error = DecodeIndexedBlockData(block, decodedBlock, message);
    if (error != GraphicDecodeError::None) {
        return MakeIndexedError(error, std::move(message));
    }

    GraphicIndexedDecodeResult result;
    result.image.width = decodedBlock.header.w;
    result.image.height = decodedBlock.header.h;
    result.image.offsetX = info.x;
    result.image.offsetY = info.y;
    result.image.mapNo = info.mapNo;
    result.image.dataHeader = decodedBlock.header;
    result.image.dataMetadata = decodedBlock.metadata;
    decodedBlock.decoded.resize(decodedBlock.pixelCount);
    result.image.pixels = std::move(decodedBlock.decoded);
    return result;
}

GraphicPaletteDecodeResult GraphicDecoder::DecodeEmbeddedPaletteFromFile(
    const GraphicInfo& info,
    const std::filesystem::path& graphicDataPath,
    const Palette& defaultPalette) const
{
    std::uint64_t offset = 0;
    std::uint64_t length = 0;
    if (!TryToU64(info.offset, offset) || !TryToU64(info.length, length) || length == 0) {
        auto result = MakePaletteError(GraphicDecodeError::InvalidInfo, "GraphicInfo offset/length 无效");
        result.path = graphicDataPath;
        return result;
    }

    BinaryReader reader(graphicDataPath);
    if (!reader.IsOpen()) {
        auto result = MakePaletteError(GraphicDecodeError::FileOpenFailed, "Graphic 数据文件打开失败");
        result.path = graphicDataPath;
        return result;
    }

    if (offset > reader.Size()
        || length > reader.Size() - offset
        || length > std::numeric_limits<std::size_t>::max()) {
        auto result = MakePaletteError(GraphicDecodeError::UnexpectedEof, "GraphicInfo 指向范围越过文件结尾");
        result.path = graphicDataPath;
        return result;
    }

    std::vector<std::uint8_t> block;
    if (!reader.Seek(offset) || !reader.ReadBytes(block, static_cast<std::size_t>(length))) {
        auto result = MakePaletteError(GraphicDecodeError::UnexpectedEof, "Graphic 数据读取不完整");
        result.path = graphicDataPath;
        return result;
    }

    auto result = DecodeEmbeddedPaletteFromBlock(block, defaultPalette);
    result.path = graphicDataPath;
    return result;
}

GraphicPaletteDecodeResult GraphicDecoder::DecodeEmbeddedPaletteFromBlock(
    std::span<const std::uint8_t> block,
    const Palette& defaultPalette) const
{
    DecodedIndexedBlock decodedBlock;
    std::string message;
    const GraphicDecodeError error = DecodeIndexedBlockData(block, decodedBlock, message);
    if (error != GraphicDecodeError::None) {
        return MakePaletteError(error, std::move(message));
    }

    const std::span<const std::uint8_t> paletteBytes = EmbeddedPaletteBytes(decodedBlock);
    if (paletteBytes.empty()) {
        return MakePaletteError(GraphicDecodeError::InvalidPalette, "Graphic 未包含内嵌 palette");
    }

    GraphicPaletteDecodeResult result;
    if (!BuildEmbeddedPalette(defaultPalette, paletteBytes, result.palette)) {
        return MakePaletteError(GraphicDecodeError::InvalidPalette, "Graphic 内嵌 palette 长度无效");
    }

    return result;
}

} // namespace cg::resource
