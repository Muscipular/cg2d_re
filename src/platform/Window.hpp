#pragma once

#include <SDL3/SDL_video.h>

#include <string_view>

namespace cg::platform {

// WindowSize 表示窗口逻辑尺寸或像素尺寸。
struct WindowSize {
    int width = 0;
    int height = 0;
};

// Window 负责持有并释放 SDL_Window。
class Window {
public:
    Window() = default;
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    // Create 创建 SDL 窗口；失败时可通过 SDL_GetError() 获取原因。
    [[nodiscard]] bool Create(std::string_view title, int width, int height, SDL_WindowFlags flags = 0);

    // Destroy 释放已创建的 SDL 窗口。
    void Destroy() noexcept;

    // IsValid 表示当前是否持有有效 SDL 窗口。
    [[nodiscard]] bool IsValid() const noexcept;

    // GetSize 获取窗口客户区逻辑尺寸。
    [[nodiscard]] WindowSize GetSize() const noexcept;

    // SetSize 设置窗口客户区逻辑尺寸。
    [[nodiscard]] bool SetSize(int width, int height) noexcept;

    // GetPixelSize 获取窗口客户区像素尺寸。
    [[nodiscard]] WindowSize GetPixelSize() const noexcept;

    // GetDpiScale 获取窗口所在显示器的 DPI 缩放。
    [[nodiscard]] float GetDpiScale() const noexcept;

    // SetFullscreen 切换窗口全屏状态。
    [[nodiscard]] bool SetFullscreen(bool fullscreen) noexcept;

    // IsFullscreen 查询窗口是否处于全屏状态。
    [[nodiscard]] bool IsFullscreen() const noexcept;

    // GetId 获取 SDL 窗口 ID，0 表示无效。
    [[nodiscard]] SDL_WindowID GetId() const noexcept;

private:
    SDL_Window* window_ = nullptr;
};

} // namespace cg::platform
