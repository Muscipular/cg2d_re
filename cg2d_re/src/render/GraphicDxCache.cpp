#include "GraphicDxCacheState.hpp"

namespace cg::render {

DxGraphicEntry* gGraphicDxCache103CFA4 = nullptr;
std::array<TextureNode, kTextureNodeCapacity> gTextureNodes {};
std::int32_t gTextureNodeCount = 0;
std::int32_t gTextureScanIndex = 0;

void ResetTextureNodes_523310() noexcept
{
    gTextureScanIndex = 0;
    for (std::int32_t index = 0; index < gTextureNodeCount; ++index) {
        auto& node = gTextureNodes[static_cast<std::size_t>(index)];
        node.imageNo = -1;
        node.reuseState = 0;
        node.next = nullptr;
    }
}

} // namespace cg::render
