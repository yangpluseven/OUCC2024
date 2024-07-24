cmake_minimum_required(VERSION 3.10)

project(oucc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)
include_directories(runtime/antlr4-cpp-runtime-4.13.1/runtime/src)

add_subdirectory(runtime/antlr4-cpp-runtime-4.13.1)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS}
        oucc.cpp)

target_link_libraries(${PROJECT_NAME} antlr4_static)
