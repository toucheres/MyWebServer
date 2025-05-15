#include <openssl/evp.h>
#include <string>
#include <webSocketFile.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <array>
#include <httpServerFile.h>
#include <algorithm> // 添加algorithm头文件以使用std::transform

// 简单的WebSocket帧创建函数
std::string WebSocketUtil::createWebSocketFrame(bool fin, uint8_t opcode, const std::string& payload,
                                 bool masked)
{
    std::string frame;
    frame.reserve(10 + payload.size()); // 预留空间避免重新分配

    // 第一个字节: FIN(1位) + RSV1-3(3位) + Opcode(4位)
    uint8_t first_byte = opcode & 0x0F;
    if (fin)
    {
        first_byte |= 0x80;
    }
    frame.push_back(static_cast<char>(first_byte));

    // 第二个字节: MASK(1位) + Payload长度(7位)
    uint8_t second_byte = 0;
    if (masked)
    {
        second_byte |= 0x80;
    }

    // 处理payload长度
    if (payload.size() < 126)
    {
        second_byte |= payload.size();
        frame.push_back(static_cast<char>(second_byte));
    }
    else if (payload.size() <= 0xFFFF)
    {
        second_byte |= 126;
        frame.push_back(static_cast<char>(second_byte));
        frame.push_back(static_cast<char>((payload.size() >> 8) & 0xFF));
        frame.push_back(static_cast<char>(payload.size() & 0xFF));
    }
    else
    {
        second_byte |= 127;
        frame.push_back(static_cast<char>(second_byte));

        // 64位长度
        for (int i = 7; i >= 0; --i)
        {
            frame.push_back(static_cast<char>((payload.size() >> (i * 8)) & 0xFF));
        }
    }

    // 添加payload
    frame.append(payload);

    return frame;
}

// 解析WebSocket帧的简单实现
std::string WebSocketUtil::parseWebSocketFrame(const std::string& frame)
{
    if (frame.empty())
    {
        return "";
    }

    // 检查是否有足够的数据
    if (frame.size() < 2)
    {
        return "";
    }

    // 获取帧头信息
    bool fin = (frame[0] & 0x80) != 0;
    uint8_t opcode = frame[0] & 0x0F;
    bool masked = (frame[1] & 0x80) != 0;
    size_t payload_len = frame[1] & 0x7F;

    size_t header_size = 2;

    // 扩展长度
    if (payload_len == 126)
    {
        if (frame.size() < 4)
        {
            return "";
        }
        payload_len = ((static_cast<uint16_t>(frame[2]) << 8) | static_cast<uint8_t>(frame[3]));
        header_size = 4;
    }
    else if (payload_len == 127)
    {
        if (frame.size() < 10)
        {
            return "";
        }
        payload_len = 0;
        for (int i = 0; i < 8; ++i)
        {
            payload_len = (payload_len << 8) | static_cast<uint8_t>(frame[2 + i]);
        }
        header_size = 10;
    }

    // 掩码
    std::array<uint8_t, 4> mask_key = {0, 0, 0, 0};
    if (masked)
    {
        if (frame.size() < header_size + 4)
        {
            return "";
        }
        for (int i = 0; i < 4; ++i)
        {
            mask_key[i] = static_cast<uint8_t>(frame[header_size + i]);
        }
        header_size += 4;
    }

    // 检查是否有完整的payload
    if (frame.size() < header_size + payload_len)
    {
        return "";
    }

    // 解析payload
    std::string payload;
    payload.reserve(payload_len);

    for (size_t i = 0; i < payload_len; ++i)
    {
        char byte = frame[header_size + i];
        if (masked)
        {
            byte ^= mask_key[i % 4];
        }
        payload.push_back(byte);
    }

    return payload;
}

// 创建WebSocket握手响应的方法
std::string WebSocketUtil::createWebSocketHandshake(const std::string& clientKey)
{
    // 将key与魔术字符串连接
    std::string combined = clientKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    // 计算SHA1哈希
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(combined.c_str()),
         combined.length(), hash);

    // Base64编码
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, hash, SHA_DIGEST_LENGTH);
    BIO_flush(b64);

    BUF_MEM* bptr;
    BIO_get_mem_ptr(b64, &bptr);
    std::string acceptKey(bptr->data, bptr->length);
    BIO_free_all(b64);
    
    // 构建WebSocket握手响应
    std::string response = "HTTP/1.1 101 Switching Protocols\r\n"
                           "Upgrade: websocket\r\n"
                           "Connection: Upgrade\r\n"
                           "Sec-WebSocket-Accept: " +
                           acceptKey + "\r\n\r\n";
    
    return response;
}

// 从HttpServerFile移动的WebSocket事件循环方法
Task<void, void> WebSocketUtil::wsEventloop(serverFile* self)
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

    // 获取HttpServerFile对象的fileState成员
    HttpServerFile* httpSelf = dynamic_cast<HttpServerFile*>(self);
    if (!httpSelf) {
        co_return;  // 如果不是HttpServerFile，直接返回
    }

    int& fileState = httpSelf->fileState;

    while (fileState)
    {
        int ret = self->socketfile.eventGo();
        if (ret == -1)
        {
            fileState = false;
            co_yield {};
        }

        if ((!self->socketfile.handle.context) ||
            (self->socketfile.handle.context->fd_state == SocketFile::WRONG))
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
            std::string_view header_bytes = self->socketfile.read_num(2);
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
            std::string_view length_bytes = self->socketfile.read_num(length_bytes_needed);
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
            std::string_view mask_bytes = self->socketfile.read_num(4);
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
                    std::string close_frame =
                        WebSocketUtil::createWebSocketFrame(true, WebSocketUtil::CLOSE, "");
                    self->socketfile.writeFile(close_frame);
                    fileState = false;
                    break;
                }
                case WebSocketUtil::PING:
                {
                    // 回应Ping帧
                    std::string pong_frame =
                        WebSocketUtil::createWebSocketFrame(true, WebSocketUtil::PONG, "");
                    self->socketfile.writeFile(pong_frame);
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
            std::string_view payload_bytes = self->socketfile.read_num(payload_len);
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
                std::string close_frame =
                    WebSocketUtil::createWebSocketFrame(true, WebSocketUtil::CLOSE, "");
                self->socketfile.writeFile(close_frame);
                fileState = false;
                break;
            }
            case WebSocketUtil::PING:
            {
                // 回应Ping帧
                std::string pong_frame =
                    WebSocketUtil::createWebSocketFrame(true, WebSocketUtil::PONG, payload);
                self->socketfile.writeFile(pong_frame);
                break;
            }
            case WebSocketUtil::TEXT:
            case WebSocketUtil::BINARY:
            {
                // 更新内容
                self->getContent()["message"] = payload;
                self->getContent()["type"] = (opcode == WebSocketUtil::TEXT) ? "text" : "binary";

                // 如果设置了回调，则调用
                if (httpSelf->callback)
                {
                    httpSelf->callback(*self);
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

// 从HttpServerFile移动过来的方法
bool WebSocketUtil::shouldbeUpdataToWS(const serverFile& httpfile)
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