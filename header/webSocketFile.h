#pragma once
#include <string>
#include <array>

// WebSocket工具类 - 只包含静态方法和常量
class WebSocketUtil
{
public:
    // WebSocket操作码枚举
    enum WebSocketOpcode
    {
        CONTINUATION = 0x0,
        TEXT = 0x1,
        BINARY = 0x2,
        CLOSE = 0x8,
        PING = 0x9,
        PONG = 0xA
    };

    // WebSocket帧操作的静态方法
    static std::string createWebSocketFrame(bool fin, uint8_t opcode, const std::string& payload,
                                         bool masked = false);
    static std::string parseWebSocketFrame(const std::string& frame);
    
    // WebSocket握手响应方法
    static std::string createWebSocketHandshake(const std::string& clientKey);
    
    // 禁止实例化
    WebSocketUtil() = delete;
    ~WebSocketUtil() = delete;
    WebSocketUtil(const WebSocketUtil&) = delete;
    WebSocketUtil& operator=(const WebSocketUtil&) = delete;
};