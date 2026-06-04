#include "ui/UiTextRenderer.hpp"

#include "render/RenderQueue.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace cg::ui {
namespace {

[[nodiscard]] std::uint8_t ToColorByte(float value) noexcept
{
    const float clamped = std::clamp(value, 0.0F, 1.0F);
    return static_cast<std::uint8_t>(std::lround(clamped * 255.0F));
}

[[nodiscard]] std::string MakeCacheKey(std::string_view fontKey, const UiTextDraw& draw)
{
    std::string key{fontKey};
    key.push_back('\x1F');
    key += draw.text;
    key.push_back('\x1F');
    key += std::to_string(ToColorByte(draw.color.r));
    key.push_back(',');
    key += std::to_string(ToColorByte(draw.color.g));
    key.push_back(',');
    key += std::to_string(ToColorByte(draw.color.b));
    key.push_back(',');
    key += std::to_string(ToColorByte(draw.color.a));
    return key;
}

[[nodiscard]] cg::render::RectF ResolveDestination(
    const UiTextDraw& draw,
    const cg::render::TextTexture& texture) noexcept
{
    cg::render::RectF destination = draw.destination;
    if (destination.width <= 0.0F) {
        destination.width = static_cast<float>(texture.width);
    }
    if (destination.height <= 0.0F) {
        destination.height = static_cast<float>(texture.height);
    }
    return destination;
}

} // namespace

bool UiTextRenderer::PushTextDraws(
    cg::render::FontCache& fontCache,
    std::string_view fontKey,
    std::span<const UiTextDraw> draws,
    cg::render::RenderQueue& queue)
{
    bool allSubmitted = true;
    for (const UiTextDraw& draw : draws) {
        if (draw.text.empty()) {
            continue;
        }

        const cg::render::TextTexture textTexture = FindOrRender(fontCache, fontKey, draw);
        if (!textTexture.texture.IsValid()) {
            allSubmitted = false;
            continue;
        }

        queue.PushSprite(cg::render::SpriteCmd{
            textTexture.texture,
            {0.0F, 0.0F, static_cast<float>(textTexture.width), static_cast<float>(textTexture.height)},
            ResolveDestination(draw, textTexture),
            {1.0F, 1.0F, 1.0F, 1.0F},
            draw.layer,
            static_cast<std::int32_t>(draw.destination.y),
            draw.order,
        });
    }

    return allSubmitted;
}

void UiTextRenderer::Clear() noexcept
{
    cache_.clear();
}

cg::render::TextTexture UiTextRenderer::FindOrRender(
    cg::render::FontCache& fontCache,
    std::string_view fontKey,
    const UiTextDraw& draw)
{
    const std::string key = MakeCacheKey(fontKey, draw);
    const auto found = cache_.find(key);
    if (found != cache_.end()) {
        return found->second;
    }

    const cg::render::TextTexture textTexture = fontCache.RenderText(fontKey, draw.text, draw.color);
    if (textTexture.texture.IsValid()) {
        cache_.emplace(key, textTexture);
    }
    return textTexture;
}

} // namespace cg::ui
