#include "cg/render/GraphicCacheState.hpp"

namespace cg::render {

std::array<std::uint32_t, 6> gGraphicCache4Queue {};

static_assert(sizeof(gGraphicCache4Queue) == 0x18);

} // namespace cg::render
