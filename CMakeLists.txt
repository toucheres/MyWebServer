# 您的项目 CMakeLists.txt

cmake_minimum_required(VERSION 3.16)
project(my_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# 添加性能分析选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")

file(GLOB SRC src/*.cpp)
# 创建您的可执行文件
add_executable(my_app ${SRC})
target_include_directories(my_app PRIVATE header)
