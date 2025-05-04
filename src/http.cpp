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
    close(server_fd);
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
    return httpState;
}

HttpFile::HttpFile(int fd) : socketfile(fd)
{
}

int HttpFile::handle()
{
    const char* response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Connection: close\r\n"
                           "\r\n"
                           "Hello from MyWebServer!\r\n";

    // write(socketfile.handle.context->fd, response, strlen(response));
    this->socketfile.writeFile(response);
    // 输出解析结果
    // std::cout << "Request parsed: " << method << " " << path << "\n";
    std::cout << "Headers count: " << content.size() << "\n";

    // 关闭连接并更新状态
    close(socketfile.handle.context->fd);
    std::cout << "连接已主动关闭: " << socketfile.handle.context->fd << std::endl;
    socketfile.handle.context->fd_state = SocketFile::WRONG;

    return 0;
}

Task<void, void> HttpFile::eventloop()
{
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

    while (1)
    {
        int ret = socketfile.eventGo();
        if (ret == -1)
        {
            this->httpState = false;
            co_yield {};
        }

        // 检查连接状态
        if (socketfile.handle.context && socketfile.handle.context->fd_state == SocketFile::WRONG)
        {
            httpState = -1;
            std::cout << "连接已关闭: " << socketfile.handle.context->fd << std::endl;
            break;
        }
        std::string_view tp = socketfile.read_line();
        switch (state)
        {
        case REQUEST_LINE:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (!tp.empty())
            {
                std::cout << "fd: " << socketfile.handle.context.get()->fd << " 请求行: " << tp;

                // 解析请求行: METHOD PATH VERSION
                size_t first_space = tp.find(' ');
                size_t second_space = tp.find(' ', first_space + 1);

                if (first_space != std::string_view::npos && second_space != std::string_view::npos)
                {
                    method = std::string(tp.substr(0, first_space));
                    path = std::string(tp.substr(first_space + 1, second_space - first_space - 1));
                    version = std::string(tp.substr(second_space + 1,
                                                    tp.length() - second_space - 3)); // 移除\r\n

                    // 存储请求信息
                    content.try_emplace("method", method);
                    content.try_emplace("path", path);
                    content.try_emplace("version", version);

                    state = HEADERS;
                }
            }
            break;

        case HEADERS:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (tp == "\r\n")
            {
                // 空行，表示头部结束

                // 检查是否是POST请求
                if (method == "POST")
                {
                    // 查找Content-Length头部
                    auto it = content.find("content-length");
                    if (it != content.end())
                    {
                        try
                        {
                            content_length = std::stoul(std::string(it->second));
                            state = BODY;
                        }
                        catch (...)
                        {
                            content_length = 0;
                            state = COMPLETE;
                        }
                    }
                    else
                    {
                        state = COMPLETE;
                    }
                }
                else
                {
                    state = COMPLETE;
                }
            }
            else
            {
                // 解析头部
                size_t index = tp.find(": ");
                if (index != std::string_view::npos)
                {
                    // 转换为小写键
                    std::string key(tp.substr(0, index));
                    std::transform(key.begin(), key.end(), key.begin(),
                                   [](unsigned char c) { return std::tolower(c); });

                    // 提取值（去除尾部的\r\n）
                    std::string val;
                    if (index + 2 < tp.length())
                    {
                        size_t val_len = tp.length() - (index + 2) - 2; // 减去\r\n
                        val = std::string(tp.substr(index + 2, val_len));
                    }

                    content.try_emplace(key, val);
                    std::cout << key << ": " << val << '\n';
                }
            }
            break;

        case BODY:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            // 处理POST请求体
            if (body_read < content_length)
            {
                body_buffer.append(tp.data(), tp.length());
                body_read += tp.length();

                if (body_read >= content_length)
                {
                    // 所有内容已读取完成
                    content.try_emplace("postcontent", body_buffer);
                    state = COMPLETE;
                }
            }
            break;

        case COMPLETE:
            // 请求处理完成，发送响应
            {
                // 检查连接状态后再写入
                if (socketfile.handle.context &&
                    socketfile.handle.context->fd_state != SocketFile::WRONG)
                {
                    handle();
                }
                // 重置解析状态
                content.clear();
                state = REQUEST_LINE;
                method.clear();
                path.clear();
                version.clear();
                content_length = 0;
                body_read = 0;
                body_buffer.clear();

                // 通知系统该连接已结束处理
                httpState = -1;
            }
            break;
        }

        co_yield {};
    }
    co_return;
}
int HttpFiles::eventGo()
{
    std::vector<int> toRemove; // Store keys of elements to remove

    for (auto& file : fileMap)
    {
        int ret = file.second.eventGo();
        if (ret == -1)
        {
            toRemove.push_back(file.first); // Add key to removal list
        }
    }

    // Remove elements with ret == -1
    for (int fd : toRemove)
    {
        fileMap.erase(fd);
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