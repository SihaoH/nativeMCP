#pragma once
#include "MCPServer.h"

class ComputerUse : public MCPServer
{
    Q_OBJECT

public:
    ComputerUse();
    ~ComputerUse() = default;

public slots:
    QString waitTime(const QVariant& sec);
    MCPServer::ToolInfo waitTime$info();

    QString moveCursor(const QVariant& delta_x, const QVariant& delta_y);
    MCPServer::ToolInfo moveCursor$info();

    QString setCursor(const QVariant& x, const QVariant& y);
    MCPServer::ToolInfo setCursor$info();

    QString click();
    MCPServer::ToolInfo click$info();

    QString doubleClick();
    MCPServer::ToolInfo doubleClick$info();

    QString type(const QVariant& text);
    MCPServer::ToolInfo type$info();

    QString pressKey(const QVariant& key);
    MCPServer::ToolInfo pressKey$info();

    QString releaseKey(const QVariant& key);
    MCPServer::ToolInfo releaseKey$info();

    QString knockKey(const QVariant& key);
    MCPServer::ToolInfo knockKey$info();
};