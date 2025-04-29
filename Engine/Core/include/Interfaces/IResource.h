#pragma once
#include <d3d12.h>
#include <wrl/client.h>

namespace Graphics
{
    class IResource
    {
    public:
        virtual ~IResource() = default;
        virtual bool Create(ID3D12Device* device) = 0;
        virtual ID3D12Resource* GetResource() const = 0;
        virtual void Release() = 0;
    };
}
