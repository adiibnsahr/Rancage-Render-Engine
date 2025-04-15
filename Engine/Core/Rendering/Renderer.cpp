#include "Renderer.h"
#include "../Utils/Logger.h"

bool Renderer::Initialize(DeviceContext* deviceContext)
{
	if (!deviceContext)
	{
		Logger::Log(LogLevel::Error, "Device context is null in Renderer::Initialize");
		return false;
	}

	m_DeviceContext = deviceContext;
	GetBuffers();
	CreateRenderTargetViews();
	CreateDepthStencilView();
	CreateShaderResourceViews();

	Logger::Log(LogLevel::Info, "Renderer initialized (buffers retrieved)");
	return true;
}

void Renderer::GetBuffers()
{
	// Ambil back buffers dari swap chain
	for (UINT i = 0; i < FrameCount; i++)
	{
		HRESULT hr = m_DeviceContext->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
		if (FAILED(hr))
		{
			Logger::Log(LogLevel::Error, "Failed to get back buffer " + std::to_string(i) + ": HRESULT " + std::to_string(hr));
			return;
		}
		Logger::Log(LogLevel::Info, "Back buffer " + std::to_string(i) + " retrieved");
	}
}

void Renderer::CreateRenderTargetViews()
{
	// Membuat RTVs untuk setiap back buffer
	for (UINT i = 0; i < FrameCount; i++)
	{
		if (!m_RenderTargets[i])
		{
			Logger::Log(LogLevel::Error, "Render target " + std::to_string(i) + " is null");
			return;
		}

		// Ambil CPU handle dari RTV heap
		m_RTVHandles[i] = m_DeviceContext->GetRTVHeapObject().GetCPUHandle(i); // Menggunakan fungsi dari DescriptorHeap

		// Buat RTV
		m_DeviceContext->GetDevice()->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, m_RTVHandles[i]);
		Logger::Log(LogLevel::Info, "Render target view created for back buffer " + std::to_string(i));
	}
}

void Renderer::CreateDepthStencilView()
{
	// Ambil DSV handle dari DSV heap
	m_DSVHandle = m_DeviceContext->GetDSVHeap()->GetCPUDescriptorHandleForHeapStart();
	Logger::Log(LogLevel::Info, "Depth stencil view retrieved");
}

void Renderer::CreateShaderResourceViews()
{
	m_SRVHandle = m_DeviceContext->GetTexture().GetSRV();
	Logger::Log(LogLevel::Info, "Shader resource view retrieved for texture");
}
