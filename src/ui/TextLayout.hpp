#pragma once

#include "render/FontCache.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace cg::ui {

struct TextLine {
    std::string text;
    int width = 0;
    int height = 0;
};

struct TextLayoutResult {
    std::vector<TextLine> lines;
    int width = 0;
    int height = 0;
};

[[nodiscard]] TextLayoutResult LayoutText(
    const render::FontCache& fontCache,
    std::string_view fontKey,
    std::string_view text,
    int maxWidth);

} // namespace cg::ui
