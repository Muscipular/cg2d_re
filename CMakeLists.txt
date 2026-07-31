cmake_minimum_required(VERSION 3.25)

project(cg_engine
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CG_SDL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sdl" CACHE PATH "SDL3 release package root")
set(CG_YAML_CPP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/yaml-cpp/yaml-cpp-0.9.0" CACHE PATH "yaml-cpp source root")

set(SDL3_DIR "${CG_SDL_ROOT}/SDL3-3.4.8/cmake" CACHE PATH "SDL3 CMake config directory")
set(SDL3_image_DIR "${CG_SDL_ROOT}/SDL3_image-3.4.4/cmake" CACHE PATH "SDL3_image CMake config directory")
set(SDL3_ttf_DIR "${CG_SDL_ROOT}/SDL3_ttf-3.2.2/cmake" CACHE PATH "SDL3_ttf CMake config directory")

foreach(required_dir IN ITEMS "${SDL3_DIR}" "${SDL3_image_DIR}" "${SDL3_ttf_DIR}")
    if(NOT EXISTS "${required_dir}")
        message(FATAL_ERROR "找不到 SDL3 依赖目录：${required_dir}。请先运行 scripts/download_sdl3.ps1。")
    endif()
endforeach()

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

if(NOT EXISTS "${CG_YAML_CPP_ROOT}/CMakeLists.txt")
    message(FATAL_ERROR "找不到 yaml-cpp 源码目录：${CG_YAML_CPP_ROOT}。请先运行 scripts/download_yaml_cpp.ps1。")
endif()

set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "Build yaml-cpp shared libraries" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Build yaml-cpp tests" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "Build yaml-cpp tools" FORCE)
set(YAML_CPP_INSTALL OFF CACHE BOOL "Install yaml-cpp targets" FORCE)
add_subdirectory("${CG_YAML_CPP_ROOT}" "${CMAKE_BINARY_DIR}/third_party/yaml-cpp")

add_executable(cg-client
    src/app/main.cpp
    src/app/SceneClientLoopBridge.cpp
    src/core/Log.cpp
    src/game/GameStateMachine.cpp
    src/render/FontCache.cpp
    src/render/RenderQueue.cpp
    src/render/SdlRendererBackend.cpp
    src/render/TextureAtlas.cpp
    src/render/TextureCache.cpp
    src/scene/SceneManager.cpp
    src/scene/WorldScene.cpp
    src/ui/GraphicalUiPresenter.cpp
    src/ui/SdlTextUiPresenter.cpp
    src/ui/UiTextRenderer.cpp
    src/ui/UiIntent.cpp
    src/world/EntitySpriteBuilder.cpp
    src/world/EntityStore.cpp
    src/world/IdaRenderRules.cpp
    src/world/MapObjectSpriteBuilder.cpp
    src/world/VisibilityCulling.cpp
    src/world/MapObjectTextureCache.cpp
    src/world/WorldRenderer.cpp
    src/world/AnimationState.cpp
    src/world/Movement.cpp
    src/world/MovementAnimation.cpp
    src/world/SceneGroundAtlasCache.cpp
    src/world/SceneGroundSpriteBuilder.cpp
    src/world/Transform.cpp
    src/text/TextEncoding.cpp
    src/ui/ChatBox.cpp
    src/ui/InputBox.cpp
    src/ui/TextLayout.cpp
    src/platform/Console.cpp
    src/platform/Event.cpp
    src/platform/Ime.cpp
    src/platform/Input.cpp
    src/platform/Path.cpp
    src/platform/Thread.cpp
    src/platform/Timer.cpp
    src/platform/Window.cpp
    src/resource/AnimationLoader.cpp
    src/resource/AutoMapData.cpp
    src/resource/AutoMapRenderer.cpp
    src/resource/AutoMapRules.cpp
    src/resource/BinaryReader.cpp
    src/resource/BinSetConfigLoader.cpp
    src/resource/GraphicDecoder.cpp
    src/resource/HiddenPaletteQueue.cpp
    src/resource/MapLoader.cpp
    src/resource/Palette.cpp
    src/resource/ResourceManager.cpp
)

add_executable(cg-map-viewer
    tools/map_viewer/main.cpp
    src/core/Log.cpp
    src/resource/AutoMapData.cpp
    src/resource/AutoMapRenderer.cpp
    src/resource/AnimationLoader.cpp
    src/resource/AutoMapRules.cpp
    src/resource/BinaryReader.cpp
    src/resource/BinSetConfigLoader.cpp
    src/resource/GraphicDecoder.cpp
    src/resource/HiddenPaletteQueue.cpp
    src/resource/MapLoader.cpp
    src/resource/Palette.cpp
    src/resource/ResourceManager.cpp
    src/platform/Console.cpp
)

add_executable(cg-asset-viewer
    tools/asset_viewer/main.cpp
    src/core/Log.cpp
    src/resource/AnimationLoader.cpp
    src/resource/AutoMapRules.cpp
    src/resource/BinaryReader.cpp
    src/resource/BinSetConfigLoader.cpp
    src/resource/GraphicDecoder.cpp
    src/resource/HiddenPaletteQueue.cpp
    src/resource/Palette.cpp
    src/resource/ResourceManager.cpp
    src/platform/Console.cpp
)

add_executable(cg-anim-viewer
    tools/anim_viewer/main.cpp
    src/core/Log.cpp
    src/resource/AnimationLoader.cpp
    src/resource/AutoMapRules.cpp
    src/resource/BinaryReader.cpp
    src/resource/BinSetConfigLoader.cpp
    src/resource/GraphicDecoder.cpp
    src/resource/HiddenPaletteQueue.cpp
    src/resource/Palette.cpp
    src/resource/ResourceManager.cpp
    src/platform/Console.cpp
)

add_executable(cg-anim-exporter
    tools/anim_exporter/main.cpp
    src/core/Log.cpp
    src/resource/AnimationLoader.cpp
    src/resource/AutoMapRules.cpp
    src/resource/BinaryReader.cpp
    src/resource/BinSetConfigLoader.cpp
    src/resource/GraphicDecoder.cpp
    src/resource/HiddenPaletteQueue.cpp
    src/resource/Palette.cpp
    src/resource/ResourceManager.cpp
    src/platform/Console.cpp
)

target_include_directories(cg-client
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(cg-map-viewer
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(cg-asset-viewer
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(cg-anim-viewer
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(cg-anim-exporter
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(cg-client
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        yaml-cpp::yaml-cpp
)

target_link_libraries(cg-map-viewer
    PRIVATE
        SDL3::SDL3
        yaml-cpp::yaml-cpp
)

target_link_libraries(cg-asset-viewer
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        yaml-cpp::yaml-cpp
)

target_link_libraries(cg-anim-viewer
    PRIVATE
        SDL3::SDL3
        yaml-cpp::yaml-cpp
)

target_link_libraries(cg-anim-exporter
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        yaml-cpp::yaml-cpp
)

enable_testing()

add_executable(cg-automap-renderer-test
    tests/resource/AutoMapRendererTest.cpp
    src/resource/AutoMapData.cpp
    src/resource/AnimationLoader.cpp
    src/resource/AutoMapRenderer.cpp
    src/resource/AutoMapRules.cpp
    src/resource/BinaryReader.cpp
    src/resource/GraphicDecoder.cpp
    src/resource/HiddenPaletteQueue.cpp
    src/resource/Palette.cpp
    src/resource/ResourceManager.cpp
)

target_include_directories(cg-automap-renderer-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-automap-renderer-test COMMAND cg-automap-renderer-test)

add_executable(cg-resource-graphic-decoder-transparency-test
    tests/resource/GraphicDecoderTransparencyTest.cpp
    src/resource/BinaryReader.cpp
    src/resource/GraphicDecoder.cpp
    src/resource/Palette.cpp
)

target_include_directories(cg-resource-graphic-decoder-transparency-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-resource-graphic-decoder-transparency-test COMMAND cg-resource-graphic-decoder-transparency-test)

add_executable(cg-world-ida-render-rules-test
    tests/world/IdaRenderRulesTest.cpp
    src/world/IdaRenderRules.cpp
)

target_include_directories(cg-world-ida-render-rules-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-ida-render-rules-test COMMAND cg-world-ida-render-rules-test)

add_executable(cg-world-map-tile-atlas-cache-key-test
    tests/world/MapTileAtlasCacheKeyTest.cpp
)

target_include_directories(cg-world-map-tile-atlas-cache-key-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-map-tile-atlas-cache-key-test COMMAND cg-world-map-tile-atlas-cache-key-test)

add_executable(cg-world-map-object-texture-cache-test
    tests/world/MapObjectTextureCacheTest.cpp
    src/world/MapObjectTextureCache.cpp
    src/render/TextureCache.cpp
)

target_include_directories(cg-world-map-object-texture-cache-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-map-object-texture-cache-test COMMAND cg-world-map-object-texture-cache-test)

add_executable(cg-world-map-object-sprite-builder-test
    tests/world/MapObjectSpriteBuilderTest.cpp
    src/world/MapObjectSpriteBuilder.cpp
    src/world/VisibilityCulling.cpp
    src/world/Transform.cpp
    src/world/IdaRenderRules.cpp
)

target_include_directories(cg-world-map-object-sprite-builder-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-map-object-sprite-builder-test COMMAND cg-world-map-object-sprite-builder-test)

add_executable(cg-world-scene-ground-atlas-cache-test
    tests/world/SceneGroundAtlasCacheTest.cpp
    src/world/SceneGroundAtlasCache.cpp
    src/world/IdaRenderRules.cpp
    src/render/TextureAtlas.cpp
    src/render/TextureCache.cpp
)

target_include_directories(cg-world-scene-ground-atlas-cache-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-scene-ground-atlas-cache-test COMMAND cg-world-scene-ground-atlas-cache-test)

add_executable(cg-world-scene-ground-sprite-builder-test
    tests/world/SceneGroundSpriteBuilderTest.cpp
    src/world/SceneGroundSpriteBuilder.cpp
    src/world/VisibilityCulling.cpp
    src/world/Transform.cpp
    src/world/IdaRenderRules.cpp
    src/render/TextureAtlas.cpp
)

target_include_directories(cg-world-scene-ground-sprite-builder-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-scene-ground-sprite-builder-test COMMAND cg-world-scene-ground-sprite-builder-test)

add_executable(cg-world-visibility-culling-test
    tests/world/VisibilityCullingTest.cpp
    src/world/VisibilityCulling.cpp
    src/world/SceneGroundSpriteBuilder.cpp
    src/world/MapObjectSpriteBuilder.cpp
    src/world/Transform.cpp
    src/world/IdaRenderRules.cpp
    src/render/TextureAtlas.cpp
)

target_include_directories(cg-world-visibility-culling-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-visibility-culling-test COMMAND cg-world-visibility-culling-test)

add_executable(cg-world-render-order-test
    tests/world/WorldRenderOrderTest.cpp
    src/render/RenderQueue.cpp
    src/world/IdaRenderRules.cpp
    src/world/SceneGroundSpriteBuilder.cpp
    src/world/MapObjectSpriteBuilder.cpp
    src/world/VisibilityCulling.cpp
    src/world/Transform.cpp
    src/render/TextureAtlas.cpp
)

target_include_directories(cg-world-render-order-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-render-order-test COMMAND cg-world-render-order-test)

add_executable(cg-world-world-renderer-test
    tests/world/WorldRendererTest.cpp
    src/world/WorldRenderer.cpp
    src/world/EntitySpriteBuilder.cpp
    src/world/MovementAnimation.cpp
    src/world/Movement.cpp
    src/world/AnimationState.cpp
    src/world/EntityStore.cpp
    src/world/SceneGroundAtlasCache.cpp
    src/world/SceneGroundSpriteBuilder.cpp
    src/world/MapObjectTextureCache.cpp
    src/world/MapObjectSpriteBuilder.cpp
    src/world/VisibilityCulling.cpp
    src/world/Transform.cpp
    src/world/IdaRenderRules.cpp
    src/render/TextureAtlas.cpp
    src/render/TextureCache.cpp
    src/resource/AnimationLoader.cpp
    src/resource/BinaryReader.cpp
)

target_include_directories(cg-world-world-renderer-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-world-renderer-test COMMAND cg-world-world-renderer-test)

add_executable(cg-world-synthetic-population-test
    tests/world/SyntheticPopulationTest.cpp
    src/world/SyntheticPopulation.cpp
    src/world/EntityStore.cpp
    src/world/Transform.cpp
    src/render/RenderQueue.cpp
)

target_include_directories(cg-world-synthetic-population-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-synthetic-population-test COMMAND cg-world-synthetic-population-test)

add_executable(cg-world-entity-store-test
    tests/world/EntityStoreTest.cpp
    src/world/EntityStore.cpp
)

target_include_directories(cg-world-entity-store-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-entity-store-test COMMAND cg-world-entity-store-test)

add_executable(cg-world-transform-test
    tests/world/TransformTest.cpp
    src/world/Transform.cpp
)

target_include_directories(cg-world-transform-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-transform-test COMMAND cg-world-transform-test)

add_executable(cg-world-movement-test
    tests/world/MovementTest.cpp
    src/world/Movement.cpp
    src/world/Transform.cpp
)

target_include_directories(cg-world-movement-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-movement-test COMMAND cg-world-movement-test)

add_executable(cg-world-animation-state-test
    tests/world/AnimationStateTest.cpp
    src/world/AnimationState.cpp
    src/resource/AnimationLoader.cpp
    src/resource/BinaryReader.cpp
)

target_include_directories(cg-world-animation-state-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-animation-state-test COMMAND cg-world-animation-state-test)

add_executable(cg-world-movement-animation-test
    tests/world/MovementAnimationTest.cpp
    src/world/MovementAnimation.cpp
    src/world/Movement.cpp
    src/world/AnimationState.cpp
    src/world/Transform.cpp
    src/resource/AnimationLoader.cpp
    src/resource/BinaryReader.cpp
)

target_include_directories(cg-world-movement-animation-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-movement-animation-test COMMAND cg-world-movement-animation-test)

add_executable(cg-world-entity-sprite-builder-test
    tests/world/EntitySpriteBuilderTest.cpp
    src/world/EntitySpriteBuilder.cpp
    src/world/MovementAnimation.cpp
    src/world/Movement.cpp
    src/world/AnimationState.cpp
    src/world/EntityStore.cpp
    src/world/Transform.cpp
    src/resource/AnimationLoader.cpp
    src/resource/BinaryReader.cpp
)

target_include_directories(cg-world-entity-sprite-builder-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-world-entity-sprite-builder-test COMMAND cg-world-entity-sprite-builder-test)

add_executable(cg-game-state-machine-test
    tests/game/GameStateMachineTest.cpp
    src/game/GameStateMachine.cpp
)

target_include_directories(cg-game-state-machine-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_definitions(cg-game-state-machine-test
    PRIVATE
        CG_GAME_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_test(NAME cg-game-state-machine-test COMMAND cg-game-state-machine-test)

add_executable(cg-scene-manager-test
    tests/scene/SceneManagerTest.cpp
    src/render/RenderQueue.cpp
    src/scene/SceneManager.cpp
    src/scene/WorldScene.cpp
    src/world/EntityStore.cpp
)

target_include_directories(cg-scene-manager-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_definitions(cg-scene-manager-test
    PRIVATE
        CG_SCENE_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_test(NAME cg-scene-manager-test COMMAND cg-scene-manager-test)

add_executable(cg-ui-presenter-test
    tests/ui/UiPresenterTest.cpp
    src/ui/UiIntent.cpp
    src/ui/SdlTextUiPresenter.cpp
    src/ui/GraphicalUiPresenter.cpp
)

target_include_directories(cg-ui-presenter-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_definitions(cg-ui-presenter-test
    PRIVATE
        CG_UI_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_test(NAME cg-ui-presenter-test COMMAND cg-ui-presenter-test)

add_executable(cg-app-scene-client-loop-bridge-test
    tests/app/SceneClientLoopBridgeTest.cpp
    src/app/SceneClientLoopBridge.cpp
    src/game/GameStateMachine.cpp
    src/scene/SceneManager.cpp
    src/ui/SdlTextUiPresenter.cpp
    src/ui/UiIntent.cpp
)

target_include_directories(cg-app-scene-client-loop-bridge-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_definitions(cg-app-scene-client-loop-bridge-test
    PRIVATE
        CG_APP_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_test(NAME cg-app-scene-client-loop-bridge-test COMMAND cg-app-scene-client-loop-bridge-test)

add_executable(cg-packet-types-test
    tests/packet/PacketTypesTest.cpp
)

target_include_directories(cg-packet-types-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-types-test COMMAND cg-packet-types-test)

add_executable(cg-packet-character-list-test
    tests/packet/CharacterListPacketTest.cpp
)
target_include_directories(cg-packet-character-list-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(NAME cg-packet-character-list-test COMMAND cg-packet-character-list-test)

add_executable(cg-packet-character-logout-request-test
    tests/packet/CharacterLogoutRequestPacketTest.cpp
)
target_include_directories(cg-packet-character-logout-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-character-logout-request-test
    COMMAND cg-packet-character-logout-request-test
)

add_executable(cg-packet-character-login-request-test
    tests/packet/CharacterLoginRequestPacketTest.cpp
)
target_include_directories(cg-packet-character-login-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-character-login-request-test
    COMMAND cg-packet-character-login-request-test
)

add_executable(cg-packet-profile-status-update-request-test
    tests/packet/ProfileStatusUpdateRequestPacketTest.cpp
)
target_include_directories(cg-packet-profile-status-update-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-profile-status-update-request-test
    COMMAND cg-packet-profile-status-update-request-test
)

add_executable(cg-packet-echo-request-test
    tests/packet/EchoRequestPacketTest.cpp
)
target_include_directories(cg-packet-echo-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-echo-request-test
    COMMAND cg-packet-echo-request-test
)

add_executable(cg-packet-echo-response-test
    tests/packet/EchoResponsePacketTest.cpp
)
target_include_directories(cg-packet-echo-response-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-echo-response-test
    COMMAND cg-packet-echo-response-test
)

add_executable(cg-packet-keep-alive-request-test
    tests/packet/KeepAliveRequestPacketTest.cpp
)
target_include_directories(cg-packet-keep-alive-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-keep-alive-request-test
    COMMAND cg-packet-keep-alive-request-test
)

add_executable(cg-packet-move-request-test
    tests/packet/MoveRequestPacketTest.cpp
)
target_include_directories(cg-packet-move-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-move-request-test
    COMMAND cg-packet-move-request-test
)

add_executable(cg-packet-party-leave-position-sync-test
    tests/packet/PartyLeavePositionSyncPacketTest.cpp
)
target_include_directories(cg-packet-party-leave-position-sync-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-party-leave-position-sync-test
    COMMAND cg-packet-party-leave-position-sync-test
)

add_executable(cg-packet-item-use-request-test
    tests/packet/ItemUseRequestPacketTest.cpp
)
target_include_directories(cg-packet-item-use-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-item-use-request-test
    COMMAND cg-packet-item-use-request-test
)

add_executable(cg-packet-field-recovery-item-use-request-test
    tests/packet/FieldRecoveryItemUseRequestPacketTest.cpp
)
target_include_directories(cg-packet-field-recovery-item-use-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-field-recovery-item-use-request-test
    COMMAND cg-packet-field-recovery-item-use-request-test
)

add_executable(cg-packet-pick-up-item-test
    tests/packet/PickUpItemPacketTest.cpp
)
target_include_directories(cg-packet-pick-up-item-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-pick-up-item-test
    COMMAND cg-packet-pick-up-item-test
)

add_executable(cg-packet-trade-status-test
    tests/packet/TradeStatusPacketTest.cpp
)
target_include_directories(cg-packet-trade-status-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-trade-status-test
    COMMAND cg-packet-trade-status-test
)

add_executable(cg-packet-guild-dissolve-request-test
    tests/packet/GuildDissolveRequestPacketTest.cpp
)
target_include_directories(cg-packet-guild-dissolve-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-guild-dissolve-request-test
    COMMAND cg-packet-guild-dissolve-request-test
)

add_executable(cg-packet-bag-page-manager-request-test
    tests/packet/BagPageManagerRequestPacketTest.cpp
)
target_include_directories(cg-packet-bag-page-manager-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-bag-page-manager-request-test
    COMMAND cg-packet-bag-page-manager-request-test
)

add_executable(cg-packet-guild-join-notification-test
    tests/packet/GuildJoinNotificationPacketTest.cpp
)
target_include_directories(cg-packet-guild-join-notification-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-guild-join-notification-test
    COMMAND cg-packet-guild-join-notification-test
)

add_executable(cg-packet-npc-event-request-test
    tests/packet/NpcEventRequestPacketTest.cpp
)
target_include_directories(cg-packet-npc-event-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-npc-event-request-test
    COMMAND cg-packet-npc-event-request-test
)

add_executable(cg-packet-encounter-request-test
    tests/packet/EncounterRequestPacketTest.cpp
)
target_include_directories(cg-packet-encounter-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-encounter-request-test
    COMMAND cg-packet-encounter-request-test
)

add_executable(cg-packet-duel-request-test
    tests/packet/DuelRequestPacketTest.cpp
)
target_include_directories(cg-packet-duel-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-duel-request-test
    COMMAND cg-packet-duel-request-test
)

add_executable(cg-packet-battle-join-request-test
    tests/packet/BattleJoinRequestPacketTest.cpp
)
target_include_directories(cg-packet-battle-join-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-battle-join-request-test
    COMMAND cg-packet-battle-join-request-test
)

add_executable(cg-packet-battle-watch-request-test
    tests/packet/BattleWatchRequestPacketTest.cpp
)
target_include_directories(cg-packet-battle-watch-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-battle-watch-request-test
    COMMAND cg-packet-battle-watch-request-test
)

add_executable(cg-packet-battle-exit-request-test
    tests/packet/BattleExitRequestPacketTest.cpp
)
target_include_directories(cg-packet-battle-exit-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-battle-exit-request-test
    COMMAND cg-packet-battle-exit-request-test
)

add_executable(cg-packet-battle-watch-stop-request-test
    tests/packet/BattleWatchStopRequestPacketTest.cpp
)
target_include_directories(cg-packet-battle-watch-stop-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-battle-watch-stop-request-test
    COMMAND cg-packet-battle-watch-stop-request-test
)

add_executable(cg-packet-npc-event-result-test
    tests/packet/NpcEventResultPacketTest.cpp
)
target_include_directories(cg-packet-npc-event-result-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-npc-event-result-test
    COMMAND cg-packet-npc-event-result-test
)

add_executable(cg-packet-battle-entry-notification-test
    tests/packet/BattleEntryNotificationPacketTest.cpp
)
target_include_directories(cg-packet-battle-entry-notification-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-battle-entry-notification-test
    COMMAND cg-packet-battle-entry-notification-test
)

add_executable(cg-packet-battle-command-request-test
    tests/packet/BattleCommandRequestPacketTest.cpp
)
target_include_directories(cg-packet-battle-command-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-battle-command-request-test
    COMMAND cg-packet-battle-command-request-test
)

add_executable(cg-packet-item-front-char-list-request-test
    tests/packet/ItemFrontCharListRequestPacketTest.cpp
)
target_include_directories(cg-packet-item-front-char-list-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-item-front-char-list-request-test
    COMMAND cg-packet-item-front-char-list-request-test
)

add_executable(cg-packet-trade-player-list-request-test
    tests/packet/TradePlayerListRequestPacketTest.cpp
)
target_include_directories(cg-packet-trade-player-list-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-trade-player-list-request-test
    COMMAND cg-packet-trade-player-list-request-test
)

add_executable(cg-packet-trade-start-request-test
    tests/packet/TradeStartRequestPacketTest.cpp
)
target_include_directories(cg-packet-trade-start-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-trade-start-request-test
    COMMAND cg-packet-trade-start-request-test
)

add_executable(cg-packet-trade-confirmation-request-test
    tests/packet/TradeConfirmationRequestPacketTest.cpp
)
target_include_directories(cg-packet-trade-confirmation-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-trade-confirmation-request-test
    COMMAND cg-packet-trade-confirmation-request-test
)

add_executable(cg-packet-title-selection-request-test
    tests/packet/TitleSelectionRequestPacketTest.cpp
)
target_include_directories(cg-packet-title-selection-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-title-selection-request-test
    COMMAND cg-packet-title-selection-request-test
)

add_executable(cg-packet-title-deletion-request-test
    tests/packet/TitleDeletionRequestPacketTest.cpp
)
target_include_directories(cg-packet-title-deletion-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-title-deletion-request-test
    COMMAND cg-packet-title-deletion-request-test
)

add_executable(cg-packet-custom-title-input-request-test
    tests/packet/CustomTitleInputRequestPacketTest.cpp
)
target_include_directories(cg-packet-custom-title-input-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-custom-title-input-request-test
    COMMAND cg-packet-custom-title-input-request-test
)

add_executable(cg-packet-character-level-up-point-test
    tests/packet/CharacterLevelUpPointPacketTest.cpp
)
target_include_directories(cg-packet-character-level-up-point-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-character-level-up-point-test
    COMMAND cg-packet-character-level-up-point-test
)

add_executable(cg-packet-character-skill-swap-test
    tests/packet/CharacterSkillSwapPacketTest.cpp
)
target_include_directories(cg-packet-character-skill-swap-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-character-skill-swap-test
    COMMAND cg-packet-character-skill-swap-test
)

add_executable(cg-packet-character-delete-request-test
    tests/packet/CharacterDeleteRequestPacketTest.cpp
)
target_include_directories(cg-packet-character-delete-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-character-delete-request-test
    COMMAND cg-packet-character-delete-request-test
)

add_executable(cg-packet-version-upgrade-request-test
    tests/packet/VersionUpgradeRequestPacketTest.cpp
)
target_include_directories(cg-packet-version-upgrade-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-version-upgrade-request-test
    COMMAND cg-packet-version-upgrade-request-test
)

add_executable(cg-packet-trade-selection-cancel-request-test
    tests/packet/TradeSelectionCancelRequestPacketTest.cpp
)
target_include_directories(cg-packet-trade-selection-cancel-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-trade-selection-cancel-request-test
    COMMAND cg-packet-trade-selection-cancel-request-test
)

add_executable(cg-packet-battle-position-toggle-request-test
    tests/packet/BattlePositionToggleRequestPacketTest.cpp
)
target_include_directories(cg-packet-battle-position-toggle-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-battle-position-toggle-request-test
    COMMAND cg-packet-battle-position-toggle-request-test
)

add_executable(cg-packet-return-to-login-point-request-test
    tests/packet/ReturnToLoginPointRequestPacketTest.cpp
)
target_include_directories(cg-packet-return-to-login-point-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-return-to-login-point-request-test
    COMMAND cg-packet-return-to-login-point-request-test
)

add_executable(cg-packet-stall-end-request-test
    tests/packet/StallEndRequestPacketTest.cpp
)
target_include_directories(cg-packet-stall-end-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-stall-end-request-test
    COMMAND cg-packet-stall-end-request-test
)

add_executable(cg-packet-stall-browse-request-test
    tests/packet/StallBrowseRequestPacketTest.cpp
)
target_include_directories(cg-packet-stall-browse-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-stall-browse-request-test
    COMMAND cg-packet-stall-browse-request-test
)

add_executable(cg-packet-stall-buy-request-test
    tests/packet/StallBuyRequestPacketTest.cpp
)
target_include_directories(cg-packet-stall-buy-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-stall-buy-request-test
    COMMAND cg-packet-stall-buy-request-test
)

add_executable(cg-packet-produce-time-request-test
    tests/packet/ProduceTimeRequestPacketTest.cpp
)
target_include_directories(cg-packet-produce-time-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-produce-time-request-test
    COMMAND cg-packet-produce-time-request-test
)

add_executable(cg-packet-bank-page-request-test
    tests/packet/BankPageRequestPacketTest.cpp
)
target_include_directories(cg-packet-bank-page-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-bank-page-request-test
    COMMAND cg-packet-bank-page-request-test
)

add_executable(cg-packet-pet-item-move-request-test
    tests/packet/PetItemMoveRequestPacketTest.cpp
)
target_include_directories(cg-packet-pet-item-move-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-pet-item-move-request-test
    COMMAND cg-packet-pet-item-move-request-test
)

add_executable(cg-packet-bag-page-request-test
    tests/packet/BagPageRequestPacketTest.cpp
)
target_include_directories(cg-packet-bag-page-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-bag-page-request-test
    COMMAND cg-packet-bag-page-request-test
)

add_executable(cg-packet-bag-page-move-request-test
    tests/packet/BagPageMoveRequestPacketTest.cpp
)
target_include_directories(cg-packet-bag-page-move-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-bag-page-move-request-test
    COMMAND cg-packet-bag-page-move-request-test
)

add_executable(cg-packet-bag-page-move-ex-request-test
    tests/packet/BagPageMoveExRequestPacketTest.cpp
)
target_include_directories(cg-packet-bag-page-move-ex-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-bag-page-move-ex-request-test
    COMMAND cg-packet-bag-page-move-ex-request-test
)

add_executable(cg-packet-pfm-notification-test
    tests/packet/PfMNotificationPacketTest.cpp
)
target_include_directories(cg-packet-pfm-notification-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-pfm-notification-test
    COMMAND cg-packet-pfm-notification-test
)

add_executable(cg-packet-character-login-result-test
    tests/packet/CharacterLoginResultPacketTest.cpp
)
target_include_directories(cg-packet-character-login-result-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-character-login-result-test
    COMMAND cg-packet-character-login-result-test
)

add_executable(cg-packet-character-logout-result-test
    tests/packet/CharacterLogoutResultPacketTest.cpp
)
target_include_directories(cg-packet-character-logout-result-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-character-logout-result-test
    COMMAND cg-packet-character-logout-result-test
)

add_executable(cg-packet-client-index-test
    tests/packet/ClientIndexPacketTest.cpp
)
target_include_directories(cg-packet-client-index-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-client-index-test
    COMMAND cg-packet-client-index-test
)

add_executable(cg-packet-coordinate-update-test
    tests/packet/CoordinateUpdatePacketTest.cpp
)
target_include_directories(cg-packet-coordinate-update-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-coordinate-update-test
    COMMAND cg-packet-coordinate-update-test
)

add_executable(cg-packet-encounter-probability-test
    tests/packet/EncounterProbabilityPacketTest.cpp
)
target_include_directories(cg-packet-encounter-probability-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-encounter-probability-test
    COMMAND cg-packet-encounter-probability-test
)

add_executable(cg-packet-player-status-flags-test
    tests/packet/PlayerStatusFlagsPacketTest.cpp
)
target_include_directories(cg-packet-player-status-flags-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-player-status-flags-test
    COMMAND cg-packet-player-status-flags-test
)

add_executable(cg-packet-map-checksum-test
    tests/packet/MapChecksumPacketTest.cpp
)
target_include_directories(cg-packet-map-checksum-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-map-checksum-test
    COMMAND cg-packet-map-checksum-test
)

add_executable(cg-packet-fever-status-test
    tests/packet/FeverStatusPacketTest.cpp
)
target_include_directories(cg-packet-fever-status-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-fever-status-test
    COMMAND cg-packet-fever-status-test
)

add_executable(cg-packet-battle-position-test
    tests/packet/BattlePositionPacketTest.cpp
)
target_include_directories(cg-packet-battle-position-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-battle-position-test
    COMMAND cg-packet-battle-position-test
)

add_executable(cg-packet-player-position-sync-test
    tests/packet/PlayerPositionSyncPacketTest.cpp
)
target_include_directories(cg-packet-player-position-sync-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-player-position-sync-test
    COMMAND cg-packet-player-position-sync-test
)

add_executable(cg-packet-client-ip-notification-test
    tests/packet/ClientIpNotificationPacketTest.cpp
)
target_include_directories(cg-packet-client-ip-notification-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-client-ip-notification-test
    COMMAND cg-packet-client-ip-notification-test
)

add_executable(cg-packet-macro-check-config-test
    tests/packet/MacroCheckConfigPacketTest.cpp
)
target_include_directories(cg-packet-macro-check-config-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
add_test(
    NAME cg-packet-macro-check-config-test
    COMMAND cg-packet-macro-check-config-test
)

add_executable(cg-packet-create-new-char-result-test
    tests/packet/CreateNewCharResultPacketTest.cpp
)
target_include_directories(cg-packet-create-new-char-result-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(NAME cg-packet-create-new-char-result-test COMMAND cg-packet-create-new-char-result-test)

add_executable(cg-packet-create-new-char-send-test
    tests/packet/CreateNewCharSendPacketTest.cpp
)
target_include_directories(cg-packet-create-new-char-send-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(NAME cg-packet-create-new-char-send-test COMMAND cg-packet-create-new-char-send-test)

add_executable(cg-packet-character-delete-result-test
    tests/packet/CharacterDeleteResultPacketTest.cpp
)
target_include_directories(cg-packet-character-delete-result-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(NAME cg-packet-character-delete-result-test COMMAND cg-packet-character-delete-result-test)

add_executable(cg-packet-process-status-test
    tests/packet/ProcessStatusPacketTest.cpp
)
target_include_directories(cg-packet-process-status-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(NAME cg-packet-process-status-test COMMAND cg-packet-process-status-test)

add_executable(cg-packet-version-upgrade-result-test
    tests/packet/VersionUpgradeResultPacketTest.cpp
)
target_include_directories(cg-packet-version-upgrade-result-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(
    NAME cg-packet-version-upgrade-result-test
    COMMAND cg-packet-version-upgrade-result-test
)

add_executable(cg-packet-stall-start-result-test
    tests/packet/StallStartResultPacketTest.cpp
)
target_include_directories(cg-packet-stall-start-result-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(
    NAME cg-packet-stall-start-result-test
    COMMAND cg-packet-stall-start-result-test
)

add_executable(cg-packet-stall-end-result-test
    tests/packet/StallEndResultPacketTest.cpp
)
target_include_directories(cg-packet-stall-end-result-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(
    NAME cg-packet-stall-end-result-test
    COMMAND cg-packet-stall-end-result-test
)

add_executable(cg-packet-stall-buy-result-test
    tests/packet/StallBuyResultPacketTest.cpp
)
target_include_directories(cg-packet-stall-buy-result-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(
    NAME cg-packet-stall-buy-result-test
    COMMAND cg-packet-stall-buy-result-test
)

add_executable(cg-packet-pet-item-swap-test
    tests/packet/PetItemSwapPacketTest.cpp
)
target_include_directories(cg-packet-pet-item-swap-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(
    NAME cg-packet-pet-item-swap-test
    COMMAND cg-packet-pet-item-swap-test
)

add_executable(cg-packet-drop-item-test
    tests/packet/DropItemPacketTest.cpp
)
target_include_directories(cg-packet-drop-item-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(
    NAME cg-packet-drop-item-test
    COMMAND cg-packet-drop-item-test
)

add_executable(cg-packet-cheat-detection-report-test
    tests/packet/CheatDetectionReportPacketTest.cpp
)
target_include_directories(cg-packet-cheat-detection-report-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(
    NAME cg-packet-cheat-detection-report-test
    COMMAND cg-packet-cheat-detection-report-test
)

add_executable(cg-packet-int62-codec-test
    tests/packet/Int62CodecTest.cpp
)

target_include_directories(cg-packet-int62-codec-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-int62-codec-test COMMAND cg-packet-int62-codec-test)

add_executable(cg-packet-es-string-codec-test
    tests/packet/EsStringCodecTest.cpp
)

target_include_directories(cg-packet-es-string-codec-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-es-string-codec-test COMMAND cg-packet-es-string-codec-test)

add_executable(cg-packet-token-codec-test
    tests/packet/TokenCodecTest.cpp
)

target_include_directories(cg-packet-token-codec-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-token-codec-test COMMAND cg-packet-token-codec-test)

add_executable(cg-packet-encode64-codec-test
    tests/packet/Encode64CodecTest.cpp
)

target_include_directories(cg-packet-encode64-codec-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-encode64-codec-test COMMAND cg-packet-encode64-codec-test)

add_executable(cg-packet-parser-test
    tests/packet/PacketParserTest.cpp
)

target_include_directories(cg-packet-parser-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-parser-test COMMAND cg-packet-parser-test)

add_executable(cg-packet-builder-test
    tests/packet/PacketBuilderTest.cpp
)

target_include_directories(cg-packet-builder-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-builder-test COMMAND cg-packet-builder-test)

add_executable(cg-packet-map-name-test
    tests/packet/MapNamePacketTest.cpp
)

target_include_directories(cg-packet-map-name-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-map-name-test COMMAND cg-packet-map-name-test)

add_executable(cg-packet-object-list-test
    tests/packet/ObjectListPacketTest.cpp
)

target_include_directories(cg-packet-object-list-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-object-list-test COMMAND cg-packet-object-list-test)

add_executable(cg-packet-address-book-test
    tests/packet/AddressBookPacketTest.cpp
)

target_include_directories(cg-packet-address-book-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-address-book-test COMMAND cg-packet-address-book-test)

add_executable(cg-packet-address-book-guild-info-test
    tests/packet/AddressBookGuildInfoPacketTest.cpp
)

target_include_directories(cg-packet-address-book-guild-info-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-address-book-guild-info-test COMMAND cg-packet-address-book-guild-info-test)

add_executable(cg-packet-guild-info-test
    tests/packet/GuildInfoPacketTest.cpp
)

target_include_directories(cg-packet-guild-info-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-guild-info-test COMMAND cg-packet-guild-info-test)

add_executable(cg-packet-guild-title-test
    tests/packet/GuildTitlePacketTest.cpp
)

target_include_directories(cg-packet-guild-title-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-guild-title-test COMMAND cg-packet-guild-title-test)

add_executable(cg-packet-guild-member-test
    tests/packet/GuildMemberPacketTest.cpp
)

target_include_directories(cg-packet-guild-member-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-guild-member-test COMMAND cg-packet-guild-member-test)

add_executable(cg-packet-guild-member-info-test
    tests/packet/GuildMemberInfoPacketTest.cpp
)

target_include_directories(cg-packet-guild-member-info-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-guild-member-info-test COMMAND cg-packet-guild-member-info-test)

add_executable(cg-packet-guild-mail-test
    tests/packet/GuildMailPacketTest.cpp
)

target_include_directories(cg-packet-guild-mail-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-guild-mail-test COMMAND cg-packet-guild-mail-test)

add_executable(cg-packet-trade-player-list-test
    tests/packet/TradePlayerListPacketTest.cpp
)

target_include_directories(cg-packet-trade-player-list-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-trade-player-list-test COMMAND cg-packet-trade-player-list-test)

add_executable(cg-packet-trade-start-test
    tests/packet/TradeStartPacketTest.cpp
)

target_include_directories(cg-packet-trade-start-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-trade-start-test COMMAND cg-packet-trade-start-test)

add_executable(cg-packet-trade-item-info-test
    tests/packet/TradeItemInfoPacketTest.cpp
)

target_include_directories(cg-packet-trade-item-info-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-trade-item-info-test COMMAND cg-packet-trade-item-info-test)

add_executable(cg-packet-trade-pet-info-test
    tests/packet/TradePetInfoPacketTest.cpp
)

target_include_directories(cg-packet-trade-pet-info-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-trade-pet-info-test COMMAND cg-packet-trade-pet-info-test)

add_executable(cg-packet-trade-pet-skill-info-test
    tests/packet/TradePetSkillInfoPacketTest.cpp
)

target_include_directories(cg-packet-trade-pet-skill-info-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-trade-pet-skill-info-test COMMAND cg-packet-trade-pet-skill-info-test)

add_executable(cg-packet-drop-gold-test
    tests/packet/DropGoldPacketTest.cpp
)

target_include_directories(cg-packet-drop-gold-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-drop-gold-test COMMAND cg-packet-drop-gold-test)

add_executable(cg-packet-drop-pet-test
    tests/packet/DropPetPacketTest.cpp
)

target_include_directories(cg-packet-drop-pet-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-drop-pet-test COMMAND cg-packet-drop-pet-test)

add_executable(cg-packet-item-move-request-test
    tests/packet/ItemMoveRequestPacketTest.cpp
)

target_include_directories(cg-packet-item-move-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-item-move-request-test COMMAND cg-packet-item-move-request-test)

add_executable(cg-packet-item-recipe-request-test
    tests/packet/ItemRecipeRequestPacketTest.cpp
)

target_include_directories(cg-packet-item-recipe-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-item-recipe-request-test COMMAND cg-packet-item-recipe-request-test)

add_executable(cg-packet-album-info-request-test
    tests/packet/AlbumInfoRequestPacketTest.cpp
)

target_include_directories(cg-packet-album-info-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-album-info-request-test COMMAND cg-packet-album-info-request-test)

add_executable(cg-packet-album-name-request-test
    tests/packet/AlbumNameRequestPacketTest.cpp
)

target_include_directories(cg-packet-album-name-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-album-name-request-test COMMAND cg-packet-album-name-request-test)

add_executable(cg-packet-address-book-delete-request-test
    tests/packet/AddressBookDeleteRequestPacketTest.cpp
)

target_include_directories(cg-packet-address-book-delete-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-address-book-delete-request-test
    COMMAND cg-packet-address-book-delete-request-test
)

add_executable(cg-packet-address-book-exchange-request-test
    tests/packet/AddressBookExchangeRequestPacketTest.cpp
)

target_include_directories(cg-packet-address-book-exchange-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-address-book-exchange-request-test
    COMMAND cg-packet-address-book-exchange-request-test
)

add_executable(cg-packet-guild-info-change-test
    tests/packet/GuildInfoChangePacketTest.cpp
)

target_include_directories(cg-packet-guild-info-change-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-guild-info-change-test COMMAND cg-packet-guild-info-change-test)

add_executable(cg-packet-guild-title-set-test
    tests/packet/GuildTitleSetPacketTest.cpp
)

target_include_directories(cg-packet-guild-title-set-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-guild-title-set-test COMMAND cg-packet-guild-title-set-test)

add_executable(cg-packet-guild-member-list-request-test
    tests/packet/GuildMemberListRequestPacketTest.cpp
)

target_include_directories(cg-packet-guild-member-list-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-guild-member-list-request-test
    COMMAND cg-packet-guild-member-list-request-test
)

add_executable(cg-packet-guild-member-title-change-request-test
    tests/packet/GuildMemberTitleChangeRequestPacketTest.cpp
)

target_include_directories(cg-packet-guild-member-title-change-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-guild-member-title-change-request-test
    COMMAND cg-packet-guild-member-title-change-request-test
)

add_executable(cg-packet-guild-member-invite-request-test
    tests/packet/GuildMemberInviteRequestPacketTest.cpp
)

target_include_directories(cg-packet-guild-member-invite-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-guild-member-invite-request-test
    COMMAND cg-packet-guild-member-invite-request-test
)

add_executable(cg-packet-guild-leave-request-test
    tests/packet/GuildLeaveRequestPacketTest.cpp
)

target_include_directories(cg-packet-guild-leave-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-guild-leave-request-test
    COMMAND cg-packet-guild-leave-request-test
)

add_executable(cg-packet-guild-member-remove-request-test
    tests/packet/GuildMemberRemoveRequestPacketTest.cpp
)

target_include_directories(cg-packet-guild-member-remove-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-guild-member-remove-request-test
    COMMAND cg-packet-guild-member-remove-request-test
)

add_executable(cg-packet-profile-lists-request-test
    tests/packet/ProfileListsRequestPacketTest.cpp
)

target_include_directories(cg-packet-profile-lists-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-profile-lists-request-test
    COMMAND cg-packet-profile-lists-request-test
)

add_executable(cg-packet-profile-data-request-test
    tests/packet/ProfileDataRequestPacketTest.cpp
)

target_include_directories(cg-packet-profile-data-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-profile-data-request-test
    COMMAND cg-packet-profile-data-request-test
)

add_executable(cg-packet-player-status-flags-update-request-test
    tests/packet/PlayerStatusFlagsUpdateRequestPacketTest.cpp
)

target_include_directories(cg-packet-player-status-flags-update-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-player-status-flags-update-request-test
    COMMAND cg-packet-player-status-flags-update-request-test
)

add_executable(cg-packet-party-membership-request-test
    tests/packet/PartyMembershipRequestPacketTest.cpp
)

target_include_directories(cg-packet-party-membership-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-party-membership-request-test
    COMMAND cg-packet-party-membership-request-test
)

add_executable(cg-packet-pet-battle-status-update-request-test
    tests/packet/PetBattleStatusUpdateRequestPacketTest.cpp
)

target_include_directories(cg-packet-pet-battle-status-update-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-pet-battle-status-update-request-test
    COMMAND cg-packet-pet-battle-status-update-request-test
)

add_executable(cg-packet-pet-move-request-test
    tests/packet/PetMoveRequestPacketTest.cpp
)

target_include_directories(cg-packet-pet-move-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-pet-move-request-test
    COMMAND cg-packet-pet-move-request-test
)

add_executable(cg-packet-skill-front-char-list-request-test
    tests/packet/SkillFrontCharListRequestPacketTest.cpp
)

target_include_directories(cg-packet-skill-front-char-list-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-skill-front-char-list-request-test
    COMMAND cg-packet-skill-front-char-list-request-test
)

add_executable(cg-packet-skill-character-pet-list-request-test
    tests/packet/SkillCharacterPetListRequestPacketTest.cpp
)

target_include_directories(cg-packet-skill-character-pet-list-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-skill-character-pet-list-request-test
    COMMAND cg-packet-skill-character-pet-list-request-test
)

add_executable(cg-packet-item-character-pet-list-request-test
    tests/packet/ItemCharacterPetListRequestPacketTest.cpp
)

target_include_directories(cg-packet-item-character-pet-list-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-item-character-pet-list-request-test
    COMMAND cg-packet-item-character-pet-list-request-test
)

add_executable(cg-packet-item-stamp-request-test
    tests/packet/ItemStampRequestPacketTest.cpp
)

target_include_directories(cg-packet-item-stamp-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-item-stamp-request-test
    COMMAND cg-packet-item-stamp-request-test
)

add_executable(cg-packet-character-action-request-test
    tests/packet/CharacterActionRequestPacketTest.cpp
)

target_include_directories(cg-packet-character-action-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-character-action-request-test
    COMMAND cg-packet-character-action-request-test
)

add_executable(cg-packet-action-skill-icon-request-test
    tests/packet/ActionSkillIconRequestPacketTest.cpp
)

target_include_directories(cg-packet-action-skill-icon-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-action-skill-icon-request-test
    COMMAND cg-packet-action-skill-icon-request-test
)

add_executable(cg-packet-tech-use-request-test
    tests/packet/TechUseRequestPacketTest.cpp
)

target_include_directories(cg-packet-tech-use-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-tech-use-request-test
    COMMAND cg-packet-tech-use-request-test
)

add_executable(cg-packet-guild-mail-send-test
    tests/packet/GuildMailSendPacketTest.cpp
)

target_include_directories(cg-packet-guild-mail-send-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-guild-mail-send-test COMMAND cg-packet-guild-mail-send-test)

add_executable(cg-packet-profile-mail-friend-delete-test
    tests/packet/ProfileMailFriendDeletePacketTest.cpp
)

target_include_directories(cg-packet-profile-mail-friend-delete-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-mail-friend-delete-test COMMAND cg-packet-profile-mail-friend-delete-test)

add_executable(cg-packet-profile-mini-mail-send-test
    tests/packet/ProfileMiniMailSendPacketTest.cpp
)

target_include_directories(cg-packet-profile-mini-mail-send-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-mini-mail-send-test COMMAND cg-packet-profile-mini-mail-send-test)

add_executable(cg-packet-trade-take-out-test
    tests/packet/TradeTakeOutPacketTest.cpp
)

target_include_directories(cg-packet-trade-take-out-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-trade-take-out-test COMMAND cg-packet-trade-take-out-test)

add_executable(cg-packet-stall-start-test
    tests/packet/StallStartPacketTest.cpp
)

target_include_directories(cg-packet-stall-start-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-stall-start-test COMMAND cg-packet-stall-start-test)

add_executable(cg-packet-character-icon-test
    tests/packet/CharacterIconPacketTest.cpp
)

target_include_directories(cg-packet-character-icon-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-character-icon-test COMMAND cg-packet-character-icon-test)

add_executable(cg-packet-object-delete-test
    tests/packet/ObjectDeletePacketTest.cpp
)

target_include_directories(cg-packet-object-delete-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-object-delete-test COMMAND cg-packet-object-delete-test)

add_executable(cg-packet-map-data-test
    tests/packet/MapDataPacketTest.cpp
)

target_include_directories(cg-packet-map-data-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-map-data-test COMMAND cg-packet-map-data-test)

add_executable(cg-packet-map-data-request-test
    tests/packet/MapDataRequestPacketTest.cpp
)

target_include_directories(cg-packet-map-data-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-map-data-request-test COMMAND cg-packet-map-data-request-test)

add_executable(cg-packet-profile-version-test
    tests/packet/ProfileVersionPacketTest.cpp
)

target_include_directories(cg-packet-profile-version-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-version-test COMMAND cg-packet-profile-version-test)

add_executable(cg-packet-character-profile-test
    tests/packet/CharacterProfilePacketTest.cpp
)

target_include_directories(cg-packet-character-profile-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-character-profile-test COMMAND cg-packet-character-profile-test)

add_executable(cg-packet-char-data2-test
    tests/packet/CharData2PacketTest.cpp
)

target_include_directories(cg-packet-char-data2-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-char-data2-test COMMAND cg-packet-char-data2-test)

add_executable(cg-packet-job-name-test
    tests/packet/JobNamePacketTest.cpp
)

target_include_directories(cg-packet-job-name-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-job-name-test COMMAND cg-packet-job-name-test)

add_executable(cg-packet-title-test
    tests/packet/TitlePacketTest.cpp
)

target_include_directories(cg-packet-title-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-title-test COMMAND cg-packet-title-test)

add_executable(cg-packet-profile-list-test
    tests/packet/ProfileListPacketTest.cpp
)

target_include_directories(cg-packet-profile-list-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-list-test COMMAND cg-packet-profile-list-test)

add_executable(cg-packet-about-list-test
    tests/packet/AboutListPacketTest.cpp
)

target_include_directories(cg-packet-about-list-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-about-list-test COMMAND cg-packet-about-list-test)

add_executable(cg-packet-profile-data-test
    tests/packet/ProfileDataPacketTest.cpp
)

target_include_directories(cg-packet-profile-data-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-data-test COMMAND cg-packet-profile-data-test)

add_executable(cg-packet-profile-set-data-test
    tests/packet/ProfileSetDataPacketTest.cpp
)

target_include_directories(cg-packet-profile-set-data-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-set-data-test COMMAND cg-packet-profile-set-data-test)

add_executable(cg-packet-magic-use-request-test
    tests/packet/MagicUseRequestPacketTest.cpp
)

target_include_directories(cg-packet-magic-use-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-magic-use-request-test COMMAND cg-packet-magic-use-request-test)

add_executable(cg-packet-pet-skill-use-request-test
    tests/packet/PetSkillUseRequestPacketTest.cpp
)

target_include_directories(cg-packet-pet-skill-use-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-pet-skill-use-request-test COMMAND cg-packet-pet-skill-use-request-test)

add_executable(cg-packet-pet-level-up-point-test
    tests/packet/PetLevelUpPointPacketTest.cpp
)

target_include_directories(cg-packet-pet-level-up-point-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-pet-level-up-point-test COMMAND cg-packet-pet-level-up-point-test)

add_executable(cg-packet-pet-skill-swap-test
    tests/packet/PetSkillSwapPacketTest.cpp
)

target_include_directories(cg-packet-pet-skill-swap-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-pet-skill-swap-test COMMAND cg-packet-pet-skill-swap-test)

add_executable(cg-packet-user-pet-name-input-test
    tests/packet/UserPetNameInputPacketTest.cpp
)

target_include_directories(cg-packet-user-pet-name-input-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-user-pet-name-input-test COMMAND cg-packet-user-pet-name-input-test)

add_executable(cg-packet-character-skill-data-test
    tests/packet/CharacterSkillDataPacketTest.cpp
)

target_include_directories(cg-packet-character-skill-data-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-character-skill-data-test COMMAND cg-packet-character-skill-data-test)

add_executable(cg-packet-item-data-test
    tests/packet/ItemDataPacketTest.cpp
)

target_include_directories(cg-packet-item-data-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-item-data-test COMMAND cg-packet-item-data-test)

add_executable(cg-packet-pet-item-data-test
    tests/packet/PetItemDataPacketTest.cpp
)

target_include_directories(cg-packet-pet-item-data-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-pet-item-data-test COMMAND cg-packet-pet-item-data-test)

add_executable(cg-packet-equip-suit-test
    tests/packet/EquipSuitPacketTest.cpp
)

target_include_directories(cg-packet-equip-suit-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-equip-suit-test COMMAND cg-packet-equip-suit-test)

add_executable(cg-packet-item-info-list-test
    tests/packet/ItemInfoListPacketTest.cpp
)

target_include_directories(cg-packet-item-info-list-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-item-info-list-test COMMAND cg-packet-item-info-list-test)

add_executable(cg-packet-item-recipe-test
    tests/packet/ItemRecipePacketTest.cpp
)

target_include_directories(cg-packet-item-recipe-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-item-recipe-test COMMAND cg-packet-item-recipe-test)

add_executable(cg-packet-album-flags-test
    tests/packet/AlbumFlagsPacketTest.cpp
)

target_include_directories(cg-packet-album-flags-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-album-flags-test COMMAND cg-packet-album-flags-test)

add_executable(cg-packet-album-info-test
    tests/packet/AlbumInfoPacketTest.cpp
)

target_include_directories(cg-packet-album-info-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-album-info-test COMMAND cg-packet-album-info-test)

add_executable(cg-packet-album-name-test
    tests/packet/AlbumNamePacketTest.cpp
)

target_include_directories(cg-packet-album-name-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-album-name-test COMMAND cg-packet-album-name-test)

add_executable(cg-packet-pet-mail-animation-test
    tests/packet/PetMailAnimationPacketTest.cpp
)

target_include_directories(cg-packet-pet-mail-animation-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-pet-mail-animation-test COMMAND cg-packet-pet-mail-animation-test)

add_executable(cg-packet-mail-message-send-test
    tests/packet/MailMessageSendPacketTest.cpp
)

target_include_directories(cg-packet-mail-message-send-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-mail-message-send-test COMMAND cg-packet-mail-message-send-test)

add_executable(cg-packet-chat-message-send-test
    tests/packet/ChatMessageSendPacketTest.cpp
)

target_include_directories(cg-packet-chat-message-send-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-chat-message-send-test COMMAND cg-packet-chat-message-send-test)

add_executable(cg-packet-window-open-send-test
    tests/packet/WindowOpenSendPacketTest.cpp
)

target_include_directories(cg-packet-window-open-send-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-window-open-send-test COMMAND cg-packet-window-open-send-test)

add_executable(cg-packet-shutdown-request-test
    tests/packet/ShutdownRequestPacketTest.cpp
)

target_include_directories(cg-packet-shutdown-request-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-shutdown-request-test COMMAND cg-packet-shutdown-request-test)

add_executable(cg-packet-pet-mail-send-test
    tests/packet/PetMailSendPacketTest.cpp
)

target_include_directories(cg-packet-pet-mail-send-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-pet-mail-send-test COMMAND cg-packet-pet-mail-send-test)

add_executable(cg-packet-pet-mail-register-test
    tests/packet/PetMailRegisterPacketTest.cpp
)

target_include_directories(cg-packet-pet-mail-register-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-pet-mail-register-test COMMAND cg-packet-pet-mail-register-test)

add_executable(cg-packet-profile-mail-delete-test
    tests/packet/ProfileMailDeletePacketTest.cpp
)

target_include_directories(cg-packet-profile-mail-delete-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-mail-delete-test COMMAND cg-packet-profile-mail-delete-test)

add_executable(cg-packet-profile-mail-receipt-test
    tests/packet/ProfileMailReceiptPacketTest.cpp
)

target_include_directories(cg-packet-profile-mail-receipt-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-mail-receipt-test COMMAND cg-packet-profile-mail-receipt-test)

add_executable(cg-packet-profile-mail-arrive-test
    tests/packet/ProfileMailArrivePacketTest.cpp
)

target_include_directories(cg-packet-profile-mail-arrive-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-mail-arrive-test COMMAND cg-packet-profile-mail-arrive-test)

add_executable(cg-packet-party-param-test
    tests/packet/PartyParamPacketTest.cpp
)

target_include_directories(cg-packet-party-param-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-party-param-test COMMAND cg-packet-party-param-test)

add_executable(cg-packet-party-member-status-test
    tests/packet/PartyMemberStatusPacketTest.cpp
)

target_include_directories(cg-packet-party-member-status-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-party-member-status-test COMMAND cg-packet-party-member-status-test)

add_executable(cg-packet-tech-list-test
    tests/packet/TechListPacketTest.cpp
)

target_include_directories(cg-packet-tech-list-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-tech-list-test COMMAND cg-packet-tech-list-test)

add_executable(cg-packet-pet-tech-data-test
    tests/packet/PetTechDataPacketTest.cpp
)

target_include_directories(cg-packet-pet-tech-data-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-pet-tech-data-test COMMAND cg-packet-pet-tech-data-test)

add_executable(cg-packet-ride-pet-data-test
    tests/packet/RidePetDataPacketTest.cpp
)

target_include_directories(cg-packet-ride-pet-data-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-ride-pet-data-test COMMAND cg-packet-ride-pet-data-test)

add_executable(cg-packet-ride-pet-extra-data-test
    tests/packet/RidePetExtraDataPacketTest.cpp
)

target_include_directories(cg-packet-ride-pet-extra-data-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-ride-pet-extra-data-test COMMAND cg-packet-ride-pet-extra-data-test)

add_executable(cg-packet-tech-use-result-test
    tests/packet/TechUseResultPacketTest.cpp
)

target_include_directories(cg-packet-tech-use-result-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-tech-use-result-test COMMAND cg-packet-tech-use-result-test)

add_executable(cg-packet-profile-status-test
    tests/packet/ProfileStatusPacketTest.cpp
)

target_include_directories(cg-packet-profile-status-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-status-test COMMAND cg-packet-profile-status-test)

add_executable(cg-packet-front-char-list-test
    tests/packet/FrontCharListPacketTest.cpp
)

target_include_directories(cg-packet-front-char-list-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-front-char-list-test COMMAND cg-packet-front-char-list-test)

add_executable(cg-packet-item-front-char-list-test
    tests/packet/ItemFrontCharListPacketTest.cpp
)

target_include_directories(cg-packet-item-front-char-list-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-item-front-char-list-test COMMAND cg-packet-item-front-char-list-test)

add_executable(cg-packet-character-pet-list-test
    tests/packet/CharacterPetListPacketTest.cpp
)

target_include_directories(cg-packet-character-pet-list-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-character-pet-list-test COMMAND cg-packet-character-pet-list-test)

add_executable(cg-packet-char-and-pet-item-list-test
    tests/packet/CharAndPetItemListPacketTest.cpp
)

target_include_directories(cg-packet-char-and-pet-item-list-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-char-and-pet-item-list-test COMMAND cg-packet-char-and-pet-item-list-test)

add_executable(cg-packet-profile-window-test
    tests/packet/ProfileWindowPacketTest.cpp
)

target_include_directories(cg-packet-profile-window-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-window-test COMMAND cg-packet-profile-window-test)

add_executable(cg-packet-system-message-test
    tests/packet/SystemMessagePacketTest.cpp
)

target_include_directories(cg-packet-system-message-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-system-message-test COMMAND cg-packet-system-message-test)

add_executable(cg-packet-effect-test
    tests/packet/EffectPacketTest.cpp
)

target_include_directories(cg-packet-effect-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-effect-test COMMAND cg-packet-effect-test)

add_executable(cg-packet-battle-reward-test
    tests/packet/BattleRewardPacketTest.cpp
)

target_include_directories(cg-packet-battle-reward-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-battle-reward-test COMMAND cg-packet-battle-reward-test)

add_executable(cg-packet-duel-point-test
    tests/packet/DuelPointPacketTest.cpp
)

target_include_directories(cg-packet-duel-point-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-duel-point-test COMMAND cg-packet-duel-point-test)

add_executable(cg-packet-battle-command-test
    tests/packet/BattleCommandPacketTest.cpp
)

target_include_directories(cg-packet-battle-command-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-battle-command-test COMMAND cg-packet-battle-command-test)

add_executable(cg-packet-profile-mail-message-test
    tests/packet/ProfileMailMessagePacketTest.cpp
)

target_include_directories(cg-packet-profile-mail-message-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-profile-mail-message-test COMMAND cg-packet-profile-mail-message-test)

add_executable(cg-packet-chat-message-test
    tests/packet/ChatMessagePacketTest.cpp
)

target_include_directories(cg-packet-chat-message-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-chat-message-test COMMAND cg-packet-chat-message-test)

add_executable(cg-packet-window-packet-test
    tests/packet/WindowPacketTest.cpp
)

target_include_directories(cg-packet-window-packet-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-window-packet-test COMMAND cg-packet-window-packet-test)

add_executable(cg-packet-stall-browse-test
    tests/packet/StallBrowsePacketTest.cpp
)

target_include_directories(cg-packet-stall-browse-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-stall-browse-test COMMAND cg-packet-stall-browse-test)

add_executable(cg-packet-payload-codec-test
    tests/packet/PayloadCodecTest.cpp
)

target_include_directories(cg-packet-payload-codec-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-payload-codec-test COMMAND cg-packet-payload-codec-test)

add_executable(cg-packet-coverage-schema-test
    tests/packet/PacketCoverageSchemaTest.cpp
)

target_include_directories(cg-packet-coverage-schema-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(
    NAME cg-packet-coverage-schema-test
    COMMAND cg-packet-coverage-schema-test
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/protocol/schema/packet_coverage_schema.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/protocol/plain"
)

add_executable(cg-protocol-production-surface-smoke
    tests/packet/ProductionSurfaceSmoke.cpp
)

target_include_directories(cg-protocol-production-surface-smoke
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_definitions(cg-protocol-production-surface-smoke
    PRIVATE
        CG_PACKET_ENABLE_DIAGNOSTIC_TEXT=0
        CG_PACKET_ENABLE_TEST_HELPERS=0
)

if(MSVC)
    target_compile_options(cg-protocol-production-surface-smoke
        PRIVATE
            /W4
            /utf-8
            /permissive-
            /GR-
    )
else()
    target_compile_options(cg-protocol-production-surface-smoke
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -fno-rtti
    )
endif()

add_test(
    NAME cg-protocol-production-surface-smoke
    COMMAND cg-protocol-production-surface-smoke
)

get_property(cg_targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
foreach(cg_target IN LISTS cg_targets)
    if(cg_target MATCHES "^cg-packet-.*-test$")
        target_compile_definitions(${cg_target} PRIVATE
            CG_PACKET_ENABLE_DIAGNOSTIC_TEXT=1
            CG_PACKET_ENABLE_TEST_HELPERS=1)
    endif()
endforeach()

if(MSVC)
    target_compile_options(cg-client PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-map-viewer PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-asset-viewer PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-anim-viewer PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-anim-exporter PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-automap-renderer-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-resource-graphic-decoder-transparency-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-ida-render-rules-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-map-tile-atlas-cache-key-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-map-object-texture-cache-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-map-object-sprite-builder-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-scene-ground-atlas-cache-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-scene-ground-sprite-builder-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-visibility-culling-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-render-order-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-world-renderer-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-synthetic-population-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-entity-store-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-transform-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-movement-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-animation-state-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-movement-animation-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-world-entity-sprite-builder-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-game-state-machine-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-scene-manager-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-ui-presenter-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-app-scene-client-loop-bridge-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-types-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-logout-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-login-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-status-update-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-echo-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-echo-response-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-keep-alive-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-move-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-party-leave-position-sync-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-item-use-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-field-recovery-item-use-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pick-up-item-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-status-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-dissolve-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-bag-page-manager-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-join-notification-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-npc-event-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-encounter-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-duel-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-battle-join-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-battle-watch-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-battle-exit-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-battle-watch-stop-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-npc-event-result-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-battle-entry-notification-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-battle-command-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-item-front-char-list-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-player-list-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-start-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-confirmation-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-title-selection-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-title-deletion-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-custom-title-input-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-level-up-point-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-skill-swap-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-delete-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-version-upgrade-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-selection-cancel-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-battle-position-toggle-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-return-to-login-point-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-stall-end-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-stall-browse-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-stall-buy-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-produce-time-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-bank-page-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-item-move-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-bag-page-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-bag-page-move-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-bag-page-move-ex-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pfm-notification-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-login-result-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-logout-result-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-client-index-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-coordinate-update-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-encounter-probability-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-player-status-flags-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-map-checksum-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-fever-status-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-battle-position-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-player-position-sync-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-client-ip-notification-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-macro-check-config-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-create-new-char-result-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-create-new-char-send-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-delete-result-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-process-status-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-version-upgrade-result-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-stall-start-result-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-stall-end-result-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-stall-buy-result-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-item-swap-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-drop-item-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-cheat-detection-report-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-int62-codec-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-es-string-codec-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-token-codec-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-encode64-codec-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-parser-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-builder-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-map-name-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-object-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-address-book-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-address-book-guild-info-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-info-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-title-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-member-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-member-info-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-mail-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-player-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-start-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-item-info-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-pet-info-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-pet-skill-info-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-drop-gold-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-drop-pet-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-item-move-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-item-recipe-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-album-info-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-album-name-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-address-book-delete-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-address-book-exchange-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-info-change-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-title-set-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-member-list-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-member-title-change-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-member-invite-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-leave-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-member-remove-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-lists-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-data-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-player-status-flags-update-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-party-membership-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-battle-status-update-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-move-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-skill-front-char-list-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-skill-character-pet-list-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-item-character-pet-list-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-item-stamp-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-action-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-action-skill-icon-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-tech-use-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-guild-mail-send-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-mail-friend-delete-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-mini-mail-send-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-trade-take-out-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-stall-start-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-icon-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-object-delete-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-map-data-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-map-data-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-version-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-profile-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-char-data2-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-job-name-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-title-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-about-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-data-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-set-data-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-magic-use-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-skill-use-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-level-up-point-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-skill-swap-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-user-pet-name-input-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-skill-data-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-item-data-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-item-data-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-equip-suit-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-item-info-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-item-recipe-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-album-flags-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-album-info-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-album-name-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-mail-animation-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-mail-message-send-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-chat-message-send-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-window-open-send-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-shutdown-request-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-mail-send-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-mail-register-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-mail-delete-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-mail-receipt-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-mail-arrive-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-party-param-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-party-member-status-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-tech-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-pet-tech-data-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-ride-pet-data-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-ride-pet-extra-data-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-tech-use-result-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-status-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-front-char-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-item-front-char-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-character-pet-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-char-and-pet-item-list-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-window-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-system-message-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-effect-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-battle-reward-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-duel-point-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-battle-command-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-profile-mail-message-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-chat-message-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-window-packet-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-stall-browse-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-payload-codec-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-coverage-schema-test PRIVATE /W4 /utf-8 /permissive-)
else()
    target_compile_options(cg-client PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-map-viewer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-asset-viewer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-anim-viewer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-anim-exporter PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-automap-renderer-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-resource-graphic-decoder-transparency-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-ida-render-rules-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-map-tile-atlas-cache-key-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-map-object-texture-cache-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-map-object-sprite-builder-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-scene-ground-atlas-cache-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-scene-ground-sprite-builder-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-visibility-culling-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-render-order-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-world-renderer-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-synthetic-population-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-entity-store-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-transform-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-movement-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-animation-state-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-movement-animation-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-world-entity-sprite-builder-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-game-state-machine-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-scene-manager-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-ui-presenter-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-app-scene-client-loop-bridge-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-types-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-logout-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-login-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-status-update-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-echo-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-echo-response-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-keep-alive-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-move-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-party-leave-position-sync-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-item-use-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-field-recovery-item-use-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pick-up-item-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-trade-status-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-dissolve-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-bag-page-manager-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-join-notification-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-npc-event-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-encounter-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-duel-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-battle-join-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-battle-watch-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-battle-exit-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-battle-watch-stop-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-npc-event-result-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-battle-entry-notification-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-battle-command-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-item-front-char-list-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-trade-player-list-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-trade-confirmation-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-title-selection-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-title-deletion-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-custom-title-input-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-level-up-point-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-skill-swap-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-delete-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-version-upgrade-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-trade-selection-cancel-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-battle-position-toggle-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-return-to-login-point-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-stall-end-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-stall-browse-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-stall-buy-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-produce-time-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-bank-page-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-item-move-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-bag-page-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-bag-page-move-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-bag-page-move-ex-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pfm-notification-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-login-result-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-logout-result-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-client-index-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-coordinate-update-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-encounter-probability-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-player-status-flags-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-map-checksum-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-fever-status-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-battle-position-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-player-position-sync-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-client-ip-notification-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-macro-check-config-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-create-new-char-result-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-delete-result-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-process-status-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-version-upgrade-result-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-stall-start-result-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-stall-end-result-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-stall-buy-result-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-item-swap-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-drop-item-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-cheat-detection-report-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-int62-codec-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-es-string-codec-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-token-codec-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-encode64-codec-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-parser-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-builder-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-map-name-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-object-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-address-book-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-address-book-guild-info-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-info-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-title-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-member-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-member-info-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-trade-player-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-trade-start-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-trade-item-info-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-trade-pet-skill-info-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-drop-gold-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-drop-pet-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-item-move-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-item-recipe-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-album-info-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-album-name-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-address-book-delete-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-address-book-exchange-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-info-change-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-title-set-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-member-list-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-member-title-change-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-member-invite-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-leave-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-member-remove-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-lists-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-data-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-player-status-flags-update-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-party-membership-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-battle-status-update-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-move-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-skill-front-char-list-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-skill-character-pet-list-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-item-character-pet-list-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-item-stamp-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-action-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-action-skill-icon-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-tech-use-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-guild-mail-send-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-mail-friend-delete-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-mini-mail-send-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-trade-take-out-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-stall-start-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-icon-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-object-delete-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-map-data-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-map-data-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-version-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-profile-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-char-data2-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-job-name-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-title-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-about-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-data-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-set-data-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-magic-use-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-skill-use-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-level-up-point-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-skill-swap-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-user-pet-name-input-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-skill-data-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-item-data-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-item-data-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-equip-suit-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-item-info-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-item-recipe-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-album-flags-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-album-info-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-album-name-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-mail-animation-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-mail-message-send-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-chat-message-send-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-window-open-send-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-shutdown-request-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-mail-send-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-mail-register-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-mail-delete-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-mail-receipt-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-mail-arrive-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-party-param-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-party-member-status-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-tech-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-pet-tech-data-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-ride-pet-data-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-ride-pet-extra-data-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-status-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-front-char-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-item-front-char-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-character-pet-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-char-and-pet-item-list-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-window-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-system-message-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-effect-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-battle-reward-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-duel-point-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-battle-command-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-profile-mail-message-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-chat-message-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-window-packet-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-stall-browse-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-payload-codec-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-coverage-schema-test PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(WIN32)
    foreach(runtime_target IN ITEMS SDL3::SDL3-shared SDL3_image::SDL3_image-shared SDL3_ttf::SDL3_ttf-shared)
        if(TARGET ${runtime_target})
            add_custom_command(TARGET cg-client POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "$<TARGET_FILE:${runtime_target}>"
                    "$<TARGET_FILE_DIR:cg-client>"
                VERBATIM
            )
        endif()
    endforeach()

    if(TARGET SDL3::SDL3-shared)
        add_custom_command(TARGET cg-map-viewer POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:SDL3::SDL3-shared>"
                "$<TARGET_FILE_DIR:cg-map-viewer>"
            VERBATIM
        )
        add_custom_command(TARGET cg-asset-viewer POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:SDL3::SDL3-shared>"
                "$<TARGET_FILE_DIR:cg-asset-viewer>"
            VERBATIM
        )
        add_custom_command(TARGET cg-anim-viewer POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:SDL3::SDL3-shared>"
                "$<TARGET_FILE_DIR:cg-anim-viewer>"
            VERBATIM
        )
        add_custom_command(TARGET cg-anim-exporter POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:SDL3::SDL3-shared>"
                "$<TARGET_FILE_DIR:cg-anim-exporter>"
            VERBATIM
        )
    endif()

    if(TARGET SDL3_image::SDL3_image-shared)
        add_custom_command(TARGET cg-asset-viewer POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:SDL3_image::SDL3_image-shared>"
                "$<TARGET_FILE_DIR:cg-asset-viewer>"
            VERBATIM
        )
        add_custom_command(TARGET cg-anim-exporter POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:SDL3_image::SDL3_image-shared>"
                "$<TARGET_FILE_DIR:cg-anim-exporter>"
            VERBATIM
        )
    endif()
endif()
