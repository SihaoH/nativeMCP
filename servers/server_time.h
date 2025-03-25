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
    MCPServer::ToolInfo getCurrentTime_info();
};
