#pragma once
#include <corutine.hpp>
#include <cstddef>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
struct LocalFiles;
struct SocketFiles;
struct LocalFile
{
    friend LocalFiles;

  private:
    std::vector<char> content;
    std::string path = "";
    size_t size = 0;
    std::string_view test;

  public:
    bool load(std::string& path);
    std::string_view read();
    LocalFile(std::string a_path);
    LocalFile(LocalFile&& move);
};
struct SocketFile : public co_async
{
    friend SocketFiles;

  private:
    class CONTEXT
    {
        friend SocketFile;
        int fd=-1;
        std::vector<char> content={};
        mutable size_t left = 0;
        mutable size_t right = 0;
    };
    static Task<> eventfun(CONTEXT& context);
    Task_Local<CONTEXT> handle = eventfun;

  public:
    virtual int eventGo() const final;
    const std::string_view read_added() const;
    const std::string_view read_all() const;
    bool load(int a_fd);
    SocketFile(int a_fd);
    ~SocketFile();
    SocketFile()=default;
    SocketFile(SocketFile&& move);
    SocketFile& operator=(SocketFile && move);
    // SocketFile(const SocketFile& copy);
};
struct LocalFiles
{
  private:
    std::unordered_map<std::string, LocalFile> fileMap;

  public:
    bool add(std::string& path);
    LocalFile& get(std::string& path);
};
struct SocketFiles
{
  private:
    std::unordered_map<std::string, SocketFile> fileMap;

  public:
    bool add(int fd);
    SocketFile& get(int fd);
    const std::unordered_map<std::string, SocketFile>& getMap();
};
