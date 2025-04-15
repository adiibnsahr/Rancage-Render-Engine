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
	void GetBuffers();
	void CreateRenderTargetViews();

private:
	DeviceContext* m_DeviceContext = nullptr;
	static const UINT FrameCount = 2;
	ComPtr<ID3D12Resource> m_RenderTargets[FrameCount];
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandles[FrameCount];
};