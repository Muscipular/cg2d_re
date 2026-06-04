#pragma once

#include <cstdint>
#include <string>

namespace cg::ui {

enum class UiKey : std::uint8_t {
    None,
    Up,
    Down,
    Left,
    Right,
    Enter,
    Escape,
    Backspace,
};

enum class UiInputEventType : std::uint8_t {
    Key,
    TextInput,
};

struct UiInputEvent {
    UiInputEventType type = UiInputEventType::Key;
    UiKey key = UiKey::None;
    std::string text;
};

enum class UiIntentType : std::uint8_t {
    None,
    EditText,
    SubmitLogin,
    SelectCharacter,
    SubmitChat,
    SelectMenuItem,
    Cancel,
    Navigate,
};

struct UiIntent {
    UiIntentType type = UiIntentType::None;
    std::string text;
    std::uint32_t index = 0U;
};

struct UiViewModel;

[[nodiscard]] UiIntent BuildUiIntent(UiViewModel& viewModel, const UiInputEvent& event);

} // namespace cg::ui
