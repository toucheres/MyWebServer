#include <iostream>
#include <mysqlHandle.h>

MySQLHandle::MySQLHandle() : conn(nullptr), result(nullptr), connected(false)
{
    conn = mysql_init(nullptr);
    if (!conn)
    {
        std::cerr << "MySQL初始化失败" << std::endl;
    }
}

MySQLHandle::MySQLHandle(const std::string& host, const std::string& user,
                         const std::string& password, const std::string& dbname, unsigned int port)
{
    connect(host, user, password, dbname, port);
}

MySQLHandle::~MySQLHandle()
{
    disconnect();
}

bool MySQLHandle::connect(const std::string& host, const std::string& user,
                          const std::string& password, const std::string& dbname, unsigned int port)
{
    if (connected)
    {
        // disconnect();
        return false;
    }

    if (!mysql_real_connect(conn, host.c_str(), user.c_str(), password.c_str(), dbname.c_str(),
                            port, nullptr, 0))
    {
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
    if (result)
    {
        mysql_free_result(result);
        result = nullptr;
    }

    if (conn)
    {
        mysql_close(conn);
        conn = nullptr;
    }

    connected = false;
}

bool MySQLHandle::query(const std::string& sql)
{
    if (!connected)
    {
        std::cerr << "MySQL未连接" << std::endl;
        return false;
    }

    // 释放之前的结果集
    if (result)
    {
        mysql_free_result(result);
        result = nullptr;
    }

    if (mysql_query(conn, sql.c_str()))
    {
        std::cerr << "查询失败: " << mysql_error(conn) << std::endl;
        return false;
    }

    result = mysql_store_result(conn);
    return true;
}

int MySQLHandle::getRowCount()
{
    if (!result)
    {
        return 0;
    }
    return mysql_num_rows(result);
}

int MySQLHandle::getFieldCount()
{
    if (!result)
    {
        return 0;
    }
    return mysql_num_fields(result);
}

std::vector<std::string> MySQLHandle::fetchRow()
{
    std::vector<std::string> row;
    if (!result)
    {
        return row;
    }

    MYSQL_ROW mysql_row = mysql_fetch_row(result);
    // std::cout << *mysql_row<< '\n';
    if (!mysql_row)
    {
        return row;
    }

    unsigned int fieldCount = mysql_num_fields(result);
    unsigned long* lengths = mysql_fetch_lengths(result);

    for (unsigned int i = 0; i < fieldCount; ++i)
    {
        if (mysql_row[i])
        {
            row.push_back(std::string(mysql_row[i], lengths[i]));
        }
        else
        {
            row.push_back("NULL");
        }
    }

    return row;
}

bool MySQLHandle::startTransaction()
{
    return query("START TRANSACTION");
}

bool MySQLHandle::commit()
{
    return query("COMMIT");
}

bool MySQLHandle::rollback()
{
    return query("ROLLBACK");
}

std::string MySQLHandle::getLastError()
{
    return mysql_error(conn);
}
