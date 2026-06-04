#pragma once

#include "game/GameCommand.hpp"
#include "game/GameEvent.hpp"
#include "game/GameState.hpp"

#include <span>
#include <vector>

namespace cg::game {

// GameStateMachine 把高层事件转换为状态变更和无平台依赖的高层命令。
class GameStateMachine final {
public:
    [[nodiscard]] const GameState& State() const noexcept;
    [[nodiscard]] std::vector<GameCommand> HandleEvent(const GameEvent& event);

private:
    void ClearSubScenes(std::vector<GameCommand>& out) const;
    void ReplaceRoot(GameRootSceneType rootScene, std::vector<GameCommand>& out) const;
    void PushSub(GameSubSceneType subScene, std::vector<GameCommand>& out) const;
    void PopSub(GameSubSceneType subScene, std::vector<GameCommand>& out) const;
    void SendPacketIntent(GamePacketIntentType packetIntent, const std::string& payload, std::vector<GameCommand>& out) const;
    void Apply(GameCommandType type, const std::string& payload, std::vector<GameCommand>& out) const;
    void ShowMessage(std::string message, std::vector<GameCommand>& out);
    void Warn(std::string message, std::vector<GameCommand>& out);
    void ResetWorldLoadingFlags() noexcept;
    void EnterWorldIfReady(std::vector<GameCommand>& out);
    void HandleDisconnectLike(const GameEvent& event, std::vector<GameCommand>& out);
    void WarnInvalidTiming(std::vector<GameCommand>& out);

    GameState state_;
};

class GameReplayDriver final {
public:
    [[nodiscard]] std::vector<GameCommand> Replay(std::span<const GameEvent> events);
    [[nodiscard]] const GameState& State() const noexcept;
    [[nodiscard]] const std::vector<GameCommand>& Commands() const noexcept;

private:
    GameStateMachine machine_;
    std::vector<GameCommand> commands_;
};

} // namespace cg::game
