#pragma once

// ==================== 基础宏工具 ====================

// 连接宏
#define CONCAT(a, b) a##b
#define CONCAT3(a, b, c) a##b##c
#define CONCAT4(a, b, c, d) a##b##c##d

// 字符串化宏
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

// 空宏
#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()

// ==================== 宏循环实现 ====================

// 计数器宏 (支持0-20)
#define COUNT_0 0
#define COUNT_1 1
#define COUNT_2 2
#define COUNT_3 3
#define COUNT_4 4
#define COUNT_5 5
#define COUNT_6 6
#define COUNT_7 7
#define COUNT_8 8
#define COUNT_9 9
#define COUNT_10 10
#define COUNT_11 11
#define COUNT_12 12
#define COUNT_13 13
#define COUNT_14 14
#define COUNT_15 15
#define COUNT_16 16
#define COUNT_17 17
#define COUNT_18 18
#define COUNT_19 19
#define COUNT_20 20

// 递增宏
#define INC(x) CONCAT(INC_, x)
#define INC_0 1
#define INC_1 2
#define INC_2 3
#define INC_3 4
#define INC_4 5
#define INC_5 6
#define INC_6 7
#define INC_7 8
#define INC_8 9
#define INC_9 10
#define INC_10 11
#define INC_11 12
#define INC_12 13
#define INC_13 14
#define INC_14 15
#define INC_15 16
#define INC_16 17
#define INC_17 18
#define INC_18 19
#define INC_19 20
#define INC_20 21

// 递减宏
#define DEC(x) CONCAT(DEC_, x)
#define DEC_1 0
#define DEC_2 1
#define DEC_3 2
#define DEC_4 3
#define DEC_5 4
#define DEC_6 5
#define DEC_7 6
#define DEC_8 7
#define DEC_9 8
#define DEC_10 9
#define DEC_11 10
#define DEC_12 11
#define DEC_13 12
#define DEC_14 13
#define DEC_15 14
#define DEC_16 15
#define DEC_17 16
#define DEC_18 17
#define DEC_19 18
#define DEC_20 19

// 布尔宏
#define BOOL(x) CONCAT(BOOL_, x)
#define BOOL_0 0
#define BOOL_1 1
#define BOOL_2 1
#define BOOL_3 1
#define BOOL_4 1
#define BOOL_5 1
#define BOOL_6 1
#define BOOL_7 1
#define BOOL_8 1
#define BOOL_9 1
#define BOOL_10 1
#define BOOL_11 1
#define BOOL_12 1
#define BOOL_13 1
#define BOOL_14 1
#define BOOL_15 1
#define BOOL_16 1
#define BOOL_17 1
#define BOOL_18 1
#define BOOL_19 1
#define BOOL_20 1

// 条件宏
#define IF(cond) CONCAT(IF_, BOOL(cond))
#define IF_0(then, else) else
#define IF_1(then, else) then

// ==================== 循环宏实现 ====================

// 递归循环宏 - 基础版本
#define LOOP_0(macro, data)
#define LOOP_1(macro, data) macro(1, data)
#define LOOP_2(macro, data) LOOP_1(macro, data) macro(2, data)
#define LOOP_3(macro, data) LOOP_2(macro, data) macro(3, data)
#define LOOP_4(macro, data) LOOP_3(macro, data) macro(4, data)
#define LOOP_5(macro, data) LOOP_4(macro, data) macro(5, data)
#define LOOP_6(macro, data) LOOP_5(macro, data) macro(6, data)
#define LOOP_7(macro, data) LOOP_6(macro, data) macro(7, data)
#define LOOP_8(macro, data) LOOP_7(macro, data) macro(8, data)
#define LOOP_9(macro, data) LOOP_8(macro, data) macro(9, data)
#define LOOP_10(macro, data) LOOP_9(macro, data) macro(10, data)
#define LOOP_11(macro, data) LOOP_10(macro, data) macro(11, data)
#define LOOP_12(macro, data) LOOP_11(macro, data) macro(12, data)
#define LOOP_13(macro, data) LOOP_12(macro, data) macro(13, data)
#define LOOP_14(macro, data) LOOP_13(macro, data) macro(14, data)
#define LOOP_15(macro, data) LOOP_14(macro, data) macro(15, data)
#define LOOP_16(macro, data) LOOP_15(macro, data) macro(16, data)
#define LOOP_17(macro, data) LOOP_16(macro, data) macro(17, data)
#define LOOP_18(macro, data) LOOP_17(macro, data) macro(18, data)
#define LOOP_19(macro, data) LOOP_18(macro, data) macro(19, data)
#define LOOP_20(macro, data) LOOP_19(macro, data) macro(20, data)

// 从0开始的循环
#define LOOP_FROM_0_0(macro, data)
#define LOOP_FROM_0_1(macro, data) macro(0, data)
#define LOOP_FROM_0_2(macro, data) LOOP_FROM_0_1(macro, data) macro(1, data)
#define LOOP_FROM_0_3(macro, data) LOOP_FROM_0_2(macro, data) macro(2, data)
#define LOOP_FROM_0_4(macro, data) LOOP_FROM_0_3(macro, data) macro(3, data)
#define LOOP_FROM_0_5(macro, data) LOOP_FROM_0_4(macro, data) macro(4, data)
#define LOOP_FROM_0_6(macro, data) LOOP_FROM_0_5(macro, data) macro(5, data)
#define LOOP_FROM_0_7(macro, data) LOOP_FROM_0_6(macro, data) macro(6, data)
#define LOOP_FROM_0_8(macro, data) LOOP_FROM_0_7(macro, data) macro(7, data)
#define LOOP_FROM_0_9(macro, data) LOOP_FROM_0_8(macro, data) macro(8, data)
#define LOOP_FROM_0_10(macro, data) LOOP_FROM_0_9(macro, data) macro(9, data)

