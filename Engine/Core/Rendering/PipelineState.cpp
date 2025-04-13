#include "PipelineState.h"
#include "../Utils/Logger.h"
#include <d3dx12.h>
#include <d3dcompiler.h>

bool PipelineState::Initialize(ComPtr<ID3D12Device> device, ComPtr<ID3D12RootSignature> rootSignature)
{
    if (!device) {
        Logger::Log(LogLevel::Error, "Device is null when creating pipeline state");
        return false;
    }
    if (!rootSignature) {
        Logger::Log(LogLevel::Error, "Root signature is null when creating pipeline state");
        return false;
    }

    // Compile vertex shader dari file
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> errorBlob;
    Logger::Log(LogLevel::Info, "Compiling vertex shader from file...");
    HRESULT hr = D3DCompileFromFile(L"Shaders/VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShader, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Logger::Log(LogLevel::Error, "Failed to compile vertex shader: " +
                std::string(static_cast<const char*>(errorBlob->GetBufferPointer())));
        }
        else
        {
            Logger::Log(LogLevel::Error, "Failed to compile vertex shader: HRESULT " + std::to_string(hr));
        }
        return false;
    }
    Logger::Log(LogLevel::Info, "Vertex shader compiled");

    // Compile pixel shader dari file
    ComPtr<ID3DBlob> pixelShader;
    Logger::Log(LogLevel::Info, "Compiling pixel shader from file...");
    hr = D3DCompileFromFile(L"Shaders/PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShader, &errorBlob);
    if (FAILED(hr))
    {
        if (errorBlob)
        {
            Logger::Log(LogLevel::Error, "Failed to compile pixel shader: " +
                std::string(static_cast<const char*>(errorBlob->GetBufferPointer())));
        }
        else
        {
            Logger::Log(LogLevel::Error, "Failed to compile pixel shader: HRESULT " + std::to_string(hr));
        }
        return false;
    }
    Logger::Log(LogLevel::Info, "Pixel shader compiled");

    // Define input layout (vertex format)
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // Define pipeline state
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
    psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
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

    // Create pipeline state
    Logger::Log(LogLevel::Info, "Creating pipeline state...");
    hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
    if (FAILED(hr)) {
        Logger::Log(LogLevel::Error, "Failed to create pipeline state: HRESULT " + std::to_string(hr));
        return false;
    }
    Logger::Log(LogLevel::Info, "Pipeline state created");

	return true;
}
