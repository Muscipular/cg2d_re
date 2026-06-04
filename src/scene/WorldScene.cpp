#include "scene/WorldScene.hpp"

namespace cg::scene {

RootSceneId WorldScene::Id() const noexcept
{
    return RootSceneId::World;
}

void WorldScene::Update(float)
{
}

bool WorldScene::HandleInput(const SceneInputEvent&)
{
    return false;
}

void WorldScene::Render(cg::render::RenderQueue&) const
{
}

std::size_t WorldScene::ActiveEntityCount() const noexcept
{
    return entities_.ActiveCount();
}

cg::world::EntityId WorldScene::CreateDemoEntity()
{
    return entities_.Create();
}

void WorldScene::ClearWorld()
{
    entities_.Clear();
}

} // namespace cg::scene
