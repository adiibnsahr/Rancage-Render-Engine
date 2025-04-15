#include "../Core/include/Utils/Logger.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <ctime>
#include <cstdarg>
#include <Windows.h>

namespace Logger
{
	static std::wofstream m_LogStream;
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
		size_t pos = logFile.find_last_of(L"\\/");
		if (pos != std::wstring::npos)
		{
			std::wstring directory = logFile.substr(0, pos);
			if (CreateDirectoryW(directory.c_str(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				std::wcout << L"[INFO] Directory ready: " << directory << L"\n";
			}
			else
			{
				std::wcerr << L"[ERROR] Failed to create directory: " << directory << L" (Error: " << GetLastError() << L")\n";
			}
		}

		m_LogStream.open(logFile, std::ios::out | std::ios::trunc);
		if (!m_LogStream.is_open())
		{
			std::wcerr << L"[ERROR] Failed to open log file: " << logFile << L"\n";
			return;
		}
		m_LogStream << L"Log started\n";
		m_LogStream.flush();
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
		if (m_LogStream.is_open())
		{
			m_LogStream << wmessage;
			m_LogStream.flush();
		}
	}

	void Flush()
	{
		std::lock_guard<std::mutex> lock(m_LogMutex);
		if (m_LogStream.is_open())
		{
			m_LogStream.flush();
		}
		std::wcout.flush();
	}

	void Shutdown()
	{
		std::lock_guard<std::mutex> lock(m_LogMutex);
		if (m_LogStream.is_open())
		{
			m_LogStream.flush();
			m_LogStream.close();
		}
	}
}


