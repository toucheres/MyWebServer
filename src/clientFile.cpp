#include "clientFile.hpp"
#include "protocol_constants.h"
using ProtocolHandler = std::function<Task<void, void>(clientFile*)>;
std::unordered_map<Protocol, ProtocolHandler>& clientFile::getProtocolHandlers()
{
    static std::unordered_map<Protocol, ProtocolHandler> p;
    return p;
}
bool clientFile::registerProtocolHandler(Protocol protocolType, ProtocolHandler handler)
{
    getProtocolHandlers()[protocolType] = std::move(handler);
    return true;
}
bool clientFile::resetCorutine()
{
    // 根据协议类型查找相应的协程处理函数
    auto& handlers = getProtocolHandlers();
    auto it = handlers.find(protocolType_);
    if (it != handlers.end())
    {
        // 找到了对应的处理函数，调用它
        this->coru = it->second(this);
        return true;
    }
    // 未找到对应协议的处理函数
    return false;
}
Protocol clientFile::getAgreementType() const
{
    return this->protocolType_;
}
clientFile& clientFile::operator<<(std::string arg)
{
    this->write(arg);
    return *this;
}
void clientFile::write(std::string in)
{
    this->cilent_socket.writeFile(in);
}
std::string clientFile::getip() const
{
    return this->targetip;
}
port clientFile::getport() const
{
    return this->targetport;
}
bool clientFile::upgradeProtocol(Protocol newProtocol)
{
    if (newProtocol == protocolType_)
        return true;
    protocolType_ = newProtocol;
    return resetCorutine();
}