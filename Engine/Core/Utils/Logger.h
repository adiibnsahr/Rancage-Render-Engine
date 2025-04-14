#pragma once
#include <string>
#include <iostream>
#include <fstream>

enum class LogLevel { Info, Warning, Error };

class Logger {
public:
    static void Init(const std::string& logFile);
    static void Log(LogLevel level, const std::string& message);
    static void Flush();
    static void Shutdown();
private:
    static std::ofstream m_LogStream;
};