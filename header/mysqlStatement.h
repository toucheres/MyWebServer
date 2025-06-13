#ifndef MYSQL_STATEMENT_H
#define MYSQL_STATEMENT_H

#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <memory>
#include "mysqlResult.h"

// 前向声明
class MySQLHandle;

class MySQLStatement {
public:
    ~MySQLStatement();
    
    // 绑定参数（链式调用）
    MySQLStatement& bindParam(int paramIndex, const std::string& value);
    MySQLStatement& bindParam(int paramIndex, int value);
    MySQLStatement& bindParam(int paramIndex, double value);
    MySQLStatement& bindParam(int paramIndex, long long value);
    
    // 为 json_sql.hpp 提供的便利方法
    bool setString(int paramIndex, const std::string& value);
    bool setInt(int paramIndex, int value);
    bool setDouble(int paramIndex, double value);
    bool setLongLong(int paramIndex, long long value);
    
    // 执行准备好的语句
    MySQLResultPtr execute();
    
    // 获取上一次错误信息
    std::string getLastError() const;

private:
    friend class MySQLHandle; // 允许MySQLHandle访问私有成员
    
    // 只能由MySQLHandle创建
    MySQLStatement(MYSQL* conn, MYSQL_STMT* stmt, int paramCount, const std::string& sql);
    
    // 禁止复制
    MySQLStatement(const MySQLStatement&) = delete;
    MySQLStatement& operator=(const MySQLStatement&) = delete;
    
    // 释放准备语句
    void freeStatement();

    MYSQL* conn;        // 数据库连接（不拥有）
    MYSQL_STMT* stmt;   // 准备语句
    MYSQL_BIND* params; // 参数绑定
    int paramCount;     // 参数数量
    std::vector<std::vector<char>> paramBuffers;
    std::vector<unsigned long> paramLengths;
    std::string sql;    // 存储当前SQL语句用于调试
};

// 共享指针类型定义，方便使用
using MySQLStatementPtr = std::shared_ptr<MySQLStatement>;

#endif // MYSQL_STATEMENT_H
