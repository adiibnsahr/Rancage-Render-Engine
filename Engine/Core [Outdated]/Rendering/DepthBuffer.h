#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "DescriptorHeap.h"

using Microsoft::WRL::ComPtr;

class DepthBuffer
{
public:
	bool Initialize(ComPtr<ID3D12Device> device, DescriptorHeap& dsvHeap, UINT width, UINT height);
	ComPtr<ID3D12Resource> GetDepthBuffer() const { return m_DepthBuffer; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const { return m_DSV; }

private:
	ComPtr<ID3D12Resource> m_DepthBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE m_DSV;
};
