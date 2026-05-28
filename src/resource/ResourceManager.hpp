#pragma once

#include "GraphicDecoder.hpp"
#include "Palette.hpp"
#include "ResourceError.hpp"
#include "ResourceTypes.hpp"

#include <cstddef>
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

namespace cg::resource {

class BinaryReader;

struct ResourceLoadOptions {
    bool buildAutoMapColorCache = true;
    std::filesystem::path hiddenPaletteTablePath;
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
    [[nodiscard]] std::optional<int> FindHiddenPaletteMapNoByPalIndex(int palIndex) const;
    // Anime 查询只接受 AnimeInfo 中的原始 animeNo，不接受内部 indexNo；indexNo 是旧客户端派生动画编号/内部动画号，不是数组下标。
    [[nodiscard]] std::optional<AnimeRef> FindAnime(int sourceAnimeNo) const;
    [[nodiscard]] std::span<const GraphicRef> Graphics(int binSetIndex) const noexcept;

    [[nodiscard]] std::optional<DecodedImage> DecodeGraphic(const GraphicRef& ref);
    [[nodiscard]] std::optional<DecodedImage> DecodeGraphic(const GraphicRef& ref, const Palette& palette);
    [[nodiscard]] std::optional<DecodedImage> DecodeGraphicByMapNo(int mapNo);
    [[nodiscard]] std::optional<Palette> LoadHiddenPaletteByMapNo(int mapNo);
    [[nodiscard]] std::optional<Palette> LoadHiddenPaletteByPalIndex(int palIndex);
    // Anime 加载只接受 AnimeInfo 中的原始 animeNo，不接受内部 indexNo；indexNo 是旧客户端派生动画编号/内部动画号，不是数组下标。
    [[nodiscard]] std::optional<AnimationClip> LoadAnimation(int sourceAnimeNo);

    [[nodiscard]] const ResourceStats& Stats() const noexcept;
    [[nodiscard]] const ResourceError& LastError() const noexcept;
    [[nodiscard]] const Palette& DefaultPalette() const noexcept;

private:
    struct GraphicHandle {
        int binSetIndex = -1;
        std::size_t localIndex = 0;
    };

    struct BinSetState {
        BinSetDesc desc;
        int graphicStartIndex = 0;
        std::vector<GraphicRef> graphics;
        std::vector<GraphicHandle> graphicsBySeq;
        std::unordered_map<int, GraphicHandle> graphicsBySparseSeq;
    };

    [[nodiscard]] const GraphicRef* ResolveGraphicHandle(const GraphicHandle& handle) const noexcept;
    [[nodiscard]] const GraphicRef* FindGraphicInStateBySeq(const BinSetState& state, int seqNo) const noexcept;
    [[nodiscard]] const std::filesystem::path* ResolveGraphicDataPath(const GraphicRef& ref) const noexcept;
    [[nodiscard]] bool LoadGraphicIndex(
        int binSetIndex,
        const BinSetDesc& desc,
        BinSetState& state,
        const ResourceLoadOptions& options);
    [[nodiscard]] bool LoadAnimeIndex(int binSetIndex, const BinSetDesc& desc);

    void BuildAutoMapColorIndex(
        const GraphicRef& ref,
        const std::filesystem::path& graphicDataPath,
        BinaryReader& graphicDataReader,
        bool& graphicDataOpenTried);
    void LoadHiddenPaletteTable(const ResourceLoadOptions& options);
    void ClearError();
    void SetError(ResourceErrorCode code, const std::filesystem::path& path, int resourceId, std::string message);
    void AddWarning(std::string_view message);

    std::vector<BinSetState> binSets_;
    std::unordered_map<int, GraphicHandle> graphicsByMapNo_;
    std::unordered_map<int, std::uint8_t> autoMapColorByMapNo_;
    std::vector<GraphicHandle> graphicsByGlobalIndex_;
    std::unordered_map<int, GraphicHandle> graphicsBySparseGlobalIndex_;
    std::vector<int> hiddenPaletteMapNoByPalIndex_;
    std::unordered_map<int, AnimeRef> animesBySourceAnimeNo_;
    Palette defaultPalette_;
    ResourceStats stats_;
    ResourceError lastError_;
    GraphicDecoder graphicDecoder_;
    int nextGraphicGlobalIndex_ = 0;
};

} // namespace cg::resource
