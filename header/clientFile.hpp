#pragma once
#include "corutine.hpp"
#include "file.h"
#include "port_fd.h"
#include "protocol_constants.h"
#include <functional>
#include <map>
class HttpClientUtil;
class WebSocketClientUtil;
class clientFile : co_async
{
    Protocol protocolType_ = Protocol::HTTP;
    port targetport = (port)-1;
    std::string targetip = "";
    std::map<std::string, std::string> con;
    Task<void, void> coru;
    using ProtocolHandler = std::function<Task<void, void>(clientFile*)>;
    // 注册协议处理函数
    static std::unordered_map<Protocol, ProtocolHandler>& getProtocolHandlers(); // 更新键类型

  public:
    static bool registerProtocolHandler(Protocol protocolType,
                                        ProtocolHandler handler); // 更新参数类型
    SocketFile cilent_socket;
    std::function<void(clientFile& self)> callback = nullptr;
    clientFile(SocketFile&& sok);
    clientFile(port target, std::string ip);
    clientFile(clientFile&& move);
    clientFile(const clientFile& move);
    Protocol getAgreementType() const;
    std::map<std::string, std::string>& getContent();
    const std::map<std::string, std::string>& getContent() const;
    bool upgradeProtocol(Protocol newProtocol);
    bool resetCorutine();
    port getport() const;
    std::string getip() const;
    void setcallback(std::function<void(clientFile& self)>&& callbackfun);
    EventStatus eventGo();
    void write(std::string file);
    clientFile& operator<<(std::string imf); // write
    bool isKeepAlive() const;
};