#pragma once
#include <httplib.h>
#include <QString>
#include <QJsonArray>
#include <QScopedPointer>

class ModelAdapter
{
public:
    static ModelAdapter* instance();
    
    void init(const QString& url, const QString& model, const QString& api_key = QString());
    QString chat(const QJsonArray& messages, const QJsonArray& tools);

private:
    ModelAdapter();

private:
    QString chatModel = "qwen2.5:7b";
    QString path;
    QScopedPointer<httplib::Client> client;
};
