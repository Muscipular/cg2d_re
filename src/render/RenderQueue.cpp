#include "RenderQueue.hpp"

#include <algorithm>

#include "IRenderer.hpp"

namespace cg::render {

void RenderQueue::Clear() noexcept
{
    sprites_.clear();
}

void RenderQueue::PushSprite(const SpriteCmd& cmd)
{
    sprites_.push_back(cmd);
}

std::size_t RenderQueue::Size() const noexcept
{
    return sprites_.size();
}

void RenderQueue::Sort()
{
    std::stable_sort(sprites_.begin(), sprites_.end(), [](const SpriteCmd& lhs, const SpriteCmd& rhs) noexcept {
        if (lhs.layer != rhs.layer) {
            return lhs.layer < rhs.layer;
        }
        if (lhs.sortY != rhs.sortY) {
            return lhs.sortY < rhs.sortY;
        }
        return lhs.order < rhs.order;
    });
}

bool RenderQueue::Flush(IRenderer& renderer)
{
    Sort();

    for (const SpriteCmd& sprite : sprites_) {
        if (!renderer.DrawTexture(sprite.texture, sprite.source, sprite.destination)) {
            return false;
        }
    }

    return true;
}

} // namespace cg::render
