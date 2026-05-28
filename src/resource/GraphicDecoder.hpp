#pragma once

#include "resource/Palette.hpp"
#include "resource/ResourceTypes.hpp"

#include <cstdint>
#include <filesystem>
#include <span>
#include <string>

namespace cg::resource {

// GraphicDecodeError 表示 Graphic 块读取和解码错误。
enum class GraphicDecodeError {
    None,
    FileOpenFailed,
    InvalidInfo,
    InvalidHeader,
    UnexpectedEof,
    InvalidRle,
    OutputOverflow,
    InvalidPalette,
};

// GraphicBlockHeader 是 RD Graphic 块头的纯数据表示。
using GraphicBlockHeader = GraphicDataHeader;

// GraphicDecodeResultT 返回 Graphic 解码图片或失败原因。
template <typename TImage>
struct GraphicDecodeResultT {
    TImage image;
    GraphicDecodeError error = GraphicDecodeError::None;
    std::filesystem::path path;
    std::string message;

    [[nodiscard]] bool Succeeded() const noexcept
    {
        return error == GraphicDecodeError::None;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return Succeeded();
    }
};

// GraphicDecodeResult 返回 RGBA8 图片或失败原因。
using GraphicDecodeResult = GraphicDecodeResultT<DecodedImage>;

// GraphicIndexedDecodeResult 返回 indexed 色号图片或失败原因。
using GraphicIndexedDecodeResult = GraphicDecodeResultT<GraphicIndexedImage>;

// GraphicDecoder 解码 GraphicInfo 指向的 RD/CGEX 图块，不暴露 SDL 类型。
class GraphicDecoder final {
public:
    [[nodiscard]] GraphicDecodeResult DecodeFromFile(
        const GraphicInfo& info,
        const std::filesystem::path& graphicDataPath,
        const Palette& defaultPalette) const;

    [[nodiscard]] GraphicDecodeResult DecodeBlock(
        const GraphicInfo& info,
        std::span<const std::uint8_t> block,
        const Palette& defaultPalette) const;

    [[nodiscard]] GraphicIndexedDecodeResult DecodeGraphicIndexed(
        const GraphicInfo& info,
        std::span<const std::uint8_t> block) const;
};

} // namespace cg::resource
