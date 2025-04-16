#pragma once
#include "../Core/include/Graphics/Device.h"
#include "../Core/include/Core/Window.h"

namespace Core
{
    class Application
    {
    public:
        Application();
        ~Application();

        bool Initialize();
        void Run();

    private:
        Graphics::Device m_Device;
        Window m_Window;
    };
}