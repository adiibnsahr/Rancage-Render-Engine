#pragma once
#include "Window.h"
#include "../Graphics/Device.h"
#include "../Core/include/Math/Matrix4.h"
#include "../Interfaces/IRenderable.h"
#include "../Core/include/Graphics/Triangle.h"
#include "../Core/include/Graphics/Camera.h" // Tambah untuk kamera
#include <memory>
#include <vector>

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
        Window m_Window;
        Graphics::Device m_Device;
        std::vector<std::unique_ptr<Graphics::IRenderable>> m_Renderables;
        std::unique_ptr<Graphics::Camera> m_Camera; // Tambah kamera
    };
}