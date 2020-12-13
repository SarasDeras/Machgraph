cmake_minimum_required(VERSION 3.7)
project(MashGraph)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc -std=c++14")


set(SOURCE_FILES main.cpp Shader.h Camera.h Model.h)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR} "${PROJECT_SOURCE_DIR}/libs/include")
link_directories("${PROJECT_SOURCE_DIR}/libs")

add_executable(MashGraph ${SOURCE_FILES})

target_link_libraries(MashGraph -lglfw3 -lglew32s -lSOIL ${OPENGL_LIBRARIES})
