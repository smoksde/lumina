cmake_minimum_required(VERSION 3.10)
project(Lumina LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# -----------------------------
# Header-only library
# -----------------------------
add_library(lumina INTERFACE)
target_include_directories(lumina INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    # ${CMAKE_CURRENT_SOURCE_DIR}/include/lumina/libs
    ${CMAKE_CURRENT_SOURCE_DIR}/libs
)

# -----------------------------
# Fetch external dependencies
# -----------------------------
include(FetchContent)

# SDL2 (standard FetchContent)
FetchContent_Declare(
    sdl2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-2.26.5
)
FetchContent_MakeAvailable(sdl2)

# GLEW (CMake‑friendly fork)
FetchContent_Declare(
    glew_cmake
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glew_cmake)

# OpenGL
find_package(OpenGL REQUIRED)

# -----------------------------
# Link libraries for header-only interface
# -----------------------------
# The Perlmint fork builds these targets:
#   libglew_static  (static GLEW)
#   libglew_shared  (shared GLEW)
# You can pick whichever you want:
target_link_libraries(lumina
    INTERFACE
        SDL2::SDL2
        libglew_static
        OpenGL::GL
)

# -----------------------------
# Windows-specific definitions
# -----------------------------
if (WIN32)
    target_compile_definitions(lumina INTERFACE GLEW_STATIC)
endif()
