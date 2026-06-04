#include "app/SceneClientLoopBridge.hpp"
#include "render/RenderQueue.hpp"
#include "ui/SdlTextUiPresenter.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#ifndef CG_APP_TEST_SOURCE_DIR
#define CG_APP_TEST_SOURCE_DIR "."
#endif

namespace {

using cg::app::DriveGameEvent;
using cg::app::DriveUiInput;
using cg::app::ToGameEvent;
using cg::app::ToSceneInput;
using cg::game::GameCommand;
using cg::game::GameCommandType;
using cg::game::GameEventType;
using cg::game::GamePacketIntentType;
using cg::game::GameRootSceneType;
using cg::game::GameStateMachine;
using cg::game::GameStateType;
using cg::game::GameSubSceneType;
using cg::scene::IScene;
using cg::scene::ISubScene;
using cg::scene::RootSceneId;
using cg::scene::SceneInputEvent;
using cg::scene::SceneInputEventType;
using cg::scene::SceneManager;
using cg::scene::SubSceneId;
using cg::scene::SubScenePolicy;
using cg::ui::SdlTextUiPresenter;
using cg::ui::UiInputEventType;
using cg::ui::UiIntent;
using cg::ui::UiIntentType;
using cg::ui::UiKey;
using cg::ui::UiScreenType;
using cg::ui::UiViewModel;

class TestScene final : public IScene {
public:
    explicit TestScene(RootSceneId id) noexcept
        : id_(id)
    {
    }

    [[nodiscard]] RootSceneId Id() const noexcept override
    {
        return id_;
    }

    void Update(float) override
    {
    }

    bool HandleInput(const SceneInputEvent&) override
    {
        return false;
    }

    void Render(cg::render::RenderQueue&) const override
    {
    }

private:
    RootSceneId id_;
};

class TestSubScene final : public ISubScene {
public:
    TestSubScene(SubSceneId id, SubScenePolicy policy) noexcept
        : id_(id)
        , policy_(policy)
    {
    }

    [[nodiscard]] SubSceneId Id() const noexcept override
    {
        return id_;
    }

    [[nodiscard]] SubScenePolicy Policy() const noexcept override
    {
        return policy_;
    }

    void Update(float) override
    {
    }

    bool HandleInput(const SceneInputEvent&) override
    {
        return false;
    }

    void Render(cg::render::RenderQueue&) const override
    {
    }

private:
    SubSceneId id_;
    SubScenePolicy policy_;
};

class ModalSubScene final : public ISubScene {
public:
    [[nodiscard]] SubSceneId Id() const noexcept override
    {
        return SubSceneId::ConfirmDialog;
    }

    [[nodiscard]] SubScenePolicy Policy() const noexcept override
    {
        SubScenePolicy policy;
        policy.modal = true;
        return policy;
    }

    void Update(float) override
    {
    }

    bool HandleInput(const SceneInputEvent&) override
    {
        return false;
    }

    void Render(cg::render::RenderQueue&) const override
    {
    }
};

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

void RegisterRoots(SceneManager& manager)
{
    manager.RegisterRootSceneFactory(RootSceneId::Boot, []() {
        return std::make_unique<TestScene>(RootSceneId::Boot);
    });
    manager.RegisterRootSceneFactory(RootSceneId::Login, []() {
        return std::make_unique<TestScene>(RootSceneId::Login);
    });
    manager.RegisterRootSceneFactory(RootSceneId::CharacterSelect, []() {
        return std::make_unique<TestScene>(RootSceneId::CharacterSelect);
    });
    manager.RegisterRootSceneFactory(RootSceneId::Loading, []() {
        return std::make_unique<TestScene>(RootSceneId::Loading);
    });
    manager.RegisterRootSceneFactory(RootSceneId::World, []() {
        return std::make_unique<TestScene>(RootSceneId::World);
    });
}

void ReplaceTestSubScene(SceneManager& manager, GameSubSceneType subScene)
{
    SubScenePolicy policy;
    policy.transparentRender = true;
    if (subScene == GameSubSceneType::MainHud) {
        manager.ReplaceCurrentSubScene(std::make_unique<TestSubScene>(SubSceneId::MainHud, policy));
    } else if (subScene == GameSubSceneType::Overlay) {
        manager.ReplaceCurrentSubScene(std::make_unique<TestSubScene>(SubSceneId::LoadingOverlay, policy));
    }
}

int TestPublicHeaderSurface()
{
    int failures = 0;
    const std::filesystem::path root(CG_APP_TEST_SOURCE_DIR);
    const std::array<std::filesystem::path, 1> headers{{root / "src/app/SceneClientLoopBridge.hpp"}};
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
        failures += Expect(ReadFile(header, text), "app bridge 公开头可读取");
        for (const std::string& token : forbidden) {
            failures += Expect(text.find(token) == std::string::npos, "app bridge 公开头无禁止边界");
        }
    }

