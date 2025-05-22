#ifndef MYSQL_RESULT_H
#define MYSQL_RESULT_H

#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <memory>

// 前向声明
class MySQLHandle;
class MySQLStatement; // 添加 MySQLStatement 类的前向声明

class MySQLResult {
public:
    MySQLResult();
    ~MySQLResult();
    
    // 获取结果行数
    int getRowCount() const;
    
    // 获取字段数
    int getFieldCount() const;
    
    // 获取一行结果(以vector形式)
    std::vector<std::string> fetchRow();
    
    // 检查结果是否为空
    bool empty() const;
    
    // 获取上一次错误信息
    std::string getLastError() const;
    
    // 获取受影响的行数
    long long getAffectedRows() const;

private:
    friend class MySQLHandle; // 允许MySQLHandle访问私有成员
    friend class MySQLStatement; // 添加 MySQLStatement 作为友元
    
    // 只能由友元类创建
    MySQLResult(MYSQL* conn, MYSQL_RES* result, MYSQL_STMT* stmt);
    
    // 禁止复制
    MySQLResult(const MySQLResult&) = delete;
    MySQLResult& operator=(const MySQLResult&) = delete;
    
    // 结果处理辅助函数
    void bindResultBuffers();
    void freeResultBindings();

    MYSQL* conn;         // 数据库连接（不拥有）
    MYSQL_RES* result;   // 结果集
    MYSQL_STMT* stmt;    // 准备语句（不拥有）
    bool ownResult;      // 是否拥有result的所有权
    
    // 结果集绑定相关
    MYSQL_BIND* resultBinds;
    std::vector<std::vector<char>> resultBuffers;
    std::vector<unsigned long> resultLengths;
    std::vector<char> resultNulls;
};

// 共享指针类型定义，方便使用
using MySQLResultPtr = std::shared_ptr<MySQLResult>;

#endif // MYSQL_RESULT_H
