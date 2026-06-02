#include "world/MapObjectSpriteBuilder.hpp"

#include "world/EntityData.hpp"
#include "world/MapObjectTextureCache.hpp"

#include <climits>
#include <cstdint>
#include <iostream>
#include <optional>

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

int TestBuildMapObjectSpriteCmd()
{
    int failures = 0;

    const cg::world::MapObject object = MakeObject();
    const cg::world::MapObjectTexture texture = MakeTexture();

    const std::optional<cg::world::GroupedMapObjectSpriteCmd> built = cg::world::BuildMapObjectSpriteCmd(
        cg::world::MapObjectRenderInput{&object, 1, 3, &texture},
        cg::world::TileCoord{10, 20},
        cg::world::ScreenPixelCoord{300, 400},
        7U);

    failures += Expect(built.has_value(), "合法 obj 生成 sprite");
    if (!built.has_value()) {
        return failures;
    }

    const cg::render::SpriteCmd& sprite = built->sprite;
    failures += Expect(built->group == cg::world::MapObjectRenderGroup::MapNoOne, "mapNo 1 分组");
    failures += Expect(sprite.texture == texture.texture, "texture handle 来自 MapObjectTexture");
    failures += Expect(RectEquals(sprite.source, cg::render::RectF{0.0F, 0.0F, 64.0F, 48.0F}), "source 使用整张纹理");
    failures += Expect(RectEquals(sprite.destination, cg::render::RectF{328.0F, 334.0F, 64.0F, 48.0F}), "destination 使用 tile 投影和纹理偏移");
    failures += Expect(sprite.order == static_cast<std::uint32_t>(UINT_MAX) - 7U, "order 使用 IDA 兼容顺序");
    failures += Expect(sprite.sortY == 0, "sortY 保持 0");
    failures += Expect(sprite.layer == 0U, "layer 保持默认 0");
    failures += Expect(object.objectIndex == 55, "objectIndex 只保留在 MapObject");

    return failures;
}

int TestBuildMapObjectSpriteCmdGroups()
{
    int failures = 0;

    const cg::world::MapObject object = MakeObject();
    const cg::world::MapObjectTexture texture = MakeTexture();

    const std::optional<cg::world::GroupedMapObjectSpriteCmd> mapNoOne = cg::world::BuildMapObjectSpriteCmd(
        cg::world::MapObjectRenderInput{&object, 1, 2, &texture},
        cg::world::TileCoord{},
        cg::world::ScreenPixelCoord{},
        0U);
    failures += Expect(
        mapNoOne.has_value() && mapNoOne->group == cg::world::MapObjectRenderGroup::MapNoOne,
        "mapNo 1 生成 MapNoOne 分组");

    const std::optional<cg::world::GroupedMapObjectSpriteCmd> flag2Three = cg::world::BuildMapObjectSpriteCmd(
        cg::world::MapObjectRenderInput{&object, 1000, 3, &texture},
        cg::world::TileCoord{},
        cg::world::ScreenPixelCoord{},
        0U);
    failures += Expect(
        flag2Three.has_value() && flag2Three->group == cg::world::MapObjectRenderGroup::HighMapNoFlag2Three,
        "mapNo 1000 flag2 3 生成 HighMapNoFlag2Three 分组");

    const std::optional<cg::world::GroupedMapObjectSpriteCmd> flag2Other = cg::world::BuildMapObjectSpriteCmd(
        cg::world::MapObjectRenderInput{&object, 1000, 2, &texture},
        cg::world::TileCoord{},
        cg::world::ScreenPixelCoord{},
        0U);
    failures += Expect(
        flag2Other.has_value() && flag2Other->group == cg::world::MapObjectRenderGroup::HighMapNoOtherFlag2,
        "mapNo 1000 flag2 2 生成 HighMapNoOtherFlag2 分组");

    return failures;
}

int TestBuildMapObjectSpriteCmdSkips()
{
    int failures = 0;

    const cg::world::MapObject object = MakeObject();
    cg::world::MapObjectTexture texture = MakeTexture();

    failures += Expect(
        !cg::world::BuildMapObjectSpriteCmd(
            cg::world::MapObjectRenderInput{&object, 999, 3, &texture},
            cg::world::TileCoord{},
            cg::world::ScreenPixelCoord{},
            0U)
             .has_value(),
        "mapNo 999 不生成");
    failures += Expect(
        !cg::world::BuildMapObjectSpriteCmd(
            cg::world::MapObjectRenderInput{&object, 0, 0, &texture},
            cg::world::TileCoord{},
            cg::world::ScreenPixelCoord{},
            0U)
             .has_value(),
        "mapNo 0 不生成");
    failures += Expect(
        !cg::world::BuildMapObjectSpriteCmd(
            cg::world::MapObjectRenderInput{nullptr, 1, 3, &texture},
            cg::world::TileCoord{},
            cg::world::ScreenPixelCoord{},
            0U)
             .has_value(),
        "null object 跳过");
    failures += Expect(
        !cg::world::BuildMapObjectSpriteCmd(
            cg::world::MapObjectRenderInput{&object, 1, 3, nullptr},
            cg::world::TileCoord{},
            cg::world::ScreenPixelCoord{},
            0U)
             .has_value(),
        "null texture 跳过");

    texture.texture = {};
    failures += Expect(
        !cg::world::BuildMapObjectSpriteCmd(
            cg::world::MapObjectRenderInput{&object, 1, 3, &texture},
            cg::world::TileCoord{},
            cg::world::ScreenPixelCoord{},
            0U)
             .has_value(),
        "invalid texture 跳过");

    texture = MakeTexture();
    texture.width = 0;
    failures += Expect(
        !cg::world::BuildMapObjectSpriteCmd(
            cg::world::MapObjectRenderInput{&object, 1, 3, &texture},
            cg::world::TileCoord{},
            cg::world::ScreenPixelCoord{},
            0U)
             .has_value(),
        "width <= 0 跳过");

    texture = MakeTexture();
    texture.height = 0;
    failures += Expect(
        !cg::world::BuildMapObjectSpriteCmd(
            cg::world::MapObjectRenderInput{&object, 1, 3, &texture},
            cg::world::TileCoord{},
            cg::world::ScreenPixelCoord{},
            0U)
             .has_value(),
        "height <= 0 跳过");

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
    failures += TestBuildMapObjectSpriteCmd();
    failures += TestBuildMapObjectSpriteCmdGroups();
    failures += TestBuildMapObjectSpriteCmdSkips();
    failures += TestObjectIndexScope();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
