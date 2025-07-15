#include "webSocketClientFile.hpp"
#include "httpServerFile.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <openssl/sha.h>
#include <random>
#include <sstream>
#include <vector>

// 初始化静态成员，自动注册WebSocket协议处理函数
bool WebSocketClientUtil::autoRegistered = WebSocketClientUtil::initialize();

// 初始化方法，注册WebSocket协议处理函数
bool WebSocketClientUtil::initialize()
{
    return clientFile::registerProtocolHandler(Protocol::WebSocket,
                                               WebSocketClientUtil::wsEventloop);
}

// WebSocket客户端事件循环
Task<void, void> WebSocketClientUtil::wsEventloop(clientFile* self)
{
    bool connection_alive = true;
    bool is_closing = false;
    uint16_t close_code = 1000;
    std::string close_reason = "";

    while (connection_alive)
    {
        SocketFile& socket = self->cilent_socket;
        EventStatus socketEventStatus = socket.eventGo();

        if (socketEventStatus == EventStatus::Stop)
        {
            connection_alive = false;
            co_yield {};
            continue;
        }

        if (socket.getSocketStatus() == SocketStatus::WRONG)
        {
            connection_alive = false;
            co_yield {};
            continue;
        }

        // 尝试读取至少2个字节（WebSocket帧头）
        std::string_view frame_header = socket.read_num(2);
        if (frame_header.length() < 2)
        {
            co_yield {};
            continue;
        }

        // 解析WebSocket帧头
        uint8_t first_byte = static_cast<uint8_t>(frame_header[0]);
        uint8_t second_byte = static_cast<uint8_t>(frame_header[1]);

        bool fin = (first_byte & 0x80) != 0;
        uint8_t opcode = first_byte & 0x0F;
        bool masked = (second_byte & 0x80) != 0; // 服务器发送的消息不应该有掩码
        size_t payload_length = second_byte & 0x7F;

        // 读取扩展的长度
        size_t header_size = 2;
        if (payload_length == 126)
        {
            std::string_view extended_length = socket.read_num(2);
            if (extended_length.length() < 2)
            {
                co_yield {};
                continue;
            }

            payload_length = static_cast<uint16_t>(static_cast<uint8_t>(extended_length[0]) << 8 |
                                                   static_cast<uint8_t>(extended_length[1]));
            header_size += 2;
        }
        else if (payload_length == 127)
        {
            std::string_view extended_length = socket.read_num(8);
            if (extended_length.length() < 8)
            {
                co_yield {};
                continue;
            }

            // 只处理低32位，因为很少会有超大消息
            payload_length = 0;
            for (int i = 4; i < 8; ++i)
            {
                payload_length = (payload_length << 8) | static_cast<uint8_t>(extended_length[i]);
            }
            header_size += 8;
        }

        // 读取掩码（如果有）
        uint32_t mask_key = 0;
        if (masked)
        {
            std::string_view mask_bytes = socket.read_num(4);
            if (mask_bytes.length() < 4)
            {
                co_yield {};
                continue;
            }

            mask_key = (static_cast<uint32_t>(static_cast<uint8_t>(mask_bytes[0])) << 24) |
                       (static_cast<uint32_t>(static_cast<uint8_t>(mask_bytes[1])) << 16) |
                       (static_cast<uint32_t>(static_cast<uint8_t>(mask_bytes[2])) << 8) |
                       static_cast<uint32_t>(static_cast<uint8_t>(mask_bytes[3]));
            header_size += 4;
        }

        // 读取有效载荷
        std::string payload;
        if (payload_length > 0)
        {
            std::string_view payload_data = socket.read_num(payload_length);
            if (payload_data.length() < payload_length)
            {
                co_yield {};
                continue;
            }

            payload = std::string(payload_data);

            // 如果有掩码，解码数据
            if (masked)
            {
                std::vector<uint8_t> payload_vec(payload.begin(), payload.end());
                uint8_t mask_bytes[4] = {static_cast<uint8_t>(mask_key >> 24),
                                         static_cast<uint8_t>((mask_key >> 16) & 0xFF),
                                         static_cast<uint8_t>((mask_key >> 8) & 0xFF),
                                         static_cast<uint8_t>(mask_key & 0xFF)};

                for (size_t i = 0; i < payload_vec.size(); ++i)
                {
                    payload_vec[i] ^= mask_bytes[i % 4];
                }

                payload = std::string(payload_vec.begin(), payload_vec.end());
            }
        }

        // 处理不同类型的WebSocket帧
        WebSocketServerUtil::WebSocketOpcode ws_opcode =
            static_cast<WebSocketServerUtil::WebSocketOpcode>(opcode);

        switch (ws_opcode)
        {
        case WebSocketServerUtil::WebSocketOpcode::TEXT:
            self->getContent()[ContentKey::message] = payload;
            self->getContent()[ContentKey::type] = ContentKey::MessageType::text;
            self->getContent()[ContentKey::is_binary] = "false";
            break;

        case WebSocketServerUtil::WebSocketOpcode::BINARY:
            self->getContent()[ContentKey::message] = payload;
            self->getContent()[ContentKey::type] = ContentKey::MessageType::binary;
            self->getContent()[ContentKey::is_binary] = "true";
            break;

        case WebSocketServerUtil::WebSocketOpcode::CLOSE:
            // 解析关闭代码和原因
            if (payload.length() >= 2)
            {
                close_code = static_cast<uint16_t>((static_cast<uint8_t>(payload[0]) << 8) |
                                                   static_cast<uint8_t>(payload[1]));
                if (payload.length() > 2)
                {
                    close_reason = payload.substr(2);
                }
            }

            self->getContent()[ContentKey::is_closed] = "true";
            self->getContent()[ContentKey::close_code] = std::to_string(close_code);
            self->getContent()[ContentKey::close_reason] = close_reason;
            self->getContent()[ContentKey::type] = ContentKey::MessageType::close;

            // 如果还没发送关闭帧，需要回应关闭帧
            if (!is_closing)
            {
                is_closing = true;
                std::string close_frame =
                    makeWebSocketFrame("", WebSocketServerUtil::WebSocketOpcode::CLOSE, true, true);
                socket.writeFile(close_frame);
            }

            connection_alive = false;
            break;

        case WebSocketServerUtil::WebSocketOpcode::PING:
            // 自动回应PONG
            {
                std::string pong_frame = makeWebSocketFrame(
                    payload, WebSocketServerUtil::WebSocketOpcode::PONG, true, true);
                socket.writeFile(pong_frame);

                self->getContent()[ContentKey::message] = payload;
                self->getContent()[ContentKey::type] = ContentKey::MessageType::ping;
            }
            break;

        case WebSocketServerUtil::WebSocketOpcode::PONG:
            self->getContent()[ContentKey::message] = payload;
            self->getContent()[ContentKey::type] = ContentKey::MessageType::pong;
            break;

        case WebSocketServerUtil::WebSocketOpcode::CONTINUATION:
            // 暂不处理分片消息，可以在实际应用中扩展
            break;
        }

        // 触发回调
        if (self->callback && (ws_opcode == WebSocketServerUtil::WebSocketOpcode::TEXT ||
                               ws_opcode == WebSocketServerUtil::WebSocketOpcode::BINARY ||
                               ws_opcode == WebSocketServerUtil::WebSocketOpcode::CLOSE))
        {
            self->callback(*self);
        }

        co_yield {};
    }

    co_return;
}

