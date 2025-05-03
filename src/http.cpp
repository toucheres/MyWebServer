#include "http.h"
#include "file.h"
#include <errno.h>
#include <iostream>
#include <string_view>
#include <unistd.h>

int HttpServer::eventGo()
{
    handle.resume();
    return 0;
}
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

int HttpServer::AcceptSocket(int server_fd, struct sockaddr* client_addr,
                             socklen_t* client_addr_len)
{
    // 确保服务器套接字是非阻塞的
    // if (!setNonBlocking(server_fd))
    // {
    //     return -1;
    // }
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
// bool HttpServer::setNonBlocking(int sockfd)
// {
//     int flags = fcntl(sockfd, F_GETFL, 0);
//     if (flags == -1)
//     {
//         std::cerr << "获取socket标志失败: " << strerror(errno) << std::endl;
//         return false;
//     }
//     if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
//     {
//         std::cerr << "设置非阻塞模式失败: " << strerror(errno) << std::endl;
//         return false;
//     }
//     return true;
// }

// int HttpServer::AcceptSocketNonBlocking(int server_fd, struct sockaddr* client_addr,
//                                         socklen_t* client_addr_len)
// {
//     // 确保服务器套接字是非阻塞的
//     // if (!setNonBlocking(server_fd))
//     // {
//     //     return -1;
//     // }

//     int client_fd = accept(server_fd, client_addr, client_addr_len);
//     if (client_fd == -1)
//     {
//         if (errno == EAGAIN || errno == EWOULDBLOCK)
//         {
//             // 没有连接请求，非阻塞模式下会立即返回
//             return -2; // 特殊错误码表示无连接
//         }
//         else
//         {
//             std::cerr << "Accept失败: " << strerror(errno) << std::endl;
//             return -1;
//         }
//     }
//     return client_fd;
// }

HttpServer::HttpServer(std::string ip_listening, uint16_t port)
    : ip_listening(ip_listening), port(port), running(false), server_fd(-1)
{
    server_fd = makeSocket();
    setReuseAddr(server_fd);
    // 地址绑定到listenfd
    bindSocket(server_fd);
    // 监听listenfd
    listenSocket(server_fd);
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "获取socket标志失败: " << strerror(errno) << std::endl;
    }
    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "设置非阻塞模式失败: " << strerror(errno) << std::endl;
    }
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

Task<void, void> HttpServer::start()
{
    sockaddr client;
    size_t size_client = sizeof(client);
    while (1)
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        manager.go();
        int connfd = AcceptSocket(server_fd, &client, (socklen_t*)(&size_client));
        if (connfd == -2)
        {
            // no new connection
        }
        else if (connfd == -1)
        {
            std::cerr << "wrong connection\n";
        }
        else
        {
            this->sockets.add(connfd);
        }
        co_yield {};
    }
    close(server_fd);
    co_return;
}

bool HttpServer::stop()
{
    return true;
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
int HttpFile::eventGo()
{
    corutine.resume();
    return 0;
}

HttpFile::HttpFile(int fd) : socketfile(fd)
{
    corutine = eventloop();
}

Task<void, void> HttpFile::eventloop()
{
    while (1)
    {
        socketfile.eventGo();
        std::string_view tp = socketfile.read_line();
        if (tp != "")
        {
            std::cout << "fd: " << socketfile.handle.context.get()->fd << " context: " << tp;
            size_t index = tp.find_first_of(": ");
            std::string_view key = tp.substr(0, index);
            std::string_view val = tp.substr(index + 2, tp.length() - 2);
            auto [it, inserted] = content.try_emplace(key, val);
            if (inserted)
            {
                std::cout << it->first << "__" << it->second << '\n';
            }
        }
        else if (tp == "\r\n")
        {
            // get完成或post头完成
        }
        co_yield {};
    }
    co_return;
}
int HttpFiles::eventGo()
{
    for (auto& file : fileMap)
    {
        // std::cout << "httpfilethis: " << &file << '\n';
        file.second.eventGo();
    }
    return 0;
}
bool HttpFiles::add(int fd)
{
    auto [it, inserted] = this->fileMap.try_emplace(fd, fd);
    if (!inserted)
    {
        it->second = fd; // 或更新现有值
        return false;
    }
    return true;
}
HttpFile& HttpFiles::get(int fd)
{
    return fileMap.at(fd);
}
const std::unordered_map<int, HttpFile>& HttpFiles::getMap()
{
    return fileMap;
}
// Sec-Fetch-Site: none
// Sec-Fetch-Mode: navigate
// Sec-Fetch-User: ?1
// Sec-Fetch-Dest: document
// Accept-Encoding: gzip, deflate, br, zstd
// Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6