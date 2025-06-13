#ifndef MYSQL_HANDLE_H
#define MYSQL_HANDLE_H

#include <mysql/mysql.h>
#include <string>
#include "mysqlResult.h"
#include "mysqlStatement.h"

class MySQLHandle {
public:
    MySQLHandle();
    MySQLHandle(const std::string& host, const std::string& user, const std::string& password,
                const std::string& dbname, unsigned int port = 3306);
    ~MySQLHandle();

    // 连接数据库
    bool connect(const std::string& host, const std::string& user, 
                const std::string& password, const std::string& dbname, 
                unsigned int port = 3306);
    MYSQL* getConnection();
    // 断开连接
    void disconnect();

    // 执行SQL查询，返回结果对象
    MySQLResultPtr query(const std::string& sql);

    // 准备参数化查询，返回语句对象
    MySQLStatementPtr prepareStatement(const std::string& sql);

    // 开始事务
    bool startTransaction();

    // 提交事务
    bool commit();

    // 回滚事务
    bool rollback();

    // 获取上一次错误信息
    std::string getLastError();

private:
    MYSQL* conn;
    bool connected;

    // 禁止复制
    MySQLHandle(const MySQLHandle&) = delete;
    MySQLHandle& operator=(const MySQLHandle&) = delete;
};

#endif // MYSQL_HANDLE_H
