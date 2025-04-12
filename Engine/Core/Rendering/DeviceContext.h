#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>
#include "../Utils/Logger.h"
#include "CommandQueue.h"

using Microsoft::WRL::ComPtr;

class DeviceContext
{
public:
	bool Initialize();
	ComPtr<IDXGIAdapter1> GetAdapter() const { return m_Adapter; }
	ComPtr<ID3D12Device> GetDevice() const { return m_Device; }
	ComPtr<ID3D12CommandQueue> GetCommandQueue() const { return m_CommandQueue.GetQueue(); }

private:
	bool QueryAdapter();
	bool CreateDevice();
	ComPtr<IDXGIFactory4> m_Factory;
	ComPtr<IDXGIAdapter1> m_Adapter;
	ComPtr<ID3D12Device> m_Device;
	CommandQueue m_CommandQueue;
};
