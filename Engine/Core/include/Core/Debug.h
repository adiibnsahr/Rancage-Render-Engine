#pragma once
#include <dxgi.h>
#include <d3d12.h>

namespace Debug
{
	bool Initialize();
	void SetupInfoQueue(); // Untuk capture debug messages
}