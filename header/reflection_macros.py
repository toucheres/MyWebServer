#!/usr/bin/env python3
# 生成支持128个成员的反射宏文件

def generate_header():
    return """#ifndef REFLECTION_MACROS_HPP
#define REFLECTION_MACROS_HPP
"""

def generate_footer():
    return """#endif // REFLECTION_MACROS_HPP
"""

def generate_visit_cases(max_count):
    result = []
    
    # 生成VISIT_CASE_0（特殊情况）
    result.append("""// 0号特殊处理
#define VISIT_CASE_0 \\
    if constexpr (N == 0) \\
    { \\
        /* 无成员，不需要处理 */ \\
    }
""")
    
    # 生成VISIT_CASE_1到VISIT_CASE_MAX
    for i in range(1, max_count + 1):
        args = ", ".join([f"a{j}" for j in range(1, i + 1)])
        result.append(f"""#define VISIT_CASE_{i} \\
    else if constexpr (N == {i}) \\
    {{ \\
        auto&& [{args}] = bevisited; \\
        visit_helper({args}); \\
    }}
""")
    
    result.append("// 生成指定数量的分支\n#define GENERATE_VISIT_CASES(MAX) VISIT_CASES_IMPL_##MAX\n")
    
    # 生成VISIT_CASES_IMPL_0
    result.append("#define VISIT_CASES_IMPL_0 VISIT_CASE_0")
    
    # 生成VISIT_CASES_IMPL_1到VISIT_CASES_IMPL_MAX
    for i in range(1, max_count + 1):
        result.append(f"#define VISIT_CASES_IMPL_{i} VISIT_CASES_IMPL_{i-1} VISIT_CASE_{i}")
    
    return "\n".join(result)

def generate_tuple_cases(max_count):
    result = []
    
    # 生成TUPLE_CASE_0（特殊情况）
    result.append("""// 为make_static_memberptr_tuple_form_type添加的宏
#define TUPLE_CASE_0 \\
    if constexpr (Count == 0) \\
    { \\
        return std::tuple<>{}; \\
    }
""")
    
    # 生成TUPLE_CASE_1到TUPLE_CASE_MAX
    for i in range(1, max_count + 1):
        args = ", ".join([f"a{j}" for j in range(1, i + 1)])
        ptr_args = ", ".join([f"&a{j}" for j in range(1, i + 1)])
        result.append(f"""#define TUPLE_CASE_{i} \\
    else if constexpr (Count == {i}) \\
    {{ \\
        static constexpr T obj{{}}; \\
        auto&& [{args}] = obj; \\
        return std::make_tuple({ptr_args}); \\
    }}
""")
    
    result.append("// 生成指定数量的tuple分支\n#define GENERATE_TUPLE_CASES(MAX) TUPLE_CASES_IMPL_##MAX\n")
    
    # 生成TUPLE_CASES_IMPL_0
    result.append("#define TUPLE_CASES_IMPL_0 TUPLE_CASE_0")
    
    # 生成TUPLE_CASES_IMPL_1到TUPLE_CASES_IMPL_MAX
    for i in range(1, max_count + 1):
        result.append(f"#define TUPLE_CASES_IMPL_{i} TUPLE_CASES_IMPL_{i-1} TUPLE_CASE_{i}")
    
    return "\n".join(result)

def main():
    max_member_count = 128  # 支持最多128个成员,可改
    
    with open("reflection_macros.hpp", "w") as f:
        f.write(generate_header())
        f.write("\n")
        f.write(generate_visit_cases(max_member_count))
        f.write("\n\n")
        f.write(generate_tuple_cases(max_member_count))
        f.write("\n\n")
        f.write(generate_footer())
    
    print(f"已成功生成支持{max_member_count}个成员的反射宏文件 reflection_macros.hpp")

if __name__ == "__main__":
    main()
