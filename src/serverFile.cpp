#include "serverFile.h"
#include "httpServerFile.h"
#include "webSocketFile.h"

bool serverFile::resetCorutine() {
    // 根据协议类型初始化相应的协程
    switch (protocolType) {
        case Agreement::HTTP:
            corutine = HttpServerUtil::httpEventloop(this);
            break;
        case Agreement::WebSocket:
            corutine = WebSocketUtil::wsEventloop(this);
            break;
        default:
            // 未知协议类型
            return false;
    }
    return true;
}
