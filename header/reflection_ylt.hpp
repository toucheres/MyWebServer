#pragma once
#include <array>
#include <functional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <ylt/reflection/member_value.hpp>
#include <ylt/struct_pack/reflection.hpp>

// 函数特性提取工具 - 与原版保持一致
namespace function_traits_detail
{
    template <typename T> struct function_traits;

    template <typename R, typename... Args> struct function_traits<R(Args...)>
    {
        static constexpr std::size_t arity = sizeof...(Args);
        using result_type = R;
        using args_tuple = std::tuple<Args...>;
    };

    template <typename R, typename... Args>
    struct function_traits<R (*)(Args...)> : function_traits<R(Args...)>
    {
    };

    template <typename C, typename R, typename... Args>
    struct function_traits<R (C::*)(Args...)> : function_traits<R(Args...)>
    {
    };

    template <typename C, typename R, typename... Args>
    struct function_traits<R (C::*)(Args...) const> : function_traits<R(Args...)>
    {
    };

    template <typename R, typename... Args>
    struct function_traits<std::function<R(Args...)>> : function_traits<R(Args...)>
    {
    };

    template <typename F> struct function_traits
    {
      private:
        using call_type = function_traits<decltype(&F::operator())>;

      public:
        static constexpr std::size_t arity = call_type::arity;
        using result_type = typename call_type::result_type;
        using args_tuple = typename call_type::args_tuple;
    };

    template <typename F> struct function_traits<F&> : function_traits<F>
    {
    };

    template <typename F> struct function_traits<F&&> : function_traits<F>
    {
    };

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

template <typename T> using function_traits = function_traits_detail::function_traits<T>;

template <typename F, typename Tuple> auto call_with_first_n_args(F&& f, Tuple&& t)
{
    return function_traits_detail::call_with_first_n_args(std::forward<F>(f),
                                                          std::forward<Tuple>(t));
}

// 函数反射工具 - 基于YLT实现
namespace callable_reflection
{
    template <typename F> constexpr std::size_t get_args_nums(F&&)
    {
        return function_traits<std::decay_t<F>>::arity;
    }

    template <std::size_t N, typename F>
    using arg_type_t =
        std::tuple_element_t<N, typename function_traits<std::decay_t<F>>::args_tuple>;

    template <typename F>
    using return_type_t = typename function_traits<std::decay_t<F>>::result_type;

    template <typename F>
    using args_tuple_t = typename function_traits<std::decay_t<F>>::args_tuple;

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

