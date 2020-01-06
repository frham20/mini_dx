#pragma once
#include "mdx\window.h"
#include "mdx\gfx.h"

namespace mdx
{
    class App
    {
    public:
        App() = default;
        ~App() = default;

        void Init();
        int Run();
        void Close();

        const Gfx& GetGfx() const;
        const Window& GetWindow() const;

        static App& GetInstance();

    private:
        App(const App&) = delete;
        App(App&&) = delete;
        App& operator=(const App&) = delete;

        void ParseCommandLineArgs();
        bool PumpThreadMessages();

    private:
        Window m_wnd;
        Gfx m_gfx;
        
        bool m_argGfxDebug = false;
    };
}