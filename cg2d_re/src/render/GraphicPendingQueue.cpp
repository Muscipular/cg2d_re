#include "cg/render/GraphicPendingQueue.hpp"

#include "cg/platform/CommandOptions.hpp"

#include "../LoginMessageDependencies.hpp"

#include <bit>

namespace cg::render {

LegacyPendingStorage gLegacyPendingStorage {};
std::uint8_t gSpecialGraphicPending = 0;
std::int32_t gGraphicDirty = 0;
float gFloat1020EF0 = 0.0f;

void Sub_520910()
{
    gFloat1020EF0 += 0.25f;
}

} // namespace cg::render

namespace cg::platform {

char (&gTextFF4E30)[0x88] = render::gLegacyPendingStorage.prefix;

} // namespace cg::platform

namespace cg::login::detail {

void SetPendingGraphicBatchTypeAtLegacyIndex(
    const std::int32_t index,
    const std::uint8_t type)
{
    if (index == -2) {
        // 原 records[-2].batchType 正好落到 0xFF4E8C，即 prefix+0x5C。
        render::gLegacyPendingStorage.prefix[0x5C] = std::bit_cast<char>(type);
        return;
    }
    if (index < 0 || index >= 4096) {
        return;
    }

    render::gLegacyPendingStorage.records[static_cast<std::size_t>(index)].batchType = type;
}

} // namespace cg::login::detail
