#include "platform/Input.hpp"

namespace cg::platform {

void InputState::BeginFrame() noexcept
{
    keyPressed_.fill(false);
    keyReleased_.fill(false);
    mousePressed_.fill(false);
    mouseReleased_.fill(false);
}

void InputState::HandleEvent(const Event& event) noexcept
{
    switch (event.type) {
    case EventType::KeyDown: {
        if (!IsValidScancode(event.scancode)) {
            return;
        }

        const auto index = static_cast<std::size_t>(event.scancode);
        if (!event.repeat && !keyDown_[index]) {
            keyPressed_[index] = true;
        }
        keyDown_[index] = true;
        return;
    }
    case EventType::KeyUp: {
        if (!IsValidScancode(event.scancode)) {
            return;
        }

        const auto index = static_cast<std::size_t>(event.scancode);
        keyDown_[index] = false;
        if (!event.repeat) {
            keyReleased_[index] = true;
        }
        return;
    }
    case EventType::MouseMoved:
        mousePosition_ = {event.mouseX, event.mouseY};
        return;
    case EventType::MouseButtonDown: {
        mousePosition_ = {event.mouseX, event.mouseY};
        if (!IsValidMouseButton(event.button)) {
            return;
        }

        const auto index = static_cast<std::size_t>(event.button);
        if (!mouseDown_[index]) {
            mousePressed_[index] = true;
        }
        mouseDown_[index] = true;
        return;
    }
    case EventType::MouseButtonUp: {
        mousePosition_ = {event.mouseX, event.mouseY};
        if (!IsValidMouseButton(event.button)) {
            return;
        }

        const auto index = static_cast<std::size_t>(event.button);
        mouseDown_[index] = false;
        mouseReleased_[index] = true;
        return;
    }
    case EventType::MouseWheel:
        mousePosition_ = {event.mouseX, event.mouseY};
        return;
    default:
        return;
    }
}

void InputState::EndFrame() noexcept
{
}

bool InputState::IsKeyDown(SDL_Scancode scancode) const noexcept
{
    if (!IsValidScancode(scancode)) {
        return false;
    }

    return keyDown_[static_cast<std::size_t>(scancode)];
}

bool InputState::WasKeyPressed(SDL_Scancode scancode) const noexcept
{
    if (!IsValidScancode(scancode)) {
        return false;
    }

    return keyPressed_[static_cast<std::size_t>(scancode)];
}

bool InputState::WasKeyReleased(SDL_Scancode scancode) const noexcept
{
    if (!IsValidScancode(scancode)) {
        return false;
    }

    return keyReleased_[static_cast<std::size_t>(scancode)];
}

bool InputState::IsMouseDown(Uint8 button) const noexcept
{
    if (!IsValidMouseButton(button)) {
        return false;
    }

    return mouseDown_[static_cast<std::size_t>(button)];
}

bool InputState::WasMousePressed(Uint8 button) const noexcept
{
    if (!IsValidMouseButton(button)) {
        return false;
    }

    return mousePressed_[static_cast<std::size_t>(button)];
}

bool InputState::WasMouseReleased(Uint8 button) const noexcept
{
    if (!IsValidMouseButton(button)) {
        return false;
    }

    return mouseReleased_[static_cast<std::size_t>(button)];
}

MousePosition InputState::GetMousePosition() const noexcept
{
    return mousePosition_;
}

bool InputState::IsValidScancode(SDL_Scancode scancode) noexcept
{
    return scancode > SDL_SCANCODE_UNKNOWN && scancode < SDL_SCANCODE_COUNT;
}

bool InputState::IsValidMouseButton(Uint8 button) noexcept
{
    return button > 0 && static_cast<std::size_t>(button) < MouseButtonCount;
}

} // namespace cg::platform
