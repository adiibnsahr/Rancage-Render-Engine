#include "Logger.h"
#include <iostream>
#include <Windows.h>

std::ofstream Logger::m_LogStream;

void Logger::Init(const std::string& logFile) {
    size_t pos = logFile.find_last_of("/\\");
    if (pos != std::string::npos) {
        std::string directory = logFile.substr(0, pos);
        if (CreateDirectoryA(directory.c_str(), nullptr)) {
            std::cout << "[INFO] Created directory: " << directory << "\n";
        }
        else if (GetLastError() == ERROR_ALREADY_EXISTS) {
            std::cout << "[INFO] Directory already exists: " << directory << "\n";
        }
        else {
            std::cerr << "[ERROR] Failed to create directory: " << directory << " (Error: " << GetLastError() << ")\n";
        }
    }

    m_LogStream.open(logFile, std::ios::out | std::ios::trunc);
    if (!m_LogStream.is_open()) {
        std::cerr << "[ERROR] Failed to open log file: " << logFile << "\n";
        return;
    }
    m_LogStream << "Log started\n";
    m_LogStream.flush(); // Flush setelah nulis "Log started"
}

void Logger::Log(LogLevel level, const std::string& message) {
    std::string prefix;
    switch (level) {
    case LogLevel::Info: prefix = "[INFO]"; break;
    case LogLevel::Warning: prefix = "[WARNING]"; break;
    case LogLevel::Error: prefix = "[ERROR]"; break;
    }
    std::string fullMessage = prefix + " " + message + "\n";
    std::cout << fullMessage;
    if (m_LogStream.is_open()) {
        m_LogStream << fullMessage;
        m_LogStream.flush(); // Flush setiap log
    }
}

void Logger::Flush() {
    if (m_LogStream.is_open()) {
        m_LogStream.flush();
    }
    std::cout.flush();
}

void Logger::Shutdown() {
    if (m_LogStream.is_open()) {
        m_LogStream.close();
    }
}