#pragma once
#include "reflection.hpp"
#include <string>
#include <string_view>
class json
{
    std::string content = "";

  public:
    friend std::ostream& operator<<(std::ostream& os, const json& j)
    {
        return os << j.content;
    }
    template <Aggregate T> json(T in)
    {
        content += "{";
        visit_each_member(
            in,
            [this]<class M, int index>(auto&& arg)
                requires requires { (std::string) arg; } || requires { std::to_string(arg); }
            {
                if constexpr (requires { (std::string) arg; })
                {
                    add(get_member_names<M>()[index], (std::string)arg);
                }
                else if constexpr (requires { std::to_string(arg); })
                {
                    add(get_member_names<M>()[index], std::to_string(arg));
                }
                content += ',';
            });
        content.pop_back();
        content += "}";
    }
    operator std::string()
    {
        return content;
    }
    void add(std::string_view left, std::string_view right)
    {
        content += std::string_view("\"");
        content += left;
        content += "\":\"";
        content += right;
        content += "\"";
    }
};
