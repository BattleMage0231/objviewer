cmake_minimum_required(VERSION 3.20)
project(objviewer)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(tinyobjloader REQUIRED)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_executable(objviewer
    src/main.cc
    src/viewer.cc
    src/camera.cc
    src/window.cc
)

target_compile_options(objviewer PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(objviewer PRIVATE glfw glad glm tinyobjloader)
