#include "httpServerFile.h"
#include "serverFile.h"
#include <iostream>
#include <string>
#include <webSocketFile.h>

// WebSocket帧操作码
enum WebSocketOpcode
{
    CONTINUATION = 0x0,
    TEXT = 0x1,
    BINARY = 0x2,
    CLOSE = 0x8,
    PING = 0x9,
    PONG = 0xA
};

// 简单的WebSocket帧创建函数
std::string WebSocketFile::createWebSocketFrame(bool fin, uint8_t opcode, const std::string& payload,
                                 bool masked)
{
    std::string frame;

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
std::string WebSocketFile::parseWebSocketFrame(const std::string& frame)
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

void WebSocketFile::write(std::string content)
{
    // 使用类的静态方法创建WebSocket帧
    std::string frame = createWebSocketFrame(true, TEXT, content);
    socketfile.writeFile(frame);
}

void WebSocketFile::setCallback(std::function<void(serverFile&)> a_callback)
{
    this->callback = std::move(a_callback);
}
const std::map<std::string, std::string>& WebSocketFile::getContent() const
{
    return this->content;
}
WebSocketFile::WebSocketFile(SocketFile&& a_socketfile) : socketfile(std::move(a_socketfile))
{
    protocolType = Agreement::WebSocket; // 设置基类的协议类型
}
WebSocketFile::WebSocketFile(HttpServerFile&& origin)
    : socketfile(std::move(origin.socketfile)), webSocketState(true)
{
    protocolType = Agreement::WebSocket; // 设置基类的协议类型
    content["path"] = origin.content.at("path");
    this->setCallback(std::move(origin.callback));
}

void WebSocketFile::upgradefrom(HttpServerFile&& origin)
{
    socketfile = std::move(origin.socketfile);
    webSocketState = true;
    content["path"] = origin.content.at("path");
}

bool WebSocketFile::shouldbeUpdataToWS(const serverFile& httpfile)
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
int WebSocketFile::eventGo()
{
    corutin.resume();
    return webSocketState;
}

Task<void, void> WebSocketFile::eventloop()
{
    while (webSocketState)
    {
        int ret = socketfile.eventGo();
        if (ret == -1)
        {
            webSocketState = false;
            co_yield {};
        }

        if ((!socketfile.handle.context) ||
            (socketfile.handle.context->fd_state == SocketFile::WRONG))
        {
            webSocketState = false;
            co_yield {};
        }

        // 读取并处理WebSocket数据
        std::string_view data = socketfile.read_added();
        if (!data.empty())
        {
            std::cout << data << '\n';
            std::string frame_data(data);
            std::string message = parseWebSocketFrame(frame_data);

            if (!message.empty())
            {
                // 检查是否是控制帧
                uint8_t opcode = frame_data[0] & 0x0F;

                if (opcode == CLOSE)
                {
                    // 关闭帧，回应关闭并终止连接
                    std::string close_frame = createWebSocketFrame(true, CLOSE, "");
                    socketfile.writeFile(close_frame);
                    webSocketState = false;
                }
                else if (opcode == PING)
                {
                    // Ping帧，回应Pong
                    std::string pong_frame = createWebSocketFrame(true, PONG, message);
                    socketfile.writeFile(pong_frame);
                }
                else if (opcode == TEXT || opcode == BINARY)
                {
                    // 文本或二进制帧，简单回显
                    std::string response = "Echo: " + message;
                    std::string response_frame = createWebSocketFrame(true, TEXT, response);
                    socketfile.writeFile(response_frame);
                }
            }
        }

        co_yield {};
    }

    co_return;
}
int WebSocketFile::getStatus()
{
    return 0;
}
int WebSocketFile::reset()
{
    return 0;
}
// int WebSocketFile::getAgreementType()
// {
//     return Agreement::WebSocket; // 确保返回WebSocket类型
// }