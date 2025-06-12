#pragma once
#include "reflection.hpp"
#include "string"

template <bool b, class T> struct Null
{
};

struct Pri
{
};
struct UNI
{
};
template <class T>
concept is_key = requires { std::is_same_v<T, Pri>; };
template <is_key b, class T> struct Key
{
};

struct CURRENT_TIMESTAMP
{
};
template <class T>
concept is_default = requires { std::is_same_v<T, CURRENT_TIMESTAMP>; };
template <is_default b, class T> struct Default
{
};

struct auto_increment
{
};
struct DEFAULT_GENERATED
{
};
template <class T>
concept is_extra = requires { std::is_same_v<T, auto_increment>; } ||
                   requires { std::is_same_v<T, DEFAULT_GENERATED>; };
template <is_extra b, class T> struct Extra
{
};

struct only_content
{
    std::string content;
};

template <size_t size> struct varchar : only_content
{
};
struct text : only_content
{
    std::string content;
};
struct bigint : only_content
{
    std::string content;
};
struct timestamp : only_content
{
    std::string content;
};
class enable_sql
{
};
