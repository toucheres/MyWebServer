#include "serverFile.h"
#include "httpServerFile.h"
#include "protocol_constants.h" // 新增包含
#include <algorithm>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <sstream>
#include <iomanip>

// 初始化静态成员，自动注册HTTP协议处理函数
bool HttpServerUtil::autoRegistered = HttpServerUtil::initialize();

// 初始化方法，注册HTTP协议处理函数
bool HttpServerUtil::initialize() {
    return serverFile::registerProtocolHandler(Protocol::HTTP, HttpServerUtil::httpEventloop); // 使用 Protocol 枚举
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
                i += 2;  // 跳过已处理的两个十六进制字符
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
    if (data.empty()) {
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
    std::string sizeStr = (pos != std::string::npos) ? 
                         chunkSizeLine.substr(0, pos) : chunkSizeLine;
    
    // 移除前后空白字符
    size_t start = sizeStr.find_first_not_of(" \t");
    size_t end = sizeStr.find_last_not_of(" \t\r\n");
    if (start != std::string::npos && end != std::string::npos) {
        sizeStr = sizeStr.substr(start, end - start + 1);
    }
    
    try {
        return std::stoul(sizeStr, nullptr, 16); // 十六进制解析
    } catch (...) {
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
                    std::string encodedPath = std::string(tp.substr(first_space + 1, second_space - first_space - 1));
                    path = urlDecode(encodedPath);
                    version = std::string(tp.substr(second_space + 1, tp.length() - second_space - 3));

                    self->getContent()["method"] = method;
                    self->getContent()["path"] = path;
                    self->getContent()["version"] = version;

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
                        auto it = self->getContent().find("content-length");
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

                    self->getContent().try_emplace(key, val);
                }
            }
            break;

        case ParseState::BODY:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (body_read < content_length)
            {
                body_buffer.append(tp.data(), tp.length());
                body_read += tp.length();

                if (body_read >= content_length)
                {
                    self->getContent().try_emplace("postcontent", body_buffer);
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
                
                chunked_body.append(chunk_data.data(), chunk_data.size());
                chunk_data_read += chunk_data.size();
                
                if (chunk_data_read >= current_chunk_size)
                {
                    // 当前分块读取完成，读取分块后的CRLF
                    std::string_view crlf = sfile.read_line();
                    if (!crlf.empty() && crlf == "\r\n")
                    {
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
                self->getContent().try_emplace("postcontent", chunked_body);
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
                    self->getContent().try_emplace(key, val);
                }
            }
            break;

        case ParseState::COMPLETE:
        {
            if (sfile.getSocketStatus() != SocketStatus::WRONG)
            {
                self->handle();
                
                // 重置状态变量
                state = ParseState::REQUEST_LINE;
                content_length = 0;
                body_read = 0;
                is_chunked = false;
                current_chunk_size = 0;
                chunk_data_read = 0;
                chunked_body.clear();
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
HttpResponse::HttpResponse(size_t status, std::string httptype, std::string servername)
    : http_version_(std::move(httptype)), status_code_(status)
{
    if (status_num_string.count(status)) {
        reason_phrase_ = status_num_string[status];
    }
    headers_["Server"] = std::move(servername);
    headers_["Connection"] = "keep-alive";
}

HttpResponse& HttpResponse::addHeader(std::string key, std::string val)
{
    headers_[std::move(key)] = std::move(val);
    return *this;
}

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
    if (chunked_mode_) {
        body_.append(HttpServerUtil::createChunkedResponse(chunk_data));
    } else {
        body_.append(chunk_data);
    }
    return *this;
}

HttpResponse& HttpResponse::endChunked()
{
    if (chunked_mode_) {
        body_.append(HttpServerUtil::createChunkedEnd());
    }
    return *this;
}

// 新增：检测是否为分块请求
bool HttpServerUtil::isChunkedRequest(const std::map<std::string, std::string>& headers)
{
    auto it = headers.find("transfer-encoding");
    if (it != headers.end()) {
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
    
    for (const auto& header : headers) {
        // 避免重复设置Transfer-Encoding和Content-Length
        if (header.first != "transfer-encoding" && header.first != "content-length") {
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
LocalFiles& HttpResponse::getFileCache() {
    static LocalFiles fileCache;
    return fileCache;
}

HttpResponse HttpResponse::formLocalFile(std::string path, std::string type)
{
    // 使用文件缓存获取文件内容
    auto& fileCache = getFileCache();
    LocalFile& file = fileCache.get(platform::fixPath(path)); // Fix path for local system
    std::string_view file_content_view = file.read(); // Renamed to avoid conflict
    
    if (!file_content_view.empty()) {
        // 文件存在且有内容
        HttpResponse response(200);
        response.with_content(std::string(file_content_view), type);
        return response;
    } else {
        // 文件不存在或为空
        HttpResponse response(404);
        // Try to load a custom 404.html page
        LocalFile& not_found_page = fileCache.get(platform::fixPath("404.html"));
        std::string_view not_found_content = not_found_page.read();
        if (!not_found_content.empty()) {
            response.with_content(std::string(not_found_content), "text/html;charset=utf-8");
        } else {
            response.with_content("File not found: " + path, "text/plain;charset=utf-8");
        }
        return response;
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
    if (!chunked_mode_ && headers_.find("Content-Length") == headers_.end()) {
        headers_["Content-Length"] = std::to_string(body_.length());
    }
    
    for (const auto& header : headers_) {
        oss << header.first << ": " << header.second << "\r\n";
    }
    oss << "\r\n";
    oss << body_;
    return oss.str();
}