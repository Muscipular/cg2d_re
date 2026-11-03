#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::yobi {

struct YobiAction;

inline constexpr std::size_t kMapRegistryRows = 399;
inline constexpr std::size_t kMapRegistryColumns = 399;
inline constexpr std::size_t kMapRegistryEntries = kMapRegistryRows * kMapRegistryColumns;

// 每个会话持有其动作链；节点及载荷由 Yobi 生命周期函数分配和释放。
struct YobiSessionState {
    YobiAction* start = nullptr;
    YobiAction* end = nullptr;
    std::int32_t gateMode = 0; // 原 0x59E570，初值为 FullRate。
    std::int32_t phaseCounter = 0; // 原 0x59E574，UpdateAll 在 [0, 3] 间轮转。
    std::array<std::uint8_t, kMapRegistryEntries> mapRegistryFlags {}; // 原 0xB65A80。
    std::array<YobiAction*, kMapRegistryEntries> mapRegistryActions {}; // 原 0xB8C864，非拥有。
};

} // namespace cg::yobi
