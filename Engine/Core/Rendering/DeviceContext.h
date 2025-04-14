#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "CommandQueue.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"
#include "DepthBuffer.h"
#include "Model.h"
#include "Texture.h"

using Microsoft::WRL::ComPtr;

class DeviceContext {
public:
    DeviceContext() = default;
    ~DeviceContext() {
        m_Texture = Texture();
        m_Model = Model();
        m_DepthBuffer = DepthBuffer();
        m_DSVHeap = DescriptorHeap();
        m_RTVHeap = DescriptorHeap();
        m_SwapChain = SwapChain();
        m_PipelineState = PipelineState();
        m_RootSignature = RootSignature();
        m_CommandQueue = CommandQueue();
        m_Device.Reset();
        m_Adapter.Reset();
        m_Factory.Reset();
    }
    bool Initialize(HWND hwnd, UINT width, UINT height);
    ComPtr<IDXGIAdapter1> GetAdapter() const { return m_Adapter; }
    ComPtr<ID3D12Device> GetDevice() const { return m_Device; }
    ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return m_CommandQueue.GetQueue(); }
    CommandQueue& GetCommandQueueObject() { return m_CommandQueue; } // Getter baru
    ComPtr<ID3D12CommandAllocator> GetCommandAllocator() const { return m_CommandQueue.GetCommandAllocator(); }
    ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return m_CommandQueue.GetCommandList(); }
    ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_SwapChain.GetSwapChain(); }
    ComPtr<ID3D12DescriptorHeap> GetRTVHeap() const { return m_RTVHeap.GetHeap(); }
    ComPtr<ID3D12DescriptorHeap> GetDSVHeap() const { return m_DSVHeap.GetHeap(); }
    ComPtr<ID3D12Resource> GetDepthBuffer() const { return m_DepthBuffer.GetDepthBuffer(); }
    ComPtr<ID3D12RootSignature> GetRootSignature() const { return m_RootSignature.GetRootSignature(); }
    ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_PipelineState.GetPipelineState(); }
private:
    bool QueryAdapter();
    bool CreateDevice();
    ComPtr<IDXGIFactory4> m_Factory;
    ComPtr<IDXGIAdapter1> m_Adapter;
    ComPtr<ID3D12Device> m_Device;
    CommandQueue m_CommandQueue;
    RootSignature m_RootSignature;
    PipelineState m_PipelineState;
    SwapChain m_SwapChain;
    DescriptorHeap m_RTVHeap;
    DescriptorHeap m_DSVHeap;
    DepthBuffer m_DepthBuffer;
    Model m_Model;
    Texture m_Texture;
};