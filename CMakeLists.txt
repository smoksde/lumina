cmake_minimum_required(VERSION 3.10)
project(Lumina LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---- SDL configuration (must be BEFORE FetchContent) ----
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
set(SDL_TEST OFF   CACHE BOOL "" FORCE)

# -----------------------------
# Header-only library
# -----------------------------
add_library(lumina INTERFACE)

target_include_directories(lumina INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

# -----------------------------
# Fetch dependencies
# -----------------------------
include(FetchContent)

FetchContent_Declare(
    sdl2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.26.5
)
FetchContent_MakeAvailable(sdl2)

FetchContent_Declare(
    glew_cmake
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glew_cmake)

find_package(OpenGL REQUIRED)

# -----------------------------
# Link interface deps
# -----------------------------
target_link_libraries(lumina
    INTERFACE
        SDL2::SDL2-static
        SDL2main
        libglew_static
        OpenGL::GL
)

if (WIN32)
    target_compile_definitions(lumina INTERFACE
        SDL_STATIC
        GLEW_STATIC
    )
endif()
