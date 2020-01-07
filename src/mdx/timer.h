#pragma once
#include <chrono>
#include "types.h"

namespace mdx
{
    class TimeSpan
    {
    public:
        std::chrono::high_resolution_clock::duration m_span;
    };

    class Timer
    {
    public:
        Timer() = default;
        ~Timer() = default;

        void Reset();
        TimeSpan Elapsed() const;
        TimeSpan Stop();

    private:
        std::chrono::high_resolution_clock::time_point m_time;
    };
}