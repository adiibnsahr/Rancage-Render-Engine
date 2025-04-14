#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <vector>    // Pastikan ada
#include <cstdint>   // Tambah buat uint8_t
#include "DescriptorHeap.h"

using Microsoft::WRL::ComPtr;

class Texture {
public:
    Texture() = default;
    ~Texture() = default;

    bool Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DescriptorHeap& srvHeap, const std::string& filepath);
    ComPtr<ID3D12Resource> GetTexture() const { return m_Texture; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRV() const { return m_SRV; }

private:
    bool LoadTextureFromFile(const std::string& filepath);
    bool CreateTextureResource(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    void CreateShaderResourceView(ID3D12Device* device, DescriptorHeap& srvHeap);

    ComPtr<ID3D12Resource> m_Texture;
    ComPtr<ID3D12Resource> m_UploadHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_SRV;
    std::vector<uint8_t> m_TextureData; // Pastikan pake std::vector
    uint32_t m_Width = 0;
    uint32_t m_Height = 0;
    uint32_t m_BytesPerPixel = 4; // Misal RGBA 8-bit per channel
};