#pragma once
#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class RootSignature
{
public:
	bool Initialize(ComPtr<ID3D12Device> device);
	ComPtr<ID3D12RootSignature> GetRootSignature() const { return m_RootSignature; }

private:
	ComPtr<ID3D12RootSignature> m_RootSignature;
};
