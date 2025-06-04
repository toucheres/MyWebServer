#include <array>
#include <iostream>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>

class AnyType
{
  public:
    template <class A> operator A()
    {
        return std::declval<A>();
    };
};

template <class T1, class T2, class... Ts> class is_same_all
{
  public:
    constexpr static bool value = std::is_same_v<T1, T2> && is_same_all<T2, Ts...>::value;
};
template <class T1, class T2> class is_same_all<T1, T2>
{
  public:
    constexpr static bool value = std::is_same_v<T1, T2>;
};

template <class T1, class T2, class... Ts>
inline constexpr bool is_same_all_v = is_same_all<T1, T2, Ts...>::value;

template <class Type> class num_of_number
{
    template <class T, class... Args>
    static consteval int num_of_number_fun_()
        requires requires { is_same_all_v<AnyType, Args...> || sizeof...(Args) == 0; }
    {
        if constexpr (requires { T{std::declval<Args>()...}; })
        {
            return num_of_number_fun_<T, AnyType, Args...>();
        }
        else
        {
            return sizeof...(Args) - 1;
        }
    }
    template <class T_> static consteval int num_of_number_fun_start()
    {
        return num_of_number_fun_<T_, AnyType>();
    };

  public:
    static constexpr int value = num_of_number_fun_start<std::decay_t<Type>>();
};

// 变量模板简化
template <class Type> inline constexpr int num_of_number_v = num_of_number<Type>::value;

template <int N, class... Ts> void visit_members_impl(auto&& bevisited, auto&& visitor)
{
    if constexpr (N == 0)
    {
        // 无成员
    }
    else if constexpr (N == 1)
    {
        auto&& [a1] = bevisited;
        visitor(a1);
    }
    else if constexpr (N == 2)
    {
        auto&& [a1, a2] = bevisited;
        visitor(a1, a2);
    }
    else if constexpr (N == 3)
    {
        auto&& [a1, a2, a3] = bevisited;
        visitor(a1, a2, a3);
    }
    // ...
}

void visit_each_member(auto&& bevisited, auto&& visitor)
{
    using T = std::decay_t<decltype(bevisited)>;
    constexpr int N = num_of_number_v<T>;
    if constexpr (N == 0)
    {
    }
    else if constexpr (N == 1)
    {
        auto&& [a1] = bevisited;
        visitor.template operator()<T, 0>(a1);
    }
    else if constexpr (N == 2)
    {
        auto&& [a1, a2] = bevisited;
        visitor.template operator()<T, 0>(a1);
        visitor.template operator()<T, 1>(a2);
    }
    else if constexpr (N == 3)
    {
        auto&& [a1, a2, a3] = bevisited;
        visitor.template operator()<T, 0>(a1);
        visitor.template operator()<T, 1>(a2);
        visitor.template operator()<T, 2>(a3);
    }
    else if constexpr (N == 4)
    {
        auto&& [a1, a2, a3, a4] = bevisited;
        visitor.template operator()<T, 0>(a1);
        visitor.template operator()<T, 1>(a2);
        visitor.template operator()<T, 2>(a3);
        visitor.template operator()<T, 3>(a4);
    }
    // ...
}
auto visit_members(auto&& bevisited, auto&& visitor)
{
    constexpr int member_count = num_of_number_v<std::decay_t<decltype(bevisited)>>;
    visit_members_impl<member_count>(bevisited, visitor);
}

template <class T> constexpr auto make_static_tuple_form_type()
{
    constexpr size_t Count = num_of_number_v<std::decay_t<T>>;
    if constexpr (Count == 0)
    {
        return std::make_tuple();
    }
    else if constexpr (Count == 1)
    {
        static constexpr T obj{};
        auto&& [a1] = obj;
        return std::make_tuple(&a1); // 返回成员指针
    }
    else if constexpr (Count == 2)
    {
        static constexpr T obj{};
        auto&& [a1, a2] = obj;
        return std::make_tuple(&a1, &a2);
    }
    // ...
}
template <class T, int index> constexpr long long bias_member()
{
    constexpr size_t Count = num_of_number_v<std::decay_t<T>>;
    if constexpr (Count == 0)
    {
    }
    else if constexpr (Count == 1)
    {
        static constexpr T obj{};
        auto&& [a1] = obj;
        if constexpr (index == 0)
        {
            return 0;
        }
    }
    else if constexpr (Count == 2)
    {
        static constexpr T obj{};
        auto&& [a1, a2] = obj;
        if constexpr (index == 0)
        {
            return 0;
        }
        if constexpr (index == 1)
        {
            return reinterpret_cast<long long>(&a2) - reinterpret_cast<long long>(&obj);
        }
    }
    else if constexpr (Count == 3)
    {
        static constexpr T obj{};
        auto&& [a1, a2, a3] = obj;
        if constexpr (index == 0)
        {
            return 0;
        }
        if constexpr (index == 1)
        {
            return reinterpret_cast<long long>(&a2) - reinterpret_cast<long long>(&obj);
        }
        if constexpr (index == 2)
        {
            return reinterpret_cast<long long>(&a3) - reinterpret_cast<long long>(&obj);
        }
    }
    else if constexpr (Count == 4)
    {
        static constexpr T obj{};
        auto&& [a1, a2, a3, a4] = obj;
        if constexpr (index == 0)
        {
            return 0;
        }
        if constexpr (index == 1)
        {
            return reinterpret_cast<long long>(&a2) - reinterpret_cast<long long>(&obj);
        }
        if constexpr (index == 2)
        {
            return reinterpret_cast<long long>(&a3) - reinterpret_cast<long long>(&obj);
        }
        if constexpr (index == 3)
        {
            return reinterpret_cast<long long>(&a4) - reinterpret_cast<long long>(&obj);
        }
    }
    // ...
}
template <auto ptr> inline constexpr std::string_view funname()
{
    return std::source_location::current().function_name();
}

template <class T>
inline constexpr std::array<std::string_view, num_of_number_v<T>> get_member_in_fun_names()
{
    static constexpr auto pt = make_static_tuple_form_type<T>();
    return []<std::size_t... I>(std::index_sequence<I...>) -> auto
    {
        return std::array<std::string_view, num_of_number_v<T>>{funname<std::get<I>(pt)>()...};
    }(std::make_index_sequence<num_of_number_v<T>>{});
}

inline constexpr std::string_view getname_from_funname(std::string_view in)
{

    // gcc
    // constexpr std::string_view funname() [with auto ptr = (& obj.test::a); std::string_view = std::basic_string_view<char>]
    // clang
    // std::string_view funname() [ptr = &obj.a]
    // 查找最后一个点号，成员名在点号之后
    std::size_t dot_pos = in.rfind('.');
    if (dot_pos == std::string_view::npos)
    {
        return in; // 没有找到点号，返回原字符串
    }
    // 从点号后开始查找成员名
    std::size_t start = dot_pos + 1;
    // 查找成员名的结束位置（遇到 ')' 或 ';' 或 ']'）
    std::size_t end = in.find_first_of(");]", start);
    if (end == std::string_view::npos)
    {
        end = in.length();
    }
    return in.substr(start, end - start);
}

template <class T>
inline constexpr std::array<std::string_view, num_of_number_v<T>> get_member_names()
{
    static constexpr auto names_in_fun = get_member_in_fun_names<T>();
    return []<std::size_t... I>(std::index_sequence<I...>) -> auto
    {
        return std::array<std::string_view, num_of_number_v<T>>{
            getname_from_funname(std::get<I>(names_in_fun))...};
    }(std::make_index_sequence<num_of_number_v<T>>{});
}