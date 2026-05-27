#pragma once

#include <cstdint>

namespace cg::render {

struct RenderStats {
    std::uint64_t drawCalls = 0;
    std::uint64_t sprites = 0;
    std::uint64_t textureSwitches = 0;

    void Reset() noexcept
    {
        drawCalls = 0;
        sprites = 0;
        textureSwitches = 0;
    }
};

} // namespace cg::render
