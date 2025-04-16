#include "../Core/include/Graphics/Device.h"
#include "../Core/include/Utils/Logger.h"
#include "../Core/include/Math/Matrix4.h"
#include <comdef.h>
#include <d3dx12.h>

namespace Graphics
{
    Device::Device() : m_DepthBuffer(1280, 720)
    {
    }

    Device::~Device()
    {
        if (m_FenceEvent)
        {
            CloseHandle(m_FenceEvent);
            m_FenceEvent = nullptr;
        }
    }

    bool Device::Initialize(HWND hwnd, UINT width, UINT height)
    {
        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_Factory));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create DXGI factory: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "DXGI factory created");

        Microsoft::WRL::ComPtr<IDXGIAdapter1> bestAdapter;
        SIZE_T maxDedicatedVideoMemory = 0;

        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(adapterIndex, &bestAdapter); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            bestAdapter->GetDesc1(&desc);
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            Logger::Log(LogLevel::Info, "Found adapter: %ls, Dedicated VRAM: %zu MB", desc.Description, desc.DedicatedVideoMemory / (1024 * 1024));

            if (desc.DedicatedVideoMemory > maxDedicatedVideoMemory)
            {
                maxDedicatedVideoMemory = desc.DedicatedVideoMemory;
                m_Adapter = bestAdapter;
            }
        }

        if (!m_Adapter)
        {
            Logger::Log(LogLevel::Error, "No suitable D3D12 hardware adapter found");
            return false;
        }

        hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create D3D12 device: %s", err.ErrorMessage());
            return false;
        }

        DXGI_ADAPTER_DESC1 desc;
        m_Adapter->GetDesc1(&desc);
        Logger::Log(LogLevel::Info, "Selected adapter: %ls", desc.Description);
        Logger::Log(LogLevel::Info, "D3D12 device created");

        if (!m_Command.Initialize(m_Device.Get()))
        {
            Logger::Log(LogLevel::Error, "Failed to initialize command");
            return false;
        }

        if (!m_SwapChain.Initialize(m_Device.Get(), m_Command.GetQueue(), hwnd, width, height))
        {
            Logger::Log(LogLevel::Error, "Failed to initialize SwapChain");
            return false;
        }

        // Create RTV Descriptor Heap
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = 2;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        hr = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTVHeap));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create RTV descriptor heap: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "RTV descriptor heap created");

        // Create DSV Descriptor Heap
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        hr = m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create DSV descriptor heap: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "DSV descriptor heap created");

        // Create Depth Buffer
        if (!m_DepthBuffer.Create(m_Device.Get()))
        {
            Logger::Log(LogLevel::Error, "Failed to create depth buffer");
            return false;
        }

        // Create Root Signature
        if (!m_RootSignature.Configure(m_Device.Get()))
        {
            Logger::Log(LogLevel::Error, "Failed to create root signature");
            return false;
        }

        // Create Pipeline State
        m_PipelineState.SetRootSignature(m_RootSignature.GetRootSignature());
        if (!m_PipelineState.Configure(m_Device.Get()))
        {
            Logger::Log(LogLevel::Error, "Failed to create pipeline state");
            return false;
        }

        // Create Constant Buffer
        CreateConstantBuffer();

        // Create Fence
        hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create fence: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "Fence created");

        m_FenceValue = 1;
        m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!m_FenceEvent)
        {
            Logger::Log(LogLevel::Error, "Failed to create fence event");
            return false;
        }
        Logger::Log(LogLevel::Info, "Fence event created");

        CreateRenderTargetViews();
        CreateDepthStencilView();
        return true;
    }

    void Device::CreateRenderTargetViews()
    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
        UINT rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        for (UINT i = 0; i < 2; ++i)
        {
            Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
            HRESULT hr = m_SwapChain.GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
            if (FAILED(hr))
            {
                _com_error err(hr);
                Logger::Log(LogLevel::Error, "Failed to get SwapChain buffer %u: %s", i, err.ErrorMessage());
                return;
            }

            m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
            Logger::Log(LogLevel::Info, "Render target view created for buffer %u", i);
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    void Device::CreateDepthStencilView()
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;

        D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
        m_Device->CreateDepthStencilView(m_DepthBuffer.GetResource(), &dsvDesc, dsvHandle);
        m_DepthBuffer.SetDSV(dsvHandle); // Set via setter
        Logger::Log(LogLevel::Info, "Depth stencil view created");
    }

    void Device::CreateConstantBuffer()
    {
        CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(256); // Align to 256 bytes
        HRESULT hr = m_Device->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_ConstantBuffer));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create constant buffer: %s", err.ErrorMessage());
            return;
        }
        Logger::Log(LogLevel::Info, "Constant buffer created");
    }

    void Device::UpdateConstantBuffer()
    {
        // Model matrix (identity)
        Math::Matrix4 model = Math::Matrix4::Identity();

        // View matrix: kamera di z=2, ngeliat ke origin
        Math::Matrix4 view = Math::Matrix4::CreateLookAt(
            Math::Vector3(0.0f, 0.0f, 2.0f), // Lebih deket dari -5
            Math::Vector3(0.0f, 0.0f, 0.0f),
            Math::Vector3(0.0f, 1.0f, 0.0f));

        // Projection matrix: 60-degree FOV
        Math::Matrix4 proj = Math::Matrix4::CreatePerspective(
            3.14159f / 3.0f, // 60 degrees
            1280.0f / 720.0f,
            0.1f,
            10.0f); // Far plane lebih deket

        // MVP = model * view * proj
        Math::Matrix4 mvp = model * view * proj;

        // Transpose untuk HLSL (column-major)
        Math::Matrix4 mvpTransposed;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                mvpTransposed.m[i][j] = mvp.m[j][i];

        // Log MVP
        Logger::Log(LogLevel::Info, "MVP Matrix: [%.2f, %.2f, %.2f, %.2f]",
            mvpTransposed.m[0][0], mvpTransposed.m[0][1], mvpTransposed.m[0][2], mvpTransposed.m[0][3]);
        Logger::Log(LogLevel::Info, "            [%.2f, %.2f, %.2f, %.2f]",
            mvpTransposed.m[1][0], mvpTransposed.m[1][1], mvpTransposed.m[1][2], mvpTransposed.m[1][3]);
        Logger::Log(LogLevel::Info, "            [%.2f, %.2f, %.2f, %.2f]",
            mvpTransposed.m[2][0], mvpTransposed.m[2][1], mvpTransposed.m[2][2], mvpTransposed.m[2][3]);
        Logger::Log(LogLevel::Info, "            [%.2f, %.2f, %.2f, %.2f]",
            mvpTransposed.m[3][0], mvpTransposed.m[3][1], mvpTransposed.m[3][2], mvpTransposed.m[3][3]);

        void* mappedData;
        CD3DX12_RANGE readRange(0, 0);
        HRESULT hr = m_ConstantBuffer->Map(0, &readRange, &mappedData);
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to map constant buffer: %s", err.ErrorMessage());
            return;
        }
        memcpy(mappedData, &mvpTransposed, sizeof(Math::Matrix4));
        m_ConstantBuffer->Unmap(0, nullptr);
        Logger::Log(LogLevel::Info, "Constant buffer updated");
    }

    void Device::SignalFence(ID3D12CommandQueue* queue)
    {
        HRESULT hr = queue->Signal(m_Fence.Get(), m_FenceValue);
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to signal fence: %s", err.ErrorMessage());
            return;
        }
        ++m_FenceValue;
    }

    void Device::WaitForFence()
    {
        if (m_Fence->GetCompletedValue() < m_FenceValue - 1)
        {
            HRESULT hr = m_Fence->SetEventOnCompletion(m_FenceValue - 1, m_FenceEvent);
            if (FAILED(hr))
            {
                _com_error err(hr);
                Logger::Log(LogLevel::Error, "Failed to set fence event: %s", err.ErrorMessage());
                return;
            }
            WaitForSingleObject(m_FenceEvent, INFINITE);
        }
    }
}