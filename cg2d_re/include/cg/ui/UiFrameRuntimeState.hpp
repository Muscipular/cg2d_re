#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cg::input { class InputContext; }

namespace cg::ui {

struct WindowExtension;

// 扩展窗口的活动注册、排序和焦点只在所属会话内有效。
struct ExtensionWindowState {
    std::uint64_t orderSequence = 0;
    std::int32_t focusedWindow = -1;
    std::int32_t dragOwner = -1;
    std::vector<std::shared_ptr<WindowExtension>> windows;
    std::unordered_map<int, std::uint64_t> windowOrder;
    std::vector<std::pair<int, std::uint32_t>> pendingWindowOpens;
};

// 扩展窗口、Lua 场景通知及输入编码诊断的会话帧状态；尚不拥有原生输入上下文。
struct UiFrameRuntimeState {
    ExtensionWindowState extension {};
    std::int32_t lastScene = -1;
    std::int32_t lastSceneState = -1;
    input::InputContext* reportedContext = nullptr;
    std::wstring reportedText;
    bool reportedFailure = false;
};

} // namespace cg::ui
