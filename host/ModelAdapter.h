#pragma once
#include <httplib.h>
#include <QString>
#include <QJsonArray>
#include <QScopedPointer>

class ModelAdapter
{
public:
    static ModelAdapter* instance();
    
    void init(const QString& host, const QString& chat_m);
    QString chat(const QJsonArray& messages, const QJsonArray& tools);

private:
    ModelAdapter();

private:
    QString chatModel = "qwen2.5:7b";
    QScopedPointer<httplib::Client> ollama;
};
