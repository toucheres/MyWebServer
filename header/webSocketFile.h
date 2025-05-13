#pragma once
#include "corutine.hpp"
#include "file.h"
#include "httpServerFile.h"
#include "serverFile.h"
class WebSocketFile : public serverFile
{

    std::map<std::string, std::string> content;
    int webSocketState = true;
    int eventGo() override;
    virtual int getStatus() final override;
    Task<void, void> eventloop();
    Task<void, void> corutin = eventloop();
    std::function<void(serverFile&)> callback;

  public:
    SocketFile socketfile;
    ~WebSocketFile() = default;
    WebSocketFile(SocketFile&& socketfile);
    WebSocketFile(HttpServerFile&& upgradefrom);
    void upgradefrom(HttpServerFile&& upgradefrom);
    static bool shouldbeUpdataToWS(const serverFile&);
    virtual void write(std::string file) final;
    virtual int reset() final override;
    virtual const std::map<std::string, std::string>& getContent() const final override;
    virtual void setCallback(std::function<void(serverFile&)> callback) final override;
};