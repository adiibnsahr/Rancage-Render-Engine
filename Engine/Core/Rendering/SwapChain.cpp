#include "SwapChain.h"
#include "../Utils/Logger.h"

bool SwapChain::Initialize(HWND hwnd, ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12CommandQueue> commandQueue, UINT width, UINT height)
{
	if (!hwnd)
	{
		Logger::Log(LogLevel::Error, "Window handle is null when creating swap chain");
		return false;
	}

	if (!factory)
	{
		Logger::Log(LogLevel::Error, "DXGI factory is null when creating swap chain");
		return false;
	}

	if (!commandQueue)
	{
		Logger::Log(LogLevel::Error, "Command queue is null when creating swap chain");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Stereo = FALSE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2;
	desc.Scaling = DXGI_SCALING_STRETCH;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	ComPtr<IDXGISwapChain1> swapChain;
	Logger::Log(LogLevel::Info, "Creating swap chain...");
	HRESULT hr = factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &desc, nullptr, nullptr, &swapChain);
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to create swap chain: HRESULT " + std::to_string(hr));
		return false;
	}

	hr = swapChain.As(&m_SwapChain);
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to cast swap chain to IDXGISwapChain3: HRESULT " + std::to_string(hr));
		return false;
	}
	Logger::Log(LogLevel::Info, "Swap chain created");

	return true;
}
