#pragma once
#include "MCPServer.h"

class ServerTest : public MCPServer
{
    Q_OBJECT

public:
    ServerTest();
    ~ServerTest() = default;

public slots:
    QString getAvailableIP();
    MCPServer::ToolInfo getAvailableIP$info();

    QString sendToIP(const QVariant& ip, const QVariant& content);
    MCPServer::ToolInfo sendToIP$info();

    QString testMultiParams(const QVariant& c, const QVariant& b, const QVariant& a);
    MCPServer::ToolInfo testMultiParams$info();
};
