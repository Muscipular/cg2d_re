#include "world/SceneGroundSpriteBuilder.hpp"

#include "world/IdaRenderRules.hpp"
#include "world/VisibilityCulling.hpp"

namespace cg::world {

namespace {

std::size_t AppendSceneGroundSpriteCmdsImpl(
    const SceneGroundAtlas& ground,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t firstEnqueueIndex,
    const cg::render::RectF* cameraRect,
    std::vector<cg::render::SpriteCmd>& out)
{
    std::uint32_t entryIndex = 0U;
    std::uint32_t emittedCount = 0U;
    const cg::render::TextureHandle texture = ground.atlas.GetTexture();

    for (const SceneGroundAtlasEntry& entry : ground.entries) {
        const std::uint32_t currentEntryIndex = entryIndex;
        ++entryIndex;

        const cg::render::RectF destination = MakeTileSpriteRect(entry, originTile, originScreen);
        if (cameraRect != nullptr && !IsSpriteVisible(*cameraRect, destination)) {
            continue;
        }

        cg::render::SpriteCmd cmd{};
        cmd.texture = texture;
        cmd.source = entry.source;
        cmd.destination = destination;
        cmd.layer = static_cast<std::uint32_t>(entry.layer);
        cmd.sortY = 0;
        cmd.order = MakeIdaCompatibleOrder(firstEnqueueIndex + currentEntryIndex);

        out.push_back(cmd);
        ++emittedCount;
    }

    return emittedCount;
}

} // namespace

std::size_t AppendSceneGroundSpriteCmds(
    const SceneGroundAtlas& ground,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t firstEnqueueIndex,
    std::vector<cg::render::SpriteCmd>& out)
{
    return AppendSceneGroundSpriteCmdsImpl(ground, originTile, originScreen, firstEnqueueIndex, nullptr, out);
}

std::size_t AppendSceneGroundSpriteCmds(
    const SceneGroundAtlas& ground,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t firstEnqueueIndex,
    const cg::render::RectF& cameraRect,
    std::vector<cg::render::SpriteCmd>& out)
{
    return AppendSceneGroundSpriteCmdsImpl(ground, originTile, originScreen, firstEnqueueIndex, &cameraRect, out);
}

} // namespace cg::world
