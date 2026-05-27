#pragma once

#include <SDL3/SDL_events.h>

namespace cg::platform {

// EventType 表示当前平台层支持分发的事件类型。
enum class EventType {
    None,
    Quit,
    WindowCloseRequested,
    WindowResized,
    KeyDown,
    KeyUp,
    MouseMoved,
    MouseButtonDown,
    MouseButtonUp,
    MouseWheel,
};

// Event 是 SDL_Event 转换后的最小事件载体。
struct Event {
    EventType type = EventType::None;
    SDL_WindowID windowId = 0;
    SDL_Keycode key = SDLK_UNKNOWN;
    SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;
    bool repeat = false;
    float mouseX = 0.0F;
    float mouseY = 0.0F;
    Uint8 button = 0;
    float wheelX = 0.0F;
    float wheelY = 0.0F;
    int width = 0;
    int height = 0;
};

// EventPump 负责从 SDL 事件队列轮询并转换为 Event。
class EventPump {
public:
    // Poll 轮询下一个已支持事件；未支持的 SDL 事件会被跳过。
    [[nodiscard]] bool Poll(Event& out) noexcept;
};

} // namespace cg::platform
