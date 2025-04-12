#include "DeviceContext.h"

bool DeviceContext::Initialize()
{
	// CreateDXGIFactory2 minimum support nya Windows8 keatas, atau DX12
	HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_Factory));
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to create DXGI factory: HRESULT " + std::to_string(hr));
		return false;
	}
	Logger::Log(LogLevel::Info, "DXGI factory created");
	return true;
}
