#include "httpServerFile.h"
#include "protocol_constants.h" // 新增包含
#include "serverFile.h"
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <string>
#include <unordered_map>

// 初始化静态成员，自动注册HTTP协议处理函数
bool HttpServerUtil::autoRegistered = HttpServerUtil::initialize();

// 初始化方法，注册HTTP协议处理函数
bool HttpServerUtil::initialize()
{
    return serverFile::registerProtocolHandler(Protocol::HTTP,
                                               HttpServerUtil::httpEventloop); // 使用 Protocol 枚举
}

// 添加URL解码函数实现
std::string HttpServerUtil::urlDecode(const std::string& encoded)
{
    std::string result;
    result.reserve(encoded.length());

    for (size_t i = 0; i < encoded.length(); ++i)
    {
        if (encoded[i] == '%' && i + 2 < encoded.length())
        {
            // 从十六进制字符转换为整数值
            int value;
            std::istringstream iss(encoded.substr(i + 1, 2));
            if (iss >> std::hex >> value)
            {
                result += static_cast<char>(value);
                i += 2; // 跳过已处理的两个十六进制字符
            }
            else
            {
                result += encoded[i];
            }
        }
        else if (encoded[i] == '+')
        {
            // 将加号转换为空格
            result += ' ';
        }
        else
        {
            result += encoded[i];
        }
    }

    return result;
}

// 添加分块传输辅助函数实现
std::string HttpServerUtil::createChunkedResponse(const std::string& data)
{
    if (data.empty())
    {
        return createChunkedEnd();
    }

    std::ostringstream oss;
    oss << std::hex << data.length() << "\r\n";
    oss << data << "\r\n";
    return oss.str();
}

std::string HttpServerUtil::createChunkedEnd()
{
    return "0\r\n\r\n";
}

size_t HttpServerUtil::parseChunkSize(const std::string& chunkSizeLine)
{
    size_t pos = chunkSizeLine.find(';'); // 忽略可能的chunk扩展
    std::string sizeStr = (pos != std::string::npos) ? chunkSizeLine.substr(0, pos) : chunkSizeLine;

    // 移除前后空白字符
    size_t start = sizeStr.find_first_not_of(" \t");
    size_t end = sizeStr.find_last_not_of(" \t\r\n");
    if (start != std::string::npos && end != std::string::npos)
    {
        sizeStr = sizeStr.substr(start, end - start + 1);
    }

    try
    {
        return std::stoul(sizeStr, nullptr, 16); // 十六进制解析
    }
    catch (...)
    {
        return 0;
    }
}

