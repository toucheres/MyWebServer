#pragma once
#include "corutine.hpp"
#include "serverFile.h"
#include <httpServerFile.h>
#include <cstring>
#include <file.h>
#include <format.h>
#include <forward_list>
#include <functional>
#include <netinet/in.h>
#include <string>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
class HttpServerFile;
class HttpServer;

class HttpServer : public co_async
{
  private:
    int server_fd;
    uint16_t port;
    std::string ip_listening;
    bool running;
    std::unordered_map<int, HttpServerFile> fileMap; // 从HttpFiles移动过来的fileMap
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
    // std::map<std::string, std::function<void(HttpServerFile&)>> callbacks;
    std::forward_list<std::pair<Format, std::function<void(serverFile&)>>> callbacks_format;

  public:
    void autoLoginFile(LocalFiles& static_files);
    HttpServer(std::string ip_listening = "0.0.0.0", uint16_t port = 8080);
    ~HttpServer();
    bool stop();
    // void addCallback(std::string path, std::function<void(HttpServerFile&)> callback);
    void addCallbackFormat(Format format, std::function<void(serverFile&)> callback);
    int removeCallbackFormat(const Format& format);
    static std::string makeHttpHead(int status, std::string_view content,
                                    std::string_view content_type = "text/plain;charset=utf-8");
    static std::string judge_file_type(std::string_view path);
    void handleClient(int client_fd);

    // 从HttpFiles移动过来的方法
    bool add(int fd);
    HttpServerFile& get(int fd);
    const std::unordered_map<int, HttpServerFile>& getMap();
    int processFiles(); // 替代原HttpFiles的eventGo
};
