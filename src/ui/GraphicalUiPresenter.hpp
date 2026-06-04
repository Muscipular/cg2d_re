#pragma once

#include "ui/IUiPresenter.hpp"
#include "ui/UiTextDraw.hpp"

#include <cstdint>
#include <vector>

namespace cg::ui {

struct GraphicalUiLayout {
    float originX = 720.0F;
    float originY = 32.0F;
    float lineHeight = 28.0F;
    std::uint32_t layer = 110U;
    std::uint32_t orderBase = 0U;
    cg::render::ColorF titleColor{1.0F, 0.92F, 0.68F, 1.0F};
    cg::render::ColorF textColor{0.86F, 0.90F, 1.0F, 1.0F};
};

// GraphicalUiPresenter 复用 ViewModel/Intent，并输出图形 UI 文本绘制输入。
class GraphicalUiPresenter final : public IUiPresenter {
public:
    void SetViewModel(UiViewModel viewModel) override;
    [[nodiscard]] const UiViewModel& ViewModel() const noexcept override;
    [[nodiscard]] UiIntent HandleInput(const UiInputEvent& event) override;
    [[nodiscard]] std::vector<UiTextDraw> BuildTextDraws(const GraphicalUiLayout& layout = {}) const;

private:
    UiViewModel viewModel_;
};

} // namespace cg::ui
