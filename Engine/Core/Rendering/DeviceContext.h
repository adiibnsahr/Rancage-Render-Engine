#pragma once
#include <dxgi1_6.h>
#include <wrl.h>
#include "../Utils/Logger.h"

using Microsoft::WRL::ComPtr;

class DeviceContext
{
public:
	bool Initialize();
	ComPtr<IDXGIAdapter1> GetAdapter() const { return m_Adapter; }

private:
	bool QueryAdapter();
	ComPtr<IDXGIFactory4> m_Factory;
	ComPtr<IDXGIAdapter1> m_Adapter;
};
