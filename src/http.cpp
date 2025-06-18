#include "http.h"
#include "file.h"
#include "platform.h"
#include "protocol_constants.h" // 新增包含
#include "serverFile.h"
#include <filesystem>
#include <httpServerFile.h>
#include <iostream>
#include <memory>
#include <string_view>

EventStatus HttpServer::eventGo()
{
    if (server_task_handle)
        server_task_handle.resume(); // Use renamed handle
    processFiles();                  // 添加对文件的处理
    return EventStatus::Continue;    // HttpServer typically runs indefinitely
}

// 新增：从HttpFiles移动的fileMap处理功能
void HttpServer::processFiles() // Return type changed to void
{
    std::vector<int> toRemove; // 存储需要移除的元素的键

    for (auto& pair : fileMap) // Iterate over pairs
    {
        // Ensure file.second is not null before calling eventGo
        if (pair.second && pair.second->eventGo() == EventStatus::Stop)
        {
            toRemove.push_back(pair.first); // 添加到移除列表
        }
    }

    for (int fd : toRemove)
    {
        fileMap.erase(fd);
    }
}

// 新增：从HttpFiles移动的add方法
bool HttpServer::add(int fd)
{
    std::shared_ptr<serverFile> ptr = std::make_shared<serverFile>(fd);
    auto [it, inserted] = this->fileMap.try_emplace(fd, ptr);

    if (!inserted)
    {
        return false; // Indicate fd already exists or failed to insert
    }
    else
    {
        // 初始化HTTP协议
        it->second->upgradeProtocol(Protocol::HTTP); // 使用 Protocol 枚举
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
    if (!platform::initSocketLib())
    {
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
    : server_fd(-1), port(port), ip_listening(std::move(ip_listening)), running(false),
      server_task_handle(start()) // Initialize renamed handle
{
    server_fd = makeSocket();
    if (server_fd == static_cast<int>(INVALID_SOCKET_VALUE))
    {
        return;
    }
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
                                      std::string head = HttpServerUtil::makeHttpHead(
                                          200, content, HttpServerUtil::judge_file_type(path));
                                      file.write(std::move(head));
                                      file.write(std::string(content)); // 移除不必要的std::move
                                  }
                              });
        }
    }
}

void HttpServer::callback_callback(std::string format,serverFile& file)
{
    for (auto call = callbacks_format.begin(); call != callbacks_format.end(); call++)
    {
        if (call->first == format)
        {
            call->second(file);
            break;
        }
    }
}

Task<void, void> HttpServer::start()
{
    sockaddr client;
    size_t size_client = sizeof(client);
    while (1)
    {
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

const std::unordered_map<int, std::shared_ptr<serverFile>>& HttpServer::getfilemap()
{
    return fileMap;
}
