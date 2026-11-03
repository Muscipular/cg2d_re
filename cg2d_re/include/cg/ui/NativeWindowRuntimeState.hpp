#pragma once

#include "cg/ui/WindowRuntime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::ui::detail {

union WindowUserState {
    std::uint32_t value; // 原+18的数值路径。
    WindowValueCallback callback; // 原窗口42的+18，禁止截成W32。
};

struct WindowState {
    std::uint32_t flags; // 原+00，独立于实例state。
    std::int32_t x; // 原+04。
    std::int32_t y; // 原+08。
    WindowInstance* window; // 原+0C；不沿用原记录0x54步长。
    std::uint32_t value10;
    std::uint32_t value14;
    WindowUserState userState; // 原+18；由生产者按实际用途激活成员。
    std::uint32_t actionFlags1C; // 窗口42数值面板消费低U8。
    std::uint32_t value20;
    std::uint32_t value24;
    std::uint32_t value28;
    std::array<std::byte, 36> bytes2C; // 未消费区，不据此冻结未知指针映射。
    std::uint32_t marker50;
};

inline constexpr std::size_t kSavedWindowCount = 71; // WinSaveDate格式固定，新增窗口不扩磁盘记录。
inline constexpr std::size_t kWindowCount = 73; // 原71槽加大地图、宠物装备原型。

} // namespace cg::ui::detail

namespace cg::ui {

// 原生窗口实例地址稳定，状态记录与排序数组由同一账号会话持有。
struct NativeWindowRuntimeState {
    std::array<WindowInstance*, detail::kWindowCount> windows {}; // 原0xC8B094，非拥有排序引用。
    std::array<detail::WindowState, detail::kWindowCount> states {}; // 原0xC9A9B0，按ID寻址。
};

} // namespace cg::ui
