#pragma once

#include "render/SpriteCmd.hpp"
#include "scene/Scene.hpp"
#include "world/EntityStore.hpp"

#include <functional>
#include <span>
#include <vector>

namespace cg::scene {

// WorldScene 是 scene 层唯一直接持有 EntityStore 的最小桥接。
class WorldScene final : public IScene {
public:
    using RenderProvider = std::function<void(float deltaSeconds, std::vector<cg::render::SpriteCmd>& out)>;

    [[nodiscard]] RootSceneId Id() const noexcept override;
    void Update(float deltaSeconds) override;
    [[nodiscard]] bool HandleInput(const SceneInputEvent& event) override;
    void Render(cg::render::RenderQueue& queue) const override;

    [[nodiscard]] std::size_t ActiveEntityCount() const noexcept;
    [[nodiscard]] cg::world::EntityId CreateEntity();
    void ClearWorld();
    void SetRenderProvider(RenderProvider provider);
    void SetRenderSprites(std::span<const cg::render::SpriteCmd> sprites);

private:
    cg::world::EntityStore entities_;
    std::vector<cg::render::SpriteCmd> renderSprites_;
    RenderProvider renderProvider_;
};

} // namespace cg::scene
