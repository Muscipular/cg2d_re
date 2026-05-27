#pragma once

#include <SDL3/SDL_video.h>

namespace cg::platform {

class Window;

// ImeController 负责指定窗口的 SDL 文本输入启停。
class ImeController {
public:
    ImeController() = default;
    explicit ImeController(SDL_WindowID windowId) noexcept;
    explicit ImeController(const Window& window) noexcept;

    // BindWindow 绑定文本输入所属窗口，windowId 为 0 表示无效窗口。
    void BindWindow(SDL_WindowID windowId) noexcept;
    void BindWindow(const Window& window) noexcept;

    // GetWindowId 获取当前绑定的 SDL 窗口 ID，0 表示未绑定或无效。
    [[nodiscard]] SDL_WindowID GetWindowId() const noexcept;

    // BeginTextInput 开始接收当前窗口的文本输入事件。
    [[nodiscard]] bool BeginTextInput() noexcept;

    // EndTextInput 停止接收当前窗口的文本输入事件。
    [[nodiscard]] bool EndTextInput() noexcept;

    // IsTextInputActive 查询当前窗口是否已启用文本输入。
    [[nodiscard]] bool IsTextInputActive() const noexcept;

private:
    SDL_WindowID windowId_ = 0;
};

} // namespace cg::platform
