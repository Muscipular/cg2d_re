#pragma once

#include "ui/UiIntent.hpp"
#include "ui/UiViewModel.hpp"

namespace cg::ui {

// IUiPresenter 使用纯 UI 输入边界，避免把底层事件泄漏到 UI 层。
class IUiPresenter {
public:
    virtual ~IUiPresenter() = default;

    virtual void SetViewModel(UiViewModel viewModel) = 0;
    [[nodiscard]] virtual const UiViewModel& ViewModel() const noexcept = 0;
    [[nodiscard]] virtual UiIntent HandleInput(const UiInputEvent& event) = 0;
};

} // namespace cg::ui
