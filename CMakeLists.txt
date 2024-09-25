cmake_minimum_required(VERSION 3.10)
project(OpenGL_SDL_GLEW_Project)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

include_directories(external)

include_directories(${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include)

add_executable(opengl_sdl_glew examples/main.cpp)

target_link_libraries(opengl_sdl_glew ${SDL2_LIBRARIES} ${GLEW_LIBRARIES} GL)