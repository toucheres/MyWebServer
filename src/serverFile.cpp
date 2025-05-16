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

int serverFile::getAgreementType() { return protocolType; }

std::map<std::string, std::string>& serverFile::getContent() { return content; }

const std::map<std::string, std::string>& serverFile::getContent() const { return content; }

void serverFile::write(std::string file) { socketfile.writeFile(file); }

void serverFile::setCallback(std::function<void(serverFile&)> a_callback) { callback = a_callback; }

int serverFile::getStatus() { return fileState; }

int serverFile::handle() {
    if (callback) {
        callback(*this);
    }
    return 0;
}

void serverFile::closeIt() { socketfile.closeIt(); }

bool serverFile::upgradeProtocol(int newProtocol) {
    if (newProtocol == protocolType) return true;
    protocolType = newProtocol;
    return resetCorutine();
}

int serverFile::eventGo() {
    corutine.resume();
    return fileState;
}

Task<void, void> serverFile::httpEventloop() { 
    // 默认实现，如果HTTP协议没有被正确初始化或覆盖，则不执行任何操作。
    // 实际的HTTP事件循环应由HttpServerUtil::httpEventloop提供并通过registerProtocolHandler注册。
    return {}; 
}
