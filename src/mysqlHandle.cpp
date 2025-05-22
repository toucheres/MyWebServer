#include <iostream>
#include <mysqlHandle.h>
#include <cstring>

MySQLHandle::MySQLHandle() : conn(nullptr), result(nullptr), connected(false), stmt(nullptr), params(nullptr), paramCount(0)
{
    conn = mysql_init(nullptr);
    if (!conn)
    {
        std::cerr << "MySQL初始化失败" << std::endl;
    }
}

MySQLHandle::MySQLHandle(const std::string& host, const std::string& user,
                         const std::string& password, const std::string& dbname, unsigned int port)
    : stmt(nullptr), params(nullptr), paramCount(0)
{
    connect(host, user, password, dbname, port);
}

MySQLHandle::~MySQLHandle()
{
    freeStatement();
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

    // 关键修复：如果conn为NULL（之前已断开连接），需要重新初始化
    if (!conn)
    {
        conn = mysql_init(nullptr);
        if (!conn)
        {
            std::cerr << "MySQL初始化失败" << std::endl;
            return false;
        }
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
        conn = nullptr;  // 已经正确设置为nullptr
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

bool MySQLHandle::prepareStatement(const std::string& sql)
{
    if (!connected)
    {
        std::cerr << "MySQL未连接" << std::endl;
        return false;
    }

    // 释放之前的准备语句（如果有）
    freeStatement();
    
    // 存储当前SQL语句用于调试
    currentSql = sql;

    // 创建准备语句
    stmt = mysql_stmt_init(conn);
    if (!stmt)
    {
        std::cerr << "初始化准备语句失败: " << mysql_error(conn) << std::endl;
        return false;
    }

    // 准备SQL语句
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length()))
    {
        std::cerr << "准备语句失败: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        stmt = nullptr;
        return false;
    }

    // 获取参数数量
    paramCount = mysql_stmt_param_count(stmt);
    if (paramCount > 0)
    {
        // 分配参数绑定数组
        params = new MYSQL_BIND[paramCount];
        memset(params, 0, sizeof(MYSQL_BIND) * paramCount);
        
        // 初始化缓冲区
        paramBuffers.resize(paramCount);
        paramLengths.resize(paramCount, 0);
    }

    return true;
}

bool MySQLHandle::bindParam(int paramIndex, const std::string& value)
{
    if (!stmt || paramIndex < 0 || paramIndex >= paramCount)
    {
        return false;
    }

    // 存储字符串值到缓冲区
    paramBuffers[paramIndex].resize(value.length());
    memcpy(paramBuffers[paramIndex].data(), value.c_str(), value.length());
    paramLengths[paramIndex] = value.length();

    // 设置绑定
    params[paramIndex].buffer_type = MYSQL_TYPE_STRING;
    params[paramIndex].buffer = paramBuffers[paramIndex].data();
    params[paramIndex].buffer_length = value.length();
    params[paramIndex].length = &paramLengths[paramIndex];
    params[paramIndex].is_null = 0;

    return true;
}

bool MySQLHandle::bindParam(int paramIndex, int value)
{
    if (!stmt || paramIndex < 0 || paramIndex >= paramCount)
    {
        return false;
    }

    // 存储整数值到缓冲区
    paramBuffers[paramIndex].resize(sizeof(int));
    *(int*)paramBuffers[paramIndex].data() = value;
    paramLengths[paramIndex] = sizeof(int);

    // 设置绑定
    params[paramIndex].buffer_type = MYSQL_TYPE_LONG;
    params[paramIndex].buffer = paramBuffers[paramIndex].data();
    params[paramIndex].buffer_length = sizeof(int);
    params[paramIndex].length = &paramLengths[paramIndex];
    params[paramIndex].is_null = 0;

    return true;
}

bool MySQLHandle::bindParam(int paramIndex, double value)
{
    if (!stmt || paramIndex < 0 || paramIndex >= paramCount)
    {
        return false;
    }

    // 存储浮点值到缓冲区
    paramBuffers[paramIndex].resize(sizeof(double));
    *(double*)paramBuffers[paramIndex].data() = value;
    paramLengths[paramIndex] = sizeof(double);

    // 设置绑定
    params[paramIndex].buffer_type = MYSQL_TYPE_DOUBLE;
    params[paramIndex].buffer = paramBuffers[paramIndex].data();
    params[paramIndex].buffer_length = sizeof(double);
    params[paramIndex].length = &paramLengths[paramIndex];
    params[paramIndex].is_null = 0;

    return true;
}

