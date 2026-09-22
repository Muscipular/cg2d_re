#pragma once

#include <array>
#include <cstdint>

namespace cg::world {

// 原 0xFF0D40；引用 platform::gValueFF0D40，不建立第二份当前场景类型存储。
extern std::int32_t& gCurrentSceneType;

extern std::int32_t gPendingSceneState; // 原 0xFF0D60。
extern std::int32_t gCurrentSceneState; // 原 0xFF0D64。
extern std::int32_t gPendingSceneType;  // 原 0xFF0D6C。

// World::Update@0x51C430 直接访问的剩余原全局；地址以外的业务含义仍未确认。
extern std::uint8_t gValueC82204;              // 原 0xC82204，W8。
extern std::uint32_t gValue5ABFC4;             // 原 0x5ABFC4，W32。
extern std::uint32_t gValue9A6E0C;             // 原 0x9A6E0C，W32。
extern std::uint32_t gValueFF0D5C;             // 原 0xFF0D5C，W32。
extern std::uint32_t gValueFF0D58;             // 原 0xFF0D58，W32。
extern std::uint32_t gValueD67E78;             // 原 0xD67E78，W32。
extern std::uint32_t gValueFF0D74;             // 原 0xFF0D74，W32。
extern std::uint8_t gValue9DAD30;              // 原 0x9DAD30，W8。
extern std::array<std::uint32_t, 7> gValueD67B08; // 原 0xD67B08，严格 7 个 DWORD。
extern std::uint32_t gValue58B6D0;             // 原 0x58B6D0，W32，映像初值 1。
extern std::uint8_t gValueD9F2CA;              // 原 0xD9F2CA，W8。

// 原世界等距投影状态的当前值与帧末快照；八槽均为独立 uint32_t，映像初值 0。
extern std::uint32_t gWorldProjectionSourceA;   // 原 0x9EE908，R32。
extern std::uint32_t gWorldProjectionSourceB;   // 原 0x9EE90C，R32。
extern std::uint32_t gWorldProjectionSourceC;   // 原 0x9EE904，R32。
extern std::uint32_t gWorldProjectionSourceD;   // 原 0x9EE8F4，R32。
extern std::uint32_t gWorldProjectionSnapshotA; // 原 0xAF10C4，W32。
extern std::uint32_t gWorldProjectionSnapshotB; // 原 0xAF10CC，W32。
extern std::uint32_t gWorldProjectionSnapshotC; // 原 0xAF10D0，W32。
extern std::uint32_t gWorldProjectionSnapshotD; // 原 0xAF10D4，W32。

// 原 sub_51C770@0x51C770；先清当前场景状态，再写当前场景类型，返回 sceneType。
std::int32_t SetCurrentSceneType(std::int32_t sceneType);

// 原 sub_51C790@0x51C790；依次写当前场景类型、状态，返回 sceneType。
std::int32_t SetCurrentSceneTypeAndState(std::int32_t sceneType, std::int32_t sceneState);

// 原 sub_51C7B0@0x51C7B0；先清待切换场景状态，再写待切换场景类型，返回 sceneType。
std::int32_t SetPendingSceneType(std::int32_t sceneType);

// 原 sub_468FF0@0x468FF0；按原指令顺序把世界投影当前值复制到帧末快照，返回 D。
std::uint32_t CopyWorldProjectionState_468FF0();

// 原 World::Update@0x51C430；完整场景状态消费与分发，场景目标仍为真实未实现依赖。
void Update();

} // namespace cg::world
