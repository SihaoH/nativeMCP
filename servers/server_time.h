#pragma once
#include "MCPServer.h"

class ServerTime : public MCPServer
{
    Q_OBJECT

public:
    ServerTime();
    ~ServerTime() = default;

public slots:
    QString getCurrentTime(const QVariant& timezone);
    MCPServer::ToolInfo getCurrentTime$info();

    QString waitTime(const QVariant& sec);
    MCPServer::ToolInfo waitTime$info();
};
