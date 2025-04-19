#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>

namespace Graphics
{
    class SwapChain
    {
    public:
        SwapChain() = default;
        ~SwapChain() = default;

        bool Initialize(ID3D12Device* device, ID3D12CommandQueue* queue, HWND hwnd, UINT width, UINT height);
        IDXGISwapChain4* GetSwapChain() const { return m_SwapChain.Get(); }

    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
    };
}
