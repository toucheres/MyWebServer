#pragma once
#include "corutine.hpp"
#include "file.h"
#include "httpServerFile.h"
class HttpServerFile;
class WebSocketFile : public serverFile
{
    SocketFile socketfile;
    std::map<std::string, std::string> content;
    int webSocketState = true;
    int eventGo() override;
    virtual int getStatus() final override;
    Task<void, void> eventloop();
    Task<void, void> corutin = eventloop();

  public:
    ~WebSocketFile() = default;
    WebSocketFile(HttpServerFile&& upgradefrom);
    void upgradefrom(HttpServerFile&& upgradefrom);
    virtual void write(std::string file) final;
    virtual const std::map<std::string, std::string>& getContent() final;
};