#pragma once

#include "LoginStateDependencies.hpp"

#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <array>
#include <cstdint>

namespace cg::login::detail {

// 登录/消息共用音效边界：第三个原压栈值不被52C450读取，返回完整S32结果。
std::int32_t PlaySe(
    std::int32_t seNo,
    std::int32_t screenX,
    std::int32_t unused);

// 原 mouseX@0xD68CF8 与 mouseY@0xD68D08 的单次 CryptInt 读取边界。
std::int32_t ReadLegacyMouseX();
std::int32_t ReadLegacyMouseY();

// 原共享系统存储；本模块只按注释所列宽度读取，不建立第二份定义。
extern std::uint32_t& gValueD68D9C; // 原 0xD68D9C，本函数读取低 8 位。
extern std::uint32_t& gValueD68D98; // 原 0xD68D98，sub_503990 读取低 8 位。
extern std::uint32_t& gValueD68DA0; // 原 0xD68DA0，sub_503990 读取低 8 位。

} // namespace cg::login::detail
