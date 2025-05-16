#pragma once
#include "corutine.hpp"
#include "serverFile.h"
#include <string>

// Declare Protocol namespace and WebSocket constant
namespace Protocol {
    constexpr int WebSocket = 1;
}

// WebSocket工具类 - 只包含静态方法和常量
class WebSocketUtil
{
  private:
    // 添加自动注册WebSocket协议处理函数的静态变量
    static bool autoRegistered;

  public:
    // WebSocket操作码枚举
    enum class WebSocketOpcode // Changed to enum class
    {
        CONTINUATION = 0x0,
        TEXT = 0x1,
        BINARY = 0x2,
        CLOSE = 0x8,
        PING = 0x9,
        PONG = 0xA
    };

    // WebSocket帧操作的静态方法
    static std::string makeWebSocketFrame(
        const std::string& payload,
        WebSocketOpcode opcode = WebSocketUtil::WebSocketOpcode::TEXT, // Use enum class
        bool fin = true, bool masked = false);
    static std::string parseWebSocketFrame(const std::string& frame);

    // WebSocket握手响应方法
    static std::string makeWebSocketHandshake(const std::string& clientKey);
    static std::string makeWebSocketHandshake(const serverFile& clientKey);

    // WebSocket事件循环 - 从HttpServerFile移动过来的静态方法
    static Task<void, void> wsEventloop(serverFile* self);

    // 从HttpServerFile移动过来的方法 - 判断是否应该升级到WebSocket
    static bool shouldbeUpdataToWS(const serverFile& httpfile);

    // 初始化方法，注册WebSocket协议处理函数
    static bool initialize();

    // 禁止实例化
    WebSocketUtil() = delete;
    ~WebSocketUtil() = delete;
    WebSocketUtil(const WebSocketUtil&) = delete;
    WebSocketUtil& operator=(const WebSocketUtil&) = delete;
};

// WebSocketResponse类 - 简化WebSocket响应创建
class WebSocketResponse
{
  private:
    std::string content_;
    WebSocketUtil::WebSocketOpcode opcode_;
    bool fin_;
    bool masked_;

  public:
    // 构造函数
    WebSocketResponse(WebSocketUtil::WebSocketOpcode opcode = WebSocketUtil::WebSocketOpcode::TEXT,
                      bool fin = true, bool masked = false); // Use enum class

    // 设置内容
    WebSocketResponse& with_content(const std::string& content);

    // 静态工厂方法
    static WebSocketResponse text(const std::string& content);
    static WebSocketResponse binary(const std::string& content);
    static WebSocketResponse ping(const std::string& content = "");
    static WebSocketResponse pong(const std::string& content = "");
    static WebSocketResponse close(uint16_t code = 1000, const std::string& reason = "");

    // 转换为字符串（WebSocket帧）
    operator std::string() const;
};