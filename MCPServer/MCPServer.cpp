#include "MCPServer.h"
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaMethod>

class MCPServerPrivate
{
public:
    QString handleRequest(const QString& req_json)
    {
        QJsonDocument doc = QJsonDocument::fromJson(req_json.toUtf8());
        if (doc.isNull() || !doc.isObject()) {
            QJsonObject error_json {
                {"jsonrpc", "2.0"},
                {"error", QJsonObject{
                    {"code", -32700},
                    {"message", "Parse error"}
                }},
                {"id", QJsonValue::Null}
            };
            return QJsonDocument(error_json).toJson(QJsonDocument::Compact);
        }

        QJsonObject req_obj = doc.object();
        QJsonObject rsp_obj;
        auto method = req_obj["method"].toString();
        rsp_obj["jsonrpc"] = "2.0";
        rsp_obj["id"] = req_obj["id"];

        if (method == "initialize") {
            QString method = req_obj["method"].toString();
            QJsonObject ret_obj;
            ret_obj["protocolVersion"] = req_obj["params"].toObject()["protocolVersion"].toString();
            ret_obj["capabilities"] = QJsonObject{
                {"experimental", QJsonObject{}},
                {"tools", QJsonObject{
                    {"listChanged", false}
                }}
            };
            ret_obj["serverInfo"] = QJsonObject{
                {"name", name},
                {"version", version}
            };
        
            rsp_obj["result"] = ret_obj;
        } else if (method == "tools/list") {
            auto tools = getTools();
            QJsonArray tools_array;
            for (const auto& tool : tools) {
                QJsonObject tool_obj;
                QJsonObject inputSchema_obj;
                QJsonArray required_array;
                tool_obj["name"] = tool.name;
                tool_obj["description"] = tool.desc;
                inputSchema_obj["type"] = "object";
                QJsonObject properties_obj;
                for (const auto& param : tool.params) {
                    QJsonObject property_obj;
                    property_obj["type"] = param.type;
                    property_obj["description"] = param.desc;
                    properties_obj[param.name] = property_obj;
                    required_array.append(param.name);
                }
                inputSchema_obj["properties"] = properties_obj;
                inputSchema_obj["required"] = required_array;
                tool_obj["inputSchema"] = inputSchema_obj;
                tools_array.append(tool_obj);
            }
            QJsonObject ret_obj;
            ret_obj["tools"] = tools_array;
            rsp_obj["result"] = ret_obj;
        } else if (method == "tools/call") {
            rsp_obj["result"] = callTool(req_obj["params"].toObject());
        } else if (method.contains("notifications")) {
            // 如果是客户端的通知信息，则直接忽略
            return "";
        }

        return QJsonDocument(rsp_obj).toJson(QJsonDocument::Compact);
    }

    QList<MCPServer::ToolInfo> getTools()
    {
        // 获取所有可用的工具，即编译后缀为_info的public slots函数
        QList<MCPServer::ToolInfo> tools;
        auto meta_obj = d->metaObject();
        for (int i = meta_obj->methodOffset(); i < meta_obj->methodCount(); ++i) {
            auto tool = meta_obj->method(i);

            if (tool.methodType() == QMetaMethod::Slot && 
                tool.access() == QMetaMethod::Public && 
                tool.name().contains("$info")) {
                    MCPServer::ToolInfo tool_info;
                    tool.invoke(d, Qt::DirectConnection, qReturnArg(tool_info));
                    tools.append(tool_info);
            }
        }
        return tools;
    }

    QJsonObject callTool(const QJsonObject& req_obj)
    {
        auto tool_name = req_obj["name"].toString();
        auto arguments = req_obj["arguments"].toObject();
        auto meta_obj = d->metaObject();

        auto ret_str = QString("Tool not found");
        auto is_err = true;
        for (int i = meta_obj->methodOffset(); i < meta_obj->methodCount(); ++i) {
            auto tool = meta_obj->method(i);

            if (tool.methodType() == QMetaMethod::Slot &&
                tool.access() == QMetaMethod::Public &&
                tool.name() == tool_name) {
                // QGenericArgument似乎要在Qt7之后废弃
                QList<QGenericArgument> args(10);
                QList<QVariant> args_ref(10);
                int param_idx = 0;
                for (const QString& param_name : tool.parameterNames()) {
                    args_ref[param_idx] = arguments[param_name].toVariant();
                    args[param_idx] = QGenericArgument("QVariant", &args_ref[param_idx]);
                    param_idx += 1;
                }

                lastCallErr = false;
                is_err = !tool.invoke(
                    d, Qt::DirectConnection,
                    QGenericReturnArgument("QString", &ret_str),
                    args[0], args[1], args[2], args[3], args[4],
                    args[5], args[6], args[7], args[8], args[9]
                );
                if (is_err == false) {
                    is_err = lastCallErr;
                }
                break;
            }
        }

        QJsonObject result;
        result["content"] = QJsonArray{
            QJsonObject{
                {"type", "text"},
                {"text", ret_str}
            }
        };
        result["isError"] = is_err;
        return result;
    }

public:
    MCPServer* d = nullptr;
    bool lastCallErr = false;
    QString name = "mcp_server";
    QString version = "1.0";
};

MCPServer::MCPServer(QObject *parent)
    : QObject(parent)
    , p(new MCPServerPrivate)
{
    p->d = this;
}

MCPServer::~MCPServer()
{
    delete p;
}

int MCPServer::run()
{
    QTextStream in(stdin);
    in.setEncoding(QStringConverter::Utf8);

    // 通过stdio处理与客户端的请求
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            auto response = p->handleRequest(line);
            if (!response.isEmpty()) {
                QTextStream(stdout) << response << Qt::endl;
            }
        }
    }
    return 0;
}

void MCPServer::setName(const QString& name)
{
    p->name = name;
}

void MCPServer::setVersion(const QString& version)
{
    p->version = version;
}

void MCPServer::setLastCallError(bool error)
{
    p->lastCallErr = error;
}

bool MCPServer::lastCallError() const
{
    return p->lastCallErr;
}