bool MySQLHandle::bindParam(int paramIndex, long long value)
{
    if (!stmt || paramIndex < 0 || paramIndex >= paramCount)
    {
        return false;
    }

    // 存储长整数值到缓冲区
    paramBuffers[paramIndex].resize(sizeof(long long));
    *(long long*)paramBuffers[paramIndex].data() = value;
    paramLengths[paramIndex] = sizeof(long long);

    // 设置绑定
    params[paramIndex].buffer_type = MYSQL_TYPE_LONGLONG;
    params[paramIndex].buffer = paramBuffers[paramIndex].data();
    params[paramIndex].buffer_length = sizeof(long long);
    params[paramIndex].length = &paramLengths[paramIndex];
    params[paramIndex].is_null = 0;

    return true;
}

bool MySQLHandle::executeStatement()
{
    if (!stmt)
    {
        std::cerr << "没有准备好的语句" << std::endl;
        return false;
    }

    // 释放之前的结果集
    if (result)
    {
        mysql_free_result(result);
        result = nullptr;
    }

    // 绑定参数
    if (paramCount > 0)
    {
        if (mysql_stmt_bind_param(stmt, params))
        {
            std::cerr << "绑定参数失败: " << mysql_stmt_error(stmt) << std::endl;
            return false;
        }
    }

    // debug: 输出参数化后的语句
    {
        // 构建带参数的SQL语句
        std::string sqlWithParams = currentSql;
        std::vector<std::string> paramValues;
        
        // 收集所有参数值
        for (int i = 0; i < paramCount; ++i) {
            std::string paramValue;
            switch (params[i].buffer_type) {
                case MYSQL_TYPE_STRING: {
                    std::string strValue((char*)params[i].buffer, *params[i].length);
                    // 转义字符串中的单引号
                    size_t pos = 0;
                    while ((pos = strValue.find('\'', pos)) != std::string::npos) {
                        strValue.insert(pos, "\\");
                        pos += 2;
                    }
                    paramValue = "'" + strValue + "'";
                    break;
                }
                case MYSQL_TYPE_LONG:
                    paramValue = std::to_string(*(int*)params[i].buffer);
                    break;
                case MYSQL_TYPE_DOUBLE:
                    paramValue = std::to_string(*(double*)params[i].buffer);
                    break;
                case MYSQL_TYPE_LONGLONG:
                    paramValue = std::to_string(*(long long*)params[i].buffer);
                    break;
                default:
                    paramValue = "[未知类型]";
            }
            paramValues.push_back(paramValue);
        }
        
        // 替换SQL语句中的?占位符为实际参数值
        size_t questionPos = 0;
        for (int i = 0; i < paramCount && (questionPos = sqlWithParams.find('?', questionPos)) != std::string::npos; ++i) {
            sqlWithParams.replace(questionPos, 1, paramValues[i]);
            questionPos += paramValues[i].length();
        }
        
        std::cerr << "执行参数化查询: " << sqlWithParams << std::endl;
        std::cerr << "参数值: ";
        for (int i = 0; i < paramCount; ++i) {
            std::cerr << "参数[" << i << "]=";
            switch (params[i].buffer_type) {
                case MYSQL_TYPE_STRING:
                    std::cerr << "'" << std::string((char*)params[i].buffer, *params[i].length) << "'";
                    break;
                case MYSQL_TYPE_LONG:
                    std::cerr << *(int*)params[i].buffer;
                    break;
                case MYSQL_TYPE_DOUBLE:
                    std::cerr << *(double*)params[i].buffer;
                    break;
                case MYSQL_TYPE_LONGLONG:
                    std::cerr << *(long long*)params[i].buffer;
                    break;
                default:
                    std::cerr << "[未知类型]";
            }
            if (i < paramCount - 1) std::cerr << ", ";
        }
        std::cerr << std::endl;
    }

    // 执行准备好的语句
    if (mysql_stmt_execute(stmt))
    {
        std::cerr << "执行准备语句失败: " << mysql_stmt_error(stmt) << std::endl;
        return false;
    }

    // 获取结果集
    if (mysql_stmt_store_result(stmt))
    {
        std::cerr << "存储结果集失败: " << mysql_stmt_error(stmt) << std::endl;
        return false;
    }

    // 获取结果元数据（字段信息）
    result = mysql_stmt_result_metadata(stmt);
    
    // 只有当查询实际返回字段时才需要元数据（例如SELECT语句）
    if (!result && mysql_stmt_field_count(stmt) > 0)
    {
        std::cerr << "获取结果元数据失败: " << mysql_stmt_error(stmt) << std::endl;
        return false;
    }
    
    // 在执行语句后，检查是否有结果行
    if (mysql_stmt_field_count(stmt) > 0)
    {
        std::cout << "语句执行后，有 " << mysql_stmt_num_rows(stmt) << " 行结果" << std::endl;
        
        // 准备获取结果行的存储
        prepareResultBindings();
    }

    return true;
}

