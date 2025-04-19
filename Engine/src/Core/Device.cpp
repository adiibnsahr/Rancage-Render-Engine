// File: src/Core/Device.cpp
#include "Core/Device.h"
#include <dxgi1_6.h>

Device::Device(std::shared_ptr<ILogger> logger) : logger_(std::move(logger)) {}

bool Device::Initialize(IDXGIAdapter* adapter) {
    // Aktifkan lapisan debug DX12
#ifdef _DEBUG
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();
        logger_->Log(LogLevel::Info, "Lapisan debug DX12 diaktifkan");
    }
#endif

    // Buat factory DXGI
    ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&factory)))) {
        logger_->Log(LogLevel::Error, "Gagal membuat factory DXGI");
        return false;
    }

    // Buat perangkat DX12
    HRESULT hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device_));
    if (FAILED(hr)) {
        logger_->Log(LogLevel::Error, "Gagal membuat perangkat DX12");
        return false;
    }

    logger_->Log(LogLevel::Info, "Perangkat DX12 berhasil diinisialisasi");
    return true;
}