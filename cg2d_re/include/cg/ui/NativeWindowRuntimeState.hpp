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

// 原D40B30的521CH磁盘格式，不与含原生指针的WindowState布局混用。
struct WindowSavedRecord {
    std::int32_t x;
    std::int32_t y;
    std::int32_t value08;
    std::int32_t value0C;
    std::int32_t value10;
    std::array<std::byte, 0x114> bytes14;
};

struct WindowSavedData {
    std::int32_t version;
    std::array<WindowSavedRecord, kSavedWindowCount> records;
};

} // namespace cg::ui::detail

namespace cg::ui {

// UI布局及选择偏好；装载初值与加载文件后的默认值分开保留。
struct WindowLayoutSettings {
    std::int32_t chatMode = 0; // 原0xD66E04。
    std::int32_t chatWidthUnits = 0; // 原0xC9A930。
    std::int32_t chatHeightUnits = 0; // 原0xC9A938。
    std::uint8_t inventoryActorSelection = 0xFF; // 原0x57DA28。
    std::uint8_t battleItemActorSelection = 0xFF; // 原0x57E42C。
    std::int32_t hotkeyLayoutMode = 0; // 原0xCF4AAC。
    std::int32_t statusBarMode = 0; // 原0xCF4690。
    std::int32_t shortcutBarCollapsed = 0; // 原0xCA0440。
    std::int32_t contactsPreviewMode = 0; // 原0xD67B38。
    std::uint32_t actorBarsEnabled = 0; // 原0x5AC490，地图角色绘制与快捷键共用。
};

// 原生窗口实例地址稳定，状态记录与排序数组由同一账号会话持有。
struct NativeWindowRuntimeState {
    std::array<WindowInstance*, detail::kWindowCount> windows {}; // 原0xC8B094，非拥有排序引用。
    std::array<detail::WindowState, detail::kWindowCount> states {}; // 原0xC9A9B0，按ID寻址。
    WindowLayoutSettings settings {};
    detail::WindowSavedData savedData {}; // 原0xD40B30。
    std::int32_t savedLength = 0; // 原0xCC4238。
};

} // namespace cg::ui
