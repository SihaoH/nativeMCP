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

QString ModelAdapter::chat(const QJsonArray& messages, const QJsonArray& tools)
{
    
    QJsonObject req_body {
        {"model", chatModel},
        {"messages", messages},
        {"stream", false},
        {"tools", tools}
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
