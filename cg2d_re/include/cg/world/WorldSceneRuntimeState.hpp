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

// 世界场景的会话运行期数据；进程设备及只读资源不在此复制。
struct WorldSceneRuntimeState {
    DisconnectDialogState disconnectDialog {};
};

} // namespace cg::world
