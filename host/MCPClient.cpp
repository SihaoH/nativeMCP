#include "MCPClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QEventLoop>

MCPClient::MCPClient(QObject *parent) 
    : QObject(parent)
    , process(new QProcess(this))
{
}

int MCPClient::getMsgID()
{
    return msgID++;
}

QString MCPClient::request(const QString& msg) const
{
    process->write(msg.toUtf8());
    process->write("\n");
    process->waitForBytesWritten();
    QString resp;
    while (resp.isEmpty()) {
        process->waitForReadyRead();
        resp = process->readAll();
    }
    return resp;
}

void MCPClient::init()
{
    QJsonObject obj {
        {"method", "initialize"},
        {"jsonrpc", "2.0"},
        {"id", getMsgID()},
        {"params", QJsonObject{
            {"protocolVersion", "2024-11-05"},
            {"capabilities", QJsonObject{
                {"tools", true},
                {"prompts", false},
                {"resources", true}, 
                {"logging", false},
                {"roots", QJsonObject{
                    {"listChanged", false}
                }}
            }},
            {"clientInfo", QJsonObject{
                {"name", "mcp-cpp"},
                {"version", "0.1.0"}
            }}
        }}
    };

    auto rsp =  request(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    QJsonDocument doc = QJsonDocument::fromJson(rsp.toUtf8());
    if (doc.isObject()) {
        QJsonObject rsp_obj = doc.object();
        serverName = rsp_obj["result"].toObject()["serverInfo"].toObject()["name"].toString();
        serverVersion = rsp_obj["result"].toObject()["serverInfo"].toObject()["version"].toString();
        QJsonObject notify_obj {
            {"method", "notifications/initialized"},
            {"jsonrpc", "2.0"}
        };
        process->write(QJsonDocument(notify_obj).toJson(QJsonDocument::Compact) + "\n");
    }
}

bool MCPClient::isConnected() const
{
    return process->state() == QProcess::Running;
}

void MCPClient::connectToServer(const QString& command, const QStringList& args)
{
    process->start(command, args);
    if (process->waitForStarted()) {
        init();
    }
}

void MCPClient::disconnectFromServer()
{
    process->terminate();
}

QString MCPClient::getTools()
{
    QJsonObject obj {
        {"method", "tools/list"},
        {"jsonrpc", "2.0"},
        {"id", getMsgID()}
    };
    auto rsp = request(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    QJsonDocument doc = QJsonDocument::fromJson(rsp.toUtf8());
    if (doc.isObject()) {
        QJsonObject rsp_obj = doc.object();
        return QJsonDocument(rsp_obj["result"].toObject()["tools"].toArray()).toJson(QJsonDocument::Compact);
    }
    return "";
}

QString MCPClient::callTool(const QString& name, const QString& args)
{
    QJsonObject obj {
        {"method", "tools/call"},
        {"jsonrpc", "2.0"}, 
        {"id", getMsgID()},
        {"params", QJsonObject{
            {"name", name},
            {"arguments", QJsonDocument::fromJson(args.toUtf8()).object()}
        }}
    };
    auto rsp = request(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    QJsonDocument doc = QJsonDocument::fromJson(rsp.toUtf8());
    if (doc.isObject()) {
        QJsonObject rsp_obj = doc.object();
        return QJsonDocument(rsp_obj["result"].toObject()).toJson(QJsonDocument::Compact);
    }
    return "";
}

