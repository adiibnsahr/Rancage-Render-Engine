#include "../Core/include/Graphics/Device.h"
#include "../Core/include/Utils/Logger.h"
#include <comdef.h>

namespace Graphics
{
    Device::Device() = default;
    Device::~Device() = default;

    bool Device::Initialize(HWND hwnd, UINT width, UINT height)
    {
        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_Factory));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create DXGI factory: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "DXGI factory created");

        Microsoft::WRL::ComPtr<IDXGIAdapter1> bestAdapter;
        SIZE_T maxDedicatedVideoMemory = 0;

        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(adapterIndex, &bestAdapter); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            bestAdapter->GetDesc1(&desc);
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            Logger::Log(LogLevel::Info, "Found adapter: %ls, Dedicated VRAM: %zu MB", desc.Description, desc.DedicatedVideoMemory / (1024 * 1024));

            if (desc.DedicatedVideoMemory > maxDedicatedVideoMemory)
            {
                maxDedicatedVideoMemory = desc.DedicatedVideoMemory;
                m_Adapter = bestAdapter;
            }
        }

        if (!m_Adapter)
        {
            Logger::Log(LogLevel::Error, "No suitable D3D12 hardware adapter found");
            return false;
        }

        hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create D3D12 device: %s", err.ErrorMessage());
            return false;
        }

        DXGI_ADAPTER_DESC1 desc;
        m_Adapter->GetDesc1(&desc);
        Logger::Log(LogLevel::Info, "Selected adapter: %ls", desc.Description);
        Logger::Log(LogLevel::Info, "D3D12 device created");

        if (!m_Command.Initialize(m_Device.Get()))
        {
            Logger::Log(LogLevel::Error, "Failed to initialize command");
            return false;
        }

        if (!m_SwapChain.Initialize(m_Device.Get(), m_Command.GetQueue(), hwnd, width, height))
        {
            Logger::Log(LogLevel::Error, "Failed to initialize SwapChain");
            return false;
        }

        // Create RTV Descriptor Heap
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = 2; // 2 buffers (double buffering)
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        hr = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTVHeap));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create RTV descriptor heap: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "RTV descriptor heap created");

        CreateRenderTargetViews(); // Panggil langsung setelah heap

        return true;
    }

    void Device::CreateRenderTargetViews()
    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
        UINT rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        for (UINT i = 0; i < 2; ++i)
        {
            Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
            HRESULT hr = m_SwapChain.GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
            if (FAILED(hr))
            {
                _com_error err(hr);
                Logger::Log(LogLevel::Error, "Failed to get SwapChain buffer %u: %s", i, err.ErrorMessage());
                return;
            }

            m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
            Logger::Log(LogLevel::Info, "Render target view created for buffer %u", i);
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }
}