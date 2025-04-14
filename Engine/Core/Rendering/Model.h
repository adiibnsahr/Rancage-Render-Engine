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
private:
    std::vector<Vertex> m_Vertices;
    ComPtr<ID3D12Resource> m_VertexBuffer;
    ComPtr<ID3D12Resource> m_VertexBufferUpload;
    D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
};