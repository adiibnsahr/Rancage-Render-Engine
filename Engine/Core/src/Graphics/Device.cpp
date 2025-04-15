#include "../Core/include/Graphics/Device.h"
#include "../Core/include/Utils/Logger.h"
#include <comdef.h>

namespace Graphics
{
    Device::Device() = default;
    Device::~Device() = default;

    bool Device::Initialize()
    {
        // Langkah 2: Create DXGI Factory
        HRESULT hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_Factory));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create DXGI factory: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "DXGI factory created");

        // Langkah 3: Query Adapters, pilih yang VRAM tertinggi
        Microsoft::WRL::ComPtr<IDXGIAdapter1> bestAdapter;
        SIZE_T maxDedicatedVideoMemory = 0;

        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(adapterIndex, &bestAdapter); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            bestAdapter->GetDesc1(&desc);
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue; // Skip software adapters
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

        // Langkah 4: Create Device
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
        return true;
    }
}


