#pragma once
#include "json_sql.hpp"
struct users
{
    int id;
    varchar<50> username;
    varchar<100> password;
    timestamp created_at;
};
// 定义表描述信息
template <>
describer table_discribe<users>::description = {
    .primerkey = "id",
    .field_constraints = {"AUTO_INCREMENT PRIMARY KEY", "NOT NULL UNIQUE", "NOT NULL",
                          "DEFAULT_GENERATED"},
    .skip_on_insert = {true, false, false, true}, // 跳过id和created_at
    .skip_on_update = {true, false, false, true}, // 跳过id
    .auto_timestamp = true};