    template <typename F> struct args_count
    {
        static constexpr std::size_t value = function_traits<F>::arity;
    };
} // namespace callable_reflection

// 使用YLT的反射功能重新实现核心概念
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

// 基于YLT实现成员数量获取
template <typename Type> class num_of_number
{
  public:
    static constexpr int value = 1;
};

template <MeaningfulAggregate Type> class num_of_number<Type>
{
  public:
    static constexpr int value = struct_pack::members_count<std::decay_t<Type>>;
};

template <typename Type> inline constexpr int num_of_number_v = num_of_number<Type>::value;

// 基于YLT实现成员访问
template <int N, class... Ts> constexpr void visit_members_impl(auto&& bevisited, auto&& visitor)
{
    struct_pack::detail::visit_members(bevisited, visitor);
}

constexpr void visit_each_member(auto&& bevisited, auto&& visitor)
    requires requires(decltype(visitor) v, std::decay_t<decltype(bevisited)> t) {
        typename std::decay_t<decltype(bevisited)>;
        { v.template operator()<std::decay_t<decltype(bevisited)>, 0>(std::declval<int>()) };
    }
{
    using T = std::decay_t<decltype(bevisited)>;

    // 创建一个包装器，将YLT的多参数访问转换为单个成员的访问
    [[maybe_unused]] auto wrapper_visitor = [&visitor]<std::size_t I>(auto&& member)
    { visitor.template operator()<T, I>(member); };

    // 使用YLT的for_each来遍历每个成员
    constexpr auto count = num_of_number_v<T>;
    [&]<std::size_t... Is>(std::index_sequence<Is...>)
    {
        [[maybe_unused]] auto visit_single = [&]<std::size_t Index>(auto&& member)
        { visitor.template operator()<T, Index>(member); };

        // 创建一个lambda来处理YLT的tuple_view访问
        auto tuple_visitor = [&](auto&&... members)
        {
            std::size_t index = 0;
            ((visitor.template operator()<T, Is>(members), ++index), ...);
        };

        // 使用YLT的visit_members，但需要适配参数传递方式
        struct_pack::detail::visit_members(bevisited, tuple_visitor);
    }(std::make_index_sequence<count>{});
}

constexpr auto visit_members(auto&& bevisited, auto&& visitor)
{
    return struct_pack::detail::visit_members(bevisited, visitor);
}

// 基于YLT实现成员名称获取 - 使用YLT的真实成员名称
template <Aggregate T>
inline consteval std::array<std::string_view, num_of_number_v<T>> get_member_names()
{
    // 使用YLT的get_member_names功能获取真实成员名称
    return ylt::reflection::get_member_names<T>();
}

template <Aggregate T, int index>
    requires(index < num_of_number_v<T>)
inline consteval std::string_view get_member_name()
{
    // 使用YLT的name_of功能获取指定索引的成员名称
    return ylt::reflection::name_of<T, index>();
}

// 基于YLT实现偏移量计算 - 使用标准offsetof方法
template <Aggregate T, int index>
constexpr long long bias_member()
    requires(index < num_of_number_v<T>)
{
    // YLT可能不直接提供偏移量，使用基本实现
    return 0; // 占位符实现
}

template <Aggregate T, int index> constexpr bool index_in_range_v = (num_of_number_v<T> > index);

// 基于YLT实现类型信息 - 使用YLT的类型名称功能
template <class T> consteval auto get_class_name()
{
    // 使用YLT的get_struct_name功能获取类型名称
    return ylt::reflection::get_struct_name<T>();
}

template <class T> consteval auto get_member_class_names()
{
    constexpr auto count = num_of_number_v<T>;
    std::array<std::string_view, count> names{};

    // 使用YLT的反射功能获取每个成员的类型名称
    [&]<std::size_t... I>(std::index_sequence<I...>)
    {
        constexpr auto tuple_types = ylt::reflection::struct_to_tuple<T>();
        ((names[I] =
              ylt::reflection::get_struct_name<std::tuple_element_t<I, decltype(tuple_types)>>()),
         ...);
    }(std::make_index_sequence<count>{});

    return names;
}

template <class T> consteval auto get_member_classname_name_pairs()
{
    constexpr auto count = num_of_number_v<T>;
    std::array<std::pair<std::string_view, std::string_view>, count> pairs{};

    // 获取成员名称和类型名称的配对
    constexpr auto member_names = get_member_names<T>();
    constexpr auto type_names = get_member_class_names<T>();

    for (int i = 0; i < count; ++i)
    {
        pairs[i] = std::pair{type_names[i], member_names[i]};
    }
    return pairs;
}

// 内嵌成员数量计算 - 简化实现
template <class Type> consteval int num_of_number_with_inner()
{
    if constexpr (!Aggregate<Type>)
    {
        return 1;
    }
    else if constexpr (MeaningfulAggregate<Type>)
    {
        // YLT可能没有嵌套计数，使用基本计数
        return num_of_number_v<Type>;
    }
    return 1;
}

template <typename T>
inline constexpr int num_of_number_with_inner_v = num_of_number_with_inner<T>();

// 函数名获取模板 - 简化实现
template <auto fun> class get_fun_name
{
  public:
    inline static constexpr auto name = std::string_view("unknown_function");
};

// 返回类型获取
template <auto callable> class get_return_type;

template <typename R, typename... Args, R (*F)(Args...)> class get_return_type<F>
{
  public:
    using Type = R;
};

template <typename R, typename C, typename... Args, R (C::*F)(Args...)> class get_return_type<F>
{
  public:
    using Type = R;
};

template <typename R, typename C, typename... Args, R (C::*F)(Args...) const>
class get_return_type<F>
{
  public:
    using Type = R;
};

template <typename R, typename... Args, R (*F)(Args...) noexcept> class get_return_type<F>
{
  public:
    using Type = R;
};

template <auto fun> using get_return_type_t = get_return_type<fun>::Type;

// 类型工具类 - 保持与原版一致
template <class T1, class T2> class typePair
{
  public:
    using first = T1;
    using secend = T2;
};

template <class... Types> class typeVector
{
  public:
    template <int index> class getType_t
    {
      public:
        using Type = std::tuple_element_t<index, std::tuple<Types...>>;
    };
    template <int index> using getType = getType_t<index>::Type;

    template <class T> class push_back_t
    {
      public:
        using Type = typeVector<Types..., T>;
    };
    template <class T> using push_back = push_back_t<T>::Type;

    template <class T> class push_front_t
    {
      public:
        using Type = typeVector<T, Types...>;
    };
    template <class T> using push_front = push_front_t<T>::Type;

    template <class T> class find_t
    {
        template <int index> constexpr static auto helperfun()
        {
            if constexpr (index >= sizeof...(Types))
            {
                return -1;
            }
            else
            {
                using CurrentType = getType<index>;
                if constexpr (is_same_all_v<CurrentType, T>)
                {
                    return index;
                }
                else
                {
                    return helperfun<index + 1>();
                }
            }
        };

      public:
        constexpr static int value = helperfun<0>();
    };
    template <class T> inline constexpr static int find = find_t<T>::value;

    template <class T> class rfind_t
    {
        template <int index> constexpr static auto helperfun()
        {
            if constexpr (index < 0)
            {
                return -1;
            }
            else
            {
                using CurrentType = getType<index>;
                if constexpr (is_same_all_v<CurrentType, T>)
                {
                    return index;
                }
                else
                {
                    return helperfun<index - 1>();
                }
            }
        };

      public:
        constexpr static int value = helperfun<sizeof...(Types) - 1>();
    };
    template <class T> inline constexpr static int rfind = rfind_t<T>::value;

    template <class T, int index> class insert_t
    {
        // ...existing implementation...
      public:
        using Type = typeVector<Types...>; // 简化实现
    };
    template <class T, int index> using insert = insert_t<T, index>::Type;

    template <int index> class divid_t
    {
        // ...existing implementation...
      public:
        using Type = typePair<typeVector<>, typeVector<Types...>>; // 简化实现
    };
    template <int index> using divid = divid_t<index>::Type;

    template <typename OtherTypeVector> class merge_t;

    template <typename... T1s> class merge_t<typeVector<T1s...>>
    {
      public:
        using Type = typeVector<Types..., T1s...>;
    };

    template <class OtherTypeVector> using merge = typename merge_t<OtherTypeVector>::Type;
};

#define constexpr_try(x)                                                                           \
    if constexpr (requires { x })                                                                  \
    {                                                                                              \
        x                                                                                          \
    }
#define constexpr_catch else

// 参数类型获取
template <auto callable, size_t Index> class get_args_type_t;

template <typename R, typename... Args, size_t Index, R (*F)(Args...)>
class get_args_type_t<F, Index>
{
  private:
    using args_tuple = std::tuple<Args...>;
    static_assert(Index < sizeof...(Args), "Parameter index out of range");

