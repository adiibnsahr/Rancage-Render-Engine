#pragma once
#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class CommandQueue
{
public:
	bool Initialize(ComPtr<ID3D12Device> device);
	ComPtr<ID3D12CommandQueue> GetQueue() const { return m_Queue; }

private:
	ComPtr<ID3D12CommandQueue> m_Queue;
};