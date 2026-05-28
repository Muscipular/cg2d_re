#pragma once

#include "render/Texture.hpp"

#include <cstddef>
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

#pragma pack(push, 1)

// CG_GraphicVersion 对应 IDA CG_GraphicVersion，底层长度固定为 1 字节。
enum CG_GraphicVersion : char {
    CG_VER_UNZIP = 0,
    CG_VER_ZIP = 1,
    CG_VER_CGEX = 2,
};
static_assert(sizeof(CG_GraphicVersion) == 1);

// CG_Direction 对应 IDA CG_Direction，底层长度固定为 2 字节。
enum CG_Direction : std::int16_t {
    CG_DIRECTION_NONE = -1,
    CG_DIRECTION_NORTH = 0,
    CG_DIRECTION_NORTHEAST = 1,
    CG_DIRECTION_EAST = 2,
    CG_DIRECTION_SOUTHEAST = 3,
    CG_DIRECTION_SOUTH = 4,
    CG_DIRECTION_SOUTHWEST = 5,
    CG_DIRECTION_WEST = 6,
    CG_DIRECTION_NORTHWEST = 7,
};
static_assert(sizeof(CG_Direction) == 2);

// CG_ActionType 对应 IDA CG_ActionType，底层长度固定为 2 字节。
enum CG_ActionType : std::int16_t {
    CG_ACTIONTYPE_NONE = -1,
    CG_ACTIONTYPE_STAND = 0,
    CG_ACTIONTYPE_WALK = 1,
    CG_ACTIONTYPE_BEFORERUN = 2,
    CG_ACTIONTYPE_RUN = 3,
    CG_ACTIONTYPE_AFTERRUN = 4,
    CG_ACTIONTYPE_ATTACK = 5,
    CG_ACTIONTYPE_MAGIC = 6,
    CG_ACTIONTYPE_THROW = 7,
    CG_ACTIONTYPE_HURT = 8,
    CG_ACTIONTYPE_DEFENCE = 9,
    CG_ACTIONTYPE_DEAD = 10,
    CG_ACTIONTYPE_SIT = 11,
    CG_ACTIONTYPE_HI = 12,
    CG_ACTIONTYPE_HAPPY = 13,
    CG_ACTIONTYPE_ANGRY = 14,
    CG_ACTIONTYPE_SAD = 15,
    CG_ACTIONTYPE_SHAKE = 16,
    CG_ACTIONTYPE_ROCK = 17,
    CG_ACTIONTYPE_SCISSORS = 18,
    CG_ACTIONTYPE_PAPER = 19,
    CG_ACTIONTYPE_FISHING = 20,
};
static_assert(sizeof(CG_ActionType) == 2);

// GraphicInfo 对应 IDA CG_GraphicInfo，长度固定为 40 字节。
struct GraphicInfo {
    std::int32_t seqNo = 0;
    std::int32_t offset = 0;
    std::int32_t length = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t w = 0;
    std::int32_t h = 0;
    std::int8_t sx = 0;
    std::int8_t sy = 0;
    std::int8_t blockFlag = 0;
    std::int8_t asGround = 0;
    char pad[4] {};
    std::int32_t mapNo = 0;
};
static_assert(sizeof(GraphicInfo) == 40);

// GraphicDataHeader 对应 IDA CG_GraphicHead，长度固定为 16 字节。
struct GraphicDataHeader {
    char head[2] {};
    CG_GraphicVersion version = CG_VER_UNZIP;
    char unk1 = 0;
    int w = 0;
    int h = 0;
    int length = 0;
};
static_assert(sizeof(GraphicDataHeader) == 16);

// GraphicCgexDataHeader 对应 IDA CG_Graphic 的 V2 视图，data 是 IDA 占位字段。
struct GraphicCgexDataHeader {
    GraphicDataHeader head;
    std::int32_t palSize = 0;
    char data[4] {};
};
static_assert(sizeof(GraphicCgexDataHeader) == 24);

// AnimeInfo 对应 IDA CG_AnimateInfo，长度固定为 12 字节。
struct AnimeInfo {
    std::int32_t animeNo = 0;
    std::uint32_t offset = 0;
    std::int16_t count = 0;
    std::uint16_t flg = 0;
};
static_assert(sizeof(AnimeInfo) == 12);

// AnimeFrame 对应 IDA CG_AnimateFrame，长度固定为 10 字节。
struct AnimeFrame {
    std::int32_t graphicSeqNo = 0;
    std::int16_t x = 0;
    std::int16_t y = 0;
    std::int16_t flag = 0;
};
static_assert(sizeof(AnimeFrame) == 10);

// AnimeActionHeader 对应 IDA CG_AnimateHead，长度固定为 20 字节。
struct AnimeActionHeader {
    CG_Direction dir = CG_DIRECTION_NONE;
    CG_ActionType action = CG_ACTIONTYPE_NONE;
    std::int32_t duration = 0;
    std::int32_t frameCount = 0;
    std::int16_t pal = 0;
    std::int16_t revert = 0;
    std::int32_t colorMask = 0;
};
static_assert(sizeof(AnimeActionHeader) == 20);

#pragma pack(pop)

// GraphicDataMetadata 保存从 IDA 头部字段派生出的解码状态，不属于磁盘结构。
struct GraphicDataMetadata {
    std::uint32_t paletteSize = 0;
    std::size_t headerSize = 0;
    bool compressed = false;
    bool hasEmbeddedPalette = false;
    bool zeroTransparent = false;
};

// GraphicRef 表示可定位到具体 binSet 和数据文件的图档引用。
struct GraphicRef {
    int binSetIndex = 0;
    int globalIndex = 0;
    int seqNo = 0;
    int mapNo = 0;
    std::filesystem::path dataPath;
    GraphicInfo info;
};

// AnimeRef 表示可定位到具体 binSet 和数据文件的动画引用。
struct AnimeRef {
    int binSetIndex = 0;
    // index 是旧客户端内部连续索引；sourceAnimeNo 是 AnimeInfo 原始 animeNo。
    int index = 0;
    int sourceAnimeNo = 0;
    std::filesystem::path dataPath;
    AnimeInfo info;
};

// AnimeAction 描述动画动作头及其在帧数组中的范围。
struct AnimeAction {
    AnimeActionHeader header;
    int frameIndex = 0;
    std::size_t headerSize = 0;
    bool hasExtendedFields = false;
};

// AnimationClip 保存一个原始 animeNo 下的动作头和连续帧数据。
struct AnimationClip {
    // index 是旧客户端内部连续索引；sourceAnimeNo 是 AnimeInfo 原始 animeNo。
    int index = 0;
    int sourceAnimeNo = 0;
    int binSetIndex = -1;
    int graphicStartIndex = -1;
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
    GraphicDataHeader dataHeader;
    GraphicDataMetadata dataMetadata;
    cg::render::PixelFormat format = cg::render::PixelFormat::Rgba8;
    std::vector<std::uint8_t> pixels;
};

// GraphicIndexedImage 是 Graphic 解码后的 indexed 色号像素数据，不做 palette/透明转换。
struct GraphicIndexedImage {
    int width = 0;
    int height = 0;
    int offsetX = 0;
    int offsetY = 0;
    int mapNo = 0;
    GraphicDataHeader dataHeader;
    GraphicDataMetadata dataMetadata;
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
