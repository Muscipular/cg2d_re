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
