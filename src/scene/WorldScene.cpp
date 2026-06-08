#include "scene/WorldScene.hpp"

#include "render/RenderQueue.hpp"

#include <utility>

namespace cg::scene {

RootSceneId WorldScene::Id() const noexcept
{
    return RootSceneId::World;
}

void WorldScene::Update(float deltaSeconds)
{
    if (!renderProvider_) {
        return;
    }

    std::vector<cg::render::SpriteCmd> sprites;
    renderProvider_(deltaSeconds, sprites);
    renderSprites_ = std::move(sprites);
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

cg::world::EntityId WorldScene::CreateEntity()
{
    return entities_.Create();
}

void WorldScene::ClearWorld()
{
    entities_.Clear();
}

void WorldScene::SetRenderProvider(RenderProvider provider)
{
    renderProvider_ = std::move(provider);
}

void WorldScene::SetRenderSprites(std::span<const cg::render::SpriteCmd> sprites)
{
    renderSprites_.assign(sprites.begin(), sprites.end());
}

} // namespace cg::scene
