#pragma once
#include "reflection_macros.hpp"
#include <array>
#include <functional>
#include <source_location>
#include <string_view>
#include <tuple>
#include <type_traits>

// 函数特性提取工具 - 移到文件开头解决依赖顺序问题
namespace function_traits_detail
{
    // 获取函数的参数数量和类型
    template <typename T> struct function_traits;

    // 普通函数
    template <typename R, typename... Args> struct function_traits<R(Args...)>
    {
        static constexpr std::size_t arity = sizeof...(Args);
        using result_type = R;
        using args_tuple = std::tuple<Args...>;
    };

    // 函数指针
    template <typename R, typename... Args>
    struct function_traits<R (*)(Args...)> : function_traits<R(Args...)>
    {
    };

    // 成员函数指针
    template <typename C, typename R, typename... Args>
    struct function_traits<R (C::*)(Args...)> : function_traits<R(Args...)>
    {
    };

    // const 成员函数指针
    template <typename C, typename R, typename... Args>
    struct function_traits<R (C::*)(Args...) const> : function_traits<R(Args...)>
    {
    };

    // std::function
    template <typename R, typename... Args>
    struct function_traits<std::function<R(Args...)>> : function_traits<R(Args...)>
    {
    };

    // lambda和可调用对象
    template <typename F> struct function_traits
    {
      private:
        using call_type = function_traits<decltype(&F::operator())>;

      public:
        static constexpr std::size_t arity = call_type::arity;
        using result_type = typename call_type::result_type;
        using args_tuple = typename call_type::args_tuple;
    };

    // 移除引用和cv限定符
    template <typename F> struct function_traits<F&> : function_traits<F>
    {
    };

    template <typename F> struct function_traits<F&&> : function_traits<F>
    {
    };

    // 辅助函数：前N个参数调用
    template <typename F, typename Tuple, std::size_t... I>
    auto call_with_first_n_args_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
    {
        return f(std::get<I>(std::forward<Tuple>(t))...);
    }

    template <typename F, typename Tuple> auto call_with_first_n_args(F&& f, Tuple&& t)
    {
        constexpr std::size_t f_arity = function_traits<std::decay_t<F>>::arity;
        constexpr std::size_t tuple_size = std::tuple_size_v<std::decay_t<Tuple>>;
        constexpr std::size_t valid_args = (f_arity < tuple_size) ? f_arity : tuple_size;

        return call_with_first_n_args_impl(std::forward<F>(f), std::forward<Tuple>(t),
                                           std::make_index_sequence<valid_args>{});
    }
} // namespace function_traits_detail

// 提供简便的别名以便外部使用
template <typename T> using function_traits = function_traits_detail::function_traits<T>;

template <typename F, typename Tuple> auto call_with_first_n_args(F&& f, Tuple&& t)
{
    return function_traits_detail::call_with_first_n_args(std::forward<F>(f),
                                                          std::forward<Tuple>(t));
}

// 函数反射工具 - 扩展对可调用对象的支持
namespace callable_reflection
{
    // 获取可调用对象的参数数量
    template <typename F> constexpr std::size_t get_args_nums(F&&)
    {
        return function_traits<std::decay_t<F>>::arity;
    }

    // 获取可调用对象的参数类型
    template <std::size_t N, typename F>
    using arg_type_t =
        std::tuple_element_t<N, typename function_traits<std::decay_t<F>>::args_tuple>;

    // 获取可调用对象的返回类型
    template <typename F>
    using return_type_t = typename function_traits<std::decay_t<F>>::result_type;

    // 获取所有参数类型作为元组
    template <typename F>
    using args_tuple_t = typename function_traits<std::decay_t<F>>::args_tuple;

    // 是否为成员函数
    template <typename F> struct is_member_function
    {
        static constexpr bool value = false;
    };

    template <typename C, typename R, typename... Args> struct is_member_function<R (C::*)(Args...)>
    {
        static constexpr bool value = true;
        using class_type = C;
    };

