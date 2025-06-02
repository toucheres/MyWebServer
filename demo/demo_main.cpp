#include "http.h"
#include "mysqlHandle.h"
#include "ylt/struct_json/json_reader.h"
#include "ylt/struct_json/json_writer.h"
#include "ylt/struct_pb.hpp"
#include "ylt/struct_xml/xml_reader.h"
#include "ylt/struct_xml/xml_writer.h"
#include "ylt/struct_yaml/yaml_reader.h"
#include "ylt/struct_yaml/yaml_writer.h"
HttpServer httpServer;
MySQLHandle mysqldb;
struct simple
{
    int color;
    int id;
    std::string str;
    int age;
};

int main()
{
    simple p{.color = 2, .id = 10, .str = "hello reflection", .age = 6};

    std::string json;
    struct_json::to_json(p, json);
    std::cout<<json<<std::endl;
    std::string xml;
    struct_xml::to_xml(p, xml);
    std::cout << xml << std::endl;

    std::string yaml;
    struct_yaml::to_yaml(p, yaml);
    std::cout << yaml << std::endl;

    std::string protobuf;
    struct_pb::to_pb(p, protobuf);
    std::cout << protobuf << std::endl;

    simple p1;
    simple p2;
    simple p3;
    simple p4;
    struct_json::from_json(p1, json);
    struct_xml::from_xml(p2, xml);
    struct_yaml::from_yaml(p3, yaml);
    struct_pb::from_pb(p4, protobuf);
}