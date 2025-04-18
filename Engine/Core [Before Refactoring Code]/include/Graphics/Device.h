#pragma once
#include "Command.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "../Core/Window.h"
#include "../Core/include/Math/Matrix4.h"
#include "../Core/include/Resources/DepthBuffer.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace Graphics
{
    class Device
    {
    public:
        Device();
        bool Initialize(HWND hwnd, UINT width, UINT height);
        void WaitForFence();
        void SignalFence(ID3D12CommandQueue* queue);
        void UpdateConstantBuffer(const Math::Matrix4& modelMatrix, const Math::Matrix4& viewMatrix, const Math::Matrix4& projMatrix);

        // Getters
        ID3D12Device* GetDevice() const { return m_Device.Get(); }
        Command& GetCommand() { return m_Command; }
        SwapChain& GetSwapChain() { return m_SwapChain; }
        ID3D12DescriptorHeap* GetRTVHeap() const { return m_RTVHeap.Get(); }
        RootSignature& GetRootSignature() { return m_RootSignature; }
        PipelineState& GetPipelineState() { return m_PipelineState; }
        DepthBuffer& GetDepthBuffer() { return m_DepthBuffer; }
        ID3D12Resource* GetConstantBuffer() const { return m_ConstantBuffer.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
        Microsoft::WRL::ComPtr<IDXGIFactory6> m_Factory;
        Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstantBuffer;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
        HANDLE m_FenceEvent;
        UINT64 m_FenceValue;
        Command m_Command;
        SwapChain m_SwapChain;
        RootSignature m_RootSignature;
        PipelineState m_PipelineState;
        DepthBuffer m_DepthBuffer;
    };
}