// HTTP协议的事件循环 - 静态方法
Task<void, void> HttpServerUtil::httpEventloop(serverFile* self)
{
    // 解析状态相关变量
    ParseState state = ParseState::REQUEST_LINE;
    std::string method;
    std::string path;
    std::string version;
    size_t content_length = 0;
    size_t body_read = 0;
    std::string body_buffer;

    // 分块传输相关变量
    bool is_chunked = false;
    size_t current_chunk_size = 0;
    size_t chunk_data_read = 0;
    std::string chunked_body;

    // 新增：原始内容缓存
    std::string original_content_buffer;

    while (self->getStatus())
    {
        SocketFile& sfile = self->getSocketFile();
        EventStatus socketEventStatus = sfile.eventGo();

        if (socketEventStatus == EventStatus::Stop)
        {
            self->setFileState(false);
            co_yield {};
            continue;
        }

        if (sfile.getSocketStatus() == SocketStatus::WRONG)
        {
            self->setFileState(false);
            co_yield {};
            continue;
        }

        std::string_view tp = sfile.read_line();
        
        // 新增：将每一行都添加到原始内容缓存中
        if (!tp.empty())
        {
            original_content_buffer.append(tp.data(), tp.length());
        }

        switch (state)
        {
        case ParseState::REQUEST_LINE:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (!tp.empty())
            {
                size_t first_space = tp.find(' ');
                size_t second_space = tp.find(' ', first_space + 1);

                if (first_space != std::string_view::npos && second_space != std::string_view::npos)
                {
                    method = std::string(tp.substr(0, first_space));
                    std::string encodedPath =
                        std::string(tp.substr(first_space + 1, second_space - first_space - 1));
                    path = urlDecode(encodedPath);
                    version =
                        std::string(tp.substr(second_space + 1, tp.length() - second_space - 3));

                    self->getContent()[HttpResponse::RequestKey::method] = method;
                    self->getContent()[HttpResponse::RequestKey::path] = path;
                    self->getContent()[HttpResponse::RequestKey::version] = version;

                    state = ParseState::HEADERS;
                }
            }
            break;

        case ParseState::HEADERS:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (tp == "\r\n")
            {
                // 检查是否为分块传输
                is_chunked = isChunkedRequest(self->getContent());

                // 对于有请求体的方法（POST, PUT, PATCH等）
                if (method == "POST" || method == "PUT" || method == "PATCH")
                {
                    if (is_chunked)
                    {
                        state = ParseState::CHUNKED_SIZE;
                        chunked_body.clear();
                        // 在content中标记这是分块传输
                        self->getContent()["is_chunked"] = "true";
                    }
                    else
                    {
                        auto it = self->getContent().find(HttpResponse::RequestKey::content_length);
                        // auto it = self->getContent().find("content-length");
                        if (it != self->getContent().end())
                        {
                            try
                            {
                                content_length = std::stoul(std::string(it->second));
                                state = ParseState::BODY;
                            }
                            catch (...)
                            {
                                content_length = 0;
                                state = ParseState::COMPLETE;
                            }
                        }
                        else
                        {
                            state = ParseState::COMPLETE;
                        }
                    }
                }
                else
                {
                    state = ParseState::COMPLETE;
                }
            }
            else
            {
                size_t index = tp.find(": ");
                if (index != std::string_view::npos)
                {
                    std::string key(tp.substr(0, index));
                    std::transform(key.begin(), key.end(), key.begin(),
                                   [](unsigned char c) { return std::tolower(c); });

                    std::string val;
                    if (index + 2 < tp.length())
                    {
                        size_t val_len = tp.length() - (index + 2) - 2;
                        val = std::string(tp.substr(index + 2, val_len));
                    }

                    // 特殊处理cookie头部 - 可能包含多个值
                    if (key == "cookie")
                    {
                        // 如果已存在cookie，追加新值
                        auto it = self->getContent().find(key);
                        if (it != self->getContent().end() && !it->second.empty())
                        {
                            it->second += "; " + val;
                        }
                        else
                        {
                            self->getContent()[key] = val;
                        }
                    }
                    // 处理可能多次出现的其他头部
                    else if (key == "set-cookie" || key == "accept" || key == "accept-encoding" ||
                             key == "accept-language")
                    {
                        auto it = self->getContent().find(key);
                        if (it != self->getContent().end() && !it->second.empty())
                        {
                            it->second += ", " + val;
                        }
                        else
                        {
                            self->getContent()[key] = val;
                        }
                    }
                    // 其他头部直接覆盖
                    else
                    {
                        self->getContent()[key] = val;
                    }
                }
            }
            break;

        case ParseState::BODY:
            if (tp.empty())
            {
                // 正文可能不以\r\n结尾
                auto lefted = sfile.read_num(content_length);
                if (lefted != "")
                {
                    tp = lefted;
                    // 新增：将读取的字节数据也添加到原始内容缓存
                    original_content_buffer.append(tp.data(), tp.length());
                }
                else
                {
                    co_yield {};
                    continue;
                }
            }
            if (body_read < content_length)
            {
                body_buffer.append(tp.data(), tp.length());
                body_read += tp.length();

                // TODO 读超了退回
                if (body_read >= content_length)
                {
                    self->getContent()[HttpResponse::RequestKey::postcontent] = body_buffer;
                    state = ParseState::COMPLETE;
                }
            }
            break;

        case ParseState::CHUNKED_SIZE:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (!tp.empty())
            {
                std::string chunk_size_line(tp.data(), tp.length());
                current_chunk_size = parseChunkSize(chunk_size_line);
                chunk_data_read = 0;

                if (current_chunk_size == 0)
                {
                    // 最后一个分块，进入尾部解析状态
                    state = ParseState::CHUNKED_TRAILER;
                }
                else
                {
                    state = ParseState::CHUNKED_DATA;
                }
            }
            break;

        case ParseState::CHUNKED_DATA:
            // 对于分块数据，需要精确读取指定字节数
            if (chunk_data_read < current_chunk_size)
            {
                size_t remaining = current_chunk_size - chunk_data_read;
                std::string_view chunk_data = sfile.read_num(remaining);

                if (chunk_data.empty())
                {
                    co_yield {};
                    continue;
                }

                // 新增：将分块数据也添加到原始内容缓存
                original_content_buffer.append(chunk_data.data(), chunk_data.size());

                chunked_body.append(chunk_data.data(), chunk_data.size());
                chunk_data_read += chunk_data.size();

                if (chunk_data_read >= current_chunk_size)
                {
                    // 当前分块读取完成，读取分块后的CRLF
                    std::string_view crlf = sfile.read_line();
                    if (!crlf.empty() && crlf == "\r\n")
                    {
                        // 新增：将CRLF也添加到原始内容缓存
                        original_content_buffer.append(crlf.data(), crlf.size());
                        state = ParseState::CHUNKED_SIZE; // 继续读取下一个分块
                    }
                    // 如果CRLF还没完全读取，在下次循环中继续
                }
            }
            break;

        case ParseState::CHUNKED_TRAILER:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (tp == "\r\n")
            {
                // 分块传输结束
                self->getContent()[HttpResponse::RequestKey::postcontent] = chunked_body;
                self->getContent()["chunked_complete"] = "true";
                state = ParseState::COMPLETE;
            }
            else
            {
                // 处理可能的尾部头部
                size_t index = tp.find(": ");
                if (index != std::string_view::npos)
                {
                    std::string key = "trailer_" + std::string(tp.substr(0, index));
                    std::transform(key.begin(), key.end(), key.begin(),
                                   [](unsigned char c) { return std::tolower(c); });

                    std::string val;
                    if (index + 2 < tp.length())
                    {
                        size_t val_len = tp.length() - (index + 2) - 2;
                        val = std::string(tp.substr(index + 2, val_len));
                    }
                    self->getContent()[key] = val;
                }
            }
            break;

        case ParseState::COMPLETE:
        {
            if (sfile.getSocketStatus() != SocketStatus::WRONG)
            {
                // 新增：在处理请求前，将完整的原始内容存储到content中
                self->getContent()[HttpResponse::RequestKey::orignal_content] = original_content_buffer;

                self->handle();

                // 重置状态变量
                state = ParseState::REQUEST_LINE;
                content_length = 0;
                body_read = 0;
                is_chunked = false;
                current_chunk_size = 0;
                chunk_data_read = 0;
                chunked_body.clear();
                
                // 新增：重置原始内容缓存
                original_content_buffer.clear();
            }
        }
        break;
        }
        co_yield {};
    }
    co_return;
}

