# # 您的项目 CMakeLists.txt

# cmake_minimum_required(VERSION 3.16)
# project(my_project LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# # 设置OpenSSL根目录
# set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")

# # 显式列出所有源文件
# file(GLOB SRC src/*.cpp)

# # 创建您的可执行文件
# add_executable(my_app ${SRC})

# # 设置包含目录
# target_include_directories(my_app PRIVATE 
#     header 
#     "${OPENSSL_ROOT_DIR}/include"
# )

# # 链接OpenSSL库 - 修复路径格式问题
# if(MINGW)
#     # MinGW 使用不同的库文件命名
#     target_link_libraries(my_app PRIVATE 
#         "${OPENSSL_ROOT_DIR}/lib/mingw/libssl.a"
#         "${OPENSSL_ROOT_DIR}/lib/mingw/libcrypto.a"
#     )
# else()
#     # MSVC 使用这些库文件
#     target_link_libraries(my_app PRIVATE 
#         "${OPENSSL_ROOT_DIR}/lib/VC/x64/MTd/libssl.lib"
#         "${OPENSSL_ROOT_DIR}/lib/VC/x64/MTd/libcrypto.lib"
#     )
# endif()

# # 添加编译器警告控制
# if(MSVC)
#     target_compile_options(my_app PRIVATE /W4)
# else()
#     target_compile_options(my_app PRIVATE -Wall -Wextra)
# endif()
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