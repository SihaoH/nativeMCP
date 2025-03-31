#include "screen_parser.h"
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QGuiApplication>
#include <QScreen>
#include <QPixmap>
#include <QBuffer>
#include <QRegularExpression>

ScreenParser::ScreenParser()
    : MCPServer()
{
}

void ScreenParser::init(int argc, char* argv[])
{
    auto app = new QGuiApplication(argc, argv);
    app->setParent(this);
    if (argc >= 4) {
        QRegularExpression re("(?<!:)(?<!/)(/[^/])");
        auto url = QString::fromLocal8Bit(argv[1]);
        auto match = re.match(url);
        if (match.hasMatch()) {
            int pos = match.capturedStart();
            QString base_url = url.left(pos);
            path = url.mid(pos);
            client.reset(new httplib::Client(base_url.toStdString()));
        } else {
            client.reset(new httplib::Client(url.toStdString()));
        }

        model = QString::fromLocal8Bit(argv[2]);
        client->set_default_headers({{"Authorization", QString("Bearer %1").arg(QString::fromLocal8Bit(argv[3])).toStdString()}});
    }
}

QString ScreenParser::getScreenShot()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    QPixmap pixmap = screen->grabWindow(0);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    return QString::fromLatin1(byteArray.toBase64());
}

QString ScreenParser::request(const QJsonObject& req_body) {
    auto resp = client->Post(path.toStdString(),
                            QJsonDocument(req_body).toJson(QJsonDocument::Compact).toStdString(),
                            "application/json");

    if (resp) {
        if (resp->status == 200) {
            return QJsonDocument::fromJson(QByteArray::fromStdString(resp->body)).object()["choices"].toArray()[0].toObject()["message"].toObject()["content"].toString();
        } else {
            setLastCallError(true);
            return QString("请求错误：%1").arg(QString::fromStdString(resp->body));
        }
    } else {
        setLastCallError(true);
        return QString("连接错误：%1").arg(QString::fromStdString(httplib::to_string(resp.error()))); ;
    }
}

QString ScreenParser::getScreenInfo()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    auto screen_size = screen->size();
    QJsonArray messages;
    messages.append(QJsonObject{
        {"role", "system"},
        {"content", QString("你需要辅助一位有视力障碍的用户去使用电脑，在他面前的电脑屏幕分辨率为%1*%2，请根分析屏幕内有什么内容，请大概描述并直接给出结果即可").arg(screen_size.width()).arg(screen_size.height())}
    });
    messages.append(QJsonObject{
        {"role", "user"},
        {"content", QJsonArray{QJsonObject{
            {"type", "image_url"},
            {"image_url", QJsonObject{{"url", QString("data:image/png;base64,%1").arg(getScreenShot())}}}
        }}},
    });
    return request({
        {"model", model},
        {"messages", messages},
        {"stream", false}
    });
}

MCPServer::ToolInfo ScreenParser::getScreenInfo$info()
{
    return {"getScreenInfo", "操作电脑前或者操作完成后，都需要获取屏幕截图的基本信息，根据描述来决定下一步的操作", {}};
}

QString ScreenParser::getElementPosition(const QVariant& desc)
{
    QScreen* screen = QGuiApplication::primaryScreen();
    auto screen_size = screen->size();
    QJsonArray messages;
    messages.append(QJsonObject{
        {"role", "system"},
        {"content", QString("你需要辅助一位有视力障碍的用户去操作电脑，在他面前的电脑屏幕分辨率为%1*%2，屏幕的左上角为原点(0, 0)，向右为x轴正方向，向下为y轴正方向。请根据描述找到元素的中心位置，并返回元素的中心位置的坐标(x, y)，x和y的单位为像素，请务必精确，并直接给出结果，不要需要任何描述和计算过程！").arg(screen_size.width()).arg(screen_size.height())}
    });
    messages.append(QJsonObject{
        {"role", "user"},
        {"content", QJsonArray{
        QJsonObject{
            {"type", "image_url"},
            {"image_url", QJsonObject{{"url", QString("data:image/png;base64,%1").arg(getScreenShot())}}}
        },
        QJsonObject{
            {"type", "text"},
            {"text", desc.toString()}
        }
        }},
    });
    return request({
        {"model", model},
        {"messages", messages},
        {"stream", false}
    });
}

MCPServer::ToolInfo ScreenParser::getElementPosition$info()
{
    return {
        "getElementPosition",
        "获取屏幕内元素的中心位置，为了将光标移动到正确的位置",
        {
            {"desc", "string", "元素描述，尽可能具体，例如chrome浏览器的关闭按钮、微信聊天窗口的发送按钮"},
        }
    };
}

MCP_SERVER_MAIN(ScreenParser)
