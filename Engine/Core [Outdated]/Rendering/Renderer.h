#pragma once
#include "DeviceContext.h"
#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class Renderer
{
public:
	Renderer() = default;
	~Renderer() = default;

	bool Initialize(DeviceContext* deviceContext);
	void GetBuffers(); // Langkah 17
	void CreateRenderTargetViews(); // langkah 18
	void CreateDepthStencilView(); // Langkah 19
	void CreateShaderResourceViews(); // Langkah 20

private:
	DeviceContext* m_DeviceContext = nullptr;
	static const UINT FrameCount = 2;
	ComPtr<ID3D12Resource> m_RenderTargets[FrameCount];
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandles[FrameCount];
	D3D12_CPU_DESCRIPTOR_HANDLE m_DSVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE m_SRVHandle;
};