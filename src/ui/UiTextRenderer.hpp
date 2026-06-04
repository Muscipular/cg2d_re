#pragma once

#include "render/FontCache.hpp"
#include "ui/UiTextDraw.hpp"

#include <span>
#include <string>
#include <string_view>
#include <unordered_map>

namespace cg::render {
class RenderQueue;
}

namespace cg::ui {

// UiTextRenderer 将 UI 文本绘制输入通过 FontCache 转换为 RenderQueue sprite。
class UiTextRenderer final {
public:
    [[nodiscard]] bool PushTextDraws(
        cg::render::FontCache& fontCache,
        std::string_view fontKey,
        std::span<const UiTextDraw> draws,
        cg::render::RenderQueue& queue);

    void Clear() noexcept;

private:
    [[nodiscard]] cg::render::TextTexture FindOrRender(
        cg::render::FontCache& fontCache,
        std::string_view fontKey,
        const UiTextDraw& draw);

    std::unordered_map<std::string, cg::render::TextTexture> cache_;
};

} // namespace cg::ui
