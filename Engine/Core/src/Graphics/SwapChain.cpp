#include "../Core/include/Graphics/SwapChain.h"
#include "../Core/include/Utils/Logger.h"
#include <comdef.h>

namespace Graphics
{
    bool SwapChain::Initialize(ID3D12Device* device, ID3D12CommandQueue* queue, HWND hwnd, UINT width, UINT height)
    {
        Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
        HRESULT hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create DXGI factory for SwapChain: %s", err.ErrorMessage());
            return false;
        }

        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = 0;

        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
        hr = factory->CreateSwapChainForHwnd(queue, hwnd, &swapChainDesc, nullptr, nullptr, &swapChain1);
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to create SwapChain: %s", err.ErrorMessage());
            return false;
        }

        hr = swapChain1.As(&m_SwapChain);
        if (FAILED(hr))
        {
            _com_error err(hr);
            Logger::Log(LogLevel::Error, "Failed to cast SwapChain to IDXGISwapChain4: %s", err.ErrorMessage());
            return false;
        }

        Logger::Log(LogLevel::Info, "SwapChain created");
        return true;
    }
}