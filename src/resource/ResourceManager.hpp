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

// ResourceManager 管理动态 binSet 索引，并提供 Graphic/Anime 查询和解码入口。
class ResourceManager final {
public:
    [[nodiscard]] bool LoadBinSets(std::span<const BinSetDesc> binSets, const std::filesystem::path& defaultPalettePath);
    void Clear();

    [[nodiscard]] std::optional<GraphicRef> FindGraphicByMapNo(int mapNo) const;
    [[nodiscard]] std::optional<GraphicRef> FindGraphicBySeq(int binSetIndex, int seqNo) const;
    [[nodiscard]] std::optional<GraphicRef> FindGraphicByGlobalIndex(int globalIndex) const;
    [[nodiscard]] std::optional<GraphicRef> FindGraphicForFrame(const AnimationClip& clip, const AnimeFrame& frame) const;
    [[nodiscard]] std::optional<AnimeRef> FindAnime(int animeNo) const;

    [[nodiscard]] std::optional<DecodedImage> DecodeGraphic(const GraphicRef& ref);
    [[nodiscard]] std::optional<DecodedImage> DecodeGraphicByMapNo(int mapNo);
    [[nodiscard]] std::optional<AnimationClip> LoadAnimation(int animeNo);

    [[nodiscard]] const ResourceStats& Stats() const noexcept;
    [[nodiscard]] const ResourceError& LastError() const noexcept;
    [[nodiscard]] const Palette& DefaultPalette() const noexcept;

private:
    struct BinSetState {
        BinSetDesc desc;
        int graphicStartIndex = 0;
        std::unordered_map<int, GraphicRef> graphicsBySeq;
    };

    [[nodiscard]] bool LoadGraphicIndex(int binSetIndex, const BinSetDesc& desc, BinSetState& state);
    [[nodiscard]] bool LoadAnimeIndex(int binSetIndex, const BinSetDesc& desc);

    void ClearError();
    void SetError(ResourceErrorCode code, const std::filesystem::path& path, int resourceId, std::string message);
    void AddWarning(std::string_view message);

    std::vector<BinSetState> binSets_;
    std::unordered_map<int, GraphicRef> graphicsByMapNo_;
    std::unordered_map<int, GraphicRef> graphicsByGlobalIndex_;
    std::unordered_map<int, AnimeRef> animesByNo_;
    Palette defaultPalette_;
    ResourceStats stats_;
    ResourceError lastError_;
    GraphicDecoder graphicDecoder_;
    int nextGraphicGlobalIndex_ = 0;
};

} // namespace cg::resource
