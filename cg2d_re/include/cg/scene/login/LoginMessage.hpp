#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace cg::yobi { struct YobiAction; }

namespace cg::login {

// 原连续存储 [0x9A620C,0x9A6E0C)，第 i 行对应 0x9A620C + i * 0x100。
// UTL02：LoginScene与消息更新函数共享12行动态UTF-16正文，原行地址保留为历史映射。
struct LoginMessageState {
    std::array<std::wstring, 12> lines {};
    std::uint16_t phase = 0; // 原 0x9A6E2C。
    std::int32_t result = -2; // 原 0x55A04C。
    std::int32_t width = 0; // 原 0x9A50A8。
    std::int32_t height = 0; // 原 0x9A5658。
    std::int32_t x = 0; // 原 0x9A5660。
    std::int32_t y = 0; // 原 0x9A5ABC。
    yobi::YobiAction* action = nullptr; // 原 0x9A6E30，借用模态动作。
};

// 原 sub_45CE00@0x45CE00；只清消息内部的 16 位状态。
void ResetLoginMessage();

// 原 sub_45CE10@0x45CE10。
// UWF01：消息入口只接收UTF-16正文。
std::int32_t UpdateLoginMessage(std::wstring_view text);

} // namespace cg::login
