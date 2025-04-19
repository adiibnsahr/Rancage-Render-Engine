// File: include/Logger/Logger.h
#pragma once
#include "Logger/ILogger.h"

/// @brief Logger sederhana untuk debugging (sinkron).
/// @details Mencatat pesan ke console; akan diupgrade ke async di langkah berikutnya.
class Logger : public ILogger {
public:
    /// @brief Konstruktor untuk Logger.
    Logger() = default;

    /// @brief Mencatat pesan dengan tingkat keparahan tertentu.
    /// @param level Tingkat keparahan log.
    /// @param message Pesan yang akan dicatat.
    void Log(LogLevel level, const std::string& message) override;

private:
    /// @brief Mengonversi tingkat keparahan ke string untuk output.
    /// @param level Tingkat keparahan.
    /// @return Representasi string dari tingkat keparahan.
    std::string LevelToString(LogLevel level) const;
};