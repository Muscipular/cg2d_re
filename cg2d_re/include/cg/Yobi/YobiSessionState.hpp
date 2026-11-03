#pragma once

namespace cg::yobi {

struct YobiAction;

// 每个会话持有其动作链；节点及载荷由 Yobi 生命周期函数分配和释放。
struct YobiSessionState {
    YobiAction* start = nullptr;
    YobiAction* end = nullptr;
};

} // namespace cg::yobi
