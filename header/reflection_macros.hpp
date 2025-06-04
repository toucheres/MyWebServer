#ifndef REFLECTION_MACROS_HPP
#define REFLECTION_MACROS_HPP

// 0号特殊处理
#define VISIT_CASE_0                                                                               \
    if constexpr (N == 0)                                                                          \
    {                                                                                              \
        /* 无成员，不需要处理 */                                                                   \
    }

// 递归定义各个分支
#define VISIT_CASE_1                                                                               \
    else if constexpr (N == 1)                                                                     \
    {                                                                                              \
        auto&& [a1] = bevisited;                                                                   \
        visit_helper(a1);                                                                          \
    }

#define VISIT_CASE_2                                                                               \
    else if constexpr (N == 2)                                                                     \
    {                                                                                              \
        auto&& [a1, a2] = bevisited;                                                               \
        visit_helper(a1, a2);                                                                      \
    }

#define VISIT_CASE_3                                                                               \
    else if constexpr (N == 3)                                                                     \
    {                                                                                              \
        auto&& [a1, a2, a3] = bevisited;                                                           \
        visit_helper(a1, a2, a3);                                                                  \
    }

#define VISIT_CASE_4                                                                               \
    else if constexpr (N == 4)                                                                     \
    {                                                                                              \
        auto&& [a1, a2, a3, a4] = bevisited;                                                       \
        visit_helper(a1, a2, a3, a4);                                                              \
    }

#define VISIT_CASE_5                                                                               \
    else if constexpr (N == 5)                                                                     \
    {                                                                                              \
        auto&& [a1, a2, a3, a4, a5] = bevisited;                                                   \
        visit_helper(a1, a2, a3, a4, a5);                                                          \
    }

// 可继续添加更多分支...

// 生成指定数量的分支
#define GENERATE_VISIT_CASES(MAX) VISIT_CASES_IMPL_##MAX

// 实现递增生成
#define VISIT_CASES_IMPL_0 VISIT_CASE_0
#define VISIT_CASES_IMPL_1 VISIT_CASE_0 VISIT_CASE_1
#define VISIT_CASES_IMPL_2 VISIT_CASES_IMPL_1 VISIT_CASE_2
#define VISIT_CASES_IMPL_3 VISIT_CASES_IMPL_2 VISIT_CASE_3
#define VISIT_CASES_IMPL_4 VISIT_CASES_IMPL_3 VISIT_CASE_4
#define VISIT_CASES_IMPL_5 VISIT_CASES_IMPL_4 VISIT_CASE_5
// 可继续添加更多递增实现...

// 为make_static_memberptr_tuple_form_type添加的宏
#define TUPLE_CASE_0                                                                               \
    if constexpr (Count == 0)                                                                      \
    {                                                                                              \
        return std::tuple<>{};                                                                     \
    }

#define TUPLE_CASE_1                                                                               \
    else if constexpr (Count == 1)                                                                 \
    {                                                                                              \
        static constexpr T obj{};                                                                  \
        auto&& [a1] = obj;                                                                         \
        return std::make_tuple(&a1);                                                               \
    }

#define TUPLE_CASE_2                                                                               \
    else if constexpr (Count == 2)                                                                 \
    {                                                                                              \
        static constexpr T obj{};                                                                  \
        auto&& [a1, a2] = obj;                                                                     \
        return std::make_tuple(&a1, &a2);                                                          \
    }

#define TUPLE_CASE_3                                                                               \
    else if constexpr (Count == 3)                                                                 \
    {                                                                                              \
        static constexpr T obj{};                                                                  \
        auto&& [a1, a2, a3] = obj;                                                                 \
        return std::make_tuple(&a1, &a2, &a3);                                                     \
    }

#define TUPLE_CASE_4                                                                               \
    else if constexpr (Count == 4)                                                                 \
    {                                                                                              \
        static constexpr T obj{};                                                                  \
        auto&& [a1, a2, a3, a4] = obj;                                                             \
        return std::make_tuple(&a1, &a2, &a3, &a4);                                                \
    }

#define TUPLE_CASE_5                                                                               \
    else if constexpr (Count == 5)                                                                 \
    {                                                                                              \
        static constexpr T obj{};                                                                  \
        auto&& [a1, a2, a3, a4, a5] = obj;                                                         \
        return std::make_tuple(&a1, &a2, &a3, &a4, &a5);                                           \
    }

// 生成指定数量的tuple分支
#define GENERATE_TUPLE_CASES(MAX) TUPLE_CASES_IMPL_##MAX

// 实现递增生成
#define TUPLE_CASES_IMPL_0 TUPLE_CASE_0
#define TUPLE_CASES_IMPL_1 TUPLE_CASE_0 TUPLE_CASE_1
#define TUPLE_CASES_IMPL_2 TUPLE_CASES_IMPL_1 TUPLE_CASE_2
#define TUPLE_CASES_IMPL_3 TUPLE_CASES_IMPL_2 TUPLE_CASE_3
#define TUPLE_CASES_IMPL_4 TUPLE_CASES_IMPL_3 TUPLE_CASE_4
#define TUPLE_CASES_IMPL_5 TUPLE_CASES_IMPL_4 TUPLE_CASE_5
// 可继续添加更多递增实现...

#endif // REFLECTION_MACROS_HPP