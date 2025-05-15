#pragma once
#include "serverFile.h"

// HTTP服务器工具类 - 类似WebSocketUtil
class HttpServerUtil
{
public:
    // HTTP解析状态枚举
    enum ParseState
    {
        REQUEST_LINE,
        HEADERS,
        BODY,
        COMPLETE
    };
    
    // HTTP事件循环 - 改为静态方法
    static Task<void, void> httpEventloop(serverFile* self);
    
    // 禁止实例化
    HttpServerUtil() = delete;
    ~HttpServerUtil() = delete;
    HttpServerUtil(const HttpServerUtil&) = delete;
    HttpServerUtil& operator=(const HttpServerUtil&) = delete;
};
