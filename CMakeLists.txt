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
    src/core/Log.cpp
    src/render/FontCache.cpp
    src/render/RenderQueue.cpp
    src/render/SdlRendererBackend.cpp
    src/render/TextureAtlas.cpp
    src/render/TextureCache.cpp
    src/text/TextEncoding.cpp
    src/ui/ChatBox.cpp
    src/ui/InputBox.cpp
    src/ui/TextLayout.cpp
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

add_executable(cg-packet-types-test
    tests/packet/PacketTypesTest.cpp
)

target_include_directories(cg-packet-types-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-types-test COMMAND cg-packet-types-test)

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

add_executable(cg-packet-envelope-codec-test
    tests/packet/EnvelopeCodecTest.cpp
)

target_include_directories(cg-packet-envelope-codec-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-packet-envelope-codec-test COMMAND cg-packet-envelope-codec-test)

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
)

if(MSVC)
    target_compile_options(cg-client PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-map-viewer PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-asset-viewer PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-anim-viewer PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-anim-exporter PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-automap-renderer-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-types-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-int62-codec-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-es-string-codec-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-token-codec-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-envelope-codec-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-parser-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-builder-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-map-name-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-object-list-test PRIVATE /W4 /utf-8 /permissive-)
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
    target_compile_options(cg-packet-payload-codec-test PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-packet-coverage-schema-test PRIVATE /W4 /utf-8 /permissive-)
else()
    target_compile_options(cg-client PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-map-viewer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-asset-viewer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-anim-viewer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-anim-exporter PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-automap-renderer-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-types-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-int62-codec-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-es-string-codec-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-token-codec-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-envelope-codec-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-parser-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-builder-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-map-name-test PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-packet-object-list-test PRIVATE -Wall -Wextra -Wpedantic)
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
