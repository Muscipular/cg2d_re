#include "render/IRenderer.hpp"
#include "render/RenderQueue.hpp"
#include "world/MapObjectSpriteBuilder.hpp"
#include "world/MapObjectTextureCache.hpp"
#include "world/SceneGroundSpriteBuilder.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <optional>
#include <vector>

namespace {

class RecordingRenderer final : public cg::render::IRenderer {
public:
    [[nodiscard]] bool BeginFrame(cg::render::ClearColor) override
    {
        return true;
    }

    [[nodiscard]] bool EndFrame() override
    {
        return true;
    }

    [[nodiscard]] bool SetLogicalPresentation(const cg::render::LogicalPresentation&) override
    {
        return true;
    }

    void ResetStats() noexcept override {}

    [[nodiscard]] cg::render::RenderStats GetStats() const noexcept override
    {
        return {};
    }

    [[nodiscard]] cg::render::TextureHandle CreateTexture(const cg::render::TextureDesc&) override
    {
        return {};
    }

    [[nodiscard]] bool UpdateTexture(cg::render::TextureHandle, const void*, int) override
    {
        return true;
    }

    void DestroyTexture(cg::render::TextureHandle) noexcept override {}

    [[nodiscard]] bool DrawTexture(cg::render::TextureHandle, const cg::render::RectF&, const cg::render::RectF&) override
    {
        return true;
    }

    [[nodiscard]] bool DrawSprites(const cg::render::SpriteCmd* sprites, std::size_t count) override
    {
        for (std::size_t index = 0; index < count; ++index) {
            drawnSprites.push_back(sprites[index]);
        }

        return true;
    }

