#include "mdx.h"
#include "mdx_app.h"
#include <shellapi.h>

namespace mdx
{
    static App s_app;

    void App::ParseCommandLineArgs()
    {
        int argc = 0;
        auto argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
        if (argv == nullptr)
            return;

        for (int i = 0; i < argc; i++)
        {
            const auto arg = argv[i];
            if (_wcsicmp(arg, L"--gfx_debug") == 0)
            {
                m_argGfxDebug = true;
            }
        }

        ::LocalFree(argv);
    }

    bool App::PumpThreadMessages()
    {
        MSG msg;
        while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                return false;

            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }

        return true;
    }

    void App::Init()
    {
        ParseCommandLineArgs();

        m_wnd.Create();
        m_gfx.Init(&m_wnd, m_argGfxDebug);
    }

    void App::Close()
    {
        m_gfx.Close();
        m_wnd.Destroy();
    }

    int App::Run()
    {
        for (;;)
        {
            m_gfx.Frame();

            if (!PumpThreadMessages())
                break;

            ::SleepEx(16, TRUE);
        }

        return 0;
    }

    App& App::GetInstance()
    {
        return s_app;
    }
}

int __stdcall wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PWSTR /*pCmdLine*/, int /*nCmdShow*/)
{
    int retValue = 0;

    try
    {
        auto& app = mdx::App::GetInstance();
        app.Init();
        retValue = app.Run();
        app.Close();
    }
    catch (std::exception & /*ex*/)
    {
        if (IsDebuggerPresent())
            __debugbreak();
    }

    return retValue;
}