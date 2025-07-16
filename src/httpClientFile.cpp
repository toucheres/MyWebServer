#include "httpClientFile.h"
#include "clientFile.hpp"
#include "corutine.hpp"
#include "file.h"
#include "httpMessage.h"
#include "httpServerFile.h"
#include <algorithm>
bool HttpClientUtil::autoRegistered = HttpClientUtil::initialize();
bool HttpClientUtil::initialize()
{
    return clientFile ::registerProtocolHandler(
        Protocol::HTTP,
        HttpClientUtil::httpEventloop); // 使用 Protocol 枚举
}
Task<void, void> HttpClientUtil::httpEventloop(clientFile* self)
{
    while (true) // 外层循环用于处理多个请求
    {
        // 重置解析状态相关变量
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

        // keep-alive 相关标志
        bool connection_keep_alive = true;
        bool should_close_connection = false;

        // 内层循环处理单个HTTP响应
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

                    if (first_space != std::string_view::npos &&
                        second_space != std::string_view::npos)
                    {
                        version = std::string(tp.substr(0, first_space));
                        status_code =
                            std::string(tp.substr(first_space + 1, second_space - first_space - 1));
                        reason_phrase = std::string(
                            tp.substr(second_space + 1, tp.length() - second_space - 3));

                        self->getContent()[HttpRequst::CilentKey::version] = version;
                        self->getContent()[HttpRequst::CilentKey::status_code] = status_code;
                        self->getContent()[HttpRequst::CilentKey::reason_phrase] = reason_phrase;

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
                    // 检查Connection头部决定是否保持连接
                    auto conn_it = self->getContent().find("connection");
                    if (conn_it != self->getContent().end())
                    {
                        std::string conn_value = conn_it->second;
                        std::transform(conn_value.begin(), conn_value.end(), conn_value.begin(),
                                       [](unsigned char c) { return std::tolower(c); });

                        if (conn_value.find("close") != std::string::npos)
                        {
                            connection_keep_alive = false;
                            should_close_connection = true;
                        }
                        else if (conn_value.find("keep-alive") != std::string::npos)
                        {
                            connection_keep_alive = true;
                        }
                    }

                    // HTTP/1.0 默认不保持连接，HTTP/1.1 默认保持连接
                    if (version == "HTTP/1.0" && conn_it == self->getContent().end())
                    {
                        connection_keep_alive = false;
                        should_close_connection = true;
                    }

                    // 检查是否为分块传输
                    is_chunked = HttpServerUtil::isChunkedRequest(self->getContent());

                    // 检查Content-Length
                    auto it = self->getContent().find(HttpRequst::CilentKey::content_length);
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
                        self->getContent()[HttpRequst::CilentKey::is_chunked] = "true";
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
                        self->getContent()[HttpRequst::CilentKey::body] = body_buffer;
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
                    self->getContent()[HttpRequst::CilentKey::body] = chunked_body;
                    self->getContent()[HttpRequst::CilentKey::chunked_complete] = "true";
                    state = HttpServerUtil::ParseState::COMPLETE;
                }
                else
                {
                    // 处理可能的尾部头部
                    size_t index = tp.find(": ");
                    if (index != std::string_view::npos)
                    {
                        std::string key = std::string(HttpRequst::CilentKey::trailer_prefix) +
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
                self->getContent()[HttpRequst::CilentKey::original_content] = original_content_buffer;

                // 设置keep-alive状态
                self->getContent()[HttpRequst::CilentKey::keep_alive] =
                    connection_keep_alive ? "true" : "false";

                // 客户端解析完成，执行回调
                if (self->callback)
                {
                    self->callback(*self);
                }
                state = HttpServerUtil::ParseState::REQUEST_LINE;
                // 判断是否需要关闭连接或继续保持连接
                if (should_close_connection || !connection_keep_alive)
                {
                    // 关闭连接并退出
                    co_return;
                }
                else
                {
                    // 保持连接，清理状态准备下一个请求
                    self->getContent().clear(); // 清理上次请求的数据
                    break;                      // 跳出内层循环，开始处理下一个请求
                }
            }
            break;
            }
            co_yield {};
        }

        // 等待下一个请求或者超时
        // 这里可以添加超时逻辑，如果长时间没有新请求则关闭连接
        co_yield {};
    }
    co_return;
}

EventStatus clientFile::eventGo()
{
    cilent_socket.eventGo();
    if (!coru.done())
    {
        coru.resume();
        return EventStatus::Continue;
    }
    return EventStatus::Stop;
}
std::map<std::string, std::string>& clientFile::getContent()
{
    return con;
}
const std::map<std::string, std::string>& clientFile::getContent() const
{
    return con;
}

clientFile::clientFile(SocketFile&& sok)
    : coru(HttpClientUtil::httpEventloop(this)), cilent_socket(std::move(sok))
{
}

// 补全缺失的构造函数和方法实现
clientFile::clientFile(port target, std::string ip)
    : targetport(target), targetip(std::move(ip)), coru(HttpClientUtil::httpEventloop(this)),
      cilent_socket(SocketFile::createTcpClient(targetport, targetip))
{
}

clientFile::clientFile(clientFile&& move)
    : targetport(move.targetport), targetip(std::move(move.targetip)), con(std::move(move.con)),
      coru(std::move(move.coru)), cilent_socket(std::move(move.cilent_socket)),
      callback(std::move(move.callback))
{
    // 移动后原对象的端口设置为无效值
    move.targetport = (port)-1;
}

clientFile::clientFile(const clientFile& copy)
    : targetport(copy.targetport), targetip(copy.targetip), con(copy.con)
{
    // 注意：复制构造函数不能复制协程和socket，需要重新创建连接
    if (targetport != (port)-1 && !targetip.empty())
    {
        cilent_socket = SocketFile::createTcpClient(targetport, targetip);

        coru = HttpClientUtil::httpEventloop(this);
    }
    // callback 不复制，因为它可能包含对原对象的引用
}
void clientFile::setcallback(std::function<void(clientFile& self)>&& callback)
{
    this->callback = std::move(callback);
}