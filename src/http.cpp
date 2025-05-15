#include "http.h"
#include "file.h"
#include "platform.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <string_view>
// #include <unistd.h>

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
        int ret = file.second->eventGo();
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
    std::shared_ptr<serverFile> ptr = std::make_shared<serverFile>(fd);
    auto [it, inserted] = this->fileMap.try_emplace(fd, ptr);

    if (!inserted)
    {
        it->second = ptr;
        return false;
    }
    else
    {
        // 初始化HTTP协议
        it->second->protocolType = Agreement::HTTP;
        it->second->resetCorutine();

        it->second->setCallback(
            [this](serverFile& self)
            {
                auto pathIter = self.getContent().find("path");
                if (pathIter != self.getContent().end())
                {
                    for (auto call = callbacks_format.begin(); call != callbacks_format.end();
                         call++)
                    {
                        if (call->first == pathIter->second)
                        {
                            call->second(self);
                            break;
                        }
                    }
                }
            });
    }
    return true;
}

int HttpServer::makeSocket()
{
    // 初始化socket库 - 平台无关调用
    if (!platform::initSocketLib()) {
        std::cerr << "Failed to initialize socket library" << std::endl;
        return static_cast<int>(INVALID_SOCKET_VALUE);
    }
    
    socket_t server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET_VALUE)
    {
        platform::printError("Socket creation failed");
    }
    return static_cast<int>(server_fd);
}

int HttpServer::bindSocket(int server_fd)
{
    // 绑定地址和端口
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR_RETURN)
    {
        platform::printError("Bind failed");
        platform::closeSocket(server_fd);
        return static_cast<int>(INVALID_SOCKET_VALUE);
    }
    return server_fd;
}

bool HttpServer::listenSocket(int server_fd, size_t listenLenth)
{
    // 监听连接
    if (listen(server_fd, listenLenth) == SOCKET_ERROR_RETURN)
    {
        platform::printError("Listen failed");
        platform::closeSocket(server_fd);
        return false;
    }
    return true;
}

int HttpServer::AcceptSocket(int server_fd, struct sockaddr* client_addr,
                             socklen_t* client_addr_len)
{
    // 接受连接 - 使用平台无关函数
    socket_t client_fd = accept(server_fd, client_addr, client_addr_len);
    if (client_fd == INVALID_SOCKET_VALUE)
    {
        int lastError = platform::getLastError();
        if (platform::isWouldBlock(lastError))
        {
            // 没有连接请求，非阻塞模式下会立即返回
            return -2; // 特殊错误码表示无连接
        }
        else
        {
            platform::printError("Accept失败");
            return static_cast<int>(INVALID_SOCKET_VALUE);
        }
    }
    return static_cast<int>(client_fd);
}

// 修改构造函数，确保初始化顺序与声明顺序一致
HttpServer::HttpServer(std::string ip_listening, uint16_t port)
    : server_fd(-1), port(port), ip_listening(std::move(ip_listening)), running(false)
{
    server_fd = makeSocket();
    setReuseAddr(server_fd);
    // 地址绑定到listenfd
    bindSocket(server_fd);
    // 监听listenfd
    listenSocket(server_fd);
    // 设置非阻塞模式
    platform::setNonBlocking(server_fd);
}

HttpServer::~HttpServer()
{
    stop();
    platform::closeSocket(server_fd);
    platform::cleanupSocketLib();
}

bool HttpServer::setReuseAddr(int& fd)
{
    int opt = 1;
    // 使用平台无关的socket选项设置函数
    if (platform::setSocketOption(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        platform::printError("设置SO_REUSEADDR失败");
        platform::closeSocket(fd);
        fd = -1;
        return false;
    }
    return true;
}

// 修复autoLoginFile中的std::move警告
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
            // 确保路径使用正斜杠（适用于URL）
            std::string url_path = "/" + platform::toUrlPath(relative_path.string());
            
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
                                  // 转换为本地文件系统路径
                                  path = platform::fixPath(path);
                                  auto& Localfile = static_files.get(path);
                                  std::string_view content = Localfile.read();
                                  if (content != "")
                                  {
                                      std::string head = HttpServer::makeHttpHead(
                                          200, content, HttpServer::judge_file_type(path));
                                      file.write(std::move(head));
                                      file.write(std::string(content)); // 移除不必要的std::move
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

// 添加未使用参数的标记
std::string HttpServer::processRequest(const std::string& request [[maybe_unused]])
{
    return "";
}

void HttpServer::handleClient(int client_fd [[maybe_unused]])
{
    // 空实现
}
