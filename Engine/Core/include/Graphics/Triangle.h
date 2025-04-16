#pragma once
#include "../Interfaces/IRenderable.h"
#include "../Math/Vector3.h"
#include <d3d12.h>
#include <wrl.h>
#include <vector>

namespace Graphics
{
    class Triangle : public IRenderable
    {
    public:
        Triangle(ID3D12Device* device);
        void Render(ID3D12GraphicsCommandList* commandList) const override;

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_VBV;
        D3D12_INDEX_BUFFER_VIEW m_IBV;
    };
}