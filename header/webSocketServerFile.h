#pragma once
#include "corutine.hpp"
#include "serverFile.h"
#include "webSocketMessage.h"
#include <string>

// WebSocket工具类 - 只包含静态方法和常量
class WebSocketServerUtil
{
  private:
    // 添加自动注册WebSocket协议处理函数的静态变量
    static bool autoRegistered;
    inline static bool callcbackWhenClose = false;

  public:
    // WebSocket帧操作的静态方法 - 修复函数声明
    static std::string makeWebSocketFrame(
        const std::string& payload,
        WebSocketOpcode opcode = WebSocketOpcode::TEXT,
        bool fin = true, bool masked = false);

    static std::string parseWebSocketFrame(const std::string& frame);

    // WebSocket握手响应方法
    static std::string makeWebSocketHandshake(const std::string& clientKey);
    static std::string makeWebSocketHandshake(const serverFile& clientKey);
    static bool tryUpToWs(serverFile& file);
    // WebSocket事件循环 - 从HttpServerFile移动过来的静态方法
    static Task<void, void> wsEventloop(serverFile* self);

    // 从HttpServerFile移动过来的方法 - 判断是否应该升级到WebSocket
    static bool shouldbeUpdataToWS(const serverFile& httpfile);

    // 初始化方法，注册WebSocket协议处理函数
    static bool initialize();

    // 禁止实例化
    WebSocketServerUtil() = delete;
    ~WebSocketServerUtil() = delete;
    WebSocketServerUtil(const WebSocketServerUtil&) = delete;
    WebSocketServerUtil& operator=(const WebSocketServerUtil&) = delete;
};
