#include "cg/world/SceneState.hpp"

#include "cg/platform/CommandOptions.hpp"

namespace cg::world {

std::int32_t& gCurrentSceneType = platform::gValueFF0D40;
std::int32_t gPendingSceneState = 0;
std::int32_t gCurrentSceneState = 0;
std::int32_t gPendingSceneType = 0;

std::uint8_t gValueC82204 = 0;
std::uint32_t gValue5ABFC4 = 0;
std::uint32_t gValue9A6E0C = 0;
std::uint32_t gValueFF0D5C = 0;
std::uint32_t gValueFF0D58 = 0;
std::uint32_t gValueD67E78 = 0;
std::uint32_t gValueFF0D74 = 0;
std::uint8_t gValue9DAD30 = 0;
std::array<std::uint32_t, 7> gValueD67B08 {};
std::uint32_t gValue58B6D0 = 1;
std::uint8_t gValueD9F2CA = 0;

static_assert(sizeof(gValueD67B08) == 7 * sizeof(std::uint32_t));

std::int32_t SetCurrentSceneType(std::int32_t sceneType)
{
    gCurrentSceneState = 0;
    gCurrentSceneType = sceneType;
    return sceneType;
}

std::int32_t SetCurrentSceneTypeAndState(std::int32_t sceneType, std::int32_t sceneState)
{
    gCurrentSceneType = sceneType;
    gCurrentSceneState = sceneState;
    return sceneType;
}

std::int32_t SetPendingSceneType(std::int32_t sceneType)
{
    gPendingSceneState = 0;
    gPendingSceneType = sceneType;
    return sceneType;
}

} // namespace cg::world
