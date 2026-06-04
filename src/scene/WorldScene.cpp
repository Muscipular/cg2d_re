#include "scene/WorldScene.hpp"

#include "render/RenderQueue.hpp"

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

void WorldScene::Render(cg::render::RenderQueue& queue) const
{
    for (const cg::render::SpriteCmd& sprite : renderSprites_) {
        queue.PushSprite(sprite);
    }
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

void WorldScene::SetRenderSprites(std::span<const cg::render::SpriteCmd> sprites)
{
    renderSprites_.assign(sprites.begin(), sprites.end());
}

} // namespace cg::scene
