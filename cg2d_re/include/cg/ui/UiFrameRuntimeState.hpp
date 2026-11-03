#pragma once

#include <cstdint>
#include <string>

namespace cg::input { class InputContext; }

namespace cg::ui {

// Lua 场景通知与输入编码错误诊断的会话帧状态，不拥有输入上下文。
struct UiFrameRuntimeState {
    std::int32_t lastScene = -1;
    std::int32_t lastSceneState = -1;
    input::InputContext* reportedContext = nullptr;
    std::wstring reportedText;
    bool reportedFailure = false;
};

} // namespace cg::ui
