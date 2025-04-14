#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include "CommandQueue.h"
#include "../Math/Vector.h"

using Microsoft::WRL::ComPtr;

struct Vertex {
    Vector3 position;
    Vector3 normal;
};

class Model {
public:
    bool Initialize(ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList, CommandQueue& commandQueue, ComPtr<ID3D12CommandAllocator> commandAllocator);
    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const { return m_VertexBufferView; }
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const { return m_IndexBufferView; }
    UINT GetIndexCount() const { return static_cast<UINT>(m_Indices.size()); }
private:
    std::vector<Vertex> m_Vertices;
    std::vector<UINT> m_Indices;
    ComPtr<ID3D12Resource> m_VertexBuffer;
    ComPtr<ID3D12Resource> m_VertexBufferUpload;
    ComPtr<ID3D12Resource> m_IndexBuffer;
    ComPtr<ID3D12Resource> m_IndexBufferUpload;
    D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
    D3D12_INDEX_BUFFER_VIEW m_IndexBufferView;
};