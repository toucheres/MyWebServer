#include <string>
#include <system_error>
class Format
{
  public:
    enum class Type
    {
        prefix,
        suffix,
        same,
        regex,
        waitingCompare,
    };
    Format(std::string format, Type type = Type::waitingCompare);
    Format() = default;
    Format(const Format& format) = default;
    Format(Format&& format) = default;
    // for std::*map<>
    bool operator==(const Format& in);
    bool operator<(const Format& in);

  private:
    std::string format = "";
    Format::Type type;
};
