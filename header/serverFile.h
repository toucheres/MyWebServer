#pragma once
#include "corutine.hpp"
#include "file.h"
#include <functional>
#include <map>
#include <string>
namespace Agreement
{
    constexpr int HTTP = 0;
    constexpr int WebSocket = 1;
}
class serverFile : public co_async
{
  public:
    int protocolType = Agreement::HTTP; // 添加协议类型作为基类成员变量
    std::map<std::string, std::string> content; // 从HttpServerFile移动到基类
    SocketFile socketfile; // 从HttpServerFile移动到基类
    Task<void, void> corutine; // 当前活动的协程，从HttpServerFile移动到基类
    
  public:
    serverFile() = default;
    serverFile(int fd) : socketfile(fd) {}
    
    virtual int getAgreementType()
    {
        return protocolType;
    } // 简化为直接返回protocolType
    virtual std::map<std::string, std::string>& getContent() = 0; // 非const版本
    virtual const std::map<std::string, std::string>& getContent() const = 0; // 添加const版本
    virtual void write(std::string file) = 0;
    virtual void setCallback(std::function<void(serverFile&)> callback) = 0;
    virtual int getStatus() = 0;
    virtual int reset() = 0;
    virtual bool upgradeProtocol(int newProtocol) = 0;
    
    // 重新实现resetCorutine，不再是纯虚函数
    virtual bool resetCorutine();
    
    // 派生类需要实现的用于协程重置的方法
    virtual Task<void, void> httpEventloop() { return {}; } // 默认实现返回空协程
    
    virtual ~serverFile() = default;
};