    template <typename C, typename R, typename... Args>
    struct is_member_function<R (C::*)(Args...) const>
    {
        static constexpr bool value = true;
        using class_type = C;
    };

    // 统一的参数数量获取实现（类型版本）
    template <typename F> struct args_count
    {
        static constexpr std::size_t value = function_traits<F>::arity;
    };
} // namespace callable_reflection

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

template <int N, class... Ts> constexpr void visit_members_impl(auto&& bevisited, auto&& visitor)
{
    // 辅助函数，用于处理解构后的成员变量
    auto visit_helper = [&visitor](auto&&... args) { (visitor(args), ...); };
    GENERATE_VISIT_CASES(32) // 支持最多5个成员，可根据需要调整
}

constexpr void visit_each_member(auto&& bevisited, auto&& visitor)
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
constexpr auto visit_members(auto&& bevisited, auto&& visitor)
{
    constexpr int member_count = num_of_number_v<std::decay_t<decltype(bevisited)>>;
    visit_members_impl<member_count>(bevisited, visitor);
}

template <Aggregate T> consteval auto make_fake_constexpr_memberptr_tuple_form_type()
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
        // 使用已有的 make_fake_constexpr_memberptr_tuple_form_type 函数
        // static constexpr T obj{};
        constexpr auto member_ptrs = make_fake_constexpr_memberptr_tuple_form_type<T>();
        //[TODO] 强制类型转化破坏constexpr性
        // return reinterpret_cast<long long>(
        //     reinterpret_cast<long long>(std::get<index>(member_ptrs)) -
        //     reinterpret_cast<long long>(std::get<0>(member_ptrs)));
        return std::bit_cast<char*>(std::get<index>(member_ptrs)) -
               std::bit_cast<char*>(std::get<0>(member_ptrs));
    }
}
template <auto ptr> inline consteval std::string_view funname()
{
    return std::source_location::current().function_name();
}

template <Aggregate T>
inline consteval std::array<std::string_view, num_of_number_v<T>> get_member_in_fun_names()
{
    static constexpr auto pt = make_fake_constexpr_memberptr_tuple_form_type<T>();
    return []<std::size_t... I>(std::index_sequence<I...>) -> auto
    {
        return std::array<std::string_view, num_of_number_v<T>>{funname<std::get<I>(pt)>()...};
    }(std::make_index_sequence<num_of_number_v<T>>{});
}

inline consteval std::string_view getname_from_funname(std::string_view in)
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
inline consteval std::array<std::string_view, num_of_number_v<T>> get_member_names()
{
    static constexpr auto names_in_fun = get_member_in_fun_names<T>();
    return []<std::size_t... I>(std::index_sequence<I...>) -> auto
    {
        return std::array<std::string_view, num_of_number_v<T>>{
            getname_from_funname(std::get<I>(names_in_fun))...};
    }(std::make_index_sequence<num_of_number_v<T>>{});
}
template <Aggregate T, int index>
    requires index_in_range_v<T, index>
inline consteval std::string_view get_member_name()
{
    return get_member_names<T>()[index];
}

template <int N> class INT
{
  public:
    static constexpr int value = N;
};
template <class Type> consteval int num_of_number_with_inner()
{
    if constexpr (!Aggregate<Type>)
    {
        return 1; // 非聚合类型或特殊类型直接计为1个
    }
    else if constexpr (MeaningfulAggregate<Type>)
    {

        auto instance = make_fake_constexpr_memberptr_tuple_form_type<Type>();
        // 使用功能性lambda来递归计算嵌套成员数量
        return []<std::size_t... I>(std::index_sequence<I...>) -> auto
        {
            return (0 + ... +
                    num_of_number_with_inner<
                        std::remove_pointer_t<std::decay_t<decltype(std::get<I>(instance))>>>());
        }(std::make_index_sequence<num_of_number_v<Type>>{});
    }
    return 1;
}

