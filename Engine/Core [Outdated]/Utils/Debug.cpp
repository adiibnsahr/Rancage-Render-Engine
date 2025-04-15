#include "Debug.h"
#include "../Utils/Logger.h"
#include <comdef.h>
#include <wrl/client.h>

bool Debug::Initialize()
{
#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12Debug> debug;
    HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
    if (FAILED(hr))
    {
        Logger::Log(LogLevel::Error, "Failed to initialize DX12 debug layer: HRESULT " + std::to_string(hr));
        return false;
    }

    debug->EnableDebugLayer();
    Logger::Log(LogLevel::Info, "DX12 debug layer enabled");

    // Enable GPU-based validation dan synchronized command queue validation
    Microsoft::WRL::ComPtr<ID3D12Debug1> debug1;
    if (SUCCEEDED(debug->QueryInterface(IID_PPV_ARGS(&debug1))))
    {
        debug1->SetEnableGPUBasedValidation(true);
        debug1->SetEnableSynchronizedCommandQueueValidation(true);
        Logger::Log(LogLevel::Info, "GPU-based validation and synchronized command queue validation enabled");
    }
    else
    {
        Logger::Log(LogLevel::Warning, "Failed to enable GPU-based validation");
    }
#else
    Logger::Log(LogLevel::Info, "Debug layer skipped in release build");
#endif // _DEBUG
    return true;
}