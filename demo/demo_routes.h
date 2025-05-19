#pragma once
#include "format.h"
#include "http.h"
#include "serverFile.h"
#include "demo_message.h"
#include <map>

// 存储会话ID和用户信息的映射
extern std::map<std::string, std::string> token_name;

// 注册所有路由
void registerAllRoutes(HttpServer& httpServer);

// 各种路由处理函数
void handleRoot(serverFile& socketfile);
void handleGetHistory(serverFile& socketfile);
void handleWebSocketConnect(serverFile& socketfile);
void handleCheckSession(serverFile& socketfile);
void handleLogin(serverFile& socketfile);
void handleRegister(serverFile& socketfile);
