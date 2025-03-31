#include "Logger.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

LogStream::LogStream(int t, const char* file, int line)
{
    wholeStr = QString("[%1::%2] ").arg(file).arg(line);
    type = t;
}

LogStream& LogStream::operator<<(const char* str)
{
    wholeStr.append(str);
    return *this;
}

LogStream& LogStream::operator<<(const QString& str)
{
    wholeStr.append(str);
    return *this;
}

LogStream& LogStream::operator<<(const std::string& str)
{
    wholeStr.append(str);
    return *this;
}

LogStream::~LogStream()
{
    spdlog::log((spdlog::level::level_enum)type, qPrintable(wholeStr));
}

Logger::Logger()
{

}

Logger* Logger::instance()
{
    static Logger logger;
    return &logger;
}

void Logger::init(const QString& name)
{
    // 创建控制台 Sink
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(spdlog::color_mode::always);
    console_sink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e][%l]%v%$"); // 设置控制台输出的格式
    console_sink->set_color(spdlog::level::debug, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
    console_sink->set_color(spdlog::level::info, FOREGROUND_GREEN);

    // 创建文件 Sink
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(qPrintable(QString("logs/%1.log").arg(name)), true);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%l]%v"); // 设置文件输出的格式

    // 创建 Logger 并添加 Sink
    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("multi_sink_logger", sinks.begin(), sinks.end());
    spdlog::set_default_logger(logger);

#if _DEBUG
    spdlog::set_level(spdlog::level::debug);
    logger->flush_on(spdlog::level::debug);
#else
    logger->flush_on(spdlog::level::info);
#endif
}

Logger::~Logger()
{
}
