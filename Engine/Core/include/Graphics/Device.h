#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>
#include "../Core/include/Graphics/Command.h"
#include "../Core/include/Graphics/SwapChain.h"
#include "../Core/include/Resources/DepthBuffer.h"
#include "../Core/include/Graphics/RootSignature.h"
#include "../Core/include/Graphics/PipelineState.h"

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
        ID3D12DescriptorHeap* GetDSVHeap() const { return m_DSVHeap.Get(); }
        DepthBuffer& GetDepthBuffer() { return m_DepthBuffer; }
        RootSignature& GetRootSignature() { return m_RootSignature; }
        PipelineState& GetPipelineState() { return m_PipelineState; }
        ID3D12Resource* GetConstantBuffer() const { return m_ConstantBuffer.Get(); }
        ID3D12Fence* GetFence() const { return m_Fence.Get(); }
        UINT64 GetFenceValue() const { return m_FenceValue; }
        HANDLE GetFenceEvent() const { return m_FenceEvent; }
        void SignalFence(ID3D12CommandQueue* queue);
        void WaitForFence();
        void CreateRenderTargetViews();
        void CreateDepthStencilView();
        void CreateConstantBuffer();
        void UpdateConstantBuffer();

    private:
        Microsoft::WRL::ComPtr<IDXGIFactory6> m_Factory;
        Microsoft::WRL::ComPtr<IDXGIAdapter1> m_Adapter;
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        Command m_Command;
        SwapChain m_SwapChain;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
        DepthBuffer m_DepthBuffer;
        RootSignature m_RootSignature;
        PipelineState m_PipelineState;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstantBuffer;
        Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
        UINT64 m_FenceValue = 0;
        HANDLE m_FenceEvent = nullptr;
    };
}