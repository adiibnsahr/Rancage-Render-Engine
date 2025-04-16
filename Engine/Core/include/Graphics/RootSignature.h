#pragma once
#include "../Interfaces/IPipelineComponent.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace Graphics
{
	class RootSignature : public IPipelineComponent
	{
	public:
		RootSignature();
		~RootSignature();

		bool Configure(ID3D12Device* device) override;
		ID3D12RootSignature* GetRootSignature() const { return m_RootSignature.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
	};
}
