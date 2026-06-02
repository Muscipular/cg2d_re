#include "world/VisibilityCulling.hpp"

#include "render/SpriteCmd.hpp"
#include "world/IdaRenderRules.hpp"
#include "world/EntityData.hpp"
#include "world/MapObjectSpriteBuilder.hpp"
#include "world/MapObjectTextureCache.hpp"
#include "world/SceneGroundSpriteBuilder.hpp"

#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

namespace {

template <typename T>
concept HasObjectIndex = requires(T value) {
    value.objectIndex;
};

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

[[nodiscard]] cg::world::SceneGroundAtlasEntry MakeGroundEntry(
    std::uint16_t mapX,
    std::uint16_t mapY,
    float sourceX,
    int offsetX,
    int offsetY,
    int width,
    int height)
{
    return cg::world::SceneGroundAtlasEntry{
        mapX,
        mapY,
        1000U,
        cg::world::TileRenderLayer::Normal,
        cg::render::RectF{sourceX, 0.0F, static_cast<float>(width), static_cast<float>(height)},
        offsetX,
        offsetY,
        width,
        height,
    };
}

[[nodiscard]] cg::world::MapObject MakeObject()
{
    return cg::world::MapObject{
        55,
        cg::world::TileCoord{12, 19},
        100U,
    };
}

[[nodiscard]] cg::world::MapObjectTexture MakeTexture()
{
    return cg::world::MapObjectTexture{
        100U,
        cg::render::TextureHandle{42U, 3U},
        64,
        48,
        -4,
        6,
    };
}

int TestSpriteVisible()
{
    int failures = 0;

    const cg::render::RectF camera{0.0F, 0.0F, 100.0F, 100.0F};
    failures += Expect(cg::world::IsSpriteVisible(camera, cg::render::RectF{10.0F, 10.0F, 20.0F, 20.0F}), "完全可见");
    failures += Expect(cg::world::IsSpriteVisible(camera, cg::render::RectF{-10.0F, 10.0F, 20.0F, 20.0F}), "部分相交可见");
    failures += Expect(!cg::world::IsSpriteVisible(camera, cg::render::RectF{101.0F, 10.0F, 10.0F, 10.0F}), "完全屏外不可见");
    failures += Expect(!cg::world::IsSpriteVisible(camera, cg::render::RectF{100.0F, 10.0F, 10.0F, 10.0F}), "右边缘刚好接触不可见");
    failures += Expect(!cg::world::IsSpriteVisible(camera, cg::render::RectF{10.0F, 100.0F, 10.0F, 10.0F}), "下边缘刚好接触不可见");
    failures += Expect(
        cg::world::IsSpriteVisible(
            cg::render::RectF{-50.0F, -50.0F, 40.0F, 40.0F},
            cg::render::RectF{-20.0F, -20.0F, 5.0F, 5.0F}),
        "负坐标可见");
    failures += Expect(!cg::world::IsSpriteVisible(camera, cg::render::RectF{0.0F, 0.0F, 0.0F, 10.0F}), "sprite width <= 0 不可见");
    failures += Expect(!cg::world::IsSpriteVisible(camera, cg::render::RectF{0.0F, 0.0F, 10.0F, -1.0F}), "sprite height <= 0 不可见");
    failures += Expect(
        !cg::world::IsSpriteVisible(
            cg::render::RectF{0.0F, 0.0F, 0.0F, 100.0F},
            cg::render::RectF{0.0F, 0.0F, 10.0F, 10.0F}),
        "camera width <= 0 不可见");

    return failures;
}

int TestTileRectAndBuilderCamera()
{
    int failures = 0;

    const cg::world::SceneGroundAtlasEntry visible = MakeGroundEntry(12U, 19U, 1.0F, -4, 6, 64, 48);
    const cg::render::RectF visibleRect = cg::world::MakeTileSpriteRect(
        visible,
        cg::world::TileCoord{10, 20},
        cg::world::ScreenPixelCoord{300, 400});
    failures += Expect(RectEquals(visibleRect, cg::render::RectF{328.0F, 334.0F, 64.0F, 48.0F}), "tile rect 包含 offset 和投影");
    failures += Expect(
        cg::world::IsTileSpriteVisible(
            cg::render::RectF{320.0F, 330.0F, 20.0F, 20.0F},
            visible,
            cg::world::TileCoord{10, 20},
            cg::world::ScreenPixelCoord{300, 400}),
        "tile rect 与 camera 相交可见");

    cg::world::SceneGroundAtlas ground{};
    const cg::render::TextureHandle texture{77U, 2U};
    ground.atlas.SetTexture(texture);
    ground.entries.push_back(MakeGroundEntry(30U, 30U, 9.0F, 0, 0, 16, 16));
    ground.entries.push_back(visible);

    std::vector<cg::render::SpriteCmd> out;
    const std::size_t emitted = cg::world::AppendSceneGroundSpriteCmds(
        ground,
        cg::world::TileCoord{10, 20},
        cg::world::ScreenPixelCoord{300, 400},
        7U,
        cg::render::RectF{320.0F, 330.0F, 20.0F, 20.0F},
        out);

    failures += Expect(emitted == 1U, "ground camera overload 只提交可见 entry");
    failures += Expect(out.size() == 1U, "ground camera overload 跳过屏外 entry");
    if (out.size() == 1U) {
        failures += Expect(out[0].texture == texture, "ground 可见 sprite texture");
        failures += Expect(RectEquals(out[0].source, visible.source), "ground 可见 sprite source");
        failures += Expect(RectEquals(out[0].destination, visibleRect), "ground 可见 sprite destination");
        failures += Expect(out[0].order == cg::world::MakeIdaCompatibleOrder(7U + 1U), "ground 可见 sprite order 保留原始 entryIndex");
    }

    return failures;
}

int TestObjectRectAndBuilderCamera()
{
    int failures = 0;

    const cg::world::MapObject object = MakeObject();
    const cg::world::MapObjectTexture texture = MakeTexture();
    const cg::render::RectF objectRect = cg::world::MakeMapObjectSpriteRect(
        object,
        texture,
        cg::world::TileCoord{10, 20},
        cg::world::ScreenPixelCoord{300, 400});
    failures += Expect(RectEquals(objectRect, cg::render::RectF{328.0F, 334.0F, 64.0F, 48.0F}), "object rect 包含 texture offset 和尺寸");
    failures += Expect(
        cg::world::IsMapObjectSpriteVisible(
            cg::render::RectF{320.0F, 330.0F, 20.0F, 20.0F},
            object,
            texture,
            cg::world::TileCoord{10, 20},
            cg::world::ScreenPixelCoord{300, 400}),
        "object rect 与 camera 相交可见");

    const cg::world::MapObjectRenderInput input{&object, 1, 3, &texture};
    const std::optional<cg::world::GroupedMapObjectSpriteCmd> visible = cg::world::BuildMapObjectSpriteCmd(
        input,
        cg::world::TileCoord{10, 20},
        cg::world::ScreenPixelCoord{300, 400},
        8U,
        cg::render::RectF{320.0F, 330.0F, 20.0F, 20.0F});
    failures += Expect(visible.has_value(), "object camera overload 返回可见 sprite");
    if (visible.has_value()) {
        failures += Expect(RectEquals(visible->sprite.destination, objectRect), "object camera overload destination");
    }

    const std::optional<cg::world::GroupedMapObjectSpriteCmd> offscreen = cg::world::BuildMapObjectSpriteCmd(
        input,
        cg::world::TileCoord{10, 20},
        cg::world::ScreenPixelCoord{300, 400},
        8U,
        cg::render::RectF{0.0F, 0.0F, 10.0F, 10.0F});
    failures += Expect(!offscreen.has_value(), "object camera overload 跳过屏外 sprite");
    failures += Expect(object.objectIndex == 55, "objectIndex 仍只在 MapObject 输入中保留");

    return failures;
}

int TestEntityRect()
{
    int failures = 0;

    const cg::render::RectF entityRect = cg::world::MakeEntitySpriteRect(100.0F, 200.0F, -10.0F, 5.0F, 64.0F, 48.0F);
    failures += Expect(RectEquals(entityRect, cg::render::RectF{90.0F, 205.0F, 64.0F, 48.0F}), "entity rect 使用 anchor、drawOffset 和尺寸");
    failures += Expect(
        cg::world::IsEntitySpriteVisible(
            cg::render::RectF{80.0F, 200.0F, 20.0F, 20.0F},
            100.0F,
            200.0F,
            -10.0F,
            5.0F,
            64.0F,
            48.0F),
        "entity 与 camera 相交可见");
    failures += Expect(
        !cg::world::IsEntitySpriteVisible(
            cg::render::RectF{0.0F, 0.0F, 10.0F, 10.0F},
            100.0F,
            200.0F,
            -10.0F,
            5.0F,
            64.0F,
            48.0F),
        "entity 屏外不可见");
    failures += Expect(
        !cg::world::IsEntitySpriteVisible(
            cg::render::RectF{80.0F, 200.0F, 20.0F, 20.0F},
            100.0F,
            200.0F,
            -10.0F,
            5.0F,
            0.0F,
            48.0F),
        "entity width <= 0 不可见");
    failures += Expect(
        !cg::world::IsEntitySpriteVisible(
            cg::render::RectF{80.0F, 200.0F, 20.0F, 20.0F},
            100.0F,
            200.0F,
            -10.0F,
            5.0F,
            64.0F,
            -1.0F),
        "entity height <= 0 不可见");

    return failures;
}

int TestObjectIndexScope()
{
    int failures = 0;
    failures += Expect(HasObjectIndex<cg::world::MapObject>, "MapObject 保留 objectIndex");
    failures += Expect(!HasObjectIndex<cg::world::EntityData>, "EntityData 不新增 objectIndex");
    failures += Expect(!HasObjectIndex<cg::render::SpriteCmd>, "SpriteCmd 不新增 objectIndex");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSpriteVisible();
    failures += TestTileRectAndBuilderCamera();
    failures += TestObjectRectAndBuilderCamera();
    failures += TestEntityRect();
    failures += TestObjectIndexScope();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
