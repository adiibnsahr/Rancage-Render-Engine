#include "../Core/include/Core/Debug.h"
#include "../Core/include/Utils/Logger.h"
#include <comdef.h>
#include <wrl/client.h>

namespace Debug
{
    bool Initialize()
    {
        Logger::Log(LogLevel::Info, "DX12 debug layer disabled for stability");
        return true;
    }

    void SetupInfoQueue()
    {
        Logger::Log(LogLevel::Info, "Info queue setup skipped for stability");
    }
}