#include "cg/render/GraphicCacheState.hpp"

namespace cg::render {

std::array<std::uint32_t, 6> gGraphicCache4Queue {}; // 原全局：0x0062ECA0。

static_assert(sizeof(gGraphicCache4Queue) == 0x18);

// 原函数：0x00456D50。
std::int32_t ClearTextQueueTail_456D50(const std::int8_t mode)
{
    gGraphicCache4Queue[mode] = 0;
    return mode;
}

// 原 [0x456D30,0x456D41)：仅清六个 W32 尾槽并返回零。
std::uint32_t ResetGraphicCache_456D30()
{
    gGraphicCache4Queue.fill(0u);
    return 0u;
}

} // namespace cg::render
