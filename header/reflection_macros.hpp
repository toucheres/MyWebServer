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

#endif // REFLECTION_MACROS_HPP