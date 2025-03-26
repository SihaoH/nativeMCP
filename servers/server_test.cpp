#include "server_test.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QThread>


ServerTest::ServerTest()
    : MCPServer()
{
    setName("mcp-cpp-test");
    setVersion("0.1.0");
}

QString ServerTest::getAvailableIP()
{
    QJsonObject ret_obj{
        {"ip_list", QJsonArray{"192.168.1.201", "192.168.1.202", "192.168.1.203"}},
    };

    return QJsonDocument(ret_obj).toJson(QJsonDocument::Indented);
}

MCPServer::ToolInfo ServerTest::getAvailableIP$info()
{
    MCPServer::ToolInfo info;
    info.name = "getAvailableIP";
    info.desc = "获取可用的IP列表";
    return info;
}

QString ServerTest::sendToIP(const QVariant& ip, const QVariant& content)
{
    QJsonObject ret_obj{
        {"ip", ip.toString()},
        {"content", content.toString()},
        {"status", "发送成功"},
    };
    return QJsonDocument(ret_obj).toJson(QJsonDocument::Indented);
}

MCPServer::ToolInfo ServerTest::sendToIP$info()
{
    MCPServer::ToolInfo info;
    info.name = "sendToIP";
    info.desc = "将内容发送到指定的ip地址";
    info.params = QList<MCPServer::ParamInfo>{
        {"ip", "string", "ip地址"},
        {"content", "string", "要发送的内容"}
    };
    return info;
}

QString ServerTest::testMultiParams(const QVariant& c, const QVariant& b, const QVariant& a)
{
    QJsonObject ret_obj{
        {"result", "调用完成"},
        {"c", c.toString()},
        {"b", b.toString()},
        {"a", a.toString()}
    };
    return QJsonDocument(ret_obj).toJson(QJsonDocument::Indented);
}

MCPServer::ToolInfo ServerTest::testMultiParams$info()
{
    MCPServer::ToolInfo info;
    info.name = "testMultiParams";
    info.desc = "测试多参数的工具调用";
    info.params = QList<MCPServer::ParamInfo>{
        {"c", "string", "参数c，可以填入任意内容，测试成功会打印出来"},
        {"b", "string", "参数b，可以填入任意内容，测试成功会打印出来"},
        {"a", "string", "参数a，可以填入任意内容，测试成功会打印出来"}
    };
    return info;
}


MCP_SERVER_MAIN(ServerTest)
