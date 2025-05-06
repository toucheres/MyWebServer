#pragma once
#include "corutine.hpp"
#include <cstring>
#include <file.h>
#include <functional>
#include <map>
#include <netinet/in.h>
#include <string>
#include <string_view>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
class HttpFile;
class HttpServer;

class HttpFile : public co_async
{
    friend HttpServer;
    // friend HttpAPI; // 修改为正确引用嵌套类

    int httpState = true;
    void reset();
    enum ParseState
    {
        REQUEST_LINE,
        HEADERS,
        BODY,
        COMPLETE
    };
    ParseState state = REQUEST_LINE;

    std::string method;
    std::string path;
    std::string version;
    size_t content_length = 0;
    size_t body_read = 0;
    std::string body_buffer;

  public:
    std::map<std::string, std::string> content;
    SocketFile socketfile;
    std::function<void(HttpFile&)> callback;
    int eventGo() override;
    HttpFile(int fd, std::function<void(HttpFile&)> callback = nullptr);
    virtual int handle();
    Task<void, void> eventloop();
    Task<void, void> corutine = eventloop();
};

class HttpServer : public co_async
{
  private:
    int server_fd;
    uint16_t port;
    std::string ip_listening;
    bool running;
    std::unordered_map<int, HttpFile> fileMap; // 从HttpFiles移动过来的fileMap
    Co_Manager manager;
    // std::unordered_map<std::string, HttpAPI> callback;
    //  创建套接字
    int eventGo() override;
    int makeSocket();
    int bindSocket(int server_fd);
    bool listenSocket(int server_fd, size_t listenLenth = 5);
    int AcceptSocket(int server_fd, struct sockaddr* client_addr, socklen_t* client_addr_len);
    bool setReuseAddr(int& fd);
    Task<void, void> start();
    Task<void, void> handle = start();
    std::string processRequest(const std::string& request);
    std::map<std::string, std::function<void(HttpFile&)>> callbacks;
    std::map<std::string, std::function<void(HttpFile&)>> callbacks_format;

  public:
    HttpServer(std::string ip_listening = "0.0.0.0", uint16_t port = 8080);
    ~HttpServer();
    bool stop();
    void addCallback(std::string path, std::function<void(HttpFile&)> callback);
    void addCallbackFormat(std::string format, std::function<void(HttpFile&)> callback);
    void handleClient(int client_fd);

    // 从HttpFiles移动过来的方法
    bool add(int fd);
    HttpFile& get(int fd);
    const std::unordered_map<int, HttpFile>& getMap();
    int processFiles(); // 替代原HttpFiles的eventGo
};
