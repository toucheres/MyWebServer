#include "webSocketFile.h"
#include "protocol_constants.h" // 新增包含
#include "serverFile.h"
#include <algorithm>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <string>

// 初始化静态成员，自动注册WebSocket协议处理函数
bool WebSocketUtil::autoRegistered = WebSocketUtil::initialize();

// 初始化方法，注册WebSocket协议处理函数
bool WebSocketUtil::initialize()
{
    return serverFile::registerProtocolHandler(Protocol::WebSocket,
                                               WebSocketUtil::wsEventloop); // 使用 Protocol 枚举
}

// WebSocket帧操作
std::string WebSocketUtil::makeWebSocketFrame(const std::string& payload, WebSocketOpcode opcode,
                                              bool fin, bool masked)
{
    std::string frame;

    // 第一个字节: FIN位和操作码
    uint8_t byte1 = (fin ? 0x80 : 0x00) | (static_cast<uint8_t>(opcode) & 0x0F);
    frame.push_back(byte1);

    // 第二个字节: MASK位和负载长度
    uint8_t byte2 = 0;
    if (masked)
        byte2 |= 0x80;

    // 设置长度
    size_t payload_len = payload.size();
    if (payload_len < 126)
    {
        byte2 |= payload_len;
        frame.push_back(byte2);
    }
    else if (payload_len <= 0xFFFF)
    {
        byte2 |= 126;
        frame.push_back(byte2);

        // 16位长度
        frame.push_back((payload_len >> 8) & 0xFF);
        frame.push_back(payload_len & 0xFF);
    }
    else
    {
        byte2 |= 127;
        frame.push_back(byte2);

        // 64位长度
        for (int i = 7; i >= 0; --i)
        {
            frame.push_back((payload_len >> (i * 8)) & 0xFF);
        }
    }

    // 如果有掩码，添加掩码键和掩码数据
    if (masked)
    {
        // 生成随机掩码键
        uint8_t mask_key[4];
        for (int i = 0; i < 4; ++i)
        {
            mask_key[i] = rand() % 256;
            frame.push_back(mask_key[i]);
        }

        // 添加掩码后的负载
        for (size_t i = 0; i < payload.size(); ++i)
        {
            frame.push_back(payload[i] ^ mask_key[i % 4]);
        }
    }
    else
    {
        // 添加原始负载
        frame.append(payload);
    }

    return frame;
}

std::string WebSocketUtil::parseWebSocketFrame(const std::string& frame)
{
    // 只实现基本解析，实际应用中需要完整处理
    if (frame.size() < 2)
        return "";

    // 读取第一个字节，获取FIN位和操作码
    uint8_t byte1 = static_cast<uint8_t>(frame[0]);
    // bool fin = (byte1 & 0x80) != 0; // fin not used in this simplified parser
    // uint8_t opcode = byte1 & 0x0F; // opcode not used

    // 读取第二个字节，获取MASK位和负载长度
    uint8_t byte2 = static_cast<uint8_t>(frame[1]);
    bool masked = (byte2 & 0x80) != 0;
    uint64_t payload_len = byte2 & 0x7F;

    // 处理扩展长度
    size_t header_size = 2;
    if (payload_len == 126)
    {
        if (frame.size() < 4)
            return "";
        payload_len = (static_cast<uint8_t>(frame[2]) << 8) | static_cast<uint8_t>(frame[3]);
        header_size = 4;
    }
    else if (payload_len == 127)
    {
        if (frame.size() < 10)
            return "";
        payload_len = 0;
        for (int i = 0; i < 8; ++i)
        {
            payload_len = (payload_len << 8) | static_cast<uint8_t>(frame[2 + i]);
        }
        header_size = 10;
    }

    // 掩码处理
    if (masked)
    {
        if (frame.size() < header_size + 4)
            return "";

        // 提取掩码键
        uint8_t mask_key[4];
        for (int i = 0; i < 4; ++i)
        {
            mask_key[i] = static_cast<uint8_t>(frame[header_size + i]);
        }
        header_size += 4;

        // 提取并解码负载
        std::string payload;
        for (size_t i = 0; i < payload_len; ++i)
        {
            if (header_size + i >= frame.size())
                break;
            payload.push_back(frame[header_size + i] ^ mask_key[i % 4]);
        }
        return payload;
    }
    else
    {
        // 直接提取无掩码负载
        if (header_size + payload_len > frame.size())
        {
            payload_len = frame.size() - header_size;
        }
        return frame.substr(header_size, payload_len);
    }
}

// Base64编码
std::string base64_encode(const unsigned char* input, size_t length)
{
    BIO* bio;
    BIO* b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // 不换行
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    BIO_write(bio, input, static_cast<int>(length));
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string result(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);

    return result;
}

