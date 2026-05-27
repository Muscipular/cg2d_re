#pragma once

#include "ResourceTypes.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>

namespace cg::resource {

inline constexpr std::size_t AnimeInfoRecordSize = 12;
inline constexpr std::size_t AnimeFrameRecordSize = 10;

class BinaryReader;

// ReadAnimeInfo 读取 AnimeInfo_*.bin 中单条 12 字节记录。
[[nodiscard]] bool ReadAnimeInfo(std::span<const std::uint8_t> row, AnimeInfo& outInfo) noexcept;
[[nodiscard]] bool ReadAnimeInfo(BinaryReader& reader, AnimeInfo& outInfo);

// LoadAnimationClip 按 AnimeInfo.offset/count 从 Anime data 数据读取动作与帧。
[[nodiscard]] bool LoadAnimationClip(BinaryReader& reader,
    const AnimeInfo& info,
    AnimationClip& outClip);
[[nodiscard]] bool LoadAnimationClip(const std::filesystem::path& animeDataPath,
    const AnimeInfo& info,
    AnimationClip& outClip);

} // namespace cg::resource
