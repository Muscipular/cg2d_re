#pragma once

#include <cstdint>
#include <string>

namespace cg::scene {

// RootSceneId 表示互斥主流程场景，任一时刻只允许一个 Root Scene 激活。
enum class RootSceneId : std::uint8_t {
    Boot,
    Loading,
    Login,
    CharacterSelect,
    World,
    Battle,
    Disconnected,
    Shutdown,
};

// SubSceneId 表示 Root Scene 上方的局部流程或 UI 层。
enum class SubSceneId : std::uint8_t {
    MainHud,
    Chat,
    NpcDialog,
    Inventory,
    Trade,
    ConfirmDialog,
    LoadingOverlay,
};

// SubScenePolicy 控制 SubScene 与下层 scene 的输入、更新和渲染关系。
struct SubScenePolicy {
    bool modal = false;
    bool pausesBelow = false;
    bool receivesInput = true;
    bool transparentRender = true;
};

enum class SceneInputEventType : std::uint8_t {
    None,
    Quit,
    Confirm,
    Cancel,
    Text,
    Command,
};

// SceneInputEvent 是 scene 层输入边界；底层事件只在应用边界转换到这里。
struct SceneInputEvent {
    SceneInputEventType type = SceneInputEventType::None;
    std::string text;
};

} // namespace cg::scene
