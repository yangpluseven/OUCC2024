cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(MyProject)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含头文件目录
include_directories(include)
include_directories(runtime/antlr4-cpp-runtime-4.13.1/runtime/src)

# 添加子目录
add_subdirectory(runtime/antlr4-cpp-runtime-4.13.1)

# 收集源文件
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/**/*.hpp")

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 链接 antlr4 运行时
target_link_libraries(${PROJECT_NAME} antlr4_shared)
