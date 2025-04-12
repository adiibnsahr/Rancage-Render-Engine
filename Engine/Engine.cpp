#include "Core/Utils/Debug.h"
#include "Core/Utils/Logger.h"
#include "Core/Rendering/DeviceContext.h"
#include "Core/Utils/StringUtils.h"

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

    auto adapter = deviceContext.GetAdapter();
    if (adapter)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        std::wstring adapterName(desc.Description);
        Logger::Log(LogLevel::Info, "Using adapter: " + WStringToString(desc.Description));
    }

    auto device = deviceContext.GetDevice();
    if (device)
    {
        Logger::Log(LogLevel::Info, "DX12 device created successfully");
    }

    auto commandQueue = deviceContext.GetCommandQueue();
    if (commandQueue)
    {
        Logger::Log(LogLevel::Info, "Command queue ready");
    }

    Logger::Log(LogLevel::Info, "Engine started");
    return 0;
}
