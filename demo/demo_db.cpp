#include "demo_db.h"
#include <iostream>

bool initializeDatabase(MySQLHandle& db)
{
    bool success = true;
    
    // 确保数据库中有users表，如果没有则创建
    if (!db.query("CREATE TABLE IF NOT EXISTS users ("
                  "id INT AUTO_INCREMENT PRIMARY KEY, "
                  "username VARCHAR(50) NOT NULL UNIQUE, "
                  "password VARCHAR(100) NOT NULL, "
                  "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"))
    {
        std::cerr << "创建users表失败: " << db.getLastError() << '\n';
        success = false;
    }

    // 确保数据库中有messages表，如果没有则创建
    if (!db.query("CREATE TABLE IF NOT EXISTS messages ("
                  "id INT AUTO_INCREMENT PRIMARY KEY, "
                  "sender VARCHAR(50) NOT NULL, "
                  "content TEXT NOT NULL, "
                  "timestamp BIGINT NOT NULL, "
                  "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)"))
    {
        std::cerr << "创建messages表失败: " << db.getLastError() << '\n';
        success = false;
    }
    else if (success)
    {
        std::cout << "数据库和消息表初始化成功\n";
    }
    
    return success;
}

bool addDefaultUser(MySQLHandle& db)
{
    // 检查是否有默认用户，如果没有则添加一个
    if (db.prepareStatement("SELECT COUNT(*) FROM users"))
    {
        if (!db.executeStatement())
        {
            std::cerr << "查询用户数量失败: " << db.getLastError() << '\n';
            db.freeStatement();
            return false;
        }
        
        auto row = db.fetchRow();
        if (!row.empty() && std::stoi(row[0]) == 0)
        {
            db.freeStatement();
            
            // 添加默认管理员用户 - 使用参数化查询
            if (db.prepareStatement("INSERT INTO users (username, password) VALUES (?, ?)"))
            {
                db.bindParam(0, std::string("admin"));
                db.bindParam(1, std::string("admin123"));
                
                if (!db.executeStatement())
                {
                    std::cerr << "添加默认用户失败: " << db.getLastError() << '\n';
                    db.freeStatement();
                    return false;
                }
                else
                {
                    std::cout << "已添加默认用户：admin/admin123\n";
                    db.freeStatement();
                    return true;
                }
            }
            else
            {
                std::cerr << "准备添加默认用户语句失败: " << db.getLastError() << '\n';
                return false;
            }
        }
        
        db.freeStatement();
    }
    else
    {
        std::cerr << "准备查询用户数量语句失败: " << db.getLastError() << '\n';
        return false;
    }
    
    return true; // 已有用户，不需要添加
}
