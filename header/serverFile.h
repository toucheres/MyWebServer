#pragma once
#include "corutine.hpp"
#include "file.h"
#include <functional>
#include <map>
#include <string>
#include <unordered_map>

namespace Agreement
{
    constexpr int HTTP = 0;
    constexpr int WebSocket = 1;
}

class serverFile : public co_async
{
  public:
    int protocolType = Agreement::HTTP; 
    std::map<std::string, std::string> content;
    SocketFile socketfile;
    Task<void, void> corutine;
    std::function<void(serverFile&)> callback;
    int fileState = true;
    
    // 定义协议处理函数类型
    using ProtocolHandler = std::function<Task<void, void>(serverFile*)>;
    
    // 存储协议类型到处理函数的静态映射
    static std::unordered_map<int, ProtocolHandler> protocolHandlers;
    
    // 注册协议处理函数
    static bool registerProtocolHandler(int protocolType, ProtocolHandler handler);
    
  public:
    serverFile() = default;
    serverFile(int fd) : socketfile(fd) {}
    
    // 实现所有虚函数，不再是抽象类
    int getAgreementType() { return protocolType; }
    std::map<std::string, std::string>& getContent() { return content; }
    const std::map<std::string, std::string>& getContent() const { return content; }
    void write(std::string file) { socketfile.writeFile(file); }
    void setCallback(std::function<void(serverFile&)> a_callback) { callback = a_callback; }
    int getStatus() { return fileState; }
    int handle() {
        if (callback) {
            callback(*this);
        }
        return 0;
    }
    void closeIt() { socketfile.closeIt(); }
    bool upgradeProtocol(int newProtocol) {
        if (newProtocol == protocolType) return true;
        protocolType = newProtocol;
        return resetCorutine();
    }
    
    // 协程管理
    bool resetCorutine();
    virtual int eventGo() override {
        corutine.resume();
        return fileState;
    }
    
    // 派生类需要实现的用于协程重置的方法
    virtual Task<void, void> httpEventloop() { return {}; } // 默认实现返回空协程
    
    virtual ~serverFile() = default;
};