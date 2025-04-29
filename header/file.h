#include <corutine.hpp>
#include <cstddef>
#include <future>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

struct File
{
  private:
    std::vector<char> content;
    std::string path = "";
    size_t size = 0;

  public:
    std::string_view read_sync();
    Task<void, std::string_view> read_async();
    File(std::string a_path);
    File(const File& copy);
    File(File&& move){}
};
struct LocalFileCache
{
  private:
    std::unordered_map<std::string, File> fileMap;

  public:
    bool load(std::string);
    File& get();
};