// 从HttpServer移动过来的HTTP协议相关函数
std::string HttpServerUtil::makeHttpHead(int status, std::string_view content,
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
    response.append("Server: co_Http\r\n");
    response.append("Content-Type: ");
    response.append(std::string(content_type));
    response.append("\r\n");
    response.append("Connection: keep-alive\r\n");
    response.append("Content-Length: ");
    response.append(std::to_string(content.length()));
    response.append("\r\n\r\n");
    return response;
}

std::string HttpServerUtil::makeHttp(int status, std::string_view content,
                                     std::string_view content_type)
{
    auto head = HttpServerUtil::makeHttpHead(status, content, content_type);
    return head + std::string(content);
}

std::string HttpServerUtil::judge_file_type(std::string_view path)
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

// HttpResponse实现
HttpResponse HttpResponse::defult404 = HttpResponse{404};
HttpResponse::HttpResponse(size_t status, std::string httptype, std::string servername)
    : http_version_(std::move(httptype)), status_code_(status)
{
    if (status_num_string.count(status))
    {
        reason_phrase_ = status_num_string[status];
    }
    headers_["Server"] = std::move(servername);
    headers_["Connection"] = "keep-alive";
}

HttpResponse HttpResponse::text(std::string content, size_t status, std::string contenttype,
                                std::string httptype)
{
    return HttpResponse{status, httptype}.with_content(content, contenttype);
}

