#include "app/M8ClientLoopBridge.hpp"

#include <utility>

namespace cg::app {
namespace {

void EnsureDefaultMenu(cg::ui::UiViewModel& model)
{
    if (!model.menuItems.empty()) {
        return;
    }

    switch (model.screen) {
    case cg::ui::UiScreenType::CharacterSelect:
        model.menuItems.push_back(cg::ui::UiMenuItem{"Demo Character", true});
        break;
    case cg::ui::UiScreenType::Battle:
        model.menuItems.push_back(cg::ui::UiMenuItem{"Attack", true});
        model.menuItems.push_back(cg::ui::UiMenuItem{"Guard", true});
        break;
    case cg::ui::UiScreenType::Login:
    case cg::ui::UiScreenType::World:
        break;
    }
}

void ApplyStatus(cg::ui::UiViewModel& model, const cg::game::GameState& state)
{
    if (!state.lastMessage.empty()) {
        model.statusText = state.lastMessage;
        return;
    }

    if (!state.lastWarning.empty()) {
        model.statusText = state.lastWarning;
    }
}

} // namespace

cg::scene::RootSceneId ToM8SceneRoot(cg::game::GameRootSceneType rootScene) noexcept
{
    switch (rootScene) {
    case cg::game::GameRootSceneType::Boot:
        return cg::scene::RootSceneId::Boot;
    case cg::game::GameRootSceneType::Login:
        return cg::scene::RootSceneId::Login;
    case cg::game::GameRootSceneType::CharacterSelect:
        return cg::scene::RootSceneId::CharacterSelect;
    case cg::game::GameRootSceneType::Loading:
        return cg::scene::RootSceneId::Loading;
    case cg::game::GameRootSceneType::World:
        return cg::scene::RootSceneId::World;
    case cg::game::GameRootSceneType::Battle:
        return cg::scene::RootSceneId::Battle;
    case cg::game::GameRootSceneType::Disconnected:
        return cg::scene::RootSceneId::Disconnected;
    case cg::game::GameRootSceneType::Shutdown:
        return cg::scene::RootSceneId::Shutdown;
    case cg::game::GameRootSceneType::None:
        break;
    }

    return cg::scene::RootSceneId::Boot;
}

cg::ui::UiScreenType ToM8UiScreen(cg::game::GameStateType state) noexcept
{
    switch (state) {
    case cg::game::GameStateType::CharacterSelect:
        return cg::ui::UiScreenType::CharacterSelect;
    case cg::game::GameStateType::World:
        return cg::ui::UiScreenType::World;
    case cg::game::GameStateType::Battle:
        return cg::ui::UiScreenType::Battle;
    case cg::game::GameStateType::Boot:
    case cg::game::GameStateType::Login:
    case cg::game::GameStateType::LoginPending:
    case cg::game::GameStateType::CharacterListPending:
    case cg::game::GameStateType::WorldLoading:
    case cg::game::GameStateType::Disconnected:
    case cg::game::GameStateType::Shutdown:
        break;
    }

    return cg::ui::UiScreenType::Login;
}

const char* ToM8GameStateName(cg::game::GameStateType state) noexcept
{
    switch (state) {
    case cg::game::GameStateType::Boot:
        return "Boot";
    case cg::game::GameStateType::Login:
        return "Login";
    case cg::game::GameStateType::LoginPending:
        return "LoginPending";
    case cg::game::GameStateType::CharacterListPending:
        return "CharacterListPending";
    case cg::game::GameStateType::CharacterSelect:
        return "CharacterSelect";
    case cg::game::GameStateType::WorldLoading:
        return "WorldLoading";
    case cg::game::GameStateType::World:
        return "World";
    case cg::game::GameStateType::Battle:
        return "Battle";
    case cg::game::GameStateType::Disconnected:
        return "Disconnected";
    case cg::game::GameStateType::Shutdown:
        return "Shutdown";
    }

    return "Unknown";
}

cg::scene::SceneInputEvent ToM8SceneInput(const cg::ui::UiInputEvent& event)
{
    if (event.type == cg::ui::UiInputEventType::TextInput) {
        return cg::scene::SceneInputEvent{cg::scene::SceneInputEventType::Text, event.text};
    }

    switch (event.key) {
    case cg::ui::UiKey::Enter:
        return cg::scene::SceneInputEvent{cg::scene::SceneInputEventType::Confirm, {}};
    case cg::ui::UiKey::Escape:
        return cg::scene::SceneInputEvent{cg::scene::SceneInputEventType::Cancel, {}};
    case cg::ui::UiKey::Up:
        return cg::scene::SceneInputEvent{cg::scene::SceneInputEventType::Command, "up"};
    case cg::ui::UiKey::Down:
        return cg::scene::SceneInputEvent{cg::scene::SceneInputEventType::Command, "down"};
    case cg::ui::UiKey::Left:
        return cg::scene::SceneInputEvent{cg::scene::SceneInputEventType::Command, "left"};
    case cg::ui::UiKey::Right:
        return cg::scene::SceneInputEvent{cg::scene::SceneInputEventType::Command, "right"};
    case cg::ui::UiKey::Backspace:
        return cg::scene::SceneInputEvent{cg::scene::SceneInputEventType::Command, "backspace"};
    case cg::ui::UiKey::None:
        break;
    }

    return {};
}

std::optional<cg::game::GameEvent> ToM8GameEvent(const cg::ui::UiIntent& intent)
{
    switch (intent.type) {
    case cg::ui::UiIntentType::SubmitLogin:
        return cg::game::GameEvent{cg::game::GameEventType::LoginSubmitted, intent.text, 0};
    case cg::ui::UiIntentType::SelectCharacter:
        return cg::game::GameEvent{cg::game::GameEventType::CharacterSelected, intent.text, static_cast<int>(intent.index)};
    case cg::ui::UiIntentType::SubmitChat:
        return cg::game::GameEvent{cg::game::GameEventType::ChatSubmitted, intent.text, 0};
    case cg::ui::UiIntentType::SelectMenuItem:
        return cg::game::GameEvent{cg::game::GameEventType::BattleCommandSubmitted, intent.text, static_cast<int>(intent.index)};
    case cg::ui::UiIntentType::None:
    case cg::ui::UiIntentType::EditText:
    case cg::ui::UiIntentType::Cancel:
    case cg::ui::UiIntentType::Navigate:
        break;
    }

    return std::nullopt;
}

void SyncM8PresenterViewModel(cg::ui::IUiPresenter& presenter, const cg::game::GameState& state)
{
    cg::ui::UiViewModel model = presenter.ViewModel();
    model.screen = ToM8UiScreen(state.type);
    if (model.title.empty()) {
        model.title = "M8";
    }
    EnsureDefaultMenu(model);
    ApplyStatus(model, state);
    presenter.SetViewModel(std::move(model));
}

void ApplyM8GameCommand(
    const cg::game::GameCommand& command,
    cg::scene::SceneManager& sceneManager,
    cg::ui::IUiPresenter& presenter,
    M8PushSubSceneFn pushSubScene)
{
    switch (command.type) {
    case cg::game::GameCommandType::ReplaceRootScene:
        sceneManager.ReplaceRootScene(ToM8SceneRoot(command.rootScene));
        sceneManager.Update(0.0F);
        break;
    case cg::game::GameCommandType::PushSubScene:
        if (pushSubScene != nullptr) {
            pushSubScene(sceneManager, command.subScene);
        }
        break;
    case cg::game::GameCommandType::PopSubScene:
        sceneManager.PopSubScene();
        break;
    case cg::game::GameCommandType::ClearSubScenes:
        sceneManager.ClearSubScenes();
        break;
    case cg::game::GameCommandType::AppendChatMessage: {
        cg::ui::UiViewModel model = presenter.ViewModel();
        model.chatLines.push_back(command.payload);
        presenter.SetViewModel(std::move(model));
        break;
    }
    case cg::game::GameCommandType::ShowMessage:
    case cg::game::GameCommandType::LogWarning: {
        cg::ui::UiViewModel model = presenter.ViewModel();
        model.statusText = command.payload;
        presenter.SetViewModel(std::move(model));
        break;
    }
    case cg::game::GameCommandType::SendPacketIntent:
    case cg::game::GameCommandType::LoadMap:
    case cg::game::GameCommandType::ApplyMapData:
    case cg::game::GameCommandType::ApplyObjectList:
    case cg::game::GameCommandType::ApplyWorldUpdate:
        break;
    }
}

std::vector<cg::game::GameCommand> DriveM8GameEvent(
    cg::game::GameStateMachine& stateMachine,
    cg::scene::SceneManager& sceneManager,
    cg::ui::IUiPresenter& presenter,
    const cg::game::GameEvent& event,
    M8PushSubSceneFn pushSubScene)
{
    std::vector<cg::game::GameCommand> commands = stateMachine.HandleEvent(event);
    for (const cg::game::GameCommand& command : commands) {
        ApplyM8GameCommand(command, sceneManager, presenter, pushSubScene);
    }
    SyncM8PresenterViewModel(presenter, stateMachine.State());
    return commands;
}

std::vector<cg::game::GameCommand> DriveM8UiInput(
    cg::game::GameStateMachine& stateMachine,
    cg::scene::SceneManager& sceneManager,
    cg::ui::IUiPresenter& presenter,
    const cg::ui::UiInputEvent& event,
    M8PushSubSceneFn pushSubScene)
{
    const cg::scene::SceneInputEvent sceneInput = ToM8SceneInput(event);
    if (sceneInput.type != cg::scene::SceneInputEventType::None && sceneManager.HandleInput(sceneInput)) {
        return {};
    }

    const cg::ui::UiIntent intent = presenter.HandleInput(event);
    const std::optional<cg::game::GameEvent> gameEvent = ToM8GameEvent(intent);
    if (!gameEvent.has_value()) {
        return {};
    }

    return DriveM8GameEvent(stateMachine, sceneManager, presenter, *gameEvent, pushSubScene);
}

} // namespace cg::app
