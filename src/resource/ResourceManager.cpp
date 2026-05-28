#include "ResourceManager.hpp"

#include "AnimationLoader.hpp"
#include "AutoMapRules.hpp"
#include "BinaryReader.hpp"
#include "core/Log.hpp"
#include "HiddenPaletteQueue.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace cg::resource {
namespace {

constexpr int AnimeNoBase = 100000;
constexpr int AutoMapNormalBegin = 1000;
constexpr int AutoMapNormalEnd = 19999;
constexpr int AutoMapNewBegin = 220000;
constexpr int AutoMapNewEnd = 265535;
constexpr std::uint64_t GraphicInfoRecordSize = sizeof(GraphicInfo);
constexpr std::size_t HiddenPaletteHeaderSize = 32;
constexpr std::size_t HiddenPaletteRecordHeaderSize = 8;
constexpr std::size_t HiddenPaletteRecordMinSize = 8;
constexpr std::size_t MaxHiddenPaletteTableSize = 64U * 1024U * 1024U;
static_assert(GraphicInfoRecordSize == 40);

[[nodiscard]] bool IsEmptyPathPair(const std::filesystem::path& first, const std::filesystem::path& second)
{
    return first.empty() || second.empty();
}

[[nodiscard]] std::int8_t ToI8(std::uint8_t value) noexcept
{
    return static_cast<std::int8_t>(value);
}

[[nodiscard]] int ComputeAnimeIndexNo(int sourceAnimeNo) noexcept
{
    return sourceAnimeNo >= AnimeNoBase ? sourceAnimeNo - AnimeNoBase : sourceAnimeNo;
}

[[nodiscard]] bool IsAutoMapColorMapNo(int mapNo) noexcept
{
    return (mapNo >= AutoMapNormalBegin && mapNo <= AutoMapNormalEnd)
        || (mapNo >= AutoMapNewBegin && mapNo <= AutoMapNewEnd);
}

[[nodiscard]] bool TryReadGraphicBlock(BinaryReader& reader, const GraphicRef& ref, std::vector<std::uint8_t>& block, std::string& message)
{
    if (ref.info.offset < 0 || ref.info.length <= 0) {
        message = "GraphicInfo offset/length 无效";
        return false;
    }

    const std::uint64_t offset = static_cast<std::uint64_t>(ref.info.offset);
    const std::uint64_t length = static_cast<std::uint64_t>(ref.info.length);
    if (offset > reader.Size()
        || length > reader.Size() - offset
        || length > std::numeric_limits<std::size_t>::max()) {
        message = "GraphicInfo 指向范围越过文件结尾";
        return false;
    }

    if (!reader.Seek(offset) || !reader.ReadBytes(block, static_cast<std::size_t>(length))) {
        message = reader.LastError().message.empty() ? "Graphic 数据读取失败" : reader.LastError().message;
        return false;
    }

    return true;
}

[[nodiscard]] bool TryReadU16Le(std::span<const std::uint8_t> bytes, std::size_t offset, std::uint16_t& value)
{
    if (offset > bytes.size() || bytes.size() - offset < sizeof(std::uint16_t)) {
        return false;
    }

    value = static_cast<std::uint16_t>(bytes[offset])
        | static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[offset + 1]) << 8U);
    return true;
}

[[nodiscard]] bool TryReadU32Le(std::span<const std::uint8_t> bytes, std::size_t offset, std::uint32_t& value)
{
    if (offset > bytes.size() || bytes.size() - offset < sizeof(std::uint32_t)) {
        return false;
    }

    value = static_cast<std::uint32_t>(bytes[offset])
        | (static_cast<std::uint32_t>(bytes[offset + 1]) << 8U)
        | (static_cast<std::uint32_t>(bytes[offset + 2]) << 16U)
        | (static_cast<std::uint32_t>(bytes[offset + 3]) << 24U);
    return true;
}

