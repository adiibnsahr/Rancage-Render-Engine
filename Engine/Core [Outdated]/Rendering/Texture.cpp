#include "Texture.h"
#include <vector>
#include <cstdint> // Tambah buat uint8_t

bool Texture::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DescriptorHeap& srvHeap, const std::string& filepath) {
    // Load texture dari file
    if (!LoadTextureFromFile(filepath)) {
        return false;
    }

    // Buat texture resource
    if (!CreateTextureResource(device, commandList)) {
        return false;
    }

    // Buat SRV
    CreateShaderResourceView(device, srvHeap);

    return true;
}

bool Texture::LoadTextureFromFile(const std::string& filepath) {
    // Untuk sekarang, kita simulasi texture 2x2 pixel (RGBA)
    m_Width = 2;
    m_Height = 2;
    m_BytesPerPixel = 4; // RGBA, 8-bit per channel

    // Inisialisasi m_TextureData pake push_back biar lebih compatible
    m_TextureData.clear();
    uint8_t tempData[] = {
        255, 0, 0, 255,    // Merah
        0, 255, 0, 255,    // Hijau
        0, 0, 255, 255,    // Biru
        255, 255, 0, 255   // Kuning
    };
    for (int i = 0; i < 16; i++) {
        m_TextureData.push_back(tempData[i]);
    }

    return true;
}

bool Texture::CreateTextureResource(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
    // Buat texture resource
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.Width = m_Width;
    textureDesc.Height = m_Height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_Texture));
    if (FAILED(hr)) {
        return false;
    }

    // Hitung ukuran upload buffer secara manual
    const uint64_t uploadBufferSize = m_Width * m_Height * m_BytesPerPixel;
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC uploadDesc = {};
    uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    uploadDesc.Width = uploadBufferSize;
    uploadDesc.Height = 1;
    uploadDesc.DepthOrArraySize = 1;
    uploadDesc.MipLevels = 1;
    uploadDesc.Format = DXGI_FORMAT_UNKNOWN;
    uploadDesc.SampleDesc.Count = 1;
    uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_UploadHeap));
    if (FAILED(hr)) {
        return false;
    }

    // Copy data ke upload heap
    void* mappedData = nullptr;
    hr = m_UploadHeap->Map(0, nullptr, &mappedData);
    if (FAILED(hr)) {
        return false;
    }
    std::memcpy(mappedData, m_TextureData.data(), uploadBufferSize);
    m_UploadHeap->Unmap(0, nullptr);

    // Copy dari upload heap ke texture
    D3D12_TEXTURE_COPY_LOCATION src = {};
    src.pResource = m_UploadHeap.Get();
    src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src.PlacedFootprint.Offset = 0;
    src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    src.PlacedFootprint.Footprint.Width = m_Width;
    src.PlacedFootprint.Footprint.Height = m_Height;
    src.PlacedFootprint.Footprint.Depth = 1;
    src.PlacedFootprint.Footprint.RowPitch = m_Width * m_BytesPerPixel;

    D3D12_TEXTURE_COPY_LOCATION dst = {};
    dst.pResource = m_Texture.Get();
    dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = 0;

    commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    // Transition texture ke state SHADER_RESOURCE
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_Texture.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    commandList->ResourceBarrier(1, &barrier);

    return true;
}

void Texture::CreateShaderResourceView(ID3D12Device* device, DescriptorHeap& srvHeap) {
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    m_SRV = srvHeap.Allocate();
    device->CreateShaderResourceView(m_Texture.Get(), &srvDesc, m_SRV);
}