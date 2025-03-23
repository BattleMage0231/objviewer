cmake_minimum_required(VERSION 3.20)
project(objviewer)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)

add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE external/glm)

add_executable(objviewer
    src/main.cpp
)

target_link_libraries(objviewer PRIVATE glfw glad glm)
