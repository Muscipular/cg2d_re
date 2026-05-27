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

    std::size_t batchStart = 0;
    while (batchStart < sprites_.size()) {
        const TextureHandle texture = sprites_[batchStart].texture;
        std::size_t batchEnd = batchStart + 1;
        while (batchEnd < sprites_.size() && sprites_[batchEnd].texture == texture) {
            ++batchEnd;
        }

        if (!renderer.DrawSprites(sprites_.data() + batchStart, batchEnd - batchStart)) {
            return false;
        }

        batchStart = batchEnd;
    }

    return true;
}

} // namespace cg::render
