#pragma once

#include <cstdint>
#include <string>

namespace cg::yobi { struct YobiAction; }
namespace cg::world {

// 断线场景确认框的文字、布局和动作借用。
struct DisconnectDialogState {
    std::wstring text; // 原 0x9A4D10。
    yobi::YobiAction* action = nullptr; // 原 0x62B5B4。
    std::int32_t sentinel = 0; // 原 0x62B3A8。
    std::int32_t width = 0; // 原 0x62B32C。
    std::int32_t height = 0; // 原 0x62B304。
    std::int32_t x = 0; // 原 0x62B390。
    std::int32_t y = 0; // 原 0x62B2EC。
};

// 场景 7 的登出过渡、请求进度与结果提示；动作由 Yobi 持有。
struct LogoutSceneState {
    std::uint32_t transitionPhase = 0; // 原 0x9A6E94。
    std::wstring text; // 原 0x9A5B08。
    std::uint32_t requestPhase = 0; // 原 0x9A6204。
    std::int32_t rows = 0; // 原 0x9A5674。
    std::int32_t columns = 0; // 原 0x9A594C。
    std::int32_t top = 0; // 原 0x9A61F8。
    std::int32_t left = 0; // 原 0x9A5AC0。
    yobi::YobiAction* action = nullptr; // 原 0x9A6E98。
};

// 战斗场景的遍历游标与退出动作借用；动作本体由 Yobi 持有。
struct BattleSceneState {
    std::int32_t sceneIndex = 0; // 原 0x5ABFD8。
    yobi::YobiAction* exitAction = nullptr; // 原 0x5ABCDC。
};

// 世界场景的会话运行期数据；进程设备及只读资源不在此复制。
struct WorldSceneRuntimeState {
    DisconnectDialogState disconnectDialog {};
    LogoutSceneState logout {};
    BattleSceneState battle {};
    std::uint32_t pendingMapBgmIndex = 0; // 原 0xFF0D5C，地图回复生产、普通场景消费。
    std::uint32_t mapBgmChangePending = 0; // 原 0xFF0D58。
    std::int32_t worldBgmResumeIndex = 0; // 原 0xFF0D68，普通场景恢复号。
};

} // namespace cg::world
