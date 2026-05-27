#include "AnimationLoader.hpp"

#include "BinaryReader.hpp"

#include <bit>
#include <limits>
#include <utility>

namespace cg::resource {
namespace {

constexpr int AnimeNoBase = 100000;

[[nodiscard]] std::uint16_t ReadU16(std::span<const std::uint8_t> data, std::size_t offset) noexcept
{
    return static_cast<std::uint16_t>(data[offset])
        | static_cast<std::uint16_t>(static_cast<std::uint16_t>(data[offset + 1]) << 8U);
}

[[nodiscard]] std::uint32_t ReadU32(std::span<const std::uint8_t> data, std::size_t offset) noexcept
{
    return static_cast<std::uint32_t>(data[offset])
        | (static_cast<std::uint32_t>(data[offset + 1]) << 8U)
        | (static_cast<std::uint32_t>(data[offset + 2]) << 16U)
        | (static_cast<std::uint32_t>(data[offset + 3]) << 24U);
}

[[nodiscard]] std::int32_t ReadI32(std::span<const std::uint8_t> data, std::size_t offset) noexcept
{
    return std::bit_cast<std::int32_t>(ReadU32(data, offset));
}

[[nodiscard]] bool ReadAnimeAction(BinaryReader& reader, bool hasExtendedHeader, int frameIndex, AnimeAction& action)
{
    std::int16_t direction = 0;
    std::int16_t actionNo = 0;
    std::int32_t duration = 0;
    std::int32_t frameCount = 0;
    std::int16_t palette = 0;
    std::int16_t revertRaw = 0;
    std::uint32_t padding = 0;

    if (!reader.ReadI16(direction) || !reader.ReadI16(actionNo) || !reader.ReadI32(duration)
        || !reader.ReadI32(frameCount)) {
        return false;
    }
    if (frameCount < 0) {
        return false;
    }

    if (hasExtendedHeader
        && (!reader.ReadI16(palette) || !reader.ReadI16(revertRaw) || !reader.ReadU32(padding))) {
            return false;
    }

    action.dir = direction;
    action.action = actionNo;
    action.duration = duration;
    action.frameIndex = frameIndex;
    action.frameCount = frameCount;
    action.pal = palette;
    action.revertRaw = revertRaw;
    action.revert = revertRaw != 0;
    action.padding = padding;
    action.header.dir = direction;
    action.header.action = actionNo;
    action.header.duration = duration;
    action.header.frameCount = frameCount;
    action.header.pal = palette;
    action.header.revert = revertRaw;
    action.header.padding = padding;
    action.header.headerSize = hasExtendedHeader ? 20U : 12U;
    action.header.hasExtendedFields = hasExtendedHeader;
    return true;
}

[[nodiscard]] bool ReadAnimeFrame(BinaryReader& reader, AnimeFrame& frame)
{
    std::int32_t graphicSeqNo = 0;
    std::int16_t x = 0;
    std::int16_t y = 0;
    std::int16_t flag = 0;

    if (!reader.ReadI32(graphicSeqNo) || !reader.ReadI16(x) || !reader.ReadI16(y) || !reader.ReadI16(flag)) {
        return false;
    }

    frame.graphicSeqNo = graphicSeqNo;
    frame.x = x;
    frame.y = y;
    frame.flag = flag;
    return true;
}

} // namespace

bool ReadAnimeInfo(std::span<const std::uint8_t> row, AnimeInfo& outInfo) noexcept
{
    if (row.size() < AnimeInfoRecordSize) {
        return false;
    }

    const std::span<const std::uint8_t> record = row.first(AnimeInfoRecordSize);
    outInfo.sourceAnimeNo = ReadI32(record, 0);
    outInfo.animeNo = outInfo.sourceAnimeNo >= AnimeNoBase ? outInfo.sourceAnimeNo - AnimeNoBase : outInfo.sourceAnimeNo;
    outInfo.offset = ReadU32(record, 4);
    outInfo.count = ReadU16(record, 8);
    outInfo.flag = ReadU16(record, 10);
    return true;
}

bool ReadAnimeInfo(BinaryReader& reader, AnimeInfo& outInfo)
{
    std::int32_t animeNo = 0;
    std::uint32_t offset = 0;
    std::uint16_t count = 0;
    std::uint16_t flag = 0;

    if (!reader.ReadI32(animeNo) || !reader.ReadU32(offset) || !reader.ReadU16(count) || !reader.ReadU16(flag)) {
        return false;
    }

    outInfo.sourceAnimeNo = animeNo;
    outInfo.animeNo = animeNo >= AnimeNoBase ? animeNo - AnimeNoBase : animeNo;
    outInfo.offset = offset;
    outInfo.count = count;
    outInfo.flag = flag;
    return true;
}

bool LoadAnimationClip(BinaryReader& reader, const AnimeInfo& info, AnimationClip& outClip)
{
    if (!reader.Seek(info.offset)) {
        return false;
    }

    AnimationClip clip;
    clip.animeNo = info.animeNo;
    clip.actions.reserve(info.count);
    const bool hasExtendedHeader = info.flag == 1;

    for (std::uint16_t actionIndex = 0; actionIndex < info.count; ++actionIndex) {
        AnimeAction action;
        if (!ReadAnimeAction(reader, hasExtendedHeader, static_cast<int>(clip.frames.size()), action)) {
            return false;
        }

        clip.actions.push_back(action);
        if (action.frameCount > std::numeric_limits<std::size_t>::max() / AnimeFrameRecordSize) {
            return false;
        }

        for (int frameIndex = 0; frameIndex < action.frameCount; ++frameIndex) {
            AnimeFrame frame;
            if (!ReadAnimeFrame(reader, frame)) {
                return false;
            }

            clip.frames.push_back(frame);
        }
    }

    outClip = std::move(clip);
    return true;
}

bool LoadAnimationClip(const std::filesystem::path& animeDataPath, const AnimeInfo& info, AnimationClip& outClip)
{
    BinaryReader reader(animeDataPath);
    if (!reader.IsOpen()) {
        return false;
    }

    return LoadAnimationClip(reader, info, outClip);
}

const AnimeAction* FindAnimeAction(const AnimationClip& clip, int action, int dir) noexcept
{
    for (const AnimeAction& candidate : clip.actions) {
        if (candidate.action == action && candidate.dir == dir) {
            return &candidate;
        }
    }

    return nullptr;
}

std::span<const AnimeFrame> GetAnimeActionFrames(const AnimationClip& clip, const AnimeAction& action) noexcept
{
    if (action.frameIndex < 0 || action.frameCount < 0) {
        return {};
    }

    const auto frameIndex = static_cast<std::size_t>(action.frameIndex);
    const auto frameCount = static_cast<std::size_t>(action.frameCount);
    if (frameIndex > clip.frames.size() || frameCount > clip.frames.size() - frameIndex) {
        return {};
    }
    if (frameCount == 0) {
        return {};
    }

    return std::span<const AnimeFrame>(clip.frames.data() + frameIndex, frameCount);
}

} // namespace cg::resource