    std::vector<cg::render::SpriteCmd> drawnSprites;
};

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

[[nodiscard]] cg::render::SpriteCmd MakeMarkedSprite(
    std::uint32_t marker,
    std::uint32_t layer,
    std::uint32_t cacheIndex)
{
    cg::render::SpriteCmd cmd{};
    cmd.texture = cg::render::TextureHandle{1U, 1U};
    cmd.source = cg::render::RectF{static_cast<float>(marker), 0.0F, 1.0F, 1.0F};
    cmd.destination = cg::render::RectF{0.0F, 0.0F, 1.0F, 1.0F};
    cmd.layer = layer;
    cmd.sortY = 0;
    cmd.order = cg::world::MakeIdaCompatibleOrder(cacheIndex);
    return cmd;
}

template <std::size_t Count>
[[nodiscard]] bool SourceMarkersEqual(
    const std::vector<cg::render::SpriteCmd>& sprites,
    const std::array<std::uint32_t, Count>& expected)
{
    if (sprites.size() != expected.size()) {
        return false;
    }

    for (std::size_t index = 0; index < expected.size(); ++index) {
        if (static_cast<std::uint32_t>(sprites[index].source.x) != expected[index]) {
            return false;
        }
    }

    return true;
}

template <std::size_t Count>
[[nodiscard]] bool LayersEqual(
    const std::vector<cg::render::SpriteCmd>& sprites,
    const std::array<std::uint32_t, Count>& expected)
{
    if (sprites.size() != expected.size()) {
        return false;
    }

    for (std::size_t index = 0; index < expected.size(); ++index) {
        if (sprites[index].layer != expected[index]) {
            return false;
        }
    }

    return true;
}

int TestSameLayerIdaCacheIndexOrder()
{
    int failures = 0;

    cg::render::RenderQueue queue;
    queue.PushSprite(MakeMarkedSprite(0U, 5U, 0U));
    queue.PushSprite(MakeMarkedSprite(1U, 5U, 1U));
    queue.PushSprite(MakeMarkedSprite(2U, 5U, 2U));

    RecordingRenderer renderer;
    failures += Expect(queue.Flush(renderer), "RenderQueue Flush 成功");
    failures += Expect(
        SourceMarkersEqual(renderer.drawnSprites, std::array<std::uint32_t, 3U>{2U, 1U, 0U}),
        "同 layer/sortY/cacheIndex 按 IDA 逆序绘制");

    return failures;
}

int TestLayerBeforeCacheIndexOrder()
{
    int failures = 0;

    cg::render::RenderQueue queue;
    queue.PushSprite(MakeMarkedSprite(5U, 5U, 2U));
    queue.PushSprite(MakeMarkedSprite(1U, 1U, 0U));

    RecordingRenderer renderer;
    failures += Expect(queue.Flush(renderer), "RenderQueue Flush 成功");
    failures += Expect(
        LayersEqual(renderer.drawnSprites, std::array<std::uint32_t, 2U>{1U, 5U}),
        "layer 1 早于 layer 5");

    return failures;
}

int TestGroundBuilderRenderQueueOrder()
{
    int failures = 0;

    cg::world::SceneGroundAtlas ground{};
    ground.atlas.SetTexture(cg::render::TextureHandle{2U, 1U});
    ground.entries.push_back(cg::world::SceneGroundAtlasEntry{
        10U,
        20U,
        1000U,
        cg::world::TileRenderLayer::Normal,
        cg::render::RectF{0.0F, 0.0F, 8.0F, 8.0F},
        0,
        0,
        8,
        8,
    });
    ground.entries.push_back(cg::world::SceneGroundAtlasEntry{
        10U,
        20U,
        1001U,
        cg::world::TileRenderLayer::Normal,
        cg::render::RectF{1.0F, 0.0F, 8.0F, 8.0F},
        0,
        0,
        8,
        8,
    });
    ground.entries.push_back(cg::world::SceneGroundAtlasEntry{
        10U,
        20U,
        5000U,
        cg::world::TileRenderLayer::Special,
        cg::render::RectF{2.0F, 0.0F, 8.0F, 8.0F},
        0,
        0,
        8,
        8,
    });

    std::vector<cg::render::SpriteCmd> sprites;
    const std::size_t emitted = cg::world::AppendSceneGroundSpriteCmds(
        ground,
        cg::world::TileCoord{10, 20},
        cg::world::ScreenPixelCoord{0, 0},
        0U,
        sprites);

    cg::render::RenderQueue queue;
    for (const cg::render::SpriteCmd& sprite : sprites) {
        queue.PushSprite(sprite);
    }

    RecordingRenderer renderer;
    failures += Expect(emitted == 3U, "ground builder 输出三条 entry");
    failures += Expect(queue.Flush(renderer), "RenderQueue Flush 成功");
    failures += Expect(
        SourceMarkersEqual(renderer.drawnSprites, std::array<std::uint32_t, 3U>{2U, 1U, 0U}),
        "ground 特殊 layer 先绘制，普通同 layer 后生成先绘制");
    failures += Expect(
        LayersEqual(renderer.drawnSprites, std::array<std::uint32_t, 3U>{1U, 5U, 5U}),
        "ground layer 1 早于普通 layer 5");

    return failures;
}

int TestObjectBuilderGroupAndDefaultOrder()
{
    int failures = 0;

    const cg::world::MapObject object{
        7,
        cg::world::TileCoord{10, 20},
        1000U,
    };
    const cg::world::MapObjectTexture texture{
        1000U,
        cg::render::TextureHandle{3U, 1U},
        16,
        16,
        0,
        0,
    };

    const std::optional<cg::world::GroupedMapObjectSpriteCmd> built = cg::world::BuildMapObjectSpriteCmd(
        cg::world::MapObjectRenderInput{&object, 1000, 3, &texture},
        cg::world::TileCoord{10, 20},
        cg::world::ScreenPixelCoord{0, 0},
        4U);

    failures += Expect(built.has_value(), "object builder 输出 sprite");
    if (!built.has_value()) {
        return failures;
    }

    failures += Expect(
        built->group == cg::world::MapObjectRenderGroup::HighMapNoFlag2Three,
        "object builder 保留 group");
    failures += Expect(built->sprite.layer == 0U, "object sprite layer 默认 0");
    failures += Expect(built->sprite.sortY == 0, "object sprite sortY 默认 0");
    failures += Expect(
        built->sprite.order == cg::world::MakeIdaCompatibleOrder(4U),
        "object sprite order 使用 IDA 兼容顺序");

    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSameLayerIdaCacheIndexOrder();
    failures += TestLayerBeforeCacheIndexOrder();
    failures += TestGroundBuilderRenderQueueOrder();
    failures += TestObjectBuilderGroupAndDefaultOrder();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
