#pragma once

#include "render/Color.hpp"
#include "render/RenderTypes.hpp"

#include <cstdint>
#include <string>

namespace cg::ui {

// UiTextDraw 是 UI 层提交给字体渲染边界的纯文本绘制输入。
struct UiTextDraw {
    std::string text;
    cg::render::RectF destination{};
    cg::render::ColorF color{1.0F, 1.0F, 1.0F, 1.0F};
    std::uint32_t layer = 0U;
    std::uint32_t order = 0U;
};

} // namespace cg::ui
