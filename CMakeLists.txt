# 您的项目 CMakeLists.txt

cmake_minimum_required(VERSION 3.16)
project(my_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找 OpenSSL 包
find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found")
endif()

file(GLOB SRC src/*.cpp)
# 创建您的可执行文件
add_executable(my_app ${SRC})
target_include_directories(my_app PRIVATE header ${OPENSSL_INCLUDE_DIR})
# 链接 OpenSSL 库
target_link_libraries(my_app PRIVATE OpenSSL::SSL OpenSSL::Crypto)