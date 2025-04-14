#pragma once
#include "Logger.h"
#include <dxgi.h>
#include <d3d12.h>
#include <wrl.h>

class Debug
{
public:
	static bool Initialize();

private:
	Microsoft::WRL::ComPtr<ID3D12Debug> m_Debug;
};