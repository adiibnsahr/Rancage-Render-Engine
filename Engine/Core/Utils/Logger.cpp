#include "Logger.h"
#include <fstream>
#include <iostream>

std::string Logger::m_LogFile;

void Logger::Init(const std::string& logFile)
{
	m_LogFile = logFile;
	std::ofstream file(logFile, std::ios::out | std::ios::trunc);
	file << "Log started\n";
	file.close();
}

void Logger::Log(LogLevel level, const std::string& message)
{
	std::string prefix;
	switch (level)
	{
		case LogLevel::Info: prefix = "[INFO]";
			break;
		case LogLevel::Warning: prefix = "[WARNING]";
			break;
		case LogLevel::Error: prefix = "[ERROR]";
			break;
		default:
			break;
	}
	std::string fullMessage = prefix + " " + message + "\n";
	std::cout << fullMessage;
	std::ofstream file(m_LogFile, std::ios::app);
	file << fullMessage;
	file.close();
}
