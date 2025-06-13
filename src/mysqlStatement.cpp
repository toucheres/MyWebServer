#include "mysqlStatement.h"
#include "mysqlHandle.h"
#include <cstring>
#include <iostream>

MySQLStatement::MySQLStatement(MYSQL* conn, MYSQL_STMT* stmt, int paramCount, const std::string& sql)
    : conn(conn), stmt(stmt), paramCount(paramCount), params(nullptr), sql(sql)
{
    if (paramCount > 0) {
        // 分配参数绑定数组
        params = new MYSQL_BIND[paramCount];
        memset(params, 0, sizeof(MYSQL_BIND) * paramCount);
        
        // 初始化参数缓冲区
        paramBuffers.resize(paramCount);
        paramLengths.resize(paramCount, 0);
    }
}

MySQLStatement::~MySQLStatement()
{
    freeStatement();
}

void MySQLStatement::freeStatement()
{
    if (params) {
        delete[] params;
        params = nullptr;
    }
    
    if (stmt) {
        mysql_stmt_close(stmt);
        stmt = nullptr;
    }
    
    paramBuffers.clear();
    paramLengths.clear();
}

MySQLStatement& MySQLStatement::bindParam(int paramIndex, const std::string& value)
{
    if (paramIndex < 0 || paramIndex >= paramCount || !params) {
        std::cerr << "Invalid parameter index: " << paramIndex << std::endl;
        return *this;
    }
    
    // 调整缓冲区大小
    paramBuffers[paramIndex].resize(value.size() + 1);
    std::strcpy(paramBuffers[paramIndex].data(), value.c_str());
    paramLengths[paramIndex] = value.size();
    
    // 设置绑定参数
    params[paramIndex].buffer_type = MYSQL_TYPE_STRING;
    params[paramIndex].buffer = paramBuffers[paramIndex].data();
    params[paramIndex].buffer_length = paramBuffers[paramIndex].size();
    params[paramIndex].length = &paramLengths[paramIndex];
    params[paramIndex].is_null = nullptr;
    
    return *this;
}

MySQLStatement& MySQLStatement::bindParam(int paramIndex, int value)
{
    if (paramIndex < 0 || paramIndex >= paramCount || !params) {
        std::cerr << "Invalid parameter index: " << paramIndex << std::endl;
        return *this;
    }
    
    // 为整数分配缓冲区
    paramBuffers[paramIndex].resize(sizeof(int));
    *reinterpret_cast<int*>(paramBuffers[paramIndex].data()) = value;
    paramLengths[paramIndex] = sizeof(int);
    
    // 设置绑定参数
    params[paramIndex].buffer_type = MYSQL_TYPE_LONG;
    params[paramIndex].buffer = paramBuffers[paramIndex].data();
    params[paramIndex].buffer_length = sizeof(int);
    params[paramIndex].length = &paramLengths[paramIndex];
    params[paramIndex].is_null = nullptr;
    
    return *this;
}

MySQLStatement& MySQLStatement::bindParam(int paramIndex, double value)
{
    if (paramIndex < 0 || paramIndex >= paramCount || !params) {
        std::cerr << "Invalid parameter index: " << paramIndex << std::endl;
        return *this;
    }
    
    // 为双精度浮点数分配缓冲区
    paramBuffers[paramIndex].resize(sizeof(double));
    *reinterpret_cast<double*>(paramBuffers[paramIndex].data()) = value;
    paramLengths[paramIndex] = sizeof(double);
    
    // 设置绑定参数
    params[paramIndex].buffer_type = MYSQL_TYPE_DOUBLE;
    params[paramIndex].buffer = paramBuffers[paramIndex].data();
    params[paramIndex].buffer_length = sizeof(double);
    params[paramIndex].length = &paramLengths[paramIndex];
    params[paramIndex].is_null = nullptr;
    
    return *this;
}

MySQLStatement& MySQLStatement::bindParam(int paramIndex, long long value)
{
    if (paramIndex < 0 || paramIndex >= paramCount || !params) {
        std::cerr << "Invalid parameter index: " << paramIndex << std::endl;
        return *this;
    }
    
    // 为长整数分配缓冲区
    paramBuffers[paramIndex].resize(sizeof(long long));
    *reinterpret_cast<long long*>(paramBuffers[paramIndex].data()) = value;
    paramLengths[paramIndex] = sizeof(long long);
    
    // 设置绑定参数
    params[paramIndex].buffer_type = MYSQL_TYPE_LONGLONG;
    params[paramIndex].buffer = paramBuffers[paramIndex].data();
    params[paramIndex].buffer_length = sizeof(long long);
    params[paramIndex].length = &paramLengths[paramIndex];
    params[paramIndex].is_null = nullptr;
    
    return *this;
}

bool MySQLStatement::setString(int paramIndex, const std::string& value)
{
    bindParam(paramIndex, value);
    return true;
}

bool MySQLStatement::setInt(int paramIndex, int value)
{
    bindParam(paramIndex, value);
    return true;
}

bool MySQLStatement::setDouble(int paramIndex, double value)
{
    bindParam(paramIndex, value);
    return true;
}

bool MySQLStatement::setLongLong(int paramIndex, long long value)
{
    bindParam(paramIndex, value);
    return true;
}

MySQLResultPtr MySQLStatement::execute()
{
    if (!stmt) {
        std::cerr << "Statement is null" << std::endl;
        return MySQLResultPtr(new MySQLResult());
    }
    
    // 绑定参数（如果有参数）
    if (paramCount > 0 && params) {
        if (mysql_stmt_bind_param(stmt, params)) {
            std::cerr << "Failed to bind parameters: " << mysql_stmt_error(stmt) << std::endl;
            return MySQLResultPtr(new MySQLResult());
        }
    }
    
    // 执行语句
    if (mysql_stmt_execute(stmt)) {
        std::cerr << "Failed to execute statement: " << mysql_stmt_error(stmt) << std::endl;
        return MySQLResultPtr(new MySQLResult());
    }
    
    // 获取结果元数据
    MYSQL_RES* result = mysql_stmt_result_metadata(stmt);
    
    // 创建并返回结果对象
    return MySQLResultPtr(new MySQLResult(conn, result, stmt));
}

std::string MySQLStatement::getLastError() const
{
    return stmt ? mysql_stmt_error(stmt) : "No statement";
}
