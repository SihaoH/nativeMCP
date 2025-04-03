#pragma once
#include <QObject>

#ifdef BUILD_MCPSERVER
    #define MCPSERVER_EXPORT __declspec(dllexport)
#else
    #define MCPSERVER_EXPORT __declspec(dllimport)
#endif 

class MCPSERVER_EXPORT MCPServer : public QObject
{
    Q_OBJECT

public:
    struct MCPSERVER_EXPORT ParamInfo
    {
        QString name;
        QString type;
        QString desc;
    };
    struct MCPSERVER_EXPORT ToolInfo
    {
        QString name;
        QString desc;
        QList<ParamInfo> params;
    };

/*
// 子类提供的工具，需要按以下格式定义：
public slots:
    QString doSomthing(const QVariant& timezone); // 可用的工具/函数，参数必须是QVariant类型，返回值必须是JSON格式的字符串，内容自定
    MCPServer::ToolInfo doSomthing_info(); // 工具/函数的信息，后缀必须是$info，且无参
*/

public:
    MCPServer(QObject *parent = nullptr);
    virtual ~MCPServer();
    virtual void init(int argc, char* argv[]) {}
    virtual int run();

protected:
    void setName(const QString& name);
    void setVersion(const QString& version);
    void setLastCallError(bool error);
    bool lastCallError() const;

private:
    class MCPServerPrivate* d = nullptr;
};

// 每个服务器都是一个独立进程，需要定义一个main函数
#define MCP_SERVER_MAIN(server_name) \
    int main(int argc, char *argv[]) \
    { \
        MCPServer* server = new server_name(); \
        server->init(argc, argv); \
        return server->run(); \
    }
