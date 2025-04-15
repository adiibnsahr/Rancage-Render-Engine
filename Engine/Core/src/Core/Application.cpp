#include "../Core/include/Core/Application.h"
#include "../Core/include/Core/Debug.h"
#include "../Core/include/Utils/Logger.h"

namespace Core
{
	Application::Application() = default;
	Application::~Application() = default;

	bool Core::Application::Initialize()
	{
		Logger::Init(L"Logs/engine.log");
		if (!Debug::Initialize())
		{
			Logger::Log(LogLevel::Error, "Failed to initialize debug layer");
			Logger::Shutdown();
			return false;
		}

		if (!m_Device.Initialize())
		{
			Logger::Log(LogLevel::Error, "Failed to initialize device");
			Logger::Shutdown();
			return false;
		}

		return true;
	}

	void Core::Application::Run()
	{
		// Placeholder untuk main loop
		Logger::Log(LogLevel::Info, "Application running");
		Logger::Shutdown();
	}
}

int main()
{
	Core::Application app;
	if (!app.Initialize())
	{
		return 1;
	}
	app.Run();
	return 0;
}
