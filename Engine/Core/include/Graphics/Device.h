#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>

namespace Graphics
{
	class Device
	{
	public:
		Device();
		~Device();

		bool Initialize();
		ID3D12Device* GetDevice() const { return m_Device.Get(); }

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory6> m_Factory;
		Microsoft::WRL::ComPtr<IDXGIAdapter1> m_Adapter;
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
	};
}
