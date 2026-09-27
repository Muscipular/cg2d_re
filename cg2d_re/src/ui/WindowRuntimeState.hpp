#pragma once

#include "cg/ui/WindowRuntime.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::ui {

using WindowCallback = std::int32_t (*)(std::int32_t value);
using WindowReleaseCallback = void (*)();
using WindowElementCallback = std::int32_t (*)(std::int32_t index, std::int32_t state);
using WindowValueCallback = void (*)(std::int32_t value);

// 仅按已证字段增量映射；未消费数据不计最终布局冻结，见UI窗口SPEC。
struct WindowElement {
    std::int32_t type;
    std::uint32_t state;
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    std::int32_t visible;
    void* data; // 原+1C，七类数据共用；后续按type解释，不是地址数值。
    WindowElementCallback mouseCallback; // 原+20。
};

struct WindowInstance {
    std::uint32_t state; // 原+00；关闭请求置bit15。
    std::int32_t id;
    std::int32_t layer;
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
    std::uint32_t value1C;
    std::uint32_t value20;
    std::uint32_t flags;
    std::int32_t elementCount;
    WindowElement* elements; // 原+2C。
    WindowCallback update; // 原+30。
    WindowCallback draw; // 原+34。
    WindowReleaseCallback release; // 原+38。
    std::int32_t secondaryCount; // 原+3C。
    std::array<std::uint32_t, 40> secondaryWords40; // 原[+40,+E0)，尚未消费。
};

namespace detail {

union WindowUserState {
    std::uint32_t value; // 原+18的数值路径。
    WindowValueCallback callback; // 原窗口42的+18，禁止截成W32。
};

struct WindowState {
    std::uint32_t flags; // 原+00，独立于实例state。
    std::int32_t x;
    std::int32_t y;
    WindowInstance* window; // 原+0C；不沿用原记录0x54步长。
    std::uint32_t value10;
    std::uint32_t value14;
    WindowUserState userState; // 原+18；后续生产者按实际用途激活成员。
    std::array<std::byte, 4> bytes1C;
    std::uint32_t value20;
    std::uint32_t value24;
    std::uint32_t value28;
    std::array<std::byte, 36> bytes2C; // 未消费区，不据此冻结未知指针映射。
    std::uint32_t marker50;
};

inline constexpr std::size_t kWindowCount = 71;
extern std::array<WindowInstance*, kWindowCount> gWindows; // 原C8B094。
extern std::array<WindowState, kWindowCount> gWindowStates; // 原C9A9B0。
extern WindowInstance* gCurrentWindow; // 原CF4694。
extern WindowState* gCurrentWindowState; // 原CF4684。

static_assert(offsetof(WindowElement, data) == 0x20);
static_assert(offsetof(WindowElement, mouseCallback) == 0x28);
static_assert(sizeof(WindowElement) == 0x30);
static_assert(offsetof(WindowInstance, state) == 0);
static_assert(offsetof(WindowInstance, elements) == 0x30);
static_assert(offsetof(WindowInstance, update) == 0x38);
static_assert(offsetof(WindowInstance, draw) == 0x40);
static_assert(offsetof(WindowInstance, release) == 0x48);
static_assert(offsetof(WindowInstance, secondaryCount) == 0x50);
static_assert(offsetof(WindowInstance, secondaryWords40) == 0x54);
static_assert(sizeof(WindowInstance) == 0xF8);
static_assert(offsetof(WindowState, window) == 0x10);
static_assert(offsetof(WindowState, userState) == 0x20);
static_assert(offsetof(WindowState, marker50) == 0x5C);
static_assert(sizeof(WindowState) == 0x60);

} // namespace detail
} // namespace cg::ui
