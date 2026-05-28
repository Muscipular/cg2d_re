#pragma once

#include "GraphicDecoder.hpp"
#include "Palette.hpp"
#include "ResourceError.hpp"
#include "ResourceTypes.hpp"

#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

namespace cg::resource {

class BinaryReader;

struct ResourceLoadOptions {
    bool buildAutoMapColorCache = true;
};

// ResourceManager 管理动态 binSet 索引，并提供 Graphic/Anime 查询和解码入口。
class ResourceManager final {
public:
    [[nodiscard]] bool LoadBinSets(
        std::span<const BinSetDesc> binSets,
        const std::filesystem::path& defaultPalettePath,
        ResourceLoadOptions options = {});
    void Clear();

    [[nodiscard]] std::optional<GraphicRef> FindGraphicByMapNo(int mapNo) const;
    // 返回值 0 是有效的“无颜色/缺省颜色”索引；nullopt 仅表示未命中颜色缓存。
    [[nodiscard]] std::optional<std::uint8_t> FindAutoMapColorIndexByMapNo(int mapNo) const;
    [[nodiscard]] std::optional<std::uint8_t> FindAutoMapColorIndexByRaw(std::uint32_t raw) const;
    [[nodiscard]] std::optional<GraphicRef> FindGraphicBySeq(int binSetIndex, int seqNo) const;
    [[nodiscard]] std::optional<GraphicRef> FindGraphicByGlobalIndex(int globalIndex) const;
    [[nodiscard]] std::optional<GraphicRef> FindGraphicForFrame(const AnimationClip& clip, const AnimeFrame& frame) const;
    // Anime 查询只接受 AnimeInfo 中的原始 animeNo，不接受内部 index。
    [[nodiscard]] std::optional<AnimeRef> FindAnime(int sourceAnimeNo) const;
    [[nodiscard]] std::span<const GraphicRef> Graphics(int binSetIndex) const noexcept;

    [[nodiscard]] std::optional<DecodedImage> DecodeGraphic(const GraphicRef& ref);
    [[nodiscard]] std::optional<DecodedImage> DecodeGraphicByMapNo(int mapNo);
    // Anime 加载只接受 AnimeInfo 中的原始 animeNo，不接受内部 index。
    [[nodiscard]] std::optional<AnimationClip> LoadAnimation(int sourceAnimeNo);

    [[nodiscard]] const ResourceStats& Stats() const noexcept;
    [[nodiscard]] const ResourceError& LastError() const noexcept;
    [[nodiscard]] const Palette& DefaultPalette() const noexcept;

private:
    struct BinSetState {
        BinSetDesc desc;
        int graphicStartIndex = 0;
        std::vector<GraphicRef> graphics;
        std::unordered_map<int, GraphicRef> graphicsBySeq;
    };

    [[nodiscard]] bool LoadGraphicIndex(
        int binSetIndex,
        const BinSetDesc& desc,
        BinSetState& state,
        const ResourceLoadOptions& options);
    [[nodiscard]] bool LoadAnimeIndex(int binSetIndex, const BinSetDesc& desc);

    void BuildAutoMapColorIndex(const GraphicRef& ref, BinaryReader& graphicDataReader, bool& graphicDataOpenTried);
    void ClearError();
    void SetError(ResourceErrorCode code, const std::filesystem::path& path, int resourceId, std::string message);
    void AddWarning(std::string_view message);

    std::vector<BinSetState> binSets_;
    std::unordered_map<int, GraphicRef> graphicsByMapNo_;
    std::unordered_map<int, std::uint8_t> autoMapColorByMapNo_;
    std::unordered_map<int, GraphicRef> graphicsByGlobalIndex_;
    std::unordered_map<int, AnimeRef> animesBySourceAnimeNo_;
    Palette defaultPalette_;
    ResourceStats stats_;
    ResourceError lastError_;
    GraphicDecoder graphicDecoder_;
    int nextGraphicGlobalIndex_ = 0;
};

} // namespace cg::resource
