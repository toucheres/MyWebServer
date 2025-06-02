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

template <class Type, int MAXNUM = 32> class num_of_number
{
    template <class T, class... Args, int num = 0>
    static consteval int num_of_number_fun_()
        requires requires { is_same_all<AnyType, Args...>::value; }
    {
        if constexpr (requires { T{std::declval<Args>()...}; })
        {
            if constexpr (requires { num_of_number_fun_<T, AnyType, Args..., num + 1>() >= 0; })
            {
                return num_of_number_fun_<T, AnyType, Args..., num + 1>();
            }
        }
        else
        {
            return num - 1;
        }
    }
    template <class T_> static consteval int num_of_number_fun_start()
    {
        return num_of_number::num_of_number_fun_<T_>();
    };

  public:
    static constexpr int value = num_of_number_fun_start<Type>();
};
struct person
{
    int age;
    std::string name;
};
void testfun()
{
    person{10, "test"};
    person{AnyType{}, AnyType{}};
    num_of_number<person>::value;
    is_same_all<AnyType, AnyType, AnyType>::value;
}