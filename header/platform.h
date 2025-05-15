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
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <io.h>
typedef SOCKET socket_t;
#define INVALID_SOCKET_VALUE INVALID_SOCKET
#define SOCKET_ERROR_RETURN SOCKET_ERROR
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
// 在Windows下定义ssize_t
#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef int ssize_t;
#endif
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCKET_VALUE (-1)
#define SOCKET_ERROR_RETURN (-1)
#endif

#include <iostream>
#include <string>

// 平台无关的socket函数包装
namespace platform
{
    // 初始化socket库（Windows需要，Unix不需要）
    inline bool initSocketLib()
    {
#ifdef PLATFORM_WINDOWS
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
        return true;
#endif
    }

    // 清理socket库
    inline void cleanupSocketLib()
    {
#ifdef PLATFORM_WINDOWS
        WSACleanup();
#endif
    }

    // 关闭socket
    inline int closeSocket(socket_t sock)
    {
#ifdef PLATFORM_WINDOWS
        return closesocket(sock);
#else
        return close(sock);
#endif
    }

    // 设置socket为非阻塞模式
    inline bool setNonBlocking(socket_t sock)
    {
#ifdef PLATFORM_WINDOWS
        u_long mode = 1;
        return ioctlsocket(sock, FIONBIO, &mode) != SOCKET_ERROR;
#else
        int flags = fcntl(sock, F_GETFL, 0);
        if (flags == -1)
            return false;
        return fcntl(sock, F_SETFL, flags | O_NONBLOCK) != -1;
#endif
    }

    // 获取最后的错误码
    inline int getLastError()
    {
#ifdef PLATFORM_WINDOWS
        return WSAGetLastError();
#else
        return errno;
#endif
    }

