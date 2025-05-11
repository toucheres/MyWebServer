#pragma once
#include "corutine.hpp"
#include "file.h"
#include "httpServerFile.h"
class webSocketFile : public co_async
{
    SocketFile socketfile;
    int webSocketState = true;
    webSocketFile(HttpServerFile& upgradefrom);
    void upgradefrom(HttpServerFile& upgradefrom);
    int eventGo() override;
    Task<void, void> eventloop();
    Task<void, void> corutin = eventloop();
};