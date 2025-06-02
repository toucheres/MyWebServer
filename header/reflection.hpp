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
}

auto visit_members(auto&& bevisited, auto&& visitor)
{
    constexpr int member_count = num_of_number_v<std::decay_t<decltype(bevisited)>>;
    visit_members_impl<member_count>(bevisited, visitor);
}
template<int* p>
class getname
{
  public:
    void test()
    {
        auto imf = std::source_location::current();
        std::cout << imf.column() << '\n';
        std::cout << imf.file_name() << '\n';
        std::cout << imf.function_name() << '\n';
        std::cout << imf.line() << '\n';
    }
};