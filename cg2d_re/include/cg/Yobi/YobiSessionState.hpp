#pragma once

#include <cstdint>

namespace cg::yobi {

struct YobiAction;

// 每个会话持有其动作链；节点及载荷由 Yobi 生命周期函数分配和释放。
struct YobiSessionState {
    YobiAction* start = nullptr;
    YobiAction* end = nullptr;
    std::int32_t gateMode = 0; // 原 0x59E570，初值为 FullRate。
    std::int32_t phaseCounter = 0; // 原 0x59E574，UpdateAll 在 [0, 3] 间轮转。
};

} // namespace cg::yobi
