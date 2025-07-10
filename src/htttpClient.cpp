#include "corutine.hpp"
#include "file.h"
#include "httpClient.h"
#include "httpServerFile.h" // 复用一些工具函数
#include <algorithm>
#include <sstream>

Task<void, void> HttpClientUtil::eventloop(HttpClient* self)
{
    // 解析状态相关变量
    HttpServerUtil::ParseState state = HttpServerUtil::ParseState::REQUEST_LINE;
    std::string version;
    std::string status_code;
    std::string reason_phrase;
    size_t content_length = 0;
    size_t body_read = 0;
    std::string body_buffer;

    // 分块传输相关变量
    bool is_chunked = false;
    size_t current_chunk_size = 0;
    size_t chunk_data_read = 0;
    std::string chunked_body;

    // 原始内容缓存
    std::string original_content_buffer;

    while (true)
    {
        SocketFile& sfile = self->cilent_socket;
        EventStatus socketEventStatus = sfile.eventGo();

        if (socketEventStatus == EventStatus::Stop)
        {
            co_return;
        }

        if (sfile.getSocketStatus() == SocketStatus::WRONG)
        {
            co_return;
        }

        std::string_view tp = sfile.read_line();

        // 将每一行都添加到原始内容缓存中
        if (!tp.empty())
        {
            original_content_buffer.append(tp.data(), tp.length());
        }

        switch (state)
        {
        case HttpServerUtil::ParseState::REQUEST_LINE: // 对于客户端，这里解析状态行
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (!tp.empty())
            {
                // 解析 HTTP/1.1 200 OK
                size_t first_space = tp.find(' ');
                size_t second_space = tp.find(' ', first_space + 1);

                if (first_space != std::string_view::npos && second_space != std::string_view::npos)
                {
                    version = std::string(tp.substr(0, first_space));
                    status_code =
                        std::string(tp.substr(first_space + 1, second_space - first_space - 1));
                    reason_phrase =
                        std::string(tp.substr(second_space + 1, tp.length() - second_space - 3));

                    self->getContent()[HttpClient::CilentKey::version] = version;
                    self->getContent()[HttpClient::CilentKey::status_code] = status_code;
                    self->getContent()[HttpClient::CilentKey::reason_phrase] = reason_phrase;

                    state = HttpServerUtil::ParseState::HEADERS;
                }
            }
            break;

        case HttpServerUtil::ParseState::HEADERS:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (tp == "\r\n")
            {
                // 检查是否为分块传输
                is_chunked = HttpServerUtil::isChunkedRequest(self->getContent());

                // 检查Content-Length
                auto it = self->getContent().find(HttpClient::CilentKey::content_length);
                if (it != self->getContent().end())
                {
                    try
                    {
                        content_length = std::stoul(std::string(it->second));
                        if (content_length > 0)
                        {
                            state = HttpServerUtil::ParseState::BODY;
                        }
                        else
                        {
                            state = HttpServerUtil::ParseState::COMPLETE;
                        }
                    }
                    catch (...)
                    {
                        content_length = 0;
                        state = HttpServerUtil::ParseState::COMPLETE;
                    }
                }
                else if (is_chunked)
                {
                    state = HttpServerUtil::ParseState::CHUNKED_SIZE;
                    chunked_body.clear();
                    self->getContent()[HttpClient::CilentKey::is_chunked] = "true";
                }
                else
                {
                    state = HttpServerUtil::ParseState::COMPLETE;
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

                    // 处理可能多次出现的头部
                    if (key == "set-cookie" || key == "accept" || key == "accept-encoding" ||
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
                    else
                    {
                        self->getContent()[key] = val;
                    }
                }
            }
            break;

        case HttpServerUtil::ParseState::BODY:
            if (tp.empty())
            {
                // 正文可能不以\r\n结尾
                auto lefted = sfile.read_num(content_length - body_read);
                if (!lefted.empty())
                {
                    tp = lefted;
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

                if (body_read >= content_length)
                {
                    self->getContent()[HttpClient::CilentKey::body] = body_buffer;
                    state = HttpServerUtil::ParseState::COMPLETE;
                }
            }
            break;

        case HttpServerUtil::ParseState::CHUNKED_SIZE:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (!tp.empty())
            {
                std::string chunk_size_line(tp.data(), tp.length());
                current_chunk_size = HttpServerUtil::parseChunkSize(chunk_size_line);
                chunk_data_read = 0;

                if (current_chunk_size == 0)
                {
                    state = HttpServerUtil::ParseState::CHUNKED_TRAILER;
                }
                else
                {
                    state = HttpServerUtil::ParseState::CHUNKED_DATA;
                }
            }
            break;

        case HttpServerUtil::ParseState::CHUNKED_DATA:
            if (chunk_data_read < current_chunk_size)
            {
                size_t remaining = current_chunk_size - chunk_data_read;
                std::string_view chunk_data = sfile.read_num(remaining);

                if (chunk_data.empty())
                {
                    co_yield {};
                    continue;
                }

                original_content_buffer.append(chunk_data.data(), chunk_data.size());
                chunked_body.append(chunk_data.data(), chunk_data.size());
                chunk_data_read += chunk_data.size();

                if (chunk_data_read >= current_chunk_size)
                {
                    // 读取分块后的CRLF
                    std::string_view crlf = sfile.read_line();
                    if (!crlf.empty() && crlf == "\r\n")
                    {
                        original_content_buffer.append(crlf.data(), crlf.size());
                        state = HttpServerUtil::ParseState::CHUNKED_SIZE;
                    }
                }
            }
            break;

        case HttpServerUtil::ParseState::CHUNKED_TRAILER:
            if (tp.empty())
            {
                co_yield {};
                continue;
            }
            if (tp == "\r\n")
            {
                // 分块传输结束
                self->getContent()[HttpClient::CilentKey::body] = chunked_body;
                self->getContent()[HttpClient::CilentKey::chunked_complete] = "true";
                state = HttpServerUtil::ParseState::COMPLETE;
            }
            else
            {
                // 处理可能的尾部头部
                size_t index = tp.find(": ");
                if (index != std::string_view::npos)
                {
                    std::string key = std::string(HttpClient::CilentKey::trailer_prefix) +
                                      std::string(tp.substr(0, index));
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

        case HttpServerUtil::ParseState::COMPLETE:
        {
            // 将完整的原始内容存储
            self->getContent()[HttpClient::CilentKey::original_content] = original_content_buffer;

            // 客户端解析完成，可以退出
            if (self->callback)
            {
                self->callback(*self);
            }
            co_return;
        }
        break;
        }
        co_yield {};
    }
    co_return;
}

EventStatus HttpClient::eventGo()
{
    cilent_socket.eventGo();
    if (!coru.done())
    {
        coru.resume();
        return EventStatus::Continue;
    }
    return EventStatus::Stop;
}
std::map<std::string, std::string>& HttpClient::getContent()
{
    return con;
}

HttpClient::HttpClient(SocketFile&& sok)
    : coru(HttpClientUtil::eventloop(this)), cilent_socket(std::move(sok))
{
}

// 补全缺失的构造函数和方法实现
HttpClient::HttpClient(port target, std::string ip)
    : targetport(target), targetip(std::move(ip)), coru(HttpClientUtil::eventloop(this)),
      cilent_socket(SocketFile::createTcpClient(targetport, targetip))
{
}

HttpClient::HttpClient(HttpClient&& move)
    : con(std::move(move.con)), coru(std::move(move.coru)),
      cilent_socket(std::move(move.cilent_socket)), callback(std::move(move.callback)),
      targetport(move.targetport), targetip(std::move(move.targetip))
{
    // 移动后原对象的端口设置为无效值
    move.targetport = (port)-1;
}

HttpClient::HttpClient(const HttpClient& copy)
    : con(copy.con), targetport(copy.targetport), targetip(copy.targetip)
{
    // 注意：复制构造函数不能复制协程和socket，需要重新创建连接
    if (targetport != (port)-1 && !targetip.empty())
    {
        cilent_socket = SocketFile::createTcpClient(targetport, targetip);

        coru = HttpClientUtil::eventloop(this);
    }
    // callback 不复制，因为它可能包含对原对象的引用
}

port HttpClient::getport()
{
    return targetport;
}

std::string HttpClient::getip()
{
    return targetip;
}

void HttpClient::setcallback(std::function<void(HttpClient& self)>&& callback)
{
    this->callback = std::move(callback);
}
// ...existing code...

// HttpRequst 实现
HttpRequst::HttpRequst(const std::string& path, const std::string& method)
    : path_(path), method_(method)
{
    // 设置默认头部
    headers_["Host"] = "localhost";
    headers_["User-Agent"] = "HttpClient/1.0";
    headers_["Accept"] = "*/*";
    headers_["Connection"] = "keep-alive";
}

// 静态工厂方法
HttpRequst HttpRequst::GET(const std::string& path)
{
    return HttpRequst(path, HttpRequestType::GET);
}

HttpRequst HttpRequst::POST(const std::string& path, const std::string& body)
{
    HttpRequst request(path, HttpRequestType::POST);
    if (!body.empty())
    {
        request.setBody(body);
    }
    return request;
}

HttpRequst HttpRequst::PUT(const std::string& path, const std::string& body)
{
    HttpRequst request(path, HttpRequestType::PUT);
    if (!body.empty())
    {
        request.setBody(body);
    }
    return request;
}

HttpRequst HttpRequst::DELETE(const std::string& path)
{
    return HttpRequst(path, HttpRequestType::DELETE);
}

HttpRequst HttpRequst::text(const std::string& path, const std::string& content,
                            const std::string& method, const std::string& content_type)
{
    return HttpRequst(path, method).setBody(content, content_type);
}

HttpRequst HttpRequst::json(const std::string& path, const std::string& json_content,
                            const std::string& method)
{
    return HttpRequst(path, method).setJsonBody(json_content);
}

HttpRequst HttpRequst::form(const std::string& path,
                            const std::map<std::string, std::string>& form_data,
                            const std::string& method)
{
    return HttpRequst(path, method).setFormBody(form_data);
}

// 链式调用方法
HttpRequst& HttpRequst::addHeader(const std::string& key, const std::string& value)
{
    headers_[key] = value;
    return *this;
}

HttpRequst& HttpRequst::setUserAgent(const std::string& user_agent)
{
    headers_["User-Agent"] = user_agent;
    return *this;
}

HttpRequst& HttpRequst::setContentType(const std::string& content_type)
{
    headers_["Content-Type"] = content_type;
    return *this;
}

HttpRequst& HttpRequst::setAuthorization(const std::string& auth)
{
    headers_["Authorization"] = auth;
    return *this;
}

HttpRequst& HttpRequst::setCookie(const std::string& cookie)
{
    headers_["Cookie"] = cookie;
    return *this;
}

HttpRequst& HttpRequst::setBody(const std::string& body, const std::string& content_type)
{
    body_ = body;
    if (!content_type.empty())
    {
        setContentType(content_type);
    }
    if (!chunked_mode_)
    {
        headers_["Content-Length"] = std::to_string(body_.length());
    }
    return *this;
}

HttpRequst& HttpRequst::setJsonBody(const std::string& json)
{
    return setBody(json, ContentType::APPLICATION_JSON);
}

HttpRequst& HttpRequst::setFormBody(const std::map<std::string, std::string>& form_data)
{
    std::string form_string = formDataToString(form_data);
    return setBody(form_string, ContentType::APPLICATION_FORM);
}

HttpRequst& HttpRequst::enableChunked()
{
    chunked_mode_ = true;
    headers_["Transfer-Encoding"] = "chunked";
    headers_.erase("Content-Length"); // 分块传输时不使用Content-Length
    return *this;
}

HttpRequst& HttpRequst::addChunk(const std::string& chunk_data)
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

HttpRequst& HttpRequst::endChunked()
{
    if (chunked_mode_)
    {
        body_.append(HttpServerUtil::createChunkedEnd());
    }
    return *this;
}

// 转换为字符串
HttpRequst::operator std::string() const
{
    return toString();
}

std::string HttpRequst::toString() const
{
    std::ostringstream oss;

    // 请求行
    oss << method_ << " " << path_ << " " << http_version_ << "\r\n";

    // 如果不是分块模式且有body但没有设置Content-Length，自动设置
    if (!chunked_mode_ && !body_.empty() && headers_.find("Content-Length") == headers_.end())
    {
        const_cast<HttpRequst*>(this)->headers_["Content-Length"] = std::to_string(body_.length());
    }

    // 头部
    for (const auto& header : headers_)
    {
        oss << header.first << ": " << header.second << "\r\n";
    }

    // 空行分隔头部和正文
    oss << "\r\n";

    // 正文
    oss << body_;

    return oss.str();
}

// 辅助方法
std::string HttpRequst::urlEncode(const std::string& value) const
{
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex;

    for (char c : value)
    {
        // 保留字母、数字和一些安全字符
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            encoded << c;
        }
        else
        {
            encoded << '%' << std::setw(2) << (unsigned char)c;
        }
    }

    return encoded.str();
}

std::string HttpRequst::formDataToString(const std::map<std::string, std::string>& form_data) const
{
    std::ostringstream oss;
    bool first = true;

    for (const auto& pair : form_data)
    {
        if (!first)
        {
            oss << "&";
        }
        oss << urlEncode(pair.first) << "=" << urlEncode(pair.second);
        first = false;
    }

    return oss.str();
}

// ...existing code...