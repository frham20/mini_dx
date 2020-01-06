#pragma once
namespace mdx
{
    class Window
    {
    public:
        Window() = default;
        ~Window();

        void Create();
        void Destroy();
        void Show();

        HWND GetHandle() const;

    protected:
        LRESULT OnMessage(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

    private:
        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window& operator=(Window&&) = delete;

        static bool RegisterWindowClass();
        static LRESULT CALLBACK WindowProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

    private:
        HWND m_handle = nullptr;
        int m_width = 0;
        int m_height = 0;
    };


    inline HWND Window::GetHandle() const
    {
        return m_handle;
    }
}
