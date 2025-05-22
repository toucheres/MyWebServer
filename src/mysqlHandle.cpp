#include <iostream>
#include <mysqlHandle.h>
#include <cstring>

MySQLHandle::MySQLHandle() : conn(nullptr), connected(false)
{
    conn = mysql_init(nullptr);
    if (!conn) {
        std::cerr << "MySQL初始化失败" << std::endl;
    }
}

MySQLHandle::MySQLHandle(const std::string& host, const std::string& user,
                         const std::string& password, const std::string& dbname, unsigned int port)
{
    conn = mysql_init(nullptr);
    if (!conn) {
        std::cerr << "MySQL初始化失败" << std::endl;
        connected = false;
    } else {
        connect(host, user, password, dbname, port);
    }
}

MySQLHandle::~MySQLHandle()
{
    disconnect();
}

bool MySQLHandle::connect(const std::string& host, const std::string& user,
                          const std::string& password, const std::string& dbname, unsigned int port)
{
    if (connected) {
        return false;
    }

    // 如果conn为NULL（之前已断开连接），需要重新初始化
    if (!conn) {
        conn = mysql_init(nullptr);
        if (!conn) {
            std::cerr << "MySQL初始化失败" << std::endl;
            return false;
        }
    }

    if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), dbname.c_str(),
                            port, nullptr, 0)) {
        std::cerr << "连接MySQL失败: " << mysql_error(conn) << std::endl;
        return false;
    }

    // 设置字符集为utf8
    mysql_set_character_set(conn, "utf8");
    connected = true;
    return true;
}

MYSQL* MySQLHandle::getConnection()
{
    return conn;
}

void MySQLHandle::disconnect()
{
    if (conn) {
        mysql_close(conn);
        conn = nullptr;
    }
    connected = false;
}

MySQLResultPtr MySQLHandle::query(const std::string& sql)
{
    if (!connected) {
        std::cerr << "MySQL未连接" << std::endl;
        return MySQLResultPtr(new MySQLResult());
    }

    if (mysql_query(conn, sql.c_str())) {
        std::cerr << "查询失败: " << mysql_error(conn) << std::endl;
        return MySQLResultPtr(new MySQLResult());
    }

    MYSQL_RES* result = mysql_store_result(conn);
    return MySQLResultPtr(new MySQLResult(conn, result, nullptr));
}

MySQLStatementPtr MySQLHandle::prepareStatement(const std::string& sql)
{
    if (!connected) {
        std::cerr << "MySQL未连接" << std::endl;
        return nullptr;
    }

    // 创建准备语句
    MYSQL_STMT* stmt = mysql_stmt_init(conn);
    if (!stmt) {
        std::cerr << "初始化准备语句失败: " << mysql_error(conn) << std::endl;
        return nullptr;
    }

    // 准备SQL语句
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
        std::cerr << "准备语句失败: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return nullptr;
    }

    // 获取参数数量
    int paramCount = mysql_stmt_param_count(stmt);
    
    // 创建并返回语句对象 - 使用new替代make_shared
    return MySQLStatementPtr(new MySQLStatement(conn, stmt, paramCount, sql));
}

bool MySQLHandle::startTransaction()
{
    if (!connected) {
        std::cerr << "MySQL未连接" << std::endl;
        return false;
    }
    
    return mysql_query(conn, "START TRANSACTION") == 0;
}

bool MySQLHandle::commit()
{
    if (!connected) {
        std::cerr << "MySQL未连接" << std::endl;
        return false;
    }
    
    return mysql_query(conn, "COMMIT") == 0;
}

bool MySQLHandle::rollback()
{
    if (!connected) {
        std::cerr << "MySQL未连接" << std::endl;
        return false;
    }
    
    return mysql_query(conn, "ROLLBACK") == 0;
}

std::string MySQLHandle::getLastError()
{
    return conn ? mysql_error(conn) : "No connection";
}
