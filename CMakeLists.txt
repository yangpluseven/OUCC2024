cmake_minimum_required(VERSION 3.10)

project(oucc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(TARGET_NAME compiler)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_BUILD_TYPE Release)

include_directories(include)
include_directories(runtime/antlr4-cpp-runtime-4.13.1)


file(GLOB_RECURSE SOURCES
        "src/*.cpp"
        "runtime/*.cpp"
)
file(GLOB_RECURSE HEADERS
        "include/*.hpp"
        "runtime/*.hpp"
        "runtime/*.h"
)

add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS}
        src/compiler.cpp
)
