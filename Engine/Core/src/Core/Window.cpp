#include "../Core/include/Core/Window.h"
#include "../Core/include/Utils/Logger.h"
#include <stdexcept>

namespace Core
{
    Window::Window() : m_Handle(nullptr), m_Width(0), m_Height(0) {}
    Window::~Window()
    {
        if (m_Handle)
        {
            DestroyWindow(m_Handle);
            m_Handle = nullptr;
            UnregisterClass(L"RancageWindowClass", GetModuleHandle(nullptr));
            Logger::Log(LogLevel::Info, "Window destroyed");
        }
    }

    bool Window::Initialize(const wchar_t* title, UINT width, UINT height)
    {
        m_Width = width;
        m_Height = height;

        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = L"RancageWindowClass";

        if (!RegisterClassEx(&wc))
        {
            Logger::Log(LogLevel::Error, "Failed to register window class: Error %u", GetLastError());
            return false;
        }

        m_Handle = CreateWindowEx(
            0, L"RancageWindowClass", title,
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height,
            nullptr, nullptr, GetModuleHandle(nullptr), this
        );

        if (!m_Handle)
        {
            Logger::Log(LogLevel::Error, "Failed to create window: Error %u", GetLastError());
            return false;
        }

        ShowWindow(m_Handle, SW_SHOW);
        UpdateWindow(m_Handle);
        Logger::Log(LogLevel::Info, "Window created: %ls (%ux%u)", title, width, height);
        return true;
    }

    bool Window::ProcessMessages()
    {
        MSG msg = {};
        try
        {
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                Logger::Log(LogLevel::Info, "Processing message: 0x%04X", msg.message);
                if (msg.message == WM_QUIT)
                {
                    Logger::Log(LogLevel::Info, "Received WM_QUIT");
                    return false;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        catch (const std::exception& e)
        {
            Logger::Log(LogLevel::Error, "Exception in ProcessMessages: %s", e.what());
            return false;
        }
        catch (...)
        {
            Logger::Log(LogLevel::Error, "Unknown exception in ProcessMessages");
            return false;
        }
        return true;
    }

    LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_NCCREATE)
        {
            CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
            Window* window = reinterpret_cast<Window*>(create->lpCreateParams);
            if (!window)
            {
                Logger::Log(LogLevel::Error, "Null window pointer in WM_NCCREATE");
                return FALSE;
            }
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
            Logger::Log(LogLevel::Info, "Set window user data");
            return window->HandleMessage(hwnd, msg, wParam, lParam);
        }

        Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (window)
        {
            return window->HandleMessage(hwnd, msg, wParam, lParam);
        }
        Logger::Log(LogLevel::Info, "Default processing for message: 0x%04X", msg);
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    LRESULT Window::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        Logger::Log(LogLevel::Info, "Handling message: 0x%04X", msg);
        switch (msg)
        {
        case WM_DESTROY:
            Logger::Log(LogLevel::Info, "Window destroyed via WM_DESTROY");
            PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            Logger::Log(LogLevel::Info, "Window closing via WM_CLOSE");
            DestroyWindow(hwnd);
            return 0;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            Logger::Log(LogLevel::Info, "Handled WM_PAINT");
            return 0;
        }
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
            {
                Logger::Log(LogLevel::Info, "Escape key pressed, closing window");
                DestroyWindow(hwnd);
            }
            return 0;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYUP:
            return 0;
        }
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}