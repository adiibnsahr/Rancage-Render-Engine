#pragma once
#include <string>

enum class LogLevel { Info, Warning, Error };

namespace Logger
{
	void Init(const std::wstring& logFile);
	void Log(LogLevel level, const char* format, ...);
	void Flush();
	void Shutdown();
}
