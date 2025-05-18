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

# 查找 MySQL 包 - 改用更通用的方法
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(MYSQL QUIET mysqlclient)
endif()

# 如果pkg-config方法失败，尝试直接查找库
if(NOT MYSQL_FOUND)
    find_path(MYSQL_INCLUDE_DIR mysql.h
        PATHS /usr/include/mysql /usr/local/include/mysql /usr/mysql/include/mysql)
    find_library(MYSQL_LIBRARY NAMES mysqlclient
        PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /usr/mysql/lib /usr/mysql/lib64)
    if(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
        set(MYSQL_FOUND TRUE)
        set(MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})
        set(MYSQL_LIBRARIES ${MYSQL_LIBRARY})
    endif()
endif()

# 设置 MySQL 链接选项
if(MYSQL_FOUND)
    message(STATUS "MySQL found: ${MYSQL_LIBRARIES}")
else()
    message(FATAL_ERROR "MySQL client library not found. Install libmysqlclient-dev or equivalent package.")
endif()

# 添加nlohmann/json库
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0  # 可以更换为最新版本
)
FetchContent_MakeAvailable(json)

# 不需要再次查找 nlohmann_json 包
# 删除这一行: find_package(nlohmann_json 3.12.0 REQUIRED)

file(GLOB SRC src/*.cpp)
# 创建您的可执行文件
add_executable(my_app ${SRC})
target_include_directories(my_app PRIVATE 
    header 
    ${OPENSSL_INCLUDE_DIR}
    ${MYSQL_INCLUDE_DIRS}
)
# 链接 OpenSSL 和 MySQL 库
target_link_libraries(my_app PRIVATE 
    OpenSSL::SSL 
    OpenSSL::Crypto
    ${MYSQL_LIBRARIES}
    nlohmann_json::nlohmann_json
)