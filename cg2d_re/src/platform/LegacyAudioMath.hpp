#pragma once

#include "cg/audio/LegacyAudio.hpp"

#include <cstddef>
#include <cstdint>


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
