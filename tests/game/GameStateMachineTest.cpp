#include "game/GameStateMachine.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>

#ifndef CG_GAME_TEST_SOURCE_DIR
#define CG_GAME_TEST_SOURCE_DIR "."
#endif

namespace {

using cg::game::GameCommand;
using cg::game::GameCommandType;
using cg::game::GameEvent;
using cg::game::GameEventType;
using cg::game::GamePacketIntentType;
using cg::game::GameReplayDriver;
using cg::game::GameRootSceneType;
using cg::game::GameState;
using cg::game::GameStateMachine;
using cg::game::GameStateType;
using cg::game::GameSubSceneType;

static_assert(std::is_enum_v<GameStateType>);
static_assert(std::is_enum_v<GameEventType>);
static_assert(std::is_enum_v<GameCommandType>);
static_assert(std::is_enum_v<GamePacketIntentType>);
static_assert(std::is_default_constructible_v<GameState>);
static_assert(std::is_move_constructible_v<GameState>);
static_assert(!std::is_polymorphic_v<GameStateMachine>);
static_assert(!std::is_polymorphic_v<GameReplayDriver>);

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

std::string Join(const char* left, const char* right)
{
    std::string text(left);
    text += right;
    return text;
}

bool ReadFile(const std::filesystem::path& path, std::string& out)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        return false;
    }

    out.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    return true;
}

int TestPublicHeaderSurface()
{
    int failures = 0;
    const std::filesystem::path root(CG_GAME_TEST_SOURCE_DIR);
    const std::array<std::filesystem::path, 4> headers{{
        root / "src/game/GameState.hpp",
        root / "src/game/GameEvent.hpp",
        root / "src/game/GameCommand.hpp",
        root / "src/game/GameStateMachine.hpp",
    }};
    const std::array<std::string, 6> forbidden{{
        Join("S", "DL"),
        Join("T", "TF"),
        Join("Win", "32"),
        Join("platform", "/Event"),
        Join("dynamic", "_cast"),
        Join("type", "id"),
    }};

    for (const std::filesystem::path& header : headers) {
        std::string text;
        failures += Expect(ReadFile(header, text), "公开头可读取");
        for (const std::string& token : forbidden) {
            failures += Expect(text.find(token) == std::string::npos, "公开头不暴露禁止边界");
        }
    }

    return failures;
}

std::vector<GameCommand> DriveToWorld(GameStateMachine& machine)
{
    std::vector<GameCommand> commands;
    const std::array<GameEvent, 7> events{{
        {GameEventType::AppStarted, {}, 0},
        {GameEventType::LoginSubmitted, "login", 0},
        {GameEventType::LoginAccepted, {}, 0},
        {GameEventType::CharacterListReceived, {}, 0},
        {GameEventType::CharacterSelected, "char:1", 0},
        {GameEventType::MapReady, "map:1000", 0},
        {GameEventType::ObjectListReceived, "objects", 0},
    }};

    for (const GameEvent& event : events) {
        std::vector<GameCommand> eventCommands = machine.HandleEvent(event);
        commands.insert(commands.end(), eventCommands.begin(), eventCommands.end());
    }

    return commands;
}

int ExpectCommandType(const std::vector<GameCommand>& commands, std::size_t index, GameCommandType type, const char* message)
{
    return Expect(index < commands.size() && commands[index].type == type, message);
}

