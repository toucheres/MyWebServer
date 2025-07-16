#include "webSocketClientFile.hpp"
#include "httpClientFile.h"
#include "httpMessage.h"
#include "httpServerFile.h"
#include <algorithm>
#include <cstring>
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
        WebSocketOpcode ws_opcode = static_cast<WebSocketOpcode>(opcode);

        switch (ws_opcode)
        {
        case WebSocketOpcode::TEXT:
            self->getContent()[ContentKey::message] = payload;
            self->getContent()[ContentKey::type] = ContentKey::MessageType::text;
            self->getContent()[ContentKey::is_binary] = "false";
            break;

        case WebSocketOpcode::BINARY:
            self->getContent()[ContentKey::message] = payload;
            self->getContent()[ContentKey::type] = ContentKey::MessageType::binary;
            self->getContent()[ContentKey::is_binary] = "true";
            break;

        case WebSocketOpcode::CLOSE:
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

            if (!is_closing)
            {
                is_closing = true;
                std::string close_frame =
                    WebSocketRequst::makeWebSocketFrame("", WebSocketOpcode::CLOSE, true, true);
                socket.writeFile(close_frame);
            }

            connection_alive = false;
            break;

        case WebSocketOpcode::PING:
        {
            std::string pong_frame =
                WebSocketRequst::makeWebSocketFrame(payload, WebSocketOpcode::PONG, true, true);
            socket.writeFile(pong_frame);

            self->getContent()[ContentKey::message] = payload;
            self->getContent()[ContentKey::type] = ContentKey::MessageType::ping;
        }
        break;

        case WebSocketOpcode::PONG:
            self->getContent()[ContentKey::message] = payload;
            self->getContent()[ContentKey::type] = ContentKey::MessageType::pong;
            break;

        case WebSocketOpcode::CONTINUATION:
            break;
        }

        if (self->callback &&
            (ws_opcode == WebSocketOpcode::TEXT || ws_opcode == WebSocketOpcode::BINARY ||
             ws_opcode == WebSocketOpcode::CLOSE))
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
        auto status_it = content.find(HttpRequst::CilentKey::status_code);
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

// WebSocket帧构造 - 修复函数签名
std::string WebSocketClientUtil::makeWebSocketFrame(const std::string& payload,
                                                    WebSocketOpcode opcode, bool fin, bool masked)
{
    return WebSocketRequst::makeWebSocketFrame(payload, opcode, fin, masked);
}

// WebSocket帧解析 - 修复函数签名
std::pair<std::string, WebSocketOpcode> WebSocketClientUtil::parseWebSocketFrame(
    const std::string& frame, bool& fin)
{
    return WebSocketRequst::parseWebSocketFrame(frame, fin);
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