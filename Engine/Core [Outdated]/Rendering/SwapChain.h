#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class SwapChain
{
public:
	bool Initialize(HWND hwnd, ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12CommandQueue> commandQueue, UINT width, UINT height);
	ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_SwapChain; }

private:
	ComPtr<IDXGISwapChain3> m_SwapChain;
};