HttpResponse HttpResponse::binary(std::string content, size_t status, std::string contenttype,
                                  std::string httptype)
{
    return HttpResponse{status, httptype}.with_content(content, contenttype);
}

HttpResponse HttpResponse::fromFileCache(const LocalFile& file)
{
    if (file)
    {
        return HttpResponse{200}.with_content(std::string{file.read()},
                                              HttpServerUtil::judge_file_type(file.getPath()));
    }
    return defult404;
}

HttpResponse& HttpResponse::addHeader(std::string key, std::string val)
{
    headers_[std::move(key)] = std::move(val);
    return *this;
}
// HttpResponse& HttpResponse::addHeader(const std::string_view key, std::string val)
// {
//     headers_[std::string{key}] = std::move(val);
//     return *this;
// }

HttpResponse& HttpResponse::with_content(std::string new_content, std::string type)
{
    body_ = std::move(new_content);
    headers_["Content-Type"] = std::move(type);
    headers_["Content-Length"] = std::to_string(body_.length());
    return *this;
}

HttpResponse& HttpResponse::enableChunked()
{
    chunked_mode_ = true;
    headers_["Transfer-Encoding"] = "chunked";
    headers_.erase("Content-Length"); // 分块传输时不使用Content-Length
    return *this;
}

HttpResponse& HttpResponse::addChunk(const std::string& chunk_data)
{
    if (chunked_mode_)
    {
        body_.append(HttpServerUtil::createChunkedResponse(chunk_data));
    }
    else
    {
        body_.append(chunk_data);
    }
    return *this;
}

HttpResponse& HttpResponse::endChunked()
{
    if (chunked_mode_)
    {
        body_.append(HttpServerUtil::createChunkedEnd());
    }
    return *this;
}

// 新增：检测是否为分块请求
bool HttpServerUtil::isChunkedRequest(const std::map<std::string, std::string>& headers)
{
    auto it = headers.find("transfer-encoding");
    if (it != headers.end())
    {
        std::string transfer_encoding = it->second;
        std::transform(transfer_encoding.begin(), transfer_encoding.end(),
                       transfer_encoding.begin(), [](unsigned char c) { return std::tolower(c); });
        return transfer_encoding.find("chunked") != std::string::npos;
    }
    return false;
}

// 新增：创建分块传输请求头
std::string HttpServerUtil::createChunkedRequest(const std::string& method, const std::string& path,
                                                 const std::map<std::string, std::string>& headers)
{
    std::ostringstream oss;
    oss << method << " " << path << " HTTP/1.1\r\n";
    oss << "Transfer-Encoding: chunked\r\n";

    for (const auto& header : headers)
    {
        // 避免重复设置Transfer-Encoding和Content-Length
        if (header.first != "transfer-encoding" && header.first != "content-length")
        {
            oss << header.first << ": " << header.second << "\r\n";
        }
    }

    oss << "\r\n";
    return oss.str();
}

// 新增：为请求添加分块数据
std::string HttpServerUtil::addChunkToRequest(const std::string& chunk_data)
{
    return createChunkedResponse(chunk_data);
}

// 新增：结束分块请求
std::string HttpServerUtil::endChunkedRequest()
{
    return createChunkedEnd();
}

// 实现获取文件缓存的方法
LocalFiles& HttpResponse::getFileCache()
{
    static LocalFiles fileCache;
    return fileCache;
}

