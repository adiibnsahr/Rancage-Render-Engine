#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>
#include "../Core/include/Graphics/Command.h"
#include "../Core/include/Graphics/SwapChain.h"

namespace Graphics
{
    class Device
    {
    public:
        Device();
        ~Device();

        bool Initialize(HWND hwnd, UINT width, UINT height);
        ID3D12Device* GetDevice() const { return m_Device.Get(); }
        Command& GetCommand() { return m_Command; }
        SwapChain& GetSwapChain() { return m_SwapChain; }
        ID3D12DescriptorHeap* GetRTVHeap() const { return m_RTVHeap.Get(); }
        void CreateRenderTargetViews(); // Buat RTV

    private:
        Microsoft::WRL::ComPtr<IDXGIFactory6> m_Factory;
        Microsoft::WRL::ComPtr<IDXGIAdapter1> m_Adapter;
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        Command m_Command;
        SwapChain m_SwapChain;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap; // Heap buat RTV
    };
}