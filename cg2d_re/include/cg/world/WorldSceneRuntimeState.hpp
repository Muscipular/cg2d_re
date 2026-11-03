#pragma once

#include <array>
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
    std::int32_t backgroundSnapshot = 0; // 原 0x5ABCD8，绘制后重读的背景图号。
    std::uint32_t exitNotificationPending = 0; // 原 0x5ABCB0。
    std::uint32_t entryPending = 0; // 原 0x5ABFC4，入战回复与世界场景共用。
};

// 100 项随机旋转表及跨场景消费游标，表初始化不重置游标。
struct RotationState {
    std::array<std::int32_t, 100> table {}; // 原 0xC8A7B0，长度 0x190。
    std::int32_t cursor = 0; // 原 0xC8A940。
};

// 聊天发送节流及回复生产的独立门禁，同一账号会话内保持原读写序。
struct ChatSendState {
    std::uint64_t previousTick = 0; // 原 0xDA3B3C。
    std::uint64_t currentTick = 0; // 原 0xDA3B40。
    std::uint8_t disabled = 0; // 原 0xDA3B4C。
    std::int32_t replyGate = 0; // 原 0xDA3B54。
};

// 64 条聊天输入历史、磁盘尾索引与当前浏览游标，运行期由会话持有。
struct ChatInputHistoryState {
    std::array<std::wstring, 64> records {}; // 原 0x621FF8。
    std::uint32_t tail626838 = 0; // 原 0x626838。
    std::int32_t cursor62683C = 0; // 原 0x62683C。
};

// 世界场景的会话运行期数据；进程设备及只读资源不在此复制。
struct WorldSceneRuntimeState {
    DisconnectDialogState disconnectDialog {};
    LogoutSceneState logout {};
    BattleSceneState battle {};
    RotationState rotation {};
    ChatSendState chat {};
    ChatInputHistoryState chatInputHistory {};
    std::int32_t frameBackgroundMode = 0; // 原 0x73ECC0，世界场景生产、主循环清屏消费。
    std::int32_t forceRender = 0; // 原 0x73ECE4，普通场景请求、主循环消费。
    std::uint32_t pendingMapBgmIndex = 0; // 原 0xFF0D5C，地图回复生产、普通场景消费。
    std::uint32_t mapBgmChangePending = 0; // 原 0xFF0D58。
    std::int32_t worldBgmResumeIndex = 0; // 原 0xFF0D68，普通场景恢复号。
    std::int32_t normalSceneInitialized = 0; // 原 0xFF0D50，普通场景初始化门禁。
    std::int32_t normalSceneWaitFrames = 0; // 原 0xFF0D54，普通场景等待帧数。
    std::uint32_t observationTravelTransitionCount = 0; // 原 0xFF0D74，观察转场次数。
    std::uint64_t lastAdjacentInteractionTick = 0; // 原 0xFF0D38，邻格交互节流。
    std::uint32_t movementPathFourth = 0; // 原 0x9EE8F8，地图回复生产、zA 发送消费。
    std::uint64_t lastCursorFacingRequestTick = 0; // 原 0xC82238，目标格转向节流。
    std::uint64_t lastExplicitFacingRequestTick = 0; // 原 0xC8223C，显式转向节流。
    std::array<char, 100> moveDirectionText {}; // 原 0x9E03D0，先写前缀，发送门禁后才写 NUL。
    std::uint64_t mapRectangleRequestStartTick = 0; // 原 0xAF10C8，缺块请求取时钟后发布。
};

} // namespace cg::world
