#include "cg/LegacyAudio.hpp"

#include "cg/platform/CommandOptions.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace {

struct LegacyAudioRecord final {
    std::int32_t value00;
    std::array<std::byte, 0x48> opaque;
};

static_assert(offsetof(LegacyAudioRecord, value00) == 0x00);
static_assert(sizeof(LegacyAudioRecord) == 0x4C);

constexpr std::uint32_t kRecordCount = 128;

std::int32_t gRecordIndex = -1;
std::array<LegacyAudioRecord, kRecordCount> gRecords {};
std::int32_t gFlag = 0;
std::int32_t gQuotient = 0;

static_assert(sizeof(gRecords) == 0x2600);

} // namespace

namespace cg::audio {

// 原 0x103D1C0 的 50 项记录；52CD40 只写 +0x04/+0x00，后三个 S32 保留。
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

namespace {

std::array<SpatialAudioRecord, 50> gSpatialRecords103D1C0 {};
static_assert(sizeof(gSpatialRecords103D1C0) == 0x3E8);

} // namespace

std::uint32_t gValue103DE80 = 0;

SpatialAudioRecord* __cdecl Sub_52CD40()
{
    for (auto& record : gSpatialRecords103D1C0) {
        record.value04 = -1;
        record.value00 = 1;
    }
    return gSpatialRecords103D1C0.data() + gSpatialRecords103D1C0.size();
}

std::uint32_t Sub_52B990() noexcept
{
    const std::int32_t index = gRecordIndex;
    if (index < 0 || static_cast<std::uint32_t>(index) >= kRecordCount) {
        return std::bit_cast<std::uint32_t>(index);
    }

    gFlag = 1;
    const std::int32_t value00 = gRecords[static_cast<std::size_t>(index)].value00;
    const std::int32_t musicVolume = cg::platform::gMusicVolume;
    const std::uint32_t productBits =
        std::bit_cast<std::uint32_t>(value00)
        * std::bit_cast<std::uint32_t>(musicVolume);
    const std::int32_t product = std::bit_cast<std::int32_t>(productBits);
    gQuotient = product / 15;
    return productBits >> 31;
}

} // namespace cg::audio
