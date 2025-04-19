// File: src/Main.cpp
#include "Core/Device.h"
#include "Logger/Logger.h"
#include <memory>

int main() {
    // Inisialisasi logger
    auto logger = std::make_shared<Logger>();

    // Inisialisasi device
    Device device(logger);
    if (!device.Initialize()) {
        logger->Log(LogLevel::Fatal, "Gagal menginisialisasi render engine");
        return 1;
    }

    logger->Log(LogLevel::Info, "Render engine berhasil diinisialisasi");
    return 0;
}