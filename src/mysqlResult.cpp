#include "mysqlResult.h"
#include <iostream>
#include <cstring>

MySQLResult::MySQLResult() 
    : conn(nullptr), result(nullptr), stmt(nullptr), ownResult(false), resultBinds(nullptr)
{
}

MySQLResult::MySQLResult(MYSQL* conn, MYSQL_RES* result, MYSQL_STMT* stmt)
    : conn(conn), result(result), stmt(stmt), ownResult(true), resultBinds(nullptr)
{
    if (stmt && result) {
        bindResultBuffers();
    }
}

MySQLResult::~MySQLResult()
{
    freeResultBindings();
    
    if (ownResult && result) {
        mysql_free_result(result);
    }
}

int MySQLResult::getRowCount() const
{
    if (!result) {
        return 0;
    }
    return mysql_num_rows(result);
}

int MySQLResult::getFieldCount() const
{
    if (!result) {
        return 0;
    }
    return mysql_num_fields(result);
}

bool MySQLResult::empty() const
{
    return !result || mysql_num_rows(result) == 0;
}

std::string MySQLResult::getLastError() const
{
    return conn ? mysql_error(conn) : "No connection";
}

long long MySQLResult::getAffectedRows() const
{
    return conn ? mysql_affected_rows(conn) : 0;
}

void MySQLResult::bindResultBuffers()
{
    if (!result || !stmt || mysql_stmt_field_count(stmt) == 0) {
        return;
    }
    
    unsigned int fieldCount = mysql_num_fields(result);
    
    // 释放之前的绑定（如果有）
    freeResultBindings();
    
    // 分配新的绑定数组
    resultBinds = new MYSQL_BIND[fieldCount];
    memset(resultBinds, 0, sizeof(MYSQL_BIND) * fieldCount);
    
    // 为每个字段分配缓冲区
    resultBuffers.resize(fieldCount);
    resultLengths.resize(fieldCount, 0);
    resultNulls.resize(fieldCount, 0);
    
    // 获取字段信息
    MYSQL_FIELD* fields = mysql_fetch_fields(result);
    
    // 为每个字段设置绑定
    for (unsigned int i = 0; i < fieldCount; i++) {
        // 分配足够的缓冲区
        unsigned long bufferLength = 1024;  // 默认大小
        
        // 对于大型数据类型，增加缓冲区大小
        if (fields[i].type == MYSQL_TYPE_BLOB || 
            fields[i].type == MYSQL_TYPE_VAR_STRING || 
            fields[i].type == MYSQL_TYPE_STRING) {
            bufferLength = std::max(bufferLength, fields[i].length);
        }
        
        resultBuffers[i].resize(bufferLength);
        
        // 使用统一的字符串类型绑定
        resultBinds[i].buffer_type = MYSQL_TYPE_STRING;
        resultBinds[i].buffer = resultBuffers[i].data();
        resultBinds[i].buffer_length = bufferLength;
        resultBinds[i].length = &resultLengths[i];
        resultBinds[i].is_null = (bool*)&resultNulls[i];
    }
    
    // 绑定结果集
    if (mysql_stmt_bind_result(stmt, resultBinds)) {
        std::cerr << "绑定结果失败: " << mysql_stmt_error(stmt) << std::endl;
    }
}

void MySQLResult::freeResultBindings()
{
    if (resultBinds) {
        delete[] resultBinds;
        resultBinds = nullptr;
    }
    
    resultBuffers.clear();
    resultLengths.clear();
    resultNulls.clear();
}

std::vector<std::string> MySQLResult::fetchRow()
{
    std::vector<std::string> row;
    
    // 如果是普通查询的结果集
    if (result && !stmt) {
        MYSQL_ROW mysql_row = mysql_fetch_row(result);
        if (!mysql_row) {
            return row;
        }

        unsigned int fieldCount = mysql_num_fields(result);
        unsigned long* lengths = mysql_fetch_lengths(result);

        for (unsigned int i = 0; i < fieldCount; ++i) {
            if (mysql_row[i]) {
                row.push_back(std::string(mysql_row[i], lengths[i]));
            } else {
                row.push_back("NULL");
            }
        }
    }
    // 如果是准备语句的结果集
    else if (stmt && result) {
        // 尝试获取下一行
        int fetch_result = mysql_stmt_fetch(stmt);
        
        // 如果没有更多行或发生错误
        if (fetch_result == MYSQL_NO_DATA || fetch_result == 1) {
            if (fetch_result == 1) {
                std::cerr << "获取行失败: " << mysql_stmt_error(stmt) << std::endl;
            }
            return row;
        }
        
        // 获取字段数量
        unsigned int fieldCount = mysql_num_fields(result);
        
        // 对每个字段，转换数据到字符串
        for (unsigned int i = 0; i < fieldCount; ++i) {
            if (resultNulls[i]) {
                row.push_back("NULL");
            } else {
                // 使用resultLengths[i]作为实际数据长度
                if (resultLengths[i] > 0) {
                    row.push_back(std::string((char*)resultBuffers[i].data(), resultLengths[i]));
                } else {
                    row.push_back("");
                }
            }
        }
    }
    
    return row;
}
