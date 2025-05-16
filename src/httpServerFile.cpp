#include "serverFile.h"
#include "httpServerFile.h"
#include <algorithm>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <sstream>

// 初始化静态成员，自动注册HTTP协议处理函数
bool HttpServerUtil::autoRegistered = HttpServerUtil::initialize();

// 初始化方法，注册HTTP协议处理函数
bool HttpServerUtil::initialize() {
    return serverFile::registerProtocolHandler(Protocol::HTTP, HttpServerUtil::httpEventloop);
}

// HTTP协议的事件循环 - 静态方法
Task<void, void> HttpServerUtil::httpEventloop(serverFile* self)
{
    // 解析状态相关变量
    ParseState state = ParseState::REQUEST_LINE; // Use enum class
    std::string method;
    std::string path;
    std::string version;
    size_t content_length = 0;
    size_t body_read = 0;
    std::string body_buffer;

    while (self->getStatus()) // Use getStatus()
    {
        // Access SocketFile through serverFile's getter
        SocketFile& sfile = self->getSocketFile();
        EventStatus socketEventStatus = sfile.eventGo();

        if (socketEventStatus == EventStatus::Stop)
        {
            self->setFileState(false); // Update serverFile's state
            co_yield {};
            continue; // Skip further processing if socket stopped
        }
        
        // Check status via SocketFile's getter
        if (sfile.getSocketStatus() == SocketStatus::WRONG)
        {
            self->setFileState(false); // Update serverFile's state
            co_yield {};
            continue; // Skip further processing
        }

        std::string_view tp = sfile.read_line(); // Use sfile
        switch (state)
        {
        case ParseState::REQUEST_LINE: // Use enum class
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

                    self->getContent()["method"] = method;
                    self->getContent()["path"] = path;
                    self->getContent()["version"] = version;

                    state = ParseState::HEADERS; // Use enum class
                }
            }
            break;

        case ParseState::HEADERS: // Use enum class
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (tp == "\r\n")
            {
                if (method == "POST")
                {
                    auto it = self->getContent().find("content-length");
                    if (it != self->getContent().end())
                    {
                        try
                        {
                            content_length = std::stoul(std::string(it->second));
                            state = ParseState::BODY; // Use enum class
                        }
                        catch (...)
                        {
                            content_length = 0;
                            state = ParseState::COMPLETE; // Use enum class
                        }
                    }
                    else
                    {
                        state = ParseState::COMPLETE; // Use enum class
                    }
                }
                else
                {
                    state = ParseState::COMPLETE; // Use enum class
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

        case ParseState::BODY: // Use enum class
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
                    state = ParseState::COMPLETE; // Use enum class
                }
            }
            break;

        case ParseState::COMPLETE: // Use enum class
        {
            if (sfile.getSocketStatus() != SocketStatus::WRONG) // Check sfile status
            {
                self->handle();
                
                // 只重置必要的状态变量
                state = ParseState::REQUEST_LINE; // Use enum class
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
    for (const auto& header : headers_) {
        oss << header.first << ": " << header.second << "\r\n";
    }
    oss << "\r\n";
    oss << body_;
    return oss.str();
}