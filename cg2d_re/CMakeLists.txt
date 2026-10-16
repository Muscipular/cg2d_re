cmake_minimum_required(VERSION 3.25)

project(cg2d_re LANGUAGES CXX)

if(NOT WIN32 OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "cg2d_re 仅支持 Windows/MSVC/x64 配置。")
endif()

enable_language(RC)

set(SDL3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/sdl/SDL3-3.4.8/cmake"
    CACHE PATH "本机已有 SDL3 3.4.8 SDK 的 CMake 配置目录")
find_package(SDL3 3.4.8 EXACT CONFIG REQUIRED)

# BLM01：复用本机已有解析库，仅编译库，不启用第三方工具、测试或安装。
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_FORMAT_SOURCE OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)
# 旧库声明的3.4策略版本由父工程兼容承接，不修改第三方源码。
block()
    if(NOT DEFINED CMAKE_POLICY_VERSION_MINIMUM)
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
    endif()
    add_subdirectory(3rd/yaml-cpp EXCLUDE_FROM_ALL)
endblock()
target_compile_options(yaml-cpp PRIVATE /GR- /utf-8)

add_library(cg2d_re_core STATIC
    src/text/Encoding.cpp
    src/resource/ResourceNames.cpp
    src/resource/ResourcePaths.cpp
    src/resource/GraphicDecode.cpp
    src/resource/GraphicInfo.cpp
    src/resource/AnimationRuntime.cpp
    src/resource/AnimationCleanup.cpp
    src/resource/AnimationList.cpp
    src/resource/GraphicAlloc.cpp
    src/resource/GraphicRead.cpp
    src/render/Palette.cpp
    src/render/Font.cpp
    src/render/GraphicCacheState.cpp
    src/render/GraphicDxCache.cpp
    src/render/GraphicPaletteCache.cpp
    src/render/RendererResources.cpp
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
target_link_libraries(cg2d_re_core PRIVATE SDL3::SDL3 yaml-cpp::yaml-cpp user32 gdi32)

# 真实业务链由客户端目标承接；缺失依赖按原合同实现，不以占位体消除链接缺口。
add_library(cg2d_re_startup STATIC
    src/platform/ClientInitialization.cpp
    src/platform/LoadingScreen.cpp
    src/render/RendererRebuild.cpp
    src/platform/StartupState.cpp
    src/platform/StartupCommon.cpp
        src/platform/CommandOptions.cpp
    src/platform/WindowLifecycle.cpp
    src/platform/WindowProcedure.cpp
    src/platform/imeImpl.cpp
    src/platform/ClientShutdown.cpp
    src/platform/MainLoop.cpp
    src/platform/MainLoopSupport.cpp
    src/platform/LegacyCryptInt.cpp
    src/platform/LegacyModuleDigest.cpp
    src/platform/platformTool.cpp
    src/platform/SdlAudioBackend.cpp
    src/platform/LegacyWavesReverb.cpp
    src/platform/LegacyAudioMath.cpp
    src/platform/RuntimeInitialization.cpp
    src/platform/LegacyMouse.cpp
    src/platform/LegacyCursor.cpp
    src/platform/MapEffectMath.cpp
    # 用户排除屏幕键盘；LegacyExternalKeyboard*.cpp仅保留为历史源码。
    src/ui/HitTest.cpp
    src/ui/LegacyUiRecord.cpp
    src/world/GuildState.cpp
    src/audio/LegacyAudio.cpp
    src/world/Blacklist.cpp
    src/world/ChatHistory.cpp
    src/input/Input.cpp
    src/input/InputKeyDispatch.cpp
    src/input/InputCaret.cpp
    src/scene/login/LoginConnection.cpp
    src/scene/login/LoginInput.cpp
    src/scene/login/LoginSavedState.cpp
    src/scene/login/LoginInputController.cpp
    src/scene/login/LoginCapability3InputController.cpp
    src/scene/login/LoginCapability3Scene.cpp
    src/scene/login/LoginCapability4Modal.cpp
    src/scene/login/LoginCapability4Scene.cpp
    src/render/DrawGraphic.cpp
    src/render/DrawGraphicHover.cpp
    src/Yobi/base.cpp
    src/Yobi/animation.cpp
    src/Yobi/window.cpp
    src/Yobi/preview.cpp
    src/Yobi/map_actor.cpp
    src/world/WorldActor.cpp
    src/Yobi/map_observation.cpp
    src/Yobi/map_transition.cpp
    src/Yobi/map_render.cpp
    src/world/WorldActorDrawing.cpp
    src/Yobi/environment.cpp
    src/Yobi/battle_base.cpp
    src/Yobi/battle_scene.cpp
    src/Yobi/battle_message.cpp
    src/Yobi/battle_skill.cpp
    src/Yobi/battle_skill_attack.cpp
    src/Yobi/battle_skill_status.cpp
    src/Yobi/battle_skill_special.cpp
    src/Yobi/battle_actor.cpp
    src/Yobi/battle_attack.cpp
    src/Yobi/battle_cast.cpp
    src/Yobi/battle_status.cpp
    src/Yobi/battle_ride.cpp
    src/Yobi/battle_effect.cpp
    src/Yobi/battle_impact.cpp
    src/Yobi/battle_attachment.cpp
    src/Yobi/battle_display.cpp
    src/ui/WindowRuntime.cpp
    src/ui/windows/WindowAdventureLog.cpp
    src/ui/windows/WindowAlbumDetail.cpp
    src/ui/windows/WindowAlbumList.cpp
    src/ui/windows/WindowAppearance.cpp
    src/ui/windows/WindowAttributeTarget.cpp
    src/ui/windows/WindowBank.cpp
    src/ui/windows/WindowBattleCommand.cpp
    src/ui/windows/WindowBattleItems.cpp
    src/ui/windows/WindowBattlePets.cpp
    src/ui/windows/WindowBattleResult.cpp
    src/ui/windows/WindowBattleStateNotice.cpp
    src/ui/windows/WindowBlacklist.cpp
    src/ui/windows/WindowBottomBar.cpp
    src/ui/windows/WindowChat.cpp
    src/ui/windows/WindowConfirmation.cpp
    src/ui/windows/WindowContacts.cpp
    src/ui/windows/WindowCraft.cpp
    src/ui/windows/WindowCrystalExchangeShop.cpp
    src/ui/windows/WindowCrystalShop.cpp
    src/ui/windows/WindowCrystalTrade.cpp
    src/ui/windows/WindowDialog.cpp
    src/ui/windows/WindowEmote.cpp
    src/ui/windows/WindowGather.cpp
    src/ui/windows/WindowGroupSelect.cpp
    src/ui/windows/WindowGuildDetails.cpp
    src/ui/windows/WindowGuildRank.cpp
    src/ui/windows/WindowHotkeys.cpp
    src/ui/windows/WindowInventory.cpp
    src/ui/windows/WindowItemStorage.cpp
    src/ui/windows/WindowItemUse.cpp
    src/ui/windows/WindowMail.cpp
    src/ui/windows/WindowMailCompose.cpp
    src/ui/windows/WindowMailRead.cpp
    src/ui/windows/WindowMessageBoard.cpp
    src/ui/windows/WindowMiniMail.cpp
    src/ui/windows/WindowMiniMap.cpp
    src/ui/windows/WindowOwnProfile.cpp
    src/ui/windows/WindowOwnProfileCategory.cpp
    src/ui/windows/WindowPetAttributes.cpp
    src/ui/windows/WindowPetDetail.cpp
    src/ui/windows/WindowPetList.cpp
    src/ui/windows/WindowPetMailCompose.cpp
    src/ui/windows/WindowPetSkillChoice.cpp
    src/ui/windows/WindowPetSkills.cpp
    src/ui/windows/WindowPlayerAttributes.cpp
    src/ui/windows/WindowPlayerProfile.cpp
    src/ui/windows/WindowPlayerTitles.cpp
    src/ui/windows/WindowProfile.cpp
    src/ui/windows/WindowProfileCategory.cpp
    src/ui/windows/WindowProfileReply.cpp
    src/ui/windows/WindowProfileSearch.cpp
    src/ui/windows/WindowProfileView.cpp
    src/ui/windows/WindowQuantity.cpp
    src/ui/windows/WindowSettings.cpp
    src/ui/windows/WindowShop.cpp
    src/ui/windows/WindowShopChoice.cpp
    src/ui/windows/WindowShortcutBar.cpp
    src/ui/windows/WindowSkill.cpp
    src/ui/windows/WindowSkillResult.cpp
    src/ui/windows/WindowSpectator.cpp
    src/ui/windows/WindowStall.cpp
    src/ui/windows/WindowStatusBar.cpp
    src/ui/windows/WindowTextList.cpp
    src/ui/windows/WindowTextTarget.cpp
    src/ui/windows/WindowTimerNotice.cpp
    src/ui/windows/WindowTrade.cpp
    src/ui/windows/WindowTradeTarget.cpp
    src/ui/windows/WindowWorkResult.cpp
    src/ui/WindowPrototypes.cpp
    src/network/NetworkRuntime.cpp
    src/render/GraphicPendingQueue.cpp
    src/platform/MouseDispatch.cpp
    src/render/DrawAllQueued.cpp
    src/render/LegacyTextureDrawing.cpp
    src/render/InputText.cpp
    src/render/TextPendingQueue.cpp
    src/scene/login/LoginMessage.cpp
    src/scene/login/LoginRequest.cpp
    src/scene/login/CharacterListRequest.cpp
    src/scene/login/CharacterDeletionRequest.cpp
    src/scene/login/CharacterCreationRequest.cpp
    src/scene/login/LoginModeScene.cpp
    src/scene/login/LoginOuterScene.cpp
    src/scene/login/LoginScene.cpp
    src/scene/character_select/CharacterSelectScene.cpp
    src/scene/character_select/CharacterListProgress.cpp
    src/scene/character_select/CharacterAuthenticationFailure.cpp
    src/scene/character_select/CharacterDeletionProgress.cpp
    src/scene/character_select/CharacterDeletionConfirmation.cpp
    src/scene/character_select/CharacterModalButton.cpp
    src/scene/character_select/CharacterSelectModal.cpp
    src/scene/character_select/CharacterVersionUpgrade.cpp
    src/scene/character_select/CharacterSelectPreview.cpp
    src/scene/character_select/CharacterSelectSlot.cpp
    src/scene/login/LoginState.cpp
    src/world/GameTime.cpp
    src/world/MailState.cpp
    src/world/SceneState.cpp
    src/world/WorldObjectState.cpp
    src/world/MapDrawCache.cpp
    src/world/MapSceneDrawing.cpp
    src/world/RenderInput.cpp
    src/world/WorldUpdate.cpp
)
set_target_properties(cg2d_re_startup PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(cg2d_re_startup PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:/GR-;/utf-8;/W4;/fp:strict>")
target_compile_definitions(cg2d_re_startup PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:_CRT_SECURE_NO_WARNINGS>")
target_link_libraries(cg2d_re_startup PUBLIC cg2d_re_core PRIVATE SDL3::SDL3 user32 shell32 gdi32 imm32 ws2_32 winmm iphlpapi ole32 bcrypt)

add_library(cg2d_re_render STATIC
    src/render/SdlPresenter.cpp
)
target_include_directories(cg2d_re_render PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(cg2d_re_render PUBLIC cxx_std_20)
set_target_properties(cg2d_re_render PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(cg2d_re_render PRIVATE /GR- /utf-8 /W4)
target_compile_definitions(cg2d_re_render PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(cg2d_re_render PRIVATE SDL3::SDL3 user32 gdi32)

# BRS01/D03：由宿主 Windows CRT 进入唯一真实 WinMain，不设置私有 /ENTRY。
configure_file(src/platform/FixedMusic.rc.in "${CMAKE_CURRENT_BINARY_DIR}/FixedMusic.rc" @ONLY)
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/FixedMusic.rc" PROPERTIES
    OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/audio/fixed_0.wav;${CMAKE_CURRENT_SOURCE_DIR}/src/audio/fixed_1.wav")
add_executable(cg2d_re_client WIN32
    src/platform/ClientEntry.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/FixedMusic.rc"
)

set_target_properties(cg2d_re_client PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(cg2d_re_client PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:/GR-;/utf-8;/W4;/fp:strict>")
target_compile_definitions(cg2d_re_client PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(cg2d_re_client PRIVATE cg2d_re_startup cg2d_re_render dbghelp)
# 保留实际归档成员和入口的链接证据；不以源码未引用推断原端可达性。
target_link_options(cg2d_re_client PRIVATE "/MAP:$<TARGET_FILE_DIR:cg2d_re_client>/cg2d_re_client.map")

# CSD01：客户端构建完成后带上已链接的 SDL 动态库，见宿主客户端目标 SPEC。
add_custom_command(TARGET cg2d_re_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:cg2d_re_client>"
    VERBATIM)

# 用户要求一次性转换固定MIDI；本工具不参与默认构建或客户端链接。
add_executable(cg2d_re_midi_to_wav EXCLUDE_FROM_ALL
    src/tools/MidiToWav.cpp src/tools/LegacyMidiSynth.cpp)
target_include_directories(cg2d_re_midi_to_wav PRIVATE src/tools)
target_compile_features(cg2d_re_midi_to_wav PRIVATE cxx_std_20)
target_compile_options(cg2d_re_midi_to_wav PRIVATE /GR- /utf-8 /W4)
target_link_libraries(cg2d_re_midi_to_wav PRIVATE SDL3::SDL3)
add_custom_command(TARGET cg2d_re_midi_to_wav POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:cg2d_re_midi_to_wav>)
