#pragma once
#include <map>
#include <string>
class serverFile
{
  public:
    virtual const std::map<std::string, std::string>& getContent() = 0;
    virtual void write(std::string file) = 0;
};