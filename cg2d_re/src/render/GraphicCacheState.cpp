#include "cg/render/GraphicCacheState.hpp"
#include "cg/render/TextPendingQueue.hpp"

namespace cg::render {

// 原函数：0x00456D50。
std::int32_t ClearTextQueueTail_456D50(const std::int8_t mode)
{
    cg::render::gTextQueue.tails[mode] = 0;
    return mode;
}

// 原 [0x456D30,0x456D41)：仅清六个 W32 尾槽并返回零。
std::uint32_t ResetGraphicCache_456D30()
{
    cg::render::gTextQueue.tails.fill(0u);
    return 0u;
}

} // namespace cg::render
