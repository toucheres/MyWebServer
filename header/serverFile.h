#pragma once
#include "corutine.hpp"
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
  protected:
    int protocolType = Agreement::HTTP; // 添加协议类型作为基类成员变量
    std::map<std::string, std::string> content; // 从HttpServerFile移动到基类
    
  public:
    virtual int getAgreementType()
    {
        return protocolType;
    } // 简化为直接返回protocolType
    virtual std::map<std::string, std::string>& getContent() = 0; // 非const版本
    virtual const std::map<std::string, std::string>& getContent() const = 0; // 添加const版本
    virtual void write(std::string file) = 0;
    virtual void write_str_with_agreement(std::string file) = 0;
    virtual void setCallback(std::function<void(serverFile&)> callback) = 0;
    virtual int getStatus() = 0;
    virtual int reset() = 0;
    virtual bool upgradeProtocol(int newProtocol) = 0;
    virtual bool resetCorutine() = 0;
    virtual ~serverFile() = default;
};