#include "../WindowRuntimeInternal.hpp"

namespace cg::ui {

// 原函数：0x0050C7F0。
void WindowWorkResult_ReceiveReply(
    const std::uintptr_t, const std::int32_t result,
    const std::int32_t kind, const char* const text)
{
    if (world::gValue9A6E0C != 0 || world::ReadRippleGateOnce() != 0) {
        return;
    }
    switch (kind) {
        case 1:
            static_cast<void>(detail::WindowCraft_ReceiveRecipeResult(result, text));
            break;
        case 2:
            static_cast<void>(detail::WindowCraft_ReceiveItemWorkResult(result, text));
            break;
        case 3:
            static_cast<void>(detail::WindowSkillResult_ReceiveResult(result, text));
            break;
        case 4:
            static_cast<void>(detail::WindowGather_ReceiveResult(result, text));
            break;
        default:
            break;
    }
}

} // namespace cg::ui
