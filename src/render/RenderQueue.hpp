#pragma once

#include <cstddef>
#include <vector>

#include "SpriteCmd.hpp"

namespace cg::render {

class IRenderer;

// RenderQueue 收集并提交 sprite 绘制命令。
class RenderQueue {
public:
    void Clear() noexcept;
    void PushSprite(const SpriteCmd& cmd);
    [[nodiscard]] std::size_t Size() const noexcept;
    void Sort();
    [[nodiscard]] bool Flush(IRenderer& renderer);

private:
    std::vector<SpriteCmd> sprites_;
};

} // namespace cg::render
