#pragma once

#include <cstdint>

namespace cg::world {

// 原 0xFF0D40；引用 platform::gValueFF0D40，不建立第二份当前场景类型存储。
extern std::int32_t& gCurrentSceneType;

extern std::int32_t gPendingSceneState; // 原 0xFF0D60。
extern std::int32_t gCurrentSceneState; // 原 0xFF0D64。
extern std::int32_t gPendingSceneType;  // 原 0xFF0D6C。

// 原 sub_51C770@0x51C770；先清当前场景状态，再写当前场景类型，返回 sceneType。
std::int32_t SetCurrentSceneType(std::int32_t sceneType);

// 原 sub_51C790@0x51C790；依次写当前场景类型、状态，返回 sceneType。
std::int32_t SetCurrentSceneTypeAndState(std::int32_t sceneType, std::int32_t sceneState);

// 原 sub_51C7B0@0x51C7B0；先清待切换场景状态，再写待切换场景类型，返回 sceneType。
std::int32_t SetPendingSceneType(std::int32_t sceneType);

} // namespace cg::world