// 从HTTP客户端升级到WebSocket
bool WebSocketClientUtil::upgradeToWebSocket(clientFile& client, const std::string& host,
                                             const std::string& path,
                                             const std::map<std::string, std::string>& headers)
{
    // 生成WebSocket密钥
    std::string websocket_key = HttpServerUtil::generateWebSocketKey();
    client.getContent()[ContentKey::websocket_key] = websocket_key;

    // 创建升级请求
    std::string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "Upgrade: websocket\r\n";
    request += "Connection: Upgrade\r\n";
    request += "Sec-WebSocket-Key: " + websocket_key + "\r\n";
    request += "Sec-WebSocket-Version: 13\r\n";

    // 添加额外头部
    for (const auto& header : headers)
    {
        request += header.first + ": " + header.second + "\r\n";
    }

    request += "\r\n";

    // 发送升级请求
    client.cilent_socket.writeFile(request);

    // 此处不等待响应，由事件循环处理
    // 升级协议类型
    return client.upgradeProtocol(Protocol::WebSocket);
}

// 生成WebSocket握手请求
std::string WebSocketClientUtil::makeWebSocketHandshake(
    const std::string& path, const std::string& host,
    const std::map<std::string, std::string>& headers)
{
    // 生成随机的WebSocket密钥
    std::string websocket_key = HttpServerUtil::generateWebSocketKey();

    std::ostringstream request;
    request << "GET " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Upgrade: websocket\r\n";
    request << "Connection: Upgrade\r\n";
    request << "Sec-WebSocket-Key: " << websocket_key << "\r\n";
    request << "Sec-WebSocket-Version: 13\r\n";

    // 添加额外的头部
    for (const auto& header : headers)
    {
        request << header.first << ": " << header.second << "\r\n";
    }

    // 添加空行结束请求头
    request << "\r\n";
    return request.str();
}

