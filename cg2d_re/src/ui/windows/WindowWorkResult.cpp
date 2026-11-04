#include "cg/network/CgConnection.hpp"
#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 窗口函数实现。

// 原函数：0x0050C7F0。
void WindowWorkResult_ReceiveReply(cg::network::CgConnection& connection, const std::int32_t result,
    const std::int32_t kind, const char* const text)
{
    if (connection.Owner().WorldScene().gameplayRepliesSuppressed != 0 || world::ReadRippleGateOnce() != 0) {
        return;
    }
    switch (kind) {
        case 1:
            static_cast<void>(detail::WindowCraft_ReceiveRecipeResult(connection.Owner(), result, text));
            break;
        case 2:
            static_cast<void>(detail::WindowCraft_ReceiveItemWorkResult(connection.Owner(), result, text));
            break;
        case 3:
            static_cast<void>(detail::WindowSkillResult_ReceiveResult(connection.Owner(), result, text));
            break;
        case 4:
            static_cast<void>(detail::WindowGather_ReceiveResult(connection.Owner(), result, text));
            break;
        default:
            break;
    }
}

} // namespace cg::ui
