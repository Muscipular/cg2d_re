#pragma once

#include "ui/IUiPresenter.hpp"
#include "ui/TuiFrame.hpp"
#include "ui/UiTextDraw.hpp"

#include <cstdint>
#include <vector>

namespace cg::ui {

struct SdlTextUiLayout {
    float originX = 24.0F;
    float originY = 24.0F;
    float cellWidth = 12.0F;
    float cellHeight = 24.0F;
    std::uint32_t layer = 100U;
    std::uint32_t orderBase = 0U;
    cg::render::ColorF color{0.92F, 0.96F, 0.90F, 1.0F};
};

// SdlTextUiPresenter 构建固定网格文本 frame，并转换为可提交渲染的文本绘制输入。
class SdlTextUiPresenter final : public IUiPresenter {
public:
    void SetViewModel(UiViewModel viewModel) override;
    [[nodiscard]] const UiViewModel& ViewModel() const noexcept override;
    [[nodiscard]] UiIntent HandleInput(const UiInputEvent& event) override;

    [[nodiscard]] TuiFrame BuildFrame() const;
    [[nodiscard]] std::vector<UiTextDraw> BuildTextDraws(const SdlTextUiLayout& layout = {}) const;

private:
    UiViewModel viewModel_;
};

} // namespace cg::ui
