cmake_minimum_required(VERSION 3.16)
project(MyWebServer LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 平台特定设置
if(WIN32)
    # Windows特定设置
    add_definitions(-D_WIN32_WINNT=0x0601)
    if(MSVC)
        # MSVC编译器特定标志
        add_compile_options(/W4 /MP)
    endif()
else()
    # Linux/macOS特定设置
    add_compile_options(-Wall -Wextra)
endif()

# 设置默认构建类型为Release，如果没有指定
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

# 收集源文件
file(GLOB LIB_SRC src/*.cpp)

# 创建核心库
add_library(webserver_lib STATIC ${LIB_SRC})
target_include_directories(webserver_lib PUBLIC 
    header 
    ${OPENSSL_INCLUDE_DIR}
    ${MYSQL_INCLUDE_DIRS}
)
target_link_libraries(webserver_lib PUBLIC 
    OpenSSL::SSL 
    OpenSSL::Crypto
    ${MYSQL_LIBRARIES}
)

# 添加demo源文件
file(GLOB DEMO_SRC demo/*.cpp)

# 创建demo可执行文件
add_executable(chat_demo ${DEMO_SRC})
target_link_libraries(chat_demo PRIVATE webserver_lib)
set_target_properties(chat_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 复制demo文件夹下的静态资源到输出目录
add_custom_command(TARGET chat_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/demo/index.html
        $<TARGET_FILE_DIR:chat_demo>/index.html
)

# 添加安装目标
install(TARGETS chat_demo
    RUNTIME DESTINATION bin
)

# 安装头文件
install(DIRECTORY ${CMAKE_SOURCE_DIR}/header/
    DESTINATION include/MyWebServer
    FILES_MATCHING PATTERN "*.h*"
)

# 安装库文件
install(TARGETS webserver_lib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 打印编译指南
message(STATUS "")
message(STATUS "=== MyWebServer Build Instructions ===")
message(STATUS "1. 创建构建目录: mkdir build && cd build")
message(STATUS "2. 生成构建文件:")
message(STATUS "   - Linux/macOS: cmake ..")
message(STATUS "   - Windows: cmake ..")
message(STATUS "3. 编译:")
message(STATUS "   - Linux/macOS: make")
message(STATUS "   - Windows: cmake --build . --config Release")
message(STATUS "4. 运行:")
message(STATUS "   - Linux/macOS: ./bin/chat_demo")
message(STATUS "   - Windows: .\\bin\\Release\\chat_demo.exe")
message(STATUS "====================================")
message(STATUS "")