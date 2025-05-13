#pragma once
#include "corutine.hpp"
#include "file.h"
#include "httpServerFile.h"
#include "serverFile.h"
class WebSocketFile : public serverFile
{

    std::map<std::string, std::string> content;
    int webSocketState = true;
    int eventGo() override;
    virtual int getStatus() final override;
    Task<void, void> eventloop();
    Task<void, void> corutin = eventloop();
    std::function<void(serverFile&)> callback;

  public:
    // WebSocket帧操作的静态方法
    static std::string createWebSocketFrame(bool fin, uint8_t opcode, const std::string& payload,
                                         bool masked = false);
    static std::string parseWebSocketFrame(const std::string& frame);
    
    // 新增：创建WebSocket握手响应的方法
    static std::string createWebSocketHandshake(const std::string& clientKey);
    
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

    SocketFile socketfile;
    ~WebSocketFile() = default;
    WebSocketFile(SocketFile&& socketfile);
    WebSocketFile(HttpServerFile&& upgradefrom);
    void upgradefrom(HttpServerFile&& upgradefrom);
    static bool shouldbeUpdataToWS(const serverFile&);
    virtual void write(std::string file) final;
    virtual int reset() final override;
    virtual const std::map<std::string, std::string>& getContent() const final override;
    virtual void setCallback(std::function<void(serverFile&)> callback) final override;
    virtual bool upgradeProtocol(int newProtocol) override { return false; } // WebSocket不支持再次升级
    virtual bool resetCorutine() override { return false; } // WebSocket不支持重置协程
};