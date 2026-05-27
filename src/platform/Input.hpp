#pragma once

#include "platform/Event.hpp"

#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>

#include <array>
#include <cstddef>

namespace cg::platform {

// MousePosition 表示最近一次鼠标事件记录的窗口内坐标。
struct MousePosition {
    float x = 0.0F;
    float y = 0.0F;
};

// InputState 缓存当前帧输入状态，基于 EventPump 输出的 Event 更新。
class InputState {
public:
    // BeginFrame 清理单帧瞬时状态，应在处理本帧事件前调用。
    void BeginFrame() noexcept;

    // HandleEvent 根据平台事件更新键盘和鼠标状态；不会直接轮询 SDL 事件。
    void HandleEvent(const Event& event) noexcept;

    // EndFrame 预留帧结束阶段，目前无额外处理。
    void EndFrame() noexcept;

    // IsKeyDown 查询按键当前是否处于按下状态。
    [[nodiscard]] bool IsKeyDown(SDL_Scancode scancode) const noexcept;

    // WasKeyPressed 查询按键是否在当前帧首次按下。
    [[nodiscard]] bool WasKeyPressed(SDL_Scancode scancode) const noexcept;

    // WasKeyReleased 查询按键是否在当前帧释放。
    [[nodiscard]] bool WasKeyReleased(SDL_Scancode scancode) const noexcept;

    // IsMouseDown 查询鼠标按钮当前是否处于按下状态，button 使用 SDL_BUTTON_*。
    [[nodiscard]] bool IsMouseDown(Uint8 button) const noexcept;

    // WasMousePressed 查询鼠标按钮是否在当前帧首次按下，button 使用 SDL_BUTTON_*。
    [[nodiscard]] bool WasMousePressed(Uint8 button) const noexcept;

    // WasMouseReleased 查询鼠标按钮是否在当前帧释放，button 使用 SDL_BUTTON_*。
    [[nodiscard]] bool WasMouseReleased(Uint8 button) const noexcept;

    // GetMousePosition 获取最近一次鼠标移动、按钮或滚轮事件的位置。
    [[nodiscard]] MousePosition GetMousePosition() const noexcept;

private:
    static constexpr std::size_t MouseButtonCount = 8;

    [[nodiscard]] static bool IsValidScancode(SDL_Scancode scancode) noexcept;
    [[nodiscard]] static bool IsValidMouseButton(Uint8 button) noexcept;

    std::array<bool, SDL_SCANCODE_COUNT> keyDown_{};
    std::array<bool, SDL_SCANCODE_COUNT> keyPressed_{};
    std::array<bool, SDL_SCANCODE_COUNT> keyReleased_{};
    std::array<bool, MouseButtonCount> mouseDown_{};
    std::array<bool, MouseButtonCount> mousePressed_{};
    std::array<bool, MouseButtonCount> mouseReleased_{};
    MousePosition mousePosition_{};
};

} // namespace cg::platform
