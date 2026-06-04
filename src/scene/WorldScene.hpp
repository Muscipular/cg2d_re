#pragma once

#include "scene/Scene.hpp"
#include "world/EntityStore.hpp"

namespace cg::scene {

// WorldScene 是 scene 层唯一直接持有 EntityStore 的最小桥接。
class WorldScene final : public IScene {
public:
    [[nodiscard]] RootSceneId Id() const noexcept override;
    void Update(float deltaSeconds) override;
    [[nodiscard]] bool HandleInput(const SceneInputEvent& event) override;
    void Render(cg::render::RenderQueue& queue) const override;

    [[nodiscard]] std::size_t ActiveEntityCount() const noexcept;
    [[nodiscard]] cg::world::EntityId CreateDemoEntity();
    void ClearWorld();

private:
    cg::world::EntityStore entities_;
};

} // namespace cg::scene
