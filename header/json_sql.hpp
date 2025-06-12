#pragma once
#include "mysqlHandle.h"
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

// 解析
template <class member> struct sql_member_imf
{
    // Key<Pri, int> id
    // Key<UNI, varchar<50>> username
    // varchar<100> password
    // Null<true, timestamp> created_at
    using Type = decltype([]() {
        // ...
    }());
    std::string_view Field()
    {
        //
    }
    // ...
    constexpr std::string_view Type_name();
    constexpr std::string_view Null();
    constexpr std::string_view Key();
    constexpr std::string_view Default();
    constexpr std::string_view Extra();
};
template <class T> class data_views
{
    // ...
    auto change_all(T to);
    auto change_if(std::function<bool(const T*)> callback, T to);
    auto delete_all(T to);
    auto delete_if(std::function<bool(const T*)> callback);
    auto get_all();
    auto get_if(std::function<bool(const T*)> callback);
};

template <class T> struct struct_MySql
{
    MySQLHandle& connection;
    auto auto_create_table();
    auto delete_this_table();
    data_views<T> find();
};
