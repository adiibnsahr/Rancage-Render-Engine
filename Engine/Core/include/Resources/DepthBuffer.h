#pragma once
#include "../Interfaces/IResource.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace Graphics
{
    class DepthBuffer : public IResource
    {
    public:
        DepthBuffer(UINT width, UINT height);
        ~DepthBuffer();

        bool Create(ID3D12Device* device) override;
        ID3D12Resource* GetResource() const override { return m_DepthStencilBuffer.Get(); }
        void Release() override;

        D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const { return m_DSVHandle; }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;
        D3D12_CPU_DESCRIPTOR_HANDLE m_DSVHandle;
        UINT m_Width;
        UINT m_Height;
    };
}
