#pragma once
#include "platform.h" // 添加平台兼容性头文件
#include "port_fd.h"
#include <corutine.hpp>
#include <cstddef>
#include <memory>
#include <queue>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
struct LocalFiles;
struct SocketFiles;

enum class SocketStatus // Moved and changed to enum class
{
    UNKNOWN = -1,
    OK = 0,
    WRONG = 1,
};

struct LocalFile
{
    friend LocalFiles;

  private:
    std::vector<char> content;
    std::string path = "";
    size_t size = 0;
    std::string_view fileview;
    bool useable = false;

  public:
    bool load(std::string& path);
    std::string_view read() const;
    std::string getPath() const;
    operator bool() const;
    // std::string_view wirte();
    LocalFile(std::string a_path);
    LocalFile(LocalFile&& move);
};

struct SocketFile : public co_async
{
    friend SocketFiles;
    friend class HttpServerUtil; // Allow access for event loops
    friend class WebSocketUtil;  // Allow access for event loops

  private:
    class CONTEXT
    {
      public:
        friend SocketFile;
        socket_t fd = INVALID_SOCKET_VALUE; // 使用跨平台的socket类型
        int getfd()
        {
            return fd;
        }
        // read
        std::vector<char> content = {};
        mutable size_t r_left = 0;
        mutable size_t r_right = 0;
        //  write
        struct writingFile
        {
            std::string waitingWrite = {};
            mutable size_t w_left = 0;
            mutable size_t w_right = 0;
            writingFile(std::string&& move);
            writingFile(const std::string& copy);
        };

        std::queue<writingFile> waitingWrites;

        SocketStatus socket_status = SocketStatus::OK; // Changed from fd_state
    };
    static Task<> eventfun(std::shared_ptr<CONTEXT> context);
    Task_Away<CONTEXT> handle_{eventfun}; // Made private, renamed

  public:
    virtual EventStatus eventGo() final; // Changed return type
    const std::string_view read_added() const;
    const std::string_view read_num(size_t num) const;
    const std::string_view read_line() const;
    const std::string_view read_until(
        const std::string_view delimiter = "\r\n") const; // 添加新方法
    const std::string_view read_all() const;
    int getfd();
    void writeFile(std::string file);
    bool setNonBlocking();
    void closeIt();
    bool load(int a_fd);
    SocketFile(int a_fd);
    ~SocketFile();
    SocketFile() = default;
    SocketFile(SocketFile&& move);
    SocketFile& operator=(SocketFile&& move);

    SocketStatus getSocketStatus() const; // Getter for socket_status

    // 添加创建TCP客户端的静态方法
    static SocketFile createTcpClient(port target_port, const std::string& target_ip);
};

struct async_in_out : public co_async
{
  private:
    SocketFile in{STDIN_FILENO};
    SocketFile out{STDOUT_FILENO};
    virtual EventStatus eventGo() override; // Changed return type

  public:
    async_in_out();
    std::string_view read_added();
    std::string_view read_line();
    std::string_view read_all();
    std::string_view read_until();
    void writeFile(std::string file);
    static async_in_out& getInstance();
};

struct LocalFiles
{
  private:
    std::unordered_map<std::string, LocalFile> fileMap;

  public:
    bool add(std::string& path);
    LocalFile& get(const std::string& path);
};

struct SocketFiles : public co_async
{
  private:
    std::unordered_map<int, SocketFile> fileMap;

  public:
    virtual EventStatus eventGo() override; // Changed return type
    bool add(int fd);
    SocketFile& get(int fd);
    const std::unordered_map<int, SocketFile>& getMap();
};
