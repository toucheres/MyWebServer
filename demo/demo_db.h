#pragma once
#include <mysqlHandle.h>

// 初始化数据库表结构
bool initializeDatabase(MySQLHandle& db);

// 添加默认用户（如果没有用户）
bool addDefaultUser(MySQLHandle& db);
