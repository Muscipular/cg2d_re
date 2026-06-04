#pragma once

#include "ui/IUiPresenter.hpp"
#include "ui/TuiFrame.hpp"

namespace cg::ui {

// SdlTextUiPresenter 第一阶段只构建固定网格文本 frame，后续再接 FontCache。
class SdlTextUiPresenter final : public IUiPresenter {
public:
    void SetViewModel(UiViewModel viewModel) override;
    [[nodiscard]] const UiViewModel& ViewModel() const noexcept override;
    [[nodiscard]] UiIntent HandleInput(const UiInputEvent& event) override;

    [[nodiscard]] TuiFrame BuildFrame() const;

private:
    UiViewModel viewModel_;
};

} // namespace cg::ui
