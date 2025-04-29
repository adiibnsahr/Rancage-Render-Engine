#pragma once
#include <Windows.h>

namespace Core
{
    class Window
    {
    public:
        Window();
        ~Window();

        bool Initialize(const wchar_t* title, UINT width, UINT height);
        bool ProcessMessages();
        HWND GetHandle() const { return m_Handle; } // Tambah getter

    private:
        static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        HWND m_Handle;
        UINT m_Width;
        UINT m_Height;
    };
}