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

    std::string heapType = (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV ? "RTV" : type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV ? "DSV" : "CBV_SRV_UAV");
    Logger::Log(LogLevel::Info, std::string("Creating descriptor heap (Type: ") + heapType + ")...");
    HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_Heap));
    if (FAILED(hr))
    {
        Logger::Log(LogLevel::Error, "Failed to create descriptor heap: HRESULT " + std::to_string(hr));
        return false;
    }
    Logger::Log(LogLevel::Info, "Descriptor heap created");

    m_DescriptorSize = device->GetDescriptorHandleIncrementSize(type);
    m_MaxDescriptors = numDescriptors; // Simpen kapasitas
    m_CurrentIndex = 0; // Reset index

    return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandle(UINT index) const
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_Heap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += index * m_DescriptorSize;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::Allocate()
{
    if (m_CurrentIndex >= m_MaxDescriptors)
    {
        Logger::Log(LogLevel::Error, "Descriptor heap out of space");
        return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
    }

    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_Heap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += m_CurrentIndex * m_DescriptorSize;
    m_CurrentIndex++;

    return handle;
}