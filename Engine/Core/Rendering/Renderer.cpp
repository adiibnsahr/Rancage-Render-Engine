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