    return failures;
}

int TestIntentMapping()
{
    int failures = 0;
    const UiIntent chat{UiIntentType::SubmitChat, "hi", 0U};
    const auto chatEvent = ToGameEvent(chat);
    failures += Expect(chatEvent.has_value() && chatEvent->type == GameEventType::ChatSubmitted, "SubmitChat 映射 ChatSubmitted");

    const UiIntent battle{UiIntentType::SelectMenuItem, "Attack", 1U};
    const auto battleEvent = ToGameEvent(battle);
    failures += Expect(battleEvent.has_value() && battleEvent->type == GameEventType::BattleCommandSubmitted, "SelectMenuItem 映射 BattleCommandSubmitted");

    const SceneInputEvent confirm = ToSceneInput({UiInputEventType::Key, UiKey::Enter, {}});
    failures += Expect(confirm.type == SceneInputEventType::Confirm, "Enter 映射 scene Confirm");

    const SceneInputEvent text = ToSceneInput({UiInputEventType::TextInput, UiKey::None, "你"});
    failures += Expect(text.type == SceneInputEventType::Text && text.text == "你", "TextInput 映射 scene Text");
    return failures;
}

int TestDriveGameEventAppliesSceneAndPresenter()
{
    int failures = 0;
    GameStateMachine machine;
    SceneManager manager;
    SdlTextUiPresenter presenter;
    RegisterRoots(manager);
    (void)manager.Start(RootSceneId::Boot);

    UiViewModel model;
    model.screen = UiScreenType::Login;
    model.title = "Scene";
    presenter.SetViewModel(model);

    const std::vector<GameCommand> commands = DriveGameEvent(
        machine,
        manager,
        presenter,
        {GameEventType::AppStarted, {}, 0},
        ReplaceTestSubScene);

    failures += Expect(commands.size() == 1U && commands[0].rootScene == GameRootSceneType::Login, "AppStarted 产生命令");
    failures += Expect(manager.CurrentRootSceneId() == RootSceneId::Login, "AppStarted 应用到 SceneManager");
    failures += Expect(presenter.ViewModel().screen == UiScreenType::Login, "AppStarted 同步 Presenter ViewModel");
    return failures;
}

int TestDriveUiInputToCommand()
{
    int failures = 0;
    GameStateMachine machine;
    SceneManager manager;
    SdlTextUiPresenter presenter;
    RegisterRoots(manager);
    (void)manager.Start(RootSceneId::Boot);

    UiViewModel model;
    model.screen = UiScreenType::Login;
    model.title = "Scene";
    presenter.SetViewModel(model);
    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::AppStarted, {}, 0}, ReplaceTestSubScene);

    std::vector<GameCommand> commands = DriveUiInput(
        machine,
        manager,
        presenter,
        {UiInputEventType::TextInput, UiKey::None, "demo"},
        ReplaceTestSubScene);
    failures += Expect(commands.empty() && presenter.ViewModel().inputText == "demo", "TextInput 更新 Presenter 但不进状态机");

    commands = DriveUiInput(
        machine,
        manager,
        presenter,
        {UiInputEventType::Key, UiKey::Enter, {}},
        ReplaceTestSubScene);
    failures += Expect(machine.State().type == GameStateType::LoginPending, "Enter 通过 UI intent 驱动状态机");
    failures += Expect(commands.size() == 1U && commands[0].type == GameCommandType::SendPacketIntent, "Enter 产生命令");
    failures += Expect(commands[0].packetIntent == GamePacketIntentType::Login && commands[0].payload == "demo", "Login payload 来自 Presenter");
    return failures;
}

