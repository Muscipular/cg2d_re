#include "platform/Event.hpp"

namespace cg::platform {
namespace {

bool Translate(const SDL_Event& source, Event& target) noexcept
{
    target = {};

    switch (source.type) {
    case SDL_EVENT_QUIT:
        target.type = EventType::Quit;
        return true;
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        target.type = EventType::WindowCloseRequested;
        target.windowId = source.window.windowID;
        return true;
    case SDL_EVENT_WINDOW_RESIZED:
        target.type = EventType::WindowResized;
        target.windowId = source.window.windowID;
        target.width = source.window.data1;
        target.height = source.window.data2;
        return true;
    case SDL_EVENT_KEY_DOWN:
        target.type = EventType::KeyDown;
        target.windowId = source.key.windowID;
        target.key = source.key.key;
        target.scancode = source.key.scancode;
        target.repeat = source.key.repeat;
        return true;
    case SDL_EVENT_KEY_UP:
        target.type = EventType::KeyUp;
        target.windowId = source.key.windowID;
        target.key = source.key.key;
        target.scancode = source.key.scancode;
        target.repeat = source.key.repeat;
        return true;
    case SDL_EVENT_MOUSE_MOTION:
        target.type = EventType::MouseMoved;
        target.windowId = source.motion.windowID;
        target.mouseX = source.motion.x;
        target.mouseY = source.motion.y;
        return true;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        target.type = EventType::MouseButtonDown;
        target.windowId = source.button.windowID;
        target.mouseX = source.button.x;
        target.mouseY = source.button.y;
        target.button = source.button.button;
        return true;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        target.type = EventType::MouseButtonUp;
        target.windowId = source.button.windowID;
        target.mouseX = source.button.x;
        target.mouseY = source.button.y;
        target.button = source.button.button;
        return true;
    case SDL_EVENT_MOUSE_WHEEL:
        target.type = EventType::MouseWheel;
        target.windowId = source.wheel.windowID;
        target.mouseX = source.wheel.mouse_x;
        target.mouseY = source.wheel.mouse_y;
        target.wheelX = source.wheel.x;
        target.wheelY = source.wheel.y;
        return true;
    default:
        return false;
    }
}

} // namespace

bool EventPump::Poll(Event& out) noexcept
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        if (Translate(sdlEvent, out)) {
            return true;
        }
    }

    out = {};
    return false;
}

} // namespace cg::platform
