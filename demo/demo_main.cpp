#include "corutine.hpp"
#include "demo_db.h"
#include "demo_routes.h"
#include "http.h"
#include <file.h>
#include <iostream>
#include <ostream>
#include <ylt/struct_json/json_reader.h>
#include <ylt/struct_json/json_writer.h>
// 全局变量
HttpServer httpServer;
MySQLHandle mysqldb;

int main()
{
    LocalFiles static_files;
    // 移除对未知类型"control"的使用
    auto& coManagerInstance = Co_Start_Manager::getInstance();

    // 在程序启动后连接数据库
    if (!mysqldb.connect("localhost", "webserver", "WebServer@2025", "chat_db", 3306))
    {
        std::cerr << "数据库连接失败: " << mysqldb.getLastError() << std::endl;
        return 1;
    }
    else
    {
        std::cout << "数据库连接成功!" << std::endl;
    }

    // 初始化数据库
    if (!initializeDatabase(mysqldb))
    {
        std::cerr << "数据库初始化失败!" << std::endl;
        return 1;
    }

    // 添加默认用户
    addDefaultUser(mysqldb);

    // 注册所有路由
    registerAllRoutes(httpServer);

    // 启动服务器
    coManagerInstance.getManager().add(httpServer);
    // 移除对未知类
    coManagerInstance.setLoopInterval(std::chrono::nanoseconds(0));

    std::cout << "服务器已启动，监听端口: 8080" << std::endl;
    coManagerInstance.start();

    return 0;
}
