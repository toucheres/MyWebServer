#pragma once

// 平台检测
#include <cstring>
#if defined(_WIN32) || defined(_WIN64) || defined(WIN32)
    #define PLATFORM_WINDOWS
#elif defined(__linux__) || defined(__unix__)
    #define PLATFORM_UNIX
#else
    #error "Unsupported platform"
#endif

// 平台特定的头文件包含
#ifdef PLATFORM_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    #include <io.h>
    typedef SOCKET socket_t;
    #define INVALID_SOCKET_VALUE INVALID_SOCKET
    #define SOCKET_ERROR_RETURN SOCKET_ERROR
    #define STDIN_FILENO 0
    #define STDOUT_FILENO 1
    #define STDERR_FILENO 2
#else
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <arpa/inet.h>
    #include <errno.h>
    typedef int socket_t;
    #define INVALID_SOCKET_VALUE (-1)
    #define SOCKET_ERROR_RETURN (-1)
#endif

#include <string>
#include <iostream>

// 平台无关的socket函数包装
namespace platform {
    // 初始化socket库（Windows需要，Unix不需要）
    inline bool initSocketLib() {
        #ifdef PLATFORM_WINDOWS
            WSADATA wsaData;
            return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
        #else
            return true;
        #endif
    }

    // 清理socket库
    inline void cleanupSocketLib() {
        #ifdef PLATFORM_WINDOWS
            WSACleanup();
        #endif
    }

    // 关闭socket
    inline int closeSocket(socket_t sock) {
        #ifdef PLATFORM_WINDOWS
            return closesocket(sock);
        #else
            return close(sock);
        #endif
    }

    // 设置socket为非阻塞模式
    inline bool setNonBlocking(socket_t sock) {
        #ifdef PLATFORM_WINDOWS
            u_long mode = 1;
            return ioctlsocket(sock, FIONBIO, &mode) != SOCKET_ERROR;
        #else
            int flags = fcntl(sock, F_GETFL, 0);
            if (flags == -1) return false;
            return fcntl(sock, F_SETFL, flags | O_NONBLOCK) != -1;
        #endif
    }

    // 获取最后的错误码
    inline int getLastError() {
        #ifdef PLATFORM_WINDOWS
            return WSAGetLastError();
        #else
            return errno;
        #endif
    }

    // 获取错误信息字符串
    inline std::string getErrorString(int errCode) {
        #ifdef PLATFORM_WINDOWS
            char* errorMessage = nullptr;
            FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                nullptr, errCode, 0, (LPSTR)&errorMessage, 0, nullptr);
            std::string result = errorMessage ? errorMessage : "Unknown error";
            LocalFree(errorMessage);
            return result;
        #else
            return strerror(errCode);
        #endif
    }

    // 检查是否是WOULDBLOCK错误
    inline bool isWouldBlock(int err) {
        #ifdef PLATFORM_WINDOWS
            return err == WSAEWOULDBLOCK;
        #else
            return err == EAGAIN || err == EWOULDBLOCK;
        #endif
    }

    // 检查是否是连接重置错误
    inline bool isConnectionReset(int err) {
        #ifdef PLATFORM_WINDOWS
            return err == WSAECONNRESET || err == WSAECONNABORTED;
        #else
            return err == ECONNRESET || err == EPIPE || err == ECONNABORTED;
        #endif
    }

    // 读取socket数据
    inline ssize_t readSocket(socket_t sock, char* buffer, size_t len) {
        #ifdef PLATFORM_WINDOWS
            return recv(sock, buffer, static_cast<int>(len), 0);
        #else
            return read(sock, buffer, len);
        #endif
    }

    // 写入socket数据
    inline ssize_t writeSocket(socket_t sock, const char* buffer, size_t len) {
        #ifdef PLATFORM_WINDOWS
            return send(sock, buffer, static_cast<int>(len), 0);
        #else
            return write(sock, buffer, len);
        #endif
    }

    // 设置socket选项
    inline int setSocketOption(socket_t sock, int level, int optname, const void* optval, int optlen) {
        #ifdef PLATFORM_WINDOWS
            return setsockopt(sock, level, optname, static_cast<const char*>(optval), optlen);
        #else
            return setsockopt(sock, level, optname, optval, optlen);
        #endif
    }

    // 处理路径分隔符
    inline std::string fixPath(const std::string& path) {
        std::string result = path;
        #ifdef PLATFORM_WINDOWS
            // 将Unix风格的/转换为Windows风格的\
            for (auto& c : result) {
                if (c == '/') c = '\\';
            }
        #endif
        return result;
    }

    // 将Windows风格路径转为URL路径（Web服务器中使用）
    inline std::string toUrlPath(const std::string& path) {
        std::string result = path;
        #ifdef PLATFORM_WINDOWS
            // 将Windows风格的\转换为Web URL风格的/
            for (auto& c : result) {
                if (c == '\\') c = '/';
            }
        #endif
        return result;
    }
    
    // 打印平台相关错误信息
    inline void printError(const std::string& message) {
        std::cerr << message << ": " << getErrorString(getLastError()) << std::endl;
    }
}
