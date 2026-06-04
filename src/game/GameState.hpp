#pragma once

#include <cstdint>
#include <string>

namespace cg::game {

enum class GameStateType : std::uint8_t {
    Boot,
    Login,
    LoginPending,
    CharacterListPending,
    CharacterSelect,
    WorldLoading,
    World,
    Battle,
    Disconnected,
    Shutdown,
};

// GameState 只保存高层流程状态，不持有资源、网络或界面对象。
struct GameState {
    GameStateType type = GameStateType::Boot;
    bool mapReady = false;
    bool objectListReady = false;
    std::string lastMessage;
    std::string lastWarning;
};

} // namespace cg::game
