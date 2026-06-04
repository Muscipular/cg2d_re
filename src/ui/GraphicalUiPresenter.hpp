#pragma once

#include "ui/IUiPresenter.hpp"

namespace cg::ui {

// GraphicalUiPresenter 当前只复用 ViewModel/Intent 输入映射，渲染接入留给后续图形 UI。
class GraphicalUiPresenter final : public IUiPresenter {
public:
    void SetViewModel(UiViewModel viewModel) override;
    [[nodiscard]] const UiViewModel& ViewModel() const noexcept override;
    [[nodiscard]] UiIntent HandleInput(const UiInputEvent& event) override;

private:
    UiViewModel viewModel_;
};

} // namespace cg::ui
