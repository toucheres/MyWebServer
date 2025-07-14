#pragma once
#include "corutine.hpp"
#include "file.h"
#include "port_fd.h"
#include <functional>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

// WebSocket操作码枚举
enum class WebSocketOpcode : uint8_t
{
    CONTINUATION = 0x0,
    TEXT = 0x1,
    BINARY = 0x2,
    CLOSE = 0x8,
    PING = 0x9,
    PONG = 0xA
};

// WebSocket关闭状态码
enum class WebSocketCloseCode : uint16_t
{
    NORMAL_CLOSURE = 1000,
    GOING_AWAY = 1001,
    PROTOCOL_ERROR = 1002,
    UNSUPPORTED_DATA = 1003,
    NO_STATUS_RCVD = 1005,
    ABNORMAL_CLOSURE = 1006,
    INVALID_FRAME_PAYLOAD_DATA = 1007,
    POLICY_VIOLATION = 1008,
    MESSAGE_TOO_BIG = 1009,
    MANDATORY_EXT = 1010,
    INTERNAL_SERVER_ERROR = 1011
};

// WebSocket客户端状态
enum class WebSocketClientState
{
    CONNECTING,     // 正在连接
    HANDSHAKING,    // 正在握手
    CONNECTED,      // 已连接
    CLOSING,        // 正在关闭
    CLOSED          // 已关闭
};

// WebSocket帧结构
struct WebSocketFrame
{
    bool fin = true;
    WebSocketOpcode opcode = WebSocketOpcode::TEXT;
    bool masked = true;
    uint64_t payload_length = 0;
    uint32_t mask_key = 0;
    std::vector<uint8_t> payload;

    std::vector<uint8_t> serialize() const;
    static WebSocketFrame deserialize(const std::vector<uint8_t>& data, size_t& offset);
};

class WebSocketClientUtil;

class WebSocketClient : co_async
{
    friend WebSocketClientUtil;

private:
    port target_port = (port)-1;
    std::string target_ip = "";
    std::string target_path = "/";
    std::map<std::string, std::string> headers_;
    Task<void, void> coru;
    WebSocketClientState state_ = WebSocketClientState::CONNECTING;
    std::string sec_websocket_key_;
    std::string expected_accept_;
    
    // 回调函数
    std::function<void(WebSocketClient& self)> on_open_ = nullptr;
    std::function<void(WebSocketClient& self, const std::string& message)> on_message_ = nullptr;
    std::function<void(WebSocketClient& self, const std::vector<uint8_t>& data)> on_binary_ = nullptr;
    std::function<void(WebSocketClient& self, WebSocketCloseCode code, const std::string& reason)> on_close_ = nullptr;
    std::function<void(WebSocketClient& self, const std::string& error)> on_error_ = nullptr;
    std::function<void(WebSocketClient& self)> on_ping_ = nullptr;
    std::function<void(WebSocketClient& self)> on_pong_ = nullptr;

    // 消息缓存
    std::vector<uint8_t> frame_buffer_;
    std::vector<uint8_t> message_buffer_;
    WebSocketOpcode current_message_opcode_ = WebSocketOpcode::TEXT;

public:
    SocketFile client_socket;

    // 构造函数
    WebSocketClient(const std::string& ip, port port, const std::string& path = "/");
    WebSocketClient(SocketFile&& socket, const std::string& path = "/");
    WebSocketClient(WebSocketClient&& other);
    WebSocketClient(const WebSocketClient&) = delete;
    WebSocketClient& operator=(const WebSocketClient&) = delete;

    // 连接管理
    bool connect();
    void close(WebSocketCloseCode code = WebSocketCloseCode::NORMAL_CLOSURE, const std::string& reason = "");
    
    // 状态查询
    WebSocketClientState getState() const { return state_; }
    bool isConnected() const { return state_ == WebSocketClientState::CONNECTED; }
    
    // 消息发送
    bool sendText(const std::string& message);
    bool sendBinary(const std::vector<uint8_t>& data);
    bool sendPing(const std::vector<uint8_t>& data = {});
    bool sendPong(const std::vector<uint8_t>& data = {});
    
    // 回调设置
    void setOnOpen(std::function<void(WebSocketClient& self)> callback) { on_open_ = std::move(callback); }
    void setOnMessage(std::function<void(WebSocketClient& self, const std::string& message)> callback) { on_message_ = std::move(callback); }
    void setOnBinary(std::function<void(WebSocketClient& self, const std::vector<uint8_t>& data)> callback) { on_binary_ = std::move(callback); }
    void setOnClose(std::function<void(WebSocketClient& self, WebSocketCloseCode code, const std::string& reason)> callback) { on_close_ = std::move(callback); }
    void setOnError(std::function<void(WebSocketClient& self, const std::string& error)> callback) { on_error_ = std::move(callback); }
    void setOnPing(std::function<void(WebSocketClient& self)> callback) { on_ping_ = std::move(callback); }
    void setOnPong(std::function<void(WebSocketClient& self)> callback) { on_pong_ = std::move(callback); }

    // 协程接口
    virtual EventStatus eventGo() override;

    // 获取连接信息
    port getPort() const { return target_port; }
    const std::string& getIp() const { return target_ip; }
    const std::string& getPath() const { return target_path; }
    
    // 设置自定义头部
    void addHeader(const std::string& key, const std::string& value) { headers_[key] = value; }

private:
    // 生成WebSocket key
    std::string generateWebSocketKey();
    
    // 计算期望的Accept值
    std::string calculateExpectedAccept(const std::string& key);
    
    // 发送WebSocket握手请求
    bool sendHandshakeRequest();
    
    // 发送帧
    bool sendFrame(const WebSocketFrame& frame);
};

class WebSocketClientUtil
{
public:
    static Task<void, void> webSocketEventloop(WebSocketClient* self);
    
    // WebSocket相关工具函数
    static std::string generateWebSocketKey();
    static std::string calculateWebSocketAccept(const std::string& key);
    static uint32_t generateMaskKey();
    static void maskData(std::vector<uint8_t>& data, uint32_t mask);
    
    // 禁止实例化
    WebSocketClientUtil() = delete;
    ~WebSocketClientUtil() = delete;
    WebSocketClientUtil(const WebSocketClientUtil&) = delete;
    WebSocketClientUtil& operator=(const WebSocketClientUtil&) = delete;
};
