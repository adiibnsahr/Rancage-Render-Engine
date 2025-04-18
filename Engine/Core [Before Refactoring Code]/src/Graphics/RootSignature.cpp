#include "../Core/include/Graphics/RootSignature.h"
#include "../Core/include/Utils/Logger.h"
#include <comdef.h>
#include <d3dx12.h>

namespace Graphics
{
    RootSignature::RootSignature() = default;
    RootSignature::~RootSignature() = default;

    bool RootSignature::Configure(ID3D12Device* device)
    {
        if (!device)
        {
            Logger::Log(LogLevel::Error, "Device is null in RootSignature::Configure");
            return false;
        }

        // Define root parameter (CBV for MVP matrix)
        CD3DX12_ROOT_PARAMETER rootParameters[1];
        rootParameters[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

        // Define root signature
        CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
        rootSigDesc.Init(
            1,                        // Num parameters
            rootParameters,           // Root parameters
            0,                        // Num static samplers
            nullptr,                  // Static samplers
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        // Serialize root signature
        Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
        HRESULT hr = D3D12SerializeRootSignature(
            &rootSigDesc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            &serializedRootSig,
            &errorBlob);
        if (FAILED(hr))
        {
            _com_error err(hr);
            if (errorBlob)
            {
                Logger::Log(LogLevel::Error, "Failed to serialize root signature: %s", (const char*)errorBlob->GetBufferPointer());
            }
            else
            {
                Logger::Log(LogLevel::Error, "Failed to serialize root signature: %s", err.ErrorMessage());
            }
            return false;
        }

        // Create root signature
        hr = device->CreateRootSignature(
            0,
            serializedRootSig->GetBufferPointer(),
            serializedRootSig->GetBufferSize(),
            IID_PPV_ARGS(&m_RootSignature));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create root signature: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "Root signature created");

        return true;
    }
}