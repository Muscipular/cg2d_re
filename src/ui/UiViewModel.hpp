#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cg::ui {

enum class UiScreenType : std::uint8_t {
    Login,
    CharacterSelect,
    World,
    Battle,
};

struct UiMenuItem {
    std::string text;
    bool enabled = true;
};

struct UiViewModel {
    UiScreenType screen = UiScreenType::Login;
    std::string title;
    std::string inputText;
    std::string statusText;
    std::vector<std::string> chatLines;
    std::vector<UiMenuItem> menuItems;
    std::uint32_t selectedIndex = 0U;
};

} // namespace cg::ui
