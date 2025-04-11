#include "Debug.h"
#include <comdef.h>
#include <wrl/client.h>


bool Debug::Initialize()
{
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug> debug;
	HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to initialize DX12 debug layer: HRESULT " + std::to_string(hr));
		return false;
	}
	debug->EnableDebugLayer();
	Logger::Log(LogLevel::Info, "DX12 debug layer enabled");
#else
	Logger::Log(LogLevel::Info, "Debug layer skipped in release build");
#endif // _DEBUG
	return true;

}

