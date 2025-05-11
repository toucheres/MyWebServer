#pragma once
#include "corutine.hpp"
#include "file.h"
#include "httpServerFile.h"
class WebSocketFile : public co_async, public serverFile
{
    SocketFile socketfile;
    int webSocketState = true;
    WebSocketFile(HttpServerFile& upgradefrom);
    void upgradefrom(HttpServerFile& upgradefrom);
    int eventGo() override;
    Task<void, void> eventloop();
    Task<void, void> corutin = eventloop();
};