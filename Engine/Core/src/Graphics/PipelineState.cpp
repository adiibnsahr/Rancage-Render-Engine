#include "../Core/include/Graphics/PipelineState.h"
#include "../Core/include/Graphics/RootSignature.h"
#include "../Core/include/Utils/Logger.h"
#include <comdef.h>
#include <d3dx12.h>
#include <d3dcompiler.h>

namespace Graphics
{
    PipelineState::PipelineState() = default;
    PipelineState::~PipelineState() = default;

    bool PipelineState::Configure(ID3D12Device* device)
    {
        if (!device)
        {
            Logger::Log(LogLevel::Error, "Device is null in PipelineState::Configure");
            return false;
        }

        // Compile shaders
        Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
        Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;
        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

        HRESULT hr = D3DCompileFromFile(
            L"Shaders/VertexShader.hlsl",
            nullptr,
            nullptr,
            "main",
            "vs_5_0",
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
            0,
            &vertexShader,
            &errorBlob);
        if (FAILED(hr))
        {
            _com_error err(hr);
            if (errorBlob)
            {
                Logger::Log(LogLevel::Error, "Failed to compile vertex shader: %s", (const char*)errorBlob->GetBufferPointer());
            }
            else
            {
                Logger::Log(LogLevel::Error, "Failed to compile vertex shader: %s", err.ErrorMessage());
            }
            return false;
        }

        hr = D3DCompileFromFile(
            L"Shaders/PixelShader.hlsl",
            nullptr,
            nullptr,
            "main",
            "ps_5_0",
            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
            0,
            &pixelShader,
            &errorBlob);
        if (FAILED(hr))
        {
            _com_error err(hr);
            if (errorBlob)
            {
                Logger::Log(LogLevel::Error, "Failed to compile pixel shader: %s", (const char*)errorBlob->GetBufferPointer());
            }
            else
            {
                Logger::Log(LogLevel::Error, "Failed to compile pixel shader: %s", err.ErrorMessage());
            }
            return false;
        }

        // Define input layout
        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Define PSO
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
        psoDesc.pRootSignature = m_RootSignature; // Set in Device later
        psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
        psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;

        hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create pipeline state: %s", err.ErrorMessage());
            return false;
        }
        Logger::Log(LogLevel::Info, "Pipeline state created");

        return true;
    }
}