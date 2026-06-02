#include "world/SceneGroundSpriteBuilder.hpp"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

[[nodiscard]] bool RectEquals(const cg::render::RectF& actual, const cg::render::RectF& expected)
{
    return actual.x == expected.x
        && actual.y == expected.y
        && actual.width == expected.width
        && actual.height == expected.height;
}

int TestAppendSceneGroundSpriteCmds()
{
    int failures = 0;

    cg::world::SceneGroundAtlas ground{};
    const cg::render::TextureHandle texture{42U, 3U};
    ground.atlas.SetTexture(texture);
    ground.entries.push_back(cg::world::SceneGroundAtlasEntry{
        12U,
        19U,
        5000U,
        cg::world::TileRenderLayer::Special,
        cg::render::RectF{1.0F, 2.0F, 33.0F, 44.0F},
        -4,
        6,
        64,
        48,
    });
    ground.entries.push_back(cg::world::SceneGroundAtlasEntry{
        9U,
        22U,
        1000U,
        cg::world::TileRenderLayer::Normal,
        cg::render::RectF{5.0F, 6.0F, 17.0F, 18.0F},
        7,
        -9,
        32,
        16,
    });

    std::vector<cg::render::SpriteCmd> out;
    cg::render::SpriteCmd existing{};
    existing.texture = cg::render::TextureHandle{7U, 1U};
    existing.order = 123U;
    out.push_back(existing);

    const std::size_t emitted = cg::world::AppendSceneGroundSpriteCmds(
        ground,
        cg::world::TileCoord{10, 20},
        cg::world::ScreenPixelCoord{300, 400},
        7U,
        out);

    failures += Expect(emitted == 2U, "返回追加数量");
    failures += Expect(out.size() == 3U, "out 追加两条命令");
    failures += Expect(out[0].texture == existing.texture && out[0].order == existing.order, "保留已有 out 内容");

    const cg::render::SpriteCmd& special = out[1];
    failures += Expect(special.texture == texture, "special texture 来自 atlas");
    failures += Expect(RectEquals(special.source, cg::render::RectF{1.0F, 2.0F, 33.0F, 44.0F}), "special source");
    failures += Expect(RectEquals(special.destination, cg::render::RectF{328.0F, 334.0F, 64.0F, 48.0F}), "special destination");
    failures += Expect(special.layer == static_cast<std::uint32_t>(cg::world::TileRenderLayer::Special), "special layer");
    failures += Expect(special.sortY == 0, "special sortY");
    failures += Expect(special.order == static_cast<std::uint32_t>(UINT_MAX) - 7U, "special order");

    const cg::render::SpriteCmd& normal = out[2];
    failures += Expect(normal.texture == texture, "normal texture 来自 atlas");
    failures += Expect(RectEquals(normal.source, cg::render::RectF{5.0F, 6.0F, 17.0F, 18.0F}), "normal source");
    failures += Expect(RectEquals(normal.destination, cg::render::RectF{339.0F, 463.0F, 32.0F, 16.0F}), "normal destination");
    failures += Expect(normal.layer == static_cast<std::uint32_t>(cg::world::TileRenderLayer::Normal), "normal layer");
    failures += Expect(normal.sortY == 0, "normal sortY");
    failures += Expect(normal.order == static_cast<std::uint32_t>(UINT_MAX) - 8U, "normal order");

    return failures;
}

int TestAppendEmptySceneGroundSpriteCmds()
{
    int failures = 0;

    const cg::render::TextureHandle existingTexture{9U, 2U};
    std::vector<cg::render::SpriteCmd> out;
    cg::render::SpriteCmd existing{};
    existing.texture = existingTexture;
    existing.order = 77U;
    out.push_back(existing);

    const cg::world::SceneGroundAtlas ground{};
    const std::size_t emitted = cg::world::AppendSceneGroundSpriteCmds(
        ground,
        cg::world::TileCoord{3, 4},
        cg::world::ScreenPixelCoord{20, 30},
        5U,
        out);

    failures += Expect(emitted == 0U, "空 entries 返回 0");
    failures += Expect(out.size() == 1U, "空 entries 不追加命令");
    failures += Expect(out[0].texture == existingTexture && out[0].order == existing.order, "空 entries 保留已有 out 内容");

    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestAppendSceneGroundSpriteCmds();
    failures += TestAppendEmptySceneGroundSpriteCmds();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
