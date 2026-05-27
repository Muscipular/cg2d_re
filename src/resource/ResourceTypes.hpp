#pragma once

#include "render/Texture.hpp"

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace cg::resource {

// BinSetDesc 描述一组旧客户端 binSet 资源文件路径。
struct BinSetDesc {
    std::string name;
    std::filesystem::path graphicInfoPath;
    std::filesystem::path graphicDataPath;
    std::filesystem::path animeInfoPath;
    std::filesystem::path animeDataPath;
    std::filesystem::path coordinateInfoPath;
    std::filesystem::path coordinateDataPath;
};

// GraphicInfo 对应 GraphicInfo_*.bin 的单条图档索引信息。
struct GraphicInfo {
    int seqNo = 0;
    std::uint32_t offset = 0;
    std::uint32_t length = 0;
    int offsetX = 0;
    int offsetY = 0;
    int width = 0;
    int height = 0;
    int sizeX = 0;
    int sizeY = 0;
    std::uint32_t flag = 0;
    int mapNo = 0;
};

// GraphicRef 表示可定位到具体 binSet 和数据文件的图档引用。
struct GraphicRef {
    int binSetIndex = 0;
    int seqNo = 0;
    int mapNo = 0;
    std::filesystem::path dataPath;
    GraphicInfo info;
};

// AnimeInfo 对应 AnimeInfo_*.bin 的单条动画索引信息。
struct AnimeInfo {
    int animeNo = 0;
    std::uint32_t offset = 0;
    std::uint16_t count = 0;
    std::uint16_t flag = 0;
};

// AnimeRef 表示可定位到具体 binSet 和数据文件的动画引用。
struct AnimeRef {
    int binSetIndex = 0;
    int animeNo = 0;
    std::filesystem::path dataPath;
    AnimeInfo info;
};

// AnimeFrame 表示动画帧中引用的图档和绘制偏移。
struct AnimeFrame {
    int graphicSeqNo = 0;
    int x = 0;
    int y = 0;
    int flag = 0;
};

// AnimeAction 描述动画动作头及其在帧数组中的范围。
struct AnimeAction {
    int dir = 0;
    int action = 0;
    int duration = 0;
    int frameIndex = 0;
    int frameCount = 0;
    int pal = 0;
    bool revert = false;
    std::uint32_t padding = 0;
};

// AnimationClip 保存一个 animeNo 下的动作头和连续帧数据。
struct AnimationClip {
    int animeNo = 0;
    std::vector<AnimeAction> actions;
    std::vector<AnimeFrame> frames;
};

// DecodedImage 是 Graphic 解码后的纯 RGBA8 像素数据，不包含 SDL 类型。
struct DecodedImage {
    int width = 0;
    int height = 0;
    int offsetX = 0;
    int offsetY = 0;
    int mapNo = 0;
    cg::render::PixelFormat format = cg::render::PixelFormat::Rgba8;
    std::vector<std::uint8_t> pixels;
};

// ResourceStats 记录资源加载过程的基础统计。
struct ResourceStats {
    std::uint32_t binSetCount = 0;
    std::uint32_t graphicCount = 0;
    std::uint32_t animeCount = 0;
    std::uint32_t graphicOverrideCount = 0;
    std::uint32_t animeOverrideCount = 0;
    std::uint32_t warningCount = 0;
    std::uint32_t errorCount = 0;
};

} // namespace cg::resource
