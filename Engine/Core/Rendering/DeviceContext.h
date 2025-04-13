#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>
#include "../Utils/Logger.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "DescriptorHeap.h"

using Microsoft::WRL::ComPtr;

class DeviceContext
{
public:
	bool Initialize(HWND hwnd, UINT width, UINT height);
	ComPtr<IDXGIAdapter1> GetAdapter() const { return m_Adapter; }
	ComPtr<ID3D12Device> GetDevice() const { return m_Device; }
	ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return m_CommandQueue.GetQueue(); }
	ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_SwapChain.GetSwapChain(); }
	ComPtr<ID3D12DescriptorHeap> GetRTVHeap() const { return m_RTVHeap.GetHeap(); }
	ComPtr<ID3D12DescriptorHeap> GetDSVHeap() const { return m_DSVHeap.GetHeap(); }

private:
	bool QueryAdapter();
	bool CreateDevice();
	ComPtr<IDXGIFactory4> m_Factory;
	ComPtr<IDXGIAdapter1> m_Adapter;
	ComPtr<ID3D12Device> m_Device;
	CommandQueue m_CommandQueue;
	SwapChain m_SwapChain;
	DescriptorHeap m_RTVHeap;
	DescriptorHeap m_DSVHeap;
};
