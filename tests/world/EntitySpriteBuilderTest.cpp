#include "world/EntitySpriteBuilder.hpp"

#include "resource/ResourceTypes.hpp"
#include "world/EntityStore.hpp"

#include <cstdint>
#include <iostream>
#include <optional>
#include <unordered_map>
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

[[nodiscard]] bool NearlyEqual(float lhs, float rhs) noexcept
{
    const float diff = lhs - rhs;
    return diff > -0.0001F && diff < 0.0001F;
}

[[nodiscard]] bool RectEquals(cg::render::RectF lhs, cg::render::RectF rhs) noexcept
{
    return NearlyEqual(lhs.x, rhs.x)
        && NearlyEqual(lhs.y, rhs.y)
        && NearlyEqual(lhs.width, rhs.width)
        && NearlyEqual(lhs.height, rhs.height);
}

class FakeGraphicResolver final : public cg::world::IEntitySpriteGraphicResolver {
public:
    [[nodiscard]] std::optional<cg::world::EntitySpriteGraphic> ResolveEntityGraphic(std::int32_t graphicSeqNo) override
    {
        requests.push_back(graphicSeqNo);
        const auto iter = graphics.find(graphicSeqNo);
        if (iter == graphics.end()) {
            return std::nullopt;
        }

        return iter->second;
    }

    std::unordered_map<std::int32_t, cg::world::EntitySpriteGraphic> graphics;
    std::vector<std::int32_t> requests;
};

[[nodiscard]] cg::resource::AnimeFrame MakeFrame(std::int32_t graphicSeqNo, std::int16_t x, std::int16_t y)
{
    cg::resource::AnimeFrame frame;
    frame.graphicSeqNo = graphicSeqNo;
    frame.x = x;
    frame.y = y;
    return frame;
}

[[nodiscard]] cg::resource::AnimeAction MakeAction(
    std::int16_t action,
    cg::world::Direction direction,
    std::int32_t duration,
    int frameIndex,
    std::int32_t frameCount)
{
    cg::resource::AnimeAction result;
    result.header.action = static_cast<cg::resource::CG_ActionType>(action);
    result.header.dir = static_cast<cg::resource::CG_Direction>(static_cast<int>(direction));
    result.header.duration = duration;
    result.header.frameCount = frameCount;
    result.frameIndex = frameIndex;
    return result;
}

[[nodiscard]] cg::resource::AnimationClip MakeWalkClip()
{
    cg::resource::AnimationClip clip;
    clip.actions.push_back(MakeAction(
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        cg::world::Direction::East,
        200,
        0,
        2));
    clip.frames.push_back(MakeFrame(101, 0, 0));
    clip.frames.push_back(MakeFrame(102, 5, -2));
    return clip;
}

[[nodiscard]] cg::world::EntitySpriteBuildInput MakeBuildInput(
    cg::world::EntityId id,
    const cg::world::Movement& movement,
    cg::world::AnimationState& animation,
    const cg::resource::AnimationClip& clip,
    cg::world::IEntitySpriteGraphicResolver& resolver)
{
    cg::world::EntitySpriteBuildInput input;
    input.id = id;
    input.movement = &movement;
    input.animation = &animation;
    input.clip = &clip;
    input.graphicResolver = &resolver;
    input.movingAction = static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK);
    input.originTile = cg::world::TileCoord{0, 0};
    input.originScreen = cg::world::ScreenPixelCoord{320, 240};
    input.layer = 9U;
    input.sortY = 33;
    return input;
}

