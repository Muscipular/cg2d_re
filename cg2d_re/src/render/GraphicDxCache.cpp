#include "GraphicDxCacheState.hpp"

#include "cg/GraphicInfo.hpp"

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

std::int32_t InvalidateGraphicRefs_523350() noexcept
{
    const std::int32_t count = resource::gMaxGraphicCount;
    if (count <= 0) {
        return count;
    }

    std::int32_t processed = 0;
    std::uint32_t legacyOffset = 0;
    do {
        const auto index = static_cast<std::size_t>(processed);
        DxGraphicEntry* headBase = gGraphicDxCache103CFA4;
        ++processed;
        headBase[index].head = nullptr;

        DxGraphicEntry* paletteBase = gGraphicDxCache103CFA4;
        paletteBase[index].palette = nullptr;
        const std::int32_t currentCount = resource::gMaxGraphicCount;
        legacyOffset += 0x10;
        if (processed >= currentCount) {
            break;
        }
    } while (true);

    return static_cast<std::int32_t>(legacyOffset);
}

} // namespace cg::render
