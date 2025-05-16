#include "serverFile.h"
#include "httpServerFile.h"
#include "webSocketFile.h"

// 使用Meyer's Singleton模式实现懒加载
std::unordered_map<int, serverFile::ProtocolHandler>& 
serverFile::getProtocolHandlers() {
    static std::unordered_map<int, ProtocolHandler> protocolHandlers;
    return protocolHandlers;
}

// 注册协议处理函数的实现
bool serverFile::registerProtocolHandler(int protocolType, ProtocolHandler handler) {
    // 注册或更新处理函数
    getProtocolHandlers()[protocolType] = std::move(handler);
    return true;
}

bool serverFile::resetCorutine() {
    // 根据协议类型查找相应的协程处理函数
    auto& handlers = getProtocolHandlers();
    auto it = handlers.find(protocolType);
    if (it != handlers.end()) {
        // 找到了对应的处理函数，调用它
        corutine = it->second(this);
        return true;
    }
    
    // 未找到对应协议的处理函数
    return false;
}
