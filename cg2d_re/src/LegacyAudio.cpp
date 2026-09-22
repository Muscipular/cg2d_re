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

std::uint32_t gValue103DE80 = 0;

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
