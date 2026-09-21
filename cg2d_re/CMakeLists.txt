cmake_minimum_required(VERSION 3.25)

project(cg2d_re LANGUAGES CXX)

if(NOT WIN32 OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "cg2d_re 仅支持 Windows/MSVC/x64 配置。")
endif()

# 指针大小不足以排除 ARM64，必须同时确认编译器目标架构。
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8
    OR NOT CMAKE_CXX_COMPILER_ARCHITECTURE_ID STREQUAL "x64")
    message(FATAL_ERROR "cg2d_re 要求 8 字节指针且编译器目标架构为 x64。")
endif()

set(SDL3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/sdl/SDL3-3.4.8/cmake"
    CACHE PATH "本机已有 SDL3 3.4.8 SDK 的 CMake 配置目录")
find_package(SDL3 3.4.8 EXACT CONFIG REQUIRED)

add_library(cg2d_re_core STATIC
    src/ResourceNames.cpp
    src/ResourcePaths.cpp
    src/GraphicDecode.cpp
    src/GraphicInfo.cpp
    src/GraphicRead.cpp
    src/Palette.cpp
    src/render/Font.cpp
    src/render/GraphicCacheState.cpp
    src/render/SurfaceState.cpp
    src/platform/LegacyWindow.cpp
)

target_include_directories(cg2d_re_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_features(cg2d_re_core PUBLIC cxx_std_20)
set_target_properties(cg2d_re_core PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(cg2d_re_core PRIVATE /GR- /utf-8 /W4)
# 保留原 sprintf/fopen 的行为；有效缓冲区等调用前提在各还原接口中记录。
target_compile_definitions(cg2d_re_core PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(cg2d_re_core PRIVATE user32 shell32)

# 编译真实入口链，但不以空实现填补尚未还原的依赖或提前链接演示客户端。
add_library(cg2d_re_startup STATIC
    src/platform/ClientEntry.cpp
    src/platform/StartupState.cpp
    src/platform/StartupCommon.cpp
    src/platform/SetupIni.cpp
    src/platform/CommandOptions.cpp
    src/platform/WindowLifecycle.cpp
    src/platform/WindowProcedure.cpp
    src/platform/ImeLifecycle.cpp
    src/platform/ImeComposition.cpp
    src/platform/ClientShutdown.cpp
    src/platform/MainLoop.cpp
    src/platform/MainLoopState.cpp
    src/platform/MainLoopSupport.cpp
    src/platform/LegacyCryptInt.cpp
    src/platform/LegacyModuleDigest.cpp
    src/platform/LegacyMouse.cpp
    src/platform/LegacyMouseTracker.cpp
    src/platform/LegacyCursor.cpp
    src/platform/LegacyTime.cpp
    src/platform/LegacyExternalKeyboard.cpp
    src/platform/LegacyExternalKeyboardObject.cpp
    src/LegacyHitTest.cpp
    src/LegacyUiRecord.cpp
    src/LegacyAudio.cpp
    src/Input.cpp
    src/InputKeyDispatch.cpp
    src/InputCaret.cpp
    src/LoginInput.cpp
    src/LoginSavedState.cpp
    src/LoginInputController.cpp
    src/LoginCapability3InputController.cpp
    src/LoginCapability3Scene.cpp
    src/LoginCapability4Modal.cpp
    src/LoginCapability4Scene.cpp
    src/DrawGraphic.cpp
    src/DrawGraphicHover.cpp
    src/YobiRuntime.cpp
    src/render/GraphicPendingQueue.cpp
    src/render/InputText.cpp
    src/render/TextPendingQueue.cpp
    src/LoginMessage.cpp
    src/LoginRequest.cpp
    src/CharacterListRequest.cpp
    src/CharacterDeletionRequest.cpp
    src/LoginModeScene.cpp
    src/LoginOuterScene.cpp
    src/LoginScene.cpp
    src/CharacterSelectScene.cpp
    src/CharacterListProgress.cpp
    src/CharacterAuthenticationFailure.cpp
    src/CharacterDeletionProgress.cpp
    src/CharacterDeletionConfirmation.cpp
    src/CharacterModalButton.cpp
    src/CharacterSelectModal.cpp
    src/CharacterVersionUpgrade.cpp
    src/CharacterSelectPreview.cpp
    src/CharacterSelectSlot.cpp
    src/LoginState.cpp
    src/world/GameTime.cpp
    src/world/SceneState.cpp
    src/world/RenderInput.cpp
    src/world/WorldUpdate.cpp
)
set_target_properties(cg2d_re_startup PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(cg2d_re_startup PRIVATE /GR- /utf-8 /W4 /fp:strict)
target_compile_definitions(cg2d_re_startup PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(cg2d_re_startup PUBLIC cg2d_re_core PRIVATE user32 gdi32 imm32)

add_library(cg2d_re_render STATIC
    src/render/SdlPresenter.cpp
)
target_include_directories(cg2d_re_render PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(cg2d_re_render PUBLIC cxx_std_20)
set_target_properties(cg2d_re_render PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(cg2d_re_render PRIVATE /GR- /utf-8 /W4)
target_link_libraries(cg2d_re_render PRIVATE SDL3::SDL3)