// 通用循环宏
#define REPEAT(count, macro, data) CONCAT(LOOP_, count)(macro, data)
#define REPEAT_FROM_0(count, macro, data) CONCAT(LOOP_FROM_0_, count)(macro, data)

// ==================== 列表生成宏 ====================

// 生成逗号分隔的列表
#define COMMA_SEP_0(macro, data)
#define COMMA_SEP_1(macro, data) macro(1, data)
#define COMMA_SEP_2(macro, data) macro(1, data), macro(2, data)
#define COMMA_SEP_3(macro, data) COMMA_SEP_2(macro, data), macro(3, data)
#define COMMA_SEP_4(macro, data) COMMA_SEP_3(macro, data), macro(4, data)
#define COMMA_SEP_5(macro, data) COMMA_SEP_4(macro, data), macro(5, data)
#define COMMA_SEP_6(macro, data) COMMA_SEP_5(macro, data), macro(6, data)
#define COMMA_SEP_7(macro, data) COMMA_SEP_6(macro, data), macro(7, data)
#define COMMA_SEP_8(macro, data) COMMA_SEP_7(macro, data), macro(8, data)
#define COMMA_SEP_9(macro, data) COMMA_SEP_8(macro, data), macro(9, data)
#define COMMA_SEP_10(macro, data) COMMA_SEP_9(macro, data), macro(10, data)

#define COMMA_LIST(count, macro, data) CONCAT(COMMA_SEP_, count)(macro, data)

// 从0开始的逗号分隔列表
#define COMMA_SEP_FROM_0_0(macro, data)
#define COMMA_SEP_FROM_0_1(macro, data) macro(0, data)
#define COMMA_SEP_FROM_0_2(macro, data) macro(0, data), macro(1, data)
#define COMMA_SEP_FROM_0_3(macro, data) COMMA_SEP_FROM_0_2(macro, data), macro(2, data)
#define COMMA_SEP_FROM_0_4(macro, data) COMMA_SEP_FROM_0_3(macro, data), macro(3, data)
#define COMMA_SEP_FROM_0_5(macro, data) COMMA_SEP_FROM_0_4(macro, data), macro(4, data)
#define COMMA_SEP_FROM_0_6(macro, data) COMMA_SEP_FROM_0_5(macro, data), macro(5, data)
#define COMMA_SEP_FROM_0_7(macro, data) COMMA_SEP_FROM_0_6(macro, data), macro(6, data)
#define COMMA_SEP_FROM_0_8(macro, data) COMMA_SEP_FROM_0_7(macro, data), macro(7, data)
#define COMMA_SEP_FROM_0_9(macro, data) COMMA_SEP_FROM_0_8(macro, data), macro(8, data)
#define COMMA_SEP_FROM_0_10(macro, data) COMMA_SEP_FROM_0_9(macro, data), macro(9, data)

#define COMMA_LIST_FROM_0(count, macro, data) CONCAT(COMMA_SEP_FROM_0_, count)(macro, data)

// ==================== 常用生成宏 ====================

// 生成变量名
#define MAKE_VAR(i, prefix) CONCAT(prefix, i)
#define MAKE_TYPE(i, prefix) CONCAT(prefix, i)
#define MAKE_ARG(i, prefix) CONCAT(prefix, i)

// 生成参数声明
#define MAKE_PARAM(i, type) type CONCAT(arg, i)
#define MAKE_TEMPLATE_PARAM(i, data) class CONCAT(T, i)

// 生成初始化列表
#define MAKE_INIT(i, data)                                                                         \
    CONCAT(data, i)                                                                                \
    {                                                                                              \
    }

// ==================== 范围循环宏 ====================

// 在指定范围内循环 (start到end)
#define RANGE_LOOP_IMPL(start, end, macro, data)                                                   \
    IF(BOOL(DEC(end)), CONCAT4(RANGE_LOOP_, start, _, end)(macro, data), EMPTY())

// 范围循环的具体实现 (这里只实现几个示例)
#define RANGE_LOOP_1_5(macro, data)                                                                \
    macro(1, data) macro(2, data) macro(3, data) macro(4, data) macro(5, data)

#define RANGE_LOOP_0_3(macro, data) macro(0, data) macro(1, data) macro(2, data) macro(3, data)

// ==================== 使用示例宏 ====================

/*
使用示例1：生成变量列表
#define VAR_EXAMPLE(i, prefix) CONCAT(prefix, i)
// COMMA_LIST(3, VAR_EXAMPLE, a) 生成: a1, a2, a3

使用示例2：生成函数参数
#define PARAM_EXAMPLE(i, type) type CONCAT(arg, i)
// COMMA_LIST(3, PARAM_EXAMPLE, int) 生成: int arg1, int arg2, int arg3

使用示例3：生成模板参数
#define TEMPLATE_PARAM_EXAMPLE(i, data) class CONCAT(T, i)
// COMMA_LIST(3, TEMPLATE_PARAM_EXAMPLE, ) 生成: class T1, class T2, class T3

使用示例4：生成重复代码块
#define CASE_EXAMPLE(i, data) \
    case i: \
        handle_case_##i(); \
        break;
// REPEAT(3, CASE_EXAMPLE, ) 生成重复的case语句

使用示例5：生成if constexpr分支
#define IF_CONSTEXPR_EXAMPLE(i, condition_prefix) \
    else if constexpr (condition_prefix == i) { \
        handle_case_##i(); \
    }
// REPEAT(5, IF_CONSTEXPR_EXAMPLE, N) 生成重复的if constexpr分支
*/