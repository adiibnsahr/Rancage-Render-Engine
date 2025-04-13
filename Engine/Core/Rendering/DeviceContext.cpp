#include "DeviceContext.h"
#include <Windows.h> // Buat WideCharToMultiByte
#include "../Utils/StringUtils.h"
#include "../Utils/Logger.h"
#include "../Utils/D3DUtils.h"

bool DeviceContext::Initialize(HWND hwnd, UINT width, UINT height) {
    HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_Factory));
    if (FAILED(hr)) {
        Logger::Log(LogLevel::Error, "Failed to create DXGI factory: HRESULT " + std::to_string(hr));
        return false;
    }
    Logger::Log(LogLevel::Info, "DXGI factory created");

    if (!QueryAdapter()) {
        Logger::Log(LogLevel::Error, "Failed to query adapters");
        return false;
    }
    Logger::Log(LogLevel::Info, "Adapter selected");

    if (!CreateDevice())
    {
        Logger::Log(LogLevel::Error, "Failed to create device");
        return false;
    }
    Logger::Log(LogLevel::Info, "Device created");

    if (!m_CommandQueue.Initialize(m_Device, m_PipelineState.GetPipelineState()))
    {
        Logger::Log(LogLevel::Error, "Failed to initialize command queue");
        return false;
    }
    Logger::Log(LogLevel::Info, "Command queue initialized");

    if (!m_SwapChain.Initialize(hwnd, m_Factory, m_CommandQueue.GetQueue(), width, height))
    {
        Logger::Log(LogLevel::Error, "Failed to initialize swap chain");
        return false;
    }
    Logger::Log(LogLevel::Info, "Swap chain initialized");

    if (!m_RTVHeap.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2))
    {
        Logger::Log(LogLevel::Error, "Failed to initialize RTV descriptor heap");
        return false;
    }
    Logger::Log(LogLevel::Info, "RTV Descriptor heap initialized");

    if (!m_DSVHeap.Initialize(m_Device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1))
    {
        Logger::Log(LogLevel::Error, "Failed to initialize DSV descriptor heap");
        return false;
    }
    Logger::Log(LogLevel::Info, "DSV Descriptor heap initialized");

    if (!m_DepthBuffer.Initialize(m_Device, m_DSVHeap, width, height))
    {
        Logger::Log(LogLevel::Error, "Failed to initialize depth buffer");
        return false;
    }
    Logger::Log(LogLevel::Info, "Depth buffer initialized");

    if (!m_RootSignature.Initialize(m_Device))
    {
        Logger::Log(LogLevel::Error, "Failed to initialize root signature");
        return false;
    }
    Logger::Log(LogLevel::Info, "Root signature initialized");

    if (!m_PipelineState.Initialize(m_Device, m_RootSignature.GetRootSignature()))
    {
        Logger::Log(LogLevel::Error, "Failed to initialize pipeline state");
        return false;
    }
    Logger::Log(LogLevel::Info, "Pipeline state initialized");

    return true;
}

bool DeviceContext::QueryAdapter() {
    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<IDXGIAdapter1> bestAdapter;
    SIZE_T maxVRAM = 0;

    for (UINT i = 0; m_Factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        // Skip software adapters
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            Logger::Log(LogLevel::Info, "Skipping software adapter: " + WStringToString(desc.Description));
            continue;
        }

        // Log adapter info
        SIZE_T vramMB = desc.DedicatedVideoMemory / (1024 * 1024);
        Logger::Log(LogLevel::Info, "Found adapter: " + WStringToString(desc.Description) +
            ", VRAM: " + std::to_string(vramMB) + " MB");

        // Pilih adapter dengan VRAM terbesar
        if (vramMB > maxVRAM) {
            maxVRAM = vramMB;
            bestAdapter = adapter;
        }
    }

    if (!bestAdapter) {
        Logger::Log(LogLevel::Error, "No suitable DX12 adapter found");
        return false;
    }

    m_Adapter = bestAdapter;
    DXGI_ADAPTER_DESC1 desc;
    m_Adapter->GetDesc1(&desc);
    Logger::Log(LogLevel::Info, "Selected adapter: " + WStringToString(desc.Description) +
        ", VRAM: " + std::to_string(desc.DedicatedVideoMemory / (1024 * 1024)) + " MB");

    return true;
}

bool DeviceContext::CreateDevice()
{
    if (!m_Adapter)
    {
        Logger::Log(LogLevel::Error, "Adapter is null before creating device");
        return false;
    }

    Logger::Log(LogLevel::Info, "Attempting to create DX12 device...");

    HRESULT hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));
    if (FAILED(hr))
    {
        Logger::Log(LogLevel::Error, "Failed to create DX12 device: HRESULT " + std::to_string(hr));
        return false;
    }

    D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevels = {};
    const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_12_1 };
    featureLevels.NumFeatureLevels = _countof(levels);
    featureLevels.pFeatureLevelsRequested = levels;

    Logger::Log(LogLevel::Info, "Checking feature support...");
    hr = m_Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevels, sizeof(featureLevels));

    if (SUCCEEDED(hr))
    {
        Logger::Log(LogLevel::Info, "Max supported feature level: " +
                    FeatureLevelToString(featureLevels.MaxSupportedFeatureLevel));
    }
    else
    {
        Logger::Log(LogLevel::Warning, "Failed to check feature levels: HRESULT " + std::to_string(hr));
    }

    return true;
}
