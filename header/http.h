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
class HttpFiles;
class HttpFile : public co_async
{
    friend HttpFiles;
    SocketFile socketfile;
    std::map<std::string_view, std::string_view> content;

  public:
    int eventGo() override;
    HttpFile(int fd);
    Task<void, void> eventloop();
    Task<void, void> corutine;
};
struct HttpFiles : public co_async
{
  private:
    std::unordered_map<int, HttpFile> fileMap;

  public:
    virtual int eventGo() override;
    bool add(int fd);
    HttpFile& get(int fd);
    const std::unordered_map<int, HttpFile>& getMap();
};

class HttpServer : public co_async
{
  private:
    int server_fd;
    uint16_t port;
    std::string ip_listening;
    bool running;
    LocalFiles static_files;
    HttpFiles sockets;
    Co_Manager manager;
    // 创建套接字
    int eventGo() override;
    int makeSocket();
    int bindSocket(int server_fd);
    bool listenSocket(int server_fd, size_t listenLenth = 5);
    int AcceptSocket(int server_fd, struct sockaddr* client_addr, socklen_t* client_addr_len);
    bool setReuseAddr(int& fd);
    Task<void, void> start();
    Task<void, void> handle = start();
    std::string processRequest(const std::string& request);
    std::map<std::string, std::function<void()>> callbacks;
    std::map<std::string, std::function<void(std::string)>> callbacks_format;

  public:
    HttpServer(std::string ip_listening = "0.0.0.0", uint16_t port = 8080);
    ~HttpServer();
    bool stop();
    void addCallback(std::pair<std::string, std::function<void()>> callback);
    void addCallbackFormat(std::pair<std::string, std::function<void(std::string)>> callback);
    void handleClient(int client_fd);
};