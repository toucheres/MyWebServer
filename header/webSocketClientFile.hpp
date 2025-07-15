#pragma once
#include "clientFile.hpp"
#include "httpClientFile.h"
#include "corutine.hpp"
#include "file.h"
#include <string>
#include <functional>
#include <map>
#include <vector>
#include "webSocketServerFile.h"

// WebSocket客户端工具类 - 提供WebSocket协议的客户端实现
class WebSocketClientUtil
{
private:
    // 添加自动注册WebSocket协议处理函数的静态变量
    static bool autoRegistered;

public:
    // WebSocket事件循环处理
    static Task<void, void> wsEventloop(clientFile* self);
    
    // 初始化方法，注册WebSocket协议处理函数
    static bool initialize();
    
    // 从HTTP客户端升级到WebSocket
    static bool upgradeToWebSocket(clientFile& client, const std::string& host, 
                                   const std::string& path = "/", 
                                   const std::map<std::string, std::string>& headers = {});
    
    // WebSocket帧操作的静态方法
    static std::string makeWebSocketFrame(
        const std::string& payload,
        WebSocketServerUtil::WebSocketOpcode opcode = WebSocketServerUtil::WebSocketOpcode::TEXT,
        bool fin = true, bool masked = true);
        
    static std::pair<std::string, WebSocketServerUtil::WebSocketOpcode> parseWebSocketFrame(
        const std::string& frame, bool& fin);
    
    // 客户端特有的操作
    static uint32_t generateMaskKey();
    static void maskData(std::vector<uint8_t>& data, uint32_t mask);
    
    // 新增函数 - 生成WebSocket握手请求
    static std::string makeWebSocketHandshake(const std::string& path = "/", 
                                             const std::string& host = "localhost:80",
                                             const std::map<std::string, std::string>& headers = {});
    
    // 新增函数 - 检查是否应该升级到WebSocket
    static bool shouldbeUpdataToWS(const clientFile& client);
    
    // 内容键值
    class ContentKey
    {
    public:
        static constexpr const char* message = "message";
        static constexpr const char* type = "type";
        static constexpr const char* is_binary = "is_binary";
        static constexpr const char* is_closed = "is_closed";
        static constexpr const char* close_code = "close_code";
        static constexpr const char* close_reason = "close_reason";
        static constexpr const char* websocket_key = "websocket_key";
        static constexpr const char* websocket_accept = "websocket_accept";
        
        class MessageType
        {
        public:
            static constexpr const char* text = "text";
            static constexpr const char* binary = "binary";
            static constexpr const char* close = "close";
            static constexpr const char* ping = "ping";
            static constexpr const char* pong = "pong";
        };
    };
    
    // 禁止实例化
    WebSocketClientUtil() = delete;
    ~WebSocketClientUtil() = delete;
    WebSocketClientUtil(const WebSocketClientUtil&) = delete;
    WebSocketClientUtil& operator=(const WebSocketClientUtil&) = delete;
};

// WebSocketMessage类 - 简化WebSocket消息的创建
class WebSocketMessage
{
private:
    std::string content_;
    WebSocketServerUtil::WebSocketOpcode opcode_;
    bool fin_;
    bool masked_;
    uint32_t mask_key_;

public:
    // 构造函数
    WebSocketMessage(WebSocketServerUtil::WebSocketOpcode opcode = WebSocketServerUtil::WebSocketOpcode::TEXT,
                    bool fin = true, bool masked = true);
                    
    // 设置内容
    WebSocketMessage& withContent(const std::string& content);
    
    // 设置掩码密钥
    WebSocketMessage& withMaskKey(uint32_t key);
    
    // 静态工厂方法
    static WebSocketMessage text(const std::string& content);
    static WebSocketMessage binary(const std::string& content);
    static WebSocketMessage ping(const std::string& content = "");
    static WebSocketMessage pong(const std::string& content = "");
    static WebSocketMessage close(uint16_t code = 1000, const std::string& reason = "");
    
    // 转换为WebSocket帧
    operator std::string() const;
    std::string toString() const;
    
    // 获取内容和类型
    const std::string& getContent() const { return content_; }
    WebSocketServerUtil::WebSocketOpcode getOpcode() const { return opcode_; }
    bool isFin() const { return fin_; }
    bool isMasked() const { return masked_; }
};

// WebSocket客户端类 - 提供简易的WebSocket客户端API
// class WebSocketClient
// {
// private:
//     clientFile client_;
//     std::string host_;
//     int port_;
//     bool connected_ = false;
//     std::function<void(const WebSocketMessage&)> message_callback_ = nullptr;
//     std::function<void()> close_callback_ = nullptr;
    
// public:
//     // 构造函数
//     WebSocketClient(const std::string& host, int port);
    
//     // 连接到WebSocket服务器
//     bool connect(const std::string& path = "/", 
//                  const std::map<std::string, std::string>& headers = {});
    
//     // 发送消息
//     bool sendMessage(const WebSocketMessage& message);
//     bool sendText(const std::string& text);
//     bool sendBinary(const std::string& binary_data);
//     bool sendPing(const std::string& data = "");
//     bool sendPong(const std::string& data = "");
    
//     // 关闭连接
//     bool close(uint16_t code = 1000, const std::string& reason = "");
    
//     // 设置回调
//     void setMessageCallback(std::function<void(const WebSocketMessage&)> callback);
//     void setCloseCallback(std::function<void()> callback);
    
//     // 状态检查
//     bool isConnected() const { return connected_; }
    
//     // 处理事件
//     void handleEvents();
// };