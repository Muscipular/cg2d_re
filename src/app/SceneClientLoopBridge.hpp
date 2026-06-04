#pragma once

#include "game/GameCommand.hpp"
#include "game/GameEvent.hpp"
#include "game/GameState.hpp"
#include "game/GameStateMachine.hpp"
#include "scene/SceneManager.hpp"
#include "scene/SceneTypes.hpp"
#include "ui/IUiPresenter.hpp"
#include "ui/UiIntent.hpp"
#include "ui/UiViewModel.hpp"

#include <optional>
#include <vector>

namespace cg::app {

using SceneReplaceSubSceneFn = void (*)(cg::scene::SceneManager& manager, cg::game::GameSubSceneType subScene);

[[nodiscard]] cg::scene::RootSceneId ToSceneRoot(cg::game::GameRootSceneType rootScene) noexcept;
[[nodiscard]] cg::ui::UiScreenType ToUiScreen(cg::game::GameStateType state) noexcept;
[[nodiscard]] const char* ToGameStateName(cg::game::GameStateType state) noexcept;
[[nodiscard]] cg::scene::SceneInputEvent ToSceneInput(const cg::ui::UiInputEvent& event);
[[nodiscard]] std::optional<cg::game::GameEvent> ToGameEvent(const cg::ui::UiIntent& intent);

void SyncPresenterViewModel(cg::ui::IUiPresenter& presenter, const cg::game::GameState& state);
void ApplyGameCommand(
    const cg::game::GameCommand& command,
    cg::scene::SceneManager& sceneManager,
    cg::ui::IUiPresenter& presenter,
    SceneReplaceSubSceneFn replaceSubScene = nullptr);

[[nodiscard]] std::vector<cg::game::GameCommand> DriveGameEvent(
    cg::game::GameStateMachine& stateMachine,
    cg::scene::SceneManager& sceneManager,
    cg::ui::IUiPresenter& presenter,
    const cg::game::GameEvent& event,
    SceneReplaceSubSceneFn replaceSubScene = nullptr);

[[nodiscard]] std::vector<cg::game::GameCommand> DriveUiInput(
    cg::game::GameStateMachine& stateMachine,
    cg::scene::SceneManager& sceneManager,
    cg::ui::IUiPresenter& presenter,
    const cg::ui::UiInputEvent& event,
    SceneReplaceSubSceneFn replaceSubScene = nullptr);

} // namespace cg::app
