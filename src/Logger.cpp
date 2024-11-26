#include "Logger.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex);
    if (logFile.is_open()) {
        logFile.close();
    }
    logFile.open(filename, std::ios::app);
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex);
    currentLevel = level;
}

void Logger::log(LogLevel level, const std::string& message, const char* file, int line) {
    if (level < currentLevel) return;

    std::lock_guard<std::mutex> lock(mutex);
    std::stringstream ss;
    ss << getTimestamp() << " "
       << "[" << getLevelString(level) << "] "
       << "[" << getThreadId() << "] "
       << file << ":" << line << " "
       << message << std::endl;

    std::cout << ss.str();
    if (logFile.is_open()) {
        logFile << ss.str();
        logFile.flush();
    }
}

std::string Logger::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        default:             return "UNKNOWN";
    }
}

std::string Logger::getThreadId() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return ss.str();
}