// 检查是否应该升级到WebSocket
bool WebSocketClientUtil::shouldbeUpdataToWS(const clientFile& client)
{
    // 如果提供了空的客户端对象，使用线程局部静态对象存储上次处理的结果
    bool should_upgrade = false;

    // 如果传入了有效的客户端，则检查其响应是否指示应升级
    if (client.getport() != (port)-1)
    {
        const auto& content = client.getContent();

        // 检查状态码是否为101 (Switching Protocols)
        auto status_it = content.find(HttpClientUtil::CilentKey::status_code);
        if (status_it == content.end() || status_it->second != "101")
        {
            should_upgrade = false;
            return false;
        }

        // 检查Upgrade头部
        auto upgrade_it = content.find("upgrade");
        if (upgrade_it == content.end())
        {
            should_upgrade = false;
            return false;
        }

        std::string upgrade_val = upgrade_it->second;
        std::transform(upgrade_val.begin(), upgrade_val.end(), upgrade_val.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (upgrade_val.find("websocket") == std::string::npos)
        {
            should_upgrade = false;
            return false;
        }

        // 检查Connection头部
        auto connection_it = content.find("connection");
        if (connection_it == content.end())
        {
            should_upgrade = false;
            return false;
        }

        std::string connection_val = connection_it->second;
        std::transform(connection_val.begin(), connection_val.end(), connection_val.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (connection_val.find("upgrade") == std::string::npos)
        {
            should_upgrade = false;
            return false;
        }

        // 检查Sec-WebSocket-Accept头部
        auto accept_it = content.find("sec-websocket-accept");
        if (accept_it == content.end())
        {
            should_upgrade = false;
            return false;
        }

        // 所有条件都满足，应该升级
        should_upgrade = true;
    }

    return should_upgrade;
}

// WebSocket帧构造
std::string WebSocketClientUtil::makeWebSocketFrame(const std::string& payload,
                                                    WebSocketServerUtil::WebSocketOpcode opcode,
                                                    bool fin, bool masked)
{
    std::vector<uint8_t> frame;

    // 第一个字节：FIN位 + 保留位 + 操作码
    uint8_t first_byte = static_cast<uint8_t>(opcode);
    if (fin)
    {
        first_byte |= 0x80;
    }
    frame.push_back(first_byte);

    // 第二个字节：MASK位 + 负载长度
    uint8_t second_byte = 0;
    if (masked)
    {
        second_byte |= 0x80;
    }

    // 确定长度编码方式
    size_t payload_length = payload.length();
    if (payload_length <= 125)
    {
        second_byte |= static_cast<uint8_t>(payload_length);
        frame.push_back(second_byte);
    }
    else if (payload_length <= 65535)
    {
        second_byte |= 126;
        frame.push_back(second_byte);

        // 添加16位长度
        frame.push_back(static_cast<uint8_t>((payload_length >> 8) & 0xFF));
        frame.push_back(static_cast<uint8_t>(payload_length & 0xFF));
    }
    else
    {
        second_byte |= 127;
        frame.push_back(second_byte);

        // 添加64位长度（只使用低32位）
        for (int i = 0; i < 4; ++i)
        {
            frame.push_back(0); // 高32位设为0
        }
        frame.push_back(static_cast<uint8_t>((payload_length >> 24) & 0xFF));
        frame.push_back(static_cast<uint8_t>((payload_length >> 16) & 0xFF));
        frame.push_back(static_cast<uint8_t>((payload_length >> 8) & 0xFF));
        frame.push_back(static_cast<uint8_t>(payload_length & 0xFF));
    }

    // 如果需要掩码，添加掩码密钥
    uint32_t mask_key = 0;
    if (masked)
    {
        mask_key = generateMaskKey();
        frame.push_back(static_cast<uint8_t>((mask_key >> 24) & 0xFF));
        frame.push_back(static_cast<uint8_t>((mask_key >> 16) & 0xFF));
        frame.push_back(static_cast<uint8_t>((mask_key >> 8) & 0xFF));
        frame.push_back(static_cast<uint8_t>(mask_key & 0xFF));
    }

    // 添加负载数据
    if (!payload.empty())
    {
        std::vector<uint8_t> payload_bytes(payload.begin(), payload.end());

        // 如果需要掩码，对数据进行掩码处理
        if (masked)
        {
            maskData(payload_bytes, mask_key);
        }

        frame.insert(frame.end(), payload_bytes.begin(), payload_bytes.end());
    }

    return std::string(frame.begin(), frame.end());
}

// WebSocket帧解析
std::pair<std::string, WebSocketServerUtil::WebSocketOpcode> WebSocketClientUtil::
    parseWebSocketFrame(const std::string& frame, bool& fin)
{
    if (frame.length() < 2)
    {
        return {"", WebSocketServerUtil::WebSocketOpcode::CLOSE};
    }

    // 解析第一个字节
    uint8_t first_byte = static_cast<uint8_t>(frame[0]);
    fin = (first_byte & 0x80) != 0;
    WebSocketServerUtil::WebSocketOpcode opcode =
        static_cast<WebSocketServerUtil::WebSocketOpcode>(first_byte & 0x0F);

    // 解析第二个字节
    uint8_t second_byte = static_cast<uint8_t>(frame[1]);
    bool masked = (second_byte & 0x80) != 0;
    size_t payload_length = second_byte & 0x7F;

    // 计算头部长度
    size_t header_size = 2;

    // 如果有扩展长度
    if (payload_length == 126)
    {
        if (frame.length() < 4)
        {
            return {"", WebSocketServerUtil::WebSocketOpcode::CLOSE};
        }
        payload_length = static_cast<uint16_t>(static_cast<uint8_t>(frame[2]) << 8 |
                                               static_cast<uint8_t>(frame[3]));
        header_size += 2;
    }
    else if (payload_length == 127)
    {
        if (frame.length() < 10)
        {
            return {"", WebSocketServerUtil::WebSocketOpcode::CLOSE};
        }

        // 只处理低32位
        payload_length = 0;
        for (int i = 6; i < 10; ++i)
        {
            payload_length = (payload_length << 8) | static_cast<uint8_t>(frame[i]);
        }
        header_size += 8;
    }

    // 处理掩码
    uint32_t mask_key = 0;
    if (masked)
    {
        if (frame.length() < header_size + 4)
        {
            return {"", WebSocketServerUtil::WebSocketOpcode::CLOSE};
        }

        mask_key = (static_cast<uint32_t>(static_cast<uint8_t>(frame[header_size])) << 24) |
                   (static_cast<uint32_t>(static_cast<uint8_t>(frame[header_size + 1])) << 16) |
                   (static_cast<uint32_t>(static_cast<uint8_t>(frame[header_size + 2])) << 8) |
                   static_cast<uint32_t>(static_cast<uint8_t>(frame[header_size + 3]));
        header_size += 4;
    }

    // 检查帧是否完整
    if (frame.length() < header_size + payload_length)
    {
        return {"", WebSocketServerUtil::WebSocketOpcode::CLOSE};
    }

    // 获取负载数据
    std::string payload = frame.substr(header_size, payload_length);

    // 如果有掩码，解码数据
    if (masked && !payload.empty())
    {
        std::vector<uint8_t> payload_vec(payload.begin(), payload.end());
        uint8_t mask_bytes[4] = {
            static_cast<uint8_t>(mask_key >> 24), static_cast<uint8_t>((mask_key >> 16) & 0xFF),
            static_cast<uint8_t>((mask_key >> 8) & 0xFF), static_cast<uint8_t>(mask_key & 0xFF)};

        for (size_t i = 0; i < payload_vec.size(); ++i)
        {
            payload_vec[i] ^= mask_bytes[i % 4];
        }

        payload = std::string(payload_vec.begin(), payload_vec.end());
    }

    return {payload, opcode};
}

// 生成掩码密钥
uint32_t WebSocketClientUtil::generateMaskKey()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist;
    return dist(gen);
}

// 掩码数据处理
void WebSocketClientUtil::maskData(std::vector<uint8_t>& data, uint32_t mask)
{
    uint8_t mask_bytes[4] = {
        static_cast<uint8_t>((mask >> 24) & 0xFF), static_cast<uint8_t>((mask >> 16) & 0xFF),
        static_cast<uint8_t>((mask >> 8) & 0xFF), static_cast<uint8_t>(mask & 0xFF)};

    for (size_t i = 0; i < data.size(); ++i)
    {
        data[i] ^= mask_bytes[i % 4];
    }
}

// WebSocketMessage实现
WebSocketMessage::WebSocketMessage(WebSocketServerUtil::WebSocketOpcode opcode, bool fin,
                                   bool masked)
    : opcode_(opcode), fin_(fin), masked_(masked), mask_key_(0)
{
    if (masked)
    {
        mask_key_ = WebSocketClientUtil::generateMaskKey();
    }
}

WebSocketMessage& WebSocketMessage::withContent(const std::string& content)
{
    content_ = content;
    return *this;
}

WebSocketMessage& WebSocketMessage::withMaskKey(uint32_t key)
{
    mask_key_ = key;
    return *this;
}

WebSocketMessage WebSocketMessage::text(const std::string& content)
{
    return WebSocketMessage(WebSocketServerUtil::WebSocketOpcode::TEXT).withContent(content);
}

WebSocketMessage WebSocketMessage::binary(const std::string& content)
{
    return WebSocketMessage(WebSocketServerUtil::WebSocketOpcode::BINARY).withContent(content);
}

WebSocketMessage WebSocketMessage::ping(const std::string& content)
{
    return WebSocketMessage(WebSocketServerUtil::WebSocketOpcode::PING).withContent(content);
}

WebSocketMessage WebSocketMessage::pong(const std::string& content)
{
    return WebSocketMessage(WebSocketServerUtil::WebSocketOpcode::PONG).withContent(content);
}

WebSocketMessage WebSocketMessage::close(uint16_t code, const std::string& reason)
{
    std::string payload;
    payload.push_back(static_cast<char>((code >> 8) & 0xFF));
    payload.push_back(static_cast<char>(code & 0xFF));
    payload.append(reason);

    return WebSocketMessage(WebSocketServerUtil::WebSocketOpcode::CLOSE).withContent(payload);
}

WebSocketMessage::operator std::string() const
{
    return toString();
}

std::string WebSocketMessage::toString() const
{
    return WebSocketClientUtil::makeWebSocketFrame(content_, opcode_, fin_, masked_);
}

// // WebSocketClient实现
// WebSocketClient::WebSocketClient(const std::string& host, int port)
//     : host_(host), port_(port), client_(port, host)
// {
// }

// bool WebSocketClient::connect(const std::string& path, const std::map<std::string, std::string>&
// headers)
// {
//     if (connected_)
//     {
//         return true;
//     }

//     // 尝试连接
//     if (client_.cilent_socket.getSocketStatus() == SocketStatus::WRONG)
//     {
//         return false;
//     }

//     // 设置消息回调
//     client_.setcallback([this](clientFile& client) {
//         if (client.getAgreementType() == Protocol::HTTP)
//         {
//             // HTTP响应回调，检查升级是否成功
//             auto& content = client.getContent();
//             if (content.find(HttpClientUtil::CilentKey::status_code) != content.end() &&
//                 content[HttpClientUtil::CilentKey::status_code] == "101")
//             {
//                 connected_ = true;

//                 // 检查WebSocket接受密钥是否匹配
//                 auto key_it = content.find(WebSocketClientUtil::ContentKey::websocket_key);
//                 auto accept_it = content.find("sec-websocket-accept");
//                 if (key_it != content.end() && accept_it != content.end())
//                 {
//                     // 可以在此验证接受密钥是否正确
//                     content[WebSocketClientUtil::ContentKey::websocket_accept] =
//                     accept_it->second;
//                 }
//             }
//         }
//         else if (client.getAgreementType() == Protocol::WebSocket)
//         {
//             // WebSocket消息回调
//             auto& content = client.getContent();

//             if (content.find(WebSocketClientUtil::ContentKey::type) != content.end())
//             {
//                 std::string msg_type = content[WebSocketClientUtil::ContentKey::type];

//                 if (msg_type == WebSocketClientUtil::ContentKey::MessageType::close)
//                 {
//                     connected_ = false;
//                     if (close_callback_)
//                     {
//                         close_callback_();
//                     }
//                 }
//                 else if (message_callback_)
//                 {
//                     // 创建消息对象并回调
//                     WebSocketServerUtil::WebSocketOpcode opcode =
//                     WebSocketServerUtil::WebSocketOpcode::TEXT; if (msg_type ==
//                     WebSocketClientUtil::ContentKey::MessageType::binary)
//                     {
//                         opcode = WebSocketServerUtil::WebSocketOpcode::BINARY;
//                     }
//                     else if (msg_type == WebSocketClientUtil::ContentKey::MessageType::ping)
//                     {
//                         opcode = WebSocketServerUtil::WebSocketOpcode::PING;
//                     }
//                     else if (msg_type == WebSocketClientUtil::ContentKey::MessageType::pong)
//                     {
//                         opcode = WebSocketServerUtil::WebSocketOpcode::PONG;
//                     }

//                     std::string message = content.find(WebSocketClientUtil::ContentKey::message)
//                     != content.end() ?
//                                          content[WebSocketClientUtil::ContentKey::message] : "";

//                     WebSocketMessage wsm(opcode);
//                     wsm.withContent(message);

//                     message_callback_(wsm);
//                 }
//             }
//         }
//     });

//     // 升级到WebSocket
//     return WebSocketClientUtil::upgradeToWebSocket(client_, host_, path, headers);
// }

// bool WebSocketClient::sendMessage(const WebSocketMessage& message)
// {
//     if (!connected_)
//     {
//         return false;
//     }

//     client_.cilent_socket.writeFile(message.toString());
//     return true;
// }

// bool WebSocketClient::sendText(const std::string& text)
// {
//     return sendMessage(WebSocketMessage::text(text));
// }

// bool WebSocketClient::sendBinary(const std::string& binary_data)
// {
//     return sendMessage(WebSocketMessage::binary(binary_data));
// }

// bool WebSocketClient::sendPing(const std::string& data)
// {
//     return sendMessage(WebSocketMessage::ping(data));
// }

// bool WebSocketClient::sendPong(const std::string& data)
// {
//     return sendMessage(WebSocketMessage::pong(data));
// }

// bool WebSocketClient::close(uint16_t code, const std::string& reason)
// {
//     if (!connected_)
//     {
//         return false;
//     }

//     bool result = sendMessage(WebSocketMessage::close(code, reason));
//     connected_ = false;
//     return result;
// }

// void WebSocketClient::setMessageCallback(std::function<void(const WebSocketMessage&)> callback)
// {
//     message_callback_ = std::move(callback);
// }

// void WebSocketClient::setCloseCallback(std::function<void()> callback)
// {
//     close_callback_ = std::move(callback);
// }

// void WebSocketClient::handleEvents()
// {
//     client_.eventGo();
// }