int TestFullReplayFlow()
{
    int failures = 0;
    GameReplayDriver replay;
    const std::array<GameEvent, 9> events{{
        {GameEventType::AppStarted, {}, 0},
        {GameEventType::LoginSubmitted, "login", 0},
        {GameEventType::LoginAccepted, {}, 0},
        {GameEventType::CharacterListReceived, {}, 0},
        {GameEventType::CharacterSelected, "char:1", 0},
        {GameEventType::MapReady, "map:1000", 0},
        {GameEventType::ObjectListReceived, "objects", 0},
        {GameEventType::BattleStarted, {}, 0},
        {GameEventType::BattleEnded, {}, 0},
    }};

    const std::vector<GameCommand> commands = replay.Replay(events);
    failures += Expect(replay.State().type == GameStateType::World, "完整 replay 最终回到 World");
    failures += Expect(replay.Commands().size() == commands.size(), "replay 累计命令可读取");
    failures += Expect(commands.size() == 14U, "完整 replay 命令数量稳定");

    failures += ExpectCommandType(commands, 0U, GameCommandType::ReplaceRootScene, "Boot 后切 Login");
    failures += Expect(commands[0].rootScene == GameRootSceneType::Login, "RootScene Login");
    failures += ExpectCommandType(commands, 1U, GameCommandType::SendPacketIntent, "登录提交发送意图");
    failures += Expect(commands[1].packetIntent == GamePacketIntentType::Login, "登录包意图");
    failures += ExpectCommandType(commands, 2U, GameCommandType::SendPacketIntent, "登录接受后请求角色列表");
    failures += Expect(commands[2].packetIntent == GamePacketIntentType::RequestCharacterList, "请求角色列表包意图");
    failures += ExpectCommandType(commands, 3U, GameCommandType::ReplaceRootScene, "收到角色列表切角色选择");
    failures += Expect(commands[3].rootScene == GameRootSceneType::CharacterSelect, "RootScene CharacterSelect");
    failures += ExpectCommandType(commands, 4U, GameCommandType::ReplaceRootScene, "选择角色切 Loading");
    failures += Expect(commands[4].rootScene == GameRootSceneType::Loading, "RootScene Loading");
    failures += Expect(commands[5].packetIntent == GamePacketIntentType::SelectCharacter, "选择角色包意图");
    failures += ExpectCommandType(commands, 6U, GameCommandType::ApplyMapData, "应用地图数据");
    failures += ExpectCommandType(commands, 7U, GameCommandType::ApplyObjectList, "应用对象列表");
    failures += Expect(commands[8].rootScene == GameRootSceneType::World, "地图和对象齐备后切 World");
    failures += Expect(commands[9].subScene == GameSubSceneType::MainHud, "进入 World 后压入 MainHud");
    failures += ExpectCommandType(commands, 10U, GameCommandType::ClearSubScenes, "进战斗前清空子场景");
    failures += Expect(commands[11].rootScene == GameRootSceneType::Battle, "切 Battle root");
    failures += Expect(commands[12].rootScene == GameRootSceneType::World, "战斗结束切回 World");
    failures += Expect(commands[13].subScene == GameSubSceneType::MainHud, "战斗结束恢复 MainHud");
    return failures;
}

int TestWorldLoadingOrder()
{
    int failures = 0;

    {
        GameStateMachine machine;
        DriveToWorld(machine);
        failures += Expect(machine.State().type == GameStateType::World, "MapReady 后 ObjectListReceived 可进入 World");
    }

    {
        GameStateMachine machine;
        const std::array<GameEvent, 5> setup{{
            {GameEventType::AppStarted, {}, 0},
            {GameEventType::LoginSubmitted, "login", 0},
            {GameEventType::LoginAccepted, {}, 0},
            {GameEventType::CharacterListReceived, {}, 0},
            {GameEventType::CharacterSelected, "char:1", 0},
        }};

        for (const GameEvent& event : setup) {
            (void)machine.HandleEvent(event);
        }

        std::vector<GameCommand> commands = machine.HandleEvent({GameEventType::ObjectListReceived, "objects", 0});
        failures += Expect(machine.State().type == GameStateType::WorldLoading, "ObjectList 先到时仍在 Loading");
        failures += Expect(machine.State().objectListReady, "ObjectList 先到时记录 ready");
        failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::ApplyObjectList, "ObjectList 先到只应用对象");

        commands = machine.HandleEvent({GameEventType::MapReady, "map:1000", 0});
        failures += Expect(machine.State().type == GameStateType::World, "ObjectList 后 MapReady 可进入 World");
        failures += Expect(commands.size() == 3U, "第二个加载事件附加入场命令");
        failures += Expect(commands[0].type == GameCommandType::ApplyMapData, "第二步应用地图");
        failures += Expect(commands[1].rootScene == GameRootSceneType::World, "第二步切 World");
        failures += Expect(commands[2].subScene == GameSubSceneType::MainHud, "第二步压入 MainHud");
    }

    return failures;
}

int TestLoginRejected()
{
    int failures = 0;
    GameStateMachine machine;
    (void)machine.HandleEvent({GameEventType::AppStarted, {}, 0});
    (void)machine.HandleEvent({GameEventType::LoginSubmitted, "login", 0});

    const std::vector<GameCommand> commands = machine.HandleEvent({GameEventType::LoginRejected, "bad password", 0});
    failures += Expect(machine.State().type == GameStateType::Login, "LoginRejected 回到 Login");
    failures += Expect(commands.size() == 2U, "LoginRejected 产生命令");
    failures += Expect(commands[0].type == GameCommandType::ReplaceRootScene, "LoginRejected 切 Login scene");
    failures += Expect(commands[0].rootScene == GameRootSceneType::Login, "LoginRejected root scene");
    failures += Expect(commands[1].type == GameCommandType::ShowMessage, "LoginRejected 显示消息");
    failures += Expect(commands[1].payload == "bad password", "LoginRejected 保留消息");
    return failures;
}

int ExpectDisconnected(GameStateMachine& machine, const char* message)
{
    const std::vector<GameCommand> commands = machine.HandleEvent({GameEventType::Disconnected, "lost", 0});
    int failures = 0;
    failures += Expect(machine.State().type == GameStateType::Disconnected, message);
    failures += Expect(commands.size() == 3U, "断线命令数量");
    failures += Expect(commands[0].type == GameCommandType::ClearSubScenes, "断线清空子场景");
    failures += Expect(commands[1].rootScene == GameRootSceneType::Disconnected, "断线切 Disconnected");
    failures += Expect(commands[2].type == GameCommandType::LogWarning, "断线记录 warning");
    failures += Expect(commands[2].payload == "lost", "断线 warning 使用事件文本");
    return failures;
}