[[nodiscard]] std::int32_t U32ToI32(std::uint32_t value) noexcept
{
    if (value <= static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max())) {
        return static_cast<std::int32_t>(value);
    }

    return static_cast<std::int32_t>(static_cast<std::int64_t>(value) - 0x100000000LL);
}

[[nodiscard]] bool TryReadI32Le(std::span<const std::uint8_t> bytes, std::size_t offset, std::int32_t& value)
{
    std::uint32_t unsignedValue = 0;
    if (!TryReadU32Le(bytes, offset, unsignedValue)) {
        return false;
    }

    value = U32ToI32(unsignedValue);
    return true;
}

[[nodiscard]] bool DecompressLzo1x(
    std::span<const std::uint8_t> input,
    std::size_t outputCapacity,
    std::vector<std::uint8_t>& output,
    std::string& message)
{
    output.assign(outputCapacity, 0);
    std::size_t inputOffset = 0;
    std::size_t outputOffset = 0;

    const auto fail = [&message](std::string text) {
        message = std::move(text);
        return false;
    };
    const auto readByte = [&input, &inputOffset, &fail](std::uint8_t& value) {
        if (inputOffset >= input.size()) {
            return fail("LZO 数据提前结束");
        }
        value = input[inputOffset++];
        return true;
    };
    const auto readU16 = [&input, &inputOffset, &fail](std::uint16_t& value) {
        if (!TryReadU16Le(input, inputOffset, value)) {
            return fail("LZO 数据提前结束");
        }
        inputOffset += sizeof(std::uint16_t);
        return true;
    };
    const auto copyLiteral = [&input, &inputOffset, &output, &outputOffset, &fail](std::size_t count) {
        if (inputOffset > input.size() || count > input.size() - inputOffset) {
            return fail("LZO literal 越过输入结尾");
        }
        if (outputOffset > output.size() || count > output.size() - outputOffset) {
            return fail("LZO literal 越过输出缓冲");
        }

        std::copy_n(input.begin() + static_cast<std::ptrdiff_t>(inputOffset),
            count,
            output.begin() + static_cast<std::ptrdiff_t>(outputOffset));
        inputOffset += count;
        outputOffset += count;
        return true;
    };
    const auto copyMatch = [&output, &outputOffset, &fail](std::size_t distance, std::size_t count) {
        if (distance == 0 || distance > outputOffset) {
            return fail("LZO match 引用无效");
        }
        if (outputOffset > output.size() || count > output.size() - outputOffset) {
            return fail("LZO match 越过输出缓冲");
        }

        std::size_t source = outputOffset - distance;
        for (std::size_t index = 0; index < count; ++index) {
            output[outputOffset++] = output[source++];
        }
        return true;
    };
    const auto readExtendedLength = [&readByte](std::size_t& length, std::size_t base) {
        std::uint8_t value = 0;
        if (!readByte(value)) {
            return false;
        }
        while (value == 0) {
            length += 255U;
            if (!readByte(value)) {
                return false;
            }
        }
        length += static_cast<std::size_t>(value) + base;
        return true;
    };
    const auto afterMatch = [&input, &inputOffset, &copyLiteral, &fail](bool& readLiteralRun) {
        if (inputOffset < 2U) {
            return fail("LZO match 后状态无效");
        }

        const std::size_t literalCount = input[inputOffset - 2U] & 0x03U;
        if (literalCount == 0) {
            readLiteralRun = true;
            return true;
        }

        if (!copyLiteral(literalCount)) {
            return false;
        }
        readLiteralRun = false;
        return true;
    };

    if (input.empty()) {
        return fail("LZO 输入为空");
    }

    bool readLiteralRun = true;
    if (input[0] > 17U) {
        std::uint8_t first = 0;
        if (!readByte(first)) {
            return false;
        }
        const std::size_t literalCount = static_cast<std::size_t>(first) - 17U;
        if (!copyLiteral(literalCount)) {
            return false;
        }
        readLiteralRun = false;
    }

    while (true) {
        std::uint8_t control = 0;
        if (readLiteralRun) {
            if (!readByte(control)) {
                return false;
            }

            std::size_t literalCount = control;
            if (literalCount < 16U) {
                if (literalCount == 0 && !readExtendedLength(literalCount, 15U)) {
                    return false;
                }
                if (!copyLiteral(literalCount + 3U) || !readByte(control)) {
                    return false;
                }
            }
        } else if (!readByte(control)) {
            return false;
        }

        if (control < 16U) {
            std::uint8_t next = 0;
            if (!readByte(next)
                || !copyMatch((static_cast<std::size_t>(control) >> 2U) + 4U * next + 2049U, 3U)
                || !afterMatch(readLiteralRun)) {
                return false;
            }
            if (readLiteralRun) {
                continue;
            }
            if (!readByte(control)) {
                return false;
            }
        }

        while (true) {
            if (control >= 64U) {
                std::uint8_t next = 0;
                if (!readByte(next)
                    || !copyMatch(((static_cast<std::size_t>(control) >> 2U) & 0x07U) + 1U + 8U * next,
                        (static_cast<std::size_t>(control) >> 5U) + 1U)) {
                    return false;
                }
            } else if (control >= 32U) {
                std::size_t matchLength = control & 0x1FU;
                if (matchLength == 0 && !readExtendedLength(matchLength, 31U)) {
                    return false;
                }

                std::uint16_t pair = 0;
                if (!readU16(pair)
                    || !copyMatch((static_cast<std::size_t>(pair) >> 2U) + 1U, matchLength + 2U)) {
                    return false;
                }
            } else if (control >= 16U) {
                std::size_t matchLength = control & 0x07U;
                if (matchLength == 0 && !readExtendedLength(matchLength, 7U)) {
                    return false;
                }

                std::uint16_t pair = 0;
                if (!readU16(pair)) {
                    return false;
                }

                const std::size_t baseDistance = 2048U * (control & 0x08U)
                    + (static_cast<std::size_t>(pair) >> 2U);
                if (baseDistance == 0) {
                    if (inputOffset != input.size()) {
                        return fail("LZO 结束标记后仍有数据");
                    }
                    output.resize(outputOffset);
                    return true;
                }
                if (baseDistance > std::numeric_limits<std::size_t>::max() - 0x4000U
                    || !copyMatch(baseDistance + 0x4000U, matchLength + 2U)) {
                    return false;
                }
            } else {
                std::uint8_t next = 0;
                if (!readByte(next)
                    || !copyMatch((static_cast<std::size_t>(control) >> 2U) + 1U + 4U * next, 2U)) {
                    return false;
                }
            }

            if (!afterMatch(readLiteralRun)) {
                return false;
            }
            if (readLiteralRun) {
                break;
            }
            if (!readByte(control)) {
                return false;
            }
        }
    }
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

bool ResourceManager::LoadBinSets(
    std::span<const BinSetDesc> binSets,
    const std::filesystem::path& defaultPalettePath,
    ResourceLoadOptions options)
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

        if (!LoadGraphicIndex(binSetIndex, state.desc, state, options)) {
            return false;
        }
        if (!LoadAnimeIndex(binSetIndex, state.desc)) {
            return false;
        }

        binSets_.push_back(std::move(state));
    }

    LoadHiddenPaletteTable(options);
    stats_.binSetCount = static_cast<std::uint32_t>(binSets_.size());
    ClearError();
    return true;
}