template <typename T> inline constexpr int num_of_number_with_inner_v = 1; // Primary template

template <Aggregate T>
inline constexpr int num_of_number_with_inner_v<T> = num_of_number_with_inner<T>();

template <class T> consteval auto class_name_in_fun()
{
    return std::source_location::current().function_name();
}

// 从函数名中提取类名
inline consteval std::string_view extract_class_name(std::string_view func_name)
{
    // 支持GCC格式: [with T = users]
    std::size_t with_pos = func_name.find("with T = ");
    if (with_pos != std::string_view::npos)
    {
        std::size_t start = with_pos + 9; // "with T = "的长度
        std::size_t end = func_name.rfind(']', start);
        if (end == std::string_view::npos)
        {
            end = func_name.find(';', start);
        }
        if (end == std::string_view::npos)
        {
            end = func_name.size();
        }
        return func_name.substr(start, end - start);
    }

    // 支持Clang格式: [T = users]
    std::size_t t_eq_pos = func_name.find("T = ");
    if (t_eq_pos != std::string_view::npos)
    {
        std::size_t start = t_eq_pos + 4; // "T = "的长度
        std::size_t end = func_name.rfind(']', start);
        if (end == std::string_view::npos)
        {
            end = func_name.find(';', start);
        }
        if (end == std::string_view::npos)
        {
            end = func_name.size() - 1;
        }
        return func_name.substr(start, end - start);
    }

    return func_name; // 如果没有找到匹配模式，返回原始字符串
}

template <class T> consteval auto get_class_name()
{
    return extract_class_name(class_name_in_fun<T>());
    // constexpr auto class_name_in_fun() [with T = users] //gcc
    // auto class_name_in_fun() [T = users] //clang
}
// void fun() [with T = users] //gcc
// void fun() [T = users] clang

consteval std::string_view get_class_name_from_ptr(std::string_view type_str)
{
    // clang
    // const int *
    //  const std::basic_string<char> *
    //  const std::basic_string<char> *
    //  const std::basic_string<char> *

    // gcc
    // const int*
    // const std::__cxx11::basic_string<char>*
    // const std::__cxx11::basic_string<char>*
    // const std::__cxx11::basic_string<char>*
    // 去除前缀"const "（如果存在）
    if (type_str.starts_with("const "))
    {
        type_str.remove_prefix(6); // "const "长度为6
    }

    // 去除尾部指针符号
    std::size_t ptr_pos = type_str.find_last_of('*');
    if (ptr_pos != std::string_view::npos)
    {
        type_str = type_str.substr(0, ptr_pos);
    }

    // 去除尾部空格
    while (!type_str.empty() && type_str.back() == ' ')
    {
        type_str.remove_suffix(1);
    }

    // // 处理命名空间
    // // 对于GCC格式: std::__cxx11::basic_string<char>
    // if (std::size_t cxx11_pos = type_str.find("__cxx11::"); cxx11_pos !=
    // std::string_view::npos)
    // {
    //     std::size_t start = type_str.substr(0, cxx11_pos).find_last_of(':');
    //     if (start != std::string_view::npos)
    //     {
    //         type_str = type_str.substr(0, start + 1) +
    //                    type_str.substr(cxx11_pos + 9); // "__cxx11::"长度为9
    //     }
    // }

    return type_str;
    return {};
}

consteval std::string_view translate_name(std::string_view in)
{
    // 编译期静态映射表
    static const constexpr std::pair<std::string_view, std::string_view> map[]{
        // 标准类型映射
        std::pair{get_class_name<std::string>(), "string"},
        std::pair{get_class_name<int>(), "int"}};
    // 编译期线性查找
    for (const auto& [from, to] : map)
    {
        if (in == from)
        {
            return to;
        }
    }
    // 如果不匹配，返回原始输入
    return in;
}