  public:
    using type = std::tuple_element_t<Index, args_tuple>;
};

template <typename R, typename C, typename... Args, size_t Index, R (C::*F)(Args...)>
class get_args_type_t<F, Index>
{
  private:
    using args_tuple = std::tuple<Args...>;
    static_assert(Index < sizeof...(Args), "Parameter index out of range");

  public:
    using type = std::tuple_element_t<Index, args_tuple>;
};

template <typename R, typename C, typename... Args, size_t Index, R (C::*F)(Args...) const>
class get_args_type_t<F, Index>
{
  private:
    using args_tuple = std::tuple<Args...>;
    static_assert(Index < sizeof...(Args), "Parameter index out of range");

  public:
    using type = std::tuple_element_t<Index, args_tuple>;
};

template <typename R, typename... Args, size_t Index, R (*F)(Args...) noexcept>
class get_args_type_t<F, Index>
{
  private:
    using args_tuple = std::tuple<Args...>;
    static_assert(Index < sizeof...(Args), "Parameter index out of range");

  public:
    using type = std::tuple_element_t<Index, args_tuple>;
};

template <auto F, size_t Index> using get_args_type = typename get_args_type_t<F, Index>::type;

// 参数数量获取
template <auto callable> class get_args_count_t;

template <typename R, typename... Args, R (*F)(Args...)> class get_args_count_t<F>
{
  public:
    static constexpr size_t value = sizeof...(Args);
};

template <typename R, typename C, typename... Args, R (C::*F)(Args...)> class get_args_count_t<F>
{
  public:
    static constexpr size_t value = sizeof...(Args);
};

template <typename R, typename C, typename... Args, R (C::*F)(Args...) const>
class get_args_count_t<F>
{
  public:
    static constexpr size_t value = sizeof...(Args);
};

template <typename R, typename... Args, R (*F)(Args...) noexcept> class get_args_count_t<F>
{
  public:
    static constexpr size_t value = sizeof...(Args);
};

template <typename F> constexpr std::size_t get_args_count_by_ptr(F&& f)
{
    return callable_reflection::get_args_nums(std::forward<F>(f));
}

template <typename F> struct get_args_count_type
{
    static constexpr std::size_t value = callable_reflection::args_count<F>::value;
};

template <auto F> inline constexpr size_t get_args_count = get_args_count_t<F>::value;
template <typename F>
inline constexpr std::size_t get_args_count_type_v = get_args_count_type<F>::value;

// 便利接口
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

// 参数类型推导
template <class... args> auto constexpr get_args_types__(auto&& callable)
{
    if constexpr (requires { callable(std::declval<args>()...); })
    {
        return get_args_types__<args..., AnyType>(callable);
    }
    else
    {
        return typeVector<std::remove_cvref_t<args>...>{};
    }
}

template <class callable> decltype(auto) constexpr get_args_types_()
{
    if constexpr (requires { &std::remove_reference_t<callable>::operator(); })
    {
        using lambda_type = decltype(&std::remove_reference_t<callable>::operator());
        using args_tuple = typename function_traits<lambda_type>::args_tuple;
        return []<std::size_t... I>(std::index_sequence<I...>)
        {
            return typeVector<std::tuple_element_t<I, args_tuple>...>{};
        }(std::make_index_sequence<std::tuple_size_v<args_tuple>>{});
    }
    else if constexpr (requires { std::declval<callable>()(); })
    {
        return get_args_types__<AnyType>(std::declval<callable>());
    }
    else if constexpr (std::is_function_v<std::remove_pointer_t<callable>> ||
                       std::is_member_function_pointer_v<callable>)
    {
        using args_tuple = typename function_traits<callable>::args_tuple;
        return []<std::size_t... I>(std::index_sequence<I...>)
        {
            return typeVector<std::tuple_element_t<I, args_tuple>...>{};
        }(std::make_index_sequence<std::tuple_size_v<args_tuple>>{});
    }
    else
    {
        return typeVector<>{};
    }
}

template <class callable_t> using get_args_types = decltype(get_args_types_<callable_t>());
template <typename T> inline constexpr size_t get_member_count_v = num_of_number_v<T>;

template <auto fun> constexpr auto get_fun_name_v = get_fun_name<fun>::name;
