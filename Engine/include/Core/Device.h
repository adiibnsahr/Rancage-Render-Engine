// File: include/Core/Device.h
#pragma once
#include "IRenderContext.h"
#include "Logger/ILogger.h"
#include <wrl.h>
#include <memory>

using Microsoft::WRL::ComPtr;

/// @brief Mengelola perangkat DirectX 12 dan sumber daya inti.
/// @details Bertanggung jawab untuk menginisialisasi perangkat D3D12 dan mengaktifkan lapisan debug.
class Device : public IRenderContext {
public:
    /// @brief Konstruktor untuk Device.
    /// @param logger Pointer bersama ke logger untuk debugging.
    explicit Device(std::shared_ptr<ILogger> logger);

    /// @brief Menginisialisasi perangkat DX12 dengan adapter yang ditentukan.
    /// @param adapter Adapter DXGI yang digunakan (nullptr untuk default).
    /// @return True jika inisialisasi berhasil, false jika gagal.
    bool Initialize(IDXGIAdapter* adapter = nullptr);

    /// @brief Mengembalikan pointer ke perangkat D3D12.
    /// @return Pointer ke perangkat D3D12.
    ID3D12Device* GetDevice() const override { return device_.Get(); }

private:
    ComPtr<ID3D12Device> device_;
    std::shared_ptr<ILogger> logger_;
};