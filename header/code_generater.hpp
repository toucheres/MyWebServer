// #pragma once

// // ==================== 基础宏用法 ====================

// // 1. 简单宏定义
// #define PI 3.14159
// #define MAX_SIZE 1024

// // 2. 函数式宏
// #define SQUARE(x) ((x) * (x))
// #define MAX(a, b) ((a) > (b) ? (a) : (b))

// // 3. 多行宏（使用反斜杠续行）
// #define DEBUG_PRINT(msg)                                                                           \
//     do                                                                                             \
//     {                                                                                              \
//         std::cout << "[DEBUG] " << msg << std::endl;                                               \
//     } while (0)

// // ==================== 宏的展开机制 ====================

// // 宏展开是递归的，但有规则限制

// // 4. 字符串化操作符 #
// #define STRINGIFY(x) #x
// #define TOSTRING(x) STRINGIFY(x)

// // 演示：
// // STRINGIFY(hello) -> "hello"
// // TOSTRING(PI) -> STRINGIFY(3.14159) -> "3.14159"

// // 5. 连接操作符 ##
// #define CONCAT(a, b) a##b
// #define CONCAT3(a, b, c) a##b##c

// // 演示：
// // CONCAT(var, 1) -> var1
// // CONCAT3(my, _, function) -> my_function

// // ==================== 宏的多次展开 ====================

// // 6. 强制宏展开的技巧
// #define EXPAND(x) x
// #define DELAY(x) x EMPTY()
// #define EMPTY()

// // 这些宏用于控制展开顺序
// #define DEFER(id) id EMPTY()
// #define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()

// // ==================== 递归宏展开示例 ====================

// // 7. 计数器宏系统
// #define INC(x) CONCAT(INC_, x)
// #define INC_0 1
// #define INC_1 2
// #define INC_2 3
// #define INC_3 4
// #define INC_4 5
// #define INC_5 6
// #define INC_6 7
// #define INC_7 8
// #define INC_8 9
// #define INC_9 10

// // 演示多次展开：
// // INC(3) -> CONCAT(INC_, 3) -> INC_3 -> 4
// // INC(INC(2)) -> INC(CONCAT(INC_, 2)) -> INC(INC_2) -> INC(3) -> 4

// // 8. 递减宏
// #define DEC(x) CONCAT(DEC_, x)
// #define DEC_1 0
// #define DEC_2 1
// #define DEC_3 2
// #define DEC_4 3
// #define DEC_5 4

// // ==================== 条件宏展开 ====================

// // 9. 布尔转换宏
// #define BOOL(x) CONCAT(BOOL_, x)
// #define BOOL_0 0
// #define BOOL_1 1
// #define BOOL_2 1
// #define BOOL_3 1
// #define BOOL_4 1
// #define BOOL_5 1

// // 演示：
// // BOOL(0) -> 0
// // BOOL(5) -> 1

// // 10. 条件选择宏
// #define IF(cond) CONCAT(IF_, BOOL(cond))
// #define IF_0(then, else) else
// #define IF_1(then, else) then

// // 演示多层展开：
// // IF(3)(yes, no) -> CONCAT(IF_, BOOL(3))(yes, no)
// //                -> CONCAT(IF_, 1)(yes, no)
// //                -> IF_1(yes, no)
// //                -> yes

// // ==================== 循环宏实现 ====================

// // 11. 递归循环宏
// #define REPEAT_0(macro, data)
// #define REPEAT_1(macro, data) macro(0, data)
// #define REPEAT_2(macro, data) REPEAT_1(macro, data) macro(1, data)
// #define REPEAT_3(macro, data) REPEAT_2(macro, data) macro(2, data)
// #define REPEAT_4(macro, data) REPEAT_3(macro, data) macro(3, data)
// #define REPEAT_5(macro, data) REPEAT_4(macro, data) macro(4, data)

// #define REPEAT(n, macro, data) CONCAT(REPEAT_, n)(macro, data)

// // 演示递归展开：
// // REPEAT(3, PRINT_VAR, int)
// // -> CONCAT(REPEAT_, 3)(PRINT_VAR, int)
// // -> REPEAT_3(PRINT_VAR, int)
// // -> REPEAT_2(PRINT_VAR, int) PRINT_VAR(2, int)
// // -> REPEAT_1(PRINT_VAR, int) PRINT_VAR(1, int) PRINT_VAR(2, int)
// // -> PRINT_VAR(0, int) PRINT_VAR(1, int) PRINT_VAR(2, int)

// // ==================== 复杂宏展开示例 ====================

// // 12. 列表生成宏
// #define COMMA_SEP_0(macro, data)
// #define COMMA_SEP_1(macro, data) macro(0, data)
// #define COMMA_SEP_2(macro, data) macro(0, data), macro(1, data)
// #define COMMA_SEP_3(macro, data) COMMA_SEP_2(macro, data), macro(2, data)

