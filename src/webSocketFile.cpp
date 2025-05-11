#include "httpServerFile.h"
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
std::string createWebSocketFrame(bool fin, uint8_t opcode, const std::string& payload,
                                 bool masked = false)
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
std::string parseWebSocketFrame(const std::string& frame)
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
}
const std::map<std::string, std::string>& WebSocketFile::getContent()
{
    static std::map<std::string, std::string> nothing;
    return nothing;
}
WebSocketFile::WebSocketFile(HttpServerFile& origin)
    : socketfile(std::move(origin.socketfile)), webSocketState(true)
{
}

void WebSocketFile::upgradefrom(HttpServerFile& http)
{
    // 检查必要的WebSocket握手头部
    auto upgrade_it = http.content.find("upgrade");
    auto connection_it = http.content.find("connection");
    auto key_it = http.content.find("sec-websocket-key");

    if (upgrade_it == http.content.end() || connection_it == http.content.end() ||
        key_it == http.content.end() || upgrade_it->second != "websocket")
    {
        webSocketState = false;
        return;
    }

    // 这里应该计算正确的Sec-WebSocket-Accept值
    // 在实际应用中需要使用: base64(sha1(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"))
    // 简化示例中使用固定值
    std::string accept_key = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";

    // 发送WebSocket握手响应
    std::string response = "HTTP/1.1 101 Switching Protocols\r\n"
                           "Upgrade: websocket\r\n"
                           "Connection: Upgrade\r\n"
                           "Sec-WebSocket-Accept: " +
                           accept_key + "\r\n\r\n";

    socketfile.writeFile(response);
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