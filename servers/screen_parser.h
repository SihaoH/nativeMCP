#pragma once
#include "MCPServer.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include <QScopedPointer>
#include <QJsonObject>

class ScreenParser : public MCPServer
{
    Q_OBJECT

public:
    ScreenParser();
    ~ScreenParser() = default;

    void init(int argc, char* argv[]) override;

public slots:
    QString getScreenInfo();
    MCPServer::ToolInfo getScreenInfo$info();

    QString getElementPosition(const QVariant& desc);
    MCPServer::ToolInfo getElementPosition$info();

private:
    QString getScreenShot();
    QString request(const QJsonObject& req_body);

private:
    QString model;
    QString path;
    QScopedPointer<httplib::Client> client;
};
