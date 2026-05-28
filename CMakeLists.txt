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
    src/resource/AutoMapRenderer.cpp
    src/resource/AutoMapRules.cpp
    src/resource/BinaryReader.cpp
    src/resource/BinSetConfigLoader.cpp
    src/resource/GraphicDecoder.cpp
    src/resource/MapLoader.cpp
    src/resource/Palette.cpp
    src/resource/ResourceManager.cpp
)

add_executable(cg-map-viewer
    tools/map_viewer/main.cpp
    src/core/Log.cpp
    src/resource/AutoMapRenderer.cpp
    src/resource/AnimationLoader.cpp
    src/resource/AutoMapRules.cpp
    src/resource/BinaryReader.cpp
    src/resource/BinSetConfigLoader.cpp
    src/resource/GraphicDecoder.cpp
    src/resource/MapLoader.cpp
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

enable_testing()

add_executable(cg-automap-renderer-test
    tests/resource/AutoMapRendererTest.cpp
    src/resource/AnimationLoader.cpp
    src/resource/AutoMapRenderer.cpp
    src/resource/AutoMapRules.cpp
    src/resource/BinaryReader.cpp
    src/resource/GraphicDecoder.cpp
    src/resource/Palette.cpp
    src/resource/ResourceManager.cpp
)

target_include_directories(cg-automap-renderer-test
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

add_test(NAME cg-automap-renderer-test COMMAND cg-automap-renderer-test)

if(MSVC)
    target_compile_options(cg-client PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-map-viewer PRIVATE /W4 /utf-8 /permissive-)
    target_compile_options(cg-automap-renderer-test PRIVATE /W4 /utf-8 /permissive-)
else()
    target_compile_options(cg-client PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-map-viewer PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(cg-automap-renderer-test PRIVATE -Wall -Wextra -Wpedantic)
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
    endif()
endif()
