#include "world/SceneGroundSpriteBuilder.hpp"

#include "world/IdaRenderRules.hpp"

namespace cg::world {

std::size_t AppendSceneGroundSpriteCmds(
    const SceneGroundAtlas& ground,
    TileCoord originTile,
    ScreenPixelCoord originScreen,
    std::uint32_t firstEnqueueIndex,
    std::vector<cg::render::SpriteCmd>& out)
{
    std::uint32_t emittedIndex = 0U;
    const cg::render::TextureHandle texture = ground.atlas.GetTexture();

    for (const SceneGroundAtlasEntry& entry : ground.entries) {
        const ScreenPixelCoord screen = ProjectTileDeltaToScreen(
            TileCoord{
                static_cast<std::int32_t>(entry.mapX) - originTile.x,
                static_cast<std::int32_t>(entry.mapY) - originTile.y,
            },
            originScreen);

        cg::render::SpriteCmd cmd{};
        cmd.texture = texture;
        cmd.source = entry.source;
        cmd.destination = cg::render::RectF{
            static_cast<float>(screen.x + entry.offsetX),
            static_cast<float>(screen.y + entry.offsetY),
            static_cast<float>(entry.width),
            static_cast<float>(entry.height),
        };
        cmd.layer = static_cast<std::uint32_t>(entry.layer);
        cmd.sortY = 0;
        cmd.order = MakeIdaCompatibleOrder(firstEnqueueIndex + emittedIndex);

        out.push_back(cmd);
        ++emittedIndex;
    }

    return emittedIndex;
}

} // namespace cg::world
