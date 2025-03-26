#include "ModelAdapter.h"
#include "Logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ModelAdapter::ModelAdapter()
    : ollama(new httplib::Client("http://localhost:11434"))
{
}

ModelAdapter* ModelAdapter::instance()
{
    static ModelAdapter ollama;
    return &ollama;
}

void ModelAdapter::init(const QString& host, const QString& chat_m)
{
    ollama.reset(new httplib::Client(host.toStdString()));
    chatModel = chat_m;
    LOG(info) << "ModelAdapter初始化：" << host << ", " << chat_m;
}

QString ModelAdapter::chat(const QJsonArray& messages, const QStringList& tools)
{
    QJsonArray converted_array;
    for (const auto& tool : tools) {
        auto tool_obj = QJsonDocument::fromJson(tool.toUtf8()).object();
        QJsonObject converted_tool{
            {"type", "function"},
            {"function", QJsonObject{
                {"name", tool_obj["name"]},
                {"description", tool_obj["description"]},
                {"parameters", tool_obj["inputSchema"]}
            }}
        };
        converted_array.append(converted_tool);
    }
    QJsonObject req_body {
        {"model", chatModel},
        {"messages", messages},
        {"stream", false},
        {"tools", converted_array}
    };
    auto resp = ollama->Post("/api/chat",
                            QJsonDocument(req_body).toJson().toStdString(),
                            "application/json");
    
    if (resp && resp->status == 200) {
        return QString::fromStdString(resp->body);
    } else {
        LOG(err) << "大模型请求出错：" << QString::fromStdString(resp->body);
        return QString();
    }
}
