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

    if (currentSubScene_) {
        currentSubScene_->Leave();
    }
    currentSubScene_ = std::move(scene);
    currentSubScene_->Enter();
}

void SceneManager::PopSubScene()
{
    if (!currentSubScene_) {
        return;
    }

    currentSubScene_->Leave();
    currentSubScene_.reset();
}

bool SceneManager::PopSubScene(SubSceneId id)
{
    if (!currentSubScene_ || currentSubScene_->Id() != id) {
        return false;
    }

    PopSubScene();
    return true;
}

void SceneManager::ClearSubScenes()
{
    PopSubScene();
}

void SceneManager::Update(float deltaSeconds)
{
    const bool updateRoot = !currentSubScene_ || !currentSubScene_->Policy().pausesBelow;

    if (updateRoot && currentRootScene_) {
        currentRootScene_->Update(deltaSeconds);
    }

    if (currentSubScene_) {
        currentSubScene_->Update(deltaSeconds);
    }

    ConsumeOneRootSwitch();
}

bool SceneManager::HandleInput(const SceneInputEvent& event)
{
    if (currentSubScene_) {
        const SubScenePolicy policy = currentSubScene_->Policy();
        if (policy.receivesInput && currentSubScene_->HandleInput(event)) {
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
    if (currentRootScene_) {
        currentRootScene_->Render(queue);
    }

    if (currentSubScene_) {
        currentSubScene_->Render(queue);
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
    return currentSubScene_ ? 1U : 0U;
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
