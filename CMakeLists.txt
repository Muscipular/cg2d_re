cmake_minimum_required(VERSION 3.24)

project(cg_engine
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CG_SDL_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sdl" CACHE PATH "SDL3 release package root")

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

add_executable(cg-client
    src/app/main.cpp
    src/core/Log.cpp
)

target_include_directories(cg-client
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries(cg-client
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
)

if(MSVC)
    target_compile_options(cg-client PRIVATE /W4 /utf-8 /permissive-)
else()
    target_compile_options(cg-client PRIVATE -Wall -Wextra -Wpedantic)
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
endif()
