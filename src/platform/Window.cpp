#include "platform/Window.hpp"

#include <SDL3/SDL_video.h>

#include <string>
#include <utility>

namespace cg::platform {

Window::~Window()
{
    Destroy();
}

Window::Window(Window&& other) noexcept
    : window_(std::exchange(other.window_, nullptr))
{
}

Window& Window::operator=(Window&& other) noexcept
{
    if (this != &other) {
        Destroy();
        window_ = std::exchange(other.window_, nullptr);
    }

    return *this;
}

bool Window::Create(std::string_view title, int width, int height, SDL_WindowFlags flags)
{
    Destroy();

    const std::string windowTitle(title);
    window_ = SDL_CreateWindow(windowTitle.c_str(), width, height, flags);
    return window_ != nullptr;
}

void Window::Destroy() noexcept
{
    if (window_ == nullptr) {
        return;
    }

    SDL_DestroyWindow(window_);
    window_ = nullptr;
}

bool Window::IsValid() const noexcept
{
    return window_ != nullptr;
}

WindowSize Window::GetSize() const noexcept
{
    WindowSize size;
    if (window_ != nullptr) {
        SDL_GetWindowSize(window_, &size.width, &size.height);
    }

    return size;
}

bool Window::SetSize(int width, int height) noexcept
{
    if (window_ == nullptr) {
        return false;
    }

    return SDL_SetWindowSize(window_, width, height);
}

WindowSize Window::GetPixelSize() const noexcept
{
    WindowSize size;
    if (window_ != nullptr) {
        SDL_GetWindowSizeInPixels(window_, &size.width, &size.height);
    }

    return size;
}

float Window::GetDpiScale() const noexcept
{
    if (window_ == nullptr) {
        return 0.0F;
    }

    return SDL_GetWindowDisplayScale(window_);
}

bool Window::SetFullscreen(bool fullscreen) noexcept
{
    if (window_ == nullptr) {
        return false;
    }

    return SDL_SetWindowFullscreen(window_, fullscreen);
}

bool Window::IsFullscreen() const noexcept
{
    if (window_ == nullptr) {
        return false;
    }

    return (SDL_GetWindowFlags(window_) & SDL_WINDOW_FULLSCREEN) != 0;
}

SDL_WindowID Window::GetId() const noexcept
{
    if (window_ == nullptr) {
        return 0;
    }

    return SDL_GetWindowID(window_);
}

} // namespace cg::platform
