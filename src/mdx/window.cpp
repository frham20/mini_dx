#include "platform.h"
#include "core.h"
#include "window.h"

namespace mdx
{
    constexpr wchar_t WND_CLASS_NAME[] = L"MiniDXWindow";

    Window::~Window()
    {
        Destroy();
    }

    void Window::Create()
    {
        if (!RegisterWindowClass())
            throw Exception("Error registering window class");;

        const DWORD wndStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
        const DWORD wndStyleEx = WS_EX_OVERLAPPEDWINDOW;

        RECT wndRect = { 0, 0, 1280, 720 };
        ::AdjustWindowRectEx(&wndRect, wndStyle, FALSE, wndStyleEx);

        if (::CreateWindowExW(wndStyleEx, WND_CLASS_NAME, L"Mini DX v0.000001", wndStyle,
            0, 0, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top,
            nullptr, nullptr, ::GetModuleHandleW(nullptr), this) == nullptr)
            throw Exception("Error creating window");

        assert(m_handle != nullptr);

        ::UpdateWindow(m_handle);
        ::ShowWindow(m_handle, SW_SHOWNORMAL);
    }

    void Window::Destroy()
    {
        if (m_handle == nullptr)
            return;

        ::DestroyWindow(m_handle);
    }

    void Window::Show()
    {
        assert(m_handle != nullptr);
        ::ShowWindow(m_handle, SW_SHOW);
    }

    LRESULT Window::OnMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        switch (uMsg)
        {
        case WM_CLOSE:
        {
            ::PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE:
        {
            RECT wndRect;
            ::GetClientRect(m_handle, &wndRect);
            m_width = wndRect.right - wndRect.left;
            m_height = wndRect.bottom - wndRect.top;
            break;
        }
        }

        return ::DefWindowProcW(m_handle, uMsg, wParam, lParam);
    }

    LRESULT CALLBACK Window::WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        Window* wnd = nullptr;
        if (uMsg == WM_NCCREATE)
        {
            auto cs = reinterpret_cast<LPCREATESTRUCTW>(lParam);
            assert(cs != nullptr);
            wnd = static_cast<Window*>(cs->lpCreateParams);
            assert(wnd != nullptr);
            ::SetWindowLongPtrW(hwnd, 0, reinterpret_cast<LONG_PTR>(wnd));
            wnd->m_handle = hwnd;
        }

        LRESULT result = 0;
        wnd = reinterpret_cast<Window*>(::GetWindowLongPtrW(hwnd, 0));
        if (wnd != nullptr)
        {
            result = wnd->OnMessage(uMsg, wParam, lParam);
            
            //clean-up
            if (uMsg == WM_NCDESTROY)
            {
                wnd->m_handle = nullptr;
                wnd->m_width = 0;
                wnd->m_height = 0;
            }
        }
        else
        {
            result = ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
        }

        return result;
    }

    bool Window::RegisterWindowClass()
    {
        const HINSTANCE hInstance = ::GetModuleHandleW(nullptr);

        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        if (!::GetClassInfoExW(hInstance, WND_CLASS_NAME, &wc))
        {
            wc.cbClsExtra = 0;
            wc.cbWndExtra = sizeof(Window*);
            wc.hbrBackground = nullptr;
            wc.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
            wc.hIcon = ::LoadIconW(nullptr, IDI_APPLICATION);
            wc.hIconSm = nullptr;
            wc.hInstance = hInstance;
            wc.lpfnWndProc = &Window::WindowProc;
            wc.lpszClassName = WND_CLASS_NAME;
            wc.lpszMenuName = nullptr;
            wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;

            if (!::RegisterClassExW(&wc))
                return false;
        }

        return true;
    }
}