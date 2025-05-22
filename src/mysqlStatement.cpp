#include "mysqlStatement.h"
#include <iostream>
#include <cstring>

MySQLStatement::MySQLStatement(MYSQL* conn, MYSQL_STMT* stmt, int paramCount, const std::string& sql)
    : conn(conn), stmt(stmt), params(nullptr), paramCount(paramCount), sql(sql)
{
    if (paramCount > 0) {
        // 分配参数绑定数组
        params = new MYSQL_BIND[paramCount];
        memset(params, 0, sizeof(MYSQL_BIND) * paramCount);
        
        // 初始化缓冲区
        paramBuffers.resize(paramCount);
        paramLengths.resize(paramCount, 0);
    }
}

MySQLStatement::~MySQLStatement()
{
    freeStatement();
}

MySQLStatement& MySQLStatement::bindParam(int paramIndex, const std::string& value)
{
    if (!stmt || paramIndex < 0 || paramIndex >= paramCount) {
        std::cerr << "绑定参数失败: 无效的参数索引 " << paramIndex << std::endl;
        return *this;
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

    return *this;
}

MySQLStatement& MySQLStatement::bindParam(int paramIndex, int value)
{
    if (!stmt || paramIndex < 0 || paramIndex >= paramCount) {
        std::cerr << "绑定参数失败: 无效的参数索引 " << paramIndex << std::endl;
        return *this;
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

    return *this;
}

MySQLStatement& MySQLStatement::bindParam(int paramIndex, double value)
{
    if (!stmt || paramIndex < 0 || paramIndex >= paramCount) {
        std::cerr << "绑定参数失败: 无效的参数索引 " << paramIndex << std::endl;
        return *this;
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

    return *this;
}

MySQLStatement& MySQLStatement::bindParam(int paramIndex, long long value)
{
    if (!stmt || paramIndex < 0 || paramIndex >= paramCount) {
        std::cerr << "绑定参数失败: 无效的参数索引 " << paramIndex << std::endl;
        return *this;
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

    return *this;
}

MySQLResultPtr MySQLStatement::execute()
{
    if (!stmt) {
        std::cerr << "没有准备好的语句" << std::endl;
        return MySQLResultPtr(new MySQLResult());
    }

    // debug: 输出参数化后的语句
    {
        // 构建带参数的SQL语句用于调试
        std::string sqlWithParams = sql;
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
    }

    // 绑定参数
    if (paramCount > 0) {
        if (mysql_stmt_bind_param(stmt, params)) {
            std::cerr << "绑定参数失败: " << mysql_stmt_error(stmt) << std::endl;
            return MySQLResultPtr(new MySQLResult());
        }
    }

    // 执行准备好的语句
    if (mysql_stmt_execute(stmt)) {
        std::cerr << "执行准备语句失败: " << mysql_stmt_error(stmt) << std::endl;
        return MySQLResultPtr(new MySQLResult());
    }

    // 获取结果集
    if (mysql_stmt_store_result(stmt)) {
        std::cerr << "存储结果集失败: " << mysql_stmt_error(stmt) << std::endl;
        return MySQLResultPtr(new MySQLResult());
    }

    // 获取结果元数据（字段信息）
    MYSQL_RES* result = mysql_stmt_result_metadata(stmt);
    
    // 只有当查询实际返回字段时才需要元数据（例如SELECT语句）
    if (!result && mysql_stmt_field_count(stmt) > 0) {
        std::cerr << "获取结果元数据失败: " << mysql_stmt_error(stmt) << std::endl;
        return MySQLResultPtr(new MySQLResult());
    }
    
    // 在执行语句后，检查是否有结果行
    if (mysql_stmt_field_count(stmt) > 0) {
        std::cout << "语句执行后，有 " << mysql_stmt_num_rows(stmt) << " 行结果" << std::endl;
    }

    // 创建并返回结果对象 - 使用直接的new而不是make_shared
    return MySQLResultPtr(new MySQLResult(conn, result, stmt));
}

std::string MySQLStatement::getLastError() const
{
    return stmt ? mysql_stmt_error(stmt) : (conn ? mysql_error(conn) : "No connection");
}

void MySQLStatement::freeStatement()
{
    if (params) {
        delete[] params;
        params = nullptr;
    }

    // 注意：这里不关闭stmt，因为它是由MySQLHandle管理的
    
    paramCount = 0;
    paramBuffers.clear();
    paramLengths.clear();
    sql.clear();
}
