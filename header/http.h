#pragma once
#include "corutine.hpp"
#include "serverFile.h"
#include <cstring>
#include <file.h>
#include <format.h>
#include <forward_list>
#include <functional>
#include <platform.h>
#include <string>
#include <sys/types.h>
#include <unordered_map>

class HttpServer : public co_async
{
  private:
    int server_fd;
    uint16_t port;
    std::string ip_listening;
    bool running;
    Co_Manager manager; // This manager is specific to HttpServer for its client connections
    EventStatus eventGo() override; // Changed return type
    int makeSocket();
    int bindSocket(int server_fd);
    bool listenSocket(int server_fd, size_t listenLenth = 5);
    int AcceptSocket(int server_fd, struct sockaddr* client_addr, socklen_t* client_addr_len);
    bool setReuseAddr(int& fd);
    Task<void, void> start();
    Task<void, void> server_task_handle; // Renamed from 'handle'
    //std::string processRequest(const std::string& request);
    std::forward_list<std::pair<Format, std::function<void(serverFile&)>>> callbacks_format;
    std::unordered_map<int, std::shared_ptr<serverFile>> fileMap; // Client connections
    void processFiles();                                          // Changed return type to void
    bool add(int fd);

  public:
    void autoLoginFile(LocalFiles& static_files);
    HttpServer(std::string ip_listening = "0.0.0.0", uint16_t port = 8080);
    ~HttpServer();
    bool stop();
    void addCallbackFormat(Format format, std::function<void(serverFile&)> callback);
    int removeCallbackFormat(const Format& format);
};