int TestDisconnectedFromStates()
{
    int failures = 0;

    {
        GameStateMachine machine;
        failures += ExpectDisconnected(machine, "Boot 可断线");
    }

    {
        GameStateMachine machine;
        (void)machine.HandleEvent({GameEventType::AppStarted, {}, 0});
        (void)machine.HandleEvent({GameEventType::LoginSubmitted, "login", 0});
        failures += ExpectDisconnected(machine, "LoginPending 可断线");
    }

    {
        GameStateMachine machine;
        DriveToWorld(machine);
        failures += ExpectDisconnected(machine, "World 可断线");
    }

    {
        GameStateMachine machine;
        DriveToWorld(machine);
        (void)machine.HandleEvent({GameEventType::BattleStarted, {}, 0});
        failures += ExpectDisconnected(machine, "Battle 可断线");
    }

    {
        GameStateMachine machine;
        (void)machine.HandleEvent({GameEventType::QuitRequested, {}, 0});
        const std::vector<GameCommand> commands = machine.HandleEvent({GameEventType::Disconnected, "lost", 0});
        failures += Expect(machine.State().type == GameStateType::Shutdown, "Shutdown 后断线不改状态");
        failures += Expect(commands.empty(), "Shutdown 后断线不产生命令");
    }

    return failures;
}

int TestWorldAndBattleMessages()
{
    int failures = 0;
    GameStateMachine machine;
    DriveToWorld(machine);

    std::vector<GameCommand> commands = machine.HandleEvent({GameEventType::WorldUpdateReceived, "move", 0});
    failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::ApplyWorldUpdate, "World 可应用更新");

    commands = machine.HandleEvent({GameEventType::ChatMessageReceived, "hi", 0});
    failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::AppendChatMessage, "World 可追加聊天");

    (void)machine.HandleEvent({GameEventType::BattleStarted, {}, 0});
    commands = machine.HandleEvent({GameEventType::WorldUpdateReceived, "battle move", 0});
    failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::ApplyWorldUpdate, "Battle 可应用更新");

    commands = machine.HandleEvent({GameEventType::ChatMessageReceived, "battle chat", 0});
    failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::AppendChatMessage, "Battle 可追加聊天");
    return failures;
}

int TestUiSubmittedPacketIntents()
{
    int failures = 0;
    GameStateMachine machine;
    DriveToWorld(machine);

    std::vector<GameCommand> commands = machine.HandleEvent({GameEventType::ChatSubmitted, "hi", 0});
    failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::SendPacketIntent, "World 聊天提交发送包意图");
    failures += Expect(commands[0].packetIntent == GamePacketIntentType::SendChat && commands[0].payload == "hi", "聊天提交使用 SendChat");

    (void)machine.HandleEvent({GameEventType::BattleStarted, {}, 0});
    commands = machine.HandleEvent({GameEventType::BattleCommandSubmitted, "Attack", 0});
    failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::SendPacketIntent, "Battle 命令提交发送包意图");
    failures += Expect(commands[0].packetIntent == GamePacketIntentType::BattleCommand && commands[0].payload == "Attack", "战斗命令提交使用 BattleCommand");
    return failures;
}

int TestInvalidTimingWarns()
{
    int failures = 0;
    GameStateMachine machine;

    std::vector<GameCommand> commands = machine.HandleEvent({GameEventType::WorldUpdateReceived, "move", 0});
    failures += Expect(machine.State().type == GameStateType::Boot, "非法 WorldUpdate 不改状态");
    failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::LogWarning, "非法 WorldUpdate 只 warning");

    (void)machine.HandleEvent({GameEventType::AppStarted, {}, 0});
    commands = machine.HandleEvent({GameEventType::BattleEnded, {}, 0});
    failures += Expect(machine.State().type == GameStateType::Login, "非法 BattleEnded 不改状态");
    failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::LogWarning, "非法 BattleEnded warning");

    commands = machine.HandleEvent({GameEventType::ChatMessageReceived, "hi", 0});
    failures += Expect(machine.State().type == GameStateType::Login, "非法 ChatMessage 不改状态");
    failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::LogWarning, "非法 ChatMessage warning");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestPublicHeaderSurface();
    failures += TestFullReplayFlow();
    failures += TestWorldLoadingOrder();
    failures += TestLoginRejected();
    failures += TestDisconnectedFromStates();
    failures += TestWorldAndBattleMessages();
    failures += TestUiSubmittedPacketIntents();
    failures += TestInvalidTimingWarns();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