void MySQLHandle::prepareResultBindings()
{
    // 只有当查询返回结果时才需要准备结果绑定
    if (!result || mysql_stmt_field_count(stmt) == 0)
        return;
    
    // 获取字段数量
    unsigned int fieldCount = mysql_num_fields(result);
    
    // 释放之前的结果绑定（如果有）
    freeResultBindings();
    
    // 分配结果绑定数组
    resultBinds = new MYSQL_BIND[fieldCount];
    memset(resultBinds, 0, sizeof(MYSQL_BIND) * fieldCount);
    
    // 为每个字段分配缓冲区
    resultBuffers.resize(fieldCount);
    resultLengths.resize(fieldCount, 0);
    resultNulls.resize(fieldCount, 0);
    
    // 获取字段信息
    MYSQL_FIELD* fields = mysql_fetch_fields(result);
    
    // 为每个字段设置绑定
    for (unsigned int i = 0; i < fieldCount; i++)
    {
        // 分配足够的缓冲区
        unsigned long bufferLength = 1024;  // 默认大小，可以根据需要调整
        
        // 如果是比较大的字段类型，增加缓冲区大小
        if (fields[i].type == MYSQL_TYPE_BLOB || 
            fields[i].type == MYSQL_TYPE_VAR_STRING || 
            fields[i].type == MYSQL_TYPE_STRING)
        {
            bufferLength = std::max(bufferLength, fields[i].length);
        }
        
        // 对于所有字段类型，总是使用字符串类型来接收
        // 这可以避免类型转换问题，让MySQL处理转换
        resultBuffers[i].resize(bufferLength);
        
        // 设置绑定为字符串类型，无论原始字段类型是什么
        resultBinds[i].buffer_type = MYSQL_TYPE_STRING;
        resultBinds[i].buffer = resultBuffers[i].data();
        resultBinds[i].buffer_length = bufferLength;
        resultBinds[i].length = &resultLengths[i];
        resultBinds[i].is_null = (bool*)&resultNulls[i];
    }
    
    // 绑定结果集
    if (mysql_stmt_bind_result(stmt, resultBinds))
    {
        std::cerr << "绑定结果失败: " << mysql_stmt_error(stmt) << std::endl;
    }
}

void MySQLHandle::freeResultBindings()
{
    if (resultBinds)
    {
        delete[] resultBinds;
        resultBinds = nullptr;
    }
    
    resultBuffers.clear();
    resultLengths.clear();
    resultNulls.clear();
}

std::vector<std::string> MySQLHandle::fetchRow()
{
    std::vector<std::string> row;
    
    // 如果是普通查询的结果集
    if (result && !stmt)
    {
        MYSQL_ROW mysql_row = mysql_fetch_row(result);
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
    // 如果是准备语句的结果集
    else if (stmt && result)
    {
        // 尝试获取下一行
        int fetch_result = mysql_stmt_fetch(stmt);
        
        // 如果没有更多行或发生错误
        if (fetch_result == MYSQL_NO_DATA || fetch_result == 1)
        {
            if (fetch_result == 1)
            {
                std::cerr << "获取行失败: " << mysql_stmt_error(stmt) << std::endl;
            }
            return row;
        }
        
        // 获取字段数量
        unsigned int fieldCount = mysql_num_fields(result);
        
        // 对每个字段，转换数据到字符串
        for (unsigned int i = 0; i < fieldCount; ++i)
        {
            if (resultNulls[i])
            {
                row.push_back("NULL");
            }
            else
            {
                // 使用resultLengths[i]作为实际数据长度
                if (resultLengths[i] > 0) {
                    row.push_back(std::string((char*)resultBuffers[i].data(), resultLengths[i]));
                } else {
                    row.push_back("");
                }
            }
        }
        
        return row;
    }
    
    return row;
}

void MySQLHandle::freeStatement()
{
    // 释放结果绑定
    freeResultBindings();
    
    if (params)
    {
        delete[] params;
        params = nullptr;
    }

    if (stmt)
    {
        mysql_stmt_close(stmt);
        stmt = nullptr;
    }

    paramCount = 0;
    paramBuffers.clear();
    paramLengths.clear();

    // 清除当前SQL
    currentSql.clear();
}
