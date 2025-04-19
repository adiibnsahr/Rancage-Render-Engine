// File: include/Logger/ILogger.h
#pragma once
#include <string>

/// @brief Tingkat keparahan log.
enum class LogLevel {
    Trace, Debug, Info, Warning, Error, Fatal
};

/// @brief Antarmuka untuk sistem logging.
/// @details Menyediakan metode untuk mencatat pesan dengan tingkat keparahan tertentu.
class ILogger {
public:
    virtual ~ILogger() = default;

    /// @brief Mencatat pesan dengan tingkat keparahan tertentu.
    /// @param level Tingkat keparahan log.
    /// @param message Pesan yang akan dicatat.
    virtual void Log(LogLevel level, const std::string& message) = 0;
};