// WebSocket握手响应
std::string WebSocketUtil::makeWebSocketHandshake(const std::string& clientKey)
{
    // 按照WebSocket规范，将客户端键与WebSocket GUID连接
    const char* guid = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string concat = clientKey + guid;

    // 计算SHA-1哈希
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(concat.c_str()), concat.length(), hash);

    // Base64编码
    std::string encoded = base64_encode(hash, SHA_DIGEST_LENGTH);

    // 构建HTTP响应
    std::string response = "HTTP/1.1 101 Switching Protocols\r\n"
                           "Upgrade: websocket\r\n"
                           "Connection: Upgrade\r\n"
                           "Sec-WebSocket-Accept: " +
                           encoded +
                           "\r\n"
                           "\r\n";

    return response;
}

std::string WebSocketUtil::makeWebSocketHandshake(const serverFile& client)
{
    const auto& headers = client.getContent();
    auto key_it = headers.find("sec-websocket-key");
    // 获取客户端发送的key
    if (key_it == headers.end())
    {
        return "";
    }
    std::string clientKey = key_it->second;
    // 使用WebSocketUtil的方法生成握手响应
    return WebSocketUtil::makeWebSocketHandshake(clientKey);
}

// 判断是否应该升级到WebSocket
bool WebSocketUtil::shouldbeUpdataToWS(const serverFile& httpfile)
{
    const auto& headers = httpfile.getContent();

    // 检查必要的头部
    auto connection = headers.find("connection");
    auto upgrade = headers.find("upgrade");
    auto key = headers.find("sec-websocket-key");
    auto version = headers.find("sec-websocket-version");

    if (connection == headers.end() || upgrade == headers.end() || key == headers.end() ||
        version == headers.end())
    {
        return false;
    }

    // 检查头部值
    bool connectionUpgrade = false;
    std::string connectionValue = connection->second;
    std::transform(connectionValue.begin(), connectionValue.end(), connectionValue.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (connectionValue.find("upgrade") != std::string::npos)
    {
        connectionUpgrade = true;
    }

    std::string upgradeValue = upgrade->second;
    std::transform(upgradeValue.begin(), upgradeValue.end(), upgradeValue.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return connectionUpgrade && upgradeValue == "websocket" && !key->second.empty();
}

// [TODO]升级多段处理
// [TODO]添加区分正常事件接收与关闭连接的回调
// WebSocket事件循环
Task<void, void> WebSocketUtil::wsEventloop(serverFile* self)
{
    // 定义WebSocket解析状态
    enum class ParseState
    {
        WAITING_HEADER,
        WAITING_LENGTH,
        WAITING_MASK,
        WAITING_PAYLOAD
    };

    ParseState state = ParseState::WAITING_HEADER;

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

    while (self->getStatus())
    {
        SocketFile& sfile = self->getSocketFile();
        EventStatus socketEventStatus = sfile.eventGo();

        if (socketEventStatus == EventStatus::Stop ||
            sfile.getSocketStatus() == SocketStatus::WRONG)
        {
            self->setFileState(false);
            co_yield {};
            continue;
        }

        // 根据当前状态处理数据
        switch (state)
        {
        case ParseState::WAITING_HEADER:
        {
            // 精确读取2字节的帧头
            std::string_view header_bytes = sfile.read_num(2);
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
                state = masked ? ParseState::WAITING_MASK : ParseState::WAITING_PAYLOAD;
            }
            else if (payload_len_initial == 126)
            {
                length_bytes_needed = 2;
                state = ParseState::WAITING_LENGTH;
            }
            else // payload_len_initial == 127
            {
                length_bytes_needed = 8;
                state = ParseState::WAITING_LENGTH;
            }
            break;
        }

        case ParseState::WAITING_LENGTH:
        {
            // 精确读取扩展长度字段
            std::string_view length_bytes = sfile.read_num(length_bytes_needed);
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
                payload_len = ((static_cast<uint16_t>(static_cast<uint8_t>(length_bytes[0])) << 8) |
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

            state = masked ? ParseState::WAITING_MASK : ParseState::WAITING_PAYLOAD;
            break;
        }

        case ParseState::WAITING_MASK:
        {
            // 精确读取4字节掩码
            std::string_view mask_bytes = sfile.read_num(4);
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

            state = ParseState::WAITING_PAYLOAD;
            break;
        }

        case ParseState::WAITING_PAYLOAD:
        {
            // 检查是否有有效载荷
            if (payload_len == 0)
            {
                // 没有有效载荷，直接处理帧
                // 处理完整的帧，根据opcode执行相应操作
                switch (opcode)
                {
                case static_cast<uint8_t>(WebSocketOpcode::CLOSE):
                {
                    // 回应关闭帧并终止连接
                    self->write(std::string(WebSocketResponse::close()));
                    self->setFileState(false);
                    if (callcbackWhenClose)
                    {
                        self->getContent()["message"] = "";
                        self->getContent()["type"] = "close";
                        self->handle();
                    }
                    break;
                }
                case static_cast<uint8_t>(WebSocketOpcode::PING):
                {
                    // 回应Ping帧
                    self->write(std::string(WebSocketResponse::pong("")));
                    break;
                }
                // 空载荷的文本或二进制消息
                case static_cast<uint8_t>(WebSocketOpcode::TEXT):
                    [[fallthrough]];
                case static_cast<uint8_t>(WebSocketOpcode::BINARY):
                {
                    // 更新内容
                    self->getContent()["message"] = "";
                    self->getContent()["type"] =
                        (opcode == static_cast<uint8_t>(WebSocketOpcode::TEXT)) ? "text" : "binary";

                    // 调用回调处理消息
                    self->handle();
                    break;
                }
                }

                // 重置状态，准备处理下一帧
                state = ParseState::WAITING_HEADER;
                frame_data.clear();
                break;
            }

            // 精确读取有效载荷数据
            std::string_view payload_bytes = sfile.read_num(payload_len);
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

            // 处理完整的帧，根据opcode执行相应操作
            switch (opcode)
            {
            case static_cast<uint8_t>(WebSocketOpcode::CLOSE):
            {
                // 解析关闭代码和原因（如果存在）
                uint16_t close_code = 1000; // 默认正常关闭
                std::string reason;

                if (payload.size() >= 2)
                {
                    close_code = (static_cast<uint16_t>(static_cast<uint8_t>(payload[0])) << 8) |
                                 static_cast<uint8_t>(payload[1]);
                    if (payload.size() > 2)
                    {
                        reason = payload.substr(2);
                    }
                }
                // 回应关闭帧并终止连接
                self->write(std::string(WebSocketResponse::close(close_code, reason)));
                self->setFileState(false);
                if (callcbackWhenClose) [[unlikely]]
                {
                    self->getContent()["message"] = "";
                    self->getContent()["type"] = "close";
                    self->handle();
                }
                break;
            }
            case static_cast<uint8_t>(WebSocketOpcode::PING):
            {
                // 回应Ping帧，将接收到的载荷作为Pong的载荷
                self->write(std::string(WebSocketResponse::pong(payload)));
                break;
            }
            case static_cast<uint8_t>(WebSocketOpcode::TEXT):
                [[fallthrough]];
            case static_cast<uint8_t>(WebSocketOpcode::BINARY):
            {
                // 更新内容
                self->getContent()["message"] = payload;
                self->getContent()["type"] =
                    (opcode == static_cast<uint8_t>(WebSocketOpcode::TEXT)) ? "text" : "binary";
                // 调用回调处理消息
                self->handle();
                self->getContent()["message"] = "";
                break;
            }
            case static_cast<uint8_t>(WebSocketOpcode::PONG):
                // 收到Pong，不需要特殊处理
                break;
            case static_cast<uint8_t>(WebSocketOpcode::CONTINUATION):
                self->getContent()["message"] += payload;
                break;
            default:
                // 未知操作码，发送协议错误的关闭帧
                self->write(
                    std::string(WebSocketResponse::close(1002, "Protocol error: unknown opcode")));
                break;
            }

            // 重置状态，准备处理下一帧
            state = ParseState::WAITING_HEADER;
            frame_data.clear();
            break;
        }
        } // end of switch

        co_yield {};
    }

    co_return;
}

// WebSocketResponse实现
WebSocketResponse::WebSocketResponse(WebSocketUtil::WebSocketOpcode opcode, bool fin, bool masked)
    : opcode_(opcode), fin_(fin), masked_(masked)
{
}

WebSocketResponse& WebSocketResponse::with_content(const std::string& new_content)
{
    content_ = new_content;
    return *this;
}

WebSocketResponse WebSocketResponse::text(const std::string& content)
{
    return WebSocketResponse(WebSocketUtil::WebSocketOpcode::TEXT).with_content(content);
}

WebSocketResponse WebSocketResponse::binary(const std::string& content)
{
    return WebSocketResponse(WebSocketUtil::WebSocketOpcode::BINARY).with_content(content);
}

WebSocketResponse WebSocketResponse::ping(const std::string& content)
{
    return WebSocketResponse(WebSocketUtil::WebSocketOpcode::PING).with_content(content);
}

WebSocketResponse WebSocketResponse::pong(const std::string& content)
{
    return WebSocketResponse(WebSocketUtil::WebSocketOpcode::PONG).with_content(content);
}

WebSocketResponse WebSocketResponse::close(uint16_t code, const std::string& reason)
{
    std::string payload;
    if (code > 0)
    {
        // 添加关闭代码（网络字节序）
        payload.push_back(static_cast<char>((code >> 8) & 0xFF));
        payload.push_back(static_cast<char>(code & 0xFF));
        // 添加关闭原因
        payload.append(reason);
    }
    return WebSocketResponse(WebSocketUtil::WebSocketOpcode::CLOSE).with_content(payload);
}

WebSocketResponse::operator std::string() const
{
    // 利用WebSocketUtil的createWebSocketFrame方法
    return WebSocketUtil::makeWebSocketFrame(content_, opcode_, fin_, masked_);
}