#pragma once
#include "../Interfaces/IPipelineComponent.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace Graphics
{
	class PipelineState : public IPipelineComponent
	{
	public:
		PipelineState();
		~PipelineState();

		bool Configure(ID3D12Device* device) override;
		ID3D12PipelineState* GetPipelineState() const { return m_PipelineState.Get(); }
		void SetRootSignature(ID3D12RootSignature* rootSignature) { m_RootSignature = rootSignature; }

	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
		ID3D12RootSignature* m_RootSignature = nullptr; // Tambah buat simpan root signature
	};
}