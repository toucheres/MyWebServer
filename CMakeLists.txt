cmake_minimum_required(VERSION 3.16)
project(MyWebServer LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 重要：设置编译器标志支持协程
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
    if (CMAKE_CXX_COMPILER_VERSION MATCHES "12.*")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-maybe-uninitialized")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 添加 YLT 库 - 使用 add_subdirectory 方式
add_subdirectory(3rd/yalantinglibs)

# 查找必要的包
find_package(Threads REQUIRED)

# 平台特定设置
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    if(MSVC)
        add_compile_options(/W4 /MP)
    endif()
else()
    add_compile_options(-Wall -Wextra)
endif()

# 设置默认构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    message(STATUS "Build type not specified, defaulting to Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# 查找 OpenSSL 包
find_package(OpenSSL REQUIRED)
if(NOT OpenSSL_FOUND)
    message(FATAL_ERROR "OpenSSL not found")
endif()

# MySQL 配置保持不变
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(MYSQL QUIET mysqlclient)
endif()

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

if(MYSQL_FOUND)
    message(STATUS "MySQL found: ${MYSQL_LIBRARIES}")
else()
    message(FATAL_ERROR "MySQL client library not found. Install libmysqlclient-dev or equivalent package.")
endif()

# 收集源文件
file(GLOB LIB_SRC src/*.cpp)

# 创建核心库
add_library(webserver_lib STATIC ${LIB_SRC})
target_include_directories(webserver_lib PUBLIC 
    header 
    3rd/yalantinglibs/include
    3rd/yalantinglibs/include/ylt/thirdparty
    3rd/yalantinglibs/include/ylt/standalone
    ${OPENSSL_INCLUDE_DIR}
    ${MYSQL_INCLUDE_DIRS}
)

# 链接库
target_link_libraries(webserver_lib PUBLIC 
    OpenSSL::SSL 
    OpenSSL::Crypto
    ${MYSQL_LIBRARIES}
    Threads::Threads
)

# 添加demo源文件
file(GLOB DEMO_SRC demo/*.cpp)

# 创建demo可执行文件
add_executable(chat_demo ${DEMO_SRC})
target_include_directories(chat_demo PRIVATE 
    3rd/yalantinglibs/include
    3rd/yalantinglibs/include/ylt/thirdparty
    3rd/yalantinglibs/include/ylt/standalone
)
target_link_libraries(chat_demo PRIVATE webserver_lib)

# 复制静态文件到构建目录
if(EXISTS ${CMAKE_SOURCE_DIR}/static)
    add_custom_target(copy_static_files ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/static/.
        ${CMAKE_BINARY_DIR}/
        COMMENT "Copying static files content to build directory"
    )
    
    # 确保在构建可执行文件之后复制静态文件
    add_dependencies(chat_demo copy_static_files)
    
    message(STATUS "Static files content will be copied from ${CMAKE_SOURCE_DIR}/static/ to ${CMAKE_BINARY_DIR}/")
else()
    message(STATUS "No static directory found, skipping static file copy")
endif()

# 测试程序
if(EXISTS ${CMAKE_SOURCE_DIR}/test/test_parameterized_query.cpp)
    add_executable(test_parameterized_query test/test_parameterized_query.cpp)
    target_link_libraries(test_parameterized_query PRIVATE 
        webserver_lib
        ${MYSQL_LIBRARIES}
    )
    target_include_directories(test_parameterized_query PUBLIC 
        header
        3rd/yalantinglibs/include
        3rd/yalantinglibs/include/ylt/thirdparty
        3rd/yalantinglibs/include/ylt/standalone
    )
endif()

# 安装目标
install(TARGETS chat_demo
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/header/
    DESTINATION include/MyWebServer
    FILES_MATCHING PATTERN "*.h*"
)

install(TARGETS webserver_lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 打印编译指南
message(STATUS "")
message(STATUS "=== MyWebServer Build Instructions ===")
message(STATUS "1. 创建构建目录: mkdir build && cd build")
message(STATUS "2. 生成构建文件: cmake ..")
message(STATUS "3. 编译: make")
message(STATUS "4. 运行: ./chat_demo")
message(STATUS "====================================")
message(STATUS "")