#include "serverFile.h"
#include "httpServerFile.h"
#include <algorithm>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <fstream>
#include <sstream>

// 初始化静态成员，自动注册HTTP协议处理函数
bool HttpServerUtil::autoRegistered = HttpServerUtil::initialize();

// 初始化方法，注册HTTP协议处理函数
bool HttpServerUtil::initialize() {
    return serverFile::registerProtocolHandler(Agreement::HTTP, HttpServerUtil::httpEventloop);
}

// HTTP协议的事件循环 - 静态方法
Task<void, void> HttpServerUtil::httpEventloop(serverFile* self)
{
    // 解析状态相关变量
    ParseState state = REQUEST_LINE;
    std::string method;
    std::string path;
    std::string version;
    size_t content_length = 0;
    size_t body_read = 0;
    std::string body_buffer;

    while (self->fileState)
    {
        int ret = self->socketfile.eventGo();
        if (ret == -1)
        {
            self->fileState = false;
            co_yield {};
        }

        if ((!self->socketfile.handle.context) ||
            (self->socketfile.handle.context->fd_state == SocketFile::WRONG))
        {
            self->fileState = false;
            co_yield {};
        }

        std::string_view tp = self->socketfile.read_line();
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
                size_t first_space = tp.find(' ');
                size_t second_space = tp.find(' ', first_space + 1);

                if (first_space != std::string_view::npos && second_space != std::string_view::npos)
                {
                    method = std::string(tp.substr(0, first_space));
                    path = std::string(tp.substr(first_space + 1, second_space - first_space - 1));
                    version =
                        std::string(tp.substr(second_space + 1, tp.length() - second_space - 3));

                    self->content["method"] = method;
                    self->content["path"] = path;
                    self->content["version"] = version;

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
                if (method == "POST")
                {
                    auto it = self->content.find("content-length");
                    if (it != self->content.end())
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

                    self->content.try_emplace(key, val);
                }
            }
            break;

        case BODY:
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
                    self->content.try_emplace("postcontent", body_buffer);
                    state = COMPLETE;
                }
            }
            break;

        case COMPLETE:
        {
            if (self->socketfile.handle.context &&
                self->socketfile.handle.context->fd_state != SocketFile::WRONG)
            {
                self->handle();
                
                // 只重置必要的状态变量
                state = REQUEST_LINE;
                // method.clear();
                // path.clear();
                // version.clear();
                content_length = 0;
                body_read = 0;
                // body_buffer.clear();
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
{
    // 初始化基本的HTTP头部
    add("Status", std::to_string(status) + " " + 
        (status_num_string.count(status) > 0 ? status_num_string[status] : ""));
    add("Server", servername);
    add("Connection", "keep-alive");
}

HttpResponse& HttpResponse::add(std::string key, std::string val)
{
    // 将键值对转换为HTTP头部格式并添加到内容中
    content.append(key + ": " + val + "\r\n");
    return *this;
}

HttpResponse& HttpResponse::with_content(std::string new_content, std::string type)
{
    // 添加Content-Type和Content-Length头部，然后添加正文
    add("Content-Type", type);
    add("Content-Length", std::to_string(new_content.length()));
    content.append("\r\n"); // 头部与正文之间的空行
    content.append(new_content);
    return *this;
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
    LocalFile& file = fileCache.get(path);
    std::string_view content = file.read();
    
    HttpResponse response{200};
    
    if (!content.empty()) {
        // 文件存在且有内容
        response = HttpResponse(200);
        response.with_content(std::string(content), type);
    } else {
        // 文件不存在或为空
        response = HttpResponse(404);
        response.with_content("File not found: " + path, "text/plain");
    }
    
    return response;
}

HttpResponse HttpResponse::formLocalFile(std::string path)
{
    // 自动选择MIME类型
    return formLocalFile(path, HttpServerUtil::judge_file_type(path));
}

HttpResponse::operator std::string()
{
    // 构建完整的HTTP响应字符串
    std::string full_response = "";
    
    // 提取状态行
    size_t statusPos = content.find("Status: ");
    if (statusPos != std::string::npos) {
        size_t endPos = content.find("\r\n", statusPos);
        if (endPos != std::string::npos) {
            // 获取状态值
            std::string statusValue = content.substr(statusPos + 8, endPos - (statusPos + 8));
            
            // 构建HTTP状态行
            full_response = "HTTP/1.1 " + statusValue + "\r\n";
            
            // 移除Status行，因为它不是标准HTTP头
            content.erase(statusPos, endPos - statusPos + 2);
        }
    }
    
    // 添加剩余的头部和内容
    full_response.append(content);
    
    return full_response;
}