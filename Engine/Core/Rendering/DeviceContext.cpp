#include "DeviceContext.h"
#include <Windows.h> // Buat WideCharToMultiByte
#include "../Utils/StringUtils.h"

bool DeviceContext::Initialize() {
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