#include "http.h"
#include <errno.h>
#include <iostream>

int HttpServer::makeSocket()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }
    return server_fd;
}

int HttpServer::bindSocket(int server_fd)
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

bool HttpServer::listenSocket(int server_fd, size_t listenLenth)
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

int HttpServer::AcceptSocket(int server_fd, struct sockaddr* client_addr, socklen_t* client_addr_len)
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
}

bool HttpServer::setNonBlocking(int sockfd)
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

int HttpServer::AcceptSocketNonBlocking(int server_fd, struct sockaddr* client_addr,
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

HttpServer::HttpServer(std::string ip_listening, uint16_t port)
    : ip_listening(ip_listening), port(port), running(false), server_fd(-1)
{
    server_fd = makeSocket();
    setReuseAddr(server_fd);
    // 地址绑定到listenfd
    bindSocket(server_fd);
    // 监听listenfd
    listenSocket(server_fd);
    manager.add(sockets);
}

HttpServer::~HttpServer()
{
    stop();
}

bool HttpServer::setReuseAddr(int& fd)
{
    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "设置SO_REUSEADDR失败: " << strerror(errno) << std::endl;
        close(fd);
        fd = -1;
        return false;
    }
    return true;
}

bool HttpServer::start()
{
    sockaddr client;
    size_t size_client = sizeof(client);
    while (1)
    {
        int connfd = AcceptSocket(server_fd, &client, (socklen_t*)(&size_client));
        if (connfd < 0)
        {
            std::cout << "wrong fd\n";
            continue;
        }
        sockets.add(connfd);
        manager.go();
        for (auto& each : sockets.getMap())
        {
            std::cout << each.second.read_added();
        }
        // std::cout << "recv msg from client:%s\n" << sockets.get() << '\n';
    }
    close(server_fd);
    return 0;
}

bool HttpServer::stop()
{
    return true;
}

// run(), processRequest()和handleClient()的实现还需要补充
void HttpServer::run()
{
    // 待实现
}

std::string HttpServer::processRequest(const std::string& request)
{
    // 待实现
    return "";
}

void HttpServer::handleClient(int client_fd)
{
    // 待实现
}
