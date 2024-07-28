cmake_minimum_required(VERSION 3.10)

project(oucc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(TARGET_NAME compiler)

include_directories(include)
include_directories(runtime/antlr4-cpp-runtime-4.13.1/runtime/src)

add_subdirectory(runtime/antlr4-cpp-runtime-4.13.1)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS}
        src/main.cpp
        include/reg/register.hpp
        src/reg/register.cpp
        include/mir/mir.hpp
)

target_link_libraries(${TARGET_NAME} antlr4_static)
