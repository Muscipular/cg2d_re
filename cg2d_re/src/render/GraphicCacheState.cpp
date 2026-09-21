#include "cg/render/GraphicCacheState.hpp"

namespace cg::render {

std::array<std::uint32_t, 6> gGraphicCache4Queue {};

static_assert(sizeof(gGraphicCache4Queue) == 0x18);

// 原 [0x456D30,0x456D41)：仅清六个 W32 尾槽并返回零。
std::uint32_t ResetGraphicCache_456D30()
{
    gGraphicCache4Queue.fill(0u);
    return 0u;
}

} // namespace cg::render