template <class T> consteval auto get_member_class_names()
{
    static constexpr auto pt = make_fake_constexpr_memberptr_tuple_form_type<T>();
    return []<std::size_t... I>(std::index_sequence<I...>) -> auto
    {
        return std::array<std::string_view, num_of_number_v<T>>{translate_name(
            get_class_name_from_ptr(get_class_name<std::decay_t<decltype(std::get<I>(pt))>>()))...};
    }(std::make_index_sequence<num_of_number_v<T>>{});
    // gcc
    // int
    // std::__cxx11::basic_string<char>
    // std::__cxx11::basic_string<char>
    // std::__cxx11::basic_string<char>

    // clang
    // int
    // std::basic_string<char>
    // std::basic_string<char>
    // std::basic_string<char>
}

template <class T> consteval auto get_member_classname_name_pairs()
{
    static constexpr auto pt = make_fake_constexpr_memberptr_tuple_form_type<T>();
    return []<std::size_t... I>(std::index_sequence<I...>) -> auto
    {
        return std::array<std::pair<std::string_view, std::string_view>, num_of_number_v<T>>{
            std::pair<std::string_view, std::string_view>{
                translate_name(get_class_name_from_ptr(
                    get_class_name<std::decay_t<decltype(std::get<I>(pt))>>())),
                get_member_name<T, I>()}...};
    }(std::make_index_sequence<num_of_number_v<T>>{});
}
template <auto fun> class get_fun_name
{
    static consteval std::string_view extra_funname_from_classname()
    {
        // 查找函数名的开始位置 - 通常在最后一个 :: 之后或整个字符串开头
        constexpr std::string_view in = get_class_name<get_fun_name<fun>>();
        constexpr std::size_t left = in.find("&");
        constexpr std::size_t right = in.find(">");
        if constexpr ((left != std::string_view::npos) && (right != std::string_view::npos))
        {
            constexpr std::size_t start = left + 1; // 跳过&
            constexpr std::size_t end = right;      // 跳过>
            return in.substr(start, end - start);
        }
        else
        {
            throw "Expected '&' in function name";
        }
    }

  public:
    inline static constexpr auto name = extra_funname_from_classname();
};
template <auto P> constexpr auto get_fun_name_v = get_fun_name<P>::name;

template <auto callable> class get_return_type;

// 函数指针特化
template <typename R, typename... Args, R (*F)(Args...)> class get_return_type<F>
{
  public:
    using Type = R;
};

// 成员函数指针特化
template <typename R, typename C, typename... Args, R (C::*F)(Args...)> class get_return_type<F>
{
  public:
    using Type = R;
};

// const成员函数指针特化
template <typename R, typename C, typename... Args, R (C::*F)(Args...) const>
class get_return_type<F>
{
  public:
    using Type = R;
};

// noexcept函数指针特化
template <typename R, typename... Args, R (*F)(Args...) noexcept> class get_return_type<F>
{
  public:
    using Type = R;
};
template <auto fun> using get_return_type_t = get_return_type<fun>::Type;

#define constexpr_try(x)                                                                           \
    if constexpr (requires { x })                                                                  \
    {                                                                                              \
        x                                                                                          \
    }
#define constexpr_catch else

// 主模板声明
template <auto callable, size_t Index> class get_args_type_t;

// 函数指针特化
template <typename R, typename... Args, size_t Index, R (*F)(Args...)>
class get_args_type_t<F, Index>
{
  private:
    // 使用tuple保存所有参数类型
    using args_tuple = std::tuple<Args...>;
    static_assert(Index < sizeof...(Args), "Parameter index out of range");

  public:
    // 通过tuple_element获取指定位置的类型
    using type = std::tuple_element_t<Index, args_tuple>;
};

// 成员函数指针特化
template <typename R, typename C, typename... Args, size_t Index, R (C::*F)(Args...)>
class get_args_type_t<F, Index>
{
  private:
    using args_tuple = std::tuple<Args...>;
    static_assert(Index < sizeof...(Args), "Parameter index out of range");

