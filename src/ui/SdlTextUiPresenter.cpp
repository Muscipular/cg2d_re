#include "ui/SdlTextUiPresenter.hpp"

#include <algorithm>
#include <utility>

namespace cg::ui {
namespace {

void PushRun(TuiFrame& frame, std::uint32_t row, std::uint32_t column, std::string text)
{
    if (row >= frame.rows || column >= frame.columns) {
        return;
    }

    frame.runs.push_back(TuiTextRun{row, column, std::move(text)});
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

void SdlTextUiPresenter::SetViewModel(UiViewModel viewModel)
{
    viewModel_ = std::move(viewModel);
}

const UiViewModel& SdlTextUiPresenter::ViewModel() const noexcept
{
    return viewModel_;
}

UiIntent SdlTextUiPresenter::HandleInput(const UiInputEvent& event)
{
    return BuildUiIntent(viewModel_, event);
}

TuiFrame SdlTextUiPresenter::BuildFrame() const
{
    TuiFrame frame;
    PushRun(frame, 0U, 0U, viewModel_.title.empty() ? ScreenTitle(viewModel_.screen) : viewModel_.title);

    switch (viewModel_.screen) {
    case UiScreenType::Login:
        PushRun(frame, 2U, 0U, "Account:");
        PushRun(frame, 2U, 9U, viewModel_.inputText);
        PushRun(frame, 4U, 0U, viewModel_.statusText);
        break;
    case UiScreenType::CharacterSelect:
    case UiScreenType::Battle:
        for (std::size_t index = 0U; index < viewModel_.menuItems.size(); ++index) {
            std::string line = index == viewModel_.selectedIndex ? "> " : "  ";
            if (!viewModel_.menuItems[index].enabled) {
                line += "[x] ";
            }
            line += viewModel_.menuItems[index].text;
            PushRun(frame, static_cast<std::uint32_t>(2U + index), 0U, std::move(line));
        }
        break;
    case UiScreenType::World: {
        std::uint32_t row = 2U;
        const std::size_t maxLines = std::min<std::size_t>(viewModel_.chatLines.size(), 18U);
        const std::size_t first = viewModel_.chatLines.size() - maxLines;
        for (std::size_t index = first; index < viewModel_.chatLines.size(); ++index) {
            PushRun(frame, row, 0U, viewModel_.chatLines[index]);
            ++row;
        }
        PushRun(frame, 22U, 0U, "> ");
        PushRun(frame, 22U, 2U, viewModel_.inputText);
        break;
    }
    }

    return frame;
}

std::vector<UiTextDraw> SdlTextUiPresenter::BuildTextDraws(const SdlTextUiLayout& layout) const
{
    const TuiFrame frame = BuildFrame();
    std::vector<UiTextDraw> draws;
    draws.reserve(frame.runs.size());

    for (std::size_t index = 0U; index < frame.runs.size(); ++index) {
        const TuiTextRun& run = frame.runs[index];
        draws.push_back(UiTextDraw{
            run.text,
            cg::render::RectF{
                layout.originX + static_cast<float>(run.column) * layout.cellWidth,
                layout.originY + static_cast<float>(run.row) * layout.cellHeight,
                0.0F,
                0.0F,
            },
            layout.color,
            layout.layer,
            layout.orderBase + static_cast<std::uint32_t>(index),
        });
    }

    return draws;
}

} // namespace cg::ui
