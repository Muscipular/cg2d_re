#pragma once

#include "scene/Scene.hpp"

#include <deque>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace cg::scene {

// SceneManager 管理 Root Scene 延迟切换和 SubScene 栈。
class SceneManager final {
public:
    using RootSceneFactory = std::function<std::unique_ptr<IScene>()>;

    void RegisterRootSceneFactory(RootSceneId id, RootSceneFactory factory);

    [[nodiscard]] bool Start(RootSceneId id);
    void ReplaceRootScene(RootSceneId id);

    void PushSubScene(std::unique_ptr<ISubScene> scene);
    void PopSubScene();
    [[nodiscard]] bool PopSubScene(SubSceneId id);
    void ClearSubScenes();

    void Update(float deltaSeconds);
    [[nodiscard]] bool HandleInput(const SceneInputEvent& event);
    void Render(cg::render::RenderQueue& queue) const;

    [[nodiscard]] RootSceneId CurrentRootSceneId() const noexcept;
    [[nodiscard]] std::size_t PendingRootSceneCount() const noexcept;
    [[nodiscard]] std::size_t SubSceneCount() const noexcept;

private:
    [[nodiscard]] bool HasPendingRootScene(RootSceneId id) const noexcept;
    void ConsumeOneRootSwitch();

    std::unordered_map<RootSceneId, RootSceneFactory> rootFactories_;
    std::deque<RootSceneId> pendingRootScenes_;
    std::unique_ptr<IScene> currentRootScene_;
    std::vector<std::unique_ptr<ISubScene>> subScenes_;
};

} // namespace cg::scene
