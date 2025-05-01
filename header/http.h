#pragma once
#include <cstring>
#include <file.h>
#include <netinet/in.h>
#include <string>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class HttpServer
{
  private:
    int server_fd;
    uint16_t port;
    std::string ip_listening;
    bool running;
    LocalFiles static_files;
    Co_Manager manager;
    SocketFiles sockets;
    // 创建套接字
    int makeSocket();
    int bindSocket(int server_fd);
    bool listenSocket(int server_fd, size_t listenLenth = 5);
    int AcceptSocket(int server_fd, struct sockaddr* client_addr, socklen_t* client_addr_len);
    bool setNonBlocking(int sockfd);

    // 非阻塞版本的Accept
    int AcceptSocketNonBlocking(int server_fd, struct sockaddr* client_addr,
                                socklen_t* client_addr_len);

  public:
    HttpServer(std::string ip_listening = "0.0.0.0", uint16_t port = 8080);
    ~HttpServer();

    bool setReuseAddr(int& fd);
    bool start();
    bool stop();
    void run();

    std::string processRequest(const std::string& request);
    void handleClient(int client_fd);
};