#include "server_time.h"
#include <QDateTime>
#include <QTimeZone>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>
#include <QThread>


ServerTime::ServerTime()
    : MCPServer()
{
    setName("mcp-cpp-time");
    setVersion("0.1.0");
}

QString ServerTime::getCurrentTime(const QVariant& timezone)
{
    auto cur_time = QDateTime::currentDateTime();
    QTimeZone tz(QTimeZone::LocalTime);
    if (!timezone.toString().isEmpty()) {
        tz = QTimeZone(timezone.toString().toUtf8());
        if (!tz.isValid()) {
            setLastCallError(true);
            return "Invalid timezone specified";
        }
        cur_time = cur_time.toTimeZone(tz);
    }

    QJsonObject ret_obj{
        {"timezone", QString::fromUtf8(tz.id())},
        {"datetime", cur_time.toString(Qt::ISODateWithMs)}
    };

    return QJsonDocument(ret_obj).toJson(QJsonDocument::Indented);
}

MCPServer::ToolInfo ServerTime::getCurrentTime$info()
{
    MCPServer::ToolInfo info;
    info.name = "getCurrentTime";
    info.desc = "获取当前时间";
    info.params = QList<MCPServer::ParamInfo>{
        {"timezone", "string", "IANA时区名称(例如'Asia/Shanghai')，可以为空，为空则直接返回系统时间"}
    };
    return info;
}

QString ServerTime::waitTime(const QVariant& sec)
{
    auto sec_int = sec.toInt();
    QThread::sleep(sec_int);
    return QString("{\"result\": \"已过去%1秒，请继续操作\"}").arg(sec_int);
}

MCPServer::ToolInfo ServerTime::waitTime$info()
{
    MCPServer::ToolInfo info;
    info.name = "waitTime";
    info.desc = "等待指定的时间";
    info.params = QList<MCPServer::ParamInfo>{
        {"sec", "int", "等待的时间，单位是秒数"}
    };
    return info;
}


MCP_SERVER_MAIN(ServerTime)
