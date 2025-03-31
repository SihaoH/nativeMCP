#pragma once
#include <spdlog/common.h>
#include <QString>

#define LOG(LEVEL) LogStream(spdlog::level::##LEVEL, __FILE__, __LINE__)

class LogStream
{
public:
    LogStream(int t, const char* file, int line);
    LogStream& operator<< (const char* str);
    LogStream& operator<< (const QString& str);
    LogStream& operator<< (const std::string& str);
    ~LogStream();
private:
    int type;
    QString wholeStr;
};
class Logger
{
public:
    static Logger* instance();
    void init(const QString& name);
    ~Logger();

private:
    Logger();
};
