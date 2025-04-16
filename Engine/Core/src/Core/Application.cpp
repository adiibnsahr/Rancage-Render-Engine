#include "../Core/include/Core/Application.h"
#include "../Core/include/Core/Debug.h"
#include "../Core/include/Core/Window.h"
#include "../Core/include/Utils/Logger.h"
#include <exception>
#include <chrono>

// NVIDIA Optimus hint
extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

namespace Core
{
    Application::Application() = default;
    Application::~Application()
    {
        Logger::Log(LogLevel::Info, "Application destructor called");
    }

    bool Application::Initialize()
    {
        Logger::Init(L"Logs/engine.log");
        if (!Debug::Initialize())
        {
            Logger::Log(LogLevel::Error, "Failed to initialize debug layer");
            return false;
        }

        if (!m_Window.Initialize(L"Rancage Render Engine", 1280, 720))
        {
            Logger::Log(LogLevel::Error, "Failed to initialize window");
            return false;
        }

        if (!m_Device.Initialize(m_Window.GetHandle(), 1280, 720))
        {
            Logger::Log(LogLevel::Error, "Failed to initialize device");
            return false;
        }

        return true;
    }

    void Application::Run()
    {
        Logger::Log(LogLevel::Info, "Application running");
        try
        {
            auto start = std::chrono::steady_clock::now();
            while (m_Window.ProcessMessages())
            {
                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() > 10)
                {
                    Logger::Log(LogLevel::Info, "Timeout reached, stopping loop");
                    break;
                }
                Logger::Log(LogLevel::Info, "Message loop running");
            }
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::Error, "Exception in Run: %s", e.what());
            throw;
        }
        catch (...)
        {
            Logger::Log(LogLevel::Error, "Unknown exception in Run");
            throw;
        }
        Logger::Log(LogLevel::Info, "Application stopped");
    }
}

int main()
{
    try
    {
        Core::Application app;
        if (!app.Initialize())
        {
            Logger::Log(LogLevel::Error, "Application initialization failed");
            Logger::Shutdown();
            return 1;
        }
        app.Run();
        Logger::Log(LogLevel::Info, "Application exited normally");
        Logger::Shutdown();
        return 0;
    }
    catch (const std::exception& e)
    {
        Logger::Log(LogLevel::Error, "Exception in main: %s", e.what());
        Logger::Shutdown();
        return 1;
    }
    catch (...)
    {
        Logger::Log(LogLevel::Error, "Unknown exception in main");
        Logger::Shutdown();
        return 1;
    }
}