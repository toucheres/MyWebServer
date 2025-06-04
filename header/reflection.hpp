#include <array>
#include <iostream>
#include <source_location>
#include <string_view>
#include <type_traits>
#include "reflection_macros.hpp"
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
    // 辅助函数，用于处理解构后的成员变量
    auto visit_helper = [&visitor](auto&&... args) {
        (visitor(args), ...);
    };
    GENERATE_VISIT_CASES(5) // 支持最多5个成员，可根据需要调整
}

void visit_each_member(auto&& bevisited, auto&& visitor)
{
    using T = std::decay_t<decltype(bevisited)>;
    constexpr int N = num_of_number_v<T>;
    // 通用的辅助函数，用于处理不同数量的成员
    auto visit_helper = [&]<typename... Args>(Args&&... args) {
        [&]<std::size_t... I>(std::index_sequence<I...>)
        {
            // 使用折叠表达式展开对每个成员的访问
            (visitor.template operator()<T, I>(std::get<I>(std::forward_as_tuple(args...))), ...);
        }(std::make_index_sequence<N>{});
    };

    GENERATE_VISIT_CASES(5) // 支持最多5个成员，可根据需要调整
}
auto visit_members(auto&& bevisited, auto&& visitor)
{
    constexpr int member_count = num_of_number_v<std::decay_t<decltype(bevisited)>>;
    visit_members_impl<member_count>(bevisited, visitor);
}

template <class T> constexpr auto make_static_memberptr_tuple_form_type()
{
    constexpr size_t Count = num_of_number_v<std::decay_t<T>>;
    
    GENERATE_TUPLE_CASES(5) // 支持最多5个成员，可根据需要调整
    
    // 默认情况（理论上不会到达这里）
    return std::tuple<>{};
}

constexpr long long distance_var(const auto& mem1, const auto& mem2)
{
    return reinterpret_cast<long long>(&mem1) - reinterpret_cast<long long>(&mem2);
}

template <class T, int index>
concept index_in_range = (num_of_number_v<T>) > (index);

template <class T, int index>
constexpr long long bias_member()
    requires index_in_range<T, index>
{
    constexpr size_t Count = num_of_number_v<std::decay_t<T>>;

    if constexpr (Count == 0 || index >= Count)
    {
        return 0;
    }
    else
    {
        // 使用已有的 make_static_memberptr_tuple_form_type 函数
        static constexpr T obj{};
        constexpr auto member_ptrs = make_static_memberptr_tuple_form_type<T>();
        //[TODO] 强制类型转化破坏constexpr性
        // 直接使用 std::get 获取对应成员的指针，然后计算偏移
        return reinterpret_cast<long long>(
            reinterpret_cast<long long>(std::get<index>(member_ptrs)) -
            reinterpret_cast<long long>(std::get<0>(member_ptrs)));
    }
}
template <auto ptr> inline constexpr std::string_view funname()
{
    return std::source_location::current().function_name();
}

template <class T>
inline constexpr std::array<std::string_view, num_of_number_v<T>> get_member_in_fun_names()
{
    static constexpr auto pt = make_static_memberptr_tuple_form_type<T>();
    return []<std::size_t... I>(std::index_sequence<I...>) -> auto
    {
        return std::array<std::string_view, num_of_number_v<T>>{funname<std::get<I>(pt)>()...};
    }(std::make_index_sequence<num_of_number_v<T>>{});
}

inline constexpr std::string_view getname_from_funname(std::string_view in)
{

    // gcc
    // constexpr std::string_view funname() [with auto ptr = (& obj.test::a); std::string_view =
    // std::basic_string_view<char>] clang std::string_view funname() [ptr = &obj.a]
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