#include "cg/render/GraphicCacheState.hpp"
#include "cg/session/ClientSession.hpp"

namespace cg::render {

// 原函数：0x00456D50。
std::int32_t ClearTextQueueTail_456D50(session::ClientSession& session, const std::int8_t mode)
{
    session.TextQueue().tails[mode] = 0;
    return mode;
}

// 原 [0x456D30,0x456D41)：仅清六个 W32 尾槽并返回零。
std::uint32_t ResetGraphicCache_456D30(session::ClientSession& session)
{
    session.TextQueue().tails.fill(0u);
    return 0u;
}

} // namespace cg::render
