#pragma once

#include "LoginStateDependencies.hpp"

#include "cg/render/Font.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <array>
#include <cstdint>

namespace cg::login::detail {

// 登录/消息共用音效边界：第三个原压栈值不被52C450读取，返回完整S32结果。
std::int32_t PlaySe(
    std::int32_t seNo,
    std::int32_t screenX,
    std::int32_t unused);

} // namespace cg::login::detail
