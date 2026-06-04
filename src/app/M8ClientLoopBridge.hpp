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

using M8ReplaceSubSceneFn = void (*)(cg::scene::SceneManager& manager, cg::game::GameSubSceneType subScene);

[[nodiscard]] cg::scene::RootSceneId ToM8SceneRoot(cg::game::GameRootSceneType rootScene) noexcept;
[[nodiscard]] cg::ui::UiScreenType ToM8UiScreen(cg::game::GameStateType state) noexcept;
[[nodiscard]] const char* ToM8GameStateName(cg::game::GameStateType state) noexcept;
[[nodiscard]] cg::scene::SceneInputEvent ToM8SceneInput(const cg::ui::UiInputEvent& event);
[[nodiscard]] std::optional<cg::game::GameEvent> ToM8GameEvent(const cg::ui::UiIntent& intent);

void SyncM8PresenterViewModel(cg::ui::IUiPresenter& presenter, const cg::game::GameState& state);
void ApplyM8GameCommand(
    const cg::game::GameCommand& command,
    cg::scene::SceneManager& sceneManager,
    cg::ui::IUiPresenter& presenter,
    M8ReplaceSubSceneFn replaceSubScene = nullptr);

[[nodiscard]] std::vector<cg::game::GameCommand> DriveM8GameEvent(
    cg::game::GameStateMachine& stateMachine,
    cg::scene::SceneManager& sceneManager,
    cg::ui::IUiPresenter& presenter,
    const cg::game::GameEvent& event,
    M8ReplaceSubSceneFn replaceSubScene = nullptr);

[[nodiscard]] std::vector<cg::game::GameCommand> DriveM8UiInput(
    cg::game::GameStateMachine& stateMachine,
    cg::scene::SceneManager& sceneManager,
    cg::ui::IUiPresenter& presenter,
    const cg::ui::UiInputEvent& event,
    M8ReplaceSubSceneFn replaceSubScene = nullptr);

} // namespace cg::app
