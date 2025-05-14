#include <httpServerFile.h>
#include <string>
#include <webSocketFile.h>

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
        // WebSocket写入逻辑 - 使用WebSocketUtil的静态方法
        std::string frame = WebSocketUtil::createWebSocketFrame(true, WebSocketUtil::TEXT, file);
        this->socketfile.writeFile(frame);
    } else {
        // HTTP原始写入
        return this->socketfile.writeFile(file);
    }
}

std::map<std::string, std::string>& HttpServerFile::getContent()
{
    return content; // 直接返回基类的content
}

const std::map<std::string, std::string>& HttpServerFile::getContent() const
{
    return content; // 返回基类的const引用
}

int HttpServerFile::reset()
{
    if (protocolType == Agreement::HTTP)
    {
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
    if (newProtocol == protocolType)
    {
        return true; // 已经是该协议，无需切换
    }

    protocolType = newProtocol; // 现在直接修改基类的protocolType
    return resetCorutine();
}

bool HttpServerFile::resetCorutine()
{
    // 根据当前协议类型选择合适的协程
    if (protocolType == Agreement::HTTP)
    {
        corutine = httpEventloop();
    }
    else if (protocolType == Agreement::WebSocket)
    {
        corutine = wsEventloop();
    }
    else
    {
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

// WebSocket协议的事件循环 - 改进版，使用状态机模式
Task<void, void> HttpServerFile::wsEventloop()
{
    // 定义WebSocket解析状态
    enum ParseState
    {
        WAITING_HEADER,
        WAITING_LENGTH,
        WAITING_MASK,
        WAITING_PAYLOAD
    };

    ParseState state = WAITING_HEADER;

    // 解析状态相关变量
    bool fin = false;
    uint8_t opcode = 0;
    bool masked = false;
    size_t payload_len = 0;
    size_t header_size = 2;
    size_t length_bytes_needed = 0;
    std::array<uint8_t, 4> mask_key = {0, 0, 0, 0};
    std::string payload;
    std::string frame_data;

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

        // 根据当前状态处理数据
        switch (state)
        {
        case WAITING_HEADER:
        {
            // 精确读取2字节的帧头
            std::string_view header_bytes = socketfile.read_num(2);
            if (header_bytes.size() < 2)
            {
                co_yield {};
                continue;
            }

            // 保存帧数据
            frame_data = std::string(header_bytes);

            // 解析基本帧头
            fin = (header_bytes[0] & 0x80) != 0;
            opcode = header_bytes[0] & 0x0F;
            masked = (header_bytes[1] & 0x80) != 0;
            uint8_t payload_len_initial = header_bytes[1] & 0x7F;

            // 根据载荷长度确定下一步
            if (payload_len_initial < 126)
            {
                payload_len = payload_len_initial;
                header_size = 2;
                state = masked ? WAITING_MASK : WAITING_PAYLOAD;
            }
            else if (payload_len_initial == 126)
            {
                length_bytes_needed = 2;
                state = WAITING_LENGTH;
            }
            else // payload_len_initial == 127
            {
                length_bytes_needed = 8;
                state = WAITING_LENGTH;
            }
            break;
        }

        case WAITING_LENGTH:
        {
            // 精确读取扩展长度字段
            std::string_view length_bytes = socketfile.read_num(length_bytes_needed);
            if (length_bytes.size() < length_bytes_needed)
            {
                co_yield {};
                continue;
            }

            // 添加到帧数据
            frame_data.append(length_bytes);

            // 解析实际长度
            if (length_bytes_needed == 2)
            {
                payload_len = ((static_cast<uint16_t>(length_bytes[0]) << 8) |
                               static_cast<uint8_t>(length_bytes[1]));
            }
            else // length_bytes_needed == 8
            {
                payload_len = 0;
                for (int i = 0; i < 8; ++i)
                {
                    payload_len = (payload_len << 8) | static_cast<uint8_t>(length_bytes[i]);
                }
            }

            state = masked ? WAITING_MASK : WAITING_PAYLOAD;
            break;
        }

        case WAITING_MASK:
        {
            // 精确读取4字节掩码
            std::string_view mask_bytes = socketfile.read_num(4);
            if (mask_bytes.size() < 4)
            {
                co_yield {};
                continue;
            }

            // 添加到帧数据
            frame_data.append(mask_bytes);

            // 保存掩码键值
            for (int i = 0; i < 4; ++i)
            {
                mask_key[i] = static_cast<uint8_t>(mask_bytes[i]);
            }

            state = WAITING_PAYLOAD;
            break;
        }

        case WAITING_PAYLOAD:
        {
            // 检查是否有有效载荷
            if (payload_len == 0)
            {
                // 没有有效载荷，直接处理帧
                switch (opcode)
                {
                case WebSocketUtil::CLOSE:
                {
                    // 回应关闭帧并终止连接
                    std::string close_frame = WebSocketUtil::createWebSocketFrame(true, WebSocketUtil::CLOSE, "");
                    socketfile.writeFile(close_frame);
                    fileState = false;
                    break;
                }
                case WebSocketUtil::PING:
                {
                    // 回应Ping帧
                    std::string pong_frame = WebSocketUtil::createWebSocketFrame(true, WebSocketUtil::PONG, "");
                    socketfile.writeFile(pong_frame);
                    break;
                }
                default:
                    break;
                }

                // 重置状态，准备处理下一帧
                state = WAITING_HEADER;
                frame_data.clear();
                break;
            }

            // 精确读取有效载荷数据
            std::string_view payload_bytes = socketfile.read_num(payload_len);
            if (payload_bytes.size() < payload_len)
            {
                co_yield {};
                continue;
            }

            // 添加到帧数据
            frame_data.append(payload_bytes);

            // 解码有效载荷
            payload.clear();
            payload.reserve(payload_len);

            for (size_t i = 0; i < payload_len; ++i)
            {
                char byte = payload_bytes[i];
                if (masked)
                {
                    byte ^= mask_key[i % 4];
                }
                payload.push_back(byte);
            }

            // 处理完整的帧
            switch (opcode)
            {
            case WebSocketUtil::CLOSE:
            {
                // 回应关闭帧并终止连接
                std::string close_frame = WebSocketUtil::createWebSocketFrame(true, WebSocketUtil::CLOSE, "");
                socketfile.writeFile(close_frame);
                fileState = false;
                break;
            }
            case WebSocketUtil::PING:
            {
                // 回应Ping帧
                std::string pong_frame = WebSocketUtil::createWebSocketFrame(true, WebSocketUtil::PONG, payload);
                socketfile.writeFile(pong_frame);
                break;
            }
            case WebSocketUtil::TEXT:
            case WebSocketUtil::BINARY:
            {
                // 更新内容
                content["message"] = payload;
                content["type"] = (opcode == WebSocketUtil::TEXT) ? "text" : "binary";

                // 如果设置了回调，则调用
                if (callback)
                {
                    callback(*this);
                }
                break;
            }
            case WebSocketUtil::PONG:
                // 收到Pong，不需要特殊处理
                break;
            case WebSocketUtil::CONTINUATION:
                // 分片消息处理（为简化此处未实现）
                break;
            default:
                // 未知操作码
                break;
            }
 
            // 重置状态，准备处理下一帧
            state = WAITING_HEADER;
            frame_data.clear();
            break;
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