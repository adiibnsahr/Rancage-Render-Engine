#include "DepthBuffer.h"
#include "../Utils/Logger.h"

bool DepthBuffer::Initialize(ComPtr<ID3D12Device> device, DescriptorHeap& dsvHeap, UINT width, UINT height)
{
	if (!device)
	{
		Logger::Log(LogLevel::Error, "Device is null when creating depth buffer");
		return false;
	}

	if (!dsvHeap.GetHeap())
	{
		Logger::Log(LogLevel::Error, "DSV descriptor heap is null when creating depth buffer");
		return false;
	}

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	Logger::Log(LogLevel::Info, "Creating depth buffer...");
	HRESULT hr = device->CreateCommittedResource
	(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&m_DepthBuffer)
	);

	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to create depth buffer: HRESULT " + std::to_string(hr));
		return false;
	}
	Logger::Log(LogLevel::Info, "Depth buffer created");

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	m_DSV = dsvHeap.GetCPUHandle(0);
	device->CreateDepthStencilView(m_DepthBuffer.Get(), &dsvDesc, m_DSV);
	Logger::Log(LogLevel::Info, "Depth/stencil view created");

	return true;
}
