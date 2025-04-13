#include "Core/Utils/Debug.h"
#include "Core/Utils/Logger.h"
#include "Core/Rendering/DeviceContext.h"
#include "Core/Utils/StringUtils.h"
#include <Windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int main()
{
    const wchar_t* className = L"RenderWindowClass";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = className;
    RegisterClass(&wc);

    UINT width = 1280;
    UINT height = 720;
    HWND hwnd = CreateWindow(className, L"Rancage Render Engine", WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr,
                            nullptr, GetModuleHandle(nullptr), nullptr);

    if (!hwnd)
    {
        return 1;
    }
    ShowWindow(hwnd, SW_SHOW);

    Logger::Init("Logs/debug.log");
    if (!Debug::Initialize())
    {
        Logger::Log(LogLevel::Error, "Initialize failed");
        return 1;
    }

    DeviceContext deviceContext;
    if (!deviceContext.Initialize(hwnd, width, height))
    {
        Logger::Log(LogLevel::Error, "Device context initialization failed");
        return 1;
    }

    auto adapter = deviceContext.GetAdapter();
    if (adapter)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        std::wstring adapterName(desc.Description);
        Logger::Log(LogLevel::Info, "Using adapter: " + WStringToString(desc.Description));
    }

    auto device = deviceContext.GetDevice();
    if (device)
    {
        Logger::Log(LogLevel::Info, "DX12 device created successfully");
    }

    auto commandQueue = deviceContext.GetCommandQueue();
    if (commandQueue)
    {
        Logger::Log(LogLevel::Info, "Command queue ready (with fence)");
    }

    auto swapChain = deviceContext.GetSwapChain();
    if (swapChain)
    {
        Logger::Log(LogLevel::Info, "Swap chain ready");
    }

    auto rtvHeap = deviceContext.GetRTVHeap();
    if (rtvHeap)
    {
        Logger::Log(LogLevel::Info, "RTV descriptor heap ready");
    }

    auto dsvHeap = deviceContext.GetDSVHeap();
    if (dsvHeap)
    {
        Logger::Log(LogLevel::Info, "DSV descriptor heap ready");
    }

    auto depthBuffer = deviceContext.GetDepthBuffer();
    if (depthBuffer)
    {
        Logger::Log(LogLevel::Info, "Depth buffer ready");
    }

    auto rootSignature = deviceContext.GetRootSignature();
    if (rootSignature)
    {
        Logger::Log(LogLevel::Info, "Root signature ready");
    }

    auto pipelineState = deviceContext.GetPipelineState();
    if (pipelineState)
    {
        Logger::Log(LogLevel::Info, "Pipeline state ready");
    }

    Logger::Log(LogLevel::Info, "Engine started");

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
