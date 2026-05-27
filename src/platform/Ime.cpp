#include "platform/Ime.hpp"

#include "platform/Window.hpp"

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

namespace cg::platform {
namespace {

[[nodiscard]] SDL_Window* ResolveWindow(SDL_WindowID windowId) noexcept
{
    if (windowId == 0) {
        return nullptr;
    }

    return SDL_GetWindowFromID(windowId);
}

} // namespace

ImeController::ImeController(SDL_WindowID windowId) noexcept
    : windowId_(windowId)
{
}

ImeController::ImeController(const Window& window) noexcept
    : windowId_(window.GetId())
{
}

void ImeController::BindWindow(SDL_WindowID windowId) noexcept
{
    windowId_ = windowId;
}

void ImeController::BindWindow(const Window& window) noexcept
{
    windowId_ = window.GetId();
}

SDL_WindowID ImeController::GetWindowId() const noexcept
{
    return windowId_;
}

bool ImeController::BeginTextInput() noexcept
{
    SDL_Window* window = ResolveWindow(windowId_);
    if (window == nullptr) {
        return false;
    }

    return SDL_StartTextInput(window);
}

bool ImeController::EndTextInput() noexcept
{
    SDL_Window* window = ResolveWindow(windowId_);
    if (window == nullptr) {
        return false;
    }

    return SDL_StopTextInput(window);
}

bool ImeController::IsTextInputActive() const noexcept
{
    SDL_Window* window = ResolveWindow(windowId_);
    if (window == nullptr) {
        return false;
    }

    return SDL_TextInputActive(window);
}

bool ImeController::SetInputArea(float x, float y, float width, float height, float cursorX) noexcept
{
    SDL_Window* window = ResolveWindow(windowId_);
    if (window == nullptr) {
        return false;
    }

    const SDL_Rect rect = {
        static_cast<int>(SDL_roundf(x)),
        static_cast<int>(SDL_roundf(y)),
        static_cast<int>(SDL_roundf(width)),
        static_cast<int>(SDL_roundf(height)),
    };
    const int cursorOffset = static_cast<int>(SDL_roundf(cursorX - x));
    return SDL_SetTextInputArea(window, &rect, cursorOffset);
}

} // namespace cg::platform
