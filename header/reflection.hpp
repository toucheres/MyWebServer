#include <iostream>
#include <source_location>
#include <string>
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

// 变量模板简化
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

struct person
{
    int age;
    std::string name;
    bool a;
    bool end;
};
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
void visit_members_each_with_index(auto&& bevisited, auto&& visitor)
{
    using T = std::decay_t<decltype(bevisited)>;
    constexpr int N = num_of_number_v<T>;
    if constexpr (N == 0)
    {
    }
    else if constexpr (N == 1)
    {
        auto&& [a1] = bevisited;
        visitor.template operator()<T, std::decay_t<decltype(a1)>, 0>(a1);
    }
    else if constexpr (N == 2)
    {
        auto&& [a1, a2] = bevisited;
        visitor.template operator()<T, std::decay_t<decltype(a1)>, 0>(a1);
        visitor.template operator()<T, std::decay_t<decltype(a2)>, 1>(a2);
    }
    else if constexpr (N == 3)
    {
        auto&& [a1, a2, a3] = bevisited;
        visitor.template operator()<T, std::decay_t<decltype(a1)>, 0>(a1);
        visitor.template operator()<T, std::decay_t<decltype(a2)>, 1>(a2);
        visitor.template operator()<T, std::decay_t<decltype(a3)>, 2>(a3);
    }
    else if constexpr (N == 4)
    {
        auto&& [a1, a2, a3, a4] = bevisited;
        visitor.template operator()<T, std::decay_t<decltype(a1)>, 0>(a1);
        visitor.template operator()<T, std::decay_t<decltype(a2)>, 1>(a2);
        visitor.template operator()<T, std::decay_t<decltype(a3)>, 2>(a3);
        visitor.template operator()<T, std::decay_t<decltype(a4)>, 3>(a4);
    }
}
auto visit_members(auto&& bevisited, auto&& visitor)
{
    constexpr int member_count = num_of_number_v<std::decay_t<decltype(bevisited)>>;
    visit_members_impl<member_count>(bevisited, visitor);
}

// 在 member_ptr.hpp 中（简化版本）
template <class T> constexpr auto make_static_tuple_form()
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
template <class T, int index> constexpr long long struct_bias()
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
            return reinterpret_cast<long long>(&a2) - reinterpret_cast<long long>(&a1);
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
            return reinterpret_cast<long long>(&a2) - reinterpret_cast<long long>(&a1);
        }
        if constexpr (index == 2)
        {
            return reinterpret_cast<long long>(&a3) - reinterpret_cast<long long>(&a1);
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
            return reinterpret_cast<long long>(&a2) - reinterpret_cast<long long>(&a1);
        }
        if constexpr (index == 2)
        {
            return reinterpret_cast<long long>(&a3) - reinterpret_cast<long long>(&a1);
        }
        if constexpr (index == 3)
        {
            return reinterpret_cast<long long>(&a4) - reinterpret_cast<long long>(&a1);
        }
    }
}
// template <class T> class getname
// {
//     static constexpr T t{};

//     template <class B, class mem, mem B::* memptr> static consteval const char*
//     get_name_by_static_var_ptr_template_fun_()
//     {
//         auto imf = std::source_location::current();
//         return imf.function_name();
//     }

//     template <std::size_t I> static consteval const char* get_member_name()
//     {
//         if constexpr (I == 0 && num_of_number_v<T> >= 1)
//         {
//             return "";
//         }
//     }

//   public:
//     inline constexpr static const char* value[num_of_number_v<T>] = {get_member_name<0>(),
//                                                                      get_member_name<1>()};
// };