#include <httpServerFile.h>
#include <iostream>
#include <string>
#include <webSocketFile.h>

// 删除不再需要的全局函数和枚举
// enum WebSocketOpcode 已移至WebSocketFile.h
// createWebSocketFrame 已移至WebSocketFile类
// parseWebSocketFrame 已移至WebSocketFile类

int HttpServerFile::eventGo()
{
    corutine.resume();
    return fileState;
}

void HttpServerFile::closeIt()
{
    this->socketfile.closeIt();
}

int HttpServerFile::getStatus()
{
    return this->fileState;
}

// int HttpServerFile::getAgreementType()
// {
//     return protocolType; // 现在直接返回基类的protocolType
// }

HttpServerFile::HttpServerFile(int fd, std::function<void(serverFile&)> a_callback)
    : socketfile(fd), callback(a_callback)
{
    // 初始时使用HTTP协议处理
    protocolType = Agreement::HTTP;
    corutine = httpEventloop();
}

void HttpServerFile::setCallback(std::function<void(serverFile&)> a_callback)
{
    callback = a_callback;
}

int HttpServerFile::handle()
{
    callback(*this);
    return 0;
}

void HttpServerFile::write(std::string file)
{
    // 根据协议类型处理不同的写入逻辑
    if (protocolType == Agreement::WebSocket) {
        // WebSocket写入逻辑 - 使用WebSocketFile的静态方法
        std::string frame = WebSocketFile::createWebSocketFrame(true, WebSocketFile::TEXT, file);
        this->socketfile.writeFile(frame);
    } else {
        // HTTP原始写入
        return this->socketfile.writeFile(file);
    }
}

const std::map<std::string, std::string>& HttpServerFile::getContent() const
{
    return content;
}

int HttpServerFile::reset()
{
    if (protocolType == Agreement::HTTP) {
        // 重置HTTP协议状态
        content.clear();
        state = REQUEST_LINE;
        method.clear();
        path.clear();
        version.clear();
        content_length = 0;
        body_read = 0;
        body_buffer.clear();
    } 
    // WebSocket协议不需要清空content
    
    return 0;
}

bool HttpServerFile::upgradeProtocol(int newProtocol)
{
    if (newProtocol == protocolType) {
        return true; // 已经是该协议，无需切换
    }
    
    protocolType = newProtocol; // 现在直接修改基类的protocolType
    return resetCorutine();
}

bool HttpServerFile::resetCorutine()
{
    // 根据当前协议类型选择合适的协程
    if (protocolType == Agreement::HTTP) {
        corutine = httpEventloop();
    } else if (protocolType == Agreement::WebSocket) {
        corutine = wsEventloop();
    } else {
        return false;
    }
    return true;
}

// HTTP协议的事件循环
Task<void, void> HttpServerFile::httpEventloop()
{
    while (1)
    {
        int ret = socketfile.eventGo();
        if (ret == -1)
        {
            fileState = false;
            co_yield {};
        }

        if ((!socketfile.handle.context) ||
            (socketfile.handle.context->fd_state == SocketFile::WRONG))
        {
            fileState = false;
            co_yield {};
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
                size_t first_space = tp.find(' ');
                size_t second_space = tp.find(' ', first_space + 1);

                if (first_space != std::string_view::npos && second_space != std::string_view::npos)
                {
                    method = std::string(tp.substr(0, first_space));
                    path = std::string(tp.substr(first_space + 1, second_space - first_space - 1));
                    version =
                        std::string(tp.substr(second_space + 1, tp.length() - second_space - 3));

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
                if (method == "POST")
                {
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

                    content.try_emplace(key, val);
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
                    content.try_emplace("postcontent", body_buffer);
                    state = COMPLETE;
                }
            }
            break;

        case COMPLETE:
        {
            if (socketfile.handle.context &&
                socketfile.handle.context->fd_state != SocketFile::WRONG)
            {
                handle();
                state = REQUEST_LINE;
            }
        }
        break;
        }

        co_yield {};
    }
    co_return;
}

// WebSocket协议的事件循环 (从WebSocketFile的eventloop移植)
Task<void, void> HttpServerFile::wsEventloop()
{
    while (fileState)
    {
        int ret = socketfile.eventGo();
        if (ret == -1)
        {
            fileState = false;
            co_yield {};
        }

        if ((!socketfile.handle.context) ||
            (socketfile.handle.context->fd_state == SocketFile::WRONG))
        {
            fileState = false;
            co_yield {};
        }

        // 读取并处理WebSocket数据
        std::string_view data = socketfile.read_added();
        if (!data.empty())
        {
            std::string frame_data(data);
            std::string message = WebSocketFile::parseWebSocketFrame(frame_data);

            if (!message.empty())
            {
                // 检查是否是控制帧
                uint8_t opcode = frame_data[0] & 0x0F;

                if (opcode == WebSocketFile::CLOSE)
                {
                    // 关闭帧，回应关闭并终止连接
                    std::string close_frame = WebSocketFile::createWebSocketFrame(true, WebSocketFile::CLOSE, "");
                    socketfile.writeFile(close_frame);
                    fileState = false;
                }
                else if (opcode == WebSocketFile::PING)
                {
                    // Ping帧，回应Pong
                    std::string pong_frame = WebSocketFile::createWebSocketFrame(true, WebSocketFile::PONG, message);
                    socketfile.writeFile(pong_frame);
                }
                else if (opcode == WebSocketFile::TEXT || opcode == WebSocketFile::BINARY)
                {
                    // 文本或二进制帧，调用回调
                    if (callback) {
                        content["message"] = message;
                        callback(*this);
                    } else {
                        // 如果没有回调，简单回显
                        std::string response = "Echo: " + message;
                        std::string response_frame = WebSocketFile::createWebSocketFrame(true, WebSocketFile::TEXT, response);
                        socketfile.writeFile(response_frame);
                    }
                }
            }
        }

        co_yield {};
    }

    co_return;
}

// 从WebSocketFile移动的函数
bool HttpServerFile::shouldbeUpdataToWS(const serverFile& httpfile)
{
    // 获取HTTP请求头
    const auto& headers = httpfile.getContent();

    // 检查是否是GET请求，WebSocket升级必须使用GET
    auto method_it = headers.find("method");
    if (method_it == headers.end() || method_it->second != "GET")
        return false;

    // 检查Connection头是否包含"Upgrade"
    auto connection_it = headers.find("connection");
    if (connection_it == headers.end())
        return false;

    // Connection头可能有多个值，以逗号分隔，需要检查其中是否包含"upgrade"（不区分大小写）
    std::string connection_value = connection_it->second;
    std::transform(connection_value.begin(), connection_value.end(), connection_value.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (connection_value.find("upgrade") == std::string::npos)
        return false;

    // 检查Upgrade头是否为"websocket"
    auto upgrade_it = headers.find("upgrade");
    if (upgrade_it == headers.end())
        return false;

    std::string upgrade_value = upgrade_it->second;
    std::transform(upgrade_value.begin(), upgrade_value.end(), upgrade_value.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (upgrade_value != "websocket")
        return false;

    // 检查Sec-WebSocket-Key头是否存在
    auto key_it = headers.find("sec-websocket-key");
    if (key_it == headers.end() || key_it->second.empty())
        return false;

    // 检查Sec-WebSocket-Version头是否为13
    auto version_it = headers.find("sec-websocket-version");
    if (version_it == headers.end() || version_it->second != "13")
        return false;

    // 所有条件都满足，可以升级到WebSocket
    return true;
}