#include "server_time.h"
#include <QDateTime>
#include <QTimeZone>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextStream>


ServerTime::ServerTime()
    : MCPServer()
{
    setName("mcp-cpp-time");
    setVersion("0.1.0");
}

QString ServerTime::getCurrentTime(const QVariant& timezone)
{
    QTimeZone tz(timezone.toString().toUtf8());
    if (!tz.isValid()) {
        setLastCallError(true);
        return "Invalid timezone specified";
    }
    auto cur_time = QDateTime::currentDateTime().toTimeZone(tz).toString(Qt::ISODateWithMs);
    QJsonObject ret_obj;
    ret_obj["timezone"] = QString::fromUtf8(tz.id());
    ret_obj["datetime"] = cur_time;

    return QJsonDocument(ret_obj).toJson(QJsonDocument::Indented);
}

MCPServer::ToolInfo ServerTime::getCurrentTime_info()
{
    MCPServer::ToolInfo info;
    info.name = "getCurrentTime";
    info.desc = "获取当前时间";
    info.params = QList<MCPServer::ParamInfo>{
        {"timezone", "string", "IANA时区名称(例如 'America/New_York', 'Europe/London')."}
    };
    return info;
}


MCP_SERVER_MAIN(ServerTime)
