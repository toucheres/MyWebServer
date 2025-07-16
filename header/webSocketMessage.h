#pragma once
#include <cstdint>
#include <string>
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
class WebSocketResponse
{
  public:
    class ContentKey
    {
      public:
        static constexpr std::string message = "message";
        static constexpr std::string type = "type";
        class CallbackTypes
        {
          public:
            static constexpr std::string text = "text";
            static constexpr std::string binary = "binary";
            static constexpr std::string close = "close";
        };
    };


  private:
    std::string content_;
    WebSocketOpcode opcode_;
    bool fin_;
    bool masked_;

  public:
    // 构造函数
    WebSocketResponse(WebSocketOpcode opcode = WebSocketOpcode::TEXT, bool fin = true,
                      bool masked = false); // Use enum class

    // 设置内容
    WebSocketResponse& with_content(const std::string& content);

    // 静态工厂方法
    // WebSocket帧操作的静态方法
    static std::string makeWebSocketFrame(
        const std::string& payload,
        WebSocketOpcode opcode =
            WebSocketOpcode::TEXT, // Use enum class
        bool fin = true, bool masked = false);
    static WebSocketResponse text(const std::string& content);
    static WebSocketResponse binary(const std::string& content);
    static WebSocketResponse ping(const std::string& content = "");
    static WebSocketResponse pong(const std::string& content = "");
    static WebSocketResponse close(uint16_t code = 1000, const std::string& reason = "");

    // 转换为字符串（WebSocket帧）
    operator std::string() const;
};

// WebSocketMessage类 - 简化WebSocket消息的创建
class WebSocketRequst
{
  private:
    std::string content_;
    WebSocketOpcode opcode_;
    bool fin_;
    bool masked_;
    uint32_t mask_key_;

  public:
    // 构造函数
    WebSocketRequst(
        WebSocketOpcode opcode = WebSocketOpcode::TEXT,
        bool fin = true, bool masked = true);

    // 设置内容
    WebSocketRequst& withContent(const std::string& content);
    static uint32_t generateMaskKey();

    // 设置掩码密钥
    WebSocketRequst& withMaskKey(uint32_t key);
    // WebSocket帧操作的静态方法
    static std::string makeWebSocketFrame(
        const std::string& payload,
        WebSocketOpcode opcode = WebSocketOpcode::TEXT,
        bool fin = true, bool masked = true);

    static std::pair<std::string, WebSocketOpcode> parseWebSocketFrame(
        const std::string& frame, bool& fin);
    // 静态工厂方法
    static WebSocketRequst text(const std::string& content);
    static WebSocketRequst binary(const std::string& content);
    static WebSocketRequst ping(const std::string& content = "");
    static WebSocketRequst pong(const std::string& content = "");
    static WebSocketRequst close(uint16_t code = 1000, const std::string& reason = "");

    // 转换为WebSocket帧
    operator std::string() const;
    std::string toString() const;

    // 获取内容和类型
    const std::string& getContent() const
    {
        return content_;
    }
    WebSocketOpcode getOpcode() const
    {
        return opcode_;
    }
    bool isFin() const
    {
        return fin_;
    }
    bool isMasked() const
    {
        return masked_;
    }
};