HttpResponse HttpResponse::formLocalFile(std::string path, std::string type)
{
    // 使用文件缓存获取文件内容
    auto& fileCache = getFileCache();
    LocalFile& file = fileCache.get(platform::fixPath(path)); // Fix path for local system
    std::string_view file_content_view = file.read();         // Renamed to avoid conflict

    if (file)
    {
        HttpResponse response(200);
        response.with_content(std::string(file_content_view), type);
        return response;
    }
    else
    {
        return defult404;
    }
}

HttpResponse HttpResponse::formLocalFile(std::string path)
{
    // 自动选择MIME类型
    return formLocalFile(path, HttpServerUtil::judge_file_type(path));
}

HttpResponse::operator std::string()
{
    std::ostringstream oss;
    oss << http_version_ << " " << status_code_ << " " << reason_phrase_ << "\r\n";

    // 如果不是分块模式且没有设置Content-Length，自动设置
    if (!chunked_mode_ && headers_.find("Content-Length") == headers_.end())
    {
        headers_["Content-Length"] = std::to_string(body_.length());
    }

    for (const auto& header : headers_)
    {
        oss << header.first << ": " << header.second << "\r\n";
    }
    oss << "\r\n";
    oss << body_;
    return oss.str();
}

// 新增：网络套接字相关方法实现
int HttpServerUtil::makeSocket()
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
        return static_cast<int>(INVALID_SOCKET_VALUE);
    }
    return static_cast<int>(server_fd);
}

int HttpServerUtil::bindSocket(int server_fd, uint16_t port, const std::string& ip)
{
    // 绑定地址和端口
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;

    // 处理IP地址
    if (ip == "0.0.0.0" || ip.empty())
    {
        server_addr.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0)
        {
            platform::printError("Invalid IP address");
            platform::closeSocket(server_fd);
            return static_cast<int>(INVALID_SOCKET_VALUE);
        }
    }

    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR_RETURN)
    {
        platform::printError("Bind failed");
        platform::closeSocket(server_fd);
        return static_cast<int>(INVALID_SOCKET_VALUE);
    }
    return server_fd;
}

bool HttpServerUtil::listenSocket(int server_fd, size_t listenLength)
{
    // 监听连接
    if (listen(server_fd, static_cast<int>(listenLength)) == SOCKET_ERROR_RETURN)
    {
        platform::printError("Listen failed");
        platform::closeSocket(server_fd);
        return false;
    }
    return true;
}

int HttpServerUtil::acceptSocket(int server_fd, struct sockaddr* client_addr,
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
            platform::printError("Accept failed");
            return static_cast<int>(INVALID_SOCKET_VALUE);
        }
    }
    return static_cast<int>(client_fd);
}

bool HttpServerUtil::setReuseAddr(int fd)
{
    int opt = 1;
    // 使用平台无关的socket选项设置函数
    if (platform::setSocketOption(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        platform::printError("设置SO_REUSEADDR失败");
        return false;
    }
    return true;
}

// ...existing code...

// 新增：客户端连接相关方法实现
int HttpServerUtil::createClientSocket()
{
    // 确保socket库已初始化
    if (!platform::initSocketLib())
    {
        std::cerr << "Failed to initialize socket library" << std::endl;
        return static_cast<int>(INVALID_SOCKET_VALUE);
    }

    socket_t client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == INVALID_SOCKET_VALUE)
    {
        platform::printError("Client socket creation failed");
        return static_cast<int>(INVALID_SOCKET_VALUE);
    }

    return static_cast<int>(client_fd);
}

int HttpServerUtil::connectToServer(const std::string& host, uint16_t port)
{
    // 创建客户端套接字
    int client_fd = createClientSocket();
    if (client_fd == static_cast<int>(INVALID_SOCKET_VALUE))
    {
        return static_cast<int>(INVALID_SOCKET_VALUE);
    }

    // 设置服务器地址结构
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // 解析主机地址
    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0)
    {
        // 如果不是有效的IP地址，尝试作为localhost处理
        if (host == "localhost" || host == "127.0.0.1")
        {
            server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        }
        else
        {
            platform::printError("Invalid host address: " + host);
            platform::closeSocket(client_fd);
            return static_cast<int>(INVALID_SOCKET_VALUE);
        }
    }

    // 连接到服务器
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
        SOCKET_ERROR_RETURN)
    {
        platform::printError("Connection to " + host + ":" + std::to_string(port) + " failed");
        platform::closeSocket(client_fd);
        return static_cast<int>(INVALID_SOCKET_VALUE);
    }

    // std::cout << "Successfully connected to " << host << ":" << port << std::endl;
    return client_fd;
}