void ResourceManager::Clear()
{
    binSets_.clear();
    graphicsByMapNo_.clear();
    autoMapColorByMapNo_.clear();
    graphicsByGlobalIndex_.clear();
    hiddenPaletteMapNoByPalIndex_.clear();
    animesBySourceAnimeNo_.clear();
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

std::optional<std::uint8_t> ResourceManager::FindAutoMapColorIndexByMapNo(int mapNo) const
{
    const auto iter = autoMapColorByMapNo_.find(mapNo);
    if (iter == autoMapColorByMapNo_.end()) {
        return std::nullopt;
    }

    return iter->second;
}

std::optional<std::uint8_t> ResourceManager::FindAutoMapColorIndexByRaw(std::uint32_t raw) const
{
    const std::uint32_t mapNo = DecodeMapCell(raw);
    if (mapNo > static_cast<std::uint32_t>(std::numeric_limits<int>::max())) {
        return std::nullopt;
    }

    return FindAutoMapColorIndexByMapNo(static_cast<int>(mapNo));
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

std::optional<int> ResourceManager::FindHiddenPaletteMapNoByPalIndex(int palIndex) const
{
    if (palIndex < 0 || static_cast<std::size_t>(palIndex) >= hiddenPaletteMapNoByPalIndex_.size()) {
        return std::nullopt;
    }

    const int mapNo = hiddenPaletteMapNoByPalIndex_[static_cast<std::size_t>(palIndex)];
    if (mapNo <= 0) {
        return std::nullopt;
    }
    return mapNo;
}

std::optional<AnimeRef> ResourceManager::FindAnime(int sourceAnimeNo) const
{
    const auto iter = animesBySourceAnimeNo_.find(sourceAnimeNo);
    if (iter == animesBySourceAnimeNo_.end()) {
        return std::nullopt;
    }

    return iter->second;
}

std::span<const GraphicRef> ResourceManager::Graphics(int binSetIndex) const noexcept
{
    if (binSetIndex < 0 || static_cast<std::size_t>(binSetIndex) >= binSets_.size()) {
        return {};
    }

    return binSets_[static_cast<std::size_t>(binSetIndex)].graphics;
}

std::optional<DecodedImage> ResourceManager::DecodeGraphic(const GraphicRef& ref)
{
    return DecodeGraphic(ref, defaultPalette_);
}

std::optional<DecodedImage> ResourceManager::DecodeGraphic(const GraphicRef& ref, const Palette& palette)
{
    GraphicDecodeResult result = graphicDecoder_.DecodeFromFile(ref.info, ref.dataPath, palette);
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

std::optional<Palette> ResourceManager::LoadHiddenPaletteByMapNo(int mapNo)
{
    const std::optional<GraphicRef> ref = FindGraphicByMapNo(mapNo);
    if (!ref.has_value()) {
        SetError(ResourceErrorCode::MissingResource, {}, mapNo, "隐藏调色盘 mapNo 找不到 Graphic");
        return std::nullopt;
    }

    GraphicPaletteDecodeResult result =
        graphicDecoder_.DecodeEmbeddedPaletteFromFile(ref->info, ref->dataPath, defaultPalette_);
    if (!result) {
        SetError(ResourceErrorCode::DecodeFailed, result.path, mapNo, result.message);
        return std::nullopt;
    }

    ClearError();
    return result.palette;
}

std::optional<Palette> ResourceManager::LoadHiddenPaletteByPalIndex(int palIndex)
{
    const std::optional<int> mapNo = FindHiddenPaletteMapNoByPalIndex(palIndex);
    if (!mapNo.has_value()) {
        return std::nullopt;
    }

    return LoadHiddenPaletteByMapNo(*mapNo);
}

std::optional<AnimationClip> ResourceManager::LoadAnimation(int sourceAnimeNo)
{
    const std::optional<AnimeRef> ref = FindAnime(sourceAnimeNo);
    if (!ref.has_value()) {
        SetError(ResourceErrorCode::MissingResource, {}, sourceAnimeNo, "找不到原始 animeNo 对应 Anime");
        return std::nullopt;
    }

    AnimationClip clip;
    if (!LoadAnimationClip(ref->dataPath, ref->info, clip)) {
        SetError(ResourceErrorCode::DecodeFailed, ref->dataPath, sourceAnimeNo, "Anime 数据读取失败");
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

bool ResourceManager::LoadGraphicIndex(
    int binSetIndex,
    const BinSetDesc& desc,
    BinSetState& state,
    const ResourceLoadOptions& options)
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
    if (count <= static_cast<std::uint64_t>(std::numeric_limits<std::size_t>::max())) {
        const std::size_t recordCount = static_cast<std::size_t>(count);
        state.graphics.reserve(recordCount);
        state.graphicsBySeq.reserve(recordCount);
        if (recordCount <= std::numeric_limits<std::size_t>::max() - graphicsByGlobalIndex_.size()) {
            graphicsByGlobalIndex_.reserve(graphicsByGlobalIndex_.size() + recordCount);
        }
        if (recordCount <= std::numeric_limits<std::size_t>::max() - graphicsByMapNo_.size()) {
            graphicsByMapNo_.reserve(graphicsByMapNo_.size() + recordCount);
        }
        if (options.buildAutoMapColorCache
            && recordCount <= std::numeric_limits<std::size_t>::max() - autoMapColorByMapNo_.size()) {
            autoMapColorByMapNo_.reserve(autoMapColorByMapNo_.size() + recordCount);
        }
    }
    BinaryReader graphicDataReader;
    bool graphicDataOpenTried = false;
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
        state.graphics.push_back(ref);

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
        if (options.buildAutoMapColorCache) {
            BuildAutoMapColorIndex(ref, graphicDataReader, graphicDataOpenTried);
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

void ResourceManager::BuildAutoMapColorIndex(const GraphicRef& ref, BinaryReader& graphicDataReader, bool& graphicDataOpenTried)
{
    if (!IsAutoMapColorMapNo(ref.mapNo)) {
        return;
    }

    // twcg_hook 普通 MapNoIndex 与 newMapRes 均以 0 作为缺省颜色；后加载记录覆盖前者。
    autoMapColorByMapNo_[ref.mapNo] = 0;

    if (!graphicDataOpenTried) {
        graphicDataOpenTried = true;
        if (!graphicDataReader.Open(ref.dataPath)) {
            AddWarning("自动地图颜色索引打开 Graphic 数据失败，保留缺省颜色 0");
            return;
        }
    }
    if (!graphicDataReader.IsOpen()) {
        return;
    }

    std::vector<std::uint8_t> block;
    std::string message;
    if (!TryReadGraphicBlock(graphicDataReader, ref, block, message)) {
        AddWarning("自动地图颜色索引读取 Graphic 失败，mapNo=" + std::to_string(ref.mapNo) + "，" + message);
        return;
    }

    GraphicIndexedDecodeResult result = graphicDecoder_.DecodeGraphicIndexed(ref.info, block);
    if (!result) {
        AddWarning("自动地图颜色索引解码 Graphic 失败，mapNo=" + std::to_string(ref.mapNo) + "，" + result.message);
        return;
    }

    autoMapColorByMapNo_[ref.mapNo] = ComputeAutoMapColorIndex(result.image.pixels, defaultPalette_);
}

void ResourceManager::LoadHiddenPaletteTable(const ResourceLoadOptions& options)
{
    hiddenPaletteMapNoByPalIndex_.clear();
    stats_.hiddenPaletteCount = 0;

    const auto staticQueue = BuildStaticHiddenPaletteQueue();
    hiddenPaletteMapNoByPalIndex_.assign(staticQueue.begin(), staticQueue.end());
    for (const int mapNo : hiddenPaletteMapNoByPalIndex_) {
        if (mapNo > 0) {
            ++stats_.hiddenPaletteCount;
        }
    }

    if (options.hiddenPaletteTablePath.empty()) {
        return;
    }

    BinaryReader reader(options.hiddenPaletteTablePath);
    if (!reader.IsOpen()) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf 打开失败，跳过追加区");
        return;
    }
    if (reader.Size() < HiddenPaletteHeaderSize) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf 文件过短，跳过追加区");
        return;
    }

    std::vector<std::uint8_t> header;
    if (!reader.ReadBytes(header, HiddenPaletteHeaderSize)) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf 头读取失败，跳过追加区");
        return;
    }
    if (header[0] != 'D' || header[1] != 'B' || header[2] != 'F' || header[3] != 0) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf magic 无效，跳过追加区");
        return;
    }

    std::uint32_t outputSize = 0;
    std::uint32_t inputSize = 0;
    if (!TryReadU32Le(header, 4, outputSize) || !TryReadU32Le(header, 8, inputSize)
        || outputSize == 0 || inputSize == 0
        || outputSize > MaxHiddenPaletteTableSize
        || inputSize > MaxHiddenPaletteTableSize) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf 压缩尺寸无效，跳过追加区");
        return;
    }
    if (reader.Size() - HiddenPaletteHeaderSize < inputSize) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf 压缩数据越界，跳过追加区");
        return;
    }

    std::vector<std::uint8_t> compressed;
    if (!reader.ReadBytes(compressed, inputSize)) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf 压缩数据读取失败，跳过追加区");
        return;
    }

    std::vector<std::uint8_t> decoded;
    std::string message;
    if (!DecompressLzo1x(compressed, outputSize, decoded, message)) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf 解压失败，跳过追加区：" + message);
        return;
    }
    if (decoded.size() < HiddenPaletteRecordHeaderSize) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf 解压数据过短，跳过追加区");
        return;
    }

    std::uint32_t recordCount = 0;
    std::uint32_t recordSize = 0;
    if (!TryReadU32Le(decoded, 0, recordCount) || !TryReadU32Le(decoded, 4, recordSize)
        || recordSize < HiddenPaletteRecordMinSize) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf 记录头无效，跳过追加区");
        return;
    }

    const std::size_t payloadSize = decoded.size() - HiddenPaletteRecordHeaderSize;
    if (recordCount > payloadSize / recordSize) {
        AddWarning("隐藏调色盘 CGLoadSpr.dbf 记录范围越界，跳过追加区");
        return;
    }

    std::map<int, int> mapNoByKey;
    for (std::uint32_t index = 0; index < recordCount; ++index) {
        const std::size_t offset = HiddenPaletteRecordHeaderSize
            + static_cast<std::size_t>(index) * recordSize;
        std::int32_t key = 0;
        std::int32_t mapNo = 0;
        if (!TryReadI32Le(decoded, offset, key) || !TryReadI32Le(decoded, offset + 4U, mapNo)) {
            AddWarning("隐藏调色盘 CGLoadSpr.dbf 记录读取失败，跳过追加区");
            return;
        }
        if (mapNo > 0) {
            mapNoByKey[key] = mapNo;
        }
    }

    hiddenPaletteMapNoByPalIndex_.reserve(hiddenPaletteMapNoByPalIndex_.size() + mapNoByKey.size());
    for (const auto& [key, mapNo] : mapNoByKey) {
        (void)key;
        hiddenPaletteMapNoByPalIndex_.push_back(mapNo);
        ++stats_.hiddenPaletteCount;
    }
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
        const int indexNo = ComputeAnimeIndexNo(sourceAnimeNo);
        AnimeRef ref;
        ref.binSetIndex = binSetIndex;
        ref.indexNo = indexNo;
        ref.sourceAnimeNo = sourceAnimeNo;
        ref.dataPath = desc.animeDataPath;
        ref.info = info;

        if (animesBySourceAnimeNo_.contains(sourceAnimeNo)) {
            ++stats_.animeOverrideCount;
            AddWarning("Anime 原始 animeNo 重复，使用后加载 binSet");
        }
        animesBySourceAnimeNo_[sourceAnimeNo] = ref;
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
