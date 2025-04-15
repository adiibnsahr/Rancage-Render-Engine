#include "Model.h"
#include "../Models/TriangleData.h"
#include "CommandQueue.h"
#include "../Utils/Logger.h"
#include <d3dx12.h>

bool Model::Initialize(ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList, CommandQueue& commandQueue, ComPtr<ID3D12CommandAllocator> commandAllocator) {
    if (!device || !commandList || !commandAllocator) {
        Logger::Log(LogLevel::Error, "Device, command list, or command allocator is null when creating vertex buffer");
        return false;
    }

    // Setup Vertex Buffer
    m_Vertices = GetTriangleVertices();
    if (m_Vertices.empty()) {
        Logger::Log(LogLevel::Error, "Vertex data is empty after GetTriangleVertices");
        return false;
    }
    Logger::Log(LogLevel::Info, "Vertex data retrieved: " + std::to_string(m_Vertices.size()) + " vertices");

    const UINT vertexBufferSize = static_cast<UINT>(m_Vertices.size() * sizeof(Vertex));

    Logger::Log(LogLevel::Info, "Creating vertex buffer...");
    D3D12_HEAP_PROPERTIES defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    HRESULT hr = device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_VertexBuffer));
    if (FAILED(hr)) {
        Logger::Log(LogLevel::Error, "Failed to create vertex buffer: HRESULT 0x" + std::to_string(hr));
        return false;
    }
    Logger::Log(LogLevel::Info, "Vertex buffer created");

    Logger::Log(LogLevel::Info, "Creating vertex buffer upload heap...");
    D3D12_HEAP_PROPERTIES uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    hr = device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_VertexBufferUpload));
    if (FAILED(hr)) {
        Logger::Log(LogLevel::Error, "Failed to create vertex buffer upload heap: HRESULT 0x" + std::to_string(hr));
        return false;
    }
    Logger::Log(LogLevel::Info, "Vertex buffer upload heap created");

    void* mappedData = nullptr;
    hr = m_VertexBufferUpload->Map(0, nullptr, &mappedData);
    if (FAILED(hr)) {
        Logger::Log(LogLevel::Error, "Failed to map vertex buffer upload heap: HRESULT 0x" + std::to_string(hr));
        return false;
    }
    memcpy(mappedData, m_Vertices.data(), vertexBufferSize);
    m_VertexBufferUpload->Unmap(0, nullptr);
    Logger::Log(LogLevel::Info, "Vertex data copied to upload heap");

    // Setup Index Buffer
    m_Indices = GetTriangleIndices();
    if (m_Indices.empty()) {
        Logger::Log(LogLevel::Error, "Index data is empty after GetTriangleIndices");
        return false;
    }
    Logger::Log(LogLevel::Info, "Index data retrieved: " + std::to_string(m_Indices.size()) + " indices");

    const UINT indexBufferSize = static_cast<UINT>(m_Indices.size() * sizeof(UINT));

    Logger::Log(LogLevel::Info, "Creating index buffer...");
    bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
    hr = device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_IndexBuffer));
    if (FAILED(hr)) {
        Logger::Log(LogLevel::Error, "Failed to create index buffer: HRESULT 0x" + std::to_string(hr));
        return false;
    }
    Logger::Log(LogLevel::Info, "Index buffer created");

    Logger::Log(LogLevel::Info, "Creating index buffer upload heap...");
    hr = device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_IndexBufferUpload));
    if (FAILED(hr)) {
        Logger::Log(LogLevel::Error, "Failed to create index buffer upload heap: HRESULT 0x" + std::to_string(hr));
        return false;
    }
    Logger::Log(LogLevel::Info, "Index buffer upload heap created");

    hr = m_IndexBufferUpload->Map(0, nullptr, &mappedData);
    if (FAILED(hr)) {
        Logger::Log(LogLevel::Error, "Failed to map index buffer upload heap: HRESULT 0x" + std::to_string(hr));
        return false;
    }
    memcpy(mappedData, m_Indices.data(), indexBufferSize);
    m_IndexBufferUpload->Unmap(0, nullptr);
    Logger::Log(LogLevel::Info, "Index data copied to upload heap");

    // Sinkronisasi sebelum reset command list
    Logger::Log(LogLevel::Info, "Ensuring GPU is idle before resetting command allocator...");
    commandQueue.WaitForFence();
    Logger::Log(LogLevel::Info, "GPU idle, proceeding with command list reset");

    hr = commandList->Reset(commandAllocator.Get(), nullptr);
    if (FAILED(hr)) {
        Logger::Log(LogLevel::Error, "Failed to reset command list: HRESULT 0x" + std::to_string(hr));
        return false;
    }
    Logger::Log(LogLevel::Info, "Command list reset");

    // Copy vertex buffer
    commandList->CopyBufferRegion(m_VertexBuffer.Get(), 0, m_VertexBufferUpload.Get(), 0, vertexBufferSize);
    Logger::Log(LogLevel::Info, "CopyBufferRegion recorded for vertex buffer (expected source state: GENERIC_READ, dest state: COPY_DEST)");

    D3D12_RESOURCE_BARRIER vertexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_VertexBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    commandList->ResourceBarrier(1, &vertexBarrier);
    Logger::Log(LogLevel::Info, "Resource barrier recorded (vertex buffer: COPY_DEST to VERTEX_AND_CONSTANT_BUFFER)");

    // Copy index buffer
    commandList->CopyBufferRegion(m_IndexBuffer.Get(), 0, m_IndexBufferUpload.Get(), 0, indexBufferSize);
    Logger::Log(LogLevel::Info, "CopyBufferRegion recorded for index buffer (expected source state: GENERIC_READ, dest state: COPY_DEST)");

    D3D12_RESOURCE_BARRIER indexBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_IndexBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_INDEX_BUFFER);
    commandList->ResourceBarrier(1, &indexBarrier);
    Logger::Log(LogLevel::Info, "Resource barrier recorded (index buffer: COPY_DEST to INDEX_BUFFER)");

    hr = commandList->Close();
    if (FAILED(hr)) {
        Logger::Log(LogLevel::Error, "Failed to close command list: HRESULT 0x" + std::to_string(hr));
        return false;
    }
    Logger::Log(LogLevel::Info, "Command list closed");

    Logger::Log(LogLevel::Info, "Executing command list...");
    ID3D12CommandList* commandLists[] = { commandList.Get() };
    commandQueue.GetQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);
    Logger::Log(LogLevel::Info, "Vertex and index buffer upload executed");

    Logger::Log(LogLevel::Info, "Waiting for GPU to finish...");
    commandQueue.WaitForFence();
    Logger::Log(LogLevel::Info, "GPU finished");

    // Setup vertex buffer view
    m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
    m_VertexBufferView.SizeInBytes = vertexBufferSize;
    m_VertexBufferView.StrideInBytes = sizeof(Vertex);
    Logger::Log(LogLevel::Info, "Vertex buffer view created");

    // Setup index buffer view
    m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
    m_IndexBufferView.SizeInBytes = indexBufferSize;
    m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    Logger::Log(LogLevel::Info, "Index buffer view created");

    return true;
}