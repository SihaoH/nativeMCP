#pragma once
#include <QObject>

class MCPClient : public QObject 
{
    Q_OBJECT

public:
    MCPClient(QObject *parent = nullptr);
    ~MCPClient() = default;

    bool isConnected() const;
    void connectToServer(const QString& command, const QStringList& args);
    void disconnectFromServer();
    QString getTools();
    QString callTool(const QString& name, const QString& args);

private:
    int getMsgID();
    QString request(const QString& msg) const;
    void init();

private:
    class QProcess* process = nullptr;
    QString serverName;
    QString serverVersion;
    int msgID = 0;
};