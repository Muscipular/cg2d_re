#include "ResourceManager.hpp"

#include "AnimationLoader.hpp"
#include "BinaryReader.hpp"
#include "core/Log.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <utility>

namespace cg::resource {
namespace {

constexpr int AnimeNoBase = 100000;
constexpr std::uint64_t GraphicInfoRecordSize = sizeof(GraphicInfo);
static_assert(GraphicInfoRecordSize == 40);

[[nodiscard]] bool IsEmptyPathPair(const std::filesystem::path& first, const std::filesystem::path& second)
{
    return first.empty() || second.empty();
}

[[nodiscard]] std::int8_t ToI8(std::uint8_t value) noexcept
{
    return static_cast<std::int8_t>(value);
}

[[nodiscard]] int NormalizeAnimeNo(int animeNo) noexcept
{
    return animeNo >= AnimeNoBase ? animeNo - AnimeNoBase : animeNo;
}

[[nodiscard]] bool ReadGraphicInfo(BinaryReader& reader, GraphicInfo& info)
{
    std::int32_t seqNo = 0;
    std::int32_t offset = 0;
    std::int32_t length = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::int32_t w = 0;
    std::int32_t h = 0;
    std::uint8_t sx = 0;
    std::uint8_t sy = 0;
    std::uint8_t blockFlag = 0;
    std::uint8_t asGround = 0;
    std::array<std::uint8_t, 4> pad {};
    std::int32_t mapNo = 0;

    if (!reader.ReadI32(seqNo) || !reader.ReadI32(offset) || !reader.ReadI32(length)
        || !reader.ReadI32(x) || !reader.ReadI32(y)
        || !reader.ReadI32(w) || !reader.ReadI32(h)
        || !reader.ReadU8(sx) || !reader.ReadU8(sy) || !reader.ReadU8(blockFlag)
        || !reader.ReadU8(asGround) || !reader.ReadBytes(pad) || !reader.ReadI32(mapNo)) {
        return false;
    }

    info.seqNo = seqNo;
    info.offset = offset;
    info.length = length;
    info.x = x;
    info.y = y;
    info.w = w;
    info.h = h;
    info.sx = ToI8(sx);
    info.sy = ToI8(sy);
    info.blockFlag = ToI8(blockFlag);
    info.asGround = ToI8(asGround);
    for (std::size_t index = 0; index < 4; ++index) {
        info.pad[index] = static_cast<char>(pad[index]);
    }
    info.mapNo = mapNo;
    return true;
}

[[nodiscard]] std::string ToLogPath(const std::filesystem::path& path)
{
    return path.string();
}

} // namespace

bool ResourceManager::LoadBinSets(std::span<const BinSetDesc> binSets, const std::filesystem::path& defaultPalettePath)
{
    Clear();

    PaletteLoadResult paletteResult = LoadPaletteFromFile(defaultPalettePath);
    if (!paletteResult) {
        SetError(ResourceErrorCode::FileReadFailed, defaultPalettePath, 0, "默认 palette 读取失败");
        return false;
    }

    defaultPalette_ = paletteResult.palette;
    binSets_.reserve(binSets.size());

    for (std::size_t index = 0; index < binSets.size(); ++index) {
        BinSetState state;
        state.desc = binSets[index];
        const int binSetIndex = static_cast<int>(index);

        if (!LoadGraphicIndex(binSetIndex, state.desc, state)) {
            return false;
        }
        if (!LoadAnimeIndex(binSetIndex, state.desc)) {
            return false;
        }

        binSets_.push_back(std::move(state));
    }

    stats_.binSetCount = static_cast<std::uint32_t>(binSets_.size());
    ClearError();
    return true;
}

void ResourceManager::Clear()
{
    binSets_.clear();
    graphicsByMapNo_.clear();
    graphicsByGlobalIndex_.clear();
    animesByNo_.clear();
    stats_ = ResourceStats {};
    nextGraphicGlobalIndex_ = 0;
    ClearError();
}

std::optional<GraphicRef> ResourceManager::FindGraphicByMapNo(int mapNo) const
{
    const auto iter = graphicsByMapNo_.find(mapNo);
    if (iter == graphicsByMapNo_.end()) {
        return std::nullopt;
    }

    return iter->second;
}

std::optional<GraphicRef> ResourceManager::FindGraphicBySeq(int binSetIndex, int seqNo) const
{
    if (binSetIndex < 0 || static_cast<std::size_t>(binSetIndex) >= binSets_.size()) {
        return std::nullopt;
    }

    const auto& state = binSets_[static_cast<std::size_t>(binSetIndex)];
    const auto iter = state.graphicsBySeq.find(seqNo);
    if (iter == state.graphicsBySeq.end()) {
        return std::nullopt;
    }

    return iter->second;
}

std::optional<GraphicRef> ResourceManager::FindGraphicByGlobalIndex(int globalIndex) const
{
    const auto iter = graphicsByGlobalIndex_.find(globalIndex);
    if (iter == graphicsByGlobalIndex_.end()) {
        return std::nullopt;
    }

    return iter->second;
}

std::optional<GraphicRef> ResourceManager::FindGraphicForFrame(const AnimationClip& clip, const AnimeFrame& frame) const
{
    if (clip.graphicStartIndex >= 0) {
        const std::optional<GraphicRef> ref = FindGraphicByGlobalIndex(clip.graphicStartIndex + frame.graphicSeqNo);
        if (ref.has_value()) {
            return ref;
        }
    }
    return FindGraphicBySeq(clip.binSetIndex, frame.graphicSeqNo);
}

std::optional<AnimeRef> ResourceManager::FindAnime(int animeNo) const
{
    const auto iter = animesByNo_.find(animeNo);
    if (iter == animesByNo_.end()) {
        return std::nullopt;
    }

    return iter->second;
}

std::optional<DecodedImage> ResourceManager::DecodeGraphic(const GraphicRef& ref)
{
    GraphicDecodeResult result = graphicDecoder_.DecodeFromFile(ref.info, ref.dataPath, defaultPalette_);
    if (!result) {
        SetError(ResourceErrorCode::DecodeFailed, result.path, ref.mapNo, result.message);
        return std::nullopt;
    }

    ClearError();
    return std::move(result.image);
}

std::optional<DecodedImage> ResourceManager::DecodeGraphicByMapNo(int mapNo)
{
    const std::optional<GraphicRef> ref = FindGraphicByMapNo(mapNo);
    if (!ref.has_value()) {
        SetError(ResourceErrorCode::MissingResource, {}, mapNo, "找不到 mapNo 对应 Graphic");
        return std::nullopt;
    }

    return DecodeGraphic(*ref);
}

std::optional<AnimationClip> ResourceManager::LoadAnimation(int animeNo)
{
    const std::optional<AnimeRef> ref = FindAnime(animeNo);
    if (!ref.has_value()) {
        SetError(ResourceErrorCode::MissingResource, {}, animeNo, "找不到 animeNo 对应 Anime");
        return std::nullopt;
    }

    AnimationClip clip;
    if (!LoadAnimationClip(ref->dataPath, ref->info, clip)) {
        SetError(ResourceErrorCode::DecodeFailed, ref->dataPath, animeNo, "Anime 数据读取失败");
        return std::nullopt;
    }

    clip.binSetIndex = ref->binSetIndex;
    if (ref->binSetIndex >= 0 && static_cast<std::size_t>(ref->binSetIndex) < binSets_.size()) {
        clip.graphicStartIndex = binSets_[static_cast<std::size_t>(ref->binSetIndex)].graphicStartIndex;
    }

    ClearError();
    return clip;
}

const ResourceStats& ResourceManager::Stats() const noexcept
{
    return stats_;
}

const ResourceError& ResourceManager::LastError() const noexcept
{
    return lastError_;
}

const Palette& ResourceManager::DefaultPalette() const noexcept
{
    return defaultPalette_;
}

bool ResourceManager::LoadGraphicIndex(int binSetIndex, const BinSetDesc& desc, BinSetState& state)
{
    if (IsEmptyPathPair(desc.graphicInfoPath, desc.graphicDataPath)) {
        AddWarning("binSet 未提供 Graphic 文件对，跳过 Graphic 索引");
        return true;
    }

    BinaryReader reader(desc.graphicInfoPath);
    if (!reader.IsOpen()) {
        SetError(ResourceErrorCode::FileOpenFailed, desc.graphicInfoPath, 0, "GraphicInfo 打开失败");
        return false;
    }
    if (reader.Size() % GraphicInfoRecordSize != 0) {
        SetError(ResourceErrorCode::InvalidFormat, desc.graphicInfoPath, 0, "GraphicInfo 文件长度不是 40 的倍数");
        return false;
    }

    const std::uint64_t count = reader.Size() / GraphicInfoRecordSize;
    state.graphicStartIndex = nextGraphicGlobalIndex_;
    int maxSeqNo = -1;
    for (std::uint64_t index = 0; index < count; ++index) {
        GraphicInfo info;
        if (!ReadGraphicInfo(reader, info)) {
            SetError(ResourceErrorCode::FileReadFailed, desc.graphicInfoPath, static_cast<int>(index), "GraphicInfo 记录读取失败");
            return false;
        }

        GraphicRef ref;
        ref.binSetIndex = binSetIndex;
        ref.globalIndex = state.graphicStartIndex + info.seqNo;
        ref.seqNo = info.seqNo;
        ref.mapNo = info.mapNo;
        ref.dataPath = desc.graphicDataPath;
        ref.info = info;
        maxSeqNo = std::max(maxSeqNo, info.seqNo);

        if (state.graphicsBySeq.contains(info.seqNo)) {
            ++stats_.graphicOverrideCount;
            AddWarning("同一 binSet 内 Graphic seqNo 重复，使用后者");
        }
        state.graphicsBySeq[info.seqNo] = ref;
        graphicsByGlobalIndex_[ref.globalIndex] = ref;

        if (info.mapNo > 0) {
            if (graphicsByMapNo_.contains(info.mapNo)) {
                ++stats_.graphicOverrideCount;
                AddWarning("Graphic mapNo 重复，使用后加载 binSet");
            }
            graphicsByMapNo_[info.mapNo] = ref;
        }

        ++stats_.graphicCount;
    }
    if (maxSeqNo >= 0) {
        nextGraphicGlobalIndex_ += maxSeqNo + 1;
    }

    cg::core::Log(cg::core::LogLevel::Info,
        "resource",
        "GraphicInfo loaded: " + ToLogPath(desc.graphicInfoPath) + ", count=" + std::to_string(count));
    return true;
}

bool ResourceManager::LoadAnimeIndex(int binSetIndex, const BinSetDesc& desc)
{
    if (IsEmptyPathPair(desc.animeInfoPath, desc.animeDataPath)) {
        AddWarning("binSet 未提供 Anime 文件对，跳过 Anime 索引");
        return true;
    }

    BinaryReader reader(desc.animeInfoPath);
    if (!reader.IsOpen()) {
        SetError(ResourceErrorCode::FileOpenFailed, desc.animeInfoPath, 0, "AnimeInfo 打开失败");
        return false;
    }
    if (reader.Size() % AnimeInfoRecordSize != 0) {
        SetError(ResourceErrorCode::InvalidFormat, desc.animeInfoPath, 0, "AnimeInfo 文件长度不是 12 的倍数");
        return false;
    }

    const std::uint64_t count = reader.Size() / AnimeInfoRecordSize;
    for (std::uint64_t index = 0; index < count; ++index) {
        AnimeInfo info;
        if (!ReadAnimeInfo(reader, info)) {
            SetError(ResourceErrorCode::FileReadFailed, desc.animeInfoPath, static_cast<int>(index), "AnimeInfo 记录读取失败");
            return false;
        }

        const int sourceAnimeNo = info.animeNo;
        const int animeNo = NormalizeAnimeNo(sourceAnimeNo);
        AnimeRef ref;
        ref.binSetIndex = binSetIndex;
        ref.animeNo = animeNo;
        ref.sourceAnimeNo = sourceAnimeNo;
        ref.dataPath = desc.animeDataPath;
        ref.info = info;

        if (animesByNo_.contains(animeNo)) {
            ++stats_.animeOverrideCount;
            AddWarning("Anime animeNo 重复，使用后加载 binSet");
        }
        animesByNo_[animeNo] = ref;
        if (sourceAnimeNo != animeNo) {
            animesByNo_[sourceAnimeNo] = ref;
        }
        ++stats_.animeCount;
    }

    cg::core::Log(cg::core::LogLevel::Info,
        "resource",
        "AnimeInfo loaded: " + ToLogPath(desc.animeInfoPath) + ", count=" + std::to_string(count));
    return true;
}

void ResourceManager::ClearError()
{
    lastError_ = ResourceError {};
}

void ResourceManager::SetError(ResourceErrorCode code, const std::filesystem::path& path, int resourceId, std::string message)
{
    lastError_.code = code;
    lastError_.path = path;
    lastError_.resourceId = resourceId;
    lastError_.message = std::move(message);
    ++stats_.errorCount;
    cg::core::Log(cg::core::LogLevel::Error, "resource", lastError_.message + ": " + ToLogPath(path));
}

void ResourceManager::AddWarning(std::string_view message)
{
    ++stats_.warningCount;
    cg::core::Log(cg::core::LogLevel::Warning, "resource", message);
}

} // namespace cg::resource
