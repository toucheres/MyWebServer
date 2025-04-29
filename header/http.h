#include <coroutine>
#include <corutine.hpp>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class HttpServer
{
  public:
    // 创建套接字
    int makeSocket(int domain, int type, int protocol)
    {
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1)
        {
            std::cerr << "Socket creation failed" << std::endl;
            return -1;
        }
        return server_fd;
    }
    int bindSocket(int server_fd, uint16_t port)
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
    bool listenSocket(int server_fd)
    {
        // 监听连接
        if (listen(server_fd, 5) == -1)
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
            close(server_fd);
            return -1;
        }
        return client_fd;
    };
    HttpServer(std::string& ip_listening, uint64_t port)
    {
    }
};