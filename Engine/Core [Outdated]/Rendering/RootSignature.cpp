#include "RootSignature.h"
#include "../Utils/Logger.h"

bool RootSignature::Initialize(ComPtr<ID3D12Device> device)
{
	if (!device)
	{
		Logger::Log(LogLevel::Error, "Device is null when creating root signature");
		return false;
	}

	// Define root signature
	D3D12_ROOT_SIGNATURE_DESC desc = {};
	D3D12_ROOT_PARAMETER params[2] = {};

	// Parameter 0: Constant Buffer (CBV) buat matrices (view, projection, dll)
	params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	params[0].Descriptor.ShaderRegister = 0;
	params[0].Descriptor.RegisterSpace = 0;
	params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;  // Bisa diakses vertex + pixel shader

	// Parameter 1: Descriptor Table untuk PBR textures
	D3D12_DESCRIPTOR_RANGE range = {};
	range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // Shader Resource View (textures)
	range.NumDescriptors = 3; // Misal albedo, normal, metallic
	range.BaseShaderRegister = 0;
	range.RegisterSpace = 0;
	range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[1].DescriptorTable.NumDescriptorRanges = 1;
	params[1].DescriptorTable.pDescriptorRanges = &range;
	params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	desc.NumParameters = 2;
	desc.pParameters = params;
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// Serialize root signature
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	Logger::Log(LogLevel::Info, "Serializing root signature...");
	HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			Logger::Log(LogLevel::Error, "Failed to serialize root signature: " +
						std::string(static_cast<const char*>(errorBlob->GetBufferPointer())));
		}
		else
		{
			Logger::Log(LogLevel::Error, "Failed to serialize root signature: HRESULT " + std::to_string(hr));
		}
		return false;
	}
	Logger::Log(LogLevel::Info, "Root signature serialized");

	// Buat root signature
	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
									IID_PPV_ARGS(&m_RootSignature));
	if (FAILED(hr))
	{
		Logger::Log(LogLevel::Error, "Failed to create root signature: HRESULT " + std::to_string(hr));
		return false;
	}
	Logger::Log(LogLevel::Info, "Root signature created");

	return true;
}
