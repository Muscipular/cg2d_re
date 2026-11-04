#pragma once

#include "cg/ui/NativeWindowRuntimeState.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/ImeRuntimeState.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cg::input { class InputContext; }
namespace cg::session { class ClientSession; }

namespace cg::ui {

struct WindowExtension;

// 控件跨回调身份；实例代次和创建序号防止同 ID 新对象冒充旧对象。
struct WindowElementRef {
    int winId = -1;
    std::uint32_t generation = 0;
    std::uint32_t elementId = 0;
    std::uint64_t creationSerial = 0;
    session::ClientSession* ownerSession = nullptr;
};

// 控件拖动捕获只保存身份，窗口/控件的实际寿命仍由 UI 实例管理。
struct ElementPointerDragState {
    int windowId = -1;
    std::uint32_t generation = 0;
    std::uint32_t elementId = 0;
    std::uint64_t creationSerial = 0;
    int lastX = 0;
    int lastY = 0;
    bool moved = false;
    bool startedThisFrame = false;
};

// 扩展窗口的活动注册、排序和焦点只在所属会话内有效。
struct ExtensionWindowState {
    std::uint64_t orderSequence = 0;
    std::int32_t focusedWindow = -1;
    std::int32_t dragOwner = -1;
    std::vector<std::shared_ptr<WindowExtension>> windows;
    std::unordered_map<int, std::uint64_t> windowOrder;
    std::vector<std::pair<int, std::uint32_t>> pendingWindowOpens;
};

// 原生/扩展窗口、输入选择、Lua 场景通知及输入编码诊断的会话帧状态。
struct UiFrameRuntimeState {
    platform::MouseRuntimeState mouse {};
    int mouseWheelDelta = 0; // 扩展UI/Lua共用的本会话滚轮增量，与原鼠标尾字段独立。
    std::uint8_t keyStates[platform::LegacyKeyStateCount] {}; // 原73EFF8完整U8键态数组。
    platform::ImeRuntimeState ime {};
    NativeWindowRuntimeState native {};
    input::InputRuntimeState inputState {};
    std::uint32_t windowHasFocus = 0; // 原0xCA0AB4，元素派发成功后占用，独立于焦点窗口ID。
    std::int32_t nativeWindowCount = 0; // 原0xCC4844，原生帧发布的窗口数量。
    std::wstring hoverText; // 原0xD68AE8，同会话各提示来源共用的正文。
    ExtensionWindowState extension {};
    ElementPointerDragState pointerDrag {};
    bool pointerDragReserved = false; // 含松开/取消的当前帧，防止事件穿透。
    WindowElementRef focusedInput {};
    std::int32_t lastScene = -1;
    std::int32_t lastSceneState = -1;
    input::InputContext* reportedContext = nullptr;
    std::wstring reportedText;
    bool reportedFailure = false;
};

} // namespace cg::ui
