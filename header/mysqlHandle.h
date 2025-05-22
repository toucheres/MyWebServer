#ifndef MYSQL_HANDLE_H
#define MYSQL_HANDLE_H

#include <iostream>
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <memory>

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

    // 执行SQL查询
    bool query(const std::string& sql);

    // 参数化查询相关方法
    bool prepareStatement(const std::string& sql);
    bool bindParam(int paramIndex, const std::string& value);
    bool bindParam(int paramIndex, int value);
    bool bindParam(int paramIndex, double value);
    bool bindParam(int paramIndex, long long value);
    bool executeStatement();
    void freeStatement();

    // 获取结果行数
    int getRowCount();

    // 获取字段数
    int getFieldCount();

    // 获取一行结果(以vector形式)
    std::vector<std::string> fetchRow();

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
    MYSQL_RES* result;
    bool connected;

    // 参数化查询相关成员
    MYSQL_STMT* stmt;
    MYSQL_BIND* params;
    int paramCount;
    std::vector<std::vector<char>> paramBuffers;
    std::vector<unsigned long> paramLengths;
    std::string currentSql; // 存储当前准备的SQL语句

    // 结果集绑定相关（新增）
    MYSQL_BIND* resultBinds = nullptr;
    std::vector<std::vector<char>> resultBuffers;
    std::vector<unsigned long> resultLengths;
    std::vector<char> resultNulls;  // 用char代替bool，以便可以安全地获取地址

    // 准备结果绑定（新增）
    void prepareResultBindings();
    // 释放结果绑定（新增）
    void freeResultBindings();
};

inline void mysqlExample()
{
    std::cout << "MySQL连接示例开始..." << std::endl;

    // 创建MySQL连接对象
    MySQLHandle mysql;

    // 连接到数据库
    if (mysql.connect("localhost", "webserver", "WebServer@2025", "chat_db", 3306))
    {
        std::cout << "MySQL连接成功!" << std::endl;
        // 1. 创建表示例
        std::cout << "\n--- 创建表示例 ---" << std::endl;
        if (mysql.query("CREATE TABLE IF NOT EXISTS demo_table ("
                        "id INT AUTO_INCREMENT PRIMARY KEY, "
                        "name VARCHAR(50) NOT NULL, "
                        "age INT, "
                        "creation_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"))
        {
            std::cout << "表创建成功或已存在" << std::endl;
        }
        else
        {
            std::cout << "表创建失败: " << mysql.getLastError() << std::endl;
        }

        // 2. 插入数据示例
        std::cout << "\n--- 插入数据示例 ---" << std::endl;
        if (mysql.query("INSERT INTO demo_table (name, age) VALUES ('张三', 25), ('李四', 30)"))
        {
            std::cout << "数据插入成功" << std::endl;
        }
        else
        {
            std::cout << "数据插入失败: " << mysql.getLastError() << std::endl;
        }

        // 3. 查询数据示例
        std::cout << "\n--- 查询数据示例 ---" << std::endl;
        if (mysql.query("SELECT * FROM demo_table"))
        {
            std::cout << "查询成功，找到 " << mysql.getRowCount() << " 条记录" << std::endl;
            std::cout << "查询结果：" << std::endl;

            std::vector<std::string> row;
            while (!(row = mysql.fetchRow()).empty())
            {
                std::cout << "id: " << row[0] << ", name: " << row[1] << ", age: " << row[2]
                          << ", 创建时间: " << row[3] << std::endl;
            }
        }
        else
        {
            std::cout << "查询失败: " << mysql.getLastError() << std::endl;
        }

        // 4. 更新数据示例
        std::cout << "\n--- 更新数据示例 ---" << std::endl;
        if (mysql.query("UPDATE demo_table SET age = 26 WHERE name = '张三'"))
        {
            std::cout << "数据更新成功，影响行数：" << mysql_affected_rows(mysql.getConnection())
                      << std::endl;
        }
        else
        {
            std::cout << "数据更新失败: " << mysql.getLastError() << std::endl;
        }

        // 显示更新后的数据
        if (mysql.query("SELECT * FROM demo_table WHERE name = '张三'"))
        {
            std::cout << "更新后查询结果：" << std::endl;
            std::vector<std::string> row = mysql.fetchRow();
            if (!row.empty())
            {
                std::cout << "id: " << row[0] << ", name: " << row[1] << ", age: " << row[2]
                          << ", 创建时间: " << row[3] << std::endl;
            }
        }

        // 5. 事务示例
        std::cout << "\n--- 事务处理示例 ---" << std::endl;
        if (mysql.startTransaction())
        {
            bool success = true;

            // 在事务中执行多个操作
            if (!mysql.query("INSERT INTO demo_table (name, age) VALUES ('王五', 35)"))
            {
                std::cout << "事务中插入失败: " << mysql.getLastError() << std::endl;
                success = false;
            }

            if (success && !mysql.query("UPDATE demo_table SET age = 31 WHERE name = '李四'"))
            {
                std::cout << "事务中更新失败: " << mysql.getLastError() << std::endl;
                success = false;
            }

            // 根据操作结果提交或回滚事务
            if (success)
            {
                mysql.commit();
                std::cout << "事务提交成功" << std::endl;
            }
            else
            {
                mysql.rollback();
                std::cout << "事务已回滚" << std::endl;
            }
        }

        // 6. 条件查询示例
        std::cout << "\n--- 条件查询示例 ---" << std::endl;
        if (mysql.query("SELECT * FROM demo_table WHERE age > 30"))
        {
            std::cout << "条件查询成功，找到 " << mysql.getRowCount() << " 条记录" << std::endl;

            std::vector<std::string> row;
            while (!(row = mysql.fetchRow()).empty())
            {
                std::cout << "id: " << row[0] << ", name: " << row[1] << ", age: " << row[2]
                          << ", 创建时间: " << row[3] << std::endl;
            }
        }

        // 7. 删除数据示例
        std::cout << "\n--- 删除数据示例 ---" << std::endl;
        if (mysql.query("DELETE FROM demo_table WHERE name = '王五'"))
        {
            std::cout << "数据删除成功，影响行数：" << mysql_affected_rows(mysql.getConnection())
                      << std::endl;
        }
        else
        {
            std::cout << "数据删除失败: " << mysql.getLastError() << std::endl;
        }

        // 8. 删除表示例
        std::cout << "\n--- 删除表示例 ---" << std::endl;
        if (mysql.query("DROP TABLE IF EXISTS demo_table"))
        {
            std::cout << "表删除成功" << std::endl;
        }
        else
        {
            std::cout << "表删除失败: " << mysql.getLastError() << std::endl;
        }

        // 查看现有表
        std::cout << "\n--- 查看数据库表 ---" << std::endl;
        if (mysql.query("SHOW TABLES"))
        {
            std::cout << "数据库中的表：" << std::endl;
            std::vector<std::string> row;
            while (!(row = mysql.fetchRow()).empty())
            {
                std::cout << "- " << row[0] << std::endl;
            }
        }

        // 断开连接
        mysql.disconnect();
        std::cout << "\nMySQL连接已断开" << std::endl;
    }
    else
    {
        std::cout << "MySQL连接失败: " << mysql.getLastError() << std::endl;
    }

    std::cout << "MySQL连接示例结束" << std::endl;
}
#endif // MYSQL_HANDLE_H
