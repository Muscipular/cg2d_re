#include "game/GameStateMachine.hpp"

#include <utility>

namespace cg::game {
namespace {

bool IsWorldActive(GameStateType type) noexcept
{
    return type == GameStateType::World || type == GameStateType::Battle;
}

std::string TextOrFallback(const GameEvent& event, const char* fallback)
{
    if (!event.payload.empty()) {
        return event.payload;
    }

    return fallback;
}

} // namespace

const GameState& GameStateMachine::State() const noexcept
{
    return state_;
}

std::vector<GameCommand> GameStateMachine::HandleEvent(const GameEvent& event)
{
    std::vector<GameCommand> commands;

    if (event.type == GameEventType::QuitRequested) {
        state_.type = GameStateType::Shutdown;
        ResetWorldLoadingFlags();
        ClearSubScenes(commands);
        ReplaceRoot(GameRootSceneType::Shutdown, commands);
        return commands;
    }

    if (state_.type == GameStateType::Shutdown) {
        return commands;
    }

    if (event.type == GameEventType::Disconnected || event.type == GameEventType::ProtocolError) {
        HandleDisconnectLike(event, commands);
        return commands;
    }

    switch (event.type) {
    case GameEventType::AppStarted:
        if (state_.type != GameStateType::Boot) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.type = GameStateType::Login;
        ResetWorldLoadingFlags();
        ReplaceRoot(GameRootSceneType::Login, commands);
        break;
    case GameEventType::LoginSubmitted:
        if (state_.type != GameStateType::Login) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.type = GameStateType::LoginPending;
        SendPacketIntent(GamePacketIntentType::Login, event.payload, commands);
        break;
    case GameEventType::LoginAccepted:
        if (state_.type != GameStateType::LoginPending) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.type = GameStateType::CharacterListPending;
        SendPacketIntent(GamePacketIntentType::RequestCharacterList, event.payload, commands);
        break;
    case GameEventType::LoginRejected:
        if (state_.type != GameStateType::Login && state_.type != GameStateType::LoginPending) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.type = GameStateType::Login;
        ResetWorldLoadingFlags();
        ReplaceRoot(GameRootSceneType::Login, commands);
        ShowMessage(TextOrFallback(event, "登录失败"), commands);
        break;
    case GameEventType::CharacterListReceived:
        if (state_.type != GameStateType::CharacterListPending) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.type = GameStateType::CharacterSelect;
        ReplaceRoot(GameRootSceneType::CharacterSelect, commands);
        break;
    case GameEventType::CharacterListFailed:
        if (state_.type != GameStateType::CharacterListPending) {
            WarnInvalidTiming(commands);
            break;
        }
        Warn(TextOrFallback(event, "角色列表获取失败"), commands);
        break;
    case GameEventType::CharacterSelected:
        if (state_.type != GameStateType::CharacterSelect) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.type = GameStateType::WorldLoading;
        ResetWorldLoadingFlags();
        ReplaceRoot(GameRootSceneType::Loading, commands);
        SendPacketIntent(GamePacketIntentType::SelectCharacter, event.payload, commands);
        break;
    case GameEventType::CharacterEnterRejected:
        if (state_.type != GameStateType::WorldLoading) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.type = GameStateType::CharacterSelect;
        ResetWorldLoadingFlags();
        ReplaceRoot(GameRootSceneType::CharacterSelect, commands);
        ShowMessage(TextOrFallback(event, "进入角色失败"), commands);
        break;
    case GameEventType::MapReady:
        if (state_.type != GameStateType::WorldLoading) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.mapReady = true;
        Apply(GameCommandType::ApplyMapData, event.payload, commands);
        EnterWorldIfReady(commands);
        break;
    case GameEventType::ObjectListReceived:
        if (state_.type != GameStateType::WorldLoading) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.objectListReady = true;
        Apply(GameCommandType::ApplyObjectList, event.payload, commands);
        EnterWorldIfReady(commands);
        break;
    case GameEventType::WorldUpdateReceived:
        if (!IsWorldActive(state_.type)) {
            WarnInvalidTiming(commands);
            break;
        }
        Apply(GameCommandType::ApplyWorldUpdate, event.payload, commands);
        break;
    case GameEventType::ChatMessageReceived:
        if (!IsWorldActive(state_.type)) {
            WarnInvalidTiming(commands);
            break;
        }
        Apply(GameCommandType::AppendChatMessage, event.payload, commands);
        break;
    case GameEventType::OverlayOpened:
        if (!IsWorldActive(state_.type)) {
            WarnInvalidTiming(commands);
            break;
        }
        PushSub(GameSubSceneType::Overlay, commands);
        break;
    case GameEventType::OverlayClosed:
        if (!IsWorldActive(state_.type)) {
            WarnInvalidTiming(commands);
            break;
        }
        PopSub(GameSubSceneType::Overlay, commands);
        break;
    case GameEventType::BattleStarted:
        if (state_.type != GameStateType::World) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.type = GameStateType::Battle;
        ClearSubScenes(commands);
        ReplaceRoot(GameRootSceneType::Battle, commands);
        break;
    case GameEventType::BattleEnded:
        if (state_.type != GameStateType::Battle) {
            WarnInvalidTiming(commands);
            break;
        }
        state_.type = GameStateType::World;
        ReplaceRoot(GameRootSceneType::World, commands);
        PushSub(GameSubSceneType::MainHud, commands);
        break;
    case GameEventType::Disconnected:
    case GameEventType::ProtocolError:
    case GameEventType::QuitRequested:
        break;
    }

    return commands;
}

void GameStateMachine::ClearSubScenes(std::vector<GameCommand>& out) const
{
    GameCommand command;
    command.type = GameCommandType::ClearSubScenes;
    out.push_back(std::move(command));
}

void GameStateMachine::ReplaceRoot(GameRootSceneType rootScene, std::vector<GameCommand>& out) const
{
    GameCommand command;
    command.type = GameCommandType::ReplaceRootScene;
    command.rootScene = rootScene;
    out.push_back(std::move(command));
}

void GameStateMachine::PushSub(GameSubSceneType subScene, std::vector<GameCommand>& out) const
{
    GameCommand command;
    command.type = GameCommandType::PushSubScene;
    command.subScene = subScene;
    out.push_back(std::move(command));
}

void GameStateMachine::PopSub(GameSubSceneType subScene, std::vector<GameCommand>& out) const
{
    GameCommand command;
    command.type = GameCommandType::PopSubScene;
    command.subScene = subScene;
    out.push_back(std::move(command));
}

void GameStateMachine::SendPacketIntent(
    GamePacketIntentType packetIntent,
    const std::string& payload,
    std::vector<GameCommand>& out) const
{
    GameCommand command;
    command.type = GameCommandType::SendPacketIntent;
    command.packetIntent = packetIntent;
    command.payload = payload;
    out.push_back(std::move(command));
}

void GameStateMachine::Apply(GameCommandType type, const std::string& payload, std::vector<GameCommand>& out) const
{
    GameCommand command;
    command.type = type;
    command.payload = payload;
    out.push_back(std::move(command));
}

void GameStateMachine::ShowMessage(std::string message, std::vector<GameCommand>& out)
{
    state_.lastMessage = std::move(message);
    GameCommand command;
    command.type = GameCommandType::ShowMessage;
    command.payload = state_.lastMessage;
    out.push_back(std::move(command));
}

void GameStateMachine::Warn(std::string message, std::vector<GameCommand>& out)
{
    state_.lastWarning = std::move(message);
    GameCommand command;
    command.type = GameCommandType::LogWarning;
    command.payload = state_.lastWarning;
    out.push_back(std::move(command));
}

void GameStateMachine::ResetWorldLoadingFlags() noexcept
{
    state_.mapReady = false;
    state_.objectListReady = false;
}

void GameStateMachine::EnterWorldIfReady(std::vector<GameCommand>& out)
{
    if (!state_.mapReady || !state_.objectListReady) {
        return;
    }

    state_.type = GameStateType::World;
    ReplaceRoot(GameRootSceneType::World, out);
    PushSub(GameSubSceneType::MainHud, out);
}

void GameStateMachine::HandleDisconnectLike(const GameEvent& event, std::vector<GameCommand>& out)
{
    state_.type = GameStateType::Disconnected;
    ResetWorldLoadingFlags();
    ClearSubScenes(out);
    ReplaceRoot(GameRootSceneType::Disconnected, out);

    if (event.type == GameEventType::ProtocolError) {
        Warn(TextOrFallback(event, "协议错误"), out);
        return;
    }

    Warn(TextOrFallback(event, "连接已断开"), out);
}

void GameStateMachine::WarnInvalidTiming(std::vector<GameCommand>& out)
{
    Warn("非法游戏事件时序", out);
}

std::vector<GameCommand> GameReplayDriver::Replay(std::span<const GameEvent> events)
{
    for (const GameEvent& event : events) {
        std::vector<GameCommand> eventCommands = machine_.HandleEvent(event);
        commands_.insert(commands_.end(), eventCommands.begin(), eventCommands.end());
    }
    return commands_;
}

const GameState& GameReplayDriver::State() const noexcept
{
    return machine_.State();
}

const std::vector<GameCommand>& GameReplayDriver::Commands() const noexcept
{
    return commands_;
}

} // namespace cg::game
