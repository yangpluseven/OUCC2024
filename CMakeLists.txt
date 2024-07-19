cmake_minimum_required(VERSION 3.10)
project(oucc VERSION 0.1)

option(ANTLR_BUILD_TESTS OFF)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/runtime/antlr4-cpp-runtime-4.13.1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/**)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/**)
file(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

add_executable(oucc ${SOURCES})

target_link_libraries(oucc antlr4-cpp-runtime)