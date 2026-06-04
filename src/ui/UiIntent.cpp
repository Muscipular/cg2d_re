#include "ui/UiIntent.hpp"

#include "ui/UiViewModel.hpp"

namespace cg::ui {
namespace {

bool HasEnabledSelection(const UiViewModel& viewModel) noexcept
{
    return viewModel.selectedIndex < viewModel.menuItems.size() && viewModel.menuItems[viewModel.selectedIndex].enabled;
}

void MoveSelection(UiViewModel& viewModel, int delta)
{
    if (viewModel.menuItems.empty()) {
        viewModel.selectedIndex = 0U;
        return;
    }

    const int count = static_cast<int>(viewModel.menuItems.size());
    int next = static_cast<int>(viewModel.selectedIndex) + delta;
    if (next < 0) {
        next = count - 1;
    } else if (next >= count) {
        next = 0;
    }

    viewModel.selectedIndex = static_cast<std::uint32_t>(next);
}

UiIntent SubmitByScreen(UiViewModel& viewModel)
{
    switch (viewModel.screen) {
    case UiScreenType::Login:
        return UiIntent{UiIntentType::SubmitLogin, viewModel.inputText, 0U};
    case UiScreenType::CharacterSelect:
        if (HasEnabledSelection(viewModel)) {
            return UiIntent{UiIntentType::SelectCharacter, viewModel.menuItems[viewModel.selectedIndex].text, viewModel.selectedIndex};
        }
        break;
    case UiScreenType::World:
        return UiIntent{UiIntentType::SubmitChat, viewModel.inputText, 0U};
    case UiScreenType::Battle:
        if (HasEnabledSelection(viewModel)) {
            return UiIntent{UiIntentType::SelectMenuItem, viewModel.menuItems[viewModel.selectedIndex].text, viewModel.selectedIndex};
        }
        break;
    }

    return {};
}

} // namespace

UiIntent BuildUiIntent(UiViewModel& viewModel, const UiInputEvent& event)
{
    if (event.type == UiInputEventType::TextInput) {
        viewModel.inputText += event.text;
        return UiIntent{UiIntentType::EditText, event.text, 0U};
    }

    switch (event.key) {
    case UiKey::Up:
        MoveSelection(viewModel, -1);
        return UiIntent{UiIntentType::Navigate, {}, viewModel.selectedIndex};
    case UiKey::Down:
        MoveSelection(viewModel, 1);
        return UiIntent{UiIntentType::Navigate, {}, viewModel.selectedIndex};
    case UiKey::Enter:
        return SubmitByScreen(viewModel);
    case UiKey::Escape:
        return UiIntent{UiIntentType::Cancel, {}, 0U};
    case UiKey::Backspace:
        if (!viewModel.inputText.empty()) {
            viewModel.inputText.pop_back();
        }
        return UiIntent{UiIntentType::EditText, viewModel.inputText, 0U};
    case UiKey::None:
    case UiKey::Left:
    case UiKey::Right:
        break;
    }

    return {};
}

} // namespace cg::ui
