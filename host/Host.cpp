#include "Host.h"
#include "ModelAdapter.h"
#include "MCPClient.h"
#include "Logger.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

Host::Host(QObject *parent)
    : QObject(parent)
{
}

void Host::init()
{
    LOG(info) << "MCP Host初始化...";

    QFile cfg_file("./config.json");
    if (!cfg_file.open(QIODevice::ReadOnly)) {
        LOG(err) << "Failed to open config file:" << "./config.json";
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(cfg_file.readAll());
    if (doc.isNull()) {
        LOG(err) << "Failed to parse config file";
        return;
    }

    // 初始化模型配置
    auto cfg_obj = doc.object();
    auto model_cfg = cfg_obj["api"].toObject();
    ModelAdapter::instance()->init(model_cfg["url"].toString(), model_cfg["model"].toString(), model_cfg["api_key"].toString());

    // 初始化MCP servers配置
    auto mcp_servers = cfg_obj["mcpServers"].toObject();
    QString log_info = "\n可用工具列表：\n";
    for (auto it = mcp_servers.begin(); it != mcp_servers.end(); ++it) {
        auto server_name = it.key();
        auto server_cfg = it.value().toObject();

        MCPClient* client = new MCPClient(this);
        client->connectToServer(server_cfg["command"].toString(), server_cfg["args"].toVariant().toStringList());
        if (client->isConnected()) {
            clientMap.insert(server_name, client);
            log_info.append(server_name + ": \n");
        } else {
            LOG(err) << "无法连接到MCP server: " << server_name;
            delete client;
            continue;
        }

        auto client_tools = client->getTools();
        auto tools_array = QJsonDocument::fromJson(client_tools.toUtf8()).array();
        for (const auto& tool : tools_array) {
            auto tool_obj = tool.toObject();
            auto tool_name = tool_obj["name"].toString();
            // 将名称混合成server_name::tool_name的形式，避免冲突
            auto mix_name = QString("%1::%2").arg(server_name).arg(tool_name);
            tool_obj["name"] = mix_name;
            toolMap.insert(mix_name, QJsonDocument(tool_obj).toJson(QJsonDocument::Compact));
            log_info.append(QString("  %1 %2\n").arg(tool_name).arg(tool_obj["description"].toString()));
        }
    }
    LOG(info) << log_info;

    // 初始化对话列表
    messageList.append(QJsonObject{
        {"role", "system"},
        {"content", "你是一个智能助手。如果用户的问题需要使用工具才能回答，请使用提供的工具。如果可以直接回答，就直接给出答案，不用理会工具列表。"}
    });
}

void Host::process(const QString& content)
{
    messageList.append(QJsonObject{
        {"role", "user"},
        {"content", content}
    });

    QJsonArray tools_array;
    for (const auto& tool : toolMap.values()) {
        auto tool_obj = QJsonDocument::fromJson(tool.toUtf8()).object();
        QJsonObject converted_tool{
            {"type", "function"},
            {"function", QJsonObject{
                {"name", tool_obj["name"]},
                {"description", tool_obj["description"]},
                {"parameters", tool_obj["inputSchema"]}
            }}
        };
        tools_array.append(converted_tool);
    }
    for (;;) {
        auto response_str = ModelAdapter::instance()->chat(messageList, tools_array);
        if (response_str.isEmpty()) {
            return;
        }
        auto message_obj = QJsonDocument::fromJson(response_str.toUtf8()).object()["message"].toObject();
        
        auto resp_content = message_obj["content"].toString();
        if (!resp_content.isEmpty()) {
            messageList.append(QJsonObject{
                {"role", "assistant"},
                {"content", resp_content}
            });
        }
        
        // 如果有工具调用，添加到消息中
        if (message_obj.contains("tool_calls")) {
            QJsonArray tool_calls = message_obj["tool_calls"].toArray();
            for (const auto& call : tool_calls) {
                auto function = call.toObject()["function"].toObject();
                auto mix_name = function["name"].toString();
                auto tool_args = QJsonDocument(function["arguments"].toObject()).toJson(QJsonDocument::Compact);
                
                // 获取对应的客户端
                auto mix_name_pair = mix_name.split("::");
                if (mix_name_pair.count() != 2) {
                    LOG(err) << "工具名称不合法（应该是server_name::tool_name) " << mix_name;
                    continue;
                }
                auto client_name = mix_name_pair[0];
                auto tool_name = mix_name_pair[1];
                MCPClient* client = clientMap.value(client_name);
                if (!client) {
                    LOG(err) << "未找到对应的Client: " << client_name;
                    continue;
                }
                // 调用工具并获取结果
                LOG(info) << QString("调用MCP工具%1(%2): ").arg(mix_name).arg(tool_args);
                auto tool_result = client->callTool(tool_name, tool_args);
                tool_result = QJsonDocument::fromJson(tool_result.toUtf8()).object()["content"].toArray()[0].toObject()["text"].toString();
                LOG(info) << tool_result;

                messageList.append(QJsonObject{
                    {"role", "tool"},
                    {"name", mix_name},
                    {"content", tool_result}
                });
            }
        } else {
            QString final_response = message_obj["content"].toString();
            QTextStream out(stdout);
            out.setEncoding(QStringConverter::System);
            out << final_response + "\n" << Qt::endl;
            break;
        }
    }
}

int Host::run()
{
    QTextStream in(stdin);
    in.setEncoding(QStringConverter::System);

    // 通过stdio处理与客户端的请求
    while (!in.atEnd()) {
        QTextStream out(stdout);
        out.setEncoding(QStringConverter::System);
        out << ">>> " << Qt::flush;
        QString line = in.readLine();
        if (!line.isEmpty()) {
            process(line);
        }
    }
    return 0;
}
