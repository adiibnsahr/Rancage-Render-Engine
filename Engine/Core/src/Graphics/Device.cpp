#include "../Core/include/Graphics/Device.h"
#include "../Core/include/Utils/Logger.h"
#include "../Core/include/Math/Matrix4.h"
#include "../Core/include/Math/Vector3.h"
#include <comdef.h>
#include <d3dx12.h>

namespace Graphics
{
    Device::Device() : m_FenceValue(0), m_FenceEvent(nullptr), m_DepthBuffer(1280, 720)
    {
    }

    bool Device::Initialize(HWND hwnd, UINT width, UINT height)
    {
        HRESULT hr;

        // Create DXGI factory
        hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_Factory));
        if (FAILED(hr))
        {
            Logger::Log(LogLevel::Error, "Failed to create DXGI factory");
            return false;
        }
        Logger::Log(LogLevel::Info, "DXGI factory created");

        // Enumerate adapters and select the best one
        Microsoft::WRL::ComPtr<IDXGIAdapter4> selectedAdapter;
        Microsoft::WRL::ComPtr<IDXGIAdapter4> tempAdapter;
        UINT64 maxDedicatedVideoMemory = 0;
        for (UINT i = 0; m_Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tempAdapter)) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC3 desc;
            tempAdapter->GetDesc3(&desc);
            Logger::Log(LogLevel::Info, "Found adapter: %S, Dedicated VRAM: %llu MB", desc.Description, desc.DedicatedVideoMemory / (1024 * 1024));
            if (desc.DedicatedVideoMemory > maxDedicatedVideoMemory)
            {
                maxDedicatedVideoMemory = desc.DedicatedVideoMemory;
                selectedAdapter = tempAdapter;
            }
        }

        if (!selectedAdapter)
        {
            Logger::Log(LogLevel::Error, "No suitable adapter found");
            return false;
        }

        DXGI_ADAPTER_DESC3 adapterDesc;
        selectedAdapter->GetDesc3(&adapterDesc);
        Logger::Log(LogLevel::Info, "Selected adapter: %S", adapterDesc.Description);

        // Create device
        hr = D3D12CreateDevice(selectedAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
        if (FAILED(hr))
        {
            Logger::Log(LogLevel::Error, "Failed to create D3D12 device");
            return false;
        }
        Logger::Log(LogLevel::Info, "D3D12 device created");

        // Initialize command
        if (!m_Command.Initialize(m_Device.Get()))
        {
            Logger::Log(LogLevel::Error, "Failed to initialize command");
            return false;
        }

        // Create swap chain
        if (!m_SwapChain.Initialize(m_Device.Get(), m_Command.GetQueue(), hwnd, width, height))
        {
            Logger::Log(LogLevel::Error, "Failed to initialize swap chain");
            return false;
        }

        // Create RTV descriptor heap
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = 2;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        hr = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTVHeap));
        if (FAILED(hr))
        {
            Logger::Log(LogLevel::Error, "Failed to create RTV descriptor heap");
            return false;
        }
        Logger::Log(LogLevel::Info, "RTV descriptor heap created");

        // Create render target views
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < 2; ++i)
        {
            Microsoft::WRL::ComPtr<ID3D12Resource> renderTarget;
            hr = m_SwapChain.GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&renderTarget));
            if (FAILED(hr))
            {
                Logger::Log(LogLevel::Error, "Failed to get swap chain buffer %u", i);
                return false;
            }
            m_Device->CreateRenderTargetView(renderTarget.Get(), nullptr, rtvHandle);
            rtvHandle.ptr += m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            Logger::Log(LogLevel::Info, "Render target view created for buffer %u", i);
        }

        // Create depth buffer
        if (!m_DepthBuffer.Create(m_Device.Get()))
        {
            Logger::Log(LogLevel::Error, "Failed to create depth buffer");
            return false;
        }
        // Create DSV
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
        hr = m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));
        if (FAILED(hr))
        {
            Logger::Log(LogLevel::Error, "Failed to create DSV descriptor heap");
            return false;
        }
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        m_Device->CreateDepthStencilView(m_DepthBuffer.GetResource(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
        m_DepthBuffer.SetDSV(dsvHeap->GetCPUDescriptorHandleForHeapStart());
        Logger::Log(LogLevel::Info, "DSV created");

        // Create fence
        hr = m_Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
        if (FAILED(hr))
        {
            Logger::Log(LogLevel::Error, "Failed to create fence");
            return false;
        }
        Logger::Log(LogLevel::Info, "Fence created");

        m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!m_FenceEvent)
        {
            Logger::Log(LogLevel::Error, "Failed to create fence event");
            return false;
        }
        Logger::Log(LogLevel::Info, "Fence event created");

        // Configure root signature
        if (!m_RootSignature.Configure(m_Device.Get()))
        {
            Logger::Log(LogLevel::Error, "Failed to configure root signature");
            return false;
        }

        // Configure pipeline state
        m_PipelineState.SetRootSignature(m_RootSignature.GetRootSignature());
        if (!m_PipelineState.Configure(m_Device.Get()))
        {
            Logger::Log(LogLevel::Error, "Failed to configure pipeline state");
            return false;
        }

        // Create constant buffer
        {
            CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
            CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(256); // Cukup untuk MVP
            hr = m_Device->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &bufferDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_ConstantBuffer));
            if (FAILED(hr))
            {
                Logger::Log(LogLevel::Error, "Failed to create constant buffer");
                return false;
            }
            Logger::Log(LogLevel::Info, "Constant buffer created");
        }

        return true;
    }

    void Device::WaitForFence()
    {
        if (m_Fence->GetCompletedValue() < m_FenceValue)
        {
            m_Fence->SetEventOnCompletion(m_FenceValue, m_FenceEvent);
            WaitForSingleObject(m_FenceEvent, INFINITE);
        }
    }

    void Device::SignalFence(ID3D12CommandQueue* queue)
    {
        m_FenceValue++;
        queue->Signal(m_Fence.Get(), m_FenceValue);
    }

    void Device::UpdateConstantBuffer(const Math::Matrix4& modelMatrix, const Math::Matrix4& viewMatrix, const Math::Matrix4& projMatrix)
    {
        // MVP = M * V * P
        Math::Matrix4 mvp = modelMatrix * viewMatrix * projMatrix;

        // Transpose untuk shader (HLSL expect row-major)
        Math::Matrix4 mvpTransposed = mvp.Transpose();

        // Update constant buffer
        void* mappedData;
        CD3DX12_RANGE readRange(0, 0);
        HRESULT hr = m_ConstantBuffer->Map(0, &readRange, &mappedData);
        if (FAILED(hr))
        {
            Logger::Log(LogLevel::Error, "Failed to map constant buffer");
            return;
        }
        memcpy(mappedData, mvpTransposed.m, sizeof(float) * 16);
        m_ConstantBuffer->Unmap(0, nullptr);

        // Log MVP matrix
        Logger::Log(LogLevel::Info, "MVP Matrix (transposed): [%.2f, %.2f, %.2f, %.2f]",
            mvpTransposed.m[0][0], mvpTransposed.m[0][1], mvpTransposed.m[0][2], mvpTransposed.m[0][3]);
        Logger::Log(LogLevel::Info, "                        [%.2f, %.2f, %.2f, %.2f]",
            mvpTransposed.m[1][0], mvpTransposed.m[1][1], mvpTransposed.m[1][2], mvpTransposed.m[1][3]);
        Logger::Log(LogLevel::Info, "                        [%.2f, %.2f, %.2f, %.2f]",
            mvpTransposed.m[2][0], mvpTransposed.m[2][1], mvpTransposed.m[2][2], mvpTransposed.m[2][3]);
        Logger::Log(LogLevel::Info, "                        [%.2f, %.2f, %.2f, %.2f]",
            mvpTransposed.m[3][0], mvpTransposed.m[3][1], mvpTransposed.m[3][2], mvpTransposed.m[3][3]);
        Logger::Log(LogLevel::Info, "Constant buffer updated with camera matrices");
    }
}