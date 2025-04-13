#pragma once
#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class CommandQueue
{
public:
	bool Initialize(ComPtr<ID3D12Device> device);
	void WaitForFence();
	ComPtr<ID3D12CommandQueue> GetQueue() const { return m_Queue; }

private:
	ComPtr<ID3D12CommandQueue> m_Queue;
	ComPtr<ID3D12Fence> m_Fence;
	UINT64 m_FenceValue = 0;
	HANDLE m_FenceEvent = nullptr;
};