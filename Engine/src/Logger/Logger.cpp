// File: src/Logger/Logger.cpp
#include "Logger/Logger.h"
#include <iostream>

void Logger::Log(LogLevel level, const std::string& message) {
    std::cout << "[" << LevelToString(level) << "] " << message << std::endl;
}

std::string Logger::LevelToString(LogLevel level) const {
    switch (level) {
    case LogLevel::Trace: return "TRACE";
    case LogLevel::Debug: return "DEBUG";
    case LogLevel::Info: return "INFO";
    case LogLevel::Warning: return "WARNING";
    case LogLevel::Error: return "ERROR";
    case LogLevel::Fatal: return "FATAL";
    default: return "UNKNOWN";
    }
}