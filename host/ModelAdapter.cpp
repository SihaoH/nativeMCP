#include "ModelAdapter.h"
#include "Logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ModelAdapter::ModelAdapter()
    : client(new httplib::Client("http://localhost:11434"))
{
}

ModelAdapter* ModelAdapter::instance()
{
    static ModelAdapter instance;
    return &instance;
}

void ModelAdapter::init(const QString& url, const QString& model, const QString& api_key)
{
    if (!url.isEmpty()) {
        // 从第一个非URL协议分隔符的/处拆分URL
        QRegularExpression re("(?<!:)(?<!/)(/[^/])");
        auto match = re.match(url);
        if (match.hasMatch()) {
            int pos = match.capturedStart();
            QString base_url = url.left(pos);
            path = url.mid(pos);
            client.reset(new httplib::Client(base_url.toStdString()));
        } else {
            client.reset(new httplib::Client(url.toStdString()));
        }
    }
    if (!model.isEmpty()) {
        chatModel = model;
    }
    if (!api_key.isEmpty()) {
        client->set_default_headers({{"Authorization", QString("Bearer %1").arg(api_key).toStdString()}});
    }
    LOG(info) << "ModelAdapter初始化：" << url << ", " << model;
}

QString ModelAdapter::chat(const QJsonArray& messages, const QJsonArray& tools)
{
    QJsonObject req_body {
        {"model", chatModel},
        {"messages", messages},
        {"stream", false},
        {"tools", tools}
    };
    auto resp = client->Post(path.toStdString(),
                            QJsonDocument(req_body).toJson(QJsonDocument::Compact).toStdString(),
                            "application/json");

    if (resp) {
        if (resp->status == 200) {
            return QString::fromStdString(resp->body);
        } else {
            LOG(err) << "请求错误：" << resp->body;
            return QString();
        }
    } else {
        LOG(err) << "连接错误：" << httplib::to_string(resp.error());
        return QString();
    }
}