    // 获取错误信息字符串
    inline std::string getErrorString(int errCode)
    {
#ifdef PLATFORM_WINDOWS
        char* errorMessage = nullptr;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, nullptr,
                       errCode, 0, (LPSTR)&errorMessage, 0, nullptr);
        std::string result = errorMessage ? errorMessage : "Unknown error";
        LocalFree(errorMessage);
        return result;
#else
        return strerror(errCode);
#endif
    }

    // 检查是否是WOULDBLOCK错误
    inline bool isWouldBlock(int err)
    {
#ifdef PLATFORM_WINDOWS
        return err == WSAEWOULDBLOCK;
#else
        return err == EAGAIN || err == EWOULDBLOCK;
#endif
    }

    // 检查是否是连接重置错误
    inline bool isConnectionReset(int err)
    {
#ifdef PLATFORM_WINDOWS
        return err == WSAECONNRESET || err == WSAECONNABORTED;
#else
        return err == ECONNRESET || err == EPIPE || err == ECONNABORTED;
#endif
    }

    // 读取socket或标准I/O数据
    inline ssize_t readSocket(socket_t sock, char* buffer, size_t len)
    {
#ifdef PLATFORM_WINDOWS
        if (sock == STDIN_FILENO)
        {
            // 获取标准输入句柄
            HANDLE stdHandle = GetStdHandle(STD_INPUT_HANDLE);

            // 检查是否有可用输入数据
            DWORD availableBytes = 0;
            if (!PeekNamedPipe(stdHandle, NULL, 0, NULL, &availableBytes, NULL))
            {
                // 如果是控制台输入而非管道，PeekNamedPipe会失败
                DWORD consoleMode;
                // 获取当前控制台模式
                if (GetConsoleMode(stdHandle, &consoleMode))
                {
                    DWORD oldMode = consoleMode;
                    // 启用非阻塞模式
                    consoleMode |= ENABLE_MOUSE_INPUT; // 这是一个技巧，使Read可以立即返回
                    SetConsoleMode(stdHandle, consoleMode);

                    // 使用超时设置为0的ReadConsole
                    DWORD bytesRead = 0;
                    INPUT_RECORD record;
                    if (!PeekConsoleInput(stdHandle, &record, 1, &bytesRead) || bytesRead == 0)
                    {
                        // 没有输入可用，设置错误为WSAEWOULDBLOCK
                        SetConsoleMode(stdHandle, oldMode); // 恢复原始模式
                        WSASetLastError(WSAEWOULDBLOCK);
                        return SOCKET_ERROR_RETURN;
                    }

                    // 有数据可读，恢复原始模式
                    SetConsoleMode(stdHandle, oldMode);
                }
            }
            else if (availableBytes == 0)
            {
                // 没有数据可读，设置WSAEWOULDBLOCK错误
                WSASetLastError(WSAEWOULDBLOCK);
                return SOCKET_ERROR_RETURN;
            }

            // 读取数据
            DWORD bytesRead = 0;
            if (ReadFile(stdHandle, buffer, static_cast<DWORD>(len), &bytesRead, NULL))
            {
                return static_cast<ssize_t>(bytesRead);
            }
            else
            {
                // 读取失败，设置适当的错误码
                DWORD error = GetLastError();
                WSASetLastError(error);
                return SOCKET_ERROR_RETURN;
            }
        }
        else if (sock == STDOUT_FILENO || sock == STDERR_FILENO)
        {
            // 无法从输出流读取，现在使用与STDIN无输入相同的错误码
            WSASetLastError(WSAEWOULDBLOCK);
            return SOCKET_ERROR_RETURN;
        }
        else
        {
            return recv(sock, buffer, static_cast<int>(len), 0);
        }
#else
        return read(sock, buffer, len);
#endif
    }

    // 写入socket或标准I/O数据
    inline ssize_t writeSocket(socket_t sock, const char* buffer, size_t len)
    {
#ifdef PLATFORM_WINDOWS
        if (sock == STDOUT_FILENO)
        {
            DWORD bytesWritten = 0;
            HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
            if (WriteFile(stdHandle, buffer, static_cast<DWORD>(len), &bytesWritten, NULL))
            {
                return static_cast<ssize_t>(bytesWritten);
            }
            return SOCKET_ERROR_RETURN;
        }
        else if (sock == STDERR_FILENO)
        {
            DWORD bytesWritten = 0;
            HANDLE stdHandle = GetStdHandle(STD_ERROR_HANDLE);
            if (WriteFile(stdHandle, buffer, static_cast<DWORD>(len), &bytesWritten, NULL))
            {
                return static_cast<ssize_t>(bytesWritten);
            }
            return SOCKET_ERROR_RETURN;
        }
        else if (sock == STDIN_FILENO)
        {
            // 无法写入输入流
            WSASetLastError(WSAEINVAL);
            return SOCKET_ERROR_RETURN;
        }
        else
        {
            return send(sock, buffer, static_cast<int>(len), 0);
        }
#else
        return write(sock, buffer, len);
#endif
    }

    // 设置socket选项
    inline int setSocketOption(socket_t sock, int level, int optname, const void* optval,
                               int optlen)
    {
#ifdef PLATFORM_WINDOWS
        return setsockopt(sock, level, optname, static_cast<const char*>(optval), optlen);
#else
        return setsockopt(sock, level, optname, optval, optlen);
#endif
    }

    // 处理路径分隔符
    inline std::string fixPath(const std::string& path)
    {
        std::string result = path;
#ifdef PLATFORM_WINDOWS
        // 将Unix风格的/转换为Windows风格的
        for (auto& c : result)
        {
            if (c == '/')
                c = '\\';
        }
#endif
        return result;
    }

    // 将Windows风格路径转为URL路径（Web服务器中使用）
    inline std::string toUrlPath(const std::string& path)
    {
        std::string result = path;
#ifdef PLATFORM_WINDOWS
        // 将Windows风格的\转换为Web URL风格的/
        for (auto& c : result)
        {
            if (c == '\\')
                c = '/';
        }
#endif
        return result;
    }
    // 打印平台相关错误信息
    inline void printError(const std::string& message)
    {
        std::cerr << message << ": " << platform::getErrorString(platform::getLastError())
                  << std::endl;
    }
    class SocketLibManager
    {
      public:
        SocketLibManager()
        {
            // 程序启动时自动初始化
            initSocketLib();
        }
        ~SocketLibManager()
        {
            // 程序结束时自动清理
            cleanupSocketLib();
        }
        // 禁止复制和移动
        SocketLibManager(const SocketLibManager&) = delete;
        SocketLibManager& operator=(const SocketLibManager&) = delete;
        SocketLibManager(SocketLibManager&&) = delete;
        SocketLibManager& operator=(SocketLibManager&&) = delete;
    };

// 全局对象，确保在程序启动时初始化，退出时清理
#ifdef PLATFORM_WINDOWS
    inline SocketLibManager g_socketLibManager;
#endif
} // namespace platform