int HttpServerUtil::connectToServer(const std::string& host, uint16_t port, int timeout_ms)
{
    // 创建客户端套接字
    int client_fd = createClientSocket();
    if (client_fd == static_cast<int>(INVALID_SOCKET_VALUE))
    {
        return static_cast<int>(INVALID_SOCKET_VALUE);
    }

    // 设置为非阻塞模式以支持超时
    if (!platform::setNonBlocking(client_fd))
    {
        platform::printError("Failed to set non-blocking mode");
        platform::closeSocket(client_fd);
        return static_cast<int>(INVALID_SOCKET_VALUE);
    }

    // 设置服务器地址结构
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // 解析主机地址
    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0)
    {
        if (host == "localhost" || host == "127.0.0.1")
        {
            server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        }
        else
        {
            platform::printError("Invalid host address: " + host);
            platform::closeSocket(client_fd);
            return static_cast<int>(INVALID_SOCKET_VALUE);
        }
    }

    // 尝试连接
    int connect_result = connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    if (connect_result == 0)
    {
        // 立即连接成功
        // std::cout << "Successfully connected to " << host << ":" << port << std::endl;
        return client_fd;
    }
    else if (connect_result == SOCKET_ERROR_RETURN)
    {
        int error = platform::getLastError();

        // 检查是否是非阻塞连接进行中的情况
#ifdef PLATFORM_WINDOWS
        if (error != WSAEWOULDBLOCK && error != WSAEINPROGRESS)
#else
        if (error != EINPROGRESS && error != EWOULDBLOCK)
#endif
        {
            platform::printError("Connection failed");
            platform::closeSocket(client_fd);
            return static_cast<int>(INVALID_SOCKET_VALUE);
        }

        // 使用select等待连接完成
        fd_set write_fds, error_fds;
        FD_ZERO(&write_fds);
        FD_ZERO(&error_fds);
        FD_SET(client_fd, &write_fds);
        FD_SET(client_fd, &error_fds);

        struct timeval timeout;
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;

        int select_result = select(client_fd + 1, nullptr, &write_fds, &error_fds, &timeout);

        if (select_result > 0)
        {
            if (FD_ISSET(client_fd, &error_fds))
            {
                // 连接出错
                platform::printError("Connection failed during select");
                platform::closeSocket(client_fd);
                return static_cast<int>(INVALID_SOCKET_VALUE);
            }
            else if (FD_ISSET(client_fd, &write_fds))
            {
                // 检查连接是否真的成功
                int so_error;
                socklen_t len = sizeof(so_error);
                if (getsockopt(client_fd, SOL_SOCKET, SO_ERROR, (char*)&so_error, &len) == 0 &&
                    so_error == 0)
                {
                    // std::cout << "Successfully connected to " << host << ":" << port
                    //           << " (with timeout)" << std::endl;
                    return client_fd;
                }
                else
                {
                    platform::printError("Connection failed (SO_ERROR check)");
                    platform::closeSocket(client_fd);
                    return static_cast<int>(INVALID_SOCKET_VALUE);
                }
            }
        }
        else if (select_result == 0)
        {
            // 超时
            std::cerr << "Connection timeout to " << host << ":" << port << std::endl;
            platform::closeSocket(client_fd);
            return static_cast<int>(INVALID_SOCKET_VALUE);
        }
        else
        {
            // select错误
            platform::printError("Select failed during connection");
            platform::closeSocket(client_fd);
            return static_cast<int>(INVALID_SOCKET_VALUE);
        }
    }

    platform::closeSocket(client_fd);
    return static_cast<int>(INVALID_SOCKET_VALUE);
}