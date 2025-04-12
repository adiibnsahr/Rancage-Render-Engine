#pragma once
#include <dxgi1_6.h>
#include <wrl.h>
#include "../Utils/Logger.h"

using Microsoft::WRL::ComPtr;

class DeviceContext
{
public:
	bool Initialize();

private:
	ComPtr<IDXGIFactory4> m_Factory;
};
