#include "serverFile.h"
#include "webSocketFile.h"
#include "httpServerFile.h"

bool serverFile::resetCorutine()
{
    // 根据当前协议类型选择合适的协程
    if (protocolType == Agreement::HTTP)
    {
        corutine = HttpServerUtil::httpEventloop(this);
    }
    else if (protocolType == Agreement::WebSocket)
    {
        corutine = WebSocketUtil::wsEventloop(this);
    }
    else
    {
        return false;
    }
    return true;
}
