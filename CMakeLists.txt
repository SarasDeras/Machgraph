cmake_minimum_required(VERSION 3.16)
project(MashGraph)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(SOURCE_FILES main.cpp)

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR} "${PROJECT_SOURCE_DIR}/libs/include")
link_directories("${PROJECT_SOURCE_DIR}/libs")

add_executable(main.cpp ${SOURCE_FILES})

target_link_libraries (main.cpp  glfw3 glew32s.lib ${OPENGL_LIBRARIES})
