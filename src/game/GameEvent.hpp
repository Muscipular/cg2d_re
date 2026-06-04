#pragma once

#include <cstdint>
#include <string>

namespace cg::game {

// GameEventType 表示协议、UI 或 replay 注入到游戏状态机的高层事件。
enum class GameEventType : std::uint8_t {
    AppStarted,
    LoginSubmitted,
    LoginAccepted,
    LoginRejected,
    CharacterListReceived,
    CharacterListFailed,
    CharacterSelected,
    CharacterEnterRejected,
    MapReady,
    ObjectListReceived,
    WorldUpdateReceived,
    ChatMessageReceived,
    OverlayOpened,
    OverlayClosed,
    BattleStarted,
    BattleEnded,
    Disconnected,
    ProtocolError,
    QuitRequested,
};

struct GameEvent {
    GameEventType type = GameEventType::AppStarted;
    std::string payload;
    int value = 0;
};

} // namespace cg::game
