#pragma once
#include <QObject>
#include <QJsonArray>
#include <QMap>

class MCPClient;
class ModelAdapter;

class Host : public QObject
{
    Q_OBJECT

public:
    explicit Host(QObject *parent = nullptr);
    ~Host() = default;

    void init();
    int run();

private:
    void process(const QString& content);

private:
    QMap<QString, MCPClient*> clientMap;
    QMap<QString, QString> toolMap;
    QJsonArray messageList;
};
