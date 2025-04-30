#pragma once
#include <coroutine>
#include <cstring>
#include <file.h>
#include <iostream>
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

  public:
    // 创建套接字
    int makeSocket()
    {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1)
        {
            std::cerr << "Socket creation failed" << std::endl;
            return -1;
        }
        return server_fd;
    }
    int bindSocket(int server_fd)
    {
        // 绑定地址和端口
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(port);
        if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        {
            std::cerr << "Bind failed" << std::endl;
            close(server_fd);
            return -1;
        }
        return server_fd;
    }
    bool listenSocket(int server_fd, size_t listenLenth = 5)
    {
        // 监听连接
        if (listen(server_fd, listenLenth) == -1)
        {
            std::cerr << "Listen failed" << std::endl;
            close(server_fd);
            return false;
        }
        return true;
    }
    int AcceptSocket(int server_fd, struct sockaddr* client_addr, socklen_t* client_addr_len)
    {
        // 接受连接(阻塞)
        int client_fd = accept(server_fd, client_addr, client_addr_len);
        if (client_fd == -1)
        {
            std::cerr << "Accept failed" << std::endl;
            // close(server_fd);
            return -1;
        }
        return client_fd;
    };
    bool setNonBlocking(int sockfd)
    {
        int flags = fcntl(sockfd, F_GETFL, 0);
        if (flags == -1)
        {
            std::cerr << "获取socket标志失败: " << strerror(errno) << std::endl;
            return false;
        }
        if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            std::cerr << "设置非阻塞模式失败: " << strerror(errno) << std::endl;
            return false;
        }
        return true;
    }

    // 非阻塞版本的Accept
    int AcceptSocketNonBlocking(int server_fd, struct sockaddr* client_addr,
                                socklen_t* client_addr_len)
    {
        // 确保服务器套接字是非阻塞的
        if (!setNonBlocking(server_fd))
        {
            return -1;
        }

        int client_fd = accept(server_fd, client_addr, client_addr_len);
        if (client_fd == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // 没有连接请求，非阻塞模式下会立即返回
                return -2; // 特殊错误码表示无连接
            }
            else
            {
                std::cerr << "Accept失败: " << strerror(errno) << std::endl;
                return -1;
            }
        }
        return client_fd;
    }

    HttpServer(std::string ip_listening = "0.0.0.0", uint16_t port = 8080)
        : ip_listening(ip_listening), port(port), running(false), server_fd(-1)
    {
    }

    ~HttpServer()
    {
        stop();
    }

    bool start()
    {
        int listenfd;
        listenfd = makeSocket();
        int opt = 1;
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            std::cerr << "设置SO_REUSEADDR失败: " << strerror(errno) << std::endl;
            close(listenfd);
            listenfd = -1;
        }
        // 地址绑定到listenfd
        bindSocket(listenfd);
        // 监听listenfd
        listenSocket(listenfd);
        while (1)
        {
            sockaddr client;
            size_t size_client = sizeof(client);
            int connfd = 0;
            while (1)
            {
                connfd = AcceptSocketNonBlocking(listenfd, &client, (socklen_t*)(&size_client));
                if(connfd>0)
                {
                    break;
                }
            }

            struct sockaddr_in servaddr;
            char buff[4096];
            for (int i = 0; i < 4096; i++)
            {
                buff[i] = 0;
            }
            int n = recv(connfd, buff, 4096, 0);
            printf("recv msg from client:%s\n", buff);
            close(connfd);
        }
        close(listenfd);
        return 0;
    }
    bool stop()
    {
    }
    void run();

    std::string processRequest(const std::string& request);
    void handleClient(int client_fd);
};