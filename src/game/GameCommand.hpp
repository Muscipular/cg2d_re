#pragma once

#include <cstdint>
#include <string>

namespace cg::game {

enum class GameRootSceneType : std::uint8_t {
    None,
    Boot,
    Login,
    CharacterSelect,
    Loading,
    World,
    Battle,
    Disconnected,
    Shutdown,
};

enum class GameSubSceneType : std::uint8_t {
    None,
    MainHud,
    Overlay,
};

enum class GameCommandType : std::uint8_t {
    ReplaceRootScene,
    // SubScene 命令名保留兼容；SceneManager/M8 bridge 语义是 replace/clear 单当前 SubScene。
    PushSubScene,
    PopSubScene,
    ClearSubScenes,
    SendPacketIntent,
    LoadMap,
    ApplyMapData,
    ApplyObjectList,
    ApplyWorldUpdate,
    AppendChatMessage,
    ShowMessage,
    LogWarning,
};

enum class GamePacketIntentType : std::uint8_t {
    None,
    Login,
    RequestCharacterList,
    SelectCharacter,
    SendChat,
    WindowReply,
    BattleCommand,
};

struct GameCommand {
    GameCommandType type = GameCommandType::LogWarning;
    GameRootSceneType rootScene = GameRootSceneType::None;
    GameSubSceneType subScene = GameSubSceneType::None;
    GamePacketIntentType packetIntent = GamePacketIntentType::None;
    std::string payload;
};

} // namespace cg::game
