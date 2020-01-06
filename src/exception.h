#pragma once
#include <exception>

namespace mdx
{
    class Exception : public std::exception
    {
    public:
        Exception();
        Exception(const Exception&) = default;
        Exception(Exception&&) = default;
        explicit Exception(const char* format, ...);

        Exception& operator=(const Exception&) = default;
        Exception& operator=(Exception&&) = default;

        virtual char const* what() const override;

    private:
        char m_msg[1024];
    };


    inline Exception::Exception()
    {
        m_msg[0] = 0;
    }

    inline char const* Exception::what() const
    {
        return m_msg;
    }
}
