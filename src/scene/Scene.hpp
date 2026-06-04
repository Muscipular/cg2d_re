#pragma once

#include "scene/SceneTypes.hpp"

namespace cg::render {
class RenderQueue;
}

namespace cg::scene {

// IScene 是互斥 Root Scene 生命周期接口，不暴露底层窗口事件。
class IScene {
public:
    virtual ~IScene() = default;

    [[nodiscard]] virtual RootSceneId Id() const noexcept = 0;
    virtual void Enter() {}
    virtual void Leave() {}
    virtual void Update(float deltaSeconds) = 0;
    virtual bool HandleInput(const SceneInputEvent& event) = 0;
    virtual void Render(cg::render::RenderQueue& queue) const = 0;
};

// ISubScene 表示叠加在 Root Scene 上的局部流程或 UI 层。
class ISubScene {
public:
    virtual ~ISubScene() = default;

    [[nodiscard]] virtual SubSceneId Id() const noexcept = 0;
    [[nodiscard]] virtual SubScenePolicy Policy() const noexcept = 0;
    virtual void Enter() {}
    virtual void Leave() {}
    virtual void Update(float deltaSeconds) = 0;
    virtual bool HandleInput(const SceneInputEvent& event) = 0;
    virtual void Render(cg::render::RenderQueue& queue) const = 0;
};

} // namespace cg::scene