// #define COMMA_LIST(n, macro, data) CONCAT(COMMA_SEP_, n)(macro, data)

// // 13. 变量生成宏
// #define MAKE_VAR(i, prefix) CONCAT(prefix, i)

// // 演示复合展开：
// // COMMA_LIST(3, MAKE_VAR, var)
// // -> CONCAT(COMMA_SEP_, 3)(MAKE_VAR, var)
// // -> COMMA_SEP_3(MAKE_VAR, var)
// // -> COMMA_SEP_2(MAKE_VAR, var), MAKE_VAR(2, var)
// // -> MAKE_VAR(0, var), MAKE_VAR(1, var), MAKE_VAR(2, var)
// // -> CONCAT(var, 0), CONCAT(var, 1), CONCAT(var, 2)
// // -> var0, var1, var2

// // ==================== 高级宏技巧 ====================

// // 14. 参数计数宏
// #define NARGS(...) NARGS_(__VA_ARGS__, 5, 4, 3, 2, 1, 0)
// #define NARGS_(a, b, c, d, e, N, ...) N

// // 15. 可变参数宏
// #define FOREACH(macro, ...) FOREACH_(NARGS(__VA_ARGS__), macro, __VA_ARGS__)
// #define FOREACH_(n, macro, ...) CONCAT(FOREACH_, n)(macro, __VA_ARGS__)

// #define FOREACH_1(macro, a) macro(a)
// #define FOREACH_2(macro, a, b) macro(a) macro(b)
// #define FOREACH_3(macro, a, b, c) macro(a) macro(b) macro(c)

// // 16. 强制多次展开的宏
// #define EVAL0(...) __VA_ARGS__
// #define EVAL1(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))
// #define EVAL2(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
// #define EVAL3(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
// #define EVAL4(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
// #define EVAL(...) EVAL4(EVAL4(__VA_ARGS__))

// // ==================== 宏展开的注意事项 ====================

// /*
// 宏展开规则：
// 1. 宏只展开一次，除非结果又是宏
// 2. #和##操作符阻止进一步展开
// 3. 宏不能递归调用自身
// 4. 使用EVAL可以强制多次展开
// 5. 参数宏的参数在替换前会先展开

// 示例问题和解决方案：
// */

// // 问题：直接连接不会展开
// #define VAR var
// #define INDEX 1
// // CONCAT(VAR, INDEX) -> VARINDEX （错误）

// // 解决：先展开再连接
// #define CONCAT_EXPAND(a, b) CONCAT(a, b)
// // CONCAT_EXPAND(VAR, INDEX) -> CONCAT(var, 1) -> var1 （正确）

// // ==================== 实际应用示例 ====================

// // 17. 自动生成结构体成员
// #define DECLARE_MEMBER(type, name) type name;
// #define DECLARE_GETTER(type, name)                                                                 \
//     type get_##name() const                                                                        \
//     {                                                                                              \
//         return name;                                                                               \
//     }
// #define DECLARE_SETTER(type, name)                                                                 \
//     void set_##name(const type& val)                                                               \
//     {                                                                                              \
//         name = val;                                                                                \
//     }

// #define PROPERTY(type, name)                                                                       \
//     DECLARE_MEMBER(type, name)                                                                     \
//     DECLARE_GETTER(type, name)                                                                     \
//     DECLARE_SETTER(type, name)

// // 18. 批量生成函数重载
// #define OVERLOAD_FOR_TYPE(type)                                                                    \
//     void process(const type& val)                                                                  \
//     {                                                                                              \
//         std::cout << "Processing " #type ": " << val << std::endl;                                 \
//     }

// // 使用：
// // OVERLOAD_FOR_TYPE(int)
// // OVERLOAD_FOR_TYPE(double)
// // OVERLOAD_FOR_TYPE(string)

// // ==================== 调试宏展开 ====================

// // 19. 宏展开调试技巧
// #ifdef DEBUG_MACROS
// #define DBG_EXPAND(x) #x " = " STRINGIFY(x)
// #else
// #define DBG_EXPAND(x) x
// #endif

// // 20. 编译时打印宏值
// #define COMPILE_TIME_ASSERT_MSG(cond, msg) static_assert(cond, msg ": " #cond)

// // ==================== 使用示例 ====================

// /*
// // 使用REPEAT生成变量声明
// #define DECLARE_VAR(i, type) type var##i;
// REPEAT(5, DECLARE_VAR, int)
// // 展开为：int var0; int var1; int var2; int var3; int var4;

// // 使用COMMA_LIST生成参数列表
// #define MAKE_PARAM(i, type) type param##i
// void func(COMMA_LIST(3, MAKE_PARAM, int));
// // 展开为：void func(int param0, int param1, int param2);

// // 使用条件宏
// #define CONFIG_VALUE 1
// auto result = IF(CONFIG_VALUE)(enabled, disabled);
// // 展开为：auto result = enabled;
// */