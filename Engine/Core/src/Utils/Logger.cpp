#include "../Core/include/Utils/Logger.h"
#include <iostream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <cstdarg>

namespace Logger
{
    static std::mutex m_LogMutex;

    static std::wstring GetTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        struct tm timeInfo;
        localtime_s(&timeInfo, &time);
        wchar_t buffer[20];
        wcsftime(buffer, sizeof(buffer), L"%Y-%m-%d %H:%M:%S", &timeInfo);
        return buffer;
    }

    void Init(const std::wstring& logFile)
    {
        std::lock_guard<std::mutex> lock(m_LogMutex);
        std::wcout << L"[INFO] Logger initialized, file output disabled for stability\n";
    }

    void Log(LogLevel level, const char* format, ...)
    {
        std::lock_guard<std::mutex> lock(m_LogMutex);

        std::string prefix;
        switch (level)
        {
        case LogLevel::Info: prefix = "[INFO]"; break;
        case LogLevel::Warning: prefix = "[WARNING]"; break;
        case LogLevel::Error: prefix = "[ERROR]"; break;
        }

        char buffer[1024] = {};
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        std::wstring timestamp = GetTimestamp();
        std::string message = prefix + " " + buffer;
        std::wstring wmessage = timestamp + L" " + std::wstring(message.begin(), message.end()) + L"\n";

        std::wcout << wmessage;
    }

    void Flush()
    {
        std::lock_guard<std::mutex> lock(m_LogMutex);
        std::wcout.flush();
    }

    void Shutdown()
    {
        std::lock_guard<std::mutex> lock(m_LogMutex);
        std::wcout << L"[INFO] Logger shutdown\n";
        std::wcout.flush();
    }
}