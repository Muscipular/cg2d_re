#include "ui/GraphicalUiPresenter.hpp"

#include <utility>

namespace cg::ui {

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

} // namespace cg::ui
