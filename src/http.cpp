#include "http.h"
#include "file.h"
#include <errno.h>
#include <filesystem>
#include <iostream>
#include <string_view>
#include <unistd.h>
int HttpServer::eventGo()
{
    handle.resume();
    processFiles(); // 添加对文件的处理
    return 0;
}

// 新增：从HttpFiles移动的fileMap处理功能
int HttpServer::processFiles()
{
    std::vector<int> toRemove; // 存储需要移除的元素的键

    for (auto& file : fileMap)
    {
        int ret = file.second.eventGo();
        if (ret == false)
        {
            toRemove.push_back(file.first); // 添加到移除列表
        }
    }

    for (int fd : toRemove)
    {
        fileMap.erase(fd);
    }

    return 0;
}

// 新增：从HttpFiles移动的add方法
bool HttpServer::add(int fd)
{
    auto [it, inserted] = this->fileMap.try_emplace(fd, fd);
    if (!inserted)
    {
        it->second = fd; // 或更新现有值
        return false;
    }
    else
    {
        it->second.callback = [this](HttpServerFile& self)
        {
            auto pathIter = self.content.find("path");
            if (pathIter != self.content.end())
            {
                // std::cout << "---" << pathIter->second << "---\n";
                // auto cbIter = this->callbacks.find(pathIter->second);
                // if (cbIter != this->callbacks.end())
                // {
                //     cbIter->second(self);
                // }
                // else
                // {
                for (auto call = callbacks_format.begin(); call != callbacks_format.end(); call++)
                {
                    if (call->first == pathIter->second)
                    {
                        call->second(self);
                        break;
                    }
                }
                // }
            }
        };
    }
    return true;
}

// 新增：从HttpFiles移动的get方法
HttpServerFile& HttpServer::get(int fd)
{
    return fileMap.at(fd);
}

// 新增：从HttpFiles移动的getMap方法
const std::unordered_map<int, HttpServerFile>& HttpServer::getMap()
{
    return fileMap;
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
    // callbacks["1"] = [](HttpServerFile& self)
    // {
    //     const char* response = "HTTP/1.1 200 OK\r\n"
    //                            "Content-Type: text/plain\r\n"
    //                            "Content-Length: 25\r\n"
    //                            "Connection: keep-alive\r\n"
    //                            "\r\n"
    //                            "Hello from MyWebServer!\r\n";
    //     self.socketfile.writeFile(response);
    // };
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

void HttpServer::autoLoginFile(LocalFiles& static_files)
{
    // 获取当前工作目录
    std::filesystem::path current_path = std::filesystem::current_path();

    // 递归遍历当前目录及所有子目录
    for (const auto& entry : std::filesystem::recursive_directory_iterator(current_path))
    {
        if (entry.is_regular_file()) // 只处理普通文件
        {
            // 获取相对于当前目录的路径
            std::filesystem::path relative_path =
                std::filesystem::relative(entry.path(), current_path);
            std::string url_path = "/" + relative_path.string(); // 转换为URL路径格式
            // std::cout << url_path << '\n';
            // 为该文件路径添加回调
            addCallbackFormat(Format{url_path, Format::Type::same},
                              [&static_files](serverFile& file)
                              {
                                  auto path = file.getContent().at("path");
                                  if (path == "/")
                                  {
                                      path = "index.html";
                                  }
                                  else
                                  {
                                      path = &path.data()[1];
                                  }
                                  auto& Localfile = static_files.get(path);
                                  std::string_view content = Localfile.read();
                                  if (content != "")
                                  {
                                      // std::cout << content.size() << '\n';
                                      std::string head = HttpServer::makeHttpHead(
                                          200, content, HttpServer::judge_file_type(path));
                                      // std::cout << head << '\n';
                                      file.write(std::move(head));
                                      file.write(std::move(std::string(content)));
                                      //   file.socketfile.writeFile(std::move(head));
                                      //   file.socketfile.writeFile(std::move(std::string(content)));
                                  }
                              });
        }
    }
}

Task<void, void> HttpServer::start()
{
    sockaddr client;
    size_t size_client = sizeof(client);
    while (1)
    {
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
            this->add(connfd); // 直接使用HttpServer的add方法
        }
        co_yield {};
    }
    co_return;
}

bool HttpServer::stop()
{
    return true;
}

// void HttpServer::addCallback(std::string path, std::function<void(HttpServerFile&)> callback)
// {
//     callbacks.insert_or_assign(path, callback);
// }

void HttpServer::addCallbackFormat(Format format, std::function<void(serverFile&)> callback)
{
    callbacks_format.emplace_front(
        std::pair<Format, std::function<void(serverFile&)>>{format, callback});
}

int HttpServer::removeCallbackFormat(const Format& format)
{
    int removed_count = 0;
    callbacks_format.remove_if(
        [&format, &removed_count](const auto& pair)
        {
            bool should_remove = pair.first == format;
            if (should_remove)
            {
                ++removed_count;
            }
            return should_remove;
        });
    return removed_count;
}

std::string HttpServer::makeHttpHead(int status, std::string_view content,
                                     std::string_view content_type)
{
    std::string response = "";
    response.append("HTTP/1.1 ");
    response.append(std::to_string(status));
    if (status == 200)
        response.append(" OK");
    else if (status == 404)
        response.append(" Not Found");
    response.append("\r\n");
    response.append("Server: testHttp\r\n");
    response.append("Content-Type: ");
    response.append(std::string(content_type));
    response.append("\r\n");
    response.append("Connection: keep-alive\r\n");
    response.append("Content-Length: ");
    response.append(std::to_string(content.length()));
    response.append("\r\n\r\n");
    return response;
}

std::string HttpServer::judge_file_type(std::string_view path)
{
    static const std::unordered_map<std::string, std::string> mime_types = {
        {".html", "text/html"},        {".htm", "text/html"},
        {".css", "text/css"},          {".js", "application/javascript"},
        {".json", "application/json"}, {".png", "image/png"},
        {".jpg", "image/jpeg"},        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},         {".svg", "image/svg+xml"},
        {".txt", "text/plain"},        {".pdf", "application/pdf"},
        {".zip", "application/zip"},   {".mp3", "audio/mpeg"},
        {".mp4", "video/mp4"},
        // 添加更多需要支持的类型
    };
    std::filesystem::path file_path(path);
    std::string extension = file_path.extension().string();
    auto it = mime_types.find(extension);
    if (it != mime_types.end())
    {
        return it->second;
    }
    return "application/octet-stream"; // 默认 MIME 类型
}

std::string HttpServer::processRequest(const std::string& request)
{
    return "";
}

void HttpServer::handleClient(int client_fd)
{
}
