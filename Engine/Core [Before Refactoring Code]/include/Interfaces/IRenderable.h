#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "../Core/include/Math/Matrix4.h"

namespace Graphics
{
    class IRenderable
    {
    public:
        virtual ~IRenderable() = default;
        virtual void Render(ID3D12GraphicsCommandList* commandList) const = 0;
        virtual void Update(float deltaTime) {}
        virtual Math::Matrix4 GetModelMatrix() const = 0;
    };
}
