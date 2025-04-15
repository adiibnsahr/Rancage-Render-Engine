#include "../Core/include/Core/Debug.h"
#include "../Core/include/Utils/Logger.h"
#include <comdef.h>
#include <wrl/client.h>

namespace Debug
{
	bool Initialize()
	{
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Debug> debug;
		HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
		if (FAILED(hr))
		{
			_com_error err(hr);
			Logger::Log(LogLevel::Error, "Failed to initialize DX12 debug layer: %s", err.ErrorMessage());
			return false;
		}

		debug->EnableDebugLayer();
		Logger::Log(LogLevel::Info, "DX12 debug layer enabled");

		// Enable GPU-based validation and synchronized command queue validation
		Microsoft::WRL::ComPtr<ID3D12Debug1> debug1;
		if (SUCCEEDED(debug->QueryInterface(IID_PPV_ARGS(&debug1))))
		{
			debug1->SetEnableGPUBasedValidation(true);
			debug1->SetEnableSynchronizedCommandQueueValidation(true);
			Logger::Log(LogLevel::Info, "GPU-based validation and synchronized command queue validation enabled");
		}
		else
		{
			Logger::Log(LogLevel::Info, "Failed to enalbe GPU-based validation");
		}

		SetupInfoQueue();
#else
		Logger::Log(LogLevel::Info, "Debug layer skipped in release build");
#endif // DEBUG

		return true;
	}

	void SetupInfoQueue()
	{
#ifdef _DEBUG
		Microsoft::WRL::ComPtr<ID3D12Device> device;
		HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));
		if (SUCCEEDED(hr))
		{
			Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
			if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
			{
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
				infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
				Logger::Log(LogLevel::Info, "D3D12 info queue enabled with error breakpoints");
			}
			else
			{
				Logger::Log(LogLevel::Warning, "Failed to setup D3D12 info queue");
			}
		}
		else
		{
			_com_error err(hr);
			Logger::Log(LogLevel::Warning, "Failed to create temporary device for info queue: %s", err.ErrorMessage());
		}
#endif // DEBUG

	}
}

