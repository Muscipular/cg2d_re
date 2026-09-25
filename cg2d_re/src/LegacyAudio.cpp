#include "cg/LegacyAudio.hpp"

#include "cg/platform/CommandOptions.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <mmsystem.h>

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
volatile std::uint32_t gMciDeviceId = 0;
std::uint32_t gMciCloseResult = 0;

std::uint32_t Sub_52CCE0() noexcept
{
    if (gValue103DE80 == 1) {
        return 1;
    }

    mciSendCommandA(gMciDeviceId, MCI_STOP, 0, 0);
    const std::uint32_t result = mciSendCommandA(gMciDeviceId, MCI_CLOSE, 0, 0);
    gMciCloseResult = result;
    return (result & 0xFFFFFF00u) | static_cast<std::uint32_t>(result == 0);
}

SpatialAudioRecord* __cdecl Sub_52CD40()
{
    for (auto& record : gSpatialRecords103D1C0) {
        record.value04 = -1;
        record.value00 = 1;
    }
    return gSpatialRecords103D1C0.data() + gSpatialRecords103D1C0.size();
}

// 原 [0x52CDE0,0x52CE97)；在同一空间音频表发布更近的屏幕坐标与距离。
std::int32_t __cdecl Sub_52CDE0(
    const std::int32_t index,
    const std::int32_t sourceX,
    const std::int32_t sourceY)
{
    const float worldY = static_cast<float>(static_cast<double>(sourceY) * 64.0);
    const float worldX = static_cast<float>(static_cast<double>(sourceX) * 64.0);
    float screenX;
    float screenY;
    world::ProjectWorldToScreen_46CE30(worldX, worldY, &screenX, &screenY);

    const double offsetX = static_cast<double>(screenX) - 320.0;
    const double offsetY = static_cast<double>(screenY) - 240.0;
    const std::int32_t distance = static_cast<std::int32_t>(
        std::sqrt(offsetY * offsetY + offsetX * offsetX));

    SpatialAudioRecord& record =
        gSpatialRecords103D1C0[static_cast<std::size_t>(index)];
    if (record.value00 == 0 && record.value04 != -1
        && distance >= record.value10) {
        return index;
    }

    const double projectedX = static_cast<double>(screenX);
    record.value04 = index;
    const std::int32_t x = static_cast<std::int32_t>(projectedX);
    const double projectedY = static_cast<double>(screenY);
    record.value08 = x;
    const std::int32_t y = static_cast<std::int32_t>(projectedY);
    record.value0C = y;
    record.value10 = distance;
    record.value00 = 0;
    return y;
}

// 原 [0x52CD60,0x52CDDB)；首个距离阈值不符的有效记录立即结束扫描。
void __cdecl Sub_52CD60()
{
    for (SpatialAudioRecord& record : gSpatialRecords103D1C0) {
        const std::int32_t id = record.value04;
        if (id == -1) {
            continue;
        }

        const std::int32_t x = record.value08;
        const std::int32_t y = record.value0C;
        const std::uint32_t sumBits =
            static_cast<std::uint32_t>(x) * static_cast<std::uint32_t>(x)
            + static_cast<std::uint32_t>(y) * static_cast<std::uint32_t>(y);
        const std::int32_t distance = static_cast<std::int32_t>(
            std::sqrt(static_cast<double>(std::bit_cast<std::int32_t>(sumBits))));
        const std::int32_t quotient = (distance * 16) / 106;
        if (quotient >= 127 || quotient < 20) {
            return;
        }

        static_cast<void>(Sub_52C9E0(id + 249, x, y));
        record.value00 = 1;
    }
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
