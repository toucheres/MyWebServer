#include "serverFile.h"
#include "httpServerFile.h"
#include "webSocketFile.h"

// 初始化静态成员
std::unordered_map<int, serverFile::ProtocolHandler> serverFile::protocolHandlers;

// 注册协议处理函数的实现
bool serverFile::registerProtocolHandler(int protocolType, ProtocolHandler handler) {
    // 注册或更新处理函数
    protocolHandlers[protocolType] = std::move(handler);
    return true;
}

bool serverFile::resetCorutine() {
    // 根据协议类型查找相应的协程处理函数
    auto it = protocolHandlers.find(protocolType);
    if (it != protocolHandlers.end()) {
        // 找到了对应的处理函数，调用它
        corutine = it->second(this);
        return true;
    }
    
    // 未找到对应协议的处理函数
    return false;
}
