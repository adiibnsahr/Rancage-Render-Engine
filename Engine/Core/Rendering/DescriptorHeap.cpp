#include "DescriptorHeap.h"
#include "../Utils/Logger.h"

bool DescriptorHeap::Initialize(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible)
{
	if (!device)
	{
		Logger::Log(LogLevel::Error, "Device is null when creating descriptor heap");
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = type;
	desc.NumDescriptors = numDescriptors;
	desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;

	std::string heapType = (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV ? "RTV" : "DSV");
	Logger::Log(LogLevel::Info, std::string("Creating descriptor heap (Type: ") + heapType + ")...");
	HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_Heap));
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to create descriptor heap: HRESULT " + std::to_string(hr));
		return false;
	}
	Logger::Log(LogLevel::Info, "Descriptor heap created");

	m_DescriptorSize = device->GetDescriptorHandleIncrementSize(type);

	return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(UINT index) const
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_Heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += index * m_DescriptorSize;
	return handle;
}
