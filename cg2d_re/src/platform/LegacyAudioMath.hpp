#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::audio {

// 原103D1C0的50项记录；52CD40只写+04/+00，后三个S32保留。
struct SpatialAudioRecord final {
    std::uint32_t value00;
    std::int32_t value04;
    std::int32_t value08;
    std::int32_t value0C;
    std::int32_t value10;
};

static_assert(offsetof(SpatialAudioRecord, value00) == 0x00);
static_assert(offsetof(SpatialAudioRecord, value04) == 0x04);
static_assert(offsetof(SpatialAudioRecord, value08) == 0x08);
static_assert(offsetof(SpatialAudioRecord, value0C) == 0x0C);
static_assert(offsetof(SpatialAudioRecord, value10) == 0x10);
static_assert(sizeof(SpatialAudioRecord) == 0x14);

} // namespace cg::audio

namespace cg::platform {

// ACP01：历史X87符号由CPP/std数学提供；字段与发布序保持，不承诺x87精度。
extern "C" void Cg2dInitializeSoundTablesX87(
    std::int32_t* pan, std::int32_t* volume) noexcept;
extern "C" void Cg2dInitializeFixedMusicVolumeX87(std::int32_t* volume) noexcept;
extern "C" std::int32_t Cg2dComputeSpatialSoundX87(
    std::int32_t x, std::int32_t y, std::int32_t* distance) noexcept;
extern "C" std::int32_t Cg2dComputeSpatialScanQuotientX87(
    std::int32_t x, std::int32_t y) noexcept;
extern "C" void Cg2dScaleSpatialSourceX87(
    std::int32_t x, std::int32_t y, float* worldX, float* worldY) noexcept;
extern "C" std::int32_t Cg2dPublishSpatialProjectionX87(
    audio::SpatialAudioRecord* records, std::int32_t index,
    const float* screenX, const float* screenY) noexcept;

} // namespace cg::platform
