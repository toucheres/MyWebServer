#include "serverFile.h"
#include "httpServerFile.h"
#include "webSocketFile.h"

// serverFile constructor
serverFile::serverFile(int fd) : socketfile_(fd), protocolType_(Protocol::HTTP), fileState_(true) {}

// 使用Meyer's Singleton模式实现懒加载
std::unordered_map<Protocol, serverFile::ProtocolHandler>& 
serverFile::getProtocolHandlers() {
    static std::unordered_map<Protocol, ProtocolHandler> protocolHandlers;
    return protocolHandlers;
}

// 注册协议处理函数的实现
bool serverFile::registerProtocolHandler(Protocol protocolType, ProtocolHandler handler) {
    // 注册或更新处理函数
    getProtocolHandlers()[protocolType] = std::move(handler);
    return true;
}

bool serverFile::resetCorutine() {
    // 根据协议类型查找相应的协程处理函数
    auto& handlers = getProtocolHandlers();
    auto it = handlers.find(protocolType_);
    if (it != handlers.end()) {
        // 找到了对应的处理函数，调用它
        corutine_ = it->second(this);
        return true;
    }
    
    // 未找到对应协议的处理函数
    return false;
}

Protocol serverFile::getAgreementType() const { return protocolType_; }

std::map<std::string, std::string>& serverFile::getContent() { return content_; }

const std::map<std::string, std::string>& serverFile::getContent() const { return content_; }

void serverFile::write(std::string file) { socketfile_.writeFile(file); }

void serverFile::setCallback(std::function<void(serverFile&)> a_callback) { callback_ = a_callback; }

bool serverFile::getStatus() const { return fileState_; }

int serverFile::handle() {
    if (callback_) {
        callback_(*this);
    }
    return 0;
}

void serverFile::closeIt() { socketfile_.closeIt(); }

bool serverFile::upgradeProtocol(Protocol newProtocol) {
    if (newProtocol == protocolType_) return true;
    protocolType_ = newProtocol;
    return resetCorutine();
}

EventStatus serverFile::eventGo() {
    if (!fileState_ || !corutine_) return EventStatus::Stop; // Check if coroutine is valid
    corutine_.resume();
    return fileState_ ? EventStatus::Continue : EventStatus::Stop;
}

Task<void, void> serverFile::httpEventloop() { 
    // 默认实现，如果HTTP协议没有被正确初始化或覆盖，则不执行任何操作。
    // 实际的HTTP事件循环应由HttpServerUtil::httpEventloop提供并通过registerProtocolHandler注册。
    return {}; 
}
