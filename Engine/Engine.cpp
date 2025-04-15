#include "../Core/include/Core/Debug.h"
#include "../Core/include/Utils/Logger.h"

int main() 
{
    Logger::Init(L"Logs/engine.log");
    Debug::Initialize();
    Logger::Shutdown();

    return 0;
}