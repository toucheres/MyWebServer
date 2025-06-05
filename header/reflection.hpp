#pragma once
#include "reflection_macros.hpp"
#include <array>
#include <iostream>
#include <source_location>
#include <string_view>
#include <tuple>
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

template <typename T>
concept Aggregate = std::is_aggregate_v<T>;
template <typename T>
concept MeaningfulAggregate =
    std::is_aggregate_v<T> &&
    !std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string> &&
    !std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, std::string_view> &&
    !std::is_array_v<T>;

// Primary template
template <typename Type> class num_of_number
{
  public:
    static constexpr int value = 1;
};

// Specialization for MeaningfulAggregate
template <MeaningfulAggregate Type> class num_of_number<Type>
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
template <typename Type> inline constexpr int num_of_number_v = num_of_number<Type>::value;

// 变量模板简化 - removed duplicate declaration with different constraint

template <int N, class... Ts> void visit_members_impl(auto&& bevisited, auto&& visitor)
{
    // 辅助函数，用于处理解构后的成员变量
    auto visit_helper = [&visitor](auto&&... args) { (visitor(args), ...); };
    GENERATE_VISIT_CASES(32) // 支持最多5个成员，可根据需要调整
}

void visit_each_member(auto&& bevisited, auto&& visitor)
    requires requires(decltype(visitor) v, std::decay_t<decltype(bevisited)> t) {
        typename std::decay_t<decltype(bevisited)>;
        { v.template operator()<std::decay_t<decltype(bevisited)>, 0>(std::declval<int>()) };
    }
{
    using T = std::decay_t<decltype(bevisited)>;
    constexpr int N = num_of_number_v<T>;
    // 通用的辅助函数，用于处理不同数量的成员
    auto visit_helper = [&]<typename... Args>(Args&&... args)
    {
        [&]<std::size_t... I>(std::index_sequence<I...>)
        {
            // 使用折叠表达式展开对每个成员的访问
            (visitor.template operator()<T, I>(std::get<I>(std::forward_as_tuple(args...))), ...);
        }(std::make_index_sequence<N>{});
    };

    GENERATE_VISIT_CASES(32) // 支持最多5个成员，可根据需要调整
}
auto visit_members(auto&& bevisited, auto&& visitor)
{
    constexpr int member_count = num_of_number_v<std::decay_t<decltype(bevisited)>>;
    visit_members_impl<member_count>(bevisited, visitor);
}

template <Aggregate T> constexpr auto make_static_memberptr_tuple_form_type()
{
    constexpr size_t Count = num_of_number_v<std::decay_t<T>>;

    GENERATE_TUPLE_CASES(32) // 支持最多5个成员，可根据需要调整

    // 默认情况（理论上不会到达这里）
    // return std::tuple<>{};
}

constexpr long long distance_var(const auto& mem1, const auto& mem2)
{
    return reinterpret_cast<long long>(&mem1) - reinterpret_cast<long long>(&mem2);
}

template <Aggregate T, int index> constexpr bool index_in_range_v = (num_of_number_v<T> > index);

template <Aggregate T, int index>
constexpr long long bias_member()
    requires index_in_range_v<T, index>
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

template <Aggregate T>
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

template <Aggregate T>
inline constexpr std::array<std::string_view, num_of_number_v<T>> get_member_names()
{
    static constexpr auto names_in_fun = get_member_in_fun_names<T>();
    return []<std::size_t... I>(std::index_sequence<I...>) -> auto
    {
        return std::array<std::string_view, num_of_number_v<T>>{
            getname_from_funname(std::get<I>(names_in_fun))...};
    }(std::make_index_sequence<num_of_number_v<T>>{});
}
template <int N> class INT
{
  public:
    static constexpr int value = N;
};

template <Aggregate Type> class num_of_number_with_inner
{
    // 计算成员总数的静态辅助函数
    static constexpr int calculate_count()
    {
        if constexpr (!MeaningfulAggregate<Type>)
        {
            return 1; // 非聚合类型或特殊类型直接计为1个
        }
        else
        {
            int total = num_of_number_v<Type>; // 直接成员数量
            // 使用功能性lambda来递归计算嵌套成员数量
            static constexpr Type instance{};
            visit_members(instance,
                          [&total](auto member)
                          {
                              using MemberType = std::decay_t<decltype(member)>;
                              if constexpr (MeaningfulAggregate<MemberType>)
                              {
                                  // 递归计算成员的成员数
                                  total += num_of_number_with_inner<MemberType>::value - 1;
                              }
                          });

            return total;
        }
    }

  public:
    // 预计算并存储结果
    inline static int value = calculate_count();
};

template <typename T> inline constexpr int num_of_number_with_inner_v = 1; // Primary template

template <Aggregate T>
inline static int num_of_number_with_inner_v<T> = num_of_number_with_inner<T>::value;

template <class T> std::string_view getClassName()
{
    static constexpr std::string_view names_before{std::source_location::current().function_name()};
    std::cout << names_before;
    return names_before;
}
template <class T> constexpr auto class_name_in_fun()
{
    return std::source_location::current().function_name();
}

// 从函数名中提取类名
inline std::string_view extract_class_name(std::string_view func_name)
{
    // 支持GCC格式: [with T = users]
    std::size_t with_pos = func_name.find("with T = ");
    if (with_pos != std::string_view::npos) {
        std::size_t start = with_pos + 9; // "with T = "的长度
        std::size_t end = func_name.find(']', start);
        if (end == std::string_view::npos) {
            end = func_name.find(';', start);
        }
        if (end == std::string_view::npos) {
            end = func_name.size();
        }
        return func_name.substr(start, end - start);
    }
    
    // 支持Clang格式: [T = users]
    std::size_t t_eq_pos = func_name.find("T = ");
    if (t_eq_pos != std::string_view::npos) {
        std::size_t start = t_eq_pos + 4; // "T = "的长度
        std::size_t end = func_name.find(']', start);
        if (end == std::string_view::npos) {
            end = func_name.find(';', start);
        }
        if (end == std::string_view::npos) {
            end = func_name.size();
        }
        return func_name.substr(start, end - start);
    }
    
    return func_name; // 如果没有找到匹配模式，返回原始字符串
}

template <class T> constexpr auto get_class_name()
{
    static constexpr auto names_before = class_name_in_fun<T>();
    return extract_class_name(names_before);
    // constexpr auto class_name_in_fun() [with T = users] //gcc
    // auto class_name_in_fun() [T = users] //clang
}
// void fun() [with T = users] //gcc
// void fun() [T = users] clang

#define constexpr_try(x)                                                                           \
    if constexpr (requires { x })                                                                  \
    {                                                                                              \
        x                                                                                          \
    }
#define constexpr_catch else