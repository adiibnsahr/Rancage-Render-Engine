// File: include/Core/IRenderContext.h
#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>

/// @brief Antarmuka untuk konteks rendering, mengabstraksi API grafis.
/// @details Menyediakan akses ke perangkat grafis dan sumber daya inti.
class IRenderContext {
public:
    virtual ~IRenderContext() = default;

    /// @brief Mengembalikan pointer ke perangkat D3D12.
    /// @return Pointer ke perangkat D3D12.
    virtual ID3D12Device* GetDevice() const = 0;
};