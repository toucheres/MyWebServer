#pragma once
#include "corutine.hpp"
#include <functional>
#include <map>
#include <string>
class serverFile : public co_async
{
  public:
    virtual const std::map<std::string, std::string>& getContent() = 0;
    virtual void write(std::string file) = 0;
    virtual void setCallback(std::function<void(serverFile&)> callback) = 0;
    virtual int getStatus() = 0;
    virtual ~serverFile() = default;
};