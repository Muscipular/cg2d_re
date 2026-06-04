#include "scene/SceneManager.hpp"

#include <algorithm>
#include <utility>

namespace cg::scene {

void SceneManager::RegisterRootSceneFactory(RootSceneId id, RootSceneFactory factory)
{
    rootFactories_[id] = std::move(factory);
}

bool SceneManager::Start(RootSceneId id)
{
    if (currentRootScene_) {
        return false;
    }

    const auto found = rootFactories_.find(id);
    if (found == rootFactories_.end()) {
        return false;
    }

    std::unique_ptr<IScene> scene = found->second();
    if (!scene) {
        return false;
    }

    currentRootScene_ = std::move(scene);
    currentRootScene_->Enter();
    return true;
}

void SceneManager::ReplaceRootScene(RootSceneId id)
{
    if (currentRootScene_ && currentRootScene_->Id() == id) {
        return;
    }
    if (HasPendingRootScene(id)) {
        return;
    }

    pendingRootScenes_.push_back(id);
}

void SceneManager::PushSubScene(std::unique_ptr<ISubScene> scene)
{
    if (!scene) {
        return;
    }

    scene->Enter();
    subScenes_.push_back(std::move(scene));
}

void SceneManager::PopSubScene()
{
    if (subScenes_.empty()) {
        return;
    }

    subScenes_.back()->Leave();
    subScenes_.pop_back();
}

void SceneManager::ClearSubScenes()
{
    while (!subScenes_.empty()) {
        PopSubScene();
    }
}

void SceneManager::Update(float deltaSeconds)
{
    std::size_t firstUpdatedSubScene = 0U;
    bool updateRoot = true;
    for (std::size_t index = subScenes_.size(); index > 0U; --index) {
        const SubScenePolicy policy = subScenes_[index - 1U]->Policy();
        if (policy.pausesBelow) {
            updateRoot = false;
            firstUpdatedSubScene = index - 1U;
            break;
        }
    }

    if (updateRoot && currentRootScene_) {
        currentRootScene_->Update(deltaSeconds);
    }

    for (std::size_t index = firstUpdatedSubScene; index < subScenes_.size(); ++index) {
        subScenes_[index]->Update(deltaSeconds);
    }

    ConsumeOneRootSwitch();
}

bool SceneManager::HandleInput(const SceneInputEvent& event)
{
    for (std::size_t index = subScenes_.size(); index > 0U; --index) {
        ISubScene& scene = *subScenes_[index - 1U];
        const SubScenePolicy policy = scene.Policy();
        if (policy.receivesInput && scene.HandleInput(event)) {
            return true;
        }
        if (policy.modal) {
            return true;
        }
    }

    if (!currentRootScene_) {
        return false;
    }

    return currentRootScene_->HandleInput(event);
}

void SceneManager::Render(cg::render::RenderQueue& queue) const
{
    std::size_t firstRenderedSubScene = 0U;
    for (std::size_t index = subScenes_.size(); index > 0U; --index) {
        const SubScenePolicy policy = subScenes_[index - 1U]->Policy();
        if (!policy.transparentRender) {
            firstRenderedSubScene = index - 1U;
            break;
        }
    }

    if (currentRootScene_) {
        currentRootScene_->Render(queue);
    }

    for (std::size_t index = firstRenderedSubScene; index < subScenes_.size(); ++index) {
        subScenes_[index]->Render(queue);
    }
}

RootSceneId SceneManager::CurrentRootSceneId() const noexcept
{
    if (!currentRootScene_) {
        return RootSceneId::Boot;
    }

    return currentRootScene_->Id();
}

std::size_t SceneManager::PendingRootSceneCount() const noexcept
{
    return pendingRootScenes_.size();
}

std::size_t SceneManager::SubSceneCount() const noexcept
{
    return subScenes_.size();
}

bool SceneManager::HasPendingRootScene(RootSceneId id) const noexcept
{
    return std::find(pendingRootScenes_.begin(), pendingRootScenes_.end(), id) != pendingRootScenes_.end();
}

void SceneManager::ConsumeOneRootSwitch()
{
    if (pendingRootScenes_.empty()) {
        return;
    }

    const RootSceneId nextId = pendingRootScenes_.front();
    pendingRootScenes_.pop_front();

    if (currentRootScene_ && currentRootScene_->Id() == nextId) {
        return;
    }

    const auto found = rootFactories_.find(nextId);
    if (found == rootFactories_.end()) {
        return;
    }

    std::unique_ptr<IScene> next = found->second();
    if (!next) {
        return;
    }

    if (currentRootScene_) {
        currentRootScene_->Leave();
    }
    currentRootScene_ = std::move(next);
    currentRootScene_->Enter();
}

} // namespace cg::scene
