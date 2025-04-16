#include "../Core/include/Core/Application.h"
#include "../Core/include/Core/Debug.h"
#include "../Core/include/Core/Window.h"
#include "../Core/include/Utils/Logger.h"
#include "../Core/include/Math/Vector3.h"
#include "../Core/include/Graphics/Triangle.h"
#include <exception>
#include <chrono>
#include <comdef.h>
#include <d3dx12.h>

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
        Logger::Init(L"D:\\Projects\\Visual Studio\\Rancage Render Engine\\Logs\\engine.log");
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

        // Tambah segitiga ke renderables
        m_Renderables.push_back(std::make_unique<Graphics::Triangle>(m_Device.GetDevice()));
        Logger::Log(LogLevel::Info, "Renderables initialized");

        return true;
    }

    void Application::Run()
    {
        Logger::Log(LogLevel::Info, "Application running");
        try
        {
            auto start = std::chrono::steady_clock::now();
            auto lastTime = start;
            while (m_Window.ProcessMessages())
            {
                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() > 10)
                {
                    Logger::Log(LogLevel::Info, "Timeout reached, stopping loop");
                    break;
                }

                // Hitung delta time (detik)
                float deltaTime = std::chrono::duration<float>(now - lastTime).count();
                lastTime = now;

                // Update renderables
                for (const auto& renderable : m_Renderables)
                {
                    renderable->Update(deltaTime);
                }

                // Wait for previous frame
                m_Device.WaitForFence();

                // Get current back buffer index
                UINT backBufferIndex = m_Device.GetSwapChain().GetSwapChain()->GetCurrentBackBufferIndex();

                // Reset command allocator and list
                HRESULT hr = m_Device.GetCommand().GetAllocator()->Reset();
                if (FAILED(hr))
                {
                    _com_error err(hr);
                    Logger::Log(LogLevel::Error, "Failed to reset command allocator: %s", err.ErrorMessage());
                    break;
                }

                hr = m_Device.GetCommand().GetList()->Reset(m_Device.GetCommand().GetAllocator(), m_Device.GetPipelineState().GetPipelineState());
                if (FAILED(hr))
                {
                    _com_error err(hr);
                    Logger::Log(LogLevel::Error, "Failed to reset command list: %s", err.ErrorMessage());
                    break;
                }

                // Set pipeline state
                m_Device.GetCommand().GetList()->SetPipelineState(m_Device.GetPipelineState().GetPipelineState());
                m_Device.GetCommand().GetList()->SetGraphicsRootSignature(m_Device.GetRootSignature().GetRootSignature());

                // Update and set constant buffer
                m_Device.UpdateConstantBuffer(m_Renderables[0]->GetModelMatrix());
                m_Device.GetCommand().GetList()->SetGraphicsRootConstantBufferView(0, m_Device.GetConstantBuffer()->GetGPUVirtualAddress());

                // Clear RTV
                D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_Device.GetRTVHeap()->GetCPUDescriptorHandleForHeapStart();
                rtvHandle.ptr += backBufferIndex * m_Device.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
                m_Device.GetCommand().GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
                Logger::Log(LogLevel::Info, "Clearing back buffer %u to blue", backBufferIndex);

                // Clear DSV
                m_Device.GetCommand().GetList()->ClearDepthStencilView(
                    m_Device.GetDepthBuffer().GetDSV(),
                    D3D12_CLEAR_FLAG_DEPTH,
                    1.0f,
                    0,
                    0,
                    nullptr);
                Logger::Log(LogLevel::Info, "Clearing depth buffer");

                // Set render targets
                D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_Device.GetDepthBuffer().GetDSV();
                m_Device.GetCommand().GetList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

                // Set viewport and scissor rect
                D3D12_VIEWPORT viewport = { 0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f };
                m_Device.GetCommand().GetList()->RSSetViewports(1, &viewport);
                D3D12_RECT scissorRect = { 0, 0, 1280, 720 };
                m_Device.GetCommand().GetList()->RSSetScissorRects(1, &scissorRect);
                Logger::Log(LogLevel::Info, "Viewport and scissor rect set");

                // Render semua renderables
                for (const auto& renderable : m_Renderables)
                {
                    renderable->Render(m_Device.GetCommand().GetList());
                }

                // Close command list
                hr = m_Device.GetCommand().GetList()->Close();
                if (FAILED(hr))
                {
                    _com_error err(hr);
                    Logger::Log(LogLevel::Error, "Failed to close command list: %s", err.ErrorMessage());
                    break;
                }

                // Execute command list
                ID3D12CommandList* commandLists[] = { m_Device.GetCommand().GetList() };
                m_Device.GetCommand().GetQueue()->ExecuteCommandLists(1, commandLists);

                // Signal fence
                m_Device.SignalFence(m_Device.GetCommand().GetQueue());

                // Present
                hr = m_Device.GetSwapChain().GetSwapChain()->Present(1, 0);
                if (FAILED(hr))
                {
                    _com_error err(hr);
                    Logger::Log(LogLevel::Error, "Failed to present SwapChain: %s", err.ErrorMessage());
                    break;
                }
                Logger::Log(LogLevel::Info, "Presented back buffer %u", backBufferIndex);
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