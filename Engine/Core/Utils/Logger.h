#pragma once
#include <string>
#include <iostream>


enum class LogLevel { Info, Warning, Error };

class Logger
{
public:
	static void Init(const std::string& logFile);
	static void Log(LogLevel level, const std::string& message);

private:
	static std::string m_LogFile;
};

