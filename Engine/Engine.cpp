#include "Core/Utils/Debug.h"
#include "Core/Utils/Logger.h"
#include "Core/Rendering/DeviceContext.h"

int main()
{
    Logger::Init("Logs/debug.log");
    if (!Debug::Initialize())
    {
        Logger::Log(LogLevel::Error, "Initialize failed");
        return 1;
    }

    DeviceContext deviceContext;
    if (!deviceContext.Initialize())
    {
        Logger::Log(LogLevel::Error, "Device context initialization failed");
        return 1;
    }

    Logger::Log(LogLevel::Info, "Engine started");
    return 0;
}
