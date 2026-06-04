#include "ui/GraphicalUiPresenter.hpp"

#include <algorithm>
#include <utility>

namespace cg::ui {
namespace {

void PushDraw(
    std::vector<UiTextDraw>& draws,
    const GraphicalUiLayout& layout,
    std::string text,
    float row,
    cg::render::ColorF color)
{
    if (text.empty()) {
        return;
    }

    const std::uint32_t order = layout.orderBase + static_cast<std::uint32_t>(draws.size());
    draws.push_back(UiTextDraw{
        std::move(text),
        cg::render::RectF{layout.originX, layout.originY + row * layout.lineHeight, 0.0F, 0.0F},
        color,
        layout.layer,
        order,
    });
}

const char* ScreenTitle(UiScreenType screen) noexcept
{
    switch (screen) {
    case UiScreenType::Login:
        return "Login";
    case UiScreenType::CharacterSelect:
        return "CharacterSelect";
    case UiScreenType::World:
        return "World";
    case UiScreenType::Battle:
        return "Battle";
    }

    return "";
}

} // namespace

void GraphicalUiPresenter::SetViewModel(UiViewModel viewModel)
{
    viewModel_ = std::move(viewModel);
}

const UiViewModel& GraphicalUiPresenter::ViewModel() const noexcept
{
    return viewModel_;
}

UiIntent GraphicalUiPresenter::HandleInput(const UiInputEvent& event)
{
    return BuildUiIntent(viewModel_, event);
}

std::vector<UiTextDraw> GraphicalUiPresenter::BuildTextDraws(const GraphicalUiLayout& layout) const
{
    std::vector<UiTextDraw> draws;
    draws.reserve(8U + viewModel_.menuItems.size() + viewModel_.chatLines.size());

    PushDraw(
        draws,
        layout,
        viewModel_.title.empty() ? ScreenTitle(viewModel_.screen) : viewModel_.title,
        0.0F,
        layout.titleColor);

    switch (viewModel_.screen) {
    case UiScreenType::Login:
        PushDraw(draws, layout, "Account: " + viewModel_.inputText, 2.0F, layout.textColor);
        PushDraw(draws, layout, viewModel_.statusText, 3.0F, layout.textColor);
        break;
    case UiScreenType::CharacterSelect:
    case UiScreenType::Battle:
        for (std::size_t index = 0U; index < viewModel_.menuItems.size(); ++index) {
            std::string line = index == viewModel_.selectedIndex ? "> " : "  ";
            if (!viewModel_.menuItems[index].enabled) {
                line += "[x] ";
            }
            line += viewModel_.menuItems[index].text;
            PushDraw(draws, layout, std::move(line), 2.0F + static_cast<float>(index), layout.textColor);
        }
        break;
    case UiScreenType::World: {
        const std::size_t maxLines = std::min<std::size_t>(viewModel_.chatLines.size(), 8U);
        const std::size_t first = viewModel_.chatLines.size() - maxLines;
        for (std::size_t index = first; index < viewModel_.chatLines.size(); ++index) {
            PushDraw(draws, layout, viewModel_.chatLines[index], 2.0F + static_cast<float>(index - first), layout.textColor);
        }
        PushDraw(draws, layout, "> " + viewModel_.inputText, 11.0F, layout.textColor);
        break;
    }
    }

    return draws;
}

} // namespace cg::ui
