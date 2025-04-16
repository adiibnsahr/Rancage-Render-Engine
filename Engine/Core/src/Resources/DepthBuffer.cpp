#include "../Core/include/Resources/DepthBuffer.h"
#include "../Core/include/Utils/Logger.h"
#include <d3dx12.h>
#include <comdef.h>

namespace Graphics
{
    DepthBuffer::DepthBuffer(UINT width, UINT height) : m_Width(width), m_Height(height)
    {
    }

    DepthBuffer::~DepthBuffer()
    {
        Release();
    }

    bool DepthBuffer::Create(ID3D12Device* device)
    {
        if (!device)
        {
            Logger::Log(LogLevel::Error, "Device is null in DepthBuffer::Create");
            return false;
        }

        // Create depth/stencil buffer
        D3D12_RESOURCE_DESC depthDesc = {};
        depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthDesc.Width = m_Width;
        depthDesc.Height = m_Height;
        depthDesc.DepthOrArraySize = 1;
        depthDesc.MipLevels = 1;
        depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthDesc.SampleDesc.Count = 1;
        depthDesc.SampleDesc.Quality = 0;
        depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        // Buat variable untuk heap properties
        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

        HRESULT hr = device->CreateCommittedResource(
            &heapProperties, // Gunakan variable l-value
            D3D12_HEAP_FLAG_NONE,
            &depthDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &clearValue,
            IID_PPV_ARGS(&m_DepthStencilBuffer));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create depth buffer: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "Depth buffer created");

        return true;
    }

    void DepthBuffer::Release()
    {
        if (m_DepthStencilBuffer)
        {
            m_DepthStencilBuffer.Reset();
            Logger::Log(LogLevel::Info, "Depth buffer released");
        }
    }
}