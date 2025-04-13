#pragma once
#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class PipelineState
{
public:
	bool Initialize(ComPtr<ID3D12Device> device, ComPtr<ID3D12RootSignature> rootSignature);
	ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_PipelineState; }

private:
	ComPtr<ID3D12PipelineState> m_PipelineState;

};
