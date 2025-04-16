#include "../Core/include/Graphics/Device.h"
#include "../Core/include/Utils/Logger.h"
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
        rtvHeapDesc.NumDescriptors = 2; // 2 buffers (double buffering)
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
        dsvHeapDesc.NumDescriptors = 1; // 1 depth buffer
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

        m_Device->CreateDepthStencilView(m_DepthBuffer.GetResource(), &dsvDesc, m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
        Logger::Log(LogLevel::Info, "Depth stencil view created");
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