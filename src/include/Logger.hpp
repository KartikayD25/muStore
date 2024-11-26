#pragma once
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static Logger& getInstance();
    void setLogFile(const std::string& filename);
    void setLogLevel(LogLevel level);
    void log(LogLevel level, const std::string& message, const char* file, int line);

private:
    Logger() = default;
    std::mutex mutex;
    std::ofstream logFile;
    LogLevel currentLevel{LogLevel::INFO};
    std::string getTimestamp();
    std::string getLevelString(LogLevel level);
    std::string getThreadId();
};

#define LOG_DEBUG(msg) Logger::getInstance().log(LogLevel::DEBUG, msg, __FILE__, __LINE__)
#define LOG_INFO(msg)  Logger::getInstance().log(LogLevel::INFO, msg, __FILE__, __LINE__)
#define LOG_WARN(msg)  Logger::getInstance().log(LogLevel::WARN, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) Logger::getInstance().log(LogLevel::ERROR, msg, __FILE__, __LINE__)