#pragma once
#include "reflection_ylt.hpp"
#include "signal_slots.hpp"
#include <string_view>

// 基础类 - 使用虚函数获取类型
class object_base_common : public enable_auto_remove_slots
{
  public:
    std::unordered_multimap<std::string_view, object_base_common*> children;
    
    // 移除实例成员变量type
    // 改用虚函数获取类型
    virtual std::string_view get_type() const = 0;

    virtual ~object_base_common()
    {
        for (auto p : children)
        {
            delete (p.second);
        }
        children.clear();
    }

    auto addchild(object_base_common* child)
    {
        // 使用虚函数获取类型
        children.insert(std::pair<std::string_view, object_base_common*>(child->get_type(), child));
    }

    auto setparent(object_base_common* parent)
    {
        parent->addchild(this);
    }
    auto setparent(object_base_common& parent)
    {
        parent.addchild(this);
    }

    auto findcildren(std::string_view object_type)
    {
        return children.equal_range(object_type); // 返回 pair<iterator, iterator>
    }
};

// CRTP模式 - 使用静态类型名
template <typename Derived>
class object_with_type : public object_base_common
{
  private:
    // 每个类型有一个静态字符串
    static inline std::string_view type_name = get_class_name<Derived>();

  protected:
    object_with_type() : object_base_common() {}

  public:
    // 实现虚函数，返回静态类型名
    std::string_view get_type() const override { return type_name; }
};

// 特殊处理未知类型对象 - 允许实例级别的类型名
class unkown_object : public object_base_common
{
  private:
    std::string instance_type = "unknown"; // 实例级别存储

  protected:
    unkown_object() = default;

  public:
    // 设置实例类型名
    void set_type(std::string_view name) { instance_type = name; }

    // 获取类型实现
    std::string_view get_type() const override { return instance_type; }
};

// 使用简化的别名
using object = unkown_object;