  public:
    using type = std::tuple_element_t<Index, args_tuple>;
};

// const成员函数指针特化
template <typename R, typename C, typename... Args, size_t Index, R (C::*F)(Args...) const>
class get_args_type_t<F, Index>
{
  private:
    using args_tuple = std::tuple<Args...>;
    static_assert(Index < sizeof...(Args), "Parameter index out of range");

  public:
    using type = std::tuple_element_t<Index, args_tuple>;
};

// noexcept函数指针特化
template <typename R, typename... Args, size_t Index, R (*F)(Args...) noexcept>
class get_args_type_t<F, Index>
{
  private:
    using args_tuple = std::tuple<Args...>;
    static_assert(Index < sizeof...(Args), "Parameter index out of range");

  public:
    using type = std::tuple_element_t<Index, args_tuple>;
};

// 添加帮助模板别名
template <auto F, size_t Index> using get_args_type = typename get_args_type_t<F, Index>::type;

// 获取函数参数数量 - 非类型模板参数版本
template <auto callable> class get_args_count_t; // 保留类模板版本

// 函数指针特化
template <typename R, typename... Args, R (*F)(Args...)> class get_args_count_t<F>
{
  public:
    static constexpr size_t value = sizeof...(Args);
};

// 成员函数指针特化
template <typename R, typename C, typename... Args, R (C::*F)(Args...)> class get_args_count_t<F>
{
  public:
    static constexpr size_t value = sizeof...(Args);
};

// const成员函数指针特化
template <typename R, typename C, typename... Args, R (C::*F)(Args...) const>
class get_args_count_t<F>
{
  public:
    static constexpr size_t value = sizeof...(Args);
};

// noexcept函数指针特化
template <typename R, typename... Args, R (*F)(Args...) noexcept> class get_args_count_t<F>
{
  public:
    static constexpr size_t value = sizeof...(Args);
};

// 非模板运行时函数形式 - 使用不同的名称避免与类模板冲突
template <typename F> constexpr std::size_t get_args_count_by_ptr(F&& f)
{
    return callable_reflection::get_args_nums(std::forward<F>(f));
}

// 类型模板参数版本 - 使用不同的命名避免冲突
template <typename F> struct get_args_count_type
{
    static constexpr std::size_t value = callable_reflection::args_count<F>::value;
};

// 变量模板版本 - 非类型参数
template <auto F> inline constexpr size_t get_args_count = get_args_count_t<F>::value;

// 变量模板版本 - 类型参数
template <typename F>
inline constexpr std::size_t get_args_count_type_v = get_args_count_type<F>::value;

// 简便接口 - 直接接受可调用对象
template <typename F> constexpr std::size_t get_args_nums(F&& f)
{
    return callable_reflection::get_args_nums(std::forward<F>(f));
}

template <std::size_t N, typename F> using get_arg_type_t = callable_reflection::arg_type_t<N, F>;

template <typename F> using get_return_type_of = callable_reflection::return_type_t<F>;

template <typename F> using get_args_tuple_t = callable_reflection::args_tuple_t<F>;

template <typename F>
inline constexpr bool is_member_function_v =
    callable_reflection::is_member_function<std::decay_t<F>>::value;

// 检查参数类型兼容性
template <typename F, typename... Args>
inline constexpr bool can_be_called_with = []()
{
    if constexpr (sizeof...(Args) != callable_reflection::get_args_nums(std::declval<F>()))
    {
        return false;
    }
    else
    {
        using FArgs = callable_reflection::args_tuple_t<F>;
        return []<std::size_t... I>(std::index_sequence<I...>)
        {
            return (std::is_convertible_v<Args, std::tuple_element_t<I, FArgs>> && ...);
        }(std::make_index_sequence<sizeof...(Args)>{});
    }
}();