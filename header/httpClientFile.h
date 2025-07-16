#pragma once
#include "clientFile.hpp"
#include "corutine.hpp"
#include "file.h"
#include "port_fd.h"
#include <string>

class HttpClientUtil
{
  public:
    static bool initialize();
    static bool autoRegistered;
    static Task<void, void> httpEventloop(clientFile* self);
    SocketFile createTcpClient(port port, std::string ip);
};