int TestOverlayReplaceAndCloseClearsCurrent()
{
    int failures = 0;
    GameStateMachine machine;
    SceneManager manager;
    SdlTextUiPresenter presenter;
    RegisterRoots(manager);
    (void)manager.Start(RootSceneId::Boot);

    UiViewModel model;
    model.screen = UiScreenType::Login;
    presenter.SetViewModel(model);

    const std::array<GameEventType, 7> enterWorld{{
        GameEventType::AppStarted,
        GameEventType::LoginSubmitted,
        GameEventType::LoginAccepted,
        GameEventType::CharacterListReceived,
        GameEventType::CharacterSelected,
        GameEventType::MapReady,
        GameEventType::ObjectListReceived,
    }};
    for (const GameEventType eventType : enterWorld) {
        (void)DriveGameEvent(machine, manager, presenter, {eventType, "payload", 0}, ReplaceTestSubScene);
    }
    failures += Expect(manager.SubSceneCount() == 1U, "进入 World 后当前 SubScene 为 MainHud");

    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::OverlayClosed, {}, 0}, ReplaceTestSubScene);
    failures += Expect(manager.SubSceneCount() == 1U, "未打开 Overlay 时关闭 Overlay 不清 MainHud");

    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::OverlayOpened, {}, 0}, ReplaceTestSubScene);
    failures += Expect(manager.SubSceneCount() == 1U, "OverlayOpened replace MainHud 为当前 Overlay");

    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::OverlayClosed, {}, 0}, ReplaceTestSubScene);
    failures += Expect(manager.SubSceneCount() == 0U, "OverlayClosed 清当前 Overlay 且不恢复 MainHud");
    return failures;
}

int TestScreenChangeRebuildsMenu()
{
    int failures = 0;
    GameStateMachine machine;
    SceneManager manager;
    SdlTextUiPresenter presenter;
    RegisterRoots(manager);
    (void)manager.Start(RootSceneId::Boot);

    UiViewModel model;
    model.screen = UiScreenType::Login;
    presenter.SetViewModel(model);

    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::AppStarted, {}, 0}, ReplaceTestSubScene);
    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::LoginSubmitted, "login", 0}, ReplaceTestSubScene);
    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::LoginAccepted, {}, 0}, ReplaceTestSubScene);
    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::CharacterListReceived, {}, 0}, ReplaceTestSubScene);

    failures += Expect(presenter.ViewModel().screen == UiScreenType::CharacterSelect, "进入角色选择 screen");
    failures += Expect(presenter.ViewModel().menuItems.size() == 1U && presenter.ViewModel().menuItems[0].text == "Demo Character", "角色选择默认菜单");

    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::CharacterSelected, "demo", 0}, ReplaceTestSubScene);
    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::MapReady, "map", 0}, ReplaceTestSubScene);
    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::ObjectListReceived, "objects", 0}, ReplaceTestSubScene);

    failures += Expect(presenter.ViewModel().screen == UiScreenType::World, "进入 World screen");
    failures += Expect(presenter.ViewModel().menuItems.empty() && presenter.ViewModel().selectedIndex == 0U, "World 清理角色菜单并重置选择");

    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::BattleStarted, {}, 0}, ReplaceTestSubScene);
    failures += Expect(presenter.ViewModel().screen == UiScreenType::Battle, "进入 Battle screen");
    failures += Expect(presenter.ViewModel().menuItems.size() == 2U, "Battle 重建默认菜单");
    failures += Expect(presenter.ViewModel().menuItems[0].text == "Attack" && presenter.ViewModel().selectedIndex == 0U, "Battle 默认选中 Attack");

    const std::vector<GameCommand> commands = DriveUiInput(
        machine,
        manager,
        presenter,
        {UiInputEventType::Key, UiKey::Enter, {}},
        ReplaceTestSubScene);
    failures += Expect(commands.size() == 1U && commands[0].packetIntent == GamePacketIntentType::BattleCommand, "Battle Enter 发送战斗命令");
    failures += Expect(commands[0].payload == "Attack", "Battle Enter payload 不沿用角色菜单");
    return failures;
}

int TestModalBlocksUiInput()
{
    int failures = 0;
    GameStateMachine machine;
    SceneManager manager;
    SdlTextUiPresenter presenter;
    RegisterRoots(manager);
    (void)manager.Start(RootSceneId::Boot);

    UiViewModel model;
    model.screen = UiScreenType::Login;
    presenter.SetViewModel(model);
    (void)DriveGameEvent(machine, manager, presenter, {GameEventType::AppStarted, {}, 0}, nullptr);
    manager.ReplaceCurrentSubScene(std::make_unique<ModalSubScene>());

    const std::vector<GameCommand> commands = DriveUiInput(
        machine,
        manager,
        presenter,
        {UiInputEventType::TextInput, UiKey::None, "blocked"},
        nullptr);
    failures += Expect(commands.empty(), "modal 阻断时不产生命令");
    failures += Expect(presenter.ViewModel().inputText.empty(), "modal 阻断时 Presenter 不接收输入");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestPublicHeaderSurface();
    failures += TestIntentMapping();
    failures += TestDriveGameEventAppliesSceneAndPresenter();
    failures += TestDriveUiInputToCommand();
    failures += TestOverlayReplaceAndCloseClearsCurrent();
    failures += TestScreenChangeRebuildsMenu();
    failures += TestModalBlocksUiInput();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
