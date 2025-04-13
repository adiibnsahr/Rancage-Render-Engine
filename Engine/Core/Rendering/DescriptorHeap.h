#pragma once
#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class DescriptorHeap
{
public:
	bool Initialize(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible = false);
	ComPtr<ID3D12DescriptorHeap> GetHeap() const { return m_Heap; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index) const;
	UINT GetDescriptorSize() const { return m_DescriptorSize; }

private:
	ComPtr<ID3D12DescriptorHeap> m_Heap;
	UINT m_DescriptorSize = 0;
};
