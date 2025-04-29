#pragma once
#include "../Interfaces/IRenderable.h"
#include "../Math/Vector3.h"
#include "../Math/Transform.h"
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
        void Update(float deltaTime) override;
        Math::Matrix4 GetModelMatrix() const;

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_VertexBuffer;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_IndexBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_VBV;
        D3D12_INDEX_BUFFER_VIEW m_IBV;
        Math::Transform m_Transform;
    };
}