int TestBuildWalkingEntitySprite()
{
    cg::world::EntityStore store;
    const cg::world::EntityId id = store.Create();
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {0, 0},
        {128, 0},
        1000,
        400,
        cg::world::Direction::East);
    cg::world::AnimationState animation{};
    const cg::resource::AnimationClip clip = MakeWalkClip();
    FakeGraphicResolver resolver;
    resolver.graphics.emplace(
        102,
        cg::world::EntitySpriteGraphic{
            cg::render::TextureHandle{7U, 1U},
            cg::render::RectF{10.0F, 20.0F, 16.0F, 24.0F},
            16,
            24,
            3,
            4,
        });

    cg::world::EntitySpriteBuildInput input = MakeBuildInput(id, movement, animation, clip, resolver);
    input.nowTimeMs = 1100;
    input.deltaMs = 100;

    const std::optional<cg::world::WorldEntitySprite> sprite = cg::world::BuildEntitySprite(input);

    int failures = 0;
    failures += Expect(sprite.has_value(), "移动中实体应生成 sprite");
    failures += Expect(animation.action == cg::resource::CG_ACTIONTYPE_WALK, "移动中写入 walk action");
    failures += Expect(animation.direction == cg::world::Direction::East, "动画方向跟随移动方向");
    failures += Expect(animation.frameIndex == 1U, "移动 delta 推进动画帧");
    failures += Expect(!resolver.requests.empty() && resolver.requests[0] == 102, "使用推进后的动画帧解析图档");
    if (!sprite.has_value()) {
        return failures;
    }

    failures += Expect(sprite->id == id, "sprite id 来自实体");
    failures += Expect(sprite->texture == cg::render::TextureHandle{7U, 1U}, "sprite texture 来自 resolver");
    failures += Expect(RectEquals(sprite->source, cg::render::RectF{10.0F, 20.0F, 16.0F, 24.0F}), "sprite source");
    failures += Expect(NearlyEqual(sprite->anchorScreenX, 336.0F), "anchor x 来自移动中点投影");
    failures += Expect(NearlyEqual(sprite->anchorScreenY, 228.0F), "anchor y 来自移动中点投影");
    failures += Expect(NearlyEqual(sprite->drawOffsetX, 8.0F), "draw offset x 叠加 frame 和 graphic offset");
    failures += Expect(NearlyEqual(sprite->drawOffsetY, 2.0F), "draw offset y 叠加 frame 和 graphic offset");
    failures += Expect(NearlyEqual(sprite->width, 16.0F), "sprite width");
    failures += Expect(NearlyEqual(sprite->height, 24.0F), "sprite height");
    failures += Expect(sprite->layer == 9U, "sprite layer 由调用方提供");
    failures += Expect(sprite->sortY == 33, "sprite sortY 由调用方提供");
    return failures;
}

int TestRejectInvalidInputs()
{
    cg::world::EntityStore store;
    const cg::world::EntityId id = store.Create();
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {0, 0},
        {64, 0},
        0,
        100,
        cg::world::Direction::East);
    cg::world::AnimationState animation{};
    const cg::resource::AnimationClip clip = MakeWalkClip();
    FakeGraphicResolver resolver;

    int failures = 0;
    cg::world::EntitySpriteBuildInput input = MakeBuildInput(id, movement, animation, clip, resolver);
    input.nowTimeMs = 50;
    input.deltaMs = 50;
    failures += Expect(!cg::world::BuildEntitySprite(input).has_value(), "缺图档解析结果时不生成 sprite");

    resolver.graphics.emplace(
        101,
        cg::world::EntitySpriteGraphic{
            cg::render::TextureHandle{},
            cg::render::RectF{0.0F, 0.0F, 1.0F, 1.0F},
            1,
            1,
            0,
            0,
        });
    cg::world::AnimationState invalidTextureAnimation{};
    input.animation = &invalidTextureAnimation;
    failures += Expect(!cg::world::BuildEntitySprite(input).has_value(), "无效 texture 不生成 sprite");

    input.id = cg::world::EntityId{};
    failures += Expect(!cg::world::BuildEntitySprite(input).has_value(), "无效 EntityId 不生成 sprite");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestBuildWalkingEntitySprite();
    failures += TestRejectInvalidInputs();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
